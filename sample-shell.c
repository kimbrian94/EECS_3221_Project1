/**
 * Simple shell interface starter kit program.
 * Operating System Concepts
 * Mini Project1
 * Giseung Kim
 * 212981858
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#define MAX_LINE		80 /* 80 chars per line, per command */
char *args[MAX_LINE/2 + 1];	/* command line (of 80) has max of 40 arguments */
int has_history = 0;
static char * history[1];
static void execute(char * cmd_line){
    char * curr_cmd = malloc(sizeof(char) * MAX_LINE);
    strcpy(curr_cmd, cmd_line);
    int arg_num = 0;
    int is_parallel = 0;
    int fd1 = -1;
    int fd2 = -1;
    int status = 0;

    args[arg_num] = strtok(curr_cmd, " ");
    arg_num++;
    while(args[arg_num - 1] != NULL){
      args[arg_num] = strtok(NULL, " ");
      arg_num++;
    }
    arg_num--;

    int cmp;
    cmp = strcmp(args[arg_num - 1], "&");
    if(cmp == 0){
      is_parallel = 1;
      arg_num--;
      args[arg_num] = NULL;
    }

    if (arg_num >= 3) {
      if (0 == strcmp(args[arg_num - 2], "<")) {
        fd1 = open(args[arg_num - 1], O_RDONLY);
        args[arg_num - 2] = NULL;
      }else if (0 == strcmp(args[arg_num - 2], ">")) {
        fd2 = open(args[arg_num - 1], O_CREAT | O_WRONLY);
        args[arg_num - 2] = NULL;
      }
      arg_num = arg_num - 2;
    }

    pid_t pid = fork();
    if (pid == -1) {
      perror("fork unsuccessful");
    } else if (pid == 0) { //child
      if (fd1 >= 0) {
        dup2(fd1, STDIN_FILENO);
      }
      if (fd2 >= 0) {
        dup2(fd2, STDOUT_FILENO);
      }
      execvp(args[0], args);
    } else {
      close(fd1);
      close(fd2);
      if(is_parallel == 1){
        waitpid(pid, &status, 0);
      }
    }
    free(curr_cmd);
}

static void run_from_history(){
  if (has_history == 1) {
    execute(history[0]);
  } else if(has_history == 0){
    printf("No commands in history\n");
  }
}

int main(int argc, char *argv[])
{
    int should_run = 1;
    char * cmd_line = malloc(sizeof(char)*MAX_LINE);
    size_t cmd_size = MAX_LINE;

    while(should_run){

      printf("mysh:~$ ");
      getline(&cmd_line, &cmd_size, stdin);

      cmd_line[strlen(cmd_line) - 1] = '\0';

      if ((strcmp(cmd_line, "!!")) == 0) {
        run_from_history();
      }else if ((strcmp(cmd_line, "exit")) == 0) {
        should_run = 0;
      } else {
        history[0] = strdup(cmd_line);
        has_history = 1;
        execute(cmd_line);
      }
    }
return 0;
}
