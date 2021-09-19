#include "headers.h"
#include "history.h"

void history(int number_of_commands, char *history_file)
{
    FILE *read_file = fopen(history_file, "r");
    if (read_file == NULL && errno != 2)
    {
        perror("Error opening history file. Command not added to history.\n");
        return;
    }
    char line[10000];
    int command_count = 0;
    char last_command[10000];
    char history_commands[20][1000];

    while (fgets(line, sizeof(line), read_file))
    {
        strcpy(history_commands[command_count], line);
        command_count++;
    }
    if (fclose(read_file) != 0)
    {
        perror("Error closing file");
    }
    if (command_count < number_of_commands)
    {
        for (int x = 0; x < command_count; x++)
            printf("%s", history_commands[x]);
        printf("\n");
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
    char line[10000];
    int fputs_error = 10;
    int command_count = 0;
    char last_command[10000];
    char history_commands[20][10000];
    if (read_file == NULL && errno != 2)
    {
        perror("Error opening history file. Command not added to history.\n");
        return;
    }
    if (read_file != NULL)
        while (fgets(line, sizeof(line), read_file))
        {
            strcpy(history_commands[command_count], line);
            command_count++;
        }
    if (read_file != NULL)
    {
        if (fclose(read_file) != 0)
        {
            perror("Error closing file");
        }
    }
    if (strcmp(history_commands[command_count - 1], command) == 0)
    {
        return;
    }

    FILE *clear_file = fopen(history_file, "w");
    if (clear_file == NULL && errno != 2)
    {
        perror("Error opening history file. Command not added to history.\n");
        return;
    }
    fputs_error = fputs("", clear_file);
    if (fputs_error < 0)
    {
        perror("Error writing command to history file.\n");
        return;
    }
    if (fclose(clear_file) != 0)
    {
        perror("Error closing file");
    }
    FILE *append_file = fopen(history_file, "a");
    if (append_file == NULL && errno != 2)
    {
        perror("Error opening history file. Command not added to history.\n");
        return;
    }
    int starting_val = 0;
    if (command_count == 20)
        starting_val = 1;
    for (int x = starting_val; x < command_count; x++)
    {
        fputs_error = fputs(history_commands[x], append_file);
        if (fputs_error < 0)
        {
            perror("Error writing command to history file.\n");
            return;
        }
    }
    if (command_count != 0)
    {
        fputs_error = fputs("\n", append_file);
        if (fputs_error < 0)
        {
            perror("Error writing command to history file.\n");
            return;
        }
    }

    fputs_error = fputs(command, append_file);
    if (fputs_error < 0)
    {
        perror("Error writing command to history file.\n");
        return;
    }

    if (fclose(append_file) != 0)
    {
        perror("Error closing file");
    }
}

char *get_nth_history(int n, char *history_file)
{
    FILE *read_file = fopen(history_file, "r");
    char *return_str = calloc(10000, sizeof(char));
    if (read_file == NULL)
    {
        if (errno != 2)
            perror("Error opening history file. Command not added to history.\n");
        strcpy(return_str, "");
        return return_str;
    }
    char line[10000];
    int command_count = 0;
    char last_command[10000];
    char history_commands[20][10000];

    while (fgets(line, sizeof(line), read_file))
    {
        strcpy(history_commands[command_count], line);
        command_count++;
    }
    if (fclose(read_file) != 0)
    {
        perror("Error closing file");
    }
    int a = command_count - n;
    if (a < 0)
        strcpy(return_str, history_commands[0]);
    else
        strcpy(return_str, history_commands[a]);
    if (return_str[strlen(return_str) - 1] == '\n')
        return_str[strlen(return_str) - 1] = '\0';
    return return_str;
}