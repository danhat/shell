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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DELIM " \t\n\a"

char *get_line() {
  char *line = NULL;
  ssize_t buffer_size = 0;
  getline(&line, &buffer_size, stdin);
  return line;
}


char **get_arguments(char *command) {
  int position = 0;
  int buffer_size = 32;
  const char space[2] = " ";
 
  char **arguments = malloc(buffer_size * sizeof(char *));

  char *argument = strtok(command, DELIM);
  while (argument != NULL) {
    arguments[position] = argument;
    position++;
    if (position >= buffer_size) {
      buffer_size += 32;
      arguments = realloc(arguments, buffer_size * sizeof(char *));
    }
    // get next argument
    argument = strtok(NULL, DELIM);
  }
  position++;
  arguments[position] = (char *)0;

  int i;
  // print statement for debugging
  /*for (i = 0; i < position; i++) 
    printf("arguments[%i]: %s\n", i, arguments[i]);
  */
  return arguments;

}

int num_of_args(char *line) {
  int count = 0;
  char *arg = strtok(line, " ");
  while(arg != NULL) {
    count++;
    arg = strtok(NULL, " ");
  }
  
  return count;
}


int execute(char **arguments, int num_of_args) {
  if (arguments[0] == NULL) {
    return 1;
  }

  if (strcmp(arguments[0], "help") == 0) {
    printf("\n*** Danielle Hatten's Shell ***\n");
    return 1;
  }

  if (strcmp(arguments[0], "exit") == 0) {
    return -1;
  }

  pid_t pid, wpid;
  int status;
  int i;

  // intput fd, output fd for redirection
  int infile, outfile;

  // variables to keep track of files that are opened
  int in_open = 0;
  int out_open = 0;

  char *buf = malloc(200);
  int size = 200;

  pid = fork();
  if (pid == 0) { // child 
    // check for redirection
    for (i = 1; i < num_of_args - 1; i++) {
      if (strcmp(arguments[i], ">") == 0) {
        // open output file descriptor
        outfile = open(arguments[i+1], O_RDWR|O_CREAT);
        // replace output with output file
        dup2(outfile, 1);
        close(outfile);
        //write(outfile, buf, size);
        out_open = 1;
      }
      else if (strcmp(arguments[i], "<") == 0) {
        // open input file descripter
        infile = open(arguments[i+1], O_RDONLY);
        // replace stdin with input file
        dup2(infile, 0);
        close(infile);
        //read(infile, buf, size);
        in_open = 1;
      }
    }
    if (execv(arguments[0], arguments) < 0) {
      char msg[] = "could not execute command\nCS361 > ";
      write(1, msg, sizeof(msg));
    }
    // if output or input file descriptors are open, close them
    /*if (in_open == 1) 
      close(infile);
    if (out_open == 1)
      close(outfile);*/

    exit(0);
 
  } 
  else { // parent
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    printf("pid:%d status:%d\n", wpid, status);
  }
  

  return 1;


}



void shell() {
  char *line = NULL;
  char *command;
  char **arguments;   
  int status = 1;
  int n; 

  while(status >= 0) {
    printf("CS361 > ");
    line = get_line();
    const char sc = ';';
    
    // check if there is more than one command
    if (strchr(line, sc) != NULL) {
      // separator found and there is more than one command
      //printf("*** separator found ***\n");
      // separate commands
      char *command = strtok(line, ";");
      
      while(command) {
        //printf("*** command: %s ***\n", command);
        // get arguments for command and then execute command
        arguments = get_arguments(command);
        n = num_of_args(command);
        status = execute(arguments, n);
        free(arguments);
        // get next command
        command = strtok(NULL, ";\n\t\v\f\r\0");
      }
     
    }

    // else, there is only one command to execute
    else {
      //printf("*** only one command ***\n");
      arguments = get_arguments(line);
      n = num_of_args(line);
      status = execute(arguments, n);
    }
 
  }
  
  free(arguments);

}

void sigint_handler(int sig) {
  char message[] = "\ncaught sigint\nCS361 > ";
  write(1, message, sizeof(message));
}

void sigtstp_handler(int sig) {
  char message[] = "\ncaught sigstp\nCS361 > ";
  write(1, message, sizeof(message));
}


int main() {
  // sigint sigtstp
  signal(SIGINT, sigint_handler);
  signal(SIGTSTP, sigtstp_handler);

  shell();  
  return 0;
 
}

