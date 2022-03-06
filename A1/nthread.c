#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>


#define MAX 3000
#define BILLION 1E9

int N,n;
int* matA[MAX];
int* matB[MAX];
int* threadC[MAX];
int* seqC[MAX];

void *mythread(void *arg)
{
	int i = *((int *)arg);
	free(arg);
	for (int j = 0; j < n; j++)
	{
		for (int k = 0; k < n; k++)
		{
			threadC[i][j] += matA[i][k] * matB[k][j];
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
	N = atoi(argv[1]);
	double total_seq_time = 0.0, total_thread_time = 0.0;
	FILE *filePointer;
	filePointer = fopen("/mnt/c/users/Anukool Dwivedi/Desktop/Sem6/CS307/assign1/A1/RefQ3/times.txt","a");
	for(int count=1; count<=N; count++){
		n=count;
		for(int i=0; i<n; i++){
			matA[i] = (int*)malloc(n*sizeof(int));
			matB[i] = (int*)malloc(n*sizeof(int));
			threadC[i] = (int*)malloc(n*sizeof(int));
			seqC[i] = (int*)malloc(n*sizeof(int));
		}
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
				matA[i][j] = rand() % 10;
	
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
				matB[i][j] = rand() % 10;
	
		for (int i = 0; i < n; i++){
			for (int j = 0; j < n; j++){
				seqC[i][j] = 0;
				threadC[i][j] = 0;
			}
		}
	
	
		struct timespec requestStart, requestEnd;
		clock_gettime(CLOCK_REALTIME, &requestStart);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				for (int k = 0; k < n; k++)
				{
					seqC[i][j] += matA[i][k] * matB[k][j];
				}
			}
		}
		clock_gettime(CLOCK_REALTIME, &requestEnd);
		double cpu_time_used = ( requestEnd.tv_sec - requestStart.tv_sec )+( requestEnd.tv_nsec - requestStart.tv_nsec )/BILLION;
		pthread_t tid[n];
		clock_gettime(CLOCK_REALTIME, &requestStart);
		for (int i = 0; i < n; i++)
		{
			int *arg = (int*)malloc(sizeof(*arg));
			*arg = i;
			pthread_create(&tid[i], NULL, mythread, arg);
		}
		// join waits for the threads to finish
		for (int i = 0; i < n; i++)
			pthread_join(tid[i], NULL);
		clock_gettime(CLOCK_REALTIME, &requestEnd);
		double cpu_time_used2 = ( requestEnd.tv_sec - requestStart.tv_sec )+( requestEnd.tv_nsec - requestStart.tv_nsec )/BILLION;
	
		int verifyResult = 1;
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				if (seqC[i][j] != threadC[i][j])
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
			printf("*** Multi-threaded implementation took %f seconds ***\n", cpu_time_used2);
		}
		else
		{
			printf("Different output observed!!!\nProgram failed for correctness\n");
		}
		total_seq_time+=cpu_time_used;
		total_thread_time+=cpu_time_used2;
		char data[50];
		sprintf(data,"%f	%f",cpu_time_used,cpu_time_used2);
		fputs(data,filePointer);
		fputs("\n",filePointer);
	}
	fclose(filePointer);
	printf("Sequential time: %f\n",total_seq_time);
	printf("Multithreaded time: %f\n",total_thread_time);

	return 0;
}
