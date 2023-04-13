#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include "threader.h"

void init(int **array, int length);
void populate(int **array, int length);

int main(int argc, char **argv)
{

    // Error handling response from pthread_create
    int response, ret;

    // Check for correct usage
    if (argc != 4)
    {
	// Print error message letting user know correct usage
	fprintf(stderr, "Usage: %s <p> <n> <m>\n", argv[0]);
	fprintf(stderr, "Where <p> is the number of processes\n");
	fprintf(stderr, "Where <n> is the length of the random array\n");
        fprintf(stderr, "Where <m> is the length of the random 2D array\n");
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

    // Scalar value
    long target = atoi(argv[3]);

    // Confirm target number for user
    printf("The scalar value you entered: %ld\n", target);
    printf("\n");

    // Allocate memory for random array
    int *array = malloc(length * sizeof(int));

    // Allocate memory for random array
    int *y = malloc(length * sizeof(int));

    // Allocate memory for random array
    int *x = malloc(length * sizeof(int));

    // Allocate memory for random array
    int **A = malloc(length * sizeof(int));

    // Allocate memory for processes
    pthread_t *processes = malloc(num_threads * sizeof(pthread_t));

    // Allocate memory for args
    struct thread_args *args = malloc(num_threads * sizeof(struct thread_args));

    // Check malloc allocated correctly
    if (!array || !processes || !args || !y || !x)
    {
        // Print error message
        fprintf(stderr, "ERROR: malloc failed!\n");
	return 4;
    }

    // Randomize array
    randomizer(array, length);
    randomizer(y, length);
    randomizer(x, length);
    init(A, length);
    populate(A, length);

    // Show array contents before scaling
    print_array(array, length);

    //start timer
    struct timeval tstart, tend;
    gettimeofday(&tstart, NULL);

    // Synchronize
    for (int i = 0; i < num_threads; i++)
    {
	args[i].array = array;
	args[i].y = y;
	args[i].x = x;
	args[i].A = A;
	args[i].length = length;
	args[i].num_threads = num_threads;
	args[i].id = i;
	args[i].target = target;
    }

    //initialize the mutex
    ret = pthread_mutex_init(&mutex, NULL);
    if (ret) fprintf(stderr, "ERROR: pthread_mutex_init failed\n");

    // Parallelize
    printf("Running parallel\n");
    for (int i = 0; i < num_threads; i++)
    {
	// Run parallel threads
        response = pthread_create(&processes[i], NULL, mat_mul, &args[i]);

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

    //destroy (free) the mutex
    pthread_mutex_destroy(&mutex);

    // Show the contents of random array
    print_array(y, length);

    // Free the memory
    free(array);
    free(y);
    free(x);
    free(A);
    free(processes);
    free(args);

    // Timer end
    gettimeofday(&tend, NULL);
    double time = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/1.e6;
    printf("Wall time is %g s\n", time);

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

void *mat_mul(void* args)
{
    /*
    Matrix Multiplication
    */

    // Type cast from void* to struct thread_args
    struct thread_args * this_args = (struct thread_args*) args;

    // Get variables from function args
    int *array = this_args->array;
    int *x = this_args->x;
    int *y = this_args->y;
    int **A = this_args->A;
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

    //acquire the mutex lock
    pthread_mutex_lock(&mutex);

    // Matrix Scalar Multiplication
   for (int i = 0; i < length; i++)
   {
	 y[0] = 0.0;
    	for (int j = 0; j <length; j++)
    	{
	    y[i] += A[i][j] * x[j];
    	}
   }

    //release the mutex lock
    pthread_mutex_unlock(&mutex);

    return NULL;
}


void * scalar_multiply(void* args)
{
    /*
    Scalar multiplication on random vector
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

    //acquire the mutex lock
    pthread_mutex_lock(&mutex);

    // Scalar Multiplication
    for (int i = start; i < stop; i++)
    {
        array[i] *= target;
    }

    //release the mutex lock
    pthread_mutex_unlock(&mutex);

    return NULL;
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

void init(int **array, int length)
{
    // Initialize a 2d Array
    for (int i = 0; i < length; i++)
    {
	// malloc space for row i's N column elements
	array[i] = malloc(sizeof(int) * length);
    }
}

void populate(int **array, int length)
{
    for (int i = 0; i < length; i++)
    {
        for (int j = 0; j < length; j++)
	{
            array[i][j] = i*j;
        }
    }
}
