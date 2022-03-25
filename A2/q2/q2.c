#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

#define CHILDREN 4
#define readEnd 0
#define writeEnd 1

int pids[CHILDREN];
int pipes[CHILDREN][2]; // read end --> [i][0]; write end --> [i][1]

void child_1(int i){
        char *filename = "c1encrypt.txt";
        FILE *fp = fopen(filename, "r");

        if (fp == NULL){
                printf("Error: could not open file %s", filename);
                return;
        }

        // read one character at a time and
        // display it to the output
        char ch;
        while ((ch = fgetc(fp)) != EOF) putchar(ch);

        // close the file
        fclose(fp);
        close(pipes[i][readEnd]);
}

int main(int argc, char* argv[]){
        printf("Starting...\n");
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
                else if(pids[i] > 0){
                        // parent process
                        printf("In Parent Process...\n");
                        // send msg to child process then close write pipe
                        close(pipes[i][writeEnd]);
                }
                else if(pids[i] == 0){

                        // 1. Close unneccessary open pipes which amount to 16
                        for(int j = 0; j < CHILDREN; j++){
                                if(i != j){
                                        // close non-i pipes
                                        close(pipes[j][writeEnd]);
                                        close(pipes[j][readEnd]);
                                }
                        }

                        // 2. write each child's operation in a fn definition
                        switch(i){
                                case 0:
                                        printf("Child 1 running...\n");
                                        child_1(i);
                                        break;

                                case 1:
                                        printf("Child 2 running...\n");
                                        // child_2
                                        break;

                                case 2:
                                        printf("Child 3 running...\n");
                                        // child_3
                                        break;

                                case 3:
                                        printf("Child 4 running...\n");
                                        // child_4
                                        break;

                                default:
                                        printf("Something is really spooky!!!\nABORT MISSION");
                                        return 354;

                        }
                        return 0; // exit child process; so that child doesn't fork! IMP!!!
                }
        }

        for(int i = 0; i < CHILDREN; i++){
                printf("Waiting for child: %d to exit\n", i+1);
                wait(NULL); // wait for each Child to finish.
                // IMP Detail: Simply calling wait(NULL) will close as soon as one child finishes!
        }


        return 0;
}
