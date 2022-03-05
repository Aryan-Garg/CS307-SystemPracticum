#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <dirent.h>
#include<sys/wait.h>
#include<string.h>
#include<readline/readline.h>
#include<readline/history.h>
#define maxBuffer 1024

char* listOfCommands[maxBuffer];
char* cmd_hist[maxBuffer];
int cmd_count;

void printCurrentDirectory(){
	char cwd[maxBuffer];
	getcwd(cwd,maxBuffer);
	printf("\033[1;31m");
	printf("\nAMY:%s",cwd);
	printf("\033[0m");
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
	if(strlen(buffer)!=0){
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
        "\nList of Commands supported:"
        "\n>help"
        "\n>clr"
        "\n>pause"
        "\n>quit"
        "\n>history"
        "\n>cd"
		"\n>dir"
		"\n>echo"
		"\n>run"
		"\n>environ"
		"\n>myshell");
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

void showDirectoryContent(){
	struct dirent *de;  
    DIR *dr = opendir(".");
  
    if (dr == NULL)  
    {
        printf("Could not open current directory" );
        return ;
    }
    int cnt=0;
    while ((de = readdir(dr)) != NULL){
    	cnt++;
    	if(cnt>2){
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

void showEnvironmentVariables(char* envp[]){
	for (int i = 0; envp[i] != NULL; i++){
		printf("\n%s", envp[i]);
	}
}

void callFunction(char* cmd_str, char* envp[]){
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
		showEnvironmentVariables(envp);
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

int main(int argc, char** argv, char * envp[]){
	cmd_count=0;
	char inputStr[maxBuffer];
	init_commands();
	while(1){
		printCurrentDirectory();
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
			else if(!strcmp(inputStr,"quit")){
				printf("GoodBye\n");
				exit(0);
			}
			else if(!strcmp(inputStr,"history")){
				showHistory();
			}
			else if(!strcmp(inputStr,"dir")){
				showDirectoryContent();
			}
			else if(!strcmp(inputStr,"environ")){
				showEnvironmentVariables(envp);
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
				   callFunction(dataToBeRead,envp);
			    }
			}
		}
	}
	return 0;
}
