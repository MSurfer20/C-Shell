#include "ls.h"
#include "headers.h"

void print_file_data(char *file_name, char *file_path)
{

    struct stat file_stats;
    int stat_result = stat(file_path, &file_stats);
    if (stat_result != 0)
    {
        char err_buf[1000];
        sprintf(err_buf, "Error getting stats of %s", file_name);
        perror(err_buf);
        return;
    }
    char ch;
    if (S_ISDIR(file_stats.st_mode))
        ch = 'd';
    else
        ch = '-';
    printf("%c", ch);
    if (file_stats.st_mode & S_IRUSR)
        ch = 'r';
    else
        ch = '-';
    printf("%c", ch);
    if (file_stats.st_mode & S_IWUSR)
        ch = 'w';
    else
        ch = '-';
    printf("%c", ch);
    if (file_stats.st_mode & S_IXUSR)
        ch = 'x';
    else
        ch = '-';
    printf("%c", ch);
    if (file_stats.st_mode & S_IRGRP)
        ch = 'r';
    else
        ch = '-';
    printf("%c", ch);
    if (file_stats.st_mode & S_IWGRP)
        ch = 'w';
    else
        ch = '-';
    printf("%c", ch);
    if (file_stats.st_mode & S_IXGRP)
        ch = 'x';
    else
        ch = '-';
    printf("%c", ch);
    if (file_stats.st_mode & S_IROTH)
        ch = 'r';
    else
        ch = '-';
    printf("%c", ch);
    if (file_stats.st_mode & S_IWOTH)
        ch = 'w';
    else
        ch = '-';
    printf("%c", ch);
    if (file_stats.st_mode & S_IXOTH)
        ch = 'x';
    else
        ch = '-';
    printf("%c", ch);
    printf(" ");
    printf(" %3d", (int)file_stats.st_nlink);

    struct passwd *pw = getpwuid(file_stats.st_uid);
    if (pw == NULL)
    {
        char err_buf[1000];
        sprintf(err_buf, "\rError getting owner of %s\n", file_name);
        perror(err_buf);
        return;
    }
    printf("  %10s", pw->pw_name);
    struct group *grp = getgrgid(file_stats.st_gid);
    if (grp == NULL)
    {
        char err_buf[1000];
        sprintf(err_buf, "\rError getting groupid of %s\n", file_name);
        perror(err_buf);
        return;
    }
    printf("  %10s", grp->gr_name);
    printf(" %10d", (int)file_stats.st_size);
    struct tm *file_time = localtime(&file_stats.st_mtim.tv_sec);
    if (file_time == NULL)
    {
        char err_buf[1000];
        sprintf(err_buf, "\rError getting time of %s\n", file_name);
        perror(err_buf);
        return;
    }
    int file_date = file_time->tm_mday;
    int file_month = file_time->tm_mon;
    int file_year = 1900 + file_time->tm_year;
    int file_hour = file_time->tm_hour;
    int file_min = file_time->tm_min;
    int file_sec = file_time->tm_sec;

    time_t raw_curr_time;
    time(&raw_curr_time);
    struct tm *curr_time_info = localtime(&raw_curr_time);
    if (curr_time_info == NULL)
    {
        char err_buf[1000];
        sprintf(err_buf, "\rError getting current timr");
        perror(err_buf);
        return;
    }
    char time_array[100];

    char month_array[12][20] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    if (1900 + curr_time_info->tm_year != file_year)
    {
        file_time = localtime(&file_stats.st_mtim.tv_sec);
        if (file_time == NULL)
        {
            char err_buf[1000];
            sprintf(err_buf, "\rError getting time of %s\t", file_name);
            perror(err_buf);
            return;
        }
        strftime(time_array, sizeof(time_array), " %b %e %Y", file_time);
        printf("%s", time_array);
    }
    else if (curr_time_info->tm_mon - file_month > 6)
    {
        file_time = localtime(&file_stats.st_mtim.tv_sec);
        if (file_time == NULL)
        {
            char err_buf[1000];
            sprintf(err_buf, "\rError getting time of %s\t", file_name);
            perror(err_buf);
            return;
        }
        strftime(time_array, sizeof(time_array), " %b %e  %Y", file_time);
        printf("%s", time_array);
    }
    else if ((curr_time_info->tm_mon - file_month == 6) && (curr_time_info->tm_mday > file_date))
    {
        file_time = localtime(&file_stats.st_mtim.tv_sec);
        if (file_time == NULL)
        {
            char err_buf[1000];
            sprintf(err_buf, "\rError getting time of %s\t", file_name);
            perror(err_buf);
            return;
        }
        strftime(time_array, sizeof(time_array), " %b %e %Y", file_time);
        printf("%s", time_array);
    }
    else
    {
        file_time = localtime(&file_stats.st_mtim.tv_sec);
        if (file_time == NULL)
        {
            char err_buf[1000];
            sprintf(err_buf, "\rError getting time name of %s\t", file_name);
            perror(err_buf);
            return;
        }
        strftime(time_array, sizeof(time_array), " %b %e %H:%M", file_time);
        printf("%s", time_array);
    }

    printf(" %s", file_name);
    printf("\n");
}

