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

char *get_line() {
  char *line = NULL;
  ssize_t buffer_size = 0;
  getline(&line, &buffer_size, stdin);
  return line;
}

char **get_arguments(char *command) {
  int position = 0;
  int buffer_size = 32;
 
  char **arguments = (char **)malloc(buffer_size * sizeof(char *));

  char *argument = strtok(command, " ");
  while (argument != NULL) {
    arguments[position] = argument;
    position++;
    if (position >= buffer_size) {
      buffer_size += 32;
      arguments = realloc(arguments, buffer_size * sizeof(char *));
    }
    // get next argument
    argument = strtok(NULL, " ");
  }
  arguments[position] = (char *)0;

  /*int i;
  for (i = 0; i < position; i++) 
    printf("arguments[%i]: %s\n", i, arguments[i]);*/

  return arguments;

}

int num_of_args(char **line) {
  int count = 0;
  char *arg = strtok(line, " ");
  while(arg != NULL) {
    count++;
    arg = strtok(NULL, " ");
  }
  
  return count;
}


int execute(char **arguments, num_of_args) {
  if (arguments[0] == NULL) {
    return 1;
  }

  if (strcmp(arguments[0], "help") == 0) {
    printf("\nDanielle Hatten's Shell\n");
  }

  if (strcmp(arguments[0], "exit") == 0) {
    free(arguments);
    //exit(0);
    return -1;
  }

  pid_t pid, wpid;
  int status, i;

  pid = fork();
  if (pid == 0) { // child 
    // check for redirection
    for (i = 1; i < num_of_args - 1; i++) {
      char *file = arguments[i + 1];
      if (strcmp(arguments[i], ">") == 0) {
        freopen(file, "w", stdout);
        break;
      }
      else if (strcmp(arguments[i], "<") == 0) {
        freopen(file, "r", stdin);
        break;
      }
    }
    execv(arguments[0], arguments);
    //printf("pid:%d status:%d\n", getpid(), status);
    //exit(1);


  } 
  else { // parent
    wpid = waitpid(pid, &status, WUNTRACED);
    printf("pid:%d status:%d\n", getpid(), status);
  }
  //printf("pid:%d status:%d\n", getpid(), status);

  return 1;


}


void shell() {
  char *line = NULL;
  char *command;
  char **arguments;   
  int status = 1;

  while(status) {
    printf("CS361 > ");
    line = get_line();
    printf("line: %s\n", line); 
    // check if there is more than one command
    if (strchr(line, ';')) {
      // separate commands
      char *command = strtok(line, ";");
      
      // print statement for debugging
      // printf("command: %s\n", command);
      while(command != NULL) {
        // get arguments for command and then execute command
        arguments = get_arguments(command);
        status = execute(arguments);
        // get next command
        command = strtok(NULL, ";");
      }
    }
    // else, there is one command to execute
    else {
      arguments = get_arguments(line);
      status = execute(arguments);
    }
 
  }
  
  free(arguments);

}

void sigint_handler(int sig) {
  char message[] = " caught sigint\nCS361 > ";
  write(1, message, sizeof(message));
}

void sigtstp_handler(int sig) {
  char message[] = " caught sigstp\nCS361 > ";
  write(1, message, sizeof(message));
}


int main() {
  // sigint sigtstp
  signal(SIGINT, sigint_handler);
  signal(SIGTSTP, sigtstp_handler);

  shell();  
  return 1;
 
}

