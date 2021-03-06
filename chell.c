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
#include <limits.h>

//COLORS
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

//Global Vars =============================================================================

char *copypipe[4];
char *home;
char *pipepath;
char *ptemppath;
const char *ordspaced[] = {"2>>", "&>>", "&>", "2>", ">>", ">", "<", "|", "&"};  //so we dont do 2>> -> "2 > > "



/*======== static void cp_pipe() =========================================================
	Inputs: none
	Returns: nothing

	Description: forks for chell to run
========================================================================================*/
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


/*======== void exit_custom() ============================================================
	Inputs: none
	Returns: none

	Description: exits chell
========================================================================================*/
void exit_custom() {
    remove(pipepath);
    remove(ptemppath);
    free(pipepath);
    free(ptemppath);
    exit(0);
}


/*======== static void sig_childactive(int sig) ==========================================
	Inputs: int sig
	Returns: none

	Description: signal handler for SIGINT when a child is active
========================================================================================*/
static void sig_childactive(int sig) {
    if(sig == SIGINT /* || sig == SIGTSTP */ )
        printf("\n%s\n", strsignal(SIGINT)); 
}


/*======== static void sig_nochild(int sig) ==============================================
	Inputs: int sig
	Returns: none

	Description: signal handler for SIGINT and SIGTERM when the child is not active
========================================================================================*/
static void sig_nochild(int sig) {
    if(sig == SIGINT || sig == SIGTERM) {
	exit_custom();
    }
}


/*======== void prompt() =================================================================
	Inputs: none
	Returns: none

	Description: displays chell prompt with user info and current path 
========================================================================================*/
void prompt() {
    char *user = getpwuid(getuid())->pw_name;
    char *currdir = (char *)malloc(PATH_MAX + 1);
    getcwd(currdir, 256);
    printf("%s%s%s:%s%s%s %sCHELL%s$ ", KBLU, user, KNRM, KYEL, currdir, KNRM, KMAG, KNRM);
    free(currdir);
}


/*======== void chell(char *words) =======================================================
	Inputs: char *words
	Returns: none

	Description: wrapper function for chellFd
========================================================================================*/
void chell(char *words) {
    chellFd(words, -1, -1, -1);
}


/*======== void chellFd(char *cmd, int infd, int outfd, int errfd) =======================
	Inputs: char *cmd, int infd, int outfd, int errfd
	Returns: none

	Description: takes cmd and input, output, and errror and parses the cmd and sends
	to  command(char **words, int infd, int outfd, int errfd, int shouldiwait)
========================================================================================*/
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
                fclose(fopen(ptemppath, "w"));  // clear file
                int fd = open(ptemppath, O_WRONLY, 0644);
                words[i - 1] = 0;
                command(words, infd, fd, errfd, 1);
                fclose(fopen(pipepath, "w"));  // clear for next pipe
                cp_pipe();  // now pipe has correct output, pipetemp will be cleared soon
                infd = open(pipepath, O_RDONLY, 0644);
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


/*======== int command(char **words, int infd, int outfd, int errfd, int shouldiwait) ====
	Inputs: char *words
	Returns: none

	Description: runs words array of command with proper input, output, and error
	handeling
========================================================================================*/
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
	exit_custom();
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
        free(i);
        signal(SIGINT, sig_nochild);
        //signal(SIGTSTP, SIG_DFL);
    }
    return 0;
}


/*======== void execline(char *line, char *cmd) ==========================================
	Inputs: char *line, char *cmd
	Returns: none

	Description: parses input by semicolons and runs the individual commands
========================================================================================*/
void execline(char *line, char *cmd) {
    char *linestart = line;
    while(line){
        cmd = strsep(&line, ";");
        if (cmd != 0) chell(cmd);
    }
    line = linestart;
}


/*======== char *readall(char *path) =====================================================
	Inputs: char *path
	Returns: none

	Description: reads file
========================================================================================*/
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


/*======== char **splitread(char **full) =================================================
	Inputs: char **full
	Returns: none

	Description: splits full into lines
========================================================================================*/
char **splitread(char **full) {
    char **lines = calloc(strlen(*full), 8);  // guaranteed to fit everything
    int i = 0;
    while(full && *full) {
        lines[i] = strsep(full, "\n");
        i++;
    }
    return lines;
}


/*======== char* linerepls(char *line) ===================================================
	Inputs: char *line
	Returns: none

	Description: replaces certain things in a line
	 current replacements:
       ~: getenv("HOME")
       > >> < 2> 2>> &> &>> |: add spaces around em
========================================================================================*/
char* linerepls(char *line) {  
    int size = strlen(line) + 1;
    int origsize = size;
    char *newl = calloc(size, 1);
    int i;
    int j;
    int addNext;
    int origIdx = 0;
    int len;
    for(i = 0; origIdx < strlen(line); i++) {
        // printf("origIdx: %d, line[origIdx]: %c\n", origIdx, line[origIdx]);
        addNext = 1;
        for(j = 0; j < 9; j++) {  // iterate thru replacements
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
    //printf("%s\n", newl);
    return newl;
}


/*======== int main() ====================================================================
	Inputs: char none
	Returns: none

	Description: main function that is run when the executable file is run
========================================================================================*/
int main() {
    signal(SIGINT, sig_nochild);
    signal(SIGTERM, sig_nochild);
    home = getenv("HOME");  // gotta set it here
    char startdir[PATH_MAX + 1];
    getcwd(startdir, PATH_MAX + 1);
    pipepath = malloc(PATH_MAX + 15);
    ptemppath = malloc(PATH_MAX + 19);
    strcpy(pipepath, startdir);
    strcat(pipepath, "/pipes/pipe");
    strcpy(ptemppath, startdir);
    strcat(ptemppath, "/pipes/pipetemp");
    copypipe[0] = "cd";
    copypipe[1] = ptemppath;
    copypipe[2] = pipepath;
    copypipe[3] = 0;
    // Clear Screen
    printf("\e[1;1H\e[2J");
    char *commandInit = malloc(256);
    char *command = malloc(256);
    // Load config
    close(open(".chellrc", O_RDONLY | O_CREAT, 0644));  // make sure config exists
    char *save, *total;
    save = total = readall(".chellrc");
    char **lines = splitread(&total);
    char *replaced;
    int i;
    for(i = 0; lines[i] != 0; i++) {
        printf("\n");  //  l m a o
        replaced = linerepls(lines[i]);
        execline(replaced, command);
        free(replaced);
    }
    free(lines);
    free(save);
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
