/* Ulises Martinez
 * Program copies the content of one text file into
 * another text file using a buffer. 
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

/*
 * First parameter is file descriptor to be written to.
 * Second parameter is buffer where content is placed.
 * Third parameter is the number of bytes read that need to be written.
 * Returns the number of bytes written to file.
 */
 
ssize_t write_all(int fd, void *buffer, ssize_t num_bytes) {
	ssize_t total_bytes_written = 0, bytes_written = 0;
	
	// keep looping until all the content stored in num_bytes is copied
	while (total_bytes_written < num_bytes) {
		bytes_written = write(fd, &buffer[total_bytes_written], num_bytes - total_bytes_written);
	
		if (bytes_written < 0) {
			return bytes_written;
		} else if (bytes_written == 0) {
			return total_bytes_written;
		}		
		total_bytes_written += bytes_written;
	}
	return total_bytes_written;
}



int main(int argc, char **argv) {
	int fd1, // read content here
	    fd2; // copy content here
	ssize_t bytes_read, bytes_copied = 0;
	char buffer[BUFFER_SIZE];

	
	if (argc != 3) {
		printf("Usage: ./ <file to copy> <file name with copied content>\n", %s);
		return 1;
	}

	if ((fd1 = open(argv[1], O_RDONLY)) < 0) {
		printf("Error opening %s\n", argv[1]);
		return 1;
	}
	
	if ((fd2 = open(argv[2], O_WRONLY | O_CREAT)) < 0) {
		printf("Error opening %s\n", argv[2]);
		return 1;
	}
	
	while ((bytes_read = read(fd1, buffer, BUFFER_SIZE)) > 0) {
		bytes_copied += write_all(fd2, buffer, bytes_read);
	}
	
	printf("copied %d bytes\n", bytes_copied);
	return 0;
}
 	
