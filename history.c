#include "headers.h"
#include "history.h"

void history(int number_of_commands)
{
}

void add_history(char *command)
{
    FILE *read_file = fopen("./history.txt", "r");
    char line[1000];
    int command_count = 0;
    char last_command[10000];
    char history_commands[20][1000];

    while (fgets(line, sizeof(line), read_file))
    {
        strcpy(history_commands[command_count], line);
        command_count++;
    }
    fclose(read_file);
    if (strcmp(history_commands[command_count - 1], command) == 0)
    {
        return;
    }

    FILE *clear_file = fopen("./history.txt", "w");
    fputs("", clear_file);
    fclose(clear_file);

    FILE *append_file = fopen("./history.txt", "a");
    int starting_val = 0;
    if (command_count == 20)
        starting_val = 1;
    for (int x = starting_val; x < command_count; x++)
    {
        fputs(history_commands[x], append_file);
    }
    fputs("\n", append_file);
    fputs(command, append_file);
    fclose(append_file);
}