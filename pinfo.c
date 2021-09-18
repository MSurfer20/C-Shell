#include "headers.h"
#include "pinfo.h"
#include "prompt.h"

bool startswith(char *str1, char *str2)
{
    int len1 = strlen(str1), len2 = strlen(str2);
    if (len1 < len2)
        return false;
    for (int x = 0; x < len2; x++)
    {
        if (str1[x] != str2[x])
            return false;
    }
    return true;
}

void pinfo(pid_t pid, char *home_dir)
{
    FILE *file_ptr;
    char *line = NULL;

    char *proc_name = calloc(10000, sizeof(char));
    char *proc_file = calloc(10000, sizeof(char));
    // char *temp = calloc(10000, sizeof(char));
    sprintf(proc_name, "/proc/%d", pid);
    strcpy(proc_file, proc_name);
    strcat(proc_file, "/stat");
    char a[1000], b[1000], c[1000];
    // strcpy(proc_file, "/proc/");
    // strcpy(proc_file, pid);
    // strcpy(proc_file, "/status");
    size_t len = 0;
    ssize_t read;

    printf("pid -- %d\n", pid);
    file_ptr = fopen(proc_file, "r");
    if (file_ptr == NULL)
    {
        perror("Invalid process");
        return;
    }
    fscanf(file_ptr, "%s %s %s", a, b, c);
    printf("Process Status -- %s", c);
    if (getpgid(pid) == tcgetpgrp(STDIN_FILENO))
    {
        printf("+");
    }
    printf("\n");
    strcpy(proc_file, proc_name);
    strcat(proc_file, "/status");
    fclose(file_ptr);
    file_ptr = fopen(proc_file, "r");

    while ((read = getline(&line, &len, file_ptr)) != -1)
    {
        if (startswith(line, "VmSize:"))
            printf("memory -- %s {Virtual Memory}\n", line + 8);
    }

    strcpy(proc_file, proc_name);
    strcat(proc_file, "/exe");
    char executable_path[1000];
    if (readlink(proc_file, executable_path, 1000) < 0)
    {
        perror("Error getting exec path");
        return;
    }
    char *final_exec_path = process_path(executable_path, home_dir);
    printf("Executable Path -- %s\n", final_exec_path);

    fclose(file_ptr);
    if (line)
        free(line);
}