#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

static void sig_childactive(int sig) {
  if(sig == SIGINT || sig == SIGTSTP)
    return;
}
int main() {
  while(1) {
    char *user = getlogin();
    char currdir[256];
    getcwd(currdir, sizeof(currdir));
    //getlogin_r(user); 
    printf("%s:%s chell$ ",user,currdir);
    char command[256];
    fgets(command, 256, stdin);
    char *after = command;
    char *words[128];
	char *temp[128];
    int i, j = 0;
    while (i < 128) {
      temp[i] = 0;
	  words[i] = 0;
      i++;
    }
    
	i = 0;
    while (after) {
      //    printf("After: %s, ", after);
      temp[i] = strsep(&after, " \n\t");
      //    printf("words[i]: %s\n", words[i]);
      i++;
    }

	i=0;

	while(temp[i]){
		if(strcmp(temp[i], "") != 0){
			words[j] = temp[i];
			i++;
			j++;
		} else {
			i++;
		}
	}
		
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
    else if(!fork()) 
      return execvp(words[0], words);
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
