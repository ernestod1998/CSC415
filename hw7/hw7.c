/* Ulises Martinez
 * Advanced shell implementation. It builds on the shell created in hw 3 but also adds
 * the ability to redirect output and input using commands such as ">", ">>", "2>", etc.
 * Program calls on execvp() to execute commands and dup() along with dup2() to redirect.
 * Enter "exit" to quit shell.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024
#define DELIMITERS " \t\n"
#define MAX_ARGS 7

int main(int argc, char **argv) {
	
	char buffer[BUFFER_SIZE];  
	char buffer_copy[BUFFER_SIZE]; // copy of buffer
	char *tokens[MAX_ARGS];         // each command will be in own index
	char *token;
	pid_t pid;
	int background; // 0 if user did not enter "&" at end, 1 otherwise
	int err;        // 0 if not to stderr, 1 otherwise
	int count;      // keeps track of how many arguments user entered that are not redirect commands (ex ">", "2>")
	int outfile, infile; // if file is to be written to or read from, respectively
	
	do {
		printf("Myshell> ");
		fgets(buffer, BUFFER_SIZE, stdin);
		strncpy(buffer_copy, buffer, BUFFER_SIZE);
		// set variables to default values
		background = 0;
		err = 0;
		count = 0;
		infile = 0;
		outfile = 0;
		
		// extract each command using delimiters
		token = strtok(buffer, DELIMITERS);
		while(token != NULL) {
			if (strcmp(token, "&") == 0 )
				background = 1;
			else if (strcmp(token, ">>") == 0 || strcmp(token, "2>>") == 0){  // open file and append
				if(strcmp(token, "2>>") == 0)
					err = 1;  // error output
				token = strtok(NULL, DELIMITERS);  // get file name
				outfile = open(token, O_WRONLY|O_APPEND|O_CREAT);
				if(outfile < 0){
					printf("Error opening file %s\n", token);
					exit(0);
				}
			} else if(strcmp(token, ">") == 0 || strcmp(token, "2>") == 0){ // open file and truncate
				if(strcmp(token, "2>") == 0)
					err = 1;  // error output
				token = strtok(NULL, DELIMITERS);  // get file name
				outfile = open(token, O_WRONLY|O_CREAT|O_TRUNC);  
				if(outfile < 0){
					printf("Error opening file %s\n", token);
					exit(0);
				}
			} else if (strcmp(token, "<") == 0) {
				token = strtok(NULL, DELIMITERS);  // get file name
				infile = open(token, O_RDONLY);
				if (infile < 0) {
					printf("Error opening file %s\n", token);
					exit(0);
				}
				
			} else
				count++;
			token = strtok(NULL, DELIMITERS);
		}
		
		if (count > 0) {  // if user enters nothing, shell will be printed again
			char *args[count + 1];  // will hold arguments without redirect commands, last element is NULL
			args[0] = strtok(buffer_copy, DELIMITERS);
			int i = 0;
			while (args[i] != NULL) {
				if(background){
					if(strcmp(args[i], "&") == 0){
						args[i] = strtok(NULL, DELIMITERS);
						break;
					}
				}
				if(outfile){
					if(strcmp(args[i], ">") == 0 || strcmp(args[i], "2>") == 0 || strcmp(args[i], ">>") == 0 || strcmp(args[i], "2>>") == 0){
						strtok(NULL, DELIMITERS);
						args[i] = strtok(NULL, DELIMITERS);  
						break;
					}
				}					
				if(infile){
					if(strcmp(args[i], "<") == 0){
						strtok(NULL, DELIMITERS);
						args[i] = strtok(NULL, DELIMITERS);  
						break;
					}
				}
				
				i++;
				args[i] = strtok(NULL, DELIMITERS);
			}
		
			if (strcmp(args[0], "exit") == 0)
				return 0;  // quit shell
			
			pid = fork();
			if (pid < 0) {
				printf("Error in fork\n");
				exit(0);
				
			} else if (pid == 0) { // child process
			
				if(outfile){
					close(STDOUT_FILENO);
					dup(outfile);
				}
				if(infile){
					close(STDIN_FILENO);
					dup(infile);
				}
				if(err)
					dup2(outfile,STDERR_FILENO);
			
				if(execvp(args[0], args) < 0){
					printf("An error occurred executing %s", args[0]);
					exit(1);
				}
				if(outfile)
					close(outfile);
				
				if(infile)
					close(infile);
				
			} else {  // parent process
				if (background == 0) {
					int exit_status;
					pid_t wait_result = waitpid(pid, &exit_status, 0);
				
					if (wait_result < 0) {
						printf("An error occurred waiting for process %d\n", pid);
						return 1;
					}
				}
			}
		}		
		

	} while (1);
	
	return 0;
}
