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

struct bg_process
{
    char **agrv;
    pid_t pid;
};

struct bg_process bg_jobs[1000];

int proc_no;
int counter;