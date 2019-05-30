#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#define NTHREADS 10

/*
compile with:
gcc -pthread main.c -o prog
*/

void *check_fact(void *ptr)
{
	// We don't dare touch the ptr, and it makes the code look clearer
	// so just copying it with thread-safe function
	int number;
	memcpy(&number, ptr, sizeof(int));

	// Magic happens here
	bool isprime = true;
	if (number <= 3) isprime = true;
	else for (int i = 2; i < number / 2 + 1; i++)
	{
		if (number % i == 0)
		{
			isprime = false;
			break;
		}
	}

	// Finally, return the value
	if (isprime) printf("Found a prime number: %i\n", number);
	// void pthread_exit(void *retval)
}


void factorial(const int start, const int end)
{
	// Search for factorials in range, sending each factorial to a new thread
	pthread_t thread_id[NTHREADS];
	int total_num = end - start + 1;
	int *ptr = (int*)malloc(total_num * sizeof(int));
	for (int i = 0; i < total_num; i++)
	{
		ptr[i] = i + start;
	}

	// For now this creates and joins threads in packs of NTHREADS
	for (int counter = 0; counter < total_num;)
	{
		// Creating threads
		int j = 0;
		for (int i = 0; i < NTHREADS && counter < total_num; i++, j++, counter++)
		{
			pthread_create(&thread_id[i], NULL, check_fact, (void*) &ptr[counter]);
		}

		// Joining threads
		for (int i = 0; i < j; i++)
		{
			pthread_join(thread_id[i], NULL);
		}

	}
	free(ptr);
}


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
