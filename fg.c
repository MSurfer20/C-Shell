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
    int sig_cont_result = kill(pid, SIGCONT);
    if (sig_cont_result == -1)
    {
        perror("Error making the process running");
        return;
    }
    pid_t wtpid = waitpid(pid, &status, WUNTRACED);
    // printf("\n%d\n", getpgrp());
    // printf("BRUHHH");
    tcsetpgrp(0, getpgrp());
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);
    curr_pid = pid;
    curr_job_args = job_args;
    int i = arg_count - 1;
    curr_job_args_count = i;
    if (WIFSTOPPED(status))
    {
        int flag = 1;
        for (int x = 0; x < proc_no; x++)
        {
            if (bg_jobs[x].pid == curr_pid)
            {
                flag = 0;
                break;
            }
        }
        if (flag)
        {
            bg_jobs[proc_no].agrv = calloc(i + 4, sizeof(char *));
            for (int y = 0; y < i + 1; y++)
            {
                bg_jobs[proc_no].agrv[y] = calloc(strlen(job_args[y]) + 10, sizeof(char));
                strcpy(bg_jobs[proc_no].agrv[y], job_args[y]);
            }
            bg_jobs[proc_no].agrv[i + 2] = NULL;
            bg_jobs[proc_no].pid = pid;
            bg_jobs[proc_no].number_of_args = i + 1;
            proc_no++;
        }
        printf(" %d pushed to background\n", curr_pid);
        int sigstp_return = kill(curr_pid, SIGTSTP);
        if (sigstp_return == -1)
        {
            perror("Error stopping the process");
            return;
        }
    }
    // printf("\n");
}