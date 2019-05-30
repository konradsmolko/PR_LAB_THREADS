#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#define NTHREADS				10
#define MAXPRIMES				1000
#define THREAD_STOPPED	0xDEAD
#define THREAD_RUNNING	0xBEEF

/*
compile with:
gcc -pthread main.c -o prog
*/

struct // thread_args
{
	int number;
	int *array;
	int *status;
	pthread_mutex_t mutex;
} THRARG, *PTHRARG;


void *check_fact(void *arguments)
{
	PTHRARG args = arguments;
	int number = args->number;
	int *array = args->array;
	pthread_mutex_t m = args->mutex;
	static int index = 0;

	// Magic happens here
	if (number > 3)
		for (int i = 2; i < number / 2 + 1; i++)
			if (number % i == 0)
				{
					free(args);
					return 1;
				}

	// Finally, return the value
	// TODO return the value
	free(args);
	return 0;
}

// Search for factorials in range, sending each factorial to a new thread
void factorial(const int start, const int end)
{
	// Generate numbers in given range
	int total_num = end - start + 1;
	int *ptr = (int*) malloc(total_num * sizeof(int));
	for (int i = 0; i < total_num; i++)
		ptr[i] = i + start;

	// Thread-safe tool initialization
	pthread_t thread_id[NTHREADS];
	int thread_state[NTHREADS];
	for (int i = 0; i < NTHREADS; i++) thread_state[i] = THREAD_STOPPED;
	static int primes[MAXPRIMES];
	for (int i = 0; i < MAXPRIMES; i++) primes[i] = NULL;
	pthread_mutex_t m_p;
	pthread_mutex_init(&m_p);

	// Creating threads
	int counter = 0;
	while (counter < total_num)
	{
		for (int i = 0; i < NTHREADS; i++)
		{
			// If this thread is running, try with next.
			if (thread_state[i] == THREAD_RUNNING) continue;
			else
			{
				// This struct is passed to each thread
				// Threads take care of freeing the struct memory
				PTHRARG arg = (PTHRARG) malloc(sizeof(THRARG));
				arg->number = ptr[counter];
				arg->array = primes;
				arg->status = &thread_state[i];
				arg->mutex = m_p;

				pthread_create(&thread_id[i], NULL, check_fact, (void*) arg);
				thread_state[i] = THREAD_RUNNING;
				counter++;
				if (counter >= total_num) break;
			}
		}
	}

	// Joining threads
	// TODO rewrite for status
	for (int i = 0; i < j; i++)
	{
		pthread_join(thread_id[i], NULL);
	}


	free(ptr);
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
