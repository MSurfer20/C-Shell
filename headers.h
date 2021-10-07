#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/utsname.h>
#include <pwd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <grp.h>
#include <time.h>
#include <wait.h>
#include <ctype.h>
#include <termios.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/select.h>

struct bg_process
{
    char **agrv;
    pid_t pid;
    int number_of_args;
};

struct bg_process bg_jobs[1000];

int proc_no;
bool restart_loop;

int shell_pid, curr_pid, curr_job_args_count;
char **curr_job_args;