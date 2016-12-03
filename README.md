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
 * cd to home ``` cd home``` 
 * cd to path directories
 * exit
 * config file
 * &
 * make non space special chars work (a|b<c>>d)
 * make multiple redirects work (i.e. cmd < filein > fileout 2> fileerr)
 * do combinations work like a real shell (cmd < a > b | c > d <....)

#BUGS
 * permission errors: Not having write permissions and trying to write out will segfault, attempts to fix this using eaccess/access block permissions to multiple writable places

#FILE AND FUNCTION HEADERS

chell.c: handles everything
	
```
	/*======== static void cp_pipe() =========================================================
	Inputs: none
	Returns: nothing

	Description: forks for chell to run
	========================================================================================*/

	/*======== static void sig_childactive(int sig) ==========================================
	Inputs: int sig
	Returns: none

	Description: signal handler for SIGINT when a child is active
	========================================================================================*/
	
	/*======== static void sig_nochild(int sig) ==============================================
	Inputs: int sig
	Returns: none

	Description: signal handler for SIGINT and SIGTERM when the child is not active
	========================================================================================*/
	
	/*======== void exit_custom() ============================================================
	Inputs: none
	Returns: none

	Description:  exits chell
	========================================================================================*/
	
	/*======== void prompt() =================================================================
	Inputs: none
	Returns: none

	Description: displays chell prompt with user info and current path 
	========================================================================================*/

	/*======== void chell(char *words) =======================================================
	Inputs: char *words
	Returns: none

	Description: wrapper function for chellFd
	========================================================================================*/
	
	/*======== void chellFd(char *cmd, int infd, int outfd, int errfd) =======================
	Inputs: char *cmd, int infd, int outfd, int errfd
	Returns: none

	Description: takes cmd and input, output, and errror and parses the cmd and sends
	to  command(char **words, int infd, int outfd, int errfd, int shouldiwait)
	========================================================================================*/

	/*======== int command(char **words, int infd, int outfd, int errfd, int shouldiwait) ====
	Inputs: char *words
	Returns: none

	Description: runs words array of command with proper input, output, and error
	handeling
	========================================================================================*/
	
	/*======== void execline(char *line, char *cmd) ==========================================
	Inputs: char *line, char *cmd
	Returns: none

	Description: parses input by semicolons and runs the individual commands
	========================================================================================*/
	
	/*======== char *readall(char *path) =====================================================
	Inputs: char *path
	Returns: none

	Description: reads file
	========================================================================================*/
	
	/*======== char **splitread(char **full) =================================================
	Inputs: char **full
	Returns: none

	Description: splits full into lines
	========================================================================================*/
	
	/*======== char* linerepls(char *line) ===================================================
	Inputs: char *line
	Returns: none

	Description: replaces certain things in a line
	 current replacements:
       ~: getenv("HOME")
       > >> < 2> 2>> &> &>> |: add spaces around em
	========================================================================================*/
	
	/*======== int main() ====================================================================
	Inputs: char none
	Returns: none

	Description: main function that is run when the executable file is run
	========================================================================================*/



```

#Authors
[Shantanu Jha](https://github.com/Phionx/)
[Gabriel Marks](https://github.com/71619997a)
[Charles Zhang](https://github.com/charles-ah)
