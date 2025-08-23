#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_LEN_IN 2048
#define MAX_ARG 100 
#define MAX_ARGS 256 

int getArgs(char* argv[]);
void SIGINTHandler(int signal);

volatile sig_atomic_t pid;
volatile sig_atomic_t request_termination = 0;

int main(int argc, char *argv[]){
  signal(2, SIGINTHandler);

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

    pid = fork();
    if(pid == -1){
      perror("main-Error creating fork");
      continue;
    }

    if(pid == 0){
      if(execvp(args[0], args) == -1){
        perror("main-execvp failed");
        exit(EXIT_FAILURE);
      }
    } else {
      int status;
      waitpid(pid, &status, 0);
      pid = 0;
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
    perror("getArgs-Error reading the input\n");
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

void SIGINTHandler(int signal){

  const char msg1[] = "Signal encountered\n";
  write(STDOUT_FILENO, msg1, sizeof(msg1) - 1);

  if(pid != 0){
    char msg2[256];
    snprintf(msg2, sizeof(msg2), "found child process with pid %d, killing it\n", pid);
    write(STDOUT_FILENO, msg2, sizeof(msg2) - 1);
    kill(pid, SIGTERM);
    pid = 0;
    return;
  }
  const char msg3[] = "There is no child process, asking main process to terminate\n";
  write(STDOUT_FILENO, msg3, sizeof(msg3) - 1);
  request_termination = 1;
  kill(getpid(), SIGTERM);
}
