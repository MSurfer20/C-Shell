#include "sig.h"
#include "headers.h"

void sig(int job_no, int signal)
{
    int ret = kill(bg_jobs[job_no - 1].pid, signal);
    if (ret == -1)
    {
        perror("Error in sending signal");
    }
}