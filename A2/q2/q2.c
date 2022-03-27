#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>


#define CHILDREN 4
#define readEnd 0
#define writeEnd 1

int pids[CHILDREN];
int pipes[CHILDREN][2]; // read end --> [i][0]; write end --> [i][1]

void child_1(int i){
	char *filename = "c1encrypt.txt";
	FILE *fp = fopen(filename, "r");
	
	char orig[26], enc_orig[26];
	int count = 0;
	
	char * line = NULL;
    	size_t len = 0;
    	ssize_t read;

	while ((read = getline(&line, &len, fp)) != -1) {
		orig[count] = line[0];
		enc_orig[count++] = line[2];
    	}

	// verification print
	//for(int a = 0; a < 26; a++){
	//	printf("-> %c %c\n", orig[a], enc_orig[a]);
	//}
	
	char strstr[100];

	printf("Type a string (in uppercase): ");
	scanf("%s", strstr);
	// printf("Entered string: %s\n", strstr);
	
    	if (fp == NULL){
        	printf("Error: could not open file %s", filename);
        	return;
    	}
	
	char new_str[100];
	for(int j = 0; j < strlen(strstr); j++){
		// read all characters in i/p
		for(int asd = 0; asd < 26; asd++){
			if(orig[asd] == strstr[j]){
				// printf("Matched! %c at %d", strstr[j], asd);
				new_str[j] = enc_orig[asd];
				break;
			}
		}
	}
	//printf("Encrypted string: ");
	//for(int ab = 0; ab < strlen(strstr); ab++){
        //        printf("%c", new_str[ab]);
        //}
	// printf("\n");
		
    	// read one character at a time and display it to the output
	
	close(1); // stdout: close
    	dup(pipes[i][writeEnd]);
	write(pipes[i][writeEnd], new_str, sizeof(strstr));

    	fclose(fp);
	close(pipes[i][readEnd]);
	return;
}

void child_2(){
	char *filename = "c2text.txt";
        FILE *fp = fopen(filename, "r");
	char * line = NULL;
        size_t len = 0;
        ssize_t read;
	while ((read = getline(&line, &len, fp)) != -1) {
                printf("[C2] Speaking: %s ", line);
		// TODO: do the espeak thing here.
        }
	fclose(fp);
	return;
}

void child_3(){
	// read and copy an input file.
	char *filename = "c3file.txt";
        FILE *fp = fopen(filename, "r");
	
	char *copy_fname = "c3file_copy.txt";
	FILE *fp_cp = fopen(copy_fname, "w");

	if(fp_cp == NULL){
		printf("[ERROR] Couldn't create a file to copy content of %s.", filename);
		return;
	}
	char * line = NULL;
        size_t len = 0;
        ssize_t read;
	while ((read = getline(&line, &len, fp)) != -1) {
                fputs(line, fp_cp);
        }
	fclose(fp);
	fclose(fp_cp);
	return;
}

void child_4(int fdId){
	// CPU monitoring stuff...
	int readings = 10;
       	int delay = 2; // in seconds
	
	char str[100];
	char delim[2] = " "; 
	char* token;
	long int sum = 0, idle, lastSum = 0,lastIdle = 0;
	long double idleFraction;
	char strToSend[readings][20];

	while(readings > 0){
		char *procStatFile = "/proc/stat";
        	FILE *fp = fopen(procStatFile, "r");
		int find_idle = 0; // at index 3; store idle time
		fgets(str, 100, fp);
		fclose(fp);
		token = strtok(str, delim);
		sum = 0;
		while(token != NULL){
			token = strtok(NULL, delim);
			if(token != NULL) sum += atoi(token);
			if(find_idle == 3) idle = atoi(token);
			find_idle++;
		}
		idleFraction = 100 - (idle - lastIdle)*100.0/(sum-lastSum);
		printf("[C4] CPU Usage: %Lf %%\n", idleFraction);
		sprintf(strToSend[10-readings], "%Lf", idleFraction);
		// printf("Long float typecasted to string: %s\n", strToSend[10-readings]);	
		lastIdle = idle;
		lastSum = sum;
		
		readings--;
		sleep(delay);
	}
	
	close(1); // stdout: close
        dup(pipes[fdId][writeEnd]);

	write(pipes[fdId][writeEnd], strToSend, sizeof(strToSend));
        close(pipes[fdId][readEnd]);
	
	return;
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
			
			if(i == 0){ // c1
				close(0);
				dup(pipes[i][0]);

				char arr[100];

      				// n stores the total bytes read successfully
      				int n = read(pipes[i][0], arr, sizeof(arr));
				printf("In parent (receiving stuff from C1)...\nEncrypted string: ");
      				int ctr = 0;
				while(arr[ctr] <= 'Z' && arr[ctr] >= 'A') {
					printf("%c", arr[ctr]);
					ctr++;
				}
				printf("\n");
			}
			else if (i == 3){
				// Print the CPU usage %
				close(0);
				dup(pipes[i][readEnd]);

				char arr[1000];
				int n = read(pipes[i][readEnd], arr, sizeof(arr));
				printf("In parent (receiving from C4)...\n");
				int k = 0;
				while(k < n){
					printf("%d",arr[k++]);
					printf(" ");
				}
				printf("[!] Need to fix output received in parent from C4.\n");
			}

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
					child_2();
					break;
				
				case 2:
					printf("Child 3 running...\n");
					child_3();
					break;

				case 3:
					printf("Child 4 running...\n");
					child_4(i);
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
