#ifndef THREADER_H
#define THREADER_H

#include <stdio.h>
#include <stdlib.h>

void randomizer(int *array, long length);
void * number_search(void* args);
void print_array(int *array, long length);
void * scalar_multiply(void* args);
void * prod_cons(void* args);
void * mat_mul(void* args);

struct thread_args {
    int *array;
    int *x;
    int *y;
    int **A;
    long length;
    long num_threads;
    long id;
    long target;
    char *message;
    long consumer;
    long message_available;
};

pthread_mutex_t mutex;

#endif
