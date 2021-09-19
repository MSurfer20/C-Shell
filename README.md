### svsh -- Suyash Vardhan Shell
#### Usage
* Run `make`
* Run `./svsh.out`
#### Files
* cd.c - Contains the functionality of the cd command.
* cd.h - Defines the functions needed in `cd.c`.
* echo.c - Contains the functionality of the echo command.
* echo.h - Defines the functions needed in `echo.c`.
* history.c - Contains the functionality of the history and related functionalities. It contains the functions which can get the nth command from the history(required for arrow key task), display n commands from the history as well as a function that writes new commands into the history.
* history.h - Defines the functions needed in `history.c`.
* headers.h - Contains various `#include` statements and global structs that were required for the functioning of the code.
* ls.c - Contains the functionality of the ls command. It contains a function to print the output of each file when ls -l output is given.
* ls.h - Defines the functions needed in `ls.c`.
* main.c - Main file of the shell, which contains code that tokenizes commands, parses their arguments and calls functions to execute them.
* pinfo.c - Contains the functionality of getting a process information.
* pinfo.h - Defines the functions needed in `pinfo.c`.
* process.c - Contains code that creates and executes both foreground and background processes, as well as code that is run when a child process terminates.
* process.h - Defines the functions needed in `process.c`.
* prompt.c - Contains code that prints the prompt.
* prompt.h - Contains code that defines functions of `prompt.c`
* pwd.c - Contains code for pwd functionality
* pwd.h - Contains code that defines functions of `pwd.c`

#### Functionalities implemented
* cd: The `cd` command is supported with various flags like `.`, `..`, `-`, `~`. In addition, relative paths from `~`, `..` and `.` are handled too. `chdir` is used to change the directory.
* echo: The `echo` command is implemented with the support of reducing tabs and spaces into a single space. This was achieved by tokenizing the args(already done in common for all args).
* history: The history command prints all the history available(with a cap of 10). The history file is created in the actual home of the machine(found using HOME env variable). The history is always read and written to the file, and never stored in the program to be returned in order to support multi-session history. Also, it supports only 20 commands, after which oldest one gets removed.
* ls: The ls command supports the flags -a and -l. It also considers `-aa` or `-lll` or `-alla` as flags too, but not if any other character appears after `-`, like `-lan`. The -l output also has `total` parameter, for which I iterated twice over the directory files. It also supports the functionality that files older than 6 months show year instead of Hours and Minutes. For this, if the difference b/w months number is >6 or if it is ==6 and the date is more than the creation date, then it is considered older than 6 months.
* pinfo: Pinfo is supported for both the current shell, as well as with pid mentioned. the addition of `+` for foreground processes has support across various sessions and various processes, since it is implemented using pgid associations of foreground processes.
* Process: It has support for both foreground and background processes, with the proper steps(setting correct pgroups) for foreground process as well. It also has support for running processes with paths and args specified relative to `~` as well. Memory is properly handled too to ensure smooth processing. When the bg process terminate, they send the  SIGCHLD signal, which is then used to print the info about the exited process.
* prompt: The prompt with the username, and the systemname(hostname) is shown along with the pwd. A function checks if the home directory is a equal to, or is a substring(followed by a `/`), such that the whole portion is replaced by `~`.
* Pwd: Pwd is printed from the root(`/`) of the machine. It doesn't accept any arguments, and throws error when the same are given.
* Tokenized commands-The commands are also tokenized on `;`, which is given highest priority in command processing.