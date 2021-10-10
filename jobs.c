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

        FILE *file_ptr;
        char *line = NULL;

        char *proc_name = calloc(10000, sizeof(char));
        char *proc_file = calloc(10000, sizeof(char));
        sprintf(proc_name, "/proc/%d", pid[x]);
        strcpy(proc_file, proc_name);
        strcat(proc_file, "/stat");
        char a[1000], b[1000], c[1000], d[1000], e[1000], f[1000], g[1000], h[1000], i[1000];
        size_t len = 0;
        ssize_t read;

        file_ptr = fopen(proc_file, "r");
        if (file_ptr == NULL)
        {
            perror("Invalid process");
            return;
        }
        fscanf(file_ptr, "%s %s %s %s %s %s %s %s %s", a, b, c, d, e, f, g, h, i);
        if (strcmp(c, "T") == 0)
            run_status[x] = 0;
        else
            run_status[x] = 1;
    }

    int i, j, min_idx, temp_run, temp_job_no;
    pid_t temp_pid;
    char temp_name[1000];
    for (i = 0; i < proc_no - 1; i++)
    {
        min_idx = i;
        for (j = i + 1; j < proc_no; j++)
        {
            if (strcmp(argv[j], argv[min_idx]) < 0)
                min_idx = j;
            else if (strcmp(argv[j], argv[min_idx]) == 0)
            {
                if (job_no[j] < job_no[min_idx])
                    min_idx = j;
            }
        }
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