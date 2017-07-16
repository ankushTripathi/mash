#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>

#define MSH_RL_BUFSIZE 1024
#define MSH_TOK_BUFSIZE 64
#define MSH_TOK_DELIM " \t\r\n\a"

char *mash_read(void){

int buff_size = MSH_RL_BUFSIZE;

char *buffer = malloc(sizeof(char)*buff_size);
if(!buffer){
	fprintf(stderr, "mash: allocation error\n");
    exit(EXIT_FAILURE);
}

int pos = 0;
int c;

while((c=getchar())!='\n'){
	buffer[pos] = c;
	pos++;
	if(pos >= buff_size){
		buff_size += MSH_RL_BUFSIZE;
		buffer = realloc(buffer,buff_size);
		if(!buffer){
			fprintf(stderr, "mash: allocation error\n");
    		exit(EXIT_FAILURE);
		}
	}
}
buffer[pos] = '\0';
return buffer;
}

char **mash_args(char *line){
	int tok_size = MSH_TOK_BUFSIZE;
	int pos = 0;
	char **tokens = malloc(sizeof(char)*tok_size);
	char *token;
	if(!tokens){
		fprintf(stderr, "mash: allocation error\n");
    	exit(EXIT_FAILURE);
	}

	token = strtok(line,MSH_TOK_DELIM);
	while(token!=NULL){
		tokens[pos] = token;
		pos++;
		if(pos>tok_size){
			tok_size += MSH_TOK_BUFSIZE;
			tokens = realloc(tokens,tok_size);
			if(!tokens){
			fprintf(stderr, "mash: allocation error\n");
    		exit(EXIT_FAILURE);
		}
		}
		token = strtok(NULL,MSH_TOK_DELIM);
	}
	tokens[pos]=NULL;
	return tokens;
}


int msh_launch(char **args){
	
	pid_t pid,wpid;
	int status;

	pid = fork();

	if(pid == 0){
		if(execvp(args[0],args)== -1){
			perror("mash");
		}
		exit(EXIT_FAILURE);
	}else if(pid < 0){
		perror("mash");
	}else{
		do{
			wpid = waitpid(pid,&status,WUNTRACED);
		}while(!WIFEXITED(status)&&!WIFSIGNALED(status));
	}
	return 1;
}

int msh_cd(char **args);
int msh_help(char **args);
int msh_exit(char **args);

char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &msh_cd,
  &msh_help,
  &msh_exit
};

int msh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

int msh_cd(char **args){
	if(args[1] == NULL){
		fprintf(stderr,"mash: expected argument to \"cd\"\n");
	}else{
		if(chdir(args[1]) != 0){
			perror("mash");
		}
	}
	return 1;
}

int msh_help(char **args)
{
  int i;
  printf("Ankush Tripathi's MASH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < msh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int msh_exit(char **args){
	return 0;
}

int mash_exec(char **args){

	if(args[0] == NULL){
		return 1;
	}

	for(int i=0; i<msh_num_builtins();i++){
		if(strcmp(args[0],builtin_str[i])==0){
			return (*builtin_func[i])(args);
		}
	}

	return msh_launch(args);

}

void mash_loop(void){

int status;
char **args;
char *line;

do{
	printf(" >> ");
	
	line = mash_read();
	args = mash_args(line);
	status = mash_exec(args);	
	free(line);
	free(args);
	
}while(status);


}


int main(int argc,char **argv){
    
    mash_loop();

    return EXIT_SUCCESS;
}
