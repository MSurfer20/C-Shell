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
