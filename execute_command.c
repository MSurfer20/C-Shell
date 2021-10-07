#include "execute_command.h"
#include "prompt.h"
#include "headers.h"
#include "cd.h"
#include "echo.h"
#include "pwd.h"
#include "ls.h"
#include "process.h"
#include "pinfo.h"
#include "history.h"
#include "jobs.h"
#include "sig.h"
#include "fg.h"
#include "bg.h"
#include "baywatch.h"
#include "replay.h"
#include "execute_command.h"

void execute_command(char *command, char **args, int i, char *history_file, char *home_dir, char *pwd, char *previous_directory)
{
    char temp_directory[10000];
    char input_file[1000], output_file[1000];
    int standard_inp = dup(STDOUT_FILENO);
    int standard_output = dup(STDIN_FILENO);
    int input_redir = 0, output_redir = 0;

    for (int x = 0; x < i; x++)
    {
        if (strcmp(args[x], "<") == 0)
        {
            strcpy(input_file, args[x + 1]);
            // free(args[x]);
            // free(args[x + 1]);
            for (int y = x + 2; y < i; y++)
            {
                args[y - 2] = args[y];
            }
            i -= 2;
            x--;
            input_redir = 1;
        }
        else if (strcmp(args[x], ">") == 0)
        {
            strcpy(output_file, args[x + 1]);
            // free(args[x]);
            // free(args[x + 1]);
            for (int y = x + 2; y < i; y++)
            {
                args[y - 2] = args[y];
            }
            i -= 2;
            x--;
            output_redir = 1;
        }
        else if (strcmp(args[x], ">>") == 0)
        {
            strcpy(output_file, args[x + 1]);
            // free(args[x]);
            // free(args[x + 1]);
            for (int y = x + 2; y < i; y++)
            {
                args[y - 2] = args[y];
            }
            i -= 2;
            x--;
            output_redir = 2;
        }
    }

    if (input_redir)
    {
        int inp_fd = open(input_file, O_RDONLY);
        if (inp_fd < 0)
        {
            perror("Error opening input file");
            return;
        }
        dup2(inp_fd, 0);
        close(inp_fd);
    }

    if (output_redir == 1)
    {
        int op_fd = open(output_file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (op_fd < 0)
        {
            perror("Error opening output file");
            return;
        }
        dup2(op_fd, 1);
        close(op_fd);
    }

    if (output_redir == 2)
    {
        int op_fd = open(output_file, O_CREAT | O_WRONLY | O_APPEND, 0644);
        if (op_fd < 0)
        {
            perror("Error opening output file");
            return;
        }
        dup2(op_fd, 1);
        close(op_fd);
    }

    if (strcmp(command, "echo") == 0)
    {
        echo(args, i);
    }

    else if (strcmp(command, "pwd") == 0)
    {
        if (i > 0)
        {
            printf("Error: Too many arguments.\n");
            return;
        }
        get_pwd(pwd);
    }

    else if (strcmp(command, "ls") == 0)
    {
        char *ls_arguments[100];
        int ls_arg_count = 0;
        bool a_flag = false, l_flag = false, is_flag = false;
        for (int x = 0; x < i; x++)
        {
            is_flag = false;
            char *ag = args[x];
            if (ag[0] == '-')
            {
                is_flag = true;
                int ag_len = strlen(ag);
                for (int y = 1; y < ag_len; y++)
                {
                    if (ag[y] == 'a')
                        a_flag = true;
                    else if (ag[y] == 'l')
                        l_flag = true;
                    else
                        a_flag = false, l_flag = false, is_flag = false;
                }
            }
            if (!is_flag)
            {
                ls_arguments[ls_arg_count] = calloc(strlen(args[x]) + 10, sizeof(char));
                strcpy(ls_arguments[ls_arg_count], args[x]);
                ls_arg_count++;
            }
        }
        if (ls_arg_count == 0)
        {
            ls_arguments[0] = calloc(100, sizeof(char));
            ls_arg_count++;
            strcpy(ls_arguments[0], ".");
        }
        ls(home_dir, a_flag, l_flag, ls_arguments, ls_arg_count);
        for (int y = 0; y < ls_arg_count; y++)
            free(ls_arguments[y]);
    }

    else if (strcmp(command, "cd") == 0)
    {
        char *cd_argument;
        if (i == 0)
        {
            cd_argument = "";
        }
        else if (i == 1)
            cd_argument = args[0];
        else
        {
            printf("Error: Invalid arguments given for cd\n");
            return;
        }
        strcpy(temp_directory, pwd);
        cd(pwd, home_dir, cd_argument, previous_directory);
        strcpy(previous_directory, temp_directory);
    }

    else if (strcmp(command, "pinfo") == 0)
    {
        pid_t pid;
        if (i == 0)
        {
            pid = getpid();
        }
        else if (i == 1)
        {
            int pid_length = strlen(args[0]);
            for (int ab = 0; ab < pid_length; ab++)
            {
                if (!isdigit(args[0][ab]))
                {
                    printf("Pid can only have numbers.");
                    return;
                }
            }
            pid = atoi(args[0]);
        }
        else
        {
            printf("Error: Invalid number of arguments given for pinfo.\n");
            return;
        }
        pinfo(pid, home_dir);
    }

    else if (strcmp(command, "repeat") == 0)
    {
        char *pass_command[1000];
        int repeat_length = strlen(args[0]);
        for (int ab = 0; ab < repeat_length; ab++)
        {
            if (!isdigit(args[0][ab]))
            {
                printf("Second argument must be an integer.");
                return;
            }
        }
        int command_count = atoi(args[0]);
        for (int x = 0; x < command_count; x++)
        {
            for (int y = 2; y < i; y++)
            {
                pass_command[y - 2] = calloc(strlen(args[y]) + 20, sizeof(char));
                strcpy(pass_command[y - 2], args[y]);
            }
            int bc = i - 2;
            if (i - 2 < 0)
                bc = 0;
            execute_command(args[1], pass_command, bc, history_file, home_dir, pwd, previous_directory);
            for (int y = 2; y < i; y++)
            {
                free(pass_command[y - 2]);
            }
        }
    }

    else if (strcmp(command, "history") == 0)
    {
        // add_history("10");
        if (i == 0)
            history(10, history_file);
        else if (i == 1)
        {
            int number_length = strlen(args[0]);
            for (int ab = 0; ab < number_length; ab++)
            {
                if (!isdigit(args[0][ab]))
                {
                    printf("Second argument can only be a number.\n");
                    return;
                }
            }
            history(atoi(args[0]), history_file);
        }
        else
        {
            printf("Error: Invalid number of arguments for history.\n");
            return;
        }
    }

    else if (strcmp(command, "jobs") == 0)
    {
        int job_type = 0;
        for (int x = 0; x < i; x++)
        {
            char *ag = args[x];
            if (ag[0] == '-')
            {
                int ag_len = strlen(ag);
                for (int y = 0; y < ag_len; y++)
                {
                    if (ag[y] == 's')
                        job_type = 1;
                    else if (ag[y] == 'r')
                        job_type = 2;
                }
            }
        }
        jobs(job_type);
    }

    else if (strcmp(command, "sig") == 0)
    {
        int job_no = atoi(args[0]);
        int signal_number = atoi(args[1]);
        sig(job_no, signal_number);
    }

    else if (strcmp(command, "fg") == 0)
    {
        if (i != 1)
        {
            printf("Error: Number of arguments should be exactly 1.\n");
            return;
        }
        int job_no = atoi(args[0]);
        fg(job_no);
    }

    else if (strcmp(command, "bg") == 0)
    {
        if (i != 1)
        {
            printf("Error: Number of arguments should be exactly 1.\n");
            return;
        }
        int job_no = atoi(args[0]);
        bg(job_no);
    }

    else if (strcmp(command, "replay") == 0)
    {
        int interval = 3, period = 6;
        for (int x = 0; x < i;)
        {
            if (strcmp(args[x], "-command") == 0)
            {
                for (int y = x; y < i - 1; y++)
                    args[y] = args[y + 1];
                i--;
                // printf(" 1\n");
            }
            else if (strcmp(args[x], "-interval") == 0)
            {
                interval = atoi(args[x + 1]);
                for (int y = x; y < i - 2; y++)
                    args[y] = args[y + 2];
                i -= 2;
                // printf(" 2\n");
            }
            else if (strcmp(args[x], "-period") == 0)
            {
                period = atoi(args[x + 1]);
                for (int y = x; y < i - 2; y++)
                    args[y] = args[y + 2];
                i -= 2;
                // printf(" 3\n");
            }
            else
                x++;
        }
        // printf("%d\n", i);
        // for (int x = 0; x < i; x++)
        //     printf("%s ", args[x]);
        // printf("\n%d %d", period, interval);
        replay(args, period, interval, history_file, i);
    }

    else if (strcmp(command, "baywatch") == 0)
    {
        int time = 1, command_id = 0;
        for (int x = 0; x < i; x++)
        {
            if (strcmp(args[x], "-n") == 0)
            {
                time = atoi(args[x + 1]);
            }
            else if (strcmp(args[x], "interrupt") == 0)
                command_id = 0;
            else if (strcmp(args[x], "newborn") == 0)
                command_id = 1;
            else if (strcmp(args[x], "dirty") == 0)
                command_id = 2;
        }
        baywatch(time, command_id);
    }

    else
    {
        bool backround_process = false;
        if (i > 0)
        {
            int arg_length = strlen(args[i - 1]);
            if (args[i - 1][arg_length - 1] == '&')
            {
                backround_process = true;
                args[i - 1][arg_length - 1] = '\0';
                if (arg_length == 1)
                {
                    args[i - 1] = NULL;
                    i--;
                }
            }
        }
        int cmd_len = strlen(command);
        if (command[cmd_len - 1] == '&')
            command[cmd_len - 1] = '\0', backround_process = true;
        execute_process(command, i, args, backround_process, home_dir);
    }

    dup2(standard_inp, 0);
    dup2(standard_output, 1);
}