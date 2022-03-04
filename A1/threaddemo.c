#include <stdio.h>
#include<stdlib.h>
#include <time.h>
#define MAX 1001
double timer[MAX];
void multiply(int** mat1, int** mat2, int** res, int n)
{
    int i, j, k;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            res[i][j] = 0;
            for (k = 0; k < n; k++)
                res[i][j] += mat1[i][k] * mat2[k][j];
        }
    }
}
 
int main(int argc, char** argv)
{
	int N = atoi(argv[argc-1]);
	double total_time = 0.0;
	for(int n=1; n<=N; n++){
		int* mat1[n];
		int* mat2[n];
		int* res[n];
		for(int i=0; i<n; i++){
			mat1[i] = (int*)malloc(n*sizeof(int));
			mat2[i] = (int*)malloc(n*sizeof(int));
			res[i] = (int*)malloc(n*sizeof(int));
		}
		for(int i=0; i<n; i++){
			for(int j=0; j<n; j++){
				mat1[i][j]=1;
				mat2[i][j]=1;
			}
		}
		clock_t t;
    	t = clock();
    	multiply(mat1, mat2, res,n);
    	t = clock() - t;
    	double time_taken = ((double)t)/CLOCKS_PER_SEC;
    	total_time+=time_taken;
    	printf("TIME TAKEN: %f\n",time_taken);
    	for(int i=0; i<n; i++){
    		free(mat1[i]);
    		free(mat2[i]);
    		free(res[i]);
		}
	}
	printf("Total time: %f\n",total_time);
    printf("Average time: %f\n",total_time/(float)N);
 
    return 0;
}
