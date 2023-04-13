#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#ifndef MAX_BUF
#define MAX_BUF 200
#endif


int main(int argc, char **argv)
{
    // Says hello from the process executing.
    printf("Hello from process: %d\n", getpid());

    // File descriptor array
    int fd[2];

    // Argument vector
    char *args[6];

    pipe(fd);

    // Fork
    pid_t a = fork();

    if (a < 0)
    {
        fprintf(stderr, "Fork failed!\n");
        exit(1);
    }

    if (a == 0)
    {
        close(1);
		dup(fd[1]);
		close(fd[0]);
		close(fd[1]);


        // Child process
        printf("Hello running output of child a: %d\n", getpid());

        // Create a character array to pass to process b
        args[0] = (char*) malloc(5*sizeof(char));
		args[1] = (char*) malloc(5*sizeof(char));
		strcpy(args[0],"ls");
		strcpy(args[1],"-l");
        args[2] = NULL;

        execv(args[0], args); // send ls -l to process b
        perror("Process a failed");
		exit(2);
    }

    pid_t b = fork();

    if (b < 0)
    {
        fprintf(stderr, "Fork failed!\n");
        exit(3);
    }

    if (b == 0)
    {
        close(0);
		dup(fd[0]);
		close(fd[0]);
		close(fd[1]);

        // Child process
        printf("Hello running output of child b: %d\n", getpid());

        // Complete process communication pipe()
        char path[MAX_BUF];

        getcwd(path, MAX_BUF);
        printf("Current working directory: %s\n", path);
        args[0] = (char*) malloc(5*sizeof(char));
		args[1] = (char*) malloc(MAX_BUF*sizeof(char));
		strcpy(args[0],"grep");
		strcpy(args[1], path);
        args[2] = NULL;

        execv(args[0], args); // completing statement
        perror("Process b failed");
		exit(4);
    }


    close(fd[0]);
	close(fd[1]);
	waitpid(a, NULL, 0);
	waitpid(b, NULL, 0);
    printf("Goodbye from parent process\n");
    
    return 0;
}