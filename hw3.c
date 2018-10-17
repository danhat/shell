/*
 * Danielle Hatten
 * Homework 3: Write Your Own Shell
 * CS361 F18
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

char *read_line() {
  char *line = NULL;
  ssize_t buffer_size = 0;
  getline(&line, &buffer_size, stdin);
  return line;
}

char **split_line(char *line) {
  int position = 0;
  int buffer_size = 64;
 
  char **arguments = malloc(buffer_size * sizeof(char *));

  char *argument = strtok(line, " ");
  while (argument) {
    arguments[position] = argument;
    position++;
    if (position >= buffer_size) {
      buffer_size += 64;
      arguments = realloc(arguments, buffer_size * sizeof(char *));
      if (!arguments) {
        fprintf(stderr, "allocation error/n");
        exit(-1);
      }
    }

    argument = strtok(NULL, " ");
  }
  arguments[position] = NULL;

  int i;
  for (i = 0; i < position; i++) 
    printf("arguments[%i]: %s\n", i, arguments[i]);

  return arguments;

}


int execute(char **arguments) {
  if (arguments[0] == NULL) {
    return 1;
  }

  if (strcmp(arguments[0], "help") == 0) {
    printf("\nDanielle Hatten's Shell\n");
  }

  if (strcmp(arguments[0], "exit") == 0) {
    free(arguments);
    exit(0);
  }

  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid < 0) {
    // fork failed
    free(arguments);
    exit(-1);
  }
  else if (pid == 0) { // child
    if (execv(arguments[0], arguments) == -1) {
      // print error
    }
  } 
  else { // parent
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  
  }
  printf("pid:%d status:%d\n", getpid(), status);

  return 1;


}


void shell() {
  char *line;
  char **arguments;  
 
  while(1) {
    printf("CS361 > ");
    line = read_line();
    
    arguments = split_line(line);
    execute(arguments);

    free(line);
    free(arguments);
  }

}

void sigint_handler(int sig) {
  char message[] = " caught sigint\nCS361 > ";
  write(1, message, sizeof(message));
  //shell();
}

void sigtstp_handler(int sig) {
  char message[] = " caught sigstp\nCS361 > ";
  write(1, message, sizeof(message));
  //shell();
}


int main() {
  // sigint sigtstp
  signal(SIGINT, sigint_handler);
  signal(SIGTSTP, sigtstp_handler);

  shell();  
  return 0;
 
}

