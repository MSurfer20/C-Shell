#ifndef __HISTORY_H
#define __PINFO_H

void history(int number_of_commands, char *history_file);
void add_history(char *command, char *history_file);
char *get_nth_history(int n, char *history_file);

#endif
