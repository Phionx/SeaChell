#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>


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
    if(sig == SIGINT || sig == SIGTSTP)
        return;
}
int main() {
    //Clear Screen
    printf("\e[1;1H\e[2J");
    while(1) {
        
        //Print shell input
        char *user = getlogin();
        char currdir[256];
        getcwd(currdir, sizeof(currdir));
        //getlogin_r(user); 
        printf("%s%s%s:%s%s%s %sCHELL%s$ ", KBLU, user, KNRM, KYEL, currdir, KNRM, KMAG, KNRM);

        char *commandInit = (char *)malloc(sizeof(char)*256);
        fgets(commandInit, 256, stdin);
        char *command =    (char *)malloc(sizeof(char)*256);
        char *after;
        char *words[128];
        char *temp[128];
        
        int i, j;
    
        while(commandInit){
            command = strsep(&commandInit, ";");
            if(strcmp(command, "") != 0){
                after = command;
                i = 0;
                j = 0;
                while (i < 128) {
                    temp[i] = 0;
                    words[i] = 0;
                    i++;
                }
                
                i = 0;
                int outfd = -1;
                int errfd = -1;
                int infd = -1;
                while (after) {
                    // printf("After: %s, ", after);
                    temp[i] = strsep(&after, " \n\t");
                    if(i) {
                        int redir = 1;
                        if(!strcmp(temp[i - 1], ">")) {    // XXX Error checking
                            outfd = open(temp[i], O_WRONLY | O_CREAT, 0644);
                        }
                        else if(!strcmp(temp[i - 1], ">>")) {
                            outfd = open(temp[i], O_WRONLY | O_APPEND | O_CREAT, 0644);
                        }
                        else if(!strcmp(temp[i - 1], "2>")) {
                            errfd = open(temp[i], O_WRONLY | O_CREAT, 0644);
                        }
                        else if(!strcmp(temp[i - 1], "2>>")) {
                            errfd = open(temp[i], O_WRONLY | O_APPEND | O_CREAT, 0644);
                        }
                        else if(!strcmp(temp[i - 1], "&>")) {
                            outfd = errfd = open(temp[i], O_WRONLY | O_CREAT, 0644);
                        }
                        else if(!strcmp(temp[i - 1], "&>>")) {
                            outfd = errfd = open(temp[i], O_WRONLY | O_APPEND | O_CREAT, 0644);
                        }
			else if(!strcmp(temp[i - 1], "<")) {
                            infd = open(temp[i], O_RDONLY);
                        }
                        else {
                            redir = 0;
                        }
                        if(redir) {
                            temp[i - 1] = 0;
                            break;
                        }
                    }
                    // printf("words[i]: %s\n", words[i]);
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
            
                /*
                i = 0;
                printf("\nTEST\n:");
                while (words[i]){
                    printf("words[%d]: %s\n", i, words[i]);
                    i++;
                }
                */

                if(!strcmp("cd", words[0])) {
                    if(words[1] == 0) {
                        chdir("~");
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
                    return execvp(words[0], words);
                } else {
                    if(outfd != -1) close(outfd);
                    int *i = (int *)malloc(sizeof(int));
                    signal(SIGINT, sig_childactive);
                    signal(SIGTSTP, sig_childactive);
                    wait(i);
                    signal(SIGINT, SIG_DFL);
                    signal(SIGTSTP, SIG_DFL);
                }
            } 
        }
    }
}
