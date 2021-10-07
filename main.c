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

char home_dir[10000], pwd[10000];
char previous_directory[10000];
char temp_directory[10000];

void ignoresignal(int signal)
{
    return;
}

void exitfunction(int signal)
{
    printf("BRUH");
    exit(0);
}

void die(const char *s)
{
    // perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

/**
 * Enable row mode for the terminal
 * The ECHO feature causes each key you type to be printed to the terminal, so you can see what you’re typing.
 * Terminal attributes can be read into a termios struct by tcgetattr().
 * After modifying them, you can then apply them to the terminal using tcsetattr().
 * The TCSAFLUSH argument specifies when to apply the change: in this case, it waits for all pending output to be written to the terminal, and also discards any input that hasn’t been read.
 * The c_lflag field is for “local flags”
 */
void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

/**
 * stdout and stdin are buffered we disable buffering on that
 * After entering in raw mode we read characters one by one
 * Up arrow keys and down arrow keys are represented by 3 byte escape codes
 * starting with ascii number 27 i.e. ESC key
 * This way we interpret arrow keys
 * Tabs are usually handled by the term, but here we are simulating tabs for the sake of simplicity
 * Backspace move the cursor one control character to the left
 * @return
 */

void execute_command(char *command, char **args, int i, char *history_file)
{
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
            execute_command(args[1], pass_command, bc, history_file);
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
        int number_of_time = period / interval;
        for (int x = 0; x < number_of_time; x++)
        {
            sleep(interval);
            execute_command(args[0], args + 1, i - 1, history_file);
        }
        int remaining_time = period % interval;
        sleep(remaining_time);
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

void stop_signal()
{
    // printf("YASSSSS");
    if (getpid() == shell_pid || curr_pid == -1)
        return;
    int flag = 1;
    for (int x = 0; x < proc_no; x++)
    {
        if (bg_jobs[x].pid == curr_pid)
        {
            flag = 0;
            break;
        }
    }
    if (flag)
    {
        bg_jobs[proc_no].number_of_args = curr_job_args_count + 1;
        bg_jobs[proc_no].agrv = calloc(curr_job_args_count + 4, sizeof(char *));
        for (int y = 0; y < curr_job_args_count; y++)
        {
            bg_jobs[proc_no].agrv[y] = calloc(strlen(curr_job_args[y]) + 10, sizeof(char));
            strcpy(bg_jobs[proc_no].agrv[y], curr_job_args[y]);
        }
        bg_jobs[proc_no].agrv[curr_job_args_count + 2] = NULL;
        bg_jobs[proc_no].pid = curr_pid;
        bg_jobs[proc_no].number_of_args = curr_job_args_count + 1;
        proc_no++;
        printf("%d pushed to background\n", curr_pid);
    }
    kill(curr_pid, SIGTSTP);
}

void kill_proc()
{
    if (getpid() == shell_pid)
        return;
    if (curr_pid != -1)
    {
        kill(curr_pid, SIGINT);
    }
}

void run_command(char *each_command, char *history_file)
{
    char *savepointer2;
    char *token = strtok_r(each_command, " \t", &savepointer2);
    char *command = token;

    char *args[100];
    int i = 0;
    token = strtok_r(NULL, " \t", &savepointer2);
    while (token != NULL)
    {
        args[i] = token;
        i++;
        token = strtok_r(NULL, " \t", &savepointer2);
    }

    if (strcmp(command, "exit") == 0)
    {
        exit(0);
        printf("BRUH");
    }

    execute_command(command, args, i, history_file);
}

void parse_pipe(char *each_command, char *history_file)
{
    int pipe_count = 0, pipe_fd[2], input_fd = STDIN_FILENO, saved_output = dup(STDOUT_FILENO);
    // WTF WHY SAME STDOUT??!!
    int save_in = dup(STDIN_FILENO);
    int command_length = strlen(each_command);
    for (int x = 0; x < command_length; x++)
        if (each_command[x] == '|')
            pipe_count++;

    if (pipe_count == 0)
    {
        run_command(each_command, history_file);
        return;
    }

    char *savepointer3;
    char *token = strtok_r(each_command, "|", &savepointer3);
    int command_no = 0;

    while (token != NULL)
    {
        // printf("%s\n", token);
        int pipe_return = pipe(pipe_fd);
        if (pipe_return < 0)
        {
            perror("Error piping the commands");
            return;
        }

        pid_t child_command_proc = fork();
        if (child_command_proc < 0)
        {
            perror("Error forking process");
            return;
        }
        else if (child_command_proc == 0)
        {
            dup2(input_fd, STDIN_FILENO);
            if (command_no != pipe_count)
                dup2(pipe_fd[1], STDOUT_FILENO);
            close(pipe_fd[0]);
            run_command(token, history_file);
            exit(EXIT_SUCCESS);
        }
        else
        {
            wait(NULL);
            close(pipe_fd[1]);
            input_fd = pipe_fd[0];
        }

        token = strtok_r(NULL, "|", &savepointer3);
        command_no += 1;
    }
    dup2(save_in, STDIN_FILENO);
    dup2(saved_output, STDOUT_FILENO);
}

int main()
{
    proc_no = 0;
    proc_no = 0;
    shell_pid = getpid();
    curr_pid = shell_pid;

    char *test_return;
    test_return = getcwd(home_dir, 10000);
    if (test_return == NULL)
    {
        perror("Error in getting home directory");
        return 1;
    }

    strcpy(previous_directory, home_dir);
    if (errno == ERANGE)
    {
        perror("Buffer size exceeded");
        return 1;
    }

    signal(SIGCHLD, finish_proc);
    signal(SIGINT, kill_proc);
    signal(SIGTSTP, stop_signal);
    signal(SIGILL, exitfunction);

    char *actual_home_path = getenv("HOME");
    char *history_file = calloc(1000, sizeof(char));
    strcpy(history_file, actual_home_path);
    strcat(history_file, "/history.txt");

    char *all_commands = calloc(10000, sizeof(char));
    char c;
    int prev_command_no = 0;

    while (1)
    {
        prev_command_no = 0;

        test_return = getcwd(pwd, 10000);
        if (test_return == NULL)
        {
            perror("Error in getting home directory");
            return 1;
        }
        if (errno == ERANGE)
        {
            perror("Buffer size exceeded");
            return 1;
        }

        prompt(pwd, home_dir);

        setbuf(stdout, NULL);
        enableRawMode();
        memset(all_commands, '\0', sizeof(all_commands));
        int pt = 0;
        while (read(STDIN_FILENO, &c, 1) == 1)
        {
            if (iscntrl(c))
            {
                if (c == 10)
                    break;
                else if (c == 27)
                {
                    char buf[3];
                    buf[2] = 0;
                    if (read(STDIN_FILENO, buf, 2) == 2)
                    { // length of escape code
                        if (buf[1] == 'A')
                        {
                            printf("\r\33[2K");
                            printf("\r");
                            prompt(pwd, home_dir);
                            prev_command_no++;
                            char *history_command = get_nth_history(prev_command_no, history_file);
                            pt = strlen(history_command);
                            strcpy(all_commands, history_command);
                            all_commands[pt] = '\0';
                            printf("%s", history_command);
                            free(history_command);
                        }
                    }
                }
                else if (c == 127)
                { // backspace
                    if (pt > 0)
                    {
                        if (all_commands[pt - 1] == 9)
                        {
                            for (int i = 0; i < 7; i++)
                            {
                                printf("\b");
                            }
                        }
                        all_commands[--pt] = '\0';
                        printf("\b \b");
                    }
                }
                else if (c == 9)
                { // TAB character
                    all_commands[pt++] = c;
                    for (int i = 0; i < 8; i++)
                    { // TABS should be 8 spaces
                        printf(" ");
                    }
                }
                // else if (c == 4)
                // {
                //     exit(0);
                // }
                else if (c == 4)
                {
                    return 0;
                }
                else
                {
                    printf("%d\n", c);
                }
            }
            else
            {
                all_commands[pt++] = c;
                printf("%c", c);
            }
        }
        disableRawMode();
        all_commands[pt] = '\0';
        printf("\n");

        char *each_command;
        char *savepointer1;
        int all_commands_length = strlen(all_commands);
        if (all_commands_length == 0)
            continue;
        bool empty_input_detected = true;
        for (int command_index = 0; command_index < all_commands_length; command_index++)
            if (all_commands[command_index] != ' ' && all_commands[command_index] != '\t')
                empty_input_detected = false;
        if (empty_input_detected)
            continue;

        add_history(all_commands, history_file);

        each_command = strtok_r(all_commands, ";", &savepointer1);

        while (each_command != NULL)
        {
            parse_pipe(each_command, history_file);
            each_command = strtok_r(NULL, ";", &savepointer1);
        }
        fflush(stdin);

        // TAKE INPUT HERE
        // free(all_commands);
    }
}
