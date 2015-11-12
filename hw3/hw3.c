/* Ulises Martinez
 * Implementation of own shell on top of command line interpretor
 * for Linux. Program stores what user types into prompt, then 
 * executes the command using execvp() function. Exit out of
 * program by typing "exit" without the quotes.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define DELIMITERS " \r\n"
#define MAX_ARGS 3

int main(int argc, char **argv) {
	
	char buffer[BUFFER_SIZE];  
	char *tokens[MAX_ARGS];  // each command will be in own index
	char *token = NULL;
	int position;            // index position of buffer and later used for tokens variable
	pid_t pid;
	
	do {
		printf("Myshell> ");
		position = 0;
		char c;
		
		// place what user typed into buffer and terminate with null character
		while (1) {
			c = getchar();
			if (c == '\n') {
				buffer[position] = '\0';
				break;
			} else {
				buffer[position] = c;
				position++;
			}
		}
		
		position = 0;
		
		// put together characters in buffer into commands by using delimiters 
		token = strtok(buffer, DELIMITERS);
		while(token != NULL) {
			tokens[position] = token;
			position++;
			token = strtok(NULL, DELIMITERS);			
		}
		
		
		tokens[position] = NULL;  // ending character is null
		
		 // if user enters exit, quit program
		if (strcmp(tokens[0], "exit") == 0)
			return 0;

		pid = fork();

		if (pid == 0) { // child process 
			int exec_result = execvp(tokens[0], tokens);
			if (exec_result < 0) {
				printf("An error occurred executing %s\n", tokens[0]);
				return 1;
			}

			return 0;

		} else {  // parent process
			int exit_status;
			pid_t wait_result = waitpid(pid, &exit_status, 0);
			
			if (wait_result < 0) {
				printf("An error occurred waiting for process %d\n", pid);
				return 1;
			}
		}

	} while (1);
	
	return 0;
}