#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static volatile int pin_stored[4] = {0x2, 0x4, 0x6, 0x8};

#define USE_RDTSCP 1

static uint64_t rdtsc() 
{
	uint64_t a, d;
	__asm volatile("mfence");
	#if USE_RDTSCP
	__asm volatile("rdtscp" : "=a"(a), "=d"(d) :: "rcx");
	#else
	__asm volatile("rdtsc" : "=a"(a), "=d"(d));
	#endif
	a = (d << 32) | a;
	__asm volatile("mfence");
	return a;
}

static int check_pin(int pin_entered[4])
{
	int i = 0;
	
	for(i = 0; i < 4; i++) 
	{
		if(pin_stored[i] != pin_entered[i]) 
		{
			return -1;
		}
	}
	
	return 0;
}

#define PRINT_RESULTS

int main(void)
{
	// Average each attempt REPEAT times
	const size_t REPEAT = 100;
	
	// Outlier filtering
	const double THRESHOLD = 300;
	
	int pin_entered[4] = {0x0, 0x0, 0x0, 0x0};
	double timings[10] = { 0 };
	
	// Go over all digits
	printf("\n");
	for(int digit = 0; digit < 4; digit++)
	{
		printf("Collecting %ld timings each for all guesses for digit %d\n", REPEAT, digit);
		
		// Time each guess for the current digit
		for(int guess = 0; guess < 10; guess++)
		{
			// Use the current guess for a specific digit
			pin_entered[digit] = guess;
			
			// Average each attempt REPEAT times
			size_t cnt = 0;
			
			for(size_t r = 0; r < REPEAT; r++)
			{
				__asm volatile("mfence");
				uint64_t begin = rdtsc();
				int r = check_pin(pin_entered);
				uint64_t end = rdtsc();
				__asm volatile("mfence");
				
				const double cycles = (double)(end - begin);
				
				// Filter too large values (e.g. due to other processes, cache etc.)
				if(cycles < THRESHOLD)
				{
					timings[guess] += cycles;
					cnt++;
				}
				
				// If we ever succeed, output success
				if(r == 0)
				{
					printf("\n======================================== \n");
					printf("| W00t! PIN = %d %d %d %d                  |\n", pin_entered[0], pin_entered[1], pin_entered[2], pin_entered[3]);
					printf("======================================== \n\n");
					break;
				}
			}
			
			// Normalize the timing result for each guess
			timings[guess] /= (double)cnt;
			
			#ifdef PRINT_RESULTS
			printf("Guess %d: timing = %f\n", guess, timings[guess]);
			#endif
		}
		
		// Now, determine the maximum in timings[] to find the correct digit
		double max_val = 0;
		int max_idx = 0;
		
		for(int guess = 0; guess < 10; guess++)
		{
			if(timings[guess] > max_val)
			{
				max_idx = guess;
				max_val = timings[guess];
			}
		}
		
		printf("--> max timing %f for guess %d\n\n", max_val, max_idx);
		
		// Set the recovered digit
		pin_entered[digit] = max_idx;
		
		
	}
	

	return 0;
}


