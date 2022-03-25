#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

#define CHILDREN 4

int main(int argc, char* argv[]){
	int pids[CHILDREN];
	int pipes[CHILDREN][2]; // read end --> [i][0]; write end --> [i][1]

	for(int i = 0; i < CHILDREN; i++){
		if(pipe(pipes[i]) == -1){
			printf("Couldn't initialize pipes! Terminating...\n");
			return 1337;
		}
	}

	for(int i = 0; i < CHILDREN; i++){
		pids[i] = fork();
		if(pids[i] == -1){
			printf("Couldn't create child %d\nTerminating...", i+1);
			return 666;
		}
		else if(pids[i] == 0){
			// 1. Close unneccessary open pipes which amount to 16.
			// 2. Write fn definitions for each child's functioning. 
			return 0; // exit child process; so that child doesn't fork! IMP!!!
		}
	}

	for(int i = 0; i < CHILDREN; i++){
		wait(NULL); // wait for each Child to finish. 
		// Simply calling wait(NULL) will close as soon as one child finishes!
	}


	return 0;
}
