#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <dirent.h>
#include<sys/wait.h>
#include<string.h>
#include<readline/readline.h>
#include<readline/history.h>
#include <errno.h>
#include <dirent.h>

#define maxBuffer 1024

char* listOfCommands[maxBuffer];
char* cmd_hist[maxBuffer];
int cmd_count;
extern char **environ;

void printCurrentDirectory(int fancy){
	char cwd[maxBuffer];
	getcwd(cwd,maxBuffer);
	if(fancy){	
		printf("\033[1;31m");
		printf("\nAMY:%s",cwd);
		printf("\033[0m");
	}
	else
		printf("No arguement passed to cd(PWD); Currently at: %s",cwd);

}

void saveHistory(char* buffer){
	FILE *filePointer;
	filePointer = fopen("/mnt/c/Users/HP/Desktop/sem_6/SysPrac/A1/command_history.txt","a");
	if(strlen(buffer)>0){
		fputs(buffer,filePointer);
		fputs("\n",filePointer);
	}
	fclose(filePointer);
}

int inputTaken(char* line){
	char* buffer;
	buffer = readline("$ ");
	if(!buffer){
		printf("Ctrl-D was hit!\nQuitting... Goodbye.\n");
		exit(0);
	}
	else if(strlen(buffer)!=0){
		saveHistory(buffer);
		cmd_hist[cmd_count++]=buffer;
		add_history(buffer);
		strcpy(line,buffer);
		return 0;
	}else{
		return 1;
	}
}

void init_commands(){
	listOfCommands[0]="help";
	listOfCommands[1]="clr";
	listOfCommands[2]="pause";
	listOfCommands[3]="quit";
	listOfCommands[4]="history";
	listOfCommands[5]="cd";
	listOfCommands[6]="dir";
	listOfCommands[7]="echo";
	listOfCommands[8]="run";
	listOfCommands[9]="environ";
	listOfCommands[9]="myshell";
}

void giveHelp(){
	puts(
        "List of Commands supported:"
        "\n1. help"
        "\n2. clr"
        "\n3. pause"
        "\n4. quit"
        "\n5. history"
        "\n6. cd <directory>"
		"\n7. dir <directory>"
		"\n8. echo <your-string>"
		"\n9. run"
		"\n10. environ"
		"\n11. myshell");
}

void showHistory(){
	FILE *filePointer;
	filePointer = fopen("/mnt/c/Users/HP/Desktop/sem_6/SysPrac/A1/command_history.txt","r");
	char dataToBeRead[100];
	while( fgets ( dataToBeRead, 100, filePointer ) != NULL )
    {
        printf( "%s" , dataToBeRead ) ;
    }
}

void showDirectoryContent(char* dirName){
	struct dirent *de;  
	char cwd2[maxBuffer];
	getcwd(cwd2,maxBuffer);
	strcat(cwd2, "/");
	strcat(cwd2, dirName);
	printf("DEBUG_PRINT: %s\n", cwd2);
    
	DIR *dr = opendir(cwd2);
	
    if (dr == NULL) {
        switch (errno) {
            case EACCES: printf("Permission denied\n"); break;
            case ENOENT: printf("Directory does not exist\n"); break;
            case ENOTDIR: printf("'%s' is not a directory\n", dirName); break;
        }
		return;
    }
	int cnt = 0;
    while ((de = readdir(dr)) != NULL){
		cnt++;
		if (cnt > 2){
			printf("%s\n", de->d_name);
		}
	}
    closedir(dr);
}

void runEcho(char** tokens, int count_command){
	for(int i=1; i<count_command; i++){
		printf("%s ",tokens[i]);
	}
	printf("\n");
}

int tokenize(char* input_string, char* toks[]){
	char * token = strtok(input_string, " ");
	int ctr=0;
    while( token != NULL ) {
       toks[ctr++]=token;
       token = strtok(NULL, " ");
    }
    return ctr;
}

void changeCWD(char* new_directory){
	chdir(new_directory);
}

