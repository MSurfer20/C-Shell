#ifndef __PROCESS_H
#define __PROCESS_H

void execute_process(char *command, int i, char **args, bool backround_process, char *home_dir);
void finish_proc();

#endif
