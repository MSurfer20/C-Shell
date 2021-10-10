# svsh -- Suyash Vardhan Shell
**Name:** Suyash Vardhan Mathur

**Roll no.:** 2019114006

## Prerequisites

You must have the following installed:
* `gcc 9.3.0`
* `make`
## Usage
* Run `make`
* Run `./svsh.out`
## Files
* baywatch.c - Contains the functionality of the baywatch command. Includes all the commands that can be executed using baywatch, such as dirty, newborn and interrupt.
* baywatch.h - Contains the functions needed in `baywatch.c`
* bg.c - Contains the functionality of the bg command
* bg.h - Contains the functions needed in `bg.c`
* cd.c - Contains the functionality of the cd command.
* cd.h - Defines the functions needed in `cd.c`.
* echo.c - Contains the functionality of the echo command.
* echo.h - Defines the functions needed in `echo.c`.
* fg.c - Contains the functionality of the fg command
* fg.h - Contains the functions needed in `fg.c`
* headers.h - Contains various `#include` statements and global structs that were required for the functioning of the code.
* history.c - Contains the functionality of the history and related functionalities. It contains the functions which can get the nth command from the history(required for arrow key task), display n commands from the history as well as a function that writes new commands into the history.
* history.h - Defines the functions needed in `history.c`.
* jobs.c - Contains the functionality of the jobs command.
* jobs.h - Contains the functions needed in `jobs.c`
* ls.c - Contains the functionality of the ls command. It contains a function to print the output of each file when ls -l output is given.
* ls.h - Defines the functions needed in `ls.c`.
* main.c - Main file of the shell, which contains code that tokenizes commands, parses their arguments and calls functions to execute them. This also includes the parsing of piping and redirection, and contains the functions that implement piping and redirection after the parsing. It also contains code that executes the repeat and replay commands, since they directly call the `execute_command` function present in the file to execute the functionality.
* makefile - The makefile for the assignment. In order to make, simply run `make` while in the Assignment Directory.
* pinfo.c - Contains the functionality of getting a process information.
* pinfo.h - Defines the functions needed in `pinfo.c`.
* process.c - Contains code that creates and executes both foreground and background processes, as well as code that is run when a child process terminates.
* process.h - Defines the functions needed in `process.c`.
* prompt.c - Contains code that prints the prompt.
* prompt.h - Contains code that defines functions of `prompt.c`
* pwd.c - Contains code for pwd functionality
* pwd.h - Contains code that defines functions of `pwd.c`
* sig.c - Contains code for sig command functionality.
* sig.h - Contains function required in `sig.c`
* svsh.out - The executable of the shell, that gets created when you `make` in the main directory.

## Functionalities implemented
### Command Parsing
* **Tokenized commands:** The commands are also tokenized on `;`, which is given highest priority in command processing.
* **Arguments tokenized:** The arguments are tokenized based upon both `\t` and ` `, and so, arguments can be separated by any number of tabs and spaces, and get handled by the shell.
* **Redirection:** The tokens `>`, `<` and `>>` can be used for redirection of command ouputs into files, appending output into files, and for redirecting inputs from files respectively. **Note**: These MUST be space separated from the other arguments. **For eg** `cat < a > b`.
* **Piping:** The token `|` can be used for piping output of one command into another command. **Note:** This doesn't work with background processes. Also, note that the pipes and redirection wait for each command to finish execution, and thus, are serially executed. **For example**, `cat a | grep txt`
* **prompt:** The prompt with the username, and the systemname(hostname) is shown along with the pwd. A function checks if the home directory is a equal to, or is a substring(followed by a `/`), such that the whole portion is replaced by `~`.

### Builtin Commands
* **cd:** This command allows to change the current directory. The `cd` command is supported with various flags like `.`, `..`, `-`, `~`. In addition, relative paths from `~`, `..` and `.` are handled too. `chdir` is used to change the directory. **Example command** - `cd ~/dir1`
* **echo:** This command prints the arguments passed on the screen. The `echo` command is implemented with the support of reducing tabs and spaces into a single space. This was achieved by tokenizing the args(already done in common for all args). **Example command** - `echo "lol"`
* **history:** 
    * The history command prints all the history available(with a cap of 10).
    * The history file is created in the actual home of the machine(found using `HOME` env variable).
    * The history is always read and written to the file, and never stored in the program to be returned in order to support multi-session history.
    * Also, it supports only 20 commands, after which oldest one gets removed.
    * The commmand also supports passing an integer argument, and then shows that many commands from the history.
    * **Example command** - `history`, `history 4`
