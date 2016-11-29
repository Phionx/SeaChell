static void sig_childactive(int sig);
void prompt();
void chell(char *words);
void chellFd(char *cmd, int infd, int outfd, int errfd);
int command(char **words, int infd, int outfd, int errfd, int shouldiwait);
int main();

