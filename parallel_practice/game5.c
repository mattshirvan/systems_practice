#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int *array_alloc(int n);

int main(int argc, char **argv) 
{
    int n = 8;
    int *arr = array_alloc(n);

    for (int i = 0; i < n; i++) 
    {
      printf("%d\n", arr[i]);
    }
    return 0;
}

int *array_alloc(int n) 
{
    int array[n];
    int i;


    for  (i = 0; i < n; i++) 
    {
      array[i] = i;
    }
    return array;
}