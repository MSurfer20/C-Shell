#include "headers.h"
#include "process.h"

void execute_process(char *command, int i, char **args, bool backround_process)
{
    printf("YASSSSSSSS");
    printf("%d\n", (int)backround_process);
    char *exec_arguments[i + 2];
    exec_arguments[0] = command;
    for (int x = 0; x < i; x++)
        exec_arguments[x + 1] = args[x];
    exec_arguments[i + 1] = (char *)NULL;

    int pid = fork();
    if (pid < 0)
    {
        perror("Error running process");
        return;
    }
    else if (pid == 0)
    {
        // setpgid(0, 0);
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

            // setpgid(0, 0);
            printf("%d\n", pid);
        }
        else
        {
            int status;
            pid_t wtpid = waitpid(pid, &status, WUNTRACED);
        }
    }
}

void finish_proc()
{
    printf("YAYYYYYYYYYY");
    int status;
    bool flag = false;
    char **argv;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    // if (pid < 0)
    //     return;
    for (int x = 0; x < proc_no - 1; x++)
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
            printf("BROOOOOOOOOOO");

            break;
        }
    }

    if (!flag)
        return;
    if (!WEXITSTATUS(status) && WIFEXITED(status))
        printf("%s with PID %d exited normally.\n", argv[0], pid);
    else
        printf("%s with PID %d did not exit normally.\n", argv[0], pid);
    fflush(stdout);
    fflush(stdin);
}