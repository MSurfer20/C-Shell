#include "cd.h"
#include "headers.h"

void cd(char *cwd, char *home_dir, char *argument, char *previous_directory)
{
    char return_string[1000];
    if (strlen(argument) == 0)
        strcpy(return_string, home_dir);
    else if (strcmp("-", argument) == 0)
        strcpy(return_string, previous_directory);
    else if (argument[0] == '~')
        strcpy(return_string, home_dir),
            strcat(return_string, argument + 1);
    else
        strcpy(return_string, argument);

    int path_change = chdir(return_string);
    if (path_change == -1)
    {
        perror("Directory change Error");
    }
}