/*
* Name: Rishabh Vaidya
* PID: 5918963
* I, Rishabh Vaidya, affirm that I wrote this program myself 
* without any help from any other people or sources from the internet.
* This program aims to implement a shell that recognizes the >, >>, <, and | commands
* The shell will invoke these commands and wire up standard input and output so that they chain up 
*/

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ARGUMENTS 20		//constant declaration

void changecmd(char** args, int nargs, int counter);		//method declaration

/* 
* method to get and read arguments given by user 
* takes in two char* parameters
* returns an integer
*/
int get_args(char* cmdline, char* args[]) 
{
	int num = 0;

	//if no arguments are given
	if((args[0] = strtok(cmdline, "\n\t ")) == NULL) {
		return 0; 
	}
	while((args[++num] = strtok(NULL, "\n\t ")) != NULL) {
		if(num >= MAX_ARGUMENTS) {
			printf("ERROR: Too many arguments!\n");
			exit(1);
		}
	}
	
	// the last argument is always null
	return num;
}

/*
* method uses child and parent processes to execute user given commands
* takes in a char* parameters
* does not return anything because it is a void method
*/  
void executecmd(char* cmdline) 
{
	int pid, sid, async, number = 0, x, y, start = 0;
	char* args[MAX_ARGUMENTS];
	int pipes_arr [2];
	int nargs = get_args(cmdline, args);
	char* temp[nargs];
	 
	if(nargs <= 0) {
		return;
	}	
	if(!strcmp(args[0], "quit") || !strcmp(args[0], "exit")) {
		exit(0);
	}
	// check if async call
	if(!strcmp(args[nargs-1], "&")) { 
		async = 1; args[--nargs] = 0; 
	} else {
		async = 0;
	}
	  
	pid = fork();		//fork process
  
	//child process
	if(pid == 0) { 
		//loops through arguments and looks for given symbol, then calls the changecmd method
		while(args[number] != NULL) {
				if (strcmp(args[number], ">") == 0 || strcmp(args[number], "<") == 0) {
					changecmd(args, nargs, number);
				} else if(strcmp(args[number], ">>") == 0) {       
					changecmd(args, nargs, number);
				} else if(strcmp(args[number], "|") == 0){	//piping command
					args[number] = NULL;
					pipe(pipes_arr);
					sid = fork();
					//child process
					if (sid == 0){
						close(pipes_arr[0]);
						dup2(pipes_arr[1], 1);
						x = start;
						while (x < nargs){
							temp[y] = args[x];
							++y;
							++x;
						}
					//executes the next given command after piping
					execvp(args[start], temp);
					} else if(sid > 0){
						waitpid(sid, NULL, 0);
						start = number + 1;
						close(pipes_arr[1]);
						dup2(pipes_arr[0], 0);
					}
	 
				}
				number = number + 1;
			}
		
			x = start;
		
			while (x < nargs){
			  temp[y] = args[x];
			  ++x;
			  ++y;
			}
			temp[y] = NULL;
		
			//execute command after piping at the new starting position		
			execvp(args[start], temp);
			perror("exec failed");			//error check
			exit(-1);
	
	} else if(pid > 0) { // parent process
		if(!async){
			waitpid(pid, NULL, 0);
		} else{ 
			printf("This is an async call\n");
		}
	} else {	 //error stage
		perror("ERROR: fork failed");
		exit(1);
	}
}

/*
* method to redirect the given command
* takes in three parameters; char**, int, int
* does not return anything as it is a void method
*/
void changecmd (char **args, int nargs, int number)
{
	int input, output;
	char *file;
	FILE *temp;

	if (strcmp(args[number], ">") == 0) {
		file = args[number +1];
		args[number] = NULL;
		temp = fopen(file, "w");	//writes to file
		output = dup(1);
		close(1);
		dup(fileno(temp));
		fclose(temp);
	} else if (strcmp(args[number], "<") == 0) {
		file = args[number +1];
		args[number] = NULL;
		temp = fopen(file, "r");	//reads and outputs to standard output
		input = dup(0);
		close(0);
		dup(fileno(temp));
		fclose(temp);
	} else if (strcmp(args[number], ">>") == 0) {
		file = args[number +1];
		args[number] = NULL;
		temp = fopen(file, "a"); 		//appends
		output = dup(1);
		close(1);
		dup(fileno(temp));
		fclose(temp);
	}
	
	number ++;  //increment
 
}


int main (int argc, char* argv [])
{
	char cmdline[BUFSIZ];
	
	for(;;){
		printf("COP4338$ ");
		if(fgets(cmdline, BUFSIZ, stdin) == NULL) {
			perror("ERROR: fgets method failure");
			exit(1);
		}
		executecmd(cmdline) ;		//call to execute method
	}
	
	return 0;
}
