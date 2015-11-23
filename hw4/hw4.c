/* Ulises Martinez
 * Demonstrate the use of POSIX threads by dividing text file
 * into partitions divided equally among the number of threads.
 * Each thread counts the number of characters and puts it in
 * global 2d array. List of occurrences of each character is printed.
 */

#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>  // open/close file
#include <unistd.h> // read file

#define BUFFER_SIZE 65536  // 64kb
#define NUM_THREADS 8

char buffer[BUFFER_SIZE];
int ascii[NUM_THREADS][128];

typedef struct{
    int thread_num;
	int bounds;
} thread_info;


void *thread_funct(void *thread_infos) {
	int threadStartPos; // start position for thread
	int i;
	thread_info ti  = *(thread_info*) thread_infos;
	threadStartPos = ti.thread_num * ti.bounds;
	
	for (i = 0; i < ti.bounds && threadStartPos < BUFFER_SIZE; i++)
		ascii[ti.thread_num][ buffer[threadStartPos++] ]++;
	
	pthread_exit(NULL);
}

int main(int argc, char **argv) {
	
	pthread_t threads[NUM_THREADS];
	thread_info ti[NUM_THREADS];
	int fd;    // file descriptor
	int i, j;  // loop counter
	int bytes; // bytes in text file (max of 64kb)
	int bound; // partition of bytes divided equally among number of threads
	int total; // total for each character
	
	if (argc != 2) {
		printf("Usage: ./%s <filename.txt>", argv[0]);
		return 1;
	}
	
	if ((fd = open(argv[1], O_RDONLY)) < 0) {
		printf("Error opening %s\n", argv[1]);
		return 1;
	}
	
	bytes = read(fd, buffer, BUFFER_SIZE);
	bound = bytes / NUM_THREADS;

	// create threads and their thread number
	for (i = 0; i < NUM_THREADS; i++) {
		ti[i].thread_num = i;
		ti[i].bounds = bound;
		pthread_create(&threads[i], NULL, thread_funct, &ti[i]);
	}

	for (i = 0; i < NUM_THREADS; i++)
		pthread_join(threads[i], NULL);
	
	// loop through each ascii character and thread and combine each thread's count
	for (i = 0; i <= 127; i++) {
		total = 0;
		for (j = 0; j < NUM_THREADS; j++)
			total += ascii[j][i];
		
		printf("\n%d occurrences of 0x%d ", total, i);
		if (i >= 33 && i != 127) // print ascii characters that are printable
			printf("%c", i); 
	}
	
	printf("\nNumber of threads used: %d\n", NUM_THREADS);
	close(fd);
	return 0;
}
