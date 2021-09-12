#include "echo.h"
#include "headers.h"

void echo(char **str, int length)
{
    if (length == 0)
    {
        printf("\n");
        return;
    }
    printf("%s", str[0]);
    for (int x = 1; x < length; x++)
    {
        printf(" %s", str[x]);
    }
    printf("\n");
}