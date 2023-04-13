#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <sched.h>
#include <errno.h>

#define MAX 10000000

int main(int argc, char **argv)
{
    // Variables for measuring the System Call and Context Switch
    float res;
    double timer;
    struct timeval tstart, tend;
    int pipe1[2];
    int pipe2[2];
    pid_t pid;

    // File descriptor to read from
    int file_descriptor = open("./hw6.txt", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);

    // Seed random generator for timing
    srand((unsigned int)time(NULL));
    
    // Start measuring system call
    gettimeofday(&tstart, NULL);

    // Initiate read() system call
    for (int i = 0; i < MAX; i++)
    {
        read(file_descriptor, NULL, 0);
    }

    // End measuring system call
    gettimeofday(&tend, NULL);
    timer = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/MAX;
    printf("System call time is %g microseconds\n", timer);
    close(file_descriptor);

    // Measure context switch
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set);

    // Start pipe
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1)
    {
        perror("Pipe one or two failed");
        exit(EXIT_FAILURE);
    }

    // Fork process
    pid = fork();

    // Check if fork failed
    if (pid == -1)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    // Otherwise wait for pid to be 0 (child process)
    else if (pid == 0)
    {
        // Check if process bound to processor
        if (sched_setaffinity(getpid(), sizeof(cpu_set_t), &set) == -1)
        {
            perror("Child process");
            exit(EXIT_FAILURE);
        }

        // Run pipeline communication
        for (size_t i = 0; i < MAX; i++) 
        {
            read(pipe1[0], NULL, 0);
            write(pipe2[1], NULL, 0);
        }
    }

    // Parent process
    else
    {
        // Check if process bound to processor
        if (sched_setaffinity(getpid(), sizeof(cpu_set_t), &set) == -1)
        {
            perror("Child process");
            exit(EXIT_FAILURE);
        }

        // Start measuring context switch
        gettimeofday(&tstart, NULL);

        // Run pipeline communication
        for (size_t i = 0; i < MAX; i++) 
        {
            read(pipe1[1], NULL, 0);
            write(pipe2[0], NULL, 0);
        }

        // End measuring system call
        gettimeofday(&tend, NULL);
        timer = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/MAX;
        printf("Context switch time is %g microseconds\n", timer);
    }

    return 0;
}