* **ls:**
    * This command is same as the bash `ls` command.
    * The ls command supports the flags -a and -l. It also considers `-aa` or `-lll` or `-alla` as flags too, but not if any other character appears after `-`, like `-lan`. 
    * The -l output also has `total` parameter, for which I iterated twice over the directory files.
    * It also supports the functionality that files older than 6 months show year instead of Hours and Minutes. For this, if the difference b/w months number is >6 or if it is ==6 and the date is more than the creation date, then it is considered older than 6 months.
    * **Example command** - `ls -al file dir`
* **pwd:** This prints the present working directory of the shell. Pwd is printed from the root(`/`) of the machine. It doesn't accept any arguments, and throws error when the same are given. **Example command** - `pwd`
* **exit:** I have turned off the interrupt signals for the shell process, and in order to exit the shell, you must use the `exit` command. **Example command** - `exit`
* **Up arrow key:** Pressing the up arrow key loops over the history command, and shows the last command on the prompt. It works only until 20(or the last command stored), after which the command doesn't change. Note that the command can be changed after it shows up on the prompt.

### Process and Job Management
* **Process:**
    1. It has support for both foreground and background processes, with the proper steps(setting correct pgroups) for foreground process as well.
    1. Use & at the end of the command to run it in background.
    1. It also has support for running processes with paths and args specified relative to `~` as well. Memory is properly handled too to ensure smooth processing.
    1. When the bg process terminate, they send the  SIGCHLD signal, which is then used to print the info about the exited process.
    1. **Example command** - Foreground process: `gedit file1`, Background Process: `gedit file1 &`.
* **pinfo:** Pinfo shows the information about a process. Pinfo is supported for both the current shell, as well as with pid mentioned. The addition of `+` for foreground processes has support across various sessions and various processes, since it is implemented using pgid associations of foreground processes. **Example command** - `pinfo`, `pinfo 14288`
* **jobs:** This command prints all the running background processes in the shell, and sorts them on alphabetical order, with their PID, and state. Also, flags `-s` can be used to show stopped background processes, and `-r` flag shows the running processes. **ONLY one flag can be used at a time.** **Example command** - `jobs`, `jobs -r`
* **sig:** This sends a signal(given signal number) to a particular job. **Example command** - `sig 1 20`
* **fg:** This command brings a background process to the foreground and sets it to running. The process gets control of the terminal on becoming foreground. **Example command** - `fg 2`
* **bg:** This command makes a process in background running with `SIGCONT` signal. **Example command** - `bg 2`

### Signal Handling
* **CTRL+Z**: This pushes a running foreground process into background, and has no effect if no foreground process exists.
* **CTRL+C**: This terminates a foreground process by sending a SIGINT signal. Has no effect if there is no foreground process.
* **CTRL+D**: This logs the user out of the shell. This signal is ignored if some command is being executed by the shell.

### Repeatedly executing commands
All the commands below **work for background processes as well**
* **repeat:** This command executes a command a given number of times. **Example command** - `repeat 3 echo hello`
* **replay:** This executes a particular command repeatedly until a period of time while waiting for a certain interval before each execution. **Example command** - `replay -command echo ABC -interval 3 -period 8`
* **baywatch:** This executes either of the following commands repeatedly after waiting for a fixed interval between every repetition, and stops only when the `q` key is pressed:
    * interrupt: This prints the number of times the CPU(s) have been interrupted by keyboardcontroller(i8042 with IRQ 1). **Example command** - `baywatch -n 1 interrupt`
    * newborn: This displays PID of most recently created process on the system. **Example command** - `baywatch -n 1 newborn`
    * dirty: This prints the size of the part of the memory which is dirty. **Example command** - `baywatch -n 1 dirty`
    

## Assumptions
* A big assumption taken in this shell's working is that no arguments would contain a backpaced space(like path containing spaces as `\ `).
* Further, a lot of reasonable assumptions have been taken while doing `calloc` for memory, ranging from 1k characters to 10k characters in most. 
* It is assumed that piping won't be done with background processes.
* It is assumed that `>`, `<`, `>>` and `|` would be space separated from other tokens.
* It is asssumed that single output and input redirections would happen for each command.
* It is assumed that replay would be used with commands with ~0 execution time.
* It is assumed that replay wouldn't be run with foreground processes that don't have ~0 execution time.
## Directory Structure
```
.
├── svsh.out
├── sig.h
├── sig.c
├── README.md
├── pwd.h
├── pwd.c
├── prompt.h
├── prompt.c
├── process.h
├── process.c
├── pinfo.h
├── pinfo.c
├── makefile
├── main.c
├── ls.h
├── ls.c
├── jobs.h
├── jobs.c
├── inf.c
├── history.h
├── history.c
├── headers.h
├── fg.h
├── fg.c
├── echo.h
├── echo.c
├── cd.h
├── cd.c
├── bg.h
├── bg.c
├── baywatch.h
└── baywatch.c
```