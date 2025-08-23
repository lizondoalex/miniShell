#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LEN_IN 2048
#define MAX_ARG 100 
#define MAX_ARGS 256 

int getArgs(char* argv[]);

int main(int argc, char *argv[]){

  while(1){

    printf("miniShell-> ");
    fflush(stdout);

    char* args[MAX_ARGS];
    if(getArgs(args) == -1){
      return -1;
    }

    if(strcmp(args[0], "exit") == 0){
      break;
    }

    pid_t pid = fork();
    if(pid == -1){
      perror("Error creating fork");
      continue;
    }

    if(pid == 0){
      if(execvp(args[0], args) == -1){
        perror("execvp failed");
        exit(EXIT_FAILURE);
      }
    } else {
      int status;
      waitpid(pid, &status, 0);
    }

  }
  return 0;
}

// Function that reads commands from standart in and outputs them as 
// argv[0] == "arg0", argv[i] = "argi"

int getArgs(char* argv[]){
  char buffer[MAX_LEN_IN];
  char *word[MAX_ARG];
  int argc = 0;

  if(fgets(buffer, sizeof(buffer), stdin) == NULL){
    perror("Error reading the input\n");
    return -1;
  }

  buffer[strcspn(buffer, "\n")] = '\0';

  char *token = strtok(buffer,  " ");
  while(token != NULL && argc < MAX_ARGS) {
    argv[argc++] = token;
    token = strtok(NULL, " ");
  }
  argv[argc] = NULL;

  return argc;
}
