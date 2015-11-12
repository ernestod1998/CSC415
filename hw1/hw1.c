/* Ulises Martinez
 * Simple program that prints out
 * "Hello 415, I am Ulises!" on the
 * shell command line.
 */

#include<stdio.h>
#include<unistd.h>

#define NAME "Ulises"
#define BUFFER_SIZE 30

int main(int argc, char** argv)
{
	char str[BUFFER_SIZE];
    sprintf(str, "Hello 415, I am %s!\n", NAME);
	write(STDOUT_FILENO, str, BUFFER_SIZE);  // STDOUT_FILENO = 1

	return 0;
}