void ls(char *home_dir, bool a_flag, bool l_flag, char **argument_list, int arg_length)
{
    char *argument;
    bool dir_newline_flag = false;
    for (int y = 0; y < arg_length; y++)
    {
        bool free_flag = false;
        argument = argument_list[y];
        if (strlen(argument) == 0)
            argument[0] = '.';
        if (argument[0] == '~')
        {
            char *new_argument = calloc(1000, sizeof(char));
            strcpy(new_argument, home_dir);
            strcat(new_argument, argument + 1);
            argument = new_argument;
            free_flag = true;
        }

        //Testing if a file or directory
        struct stat stats;
        int stat_return = stat(argument, &stats);
        if (stat_return != 0)
        {
            perror("Cannot open directory or file");
            continue;
        }

        //Code for a directory
        if (S_ISDIR(stats.st_mode))
        {
            if (arg_length > 1)
            {
                if (y > 0)
                    printf("\n");
                dir_newline_flag = true;
                printf("%s:\n", argument);
            }
            struct dirent *next_file;
            DIR *dire = opendir(argument);

            if (dire == NULL)
            {
                perror("Couldnt open directory");
            }

            char *file_name = calloc(1000, sizeof(char));
            if (l_flag)
            {
                int total_siz = 0;
                next_file = readdir(dire);
                while ((next_file != NULL))
                {
                    strcpy(file_name, next_file->d_name);
                    if (!a_flag && file_name[0] == '.')
                    {
                        next_file = readdir(dire);
                        continue;
                    }

                    if (!l_flag)
                    {
                        next_file = readdir(dire);
                        continue;
                    }
                    char file_path[10000];
                    strcpy(file_path, argument);
                    strcat(file_path, "/");
                    strcat(file_path, file_name);
                    struct stat file_stats;
                    int stat_result = stat(file_path, &file_stats);
                    if (stat_result == -1)
                    {
                        char err_buf[1000];
                        sprintf(err_buf, "Error reading %s", file_name);
                        perror(err_buf);
                        return;
                    }
                    total_siz += file_stats.st_blocks;
                    next_file = readdir(dire);
                }
                printf("total %d\n", total_siz / 2);
            }

            if (closedir(dire) < 0)
            {
                perror("Error closing directory");
                return;
            }

            dire = opendir(argument);

            if (dire == NULL)
            {
                perror("Couldnt open directory");
            }

            next_file = readdir(dire);
            while ((next_file != NULL))
            {
                strcpy(file_name, next_file->d_name);
                if (!a_flag && file_name[0] == '.')
                {
                    next_file = readdir(dire);
                    continue;
                }

                if (!l_flag)
                {
                    printf("%s\n", next_file->d_name);
                    next_file = readdir(dire);
                    continue;
                }
                char file_path[10000];
                strcpy(file_path, argument);
                strcat(file_path, "/");
                strcat(file_path, file_name);
                print_file_data(file_name, file_path);
                next_file = readdir(dire);
            }
            if (free_flag)
                free(argument);
            free(file_name);
            if (closedir(dire) < 0)
            {
                perror("Error closing directory");
                return;
            }
            continue;
        }

        //Code for a file
        if (dir_newline_flag)
        {
            printf("\n");
        }
        int length = strlen(argument);
        int break_index = 0;
        for (int x = 0; x < length; x++)
            if (argument[x] == '/')
                break_index = x + 1;

        char *file_name = (char *)calloc(1000, sizeof(char));
        strcpy(file_name, argument + break_index);
        if (l_flag)
            print_file_data(argument, argument);
        else
            printf("%s\n", argument);

        free(file_name);
        if (free_flag)
            free(argument);
        continue;
    }
}
