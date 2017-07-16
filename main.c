#include<stdlib.h>
#include<stdio.h>

void mash_loop(void){

int status;
char **args;
char *line;

do{
	printf(" >> ");
	
	line = maash_read();
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
