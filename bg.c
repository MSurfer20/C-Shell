#include "headers.h"
#include "bg.h"

void bg(int job_no)
{
    if (job_no <= 0 || job_no > proc_no)
    {
        printf("Error: Job number is invalid.\n");
        return;
    }
    int kill_status = kill(bg_jobs[job_no - 1].pid, SIGCONT);
    if (kill_status == -1)
    {
        perror("Error in sending signal to process.\n");
        return;
    }
}