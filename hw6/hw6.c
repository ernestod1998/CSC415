/* Ulises Martinez
 * Program to replicate page replacement FIFO and LRU using pure demand paging
 * to calculate the number of page faults. Two arrays are used to accomplish
 * this. One array holds the page number of the virtual address n entered and
 * the other array holds the time the page n has been in that spot. 
 */
#include <string.h>
#include <stdio.h>
#include <math.h> // pow function
#include <stdlib.h>

int main(int argc, char **argv) {
	
	if (argc != 5 || ((strcmp(argv[3], "FIFO") != 0) && (strcmp(argv[3], "LRU") != 0))) {
		printf("Usage: ./%s <page size> <logical memory size> <LRU or FIFO> <frames>", argv[0]);
		return 1;
	}
	
	int i;
	int pageSize, pageNumber, logicalMemSize, frames, pageFaults = 0;
	int clock = 1;        // only if LRU is used, it ticks after every input; used to find out least recently used
	int isPageFault = 1;  // 1 is page fault, 0 is no page fault
	unsigned int address;
	char input[20];		  // store addresses input by user
	pageSize = pow(2, atoi(argv[1]));
	logicalMemSize = pow(2, atoi(argv[2]));
	frames = atoi(argv[4]);
	int counter[frames];   // keeps track how long each address[i] has stayed in pageTable[i]
	int pageTable[frames]; // keeps track of all the addresses stored 
	
	for (i = 0; i < frames; i++) {
		pageTable[i] = -1;
		counter[i] = 0;
	}
	printf("Page size: %d bytes\nLogical memory size: %d bytes\nAlgorithm: %s\nFrames: %d\n\n", pageSize, logicalMemSize, argv[3], frames);
	printf("Enter addresses in format 0x1234 on separate lines. Hit enter to quit.\n");
	while(1) {
		fgets(input, sizeof(input), stdin);
		if (input[0] == '\n')            // if empty input, exit
			break;
		address = strtol(input, NULL, 0);
		if (address > logicalMemSize) {  // ignore input
			continue;
		}
		
		pageNumber = address / pageSize;
		
		if (strcmp(argv[3], "FIFO") == 0) {
			for (i = 0; i < frames; i++) {
				if (pageNumber == pageTable[i]) {
					counter[i] = 0;	    // reset FIFO counter 
					isPageFault = 0;    // no page fault
				}
			}
			
			if (isPageFault == 1) {
				pageFaults++;
				int largest = counter[0];
				int index = 0;  // index for largest value in counter
				
				// find largest value and index in counter 
				for (i = 1; i < frames; i++) {
					if (counter[i] > largest) {
						largest = counter[i];
						index = i;
					}
				}
				
				pageTable[index] = pageNumber;
				counter[index] = 0;  // reset counter for that address
				
				// increment counter for all indexes
				for (i = 0; i < frames; i++)
					counter[i]++;
			}
		} else {  // LRU
			for (i = 0; i < frames; i++) {
				if (pageNumber == pageTable[i]) {
					counter[i] = clock;
					isPageFault = 0;  // no page fault
				}
			}
			
			if (isPageFault == 1) {
				pageFaults++;
				int smallest = counter[0];
				int index = 0;  // index for smallest value in counter
				
				// find smallest value and index in counter 
				for (i = 1; i < frames; i++) {
					if (counter[i] < smallest) {
						smallest = counter[i];
						index = i;
					}
				}
				
				pageTable[index] = pageNumber;
				counter[index] = clock;
			}
			
			clock++;
		}
		
		isPageFault = 1;  // reset
	}
	
	printf("Total number of page faults: %d\n", pageFaults);
	return 0;
}