/*****************************Header Files & Macros*****************************************/

#include<signal.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<ctype.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/wait.h>
#define MAX_BUF_LEN 1024
#define FNAME_LEN 20

/*****************************Varibales*****************************************/
void relative_path(char* cwd);
int cd_cmd(char** cmd_words, char* cwd, char* base_dir);
void pwd(char** cmd_words);
void echo(char** cmd_words, int tokens, char* cmd); 
int pinfo(char** cmd_tokens);
void normal_cmd(int word_count, char** cmd_words, char* cmd_copy);
void get_home_dir();
void get_prompt_vars();
void set_prompt();
int parsing(char* line, char** store_to,char *delimiters);
void rem_proc(int pid);
int shell, shell_pgid;
void add_proc(int pid, char* name);
int run_cmd(char** cmd_words); 
char user[MAX_BUF_LEN];
char hostname[MAX_BUF_LEN];
char base_dir[MAX_BUF_LEN];
char cmdline[MAX_BUF_LEN];
char* pipe_cmds[MAX_BUF_LEN];
char cwd[MAX_BUF_LEN]; /* current working directory */
int is_bg,word_count,cmd_count,num_jobs; 
typedef struct proc_fields {
	int pid, pgid;
	char* name;
	int active;
	int bg;
}proc_fields;
proc_fields table[MAX_BUF_LEN];
