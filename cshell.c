#include "headers.h"
char CMD_DELIMS[]= " \t\n";
char CMDLINE_DELIMS[]= ";";
int input_redi,output_redi;

int main()
{
	get_prompt_vars();
	get_home_dir();
	relative_path(cwd);
	num_jobs = 0;


	//Command Loop
	while(1) {
		set_prompt();
		int i,j;
		char** cmds = malloc((sizeof(char)*MAX_BUF_LEN)*MAX_BUF_LEN); // arr of semi-colon separated commands
		for(j = 0; j < MAX_BUF_LEN; j++)
			cmds[j] = '\0';
		fgets(cmdline,MAX_BUF_LEN-1,stdin);
		cmd_count = normal_parsing(cmdline, cmds,CMDLINE_DELIMS); // parse command line


		for(i = 0; i < cmd_count; i++)
		{
			is_bg = num_pipe = input_redi = output_redi = 0;
			char** cmd_words = malloc((sizeof(char)*MAX_BUF_LEN)*MAX_BUF_LEN); // array of command words
			for(j = 0; j < MAX_BUF_LEN; j++)
				cmd_words[j] = '\0';
			char* cmd_copy = strdup(cmds[i]);


			if(!has_piping(strdup(cmds[i])))
			{
					if(input_redi || output_redi)
					{
						normal_cmd(parse_for_redi(strdup(cmd_copy), cmd_words), cmd_words, cmd_copy);
					}
					else
					{
						word_count = normal_parsing(strdup(cmds[i]), cmd_words,CMD_DELIMS);
						normal_cmd(word_count, cmd_words, cmd_copy);
					}
			}
			else
			{
				redi_and_pipi_cmd(strdup(cmds[i]));
			}
		}
		if(cmds)
			free(cmds);
	}
	return 0;
}
