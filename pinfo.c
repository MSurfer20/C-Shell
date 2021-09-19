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
    sprintf(proc_name, "/proc/%d", pid);
    strcpy(proc_file, proc_name);
    strcat(proc_file, "/stat");
    char a[1000], b[1000], c[1000], d[1000], e[1000], f[1000], g[1000], h[1000], i[1000];
    size_t len = 0;
    ssize_t read;

    file_ptr = fopen(proc_file, "r");
    if (file_ptr == NULL)
    {
        perror("Invalid process");
        return;
    }
    printf("pid -- %d\n", pid);
    fscanf(file_ptr, "%s %s %s %s %s %s %s %s %s", a, b, c, d, e, f, g, h, i);
    printf("Process Status -- %s", c);
    // printf("%s\n", a);
    // printf("%s\n", b);
    // printf("%s\n", c);
    // printf("%s\n", d);
    // printf("%s\n", e);
    // printf("%s\n", f);
    // printf("%s\n", g);
    // printf("%s\n", h);
    int terminal_pid = atoi(h);

    // if (getpgid(pid) == tcgetpgrp(STDIN_FILENO))
    // {
    //     printf("+");
    // }
    if (strcmp(c, "R") == 0 || strcmp(c, "S") == 0)
    {
        if (getpgid(pid) == terminal_pid)
            printf("+");
    }
    printf("\n");
    strcpy(proc_file, proc_name);
    strcat(proc_file, "/status");
    if (fclose(file_ptr) != 0)
    {
        perror("Error closing file");
    }
    file_ptr = fopen(proc_file, "r");

    while ((read = getline(&line, &len, file_ptr)) != -1)
    {
        if (startswith(line, "VmSize:"))
        {
            char *temp_string = calloc(1000, sizeof(char));
            strcpy(temp_string, line + 8);
            temp_string[strlen(temp_string) - 1] = '\0';
            printf("memory -- %s {Virtual Memory}\n", temp_string);
            free(temp_string);
        }
    }

    strcpy(proc_file, proc_name);
    strcat(proc_file, "/exe");
    char *executable_path = calloc(1000, sizeof(char));
    if (readlink(proc_file, executable_path, 1000) < 0)
    {
        perror("Error getting exec path");
        return;
    }
    char *final_exec_path = process_path(executable_path, home_dir);
    printf("Executable Path -- %s\n", final_exec_path);
    free(executable_path);
    free(final_exec_path);
    if (fclose(file_ptr) != 0)
    {
        perror("Error closing file");
    }
    if (line)
        free(line);

    free(proc_name);
    free(proc_file);
}