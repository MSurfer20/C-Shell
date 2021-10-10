#include "sig.h"
#include "headers.h"

void sig(int job_no, int signal)
{
    if (job_no <= 0 || job_no > proc_no)
    {
        printf("Error: Invalid job no.\n");
        return;
    }
    // if ((signal < 1) || (signal > 64) || (signal == 16) || (signal == 32) || (signal == 33))
    // {
    //     printf("Error: Invalid signal.\n");
    //     return;
    // }
    int ret = kill(bg_jobs[job_no - 1].pid, signal);
    if (ret == -1)
    {
        perror("Error in sending signal");
    }
}