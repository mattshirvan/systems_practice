#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

int **genRandomMatrix(int n, int max);
void free_all(int **mat, int n);
float averageMat_v1(int **mat, int n);
float averageMat_v2(int **mat, int n);

int main(int argc, char **argv)
{
    if (argc != 2) {
       fprintf(stderr, "usage: %s <n>\n", argv[0]);
       fprintf(stderr, "where <n> is the dimension of the matrix\n");
       return 1;
    }

    /* Declare and initialize variables. */
    int i;
    float res;
    double timer;
    int n = strtol(argv[1], NULL, 10);
    srand(time(NULL));
    struct timeval tstart, tend;
    int ** matrix = genRandomMatrix(n, 100);

    /* Time version 1. */
    gettimeofday(&tstart, NULL);
    res = averageMat_v1(matrix, n);
    gettimeofday(&tend, NULL);
    timer = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/1.e6;
    printf("v1 average is: %.2f; time is %g\n", res, timer);

    /* Time version 2. */
    gettimeofday(&tstart, NULL);
    res = averageMat_v2(matrix, n);
    gettimeofday(&tend, NULL);
    timer = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/1.e6;
    printf("v2 average is: %.2f; time is %g\n", res, timer);

    /* Clean up. */
    free_all(matrix, n);
    return 0;
}

int **genRandomMatrix(int n, int max)
{
    /*
	Return a randomly generated matrix
    */

    // Allocate matrix
    int **mat = malloc(n * sizeof(int *));

    // Run loops to intialize matrix
    for (int i = 0; i < n; i++) {

	// Row at index i
        mat[i] = malloc(n * sizeof(int));

	// Column at index j
        for (int j = 0; j < n; j++) {
            mat[i][j] = 1 + rand() % max;
        }
    }

    return mat;
}

void free_all(int **mat, int n)
{

    // Free memory
    for (int i = 0; i < n; i++) {
        free(mat[i]);
    }

    free(mat);
}

float averageMat_v1(int **mat, int n)
{
    int total = 0;

    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            // Note indexing: [i][j]
            total += mat[row][col];
        }
    }

    return (float) total / (n * n);
}

float averageMat_v2(int **mat, int n)
{
    int row, col, total = 0, A = n*n;

    for (int i = 0; i < A; i++) {

	row = i / n;
	col = i % n;

        // Note indexing: [i][j]
        total += mat[row][col];
    }

    return (float) total / (n * n);
}
