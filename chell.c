#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "chell.h"
#include <errno.h>
//COLORS
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"


static void sig_childactive(int sig) {
    if(sig == SIGINT /* || sig == SIGTSTP */ )
        return;
}


void prompt() {
    char *user = getlogin();
    char *currdir = (char *)malloc(256);
    getcwd(currdir, 256);
    printf("%s%s%s:%s%s%s %sCHELL%s$ ", KBLU, user, KNRM, KYEL, currdir, KNRM, KMAG, KNRM);
    free(currdir);
}



void chell(char *words) {
    chellFd(words, -1, -1, -1);
}


void chellFd(char *cmd, int infd, int outfd, int errfd) {
    char *words[128];
    int i = 0;
    while (cmd) {
        // printf("After: %s, ", after);
        words[i] = strsep(&cmd, " \n\t");
        if(!strlen(words[i])) {
            continue;  // doesn't increment, so "" gets ignored
        }
        if(i) {
            int redir = 1;
            if(!strcmp(words[i - 1], ">")) {    // XXX Error checking
                outfd = open(words[i], O_WRONLY | O_CREAT, 0644);
            }
            else if(!strcmp(words[i - 1], ">>")) {
                outfd = open(words[i], O_WRONLY | O_APPEND | O_CREAT, 0644);
            }
            else if(!strcmp(words[i - 1], "2>")) {
                errfd = open(words[i], O_WRONLY | O_CREAT, 0644);
            }
            else if(!strcmp(words[i - 1], "2>>")) {
                errfd = open(words[i], O_WRONLY | O_APPEND | O_CREAT, 0644);
            }
            else if(!strcmp(words[i - 1], "&>")) {
                outfd = errfd = open(words[i], O_WRONLY | O_CREAT, 0644);
            }
            else if(!strcmp(words[i - 1], "&>>")) {
                outfd = errfd = open(words[i], O_WRONLY | O_APPEND | O_CREAT, 0644);
            }
            else if(!strcmp(words[i - 1], "<")) {
                infd = open(words[i], O_RDONLY);
            }
            else if(!strcmp(words[i - 1], "|")) {
                // }
            }
            else {
                redir = 0;
            }
            if(redir) {
                 words[i - 1] = 0;
                 break;
            }
        }
        // printf("words[i]: %s\n", words[i]);
        i++;
    }
    words[i] = 0;
    command(words, infd, outfd, errfd);
}


int command(char **words, int infd, int outfd, int errfd) {
    if(!strcmp("cd", words[0])) {
        if(words[1] == 0) {
            chdir(getenv("HOME"));
        }
        else {
            chdir(words[1]);
        }
    } else if(!strcmp("exit", words[0])) {
        return 0;
    } else if(!fork()) {
        if(outfd != -1) dup2(outfd, STDOUT_FILENO);
        if(errfd != -1) dup2(errfd, STDERR_FILENO);
        if(infd  != -1) dup2(infd,  STDIN_FILENO );
        // int i = 0;
        // while(words[i] != 0) {
        //     printf("%s\n", words[i]);
        //     i++;
        // }
        if(execvp(words[0], words) == -1) {
            int n = errno;
            printf("%s", strerror(n));
        }

    } else {
        if(outfd != -1) close(outfd);
        int *i = (int *)malloc(sizeof(int));
        signal(SIGINT, sig_childactive);
        //signal(SIGTSTP, sig_childactive);
        wait(i);
        signal(SIGINT, SIG_DFL);
        //signal(SIGTSTP, SIG_DFL);
    }
    return 0;
}

int main() {
    //Clear Screen
    printf("\e[1;1H\e[2J");
    char *commandInit = (char *)malloc(sizeof(char)*256);
    char *command = (char *)malloc(sizeof(char)*256);

    while(1) {
        prompt(); //prints out prompt
        fgets(commandInit, 256, stdin);
        /* char *after; */
        /* char *words[128]; */
        /* char *temp[128]; */
        /* int i, j; */
        while(commandInit){
            command = strsep(&commandInit, ";");
            chell(command);
        } 
    }
}
