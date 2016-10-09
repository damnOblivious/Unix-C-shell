#include "headers.h"
char CMD_DELIMS[]= " \t\n";
char CMDLINE_DELIMS[]= ";";
int main()
{
	get_prompt_vars();
	get_home_dir();
	relative_path(cwd);           /*modify current working directory relative to assumed home directory */


	//Command Loop
	while(1) {
		set_prompt();
		int i,j;
		char** cmds = malloc((sizeof(char)*MAX_BUF_LEN)*MAX_BUF_LEN); // array of semi-colon separated commands
		for(j = 0; j < MAX_BUF_LEN; j++) 
			cmds[j] = '\0';
		fgets(cmdline,1024-1,stdin);
		cmd_count = parsing(cmdline, cmds,CMDLINE_DELIMS); // parse command line
		for(i = 0; i < cmd_count; i++)
		{
			char** cmd_words = malloc((sizeof(char)*MAX_BUF_LEN)*MAX_BUF_LEN); // array of command words
			for(j = 0; j < MAX_BUF_LEN; j++)
				cmd_words[j] = '\0';
			char* cmd_copy = strdup(cmds[i]);

			word_count = parsing(strdup(cmds[i]), cmd_words,CMD_DELIMS);
			normal_cmd(word_count, cmd_words, cmd_copy);

		}
		if(cmds) 
			free(cmds);
	}
	return 0;
}
