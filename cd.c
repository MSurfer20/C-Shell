#include "cd.h"
#include "headers.h"

char *cd(char *cwd, char *home_dir, char *argument)
{
    char return_string[1000];
    printf("%s\n", home_dir);
    if (strlen(argument) == 0)
        strcpy(return_string, home_dir);
    else if (argument[0] == '~')
        strcpy(return_string, home_dir),
            strcat(return_string, argument + 1);
    else
        strcpy(return_string, argument);

    int path_change = chdir(return_string);
    printf("%s\n", return_string);
    if (path_change == -1)
    {
        perror("Directory change");
    }
}