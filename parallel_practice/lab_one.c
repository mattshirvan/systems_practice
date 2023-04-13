#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include "threader.h"


void randomizer(int *array, long length);
void * number_search(void* args);
void print_array(int *array, long length);

int main(int argc, char **argv)
{
    // User's targeted search
    long target;

    // Error handling response from pthread_create
    int response;

    // Check for correct usage
    if (argc != 3)
    {
	// Print error message letting user know correct usage
	fprintf(stderr, "Usage: %s <p> <n>\n", argv[0]);
	fprintf(stderr, "Where <p> is the number of processes\n");
	fprintf(stderr, "Where <n> is the length of the random array\n");
	return 1;
    }

    // Seed the rand
    srand(time(NULL));


    // Number of threads from command line
    long num_threads = strtol(argv[1], NULL, 10);

    // Length of random array from command line
    long length = strtol(argv[2], NULL, 10);

    // Check for correct number of threads
    if (num_threads < 1)
    {
	// Print error message
	fprintf(stderr, "ERROR: thread count must be at least 1\n");
	return 2;
    }

    // Check for correct size of random array
    if (length < 1)
    {
	// Print error message
	fprintf(stderr, "ERROR: array length must be at least 1\n");
	return 3;
    }

    // Get target number from user input
    printf("Enter the number to search for:\n");
    scanf("%ld",&target);
    printf("\n");

    // Confirm target number for user
    printf("The number you entered: %ld\n", target);
    printf("\n");

    // Allocate memory for random array
    int *array = malloc(length * sizeof(int));

    // Allocate memory for processes
    pthread_t *processes = malloc(num_threads * sizeof(pthread_t));

    // Allocate memory for args
    struct thread_args *args = malloc(num_threads * sizeof(struct thread_args));

    // Check malloc allocated correctly
    if (!array || !processes || !args)
    {
        // Print error message
        fprintf(stderr, "ERROR: malloc failed!\n");
	return 4;
    }

    // Randomize array
    randomizer(array, length);

    // Synchronize
    for (int i = 0; i < num_threads; i++)
    {
	args[i].array = array;
	args[i].length = length;
	args[i].num_threads = num_threads;
	args[i].id = i;
	args[i].target = target;
    }

    // Parallelize
    printf("Running parallel\n");
    for (int i = 0; i < num_threads; i++)
    {
	// Run parallel threads
        response = pthread_create(&processes[i], NULL, number_search, &args[i]);

	// Check if error in pthread_create
	if (response)
	{
	    // Print error message
            fprintf(stderr, "ERROR: pthread create failed\n");
	    return 5;
        }
    }

    // Join
    for (int i = 0; i < num_threads; i++)
    {
	// Join threads
	response = pthread_join(processes[i], NULL);

	// Check if pthread join failed
	if (response)
	{
	    // Print error message
	    fprintf(stderr, "ERROR: pthread join failed\n");
	    return 6;
	}
    }

    // Show the contents of random array
    print_array(array, length);

    // Free the memory
    free(array);
    free(processes);
    free(args);

    return 0;
}

void randomizer(int *array, long length)
{
    /*
    Generates a random array
    */

    for (int i = 0; i < length; i++)
    {
	array[i] = rand() % 100;
    }
}

void * number_search(void* args)
{
    /*
    Searches for a target number in array
    */

    // Type cast from void* to struct thread_args
    struct thread_args * this_args = (struct thread_args *) args;

    // Get variables from function args
    int *array = this_args->array;
    long length = this_args->length;
    long num_threads = this_args->num_threads;
    long id = this_args->id;
    long target = this_args->target;

    // Partition work load
    long pool = length / num_threads;

    // Starting point id * n/p
    long start = id * pool;

    // Stopping point start + pool
    long stop = start + pool;

    // Account for odd size array
    if (id == num_threads-1)
    {
	stop = length;
    }

    // Search for number in list chunk
    for (int i = start; i < stop; i++)
    {
	if (array[i] == target)
	{
	     printf("Target %d found in process: %ld\n", array[i], id);
	}
    }

    printf("Target was not found in process: %ld\n", id);

    return NULL;
}

void print_array(int *array, long length)
{
    printf("\n");
    printf("Contents of array\n");
    printf("[ ");
    for (int i = 0; i < length; i++)
    {
	printf("%d ", array[i]);
    }
    printf("]");
    printf("\n");
}
