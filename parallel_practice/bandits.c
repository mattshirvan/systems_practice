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
    // Initialize bandits list for returns
    list multi_armed_bandit;
    init(&multi_armed_bandit);

    // Run simulation Runs * Steps times
    multi_armed_bandit = simulate(2000, 1000, 10, .1);

    // Display values
    printlist(&multi_armed_bandit);

    // Write data to file
    FILE *f = fopen("bandit_values.data", "wb");
    fwrite(multi_armed_bandit.values, sizeof(double), sizeof(multi_armed_bandit.values), f);
    fclose(f);

    // Free list
    free(multi_armed_bandit.values);
}

list simulate(int runs, int steps, int k, double epsilon)
{
    
    // Average returns
    list returns;
    init(&returns);

    // Simulate runs
    for (int i = 1; i <= runs; i++)
    {
        // Add values to returns
        append(&returns, sum(simple_bandit(epsilon, steps, k)) / i);
    }

    return returns;
}

double sum(list values)
{   
    // Sum values
    double total = 0;

    // Run summation
    for (int i = 0; i < values.length; i++)
    {
        total += values.values[i];
    }

    return total;
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

double *normal_distribution(int n)
{   
    
    // Keep even length
    n = n + n % 2;

    // Distribution values
    double* values = (double*)calloc(n, sizeof(double));
    
    // Mean and Std. Deviation
    double average, deviation;
    
    // Distribution array is valid
    if (values)
    {   
        // Create distribution
        for (int i = 0; i < n; i += 2 )
        {
            double x, y, sq, f;
            do {
                x = 2.0 * rand() / (double)RAND_MAX - 1.0;
                y = 2.0 * rand() / (double)RAND_MAX - 1.0;
                sq = x * x + y * y;
            }while( sq >= 1. || sq == 0. );

            // Calulate value
            f = sqrt( -2.0 * log(sq) / sq );

            // Store distribution
            values[i]   = x * f;
            values[i+1] = y * f;
        }
    }
    return values;
}

void init(list *list)
{
    list->values = (double *)calloc(1, sizeof(double));
    list->length = 1;
}

double mean(double *values, int length)
{
    // Calculate the mean of a normal distribution
    double sum = 0;

    for (int i = 0; i < length; i++)
    {
        sum += values[i];
    }

    return sum/length;
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

void remove_val(list *list, double value)
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

void pop(list *list)
{
    double temp_array[list->length]; // without "-1" because it is uncertain if the list includes the element

    // adding all elements from list->list to temp_array, except the one that should be removed
    for (int i = 0; i < list->length; i++)
    {
        // The element that should be removed
        temp_array[i] = list->values[i];
        
    }
    
    free(list->values);

    // adding elements from temp_array to list->list
    for (int i = 0; i < list->length - 1; i++)
    {
        list->values[i] = temp_array[i];
    }

    list->length--;
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
    
    free(distr);
}

void del(list *l)
{
    list temp;
    
    init(&temp);

    l = &temp;
}