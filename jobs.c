#include "jobs.h"
#include "headers.h"

void jobs(int job_type)
{
    pid_t pid[proc_no];
    int run_status[proc_no];
    char *argv[proc_no];
    int job_no[proc_no];
    for (int x = 0; x < proc_no; x++)
    {
        job_no[x] = x + 1;
        pid[x] = bg_jobs[x].pid;
        argv[x] = calloc(10000, sizeof(char));
        strcpy(argv[x], bg_jobs[x].agrv[0]);
        for (int y = 1; y < bg_jobs[x].number_of_args; y++)
        {
            strcat(argv[x], " ");
            strcat(argv[x], bg_jobs[x].agrv[y]);
        }
        run_status[x] = bg_jobs[x].run_status;
    }

    int i, j, min_idx, temp_run, temp_job_no;
    pid_t temp_pid;
    char temp_name[1000];
    for (i = 0; i < proc_no - 1; i++)
    {
        min_idx = i;
        for (j = i + 1; j < proc_no; j++)
            if (strcmp(argv[j], argv[min_idx]) < 0)
                min_idx = j;
        temp_pid = pid[min_idx];
        strcpy(temp_name, argv[min_idx]);
        temp_run = run_status[min_idx];
        temp_job_no = job_no[min_idx];

        pid[min_idx] = pid[i];
        strcpy(argv[min_idx], argv[i]);
        run_status[min_idx] = run_status[i];
        job_no[min_idx] = job_no[i];

        pid[i] = temp_pid;
        strcpy(argv[i], temp_name);
        run_status[i] = temp_run;
        job_no[i] = temp_job_no;
    }

    for (i = 0; i < proc_no; i++)
    {
        if (job_type == 1 && run_status[i] == 1)
            continue;
        if (job_type == 2 && run_status[i] == 0)
            continue;
        printf("[%d] ", job_no[i]);
        if (run_status[i])
            printf("Running ");
        else
            printf("Stopped ");
        printf("%s [%d]\n", argv[i], (int)pid[i]);
    }
}