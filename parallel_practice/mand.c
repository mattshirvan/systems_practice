/*  Be sure to comment your code, including a description and 
 *  your name and date
 *
 */
#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <time.h>
// comment: where does 4.0 come from?
static double scaleFactor(int start, int end){
    double factor = 0.0;
    double num = start - end;
    factor = num / 4.0;
    return factor;
}
// scale converts from array coords to complex coords based on size
// comment: describe what kernel does
static int kernel(int row, int col, int maxCount, int size){
    int i = 0;
    complex z = CMPLX(0,0);
    complex cc = CMPLX(scaleFactor(col, size), scaleFactor(row, size));
    for (int i = 0; i < maxCount; i++){
        z = z*z + cc;
        if (cabs(z) >= 2.0){ 
            return i;
        }
    }
    return i;
}
int main(void){
    const int maxCount = 1000;
    int size = 1000;
    int arr[size][size];
    time_t start = time(NULL);  // or use gettimeofday()
    
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            arr[i][j] = kernel(i, j, maxCount, size);
        }
    }
    time_t end = time(NULL);
    printf("Time total: %f10 seconds\n", difftime(end, start)); 
    for (int i = 0; i < size; i++){
         for (int j = 0; j < size; j++){
             printf("%d", arr[i][j]);
         }
         printf("\n");
     }
}
