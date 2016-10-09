#include "headers.h"


/*****************************Built ins*****************************************/

int cd_cmd(char** cmd_words, char* cwd, char* base_dir) {
    if(cmd_words[1] == NULL || strcmp(cmd_words[1], "~/\0") == 0 || strcmp(cmd_words[1], "~\0") == 0 )
    {
        chdir(base_dir);
        strcpy(cwd, base_dir);
        relative_path(cwd);
    }
    else if(chdir(cmd_words[1]) == 0)
    {
        getcwd(cwd, MAX_BUF_LEN);
        relative_path(cwd);
        return 0;
    }
    else {
        perror("Error executing cd command\n");
    }
}


void pwd_cmd(char** cmd_words)
{
    char pwd_dir[MAX_BUF_LEN];
    getcwd(pwd_dir, MAX_BUF_LEN - 1);
    if(cmd_words[1] == NULL)
        printf("%s\n", pwd_dir);
    else
        run_cmd(cmd_words);
}

void echo(char** cmd_words, int words, char* cmd)
{
    if(cmd_words[1][0] == '-' && words > 1)
    {
        run_cmd(cmd_words);
        return;
    }
    int i, len = 0, in_quote = 0, space = 0,slen;
    slen=strlen(cmd);
    char buf[MAX_BUF_LEN] = "\0";
    for(i = 0; isspace(cmd[i]); i++);
    i+=5;
    for(;isspace(cmd[i]); i++);
    for(; cmd[i] != '\0' ; i++)
    {
        if(cmd[i] == '"')
        {
            in_quote = 1 - in_quote;
            continue;
        }
        if(!(in_quote) && (isspace(cmd[i])) && !(space))
        {
            space = 1;
            buf[len++] = ' ';
        }
        else if(in_quote || !isspace(cmd[i]))
            buf[len++] = cmd[i];
        if(!isspace(cmd[i]) && space)
            space = 0;
    }
    if(in_quote)
    {
        perror("Missing quotes\n");
        return;
    }
    else printf("%s\n", buf);
}
void jobs() {
        int i;
        for(i = 0; i < num_jobs ; i++) {
                if(table[i].active == 1) {
                        printf("[%d] %s [%d]\n", i, table[i].name, table[i].pid);
                }
        }
}

void fg(int words, char** cmd_words) {
        if(words != 2) {
                fprintf(stderr, "Invalid usage of fg");
                return;
        }

        int i, job_num = atoi(cmd_words[1]), status;
        if(table[job_num].active == 0) {
                printf("No such job exists\n");
                return;
        }
        if(table[job_num].active == 1) {
                int pid = table[job_num].pid, pgid;
                pgid = getpgid(pid);
                tcsetpgrp(shell, pgid);

                fgpid = pgid;
                if(killpg(pgid, SIGCONT) < 0)
                        perror("Can't get in foreground!\n");
                waitpid(pid, &status, WUNTRACED);
                if(!WIFSTOPPED(status)) {
                        table[job_num].active = 0;
                        fgpid = 0;
                }
                tcsetpgrp(shell, my_pid);
        }
        else fprintf(stderr, "No job found\n");
}




int pinfo(char** cmd_words) {
    int c, len=0;
    pid_t cpid;

    if(cmd_words[1] == NULL)
        cpid = getpid();
    else
        cpid = atoi(cmd_words[1]);

    char path[MAX_BUF_LEN]; // for generating path of proc/pid file
    char buf[MAX_BUF_LEN];  // for reading file line by line

    printf("pid -- %d\n", cpid);

    sprintf(path, "/proc/%d/status", cpid);
    FILE* fp = fopen(path, "r");
    if(!fp) {
        perror("Error while opening the proc/pid/status file\n");
    }
    else {
        char state;
        fgets(buf, 256, fp);     //idk y
        fgets(buf, 256, fp);
        sscanf(buf, "State:\t%c", &state);
        printf("Process Status -- %c\n", state);
        fclose(fp);
    }

    sprintf(path, "/proc/%d/statm", cpid);
    fp = fopen(path, "r");
    if(!fp) {
        perror("Error while opening the proc/pid/statm file\n");

    }
    else {
        while((c=fgetc(fp))!=' ') buf[len++] = c;
        buf[len] = '\0';
        printf("Memory -- %s\n", buf);
        fclose(fp);
    }

    sprintf(path, "/proc/%d/exe", cpid);
    len = -1;
    len = readlink(path, buf, MAX_BUF_LEN - 1);
    if(len==-1) {
        perror("Error while opening the proc/pid/exe file\n");
    }
    else {
        buf[len] = '\0';
        relative_path(buf);
        printf("Executable Path -- %s\n", buf);
    }
}



/*****************************Run command*****************************************/


int run_cmd(char** cmd_words)
{
    pid_t pid, wpid;
    pid = fork();
    if (pid < 0) // Error forking
    {
        perror("error creating child process\n");
        return -1;
    }
    else if (pid == 0) // Child process
    {
      int fin, fout;
      if(input_redi)
      {
        fin = open_infile();
        if(fin == -1)
         _exit(-1);
      }
      if(output_redi)
      {
        fout = open_outfile();
        if(fout == -1)
         _exit(-1);
      }

      if(execvp(cmd_words[0], cmd_words) < 0) {
          perror("Error executing command!\n");
          _exit(-1);
      }
      _exit(0);
    }
    else
    {
      if(is_bg == 0)
      {
        int child_status,retpid;
        pid_t tpid;
        do {
            tpid=waitpid(pid,&child_status,WUNTRACED);
          } while(!WIFEXITED(child_status) && !WIFSIGNALED(child_status));
      }
      else
      {
          add_proc(pid, cmd_words[0]);
          printf("[%d] %d\n", num_jobs, pid);
          return 0;
      }
    }
}


