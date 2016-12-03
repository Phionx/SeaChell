# SeaChell
C Shell

#THINGS TO WORK ON


(DONE - NEEDS TESTING) * make non space special chars work (a|b<c>>d)
(DONE - NEEDS TESTING) * &
(DONE) * implement config file
(DONE - NEEDS TESTING) * do combinations work like a real shell (cmd < a > b | c > d <....)
(DONE - NEEDS TESTING) * make multiple redirects work (i.e. cmd < filein > fileout 2> fileerr)
(PROBABLY IMPOSSIBLE) * Tab completion
(PROBABLY IMPOSSIBLE) * up and down for recent commands run
(DONE) * |
(DONE) * > 
(DONE) * <
(DONE) * >>
(DONE) * cd
(DONE) * exit
(DONE) * replace home dir with ~
(DONE) * figure out tab situation

#IMPLEMENTS

 * |
 * >
 * <
 * >>
 * cd to home and path directories
 * exit
 * config file
 * 

#ATTEMPTED

 * &
 * make non space special chars work (a|b<c>>d)
 * make multiple redirects work (i.e. cmd < filein > fileout 2> fileerr)
 * do combinations work like a real shell (cmd < a > b | c > d <....)

#FILE AND FUNCTION HEADERS

chell.c: handles everything
	
	static void cp_pipe() : handles piping. 

	static void sig_childactive() : handles the signals.

	Input: int sig	

	void prompt() : handles the printing of the user, directory, and commands.

	void chell() : 
	
	Input: char *words

	void chellFd(char *cmd, int infd, int outfd, int errfd)
	
	int command(char **words, int infd, int outfd, int errfd, int shouldiwait)

	void execline(char *line, char *cmd)

	char *readall(char *path)

	char **splitread(char** full)

	char* linerepls(char *line)

	