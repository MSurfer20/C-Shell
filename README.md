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