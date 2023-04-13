#include <stdbool.h>
#ifndef _KMEANS_H_
#define _KMEANS_H_

// Create a point
typedef struct point
{
   double x, y;
   int cluster_id;
} point;


// Create a cluster
typedef struct cluster
{
   point sum;
   int count;
} cluster;


// Prototypes
double distance(struct point p, struct point q);
void randomize(cluster *array, long length, int k);
void randomizer(point *array, long length, int k);
double sum(double *dist, long n);
bool stop(cluster c1, cluster c2, int t, int T);
int** alloc2D(int rows, int cols);
void init2D(int **array, int rows, int cols);
void init2DD(double **array, int rows, int cols);
int k_means(int rows, int cols, int k, int n, double threshold, int T, double **objects, double ** clusters, int *membership, int features);
double** alloc2D_D(int rows, int cols);
double distances(int features, double *p, double *q);
void plot(int k, int features, double **x, double **y);
void print2d(double **array, int k, int features);

#endif
