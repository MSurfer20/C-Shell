#include "headers.h"
#include "process.h"

void execute_process(char *command, int i, char **args, bool backround_process)
{
    fflush(stdin);
    char *exec_arguments[i + 2];
    exec_arguments[0] = calloc(strlen(command) + 2, sizeof(char));
    strcpy(exec_arguments[0], command);
    for (int x = 0; x < i; x++)
    {
        exec_arguments[x + 1] = calloc(100, sizeof(char));
        printf("%s", args[x]);
        strcpy(exec_arguments[x + 1], args[x]);
    }
    exec_arguments[i + 1] = (char *)NULL;

    int pid = fork();
    if (pid < 0)
    {
        perror("Error running process");
        return;
    }
    else if (pid == 0)
    {
        if (backround_process)
            setpgid(0, 0);
        int exec_return = execvp(command, exec_arguments);
        if (exec_return < 0)
        {
            perror("Error in process");
            exit(0);
        }
    }
    else
    {
        if (backround_process)
        {
            bg_jobs[proc_no].agrv = calloc(i, sizeof(char *));
            for (int y = 0; y < i + 1; y++)
            {
                bg_jobs[proc_no].agrv[y] = calloc(strlen(exec_arguments[y]) + 10, sizeof(char));
                strcpy(bg_jobs[proc_no].agrv[y], exec_arguments[y]);
            }
            bg_jobs[proc_no].agrv[i + 2] = NULL;
            bg_jobs[proc_no].pid = pid;
            bg_jobs[proc_no].number_of_args = i + 1;
            proc_no++;
            printf("%d\n", pid);
        }
        else
        {
            int status;
            pid_t wtpid = waitpid(pid, &status, WUNTRACED);
            printf("\n");
        }
    }
    for (int y = 0; y < i + 1; y++)
        free(exec_arguments[y]);
    return;
}

void finish_proc()
{
    int status;
    bool flag = false;
    char **argv;
    char *pname;
    pid_t pid;
    int number_of_remove_args = 0;
    int job_index = 0;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        for (int x = 0; x < proc_no; x++)
        {
            if (bg_jobs[x].pid == pid)
            {
                flag = true;
                argv = bg_jobs[x].agrv;
                number_of_remove_args = bg_jobs[x].number_of_args;

                for (int y = x; y < proc_no - 1; y++)
                {
                    bg_jobs[y].agrv = bg_jobs[y + 1].agrv;
                    bg_jobs[y].pid = bg_jobs[y + 1].pid;
                }
                proc_no--;
                int cur_rem = 0;
                break;
            }
        }

        if (!flag)
            continue;
        // for (int x = 0; x < 10; x++)
        //     printf("%s", argv[x]);
        if (!WEXITSTATUS(status) && WIFEXITED(status))
            fprintf(stderr, "\n%s with PID %d exited normally.\n", argv[0], pid);
        else
            fprintf(stderr, "\n%s with PID %d exited abnormally.\n", argv[0], pid);

        restart_loop = true;
        for (int y = 0; y < number_of_remove_args; y++)
            free(argv[y]);
        free(argv);
        fflush(stdin);
        fflush(stdout);
    }
}