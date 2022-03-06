#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "mythreads.h"

#define MAX 1000

int n;
int matA[MAX][MAX];
int matB[MAX][MAX];
int threadC[MAX][MAX];

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *mythread(void *arg)
{
	int a = *((int *)arg);
	free(arg);
	int block_i_s, block_i_e, block_j_s, block_j_e;

	switch (a)
	{
	case 0:
		block_i_s = 0;
		block_i_e = n / 2;
		block_j_s = 0;
		block_j_e = n / 2;
		break;

	case 1:
		block_i_s = 0;
		block_i_e = n / 2;
		block_j_s = n / 2;
		block_j_e = n;
		break;

	case 2:
		block_i_s = n / 2;
		block_i_e = n;
		block_j_s = 0;
		block_j_e = n / 2;
		break;

	case 3:
		block_i_s = n / 2;
		block_i_e = n;
		block_j_s = n / 2;
		block_j_e = n;
		break;
	}

	// Coarse grained lock :)
	Pthread_mutex_lock(&lock);
	printf("Thread id: %d\n", a);
	printf("It solves the block: (%d, %d) to (%d, %d)\n\n", block_i_s, block_j_s, block_i_e, block_j_e);
	for (int i = block_i_s; i < block_i_e; i++)
	{
		for (int j = 0; j < n; j++)
		{
			for (int k = block_j_s; k < block_j_e; k++)
			{
				threadC[i][j] += matA[i][k] * matB[k][j];
			}
		}
	}
	Pthread_mutex_unlock(&lock);

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
		*arg = i;
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
		printf("*** Multi-threaded implementation (1 thread per square-block) took %f seconds ***\n", cpu_time_used2);
		printf("\nSpeed up: %f\n", (double)cpu_time_used * 1.0 / cpu_time_used2);
	}
	else
	{
		printf("Different output observed!!!\n");
	}

	return 0;
}