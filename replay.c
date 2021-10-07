#include "headers.h"
#include "replay.h"
#include "execute_command.h"

void replay(char **args, int period, int interval, char *history_file, int i, char *home_dir, char *pwd, char *previous_directory)
{
    int number_of_time = period / interval;
    for (int x = 0; x < number_of_time; x++)
    {
        sleep(interval);
        execute_command(args[0], args + 1, i - 1, history_file);
    }
    int remaining_time = period % interval;
    sleep(remaining_time);
}