#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "threader.h"

long message_available;

int main(int argc, char **argv)
{
    // Error handling response from pthread_create
    int response, ret;

    // Check for correct usage
    if (argc != 3)
    {
	// Print error message letting user know correct usage
	fprintf(stderr, "Usage: %s <p> <m> \n", argv[0]);
	fprintf(stderr, "Where <p> is the number of processes\n");
        fprintf(stderr, "Where <m> is the message to pass\n");
	return 1;
    }

    // Seed the rand
    srand(time(NULL));

    // Message
   char *message = malloc(strlen(argv[2])*sizeof(char));

    // Number of threads from command line
    long num_threads = strtol(argv[1], NULL, 10);

    // Message
    message = argv[2];

    // message not available set by main
    message_available = 0;

    // Check for correct number of threads
    if (num_threads < 1)
    {
	// Print error message
	fprintf(stderr, "ERROR: thread count must be at least 1\n");
	return 2;
    }

    // Check for correct size of char array
    if (strlen(message) < 1)
    {
	// Print error message
	fprintf(stderr, "ERROR: There must be at least one message\n");
	return 3;
    }

    // Allocate memory for processes
    pthread_t *processes = malloc(num_threads * sizeof(pthread_t));

    // Allocate memory for args
    struct thread_args *args = malloc(num_threads * sizeof(struct thread_args));

    // Check malloc allocated correctly
    if (!message || !processes || !args)
    {
        // Print error message
        fprintf(stderr, "ERROR: malloc failed!\n");
	return 4;
    }

    //start timer
    struct timeval tstart, tend;
    gettimeofday(&tstart, NULL);

    // Synchronize
    for (int i = 0; i < num_threads; i++)
    {
	args[i].message = message;
	args[i].num_threads = num_threads;
	args[i].id = i;
	args[i].consumer = 0;
	args[i].message_available = 0;
    }

    //initialize the mutex
    ret = pthread_mutex_init(&mutex, NULL);
    if (ret)
    {
 	fprintf(stderr, "ERROR: pthread_mutex_init failed\n");
    }

    // Parallelize
    printf("Running parallel\n");
    for (int i = 0; i < num_threads; i++)
    {
	// Run parallel threads
        response = pthread_create(&processes[i], NULL, prod_cons, &args[i]);

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

    // Free the memory
    //free(message);
    free(processes);
    free(args);

    // Timer end
    gettimeofday(&tend, NULL);
    double time = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/1.e6;
    printf("Wall time is %g s\n", time);

    return 0;
}

void * prod_cons(void* args)
{
    /*
    Exercise 4.7
    */

    // Type cast from void* to struct thread_args
    struct thread_args * this_args = (struct thread_args *) args;

    // Get variables from function args
    long id = this_args->id;

    // Produce-Consumer Synchronization
    while (1)
    {
	pthread_mutex_lock(&mutex);

	if (id % 2 != 0 ) // general case
	{
	    if (message_available)
	    {
		printf("%s\n", this_args->message);
                pthread_mutex_unlock(&mutex);
	        break;
	    }
	}

	else
	{
	    /* my rank == producer */
	    printf("Message Produced\n");
	    message_available = 1;
	    pthread_mutex_unlock(&mutex);
	    break;
   	}
	pthread_mutex_unlock(&mutex);
    }

    printf("Message was sent in process: %ld\n", id);
    return NULL;
}
