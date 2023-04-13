#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <assert.h>
#include <omp.h>
#include "kmeans.h"

int main(int argc, char **argv)
{
    // Ensure to get number k clusters and data from user
    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s <k> <d>\n", argv[0]);
        fprintf(stderr, "Where <k> is the number of clusters\n");
        fprintf(stderr, "Where <n> is the size of the list to be clustered\n");
        fprintf(stderr, "Where <T> is the Terminal state\n");
        fprintf(stderr, "Where <F> is the number of features in the data\n");
        return 1;
    }

    // Error handling
    int response, ret;

    // k for k-means clustering
    int k = (int)strtol(argv[1], NULL, 10);

    // Length of random array from command line
    int length = (int)strtol(argv[2], NULL, 10);

    // T = terminal state
    int T = (int)strtol(argv[3], NULL, 10);

    // Features
    int features = (int)strtol(argv[4], NULL, 10);

    // Cluster vector
    double **clusters = alloc2D_D(k, length);

    // Array of data objects and features
    double **objects = alloc2D_D(k, length);

    // Array of object membership
    int *membership = (int*) calloc(k, sizeof(int));
    assert(membership != NULL);

    // Seed the rand
    srand(time(NULL));

    // Points vector
    //point points[length];

    // Kmeans without points and cluster structs
    ret = k_means(k, length, k, length, 1000, T, objects, clusters, membership, features);

    // Check return value is 2
    printf("%d\n", ret);

    print2d(objects, k, features);

    // Plot Here     *****FIX ME******
    //plot(k, length, objects, clusters);
}

void randomize(cluster *array, long length, int k)
{
    /*
    Generates a random array
    */

    for (int i = 0; i < k; i++)
    {
	    array[i].sum.x = rand() % k;
        array[i].sum.y = rand() % k;
    }
}

void randomizer(point *array, long length, int k)
{
    /*
    Generates a random array
    */

    for (int i = 0; i < length; i++)
    {
	    array[i].x = rand() % k;
        array[i].y = rand() % k;
    }
}

double distance(point p, point q)
{
   /*
    Calculates distance between two points
   */

    return sqrt(pow(p.x - q.x, 2) + pow(p.y - q.y, 2));
}

double distances(int features, double *p, double *q)
{
    double sum = 0.0;

    for (int i = 0; i < features; i++)
    {
        sum += (p[i]-q[i]) * (p[i]-q[i]);
    }

    return sum;
}

double sum(double *dist, long n)
{
    /*
    Sums an array of length n
    */

    double sums = 0;

    for (int i = 0; i < n; i++)
    {
	    sums += dist[i];
    }

    return sums;
}

bool stop(cluster c1, cluster c2, int t, int T)
{
    /*
    Stopping condition for Kmeans clustering algorithm
    */

    if (t >= T)
    {
        return true;
    }

    else if (c1.sum.cluster_id == c2.sum.cluster_id)
    {
        return true;
    }

    return false;
}

int** alloc2D(int rows, int cols)
{

    int **array;

    // allocate an array of N pointers to ints
    // malloc returns the address of this array (a pointer to (int *)'s)
    array = malloc(sizeof(int *) * rows);

    // for each row, malloc space for its column elements and add it to
    // the array of arrays
    for (int i = 0; i < rows; i++) 
    {
        // malloc space for row i's M column elements
        array[i] = malloc(sizeof(int) * cols);
    }

    init2D(array, rows, cols);

    return array;
}

double** alloc2D_D(int rows, int cols)
{

    double **array;

    // allocate an array of N pointers to ints
    // malloc returns the address of this array (a pointer to (int *)'s)
    array = malloc(sizeof(double *) * rows);

    // for each row, malloc space for its column elements and add it to
    // the array of arrays
    for (int i = 0; i < rows; i++) 
    {
        // malloc space for row i's M column elements
        array[i] = malloc(sizeof(double) * cols);
    }

    init2DD(array, rows, cols);

    return array;
}

void init2D(int **array, int rows, int cols)
{
    for (int i = 0; i < rows; i++) 
    {
        for (int j = 0; j < cols; j++) 
        {
            array[i][j] = rand() % cols;
        }
    }
}

void init2DD(double **array, int rows, int cols)
{
    for (int i = 0; i < rows; i++) 
    {
        for (int j = 0; j < cols; j++) 
        {
            array[i][j] = rand() % cols;
        }
    }
}

void print2d(double **array, int k, int features)
{
    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < features; j++)
        {
            printf("%f ", array[i][j]);
        }
        printf("\n");
    }
}

void plot(int k, int features, double **x, double **y)
{
    FILE *gnuplot = popen("gnuplot", "w");
    fprintf(gnuplot, "plot '-'\n");
    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < features; j++)
        {
            fprintf(gnuplot, "%g %g\n", x[i][j], y[i][j]);
        }
    }
    fprintf(gnuplot, "e\n");
    fflush(gnuplot);
}

int k_means(int rows, int cols, int k, int n, double threshold, int T, double **objects, double **clusters, int *membership, int features)
{
    int delta;
    double distance, dmin;
    int t, cluster_id;
    int *new_cluster_size; /* [num_clusters]: no. objects assigned in each new cluster */
    double  **new_clusters;

    new_cluster_size = (int*) calloc(k, sizeof(int));
    assert(new_cluster_size != NULL);

    new_clusters = alloc2D_D(rows, cols);

    while (delta > threshold && t < T)
    {
        delta = 0;

        dmin = distances(features, objects[0], clusters[0]);

        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                distance = distances(features, objects[i], clusters[j]);
                
                if (distance < dmin)
                {
                    dmin = distance;
                    cluster_id = j;
                }
            }

            if (membership[i] != cluster_id)
            {
                delta += 1.0;
                membership[i] = cluster_id;
            }

            new_cluster_size[cluster_id]++;

            for (int j = 0; j < features; j++)
            {
                new_clusters[cluster_id][j] += objects[i][j];
            }


            for (int i = 0; i < k; i++)
            {
                for (int j = 0; j < features; j++)
                {
                    if (new_cluster_size[i] > 0)
                    {
                        clusters[i][j] = new_clusters[i][j] / new_cluster_size[i];
                    }

                    new_clusters[i][j] = 0.0;
                }
                new_cluster_size[i] = 0;
            }
        }

        delta /= n;
    }

    free(new_clusters[0]);
    free(new_clusters);
    free(new_cluster_size);

    return 2;
}