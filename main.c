#include "prompt.h"
#include "headers.h"
#include "cd.h"
#include "echo.h"

int main()
{
    char home_dir[10000], pwd[10000];
    char *previous_directory = calloc(10000, sizeof(char));
    char *temp_directory = calloc(10000, sizeof(char));
    getcwd(home_dir, 10000);
    strcpy(previous_directory, home_dir);
    if (errno == ERANGE)
    {
        perror("Prompt details");
    }
    while (1)
    {
        getcwd(pwd, 10000);
        prompt(pwd, home_dir);
        char all_commands[10000];
        scanf("%[^\n]%*c", all_commands);
        char *each_command;
        char *savepointer1, *savepointer2;

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
            if (strcmp(command, "echo") == 0)
            {
                echo(args, i);
            }
            if (strcmp(command, "cd") == 0)
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
                    continue;
                }
                strcpy(temp_directory, pwd);
                cd(pwd, home_dir, cd_argument, previous_directory);
                strcpy(previous_directory, temp_directory);
            }

            each_command = strtok_r(NULL, ";", &savepointer1);
        }

        // TAKE INPUT HERE
    }
}
