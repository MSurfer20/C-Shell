#include "headers.h"
#include "history.h"

void history(int number_of_commands, char *history_file)
{
    FILE *read_file = fopen(history_file, "r");
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
    if (command_count < number_of_commands)
    {
        for (int x = 0; x < command_count; x++)
            printf("%s", history_commands[x]);
        return;
    }
    for (int x = command_count - number_of_commands; x < command_count; x++)
        printf("%s", history_commands[x]);
    printf("\n");
}

void add_history(char *command, char *history_file)
{
    if (strlen(command) == 0)
        return;
    FILE *read_file = fopen(history_file, "r");
    char line[1000];
    int command_count = 0;
    char last_command[10000];
    char history_commands[20][1000];
    if (read_file != NULL)
        while (fgets(line, sizeof(line), read_file))
        {
            strcpy(history_commands[command_count], line);
            command_count++;
        }
    if (read_file != NULL)
        fclose(read_file);
    if (strcmp(history_commands[command_count - 1], command) == 0)
    {
        return;
    }

    FILE *clear_file = fopen(history_file, "w");
    fputs("", clear_file);
    fclose(clear_file);

    FILE *append_file = fopen(history_file, "a");
    int starting_val = 0;
    if (command_count == 20)
        starting_val = 1;
    for (int x = starting_val; x < command_count; x++)
    {
        fputs(history_commands[x], append_file);
    }
    if (command_count != 0)
        fputs("\n", append_file);
    fputs(command, append_file);
    fclose(append_file);
}

char *get_nth_history(int n, char *history_file)
{
    FILE *read_file = fopen(history_file, "r");
    char *return_str = calloc(1000, sizeof(char));
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
    int a = command_count - n;
    if (a < 0)
        strcpy(return_str, history_commands[0]);
    else
        strcpy(return_str, history_commands[a]);
    if (return_str[strlen(return_str) - 1] == '\n')
        return_str[strlen(return_str) - 1] = '\0';
    return return_str;
}