#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

void print_error(char* message);

void *hello_world(void *id);

int main(int argc, char **argv) {
	/*
	Multi-threading
	*/

	int i, n_threads, ret;
	pthread_t *thread_array;
	long *thread_id;
	
	if (argc != 2) {
		fprintf(stderr,"Usage: %s <n>\n", argv[0]);
		fprintf(stderr, "Where <n> is the number of threads\n");
		return 1;
	}

	n_threads = strtol(argv[1], NULL, 10);
	
	if (n_threads < 1) {
		print_error("ERROR: Enter a positive number of threads");
	}

	thread_array = malloc(n_threads * sizeof(pthread_t));
	thread_id = malloc(n_threads * sizeof(long));

	if (!thread_array || !thread_id) {
		print_error("ERROR: Malloc Failed!");
	}

	for (i = 0; i < n_threads; i++) {
		thread_id[i] = i;
		ret = pthread_create(&thread_array[i], NULL, hello_world, &thread_id[i]);
		if (ret) {
			print_error("ERROR: pthread create failed!");
		}
	}

	for (i = 0; i < n_threads; i++) {
		ret = pthread_join(thread_array[i], NULL);
		if (ret) {
			print_error("ERROR: pthread join failed!");
		}
	}

	free(thread_array);
	free(thread_id);

	return 0;
}

void print_error(char* message) {
	fprintf(stderr, "%s\n", message);
	exit(2);
}

void *hello_world(void *id) {
	long *thread_id = (long *)id;

	printf("Hello World, I am thread %ld\n", *thread_id);
	return NULL;
}
