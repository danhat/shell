void shell() {
  char *line;
  char **commands;  
  int status;
  int i;
  while(1) {
    printf("CS361 > ");
    line = read_line();
    
    commands = split_line_into_commands(line);
    status = execute(commands);

    free(line);
    free(commands);
  }

}

void sigint_handler(int sig) {
  char message[] = " caught sigint\nCS361 > ";
  write(1, message, sizeof(message));
  shell();
}

void sigtstp_handler(int sig) {
  char message[] = " caught sigstp\nCS361 > ";
  write(1, message, sizeof(message));
  shell();
}


int main() {
  // sigint sigtstp
  signal(SIGINT, sigint_handler);
  signal(SIGTSTP, sigtstp_handler);

  shell();  
  
}
