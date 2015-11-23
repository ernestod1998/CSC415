/* Ulises Martinez
 * Typical consumer-producer problem. There is a buffer where producers
 * place integers in it and consumers "eat" the elements by setting the
 * value to 0. Program uses pthreads, a lock, and condition variables
 * "empty" and "full".
 */

#include <stdio.h>
#include <pthread.h>
#include <math.h>
#define BUFFER_SIZE 16

static int buffer[BUFFER_SIZE];
static pthread_mutex_t lock;
static pthread_cond_t empty;
static pthread_cond_t full;
static int counter = 0;          // keep track of number of elements in buffer
static int number_of_items;	   // number of items produced by each producer thread
static int number_consumers;     // number of consumer threads
static int number_producers;     // number of producer threads

// holds the information for each producer thread
typedef struct{
    int thread_number;
	int number_produced;
} producer_info;

void *produce(void *param) {
	int count;
	producer_info pi  = *(producer_info*) param;
	for (count = 0; count < number_of_items; count++) {
		pthread_mutex_lock(&lock);
		while (counter == BUFFER_SIZE)   // wait until buffer is not full
			pthread_cond_wait(&empty, &lock);
		buffer[counter] = pi.thread_number * pi.number_produced + count;
		printf("\nProduced %d at location %d", buffer[counter], counter);
		pi.number_produced++;
		counter++;
		// unlock and send signal
		pthread_mutex_unlock(&lock);
		pthread_cond_broadcast(&full);
	}
	pthread_exit(0);
}

void *consume(void *param) {
	int count;
	for (count = 0; count < (number_of_items*number_producers)/number_consumers; count++) {
		pthread_mutex_lock(&lock);
		while (counter == 0)  // wait until there is an element in buffer
			pthread_cond_wait(&full, &lock);
		counter--;	
		printf("\nConsumed %d at location %d", buffer[counter], counter);
		buffer[counter] = 0;  // "consume" element
		// unlock and send signal
		pthread_mutex_unlock(&lock);
		pthread_cond_broadcast(&empty);
	}
	pthread_exit(0);
}

int main(int argc, char **argv) {
	
	if (argc != 4) {
		printf("Usage: ./%s <producers> <consumers> <items>", argv[0]);
		return 1;
	}
	int i;
	
	// take arguments as exponent with base 2
	number_producers = pow(2, atoi(argv[1]));
	number_consumers = pow(2, atoi(argv[2]));
	number_of_items = pow(2, atoi(argv[3]));
	printf("Number of producers: %d\nNumber of consumers: %d\nNumber of items: %d", number_producers, number_consumers, number_of_items);
	
	// pthread lock, condition, and threads initialization
	pthread_mutex_init(&lock, NULL);
	pthread_cond_init(&full, NULL);
	pthread_cond_init(&empty, NULL);
	pthread_t producers[number_producers];
	pthread_t consumers[number_consumers];
	producer_info pi[number_producers];
	printf("\n---Begin producing & consuming!---");
	
	// create producer threads
	for (i = 0; i < number_producers; i++) {
		pi[i].thread_number = i;
		pi[i].number_produced = 0;
		pthread_create(&producers[i], NULL, produce, &pi[i]);
	}

	// create consumer threads
	for (i = 0; i < number_consumers; i++) 
		pthread_create(&consumers[i], NULL, consume, NULL);

	// join threads
	for (i = 0; i < number_producers; i++)
		pthread_join(producers[i], NULL);
	
	for (i = 0; i < number_consumers; i++)
		pthread_join(consumers[i], NULL);
	
	printf("\n---Done!---\n");
	return 0;
}
