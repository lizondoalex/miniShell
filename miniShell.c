#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_LEN_IN 2048
#define MAX_ARG 100 
#define MAX_ARGS 256 

int getArgs(char* argv[]);

void* thread_exec(void* arg){

  char** argvector= (char**) arg;
  printf("created new thread with pid %d\n", getpid());

  execvp(argvector[0], argvector);

  return NULL;
}

int main(int argc, char *argv[])
{
  pthread_t thread;
  int t_result;

  char* args[MAX_ARGS];
  if(getArgs(args) == -1){
    return -1;
  }


  char* message = "hello world";
  t_result = pthread_create(&thread, NULL, thread_exec, (void*) args);

  pthread_join(thread, NULL);

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
