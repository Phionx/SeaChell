#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <pwd.h>
#include "chell.h"

//COLORS
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

const char *copypipe[] = {"cp", "pipetemp", "pipe", 0};
char *home;

static void cp_pipe() {
    if(!fork()) {
        if(-1 == execvp("cp", copypipe)) printf("\nFailed cp_pipe\n");
        exit(0);  // making sure
    }        
    else {
        int _;
        wait(&_);
    }
}

static void sig_childactive(int sig) {
    if(sig == SIGINT /* || sig == SIGTSTP */ )
        printf("\n%s\n", strsignal(SIGINT)); 
}


void prompt() {
    char *user = getpwuid(getuid())->pw_name;
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
    int wait = 1;
    while (cmd) {
        // printf("After: %s, ", after);
        words[i] = strsep(&cmd, " \n\t");
        if(!strlen(words[i])) {
            continue;  // doesn't increment, so "" gets ignored
        }
        if(i) {
            int redir = 1;
            if(!strcmp(words[i - 1], ">")) {    // XXX Error checking
                fclose(fopen(words[i], "w"));  // clear file
                outfd = open(words[i], O_WRONLY | O_CREAT, 0644);
            }
            else if(!strcmp(words[i - 1], ">>")) {
                outfd = open(words[i], O_WRONLY | O_APPEND | O_CREAT, 0644);
            }
            else if(!strcmp(words[i - 1], "2>")) {
                fclose(fopen(words[i], "w"));  // clear file
                errfd = open(words[i], O_WRONLY | O_CREAT, 0644);
            }
            else if(!strcmp(words[i - 1], "2>>")) {
                errfd = open(words[i], O_WRONLY | O_APPEND | O_CREAT, 0644);
            }
            else if(!strcmp(words[i - 1], "&>")) {
                fclose(fopen(words[i], "w"));  // clear file
                outfd = errfd = open(words[i], O_WRONLY | O_CREAT, 0644);
            }
            else if(!strcmp(words[i - 1], "&>>")) {
                outfd = errfd = open(words[i], O_WRONLY | O_APPEND | O_CREAT, 0644);
            }
            else if(!strcmp(words[i - 1], "<")) {
                infd = open(words[i], O_RDONLY);
            }
            else if(!strcmp(words[i], "&")) {
                wait = 0;
                i++;  // later i -=2, net result i--
            }
            else if(!strcmp(words[i - 1], "|")) {
                fclose(fopen("pipetemp", "w"));  // clear file
                int fd = open("pipetemp", O_WRONLY, 0644);
                words[i - 1] = 0;
                command(words, infd, fd, errfd, 1);
                fclose(fopen("pipe", "w"));  // clear for next pipe
                cp_pipe();  // now pipe has correct output, pipetemp will be cleared soon
                infd = open("pipe", O_RDONLY, 0644);
                words[0] = words[i];
                i = 0;
                redir = 0;
	    }
            else {
                redir = 0;
            }
            if(redir) {
                i -= 2;
            }
        }
        // printf("words[i]: %s\n", words[i]);
        i++;
    }
    words[i] = 0;
    command(words, infd, outfd, errfd, wait);
}


    int command(char **words, int infd, int outfd, int errfd, int shouldiwait) {
    if(words[0] == 0) return 0;
    if(!strcmp("cd", words[0])) {
        if(words[1] == 0) {
            chdir(home);
        }
        else {
            chdir(words[1]);
        }
    } else if(!strcmp("exit", words[0])) {
      //signal(SIGTERM,sig_childactive);
      kill(getpid(),SIGTERM);
      return 0;
    } 
    else if(!fork()) {
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
            printf("%s\n", strerror(n));
            exit(n);
        }

    } else {
        if(outfd != -1) close(outfd);
        if(infd != -1) close(infd);
        if(errfd != -1) close(errfd);
        int *i = (int *)malloc(sizeof(int));
        signal(SIGINT, sig_childactive);
        //signal(SIGTSTP, sig_childactive);
        if(shouldiwait) wait(i);  // or should i go now
        signal(SIGINT, SIG_DFL);
        //signal(SIGTSTP, SIG_DFL);
    }
    return 0;
}

void execline(char *line, char *cmd) {
    char *linestart = line;
    while(line){
        cmd = strsep(&line, ";");
        if (cmd != 0) chell(cmd);
    }
    line = linestart;
}

char *readall(char *path){
    int fd = open(path, O_RDONLY);
    int size = 8;
    char *conf = malloc(8);
    char *save = conf;
    while(read(fd, conf, 8)) {
        size += 8;
        conf += 8;
        realloc(save, size);
    }
    conf = save;
    close(fd);
    return conf;
}

char **splitread(char* full) {
    char **lines = calloc(strlen(full), 8);  // guaranteed to fit everything
    int i = 0;
    while(full) {
        lines[i] = strsep(&full, "\n");
        i++;
    }
    return lines;
}

const char *ordspaced[] = {"2>>", "&>>", "&>", "2>", ">>", ">", "<", "|"};  //so we dont do 2>> -> "2 > > "

char* linerepls(char *line) {  // replaces certain things in a line. 
    /* current replacements:
       ~: getenv("HOME")
       > >> < 2> 2>> &> &>> |: add spaces around em
    */
    int size = strlen(line) + 1;
    int origsize = size;
    char *newl = malloc(size);
    int i;
    int j;
    int addNext;
    int origIdx = 0;
    int len;
    for(i = 0; origIdx < strlen(line); i++) {
        // printf("origIdx: %d, line[origIdx]: %c\n", origIdx, line[origIdx]);
        addNext = 1;
        for(j = 0; j < 8; j++) {  // iterate thru replacements
            char *tbr = ordspaced[j];
            len = strlen(tbr);
            if(strstr(line, tbr) == line + origIdx) {  // in first pos of line
                line = line + origIdx + len;
                addNext = 0;
                size += 2;
                realloc(newl, size);
                strcat(newl, " ");
                strcat(newl, tbr);
                strcat(newl, " ");
                origIdx = 0;
                i += -1 + len;
                break;
            }
        }
        if(strchr(line, '~') == line + origIdx) {
            line = line + origIdx + 1;
            addNext = 0;
            size += -1 + strlen(home);
            realloc(newl, size);
            strcat(newl, home);
            origIdx = 0;
        }   
        if(addNext) {
            newl[size - origsize + i] = line[origIdx];
            origIdx++;
        }
        // printf("%s\n", newl);
    }
    newl[size - 1] = 0;
    // printf("%s\n", newl);
    return newl;
}
            
                
int main() {
    home = getenv("HOME");  // gotta set it here
    // Clear Screen
    printf("\e[1;1H\e[2J");
    char *commandInit = malloc(256);
    char *command = malloc(256);
    // Load config
    close(open(".chellrc", O_RDONLY | O_CREAT, 0644));  // make sure config exists
    char *total = readall(".chellrc");
    char **lines = splitread(total);
    char *replaced;
    int i;
    for(i = 0; lines[i] != 0; i++) {
        printf("\n");  //  l m a o
        replaced = linerepls(lines[i]);
        execline(replaced, command);
        free(replaced);
    }
    free(lines);
    free(total);
    while(1) {
        prompt(); //prints out prompt
        if(!fgets(commandInit, 256, stdin)) continue;
        /* char *after; */
        /* char *words[128]; */
        /* char *temp[128]; */
        /* int i, j; */
        replaced = linerepls(commandInit);
        execline(replaced, command);
        free(replaced);
    }
}
