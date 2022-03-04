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
void  INThandler(int);
int count_eat[5];
int main()
 {
	signal(SIGINT, INThandler);
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

	    printf("S%d: Thinking\n",ph);
	    
	    sleep((float)rand()/(float)(RAND_MAX/2.0));
	    
		printf("S%d: Waiting for Spoons\n",ph);
	         
	    sem_wait(&forks[ph]);
	         
		printf("S%d: One spoon acquired\n",ph);
			 
	    sem_wait(&forks[(ph+1)%5]);
	         
		printf("S%d: Eating\n",ph);
			 
	    sleep(20);
		     
		sem_post(&forks[(ph+1)%5]);
	    sem_post(&forks[ph]);
	    
		count_eat[ph]++;
    }
 }
 void  INThandler(int sig)
{	
    signal(sig, SIG_IGN);
    for(int i=0; i<5; i++){
    	printf("%d ",count_eat[i]);
	}
	printf("\n");
    exit(0);
}
