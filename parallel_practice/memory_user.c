#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <math.h>
#include "lab5.h"



int main(int argc, char **argv)
{
    // PDF: probability distribution array
    double* distribution;

    // mean and standard deviation
    double average, deviation;

    // N the size of the random array in MB from command line
    int n;

    // T the time to termination
    int t;

    // Seed random generator 
    srand((unsigned int)time(NULL));

    // Check for proper amount of command line arguments 
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <N> <T>\n", argv[0]);
        fprintf(stderr, "Where <N> is the size of the list in MB\n");
        fprintf(stderr, "Where <T> is the time of termination \n");
        return 1;
    }

    // Length of random array in MB from command line
    n = (int)strtol(argv[1], NULL, 10);

    // T = terminal state
    t = (int)strtol(argv[2], NULL, 10);

    // Count up to termination t
    int i = 0;
    do
    {
        // Create normal distribution of size n MB
        distribution = normal_distribution(n);

        // Find the mean of the distribution
        average = mean(distribution, n);

        // Compute the standard deviation
        deviation = stddev(distribution, n);
        
        // Check if the distribution was successful or exists
        if ( distribution == NULL )
        {
            return EXIT_FAILURE;
        }
        // Print the mean and standard deviation
        printf("mean = %g, standard deviation = %g\n\n", average, deviation);
        
        // Print the probability density function
        printHistogram(distribution, n);

        // Call free on the data set
        free(distribution);

    } while (i++ < t);
    
    // Keep open in case termination happens before free is called.
    printf("\n%s\n", "press enter");
    getchar();

    return EXIT_SUCCESS;
}

double mean(double *values, int length)
{
    // Calculate and the mean of a normal distribution
    double sum = 0;

    for (int i = 0; i < length; i++)
    {
        sum += values[i];
    }

    return sum/length;
}

double stddev(double *values, int length)
{
    double average = mean(values, length);
    double sum = 0;

    for (int i = 0; i < length; i++)
    {
        
        sum += (values[i] - average) * (values[i] - average);
    }

    return sqrt(sum / (length - 1));
}

double *normal_distribution(int n)
{
    int i;
    int m = n + n % 2;
    double* values = (double*)calloc(m,sizeof(double));
    double average, deviation;
 
    if ( values )
    {
        for ( i = 0; i < m; i += 2 )
        {
            double x,y,rsq,f;
            do {
                x = 2.0 * rand() / (double)RAND_MAX - 1.0;
                y = 2.0 * rand() / (double)RAND_MAX - 1.0;
                rsq = x * x + y * y;
            }while( rsq >= 1. || rsq == 0. );
            f = sqrt( -2.0 * log(rsq) / rsq );
            values[i]   = x * f;
            values[i+1] = y * f;
        }
    }
    return values;
}

void printHistogram(double* values, int n)
{
    const int width = 50;    
    int max = 0;
 
    const double low   = -3.05;
    const double high  =  3.05;
    const double delta =  0.1;
 
    
    int nbins = (int)((high - low) / delta);
    int* bins = (int*)calloc(nbins,sizeof(int));
    if ( bins != NULL )
    {
        for (int i = 0; i < n; i++ )
        {
            int j = (int)( (values[i] - low) / delta );
            if ( 0 <= j  &&  j < nbins )
                bins[j]++;
        }
 
        for (int j = 0; j < nbins; j++ )
            if ( max < bins[j] )
                max = bins[j];
 
        for (int j = 0; j < nbins; j++ )
        {
            printf("(%5.2f, %5.2f) |", 
            low + j * delta, 
            low + (j + 1) * delta );
            int k = (int)( (double)width * (double)bins[j] / (double)max );
            while(k-- > 0) putchar('*');
            printf("  %-.1f%%", bins[j] * 100.0 / (double)n);
            putchar('\n');
        }
 
        free(bins);
    }
}