#include "prompt.h"
#include "headers.h"
#include "cd.h"

int main()
{
    char home_dir[10000], pwd[10000];
    getcwd(home_dir, 10000);
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

        each_command = strtok(all_commands, ";");

        while (each_command != NULL)
        {
            char *token = strtok(each_command, " ");
            char *command = token;
            char *args[100];
            int i = 0;
            token = strtok(NULL, " ");
            while (token != NULL)
            {
                args[i] = token;
                i++;
                token = strtok(NULL, " ");
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
                    perror("Invalid arguments given for cd");
                }
                cd(pwd, home_dir, cd_argument);
            }

            each_command = strtok(NULL, ";");
        }

        // TAKE INPUT HERE
    }
}
