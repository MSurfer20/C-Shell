#include "headers.h"
#include "fg.h"

void fg(int job_no)
{
    job_no--;
    if (job_no < 0 || job_no >= proc_no)
    {
        printf("Error: Invalid job no\n");
        return;
    }
    int pid = bg_jobs[job_no].pid;
    char **job_args = bg_jobs[job_no].agrv;
    int arg_count = bg_jobs[job_no].number_of_args;

    for (int y = job_no; y < proc_no - 1; y++)
    {
        bg_jobs[y].agrv = bg_jobs[1 + y].agrv;
        bg_jobs[y].pid = bg_jobs[1 + y].pid;
        bg_jobs[y].number_of_args = bg_jobs[1 + y].number_of_args;
    }
    proc_no--;

    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    int change_grp_stats = tcsetpgrp(0, pid);
    int status;
    if (change_grp_stats < 0)
    {
        perror("Error turning the process into a foreground one.");
        int k_stat = kill(getpid(), SIGTERM);
        if (k_stat < 0)
            kill(getpid(), SIGKILL);
    }
    kill(pid, SIGCONT);
    pid_t wtpid = waitpid(pid, &status, WUNTRACED);
    // printf("\n%d\n", getpgrp());
    tcsetpgrp(0, getpgrp());
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);
}