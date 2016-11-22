#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>


static void sig_childactive(int sig) {
  if(sig == SIGINT || sig == SIGTSTP)
    return;
}
int main() {
  while(1) {
    printf("? ");
    char command[256];
    fgets(command, 256, stdin);
    char *after = command;
    char *words[128];
    int i = 0;
    while (i < 128) {
      words[i] = 0;
      i++;
    }
    i = 0;
    int outfd = -1;
    while (after) {
      //    printf("After: %s, ", after);
      words[i] = strsep(&after, " \n\t");
      if(!strcmp(words[i - 1], ">")) {  // XXX Error checking
	outfd = open(words[i], O_WRONLY | O_CREAT);
	break;
      }
      else if(!strcmp(words[i - 1], ">>")) {
	outfd = open(words[i], O_WRONLY | O_APPEND | O_CREAT);
	break;
      }
      //    printf("words[i]: %s\n", words[i]);
      i++;
    }
    words[i - 1] = 0;  // removing empty string at the end
    if(!strcmp("cd", words[0])) {
      if(words[1] == 0) {
	chdir("~");
      }
      else {
	chdir(words[1]);
       }
    }
    else if(!strcmp("exit", words[0])) {
      return 0;
    }
    else if(!fork()) {
      if (outfd != -1) {
	dup2(outfd, STDOUT_FILENO);
      }
      return execvp(words[0], words);
    }
    else {
      
      int *i = (int *)malloc(sizeof(int));
      signal(SIGINT, sig_childactive);
      //signal(SIGTSTP, sig_childactive);
      wait(i);
      signal(SIGINT, SIG_DFL);
      //signal(SIGTSTP, SIG_DFL);
    }
  }
}
