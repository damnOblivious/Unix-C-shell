/*****************************Basic Setup*****************************************/
#include "headers.h"
void get_prompt_vars() {
	getlogin_r(user, MAX_BUF_LEN - 1); 
	gethostname(hostname, MAX_BUF_LEN - 1); 
}

void get_home_dir() {
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


/*****************************Parsing*****************************************/

int parsing(char* line, char** store_to,char *delimiters) {
	int num_parts = 0;
	char* part = strtok(line, delimiters);
	while(part!=NULL) {
		store_to[num_parts++] = part;
		part = strtok(NULL, delimiters);
	}
	return num_parts;
}

