#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char **argv)
{
    // Says hello from the process executing.
    printf("Hello from process: %d\n", getpid());

    // Fork
    int a = fork();

    if (a < 0)
    {
        fprintf(stderr, "Fork failed!\n");
        exit(1);
    }

    else if (a == 0)
    {
        // Child process
        printf("Hello running Kmeans clustering on child process: %d\n", getpid());

        // Create a character array to run another program
        char *args[6];
        args[0] = strdup("km");
        args[1] = strdup("5");
        args[2] = strdup("10");
        args[3] = strdup("10000");
        args[4] = strdup("2");
        args[5] = NULL;

        execv(args[0], args); // runs KMEANS clustering
        printf("This better not print\n");

    }

    else
    {
        // Call wait on parent process
        int km = wait(NULL);
        printf("Hello I am process %d parent of child: %d and wait: %d\n", getpid(), a, km);
    }
    
    return 0;
}