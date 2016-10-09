/*****************************Basic Setup*****************************************/
#include "headers.h"

char CMD_DELIMS2[]= " \t\n";
char CMDLINE_DELIMS2[]= ";";

void get_prompt_vars()
{
	gethostname(hostname, MAX_BUF_LEN - 1);
	getlogin_r(user, MAX_BUF_LEN - 1);
}

void get_home_dir()
{
	getcwd(base_dir,  MAX_BUF_LEN - 1);
	strcpy(cwd, base_dir);
}

void set_prompt() {
	printf("<%s@%s:%s> ", user, hostname, cwd);
}

void relative_path(char* cwd)
{
	int i, j;
	for(i = 0; cwd[i]==base_dir[i] && cwd[i]!='\0' && base_dir[i] != '\0'; i++);
	if(base_dir[i] == '\0')  // in sub directory of the psedo home
	{
		cwd[0] = '~';
		for(j = 1; cwd[i]!='\0'; j++) {
			cwd[j] = cwd[i++];
		}
		cwd[j] = '\0';
	}
}



int has_piping(char * cmd)
{
	int i,is=0;
	for(i=0;cmd[i];++i)
	{
		if(cmd[i]=='|')
		is=1;
		if(cmd[i]=='<')
		input_redi=1;
		if(cmd[i]=='>')
		output_redi=1;
	}
	return is;
}



/*****************************parsing*****************************************/


int normal_parsing(char* line, char** store_to,char *delimiters)
{
	int num_parts = 0;
	char* part = strtok(line, delimiters);
	while(part!=NULL)
	{
		store_to[num_parts++] = part;
		part = strtok(NULL, delimiters);
	}
	return num_parts;
}

void parse_for_piping(char* cmd) {
        char* copy = strdup(cmd);
        char* token;
        int tok = 0;
        token = strtok(copy, "|");
        while(token!= NULL) {
                pipe_cmds[tok++] = token;
                token = strtok(NULL, "|");
        }
        num_pipe = tok;
}


int parse_for_redi(char* cmd, char** cmd_words)
{
	      char* copy = strdup(cmd);
				idxi = idxo = last = input_redi = output_redi = 0;
        infile = outfile = NULL;
        int i=0,tok = 0;
        for(;cmd[i];++i)
				{
            if(cmd[i] == '<')
						{
                    input_redi = 1;
                    if(idxi==0)
										 idxi = i;
            }
            else if(cmd[i] == '>')
						{
                    output_redi = 1;
                    if(last == 0)
										 last = 1;
                    if(idxo == 0 )
										 idxo = i;
            }
            if(cmd[i] == '>' && cmd[i+1] == '>')
							last = 2;
        }
        if(input_redi && output_redi)
				{
			    char* token;
          token = strtok(copy, " <>\t\n");
          while(token!=NULL)
					 {
                  cmd_words[tok++] = strdup(token);
                  token = strtok(NULL, "<> \t\n");
          }
          if(idxi < idxo ) {
                  outfile = strdup(cmd_words[tok - 1]);
									infile = strdup(cmd_words[tok - 2]);
          }
          else {
                  outfile = strdup(cmd_words[tok - 2]);
									infile = strdup(cmd_words[tok - 1]);
          }
          cmd_words[tok - 2] = cmd_words[tok - 1] = NULL;
          return tok - 2;
        }

        if(input_redi)
				 {
          char* token;
          char* copy = strdup(cmd);

          char** input_redi_cmd = malloc((sizeof(char)*MAX_BUF_LEN)*MAX_BUF_LEN);
          token = strtok(copy, "<");
          while(token!=NULL)
					{
	          input_redi_cmd[tok++] = token;
	          token = strtok(NULL, "<");
	        }

					copy = strdup(input_redi_cmd[tok - 1]);
          token = strtok(copy, "> |\t\n");
          infile = strdup(token);

          tok = 0;
          token = strtok(input_redi_cmd[0], CMD_DELIMS2);
          while(token!=NULL)
					{
	          cmd_words[tok++] = strdup(token);
	          token = strtok(NULL, CMD_DELIMS2);
	        }

          cmd_words[tok] = NULL;

          free(input_redi_cmd);
			    }

        if(output_redi)
				{
					char* copy = strdup(cmd);
          char* token;
          char** output_redi_cmd = malloc((sizeof(char)*MAX_BUF_LEN)*MAX_BUF_LEN);
          if(last == 1)
                  token = strtok(copy, ">");
          else if(last == 2)
                  token = strtok(copy, ">>");
          while(token!=NULL) {
                  output_redi_cmd[tok++] = token;
                  if(last == 1) token = strtok(NULL, ">");
                  else if(last == 2) token = strtok(NULL, ">>");
          }

          copy = strdup(output_redi_cmd[tok - 1]);
          token = strtok(copy, "< |\t\n");
          outfile = strdup(token);

          tok = 0;
          token = strtok(output_redi_cmd[0], CMD_DELIMS2);
          while(token!=NULL) {
                  cmd_words[tok++] = token;
                  token = strtok(NULL, CMD_DELIMS2);
          }

          free(output_redi_cmd);
        }
        if(input_redi == 0 && output_redi == 0 )
					return normal_parsing(strdup(cmd), cmd_words,CMD_DELIMS2);
        else return tok;
}
