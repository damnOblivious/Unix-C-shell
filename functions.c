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

void echo(char** cmd_words, int tokens, char* cmd) 
{
	if(cmd_words[1][0] == '-' && tokens > 1) 
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

int pinfo(char** cmd_tokens) {
        int c, len=0;
        pid_t cpid;
        
        if(cmd_tokens[1] == NULL)
		cpid = getpid();
        else 
		cpid = atoi(cmd_tokens[1]);
   
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
	int status,renew,i=0;
	if(is_bg)
		while(cmd_words[i++]);
	cmd_words[i-2]='\0';
	pid = fork();
	if (pid < 0) // Error forking
	{
		perror("error creating child process\n");
	}
	else if (pid == 0) // Child process
	{
		if(execvp(cmd_words[0], cmd_words) == -1) 
		{
			perror("error excuting the command \n");
		}
 	if(!WIFSTOPPED(status)) rem_proc(pid);         /* returns true if the child process was stopped by delivery of a signal */
         
                
	} 
	else // Parent process
	{
		add_proc(pid, cmd_words[0]);
		if(!is_bg)
		{
			do
			{
				wpid = waitpid(pid, &status, WUNTRACED);
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
			if(!WIFSTOPPED(status)) rem_proc(pid);         /* returns true if the child process was stopped by delivery of a signal */

			else fprintf(stderr, "\n%s with pid %d has stopped!\n", cmd_words[0], pid);

			return 0;
		}
		printf("\[%d] %d\n", num_jobs, pid);
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
/*****************************Run command*****************************************/

void normal_cmd(int word_count, char** cmd_words, char* cmd_copy) {
	if(word_count > 0)  
	{
		if(strcmp(cmd_words[0], "cd\0") == 0)
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
			is_bg = 1;
			run_cmd(cmd_words);        // for running background process
		}
		else if(strcmp(cmd_words[0], "pinfo\0") == 0) 
{
	pinfo(cmd_words);
}
		else if(isalpha(cmd_words[0][0]))
		{
			run_cmd(cmd_words);
		}
	}
	free(cmd_words);
}



