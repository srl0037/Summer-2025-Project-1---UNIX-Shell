#include "builtins.h"

	void parse(char *line, char **my_args){
		char *token;
		const char s[3] = " \n";
		int i = 0;

		token = strtok(line, s);
		while( token != NULL ) {
			my_args[i] = token;
			token = strtok(NULL, s);
			i++;
		}
			my_args[i] = NULL;
	}
	
	void execute(char **argv){
		pid_t pid;
		int status;
		pid_t wait_result;
	
		if (argv[0] == NULL){
			return;
		}
		if (strcmp(argv[0], "exit") == 0){
			exit(0);
		}
		else if (strcmp(argv[0], "my_ls") == 0){
			my_ls(argv);
			return;
		}
		else if (strcmp(argv[0], "my_chmod") == 0){
			my_chmod(argv);
			return;
		}
		else if (strcmp(argv[0], "my_cp") == 0){
			my_cp(argv);
			return;
		}
		
		pid = fork(); // this will return a 0 if child
		if (pid == -1){
			printf("*** ERROR: forking child process failed\n");
			exit(1);
		}
		else if(pid == 0){
			execvp(argv[0], argv);
		}
		else{
			wait_result = waitpid (pid, &status, WUNTRACED);
		}
	}

	void interactive(){
		char *line_buf = NULL;
   	size_t len = 0;
   	ssize_t nread;
		char *my_args[MAX_ARGS];

		printf("my_shell: ");
		
		while ((nread = getline(&line_buf, &len, stdin)) != -1) {
			parse(line_buf, my_args);
			execute(my_args);
			printf("my_shell: ");
		}
		free(line_buf);
		exit(EXIT_SUCCESS);
		
	}

	int main(int argc, char *argv[]) {
		if (argc == 1){
			interactive();
		}
		else if (argc == 2){
			// invoke batch mode
			FILE *batch_file = fopen(argv[1], "r");
			printf("Batch processing mode with file %s\n", argv[1]); 
		}
		else{
			fprintf(stderr, "usage: ./my_shell [batchfile]\n");
		}
		return 0;
	}
