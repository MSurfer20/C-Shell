#include "ls.h"
#include "headers.h"

void print_file_data(char *file_name, char *file_path)
{

    struct stat file_stats;
    int stat_result = stat(file_path, &file_stats);
    if (stat_result == -1)
    {
        char err_buf[1100];
        sprintf(err_buf, "Error reading %s", file_name);
        perror(err_buf);
        return;
    }
    if (S_ISDIR(file_stats.st_mode))
        printf("d");
    else
        printf("-");
    if (file_stats.st_mode & S_IRUSR)
        printf("r");
    else
        printf("-");
    if (file_stats.st_mode & S_IWUSR)
        printf("w");
    else
        printf("-");
    if (file_stats.st_mode & S_IXUSR)
        printf("x");
    else
        printf("-");
    if (file_stats.st_mode & S_IRGRP)
        printf("r");
    else
        printf("-");
    if (file_stats.st_mode & S_IWGRP)
        printf("w");
    else
        printf("-");
    if (file_stats.st_mode & S_IXGRP)
        printf("x");
    else
        printf("-");
    if (file_stats.st_mode & S_IROTH)
        printf("r");
    else
        printf("-");
    if (file_stats.st_mode & S_IWOTH)
        printf("w");
    else
        printf("-");
    if (file_stats.st_mode & S_IXOTH)
        printf("x");
    else
        printf("-");
    printf(" ");
    printf(" %3d", (int)file_stats.st_nlink);

    struct passwd *pw = getpwuid(file_stats.st_uid);
    if (pw == NULL)
    {
        char err_buf[1100];
        sprintf(err_buf, "\rError getting owner name of %s\t\t\t", file_name);
        perror(err_buf);
        return;
    }
    printf("  %10s", pw->pw_name);
    struct group *grp = getgrgid(file_stats.st_gid);
    if (grp == NULL)
    {
        char err_buf[1100];
        sprintf(err_buf, "\rError getting owner name of %s\t\t\t", file_name);
        perror(err_buf);
        return;
    }
    printf("  %10s", grp->gr_name);
    printf(" %10d", (int)file_stats.st_size);
    struct tm *time = localtime(&file_stats.st_mtim.tv_sec);
    char month_array[12][20] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    printf(" %s", month_array[time->tm_mon]);
    printf(" %2d", time->tm_mday);
    printf(" %d", 1900 + time->tm_year);
    printf(" %s", file_name);
    printf("\n");
}

void ls(char *home_dir, bool a_flag, bool l_flag, char *argument)
{
    if (strlen(argument) == 0)
        argument[0] = '.';
    if (argument[0] == '~')
    {
        char *new_argument = calloc(1000, sizeof(char));
        strcpy(new_argument, home_dir);
        strcat(new_argument, argument + 1);
        argument = new_argument;
    }

    //Testing if a file or directory
    struct stat stats;
    // printf("%s\n", argument);
    int stat_return = stat(argument, &stats);
    if (stat_return != 0)
    {
        perror("Cannot open directory or file");
        return;
    }

    //Code for a directory
    if (S_ISDIR(stats.st_mode))
    {
        struct dirent *next_file;
        DIR *dire = opendir(argument);

        if (dire == NULL)
        {
            perror("Couldnt open directory");
        }

        char *file_name = calloc(1000, sizeof(char));
        if (!dire)
        {
            perror("Opening Directory");
            return;
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
        return;
    }

    //Code for a file
    int length = strlen(argument);
    int break_index = 0;
    for (int x = 0; x < length; x++)
        if (argument[x] == '/')
            break_index = x + 1;

    char *file_name = (char *)calloc(1000, sizeof(char));
    strcpy(file_name, argument + break_index);
    if (l_flag)
        print_file_data(file_name, argument);
    else
        printf("%s\n", file_name);
    return;
}
