#include <emmintrin.h> 
#include <x86intrin.h>
#include <stdio.h>
#include <stdint.h>

// Based on https://seedsecuritylabs.org/Labs_16.04/System/Meltdown_Attack/

#define ARRAY_SIZE 10
#define AVERAGE_CNT 50
#define THRESHOLD 200

uint8_t array[ARRAY_SIZE * 4096];

int main(int argc, const char **argv) 
{ 
	int junk = 0; 
	register uint64_t time1, time2; 
	volatile uint8_t *addr; 
	int i;
	int r = 0;
	
	// Initialize the array
	for(i = 0; i < ARRAY_SIZE; i++) 
	{	
		array[i*4096] = 1;
	}
	
	double times[ARRAY_SIZE] = { 0 };
	
	
	for(r = 0; r < AVERAGE_CNT; r++)
	{
		// FLUSH the array from the CPU cache 
		for(i = 0; i < ARRAY_SIZE; i++) 
		{
			_mm_clflush(&array[i*4096]);
		}
		
		// Access some of the array items - this is the victim code
		array[3 * 4096] = 100;
		array[7 * 4096] = 200;
		// End of "victim"
		
		// Probe
		for(i = 0; i < ARRAY_SIZE; i++) 
		{ 
			addr = &array[i * 4096]; 
			
			time1 = __rdtscp(&junk); 
			junk = *addr; 
			time2 = __rdtscp(&junk) - time1; 
			
			// Add to average
			times[i] += time2;
		} 
	}
	
	for(i = 0; i < ARRAY_SIZE; i++)
	{
		double t = times[i]/AVERAGE_CNT;
		printf("Access time for array[%d*4096]: %f CPU cycles ", i, t); 
			
		if(t < THRESHOLD)
		{
			printf("--> SHORT");
		}
		
		printf("\n");
	}
	
	return 0;
}