void runExecutable(char** tokens){
	pid_t pid = fork(); 
	if (pid == -1) {
	    printf("Failed forking child..\n");
	} else if (pid == 0) {
		char *args[]={tokens[1],NULL};
	    if (execvp(args[0], args) < 0) {
	        printf("\nCould not execute command.");
        }
	    exit(0);
	} else {
	    wait(NULL); 
	}
}

void showEnvironmentVariables(){
	for (char **env = environ; *env; ++env) printf("%s\n", *env);
}

void callFunction(char* cmd_str){
	if(!strcmp((const char*)cmd_str,"help")){
		giveHelp();
	}
	else if(!strcmp((const char*)cmd_str,"history")){
		showHistory();
	}
	else if(!strcmp((const char*)cmd_str,"clr")){
		system("clear");
	}
	else if(!strcmp((const char*)cmd_str,"environ")){
		showEnvironmentVariables();
	}
	char* tokens[100];
	int cnt_val = tokenize(cmd_str,tokens);
	if(!strcmp((const char*)tokens[0],"echo")){
		runEcho(tokens,cnt_val);
	}
	else if(!strcmp((const char*)tokens[0],"run")){
		runExecutable(tokens);
	}
}

int main(int argc, char** argv){
	cmd_count=0;
	char inputStr[maxBuffer];
	init_commands();
	// set shell env variable (Q10)
	char shell_env[maxBuffer] = "shell=";
	char cwd123[maxBuffer];
	getcwd(cwd123,maxBuffer);
	strcat(shell_env, cwd123);
	strcat(shell_env, "=myshell");
	if (putenv(shell_env) == -1) printf("putenv: Env var: shell failed\n");
	else printf("Setting env var shell to location from which shell was invoked.\n[+] env var: shell=%s", getenv("shell"));
		
	while(1){
		printCurrentDirectory(1);
		if(inputTaken(inputStr)){
			continue;
		}
		int counter = 0;
		char* tokens[100];
		counter = tokenize(inputStr,tokens);
		if(counter==1){
			if(!strcmp(inputStr,"help")){
				giveHelp();
			}
			else if(!strcmp(inputStr,"clr")){
				system("clear");
			}
			else if(!strcmp(inputStr,"pause")){
				printf("Press Enter to terminate indefinite pause...\n");
				char lookForEnter;
				while((lookForEnter = getchar()) != '\n');  
			}
			else if(!strcmp(inputStr,"quit")){
				printf("GoodBye\n");
				exit(0);
			}
			else if(!strcmp(inputStr, "history")){
				showHistory();
			}
			else if(!strcmp(inputStr, "dir")){
				printf("No directory provided.");
			}
			else if(!strcmp(inputStr,"environ")){
				showEnvironmentVariables();
			}
			else if(!strcmp(inputStr,"cd")){    
				char cwd_lo[maxBuffer]; 
				getcwd(cwd_lo, maxBuffer);
				printf("No arguement passed to cd(PWD)\nCurrently at: %s", cwd_lo);
				if(setenv("PWD", cwd_lo, 2) == 0)
					printf("\n[+] env var: PWD set to: %s", getenv("PWD"));
				else 
					printf("\n[-] Couldn't set env var PWD :(");
			}
		}
		
		if(counter>1){
			if(!strcmp(tokens[0],"cd")){
				changeCWD(tokens[1]);
			}
			else if(!strcmp(tokens[0],"echo")){
				runEcho(tokens,counter);
			}
			else if(!strcmp(tokens[0],"run")){
				runExecutable(tokens);
			}
			else if(!strcmp(tokens[0],"dir")){
				showDirectoryContent(tokens[1]);
			}
			else if(!strcmp(tokens[0],"myshell")){
				FILE *filePointer;
				filePointer = fopen(tokens[1],"r");
				char dataToBeRead[100];
				while( fgets ( dataToBeRead, 100, filePointer ) != NULL )
			    {
			       char *c = strchr(dataToBeRead, '\n');
				   if (c){
				   	   *c = 0;
				   }
				   callFunction(dataToBeRead);
			    }
			}
			else{
				printf("Invalid command or too many arguements passed. Try: help for more info.");
			}
		}
	}
	return 0;
}
