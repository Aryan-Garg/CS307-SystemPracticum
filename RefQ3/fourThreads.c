#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "mythreads.h"

#define MAX 5000

int n;
int matA[MAX][MAX];
int matB[MAX][MAX];
int threadC[MAX][MAX];

void *mythread(void *arg)
{
	int a = *((int *)arg);
	free(arg);
	printf("Starting row: %d\n", a);
	int x = a + n / 4;
	for (int i = a; i < x; i++)
	{
		for (int j = 0; j < n; j++)
		{
			for (int k = 0; k < n; k++)
			{
				threadC[i][j] += matA[i][k] * matB[k][j];
			}
		}
	}
	return NULL;
}

int main(int argc, char **argv)
{
	srand(time(0));
	if (argc != 2)
	{
		printf("Correct usage ./program_name <(N)arg-1>\nNOTE: DO NOT input > 1000\n");
		return 0;
	}
	n = atoi(argv[1]);
	printf("For N = %d\n\n", n);

	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			matA[i][j] = rand() % 10;

	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			matB[i][j] = rand() % 10;

	int matC[n][n];
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			matC[i][j] = 0;

	clock_t start, end;
	double cpu_time_used;
	start = clock();
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			for (int k = 0; k < n; k++)
			{
				matC[i][j] += matA[i][k] * matB[k][j];
			}
		}
	}
	end = clock();
	cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

	start = clock();
	pthread_t tid[4];
	for (int i = 0; i < 4; i++)
	{
		int *arg = malloc(sizeof(*arg));
		*arg = i * n / 4;
		Pthread_create(&tid[i], NULL, mythread, arg);
	}
	// join waits for the threads to finish
	for (int i = 0; i < 4; i++)
		Pthread_join(tid[i], NULL);
	end = clock();
	double cpu_time_used2;
	cpu_time_used2 = ((double)(end - start)) / CLOCKS_PER_SEC;

	int verifyResult = 1;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (matC[i][j] != threadC[i][j])
			{
				verifyResult = 0;
				break;
			}
		}
		if (verifyResult == 0)
		{
			break;
		}
	}

	if (verifyResult)
	{
		printf("\n[+] Verified (element-wise)! Sequential and multi-threaded solution are the same.\n");
		printf("\n*** Sequential implementation took %f seconds ***\n", cpu_time_used);
		printf("*** Multi-threaded implementation (4 threads) took %f seconds ***", cpu_time_used2);
		printf("\n\nSpeed up: %f\n", (double)cpu_time_used * 1.0 / cpu_time_used2);
	}
	else
	{
		printf("Different output observed!!!\nProgram failed for correctness\n");
	}

	return 0;
}
