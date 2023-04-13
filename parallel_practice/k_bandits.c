#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <math.h>
#include "bandits.h"

int main(int argc, char **argv)
{

    double* distribution;
    list multi_armed_bandit;
    double average, deviation;
 
    srand((unsigned int)time(NULL));

    distribution = normal_distribution(MAX);
    average = mean(distribution, MAX);
    deviation = stddev(distribution, MAX);
 
    if ( distribution != NULL )
    {
        printf("mean = %g, standard deviation = %g\n\n", average, deviation);
        printHistogram(distribution, MAX);
        free(distribution);


        multi_armed_bandit = simple_bandit(0.1, 1000, 10);
        printlist(&multi_armed_bandit);
        free(multi_armed_bandit.values);
 
        printf("\n%s\n", "press enter");
        getchar();
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

int argmax(double *Q, int k)
{
    double max = -INFINITY;

    list ties;
    init(&ties);

    for (int i = 0; i < k; i++)
    {
        if (Q[i] > max)
        {
            max = Q[i];

            del(&ties);
        }

        if (Q[i] == max)
        {
            append(&ties, i);
        }
    }

    int arg_max = rand() % k;
    int action = ties.values[arg_max];
    free(ties.values);
    return action;
}

int e_greedy(double *Q, double epsilon, int k)
{
    int action = argmax(Q, k);

    if (rand()/RAND_MAX <= epsilon)
    {
        action = rand() % k;
    }

    return action;
}

double bandit(int A)
{
    double * distribution;
    double ret;
    distribution = normal_distribution(MAX);
    ret = distribution[A];
    free(distribution);
    return ret;
}

list simple_bandit(double epsilon, int steps, int k)
{
    // Q(a) <- 0
    double *Q = calloc(k, sizeof(double));
    if (!Q)
    {
        exit(1);
    }

    // N(a) <- 0
    int *N = calloc(k, sizeof(int));
    if (!N)
    {
        exit(2);
    }

    list rewards, averages;
    init(&rewards);
    init(&averages);
    append(&rewards, 0);

    // Initialize q*(a) (bandit rewards unknown to agent)
    double *q = calloc(k, sizeof(double));
    if (!q)
    {
        exit(3);
    }
    
    // draw q*(a) from random gaussian distribution
    init_q(q, k);
    
    // Run simulation 
    for (int i = 0; i < steps; i++)
    {
        int A;
        double R;

        A = e_greedy(q, epsilon, k);

        R = bandit(A);

        N[A]++;
        
        Q[A] = Q[A] + 1/N[A] * (R - Q[A]);

        append(&rewards, rewards.values[rewards.length-1] + R);
        append(&averages, rewards.values[rewards.length-1] / (i+1)); 
    }

    free(N);
    free(Q);
    free(q);
    free(rewards.values);
    return averages;
}

list simulate(int runs, int steps, int k, double epsilon)
{

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

void init(list *list)
{
    list->values = calloc(1, sizeof(double *));
    list->length = 0;
}

void append(list *list, double value)
{
    double temp_array[list->length + 1]; // + 1 is for a new element

    // adding the list->list elements to the temp_array
    for (int i = 0; i < list->length; i++)
    {
        temp_array[i] = list->values[i];
    }

    // adding the new element to the temp_array
    temp_array[list->length] = value; 

    // freeing the list->list 
    free(list->values);

    // allocating memory for list->list
    list->values = calloc(list->length + 1, sizeof(char *)); 

    // adding elements from temp_array to list->list
    for (int i = 0; i < list->length + 1; i++)
    {
        list->values[i] = temp_array[i];
    }

    list->length++;
}

void pop(list *list, double value)
{
    double temp_array[list->length]; // without "-1" because it is uncertain if the list includes the element
    // a variable for tracking, whether or not the list includes the element; set to "false" by default
    int found = 0;

    // adding all elements from list->list to temp_array, except the one that should be removed
    for (int i = 0; i < list->length; i++)
    {
        // if the element is not the one that should be removed
        if (list->values[i] != value)
        {
            temp_array[i] = list->values[i];
        }
        else
        {
            found = 1;
        }
    }

    // if not found there is no point of doing the following
    if (found)
    {
        // freeing the list->list (it should be allocated to make this function work)
        free(list->values);

        // allocating memory for list->list
        list->values = calloc(list->length - 1, sizeof(char *));

        // adding elements from temp_array to list->list
        for (int i = 0; i < list->length - 1; i++)
        {
            list->values[i] = temp_array[i];
        }

        list->length--;
    }
}

void printlist(list *list)
{
    for (int i = 0; i < list->length; i++)
    {
        printf("%.2f ", list->values[i]);
    }
    printf("\n");
}

void init_q(double *q, int k)
{
    double *distr;
    distr = normal_distribution(MAX);
    for (int i = 0; i < k; i++)
    {
        int index = rand() % MAX;
        
        q[i] = distr[index];
    }
    
    //free(distr);
}

void del(list *l)
{
    list temp;
    
    init(&temp);

    l = &temp;
}