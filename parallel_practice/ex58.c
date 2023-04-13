#include <stdio.h>
#include <omp.h>

int main() {
    float a[] = {4.0, 3.0, 3.0, 1000.0}; 
    int i; 
    float sum = 0.0; 
    #pragma omp parallel for num_threads(2) reduction(+:sum)
    for (i = 0; i < 4; i++) 
    {
        sum += a[i];
        printf("sum = %4.1f\n", sum); 
    }
    return 0;
}
