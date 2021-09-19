#include "prompt.h"
#include "headers.h"
#include "cd.h"
#include "echo.h"
#include "pwd.h"
#include "ls.h"
#include "process.h"
#include "pinfo.h"
#include "history.h"

char home_dir[10000], pwd[10000];
char previous_directory[10000];
char temp_directory[10000];

void ignoresignal(int signal)
{
    return;
}

void die(const char *s)
{
    perror(s);
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
        char *pass_command[100];
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
                pass_command[y - 2] = calloc(strlen(args[y]), sizeof(char));
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
                    printf("Second argument can only be a number.");
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
        execute_process(command, i, args, backround_process);
    }
}

int main()
{
    proc_no = 0;

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
    signal(SIGINT, ignoresignal);
    signal(SIGTSTP, ignoresignal);
    char *history_file = calloc(1000, sizeof(char));
    strcpy(history_file, home_dir);
    strcat(history_file, "/history.txt");

    char *all_commands = calloc(1000, sizeof(char));
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
        memset(all_commands, '\0', 100);
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
                            printf("\r");
                            for (int x = 0; x < 100; x++)
                                printf(" ");
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
        char *savepointer1, *savepointer2;
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
                return 0;
            }

            execute_command(command, args, i, history_file);

            each_command = strtok_r(NULL, ";", &savepointer1);
        }
        fflush(stdin);

        // TAKE INPUT HERE
        // free(all_commands);
    }
}