void add_proc(int pid, char* name) {
    table[num_jobs].pid = pid;
    table[num_jobs].active = 1;
    table[num_jobs].name = strdup(name);
    num_jobs++;
}

void rem_proc(int pid) {
    int i;
    for(i = 0 ; i < num_jobs; ++i) {
        if(table[i].pid == pid) {
            table[i].active = 0;
            break;
        }
    }
}

void killallbg()
{
        int i;
        for(i = 0 ; i < num_jobs ; i++)
         {
                printf("%s\n",table[i].pid );
                if(table[i].active == 1)
                {
                        if(kill(table[i].pid, SIGKILL) < 0)
                          perror("Error killing process!\n");
                }
        }
}

void kjob(int tokens, char** cmd_tokens) {
        if(tokens != 3) {
                fprintf(stderr, "Invalid usage of kjob!\n");
                return;
        }
        int job_num = atoi(cmd_tokens[1]);
        if(table[job_num].active == 1) {
                if(kill(table[job_num].pid, atoi(cmd_tokens[2])) < 0)                 /* For sending signal mentioned to job mentioned */
                        fprintf(stderr, "Signal not sent!\n");
        }
        else fprintf(stderr, "Job not found\n");
}


void redi_and_pipi_cmd(char* cmd) {
        int pid, pgid, fin, fout;
        num_pipe = 0;
        parse_for_piping(cmd);
        int* pipes = (int* )malloc(sizeof(int)*(2*(num_pipe - 1)));
        int i;
        for(i = 0; i < 2*num_pipe - 3; i += 2) {
                if(pipe(pipes + i) < 0 )
                {
                        perror("Pipe not opened!\n");
                        return;
                }
        }
        int status,j;
        for(i = 0; i < num_pipe ; i++) {
                char** cmd_words = malloc((sizeof(char)*MAX_BUF_LEN)*MAX_BUF_LEN);
                int words = parse_for_redi(strdup(pipe_cmds[i]), cmd_words);
                is_bg = 0;
                pid = fork();
                if(i < num_pipe - 1)
                        add_proc(pid, cmd_words[0]);

                if(pid != 0 ) {
                        if(i == 0 ) pgid = pid;
                        setpgid(pid, pgid);
                }
                if(pid < 0) {
                        perror("Fork Error!\n");
                }
                else if(pid == 0) {
                        if(output_redi) fout = open_outfile();
                        else if(i < num_pipe - 1) dup2(pipes[2*i + 1], 1);

                        if(input_redi) fin = open_infile();
                        else if(i > 0 ) dup2(pipes[2*i -2], 0);

                        int j;
                        for(j = 0; j < 2*num_pipe - 2; j++) close(pipes[j]);

                        if(execvp(cmd_words[0], cmd_words) < 0 ) {
                                perror("Execvp error!\n");
                                _exit(-1);
                        }
                }
        }

        for(i = 0; i < 2*num_pipe - 2; i++)
         close(pipes[i]);

        if(is_bg == 0) {

                for(i = 0; i < num_pipe ; i++)
                {
                  int cpid = waitpid(-pgid, &status, WUNTRACED);
                  if(!WIFSTOPPED(status)) rem_proc(cpid);
                }
        }
}

void normal_cmd(int word_count, char** cmd_words, char* cmd_copy) {
    if(word_count > 0)
    {
      if(strcmp(cmd_words[0], "killallbg\0") == 0 ) {
        killallbg();
      }
      else if(strcmp(cmd_words[0], "fg\0") == 0 )
      {
        fg(word_count, cmd_words);
      }
        else if(strcmp(cmd_words[0], "jobs\0") == 0)
        {
          jobs();
        }
        else if(strcmp(cmd_words[0], "cd\0") == 0)
        {
            cd_cmd(cmd_words, cwd, base_dir);
        }
        else if(strcmp(cmd_words[0], "pwd\0") == 0)
        {
            pwd_cmd(cmd_words);
        }
        else if(strcmp(cmd_words[0], "echo\0") == 0)
        {
            echo(cmd_words, word_count, cmd_copy);
        }
        else if(strcmp(cmd_words[0], "quit\0") == 0)
        {
            _exit(0);
        }
        else if(strcmp(cmd_words[word_count-1], "&\0") == 0)
        {
            cmd_words[word_count-1]=NULL;
            is_bg = 1;
            run_cmd(cmd_words);        // for running background process
            printf("count=%d\n",word_count);
        }
        else if(strcmp(cmd_words[0], "pinfo\0") == 0)
        {
            pinfo(cmd_words);
        }
        else if(strcmp(cmd_words[0], "kjob\0") == 0){
          kjob(word_count, cmd_words);
        }
        else if(isalpha(cmd_words[0][0]))
        {
            run_cmd(cmd_words);
        }
    }
    free(cmd_words);
}
