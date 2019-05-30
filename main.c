#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#define NTHREADS				10
#define MAXPRIMES				1000
#define THREAD_STOPPED	0xDEAD
#define THREAD_RUNNING	0xBEEF
#define NONE						0

/*
compile with:
gcc -pthread main.c -o prog
test with:
./prog 1000000000 1000010000
*/

struct thread_args
{
	int number;
	int *array; // CRITICAL
	int *status;
	pthread_mutex_t *mutex;
} THRARG;


void *check_fact(void *arguments)
{
	// Copying the argument struct for code beauty later on
	struct thread_args *args = arguments;
	const int number = args->number;
	int *array = args->array;
	int *status = args->status;
	pthread_mutex_t *mutex = args->mutex;
	static int index = 0;

	// Magic happens here
	if (number > 3)
		for (int i = 2; i < number / 2 + 1; i++)
			if (number % i == 0)
				{
					free(args);
					status[0] = THREAD_STOPPED;
					return 0;
				}

	// Finally, return the value
	// CRITICAL SECTION
	pthread_mutex_lock(mutex);
	array[index] = number;
	index++;
	pthread_mutex_unlock(mutex);

	// Cleanup
	free(args);
	status[0] = THREAD_STOPPED;
	return 0;
}

// Search for factorials in range, sending each factorial to a new thread
void factorial(const int start, const int end)
{
	// Generate numbers in given range
	int total_num = end - start + 1;
	int *numbers = (int*) malloc(total_num * sizeof(int));
	for (int i = 0; i < total_num; i++)
		numbers[i] = i + start;

	// Initialization
	pthread_t thread_id[NTHREADS];
	int thread_state[NTHREADS];
	for (int i = 0; i < NTHREADS; i++) thread_state[i] = THREAD_STOPPED;
	static int primes[MAXPRIMES];
	for (int i = 0; i < MAXPRIMES; i++) primes[i] = NONE;
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);

	// Creating threads until we've passed all of the numbers to check
	for (int counter = 0; counter < total_num;)
	{
		for (int i = 0; i < NTHREADS; i++)
		{
			// If this thread is running, try with next
			if (thread_state[i] == THREAD_RUNNING) continue;
			else
			{
				// This struct is passed to each thread
				// Threads take care of freeing the struct memory
				struct thread_args *arg = malloc(sizeof(THRARG));
				arg->number = numbers[counter];
				arg->array = primes;
				arg->status = &thread_state[i];
				// Setting this before creating the thread to avoid race condition
				thread_state[i] = THREAD_RUNNING;
				arg->mutex = &mutex;

				pthread_create(&thread_id[i], NULL, check_fact, (void*) arg);

				counter++;
				// Check to prevent creating more threads when we're already done
				if (counter >= total_num) break;
			}
		}
	}

	// Joining the remaining threads
	for (int i = 0; i < NTHREADS; i++)
		if (thread_state[i] == THREAD_RUNNING)
			pthread_join(thread_id[i], NULL);

	// Printing the primes
	for (int i = 0; i < MAXPRIMES; i++)
	{
		if (primes[i] == NONE) break;
		else printf("Prime: %i\n", primes[i]);
	}

	// Cleanup
	pthread_mutex_destroy(&mutex);
	free(numbers);
}

/*

*/
int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("Usage: prog [start end]\nStarting with default range <10,100>\n");
		factorial(10,100);
	}
	else
	{
		int start = atoi(argv[1]);
		int end = atoi(argv[2]);
		if (start > end || start <= 0) return 1;
		printf("Starting with range <%i,%i>\n", start, end);
		factorial(start, end);
	}
	return 0;
}
