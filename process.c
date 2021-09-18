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
            bg_jobs[proc_no].agrv = exec_arguments;
            bg_jobs[proc_no].pid = pid;
            proc_no++;
            printf("%d\n", pid);
        }
        else
        {
            int status;
            pid_t wtpid = waitpid(pid, &status, WUNTRACED);
        }
    }
    return;
}

void finish_proc()
{
    int status;
    bool flag = false;
    char **argv;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        for (int x = 0; x < proc_no; x++)
        {
            if (bg_jobs[x].pid == pid)
            {
                flag = true;
                argv = bg_jobs[x].agrv;

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
            return;
        if (!WEXITSTATUS(status) && WIFEXITED(status))
            printf("%s with PID %d exited normally.\n", argv[1], pid);
        else
            printf("%s with PID %d did not exit normally.\n", argv[1], pid);
        restart_loop = true;
        fflush(stdin);
        fflush(stdout);
    }
}