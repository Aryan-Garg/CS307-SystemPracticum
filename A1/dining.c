#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include <time.h>
#include  <signal.h>
#include<string.h>
sem_t forks[5];
void * philosophers(void *);
void eating(int);
void thinking(int);
void waiting(int);
void puttingForks(int);
void  INThandler(int);
int count_eat[5];
int main()
 {
	signal(SIGTERM, INThandler);
 	for(int i=0; i<5; i++){
 		count_eat[i]=0;
	}
    int i,state[5];
    pthread_t threads[5];
    for(i=0;i<5;i++){
    	sem_init(&forks[i],0,1);	
	}
    for(i=0;i<5;i++){
        state[i]=i;
        pthread_create(&threads[i],NULL,philosophers,(void *)&state[i]);
    }
    for(i=0;i<5;i++){
    	pthread_join(threads[i],NULL);	
	}
 }
void * philosophers(void * n)
 {
 	while(1){
	    int ph=*(int *)n;
		
		thinking(ph);
		waiting(ph);
		eating(ph);
		puttingForks(ph);
		count_eat[ph]++;
    }
 }

void  INThandler(int sig){	
    signal(sig, SIG_IGN);
    for(int i=0; i<5; i++){
    	printf("S%d: %d ",i+1,count_eat[i]);
	}
	printf("\n");
    exit(0);
}

void eating(int ph_no){
	printf("S%d: Eating\n",ph_no);
	sleep(20);
	
}
void thinking(int ph_no){
	printf("S%d: Thinking\n",ph_no);
	sleep((float)rand()/(float)(RAND_MAX/2.0));
}
void waiting(int ph_no){
	if(ph_no==4){
		printf("S%d: Waiting for Spoons\n",ph_no);
		sem_wait(&forks[(ph_no+1)%5]);
		printf("S%d: One spoon acquired\n",ph_no);
		sem_wait(&forks[ph_no]);
	}
	else{
		printf("S%d: Waiting for Spoons\n",ph_no);
		sem_wait(&forks[ph_no]);
		printf("S%d: One spoon acquired\n",ph_no);
		sem_wait(&forks[(ph_no+1)%5]);
	}
	
}
void puttingForks(int ph_no){
	sem_post(&forks[ph_no]);
	sem_post(&forks[(ph_no+1)%5]);
}
