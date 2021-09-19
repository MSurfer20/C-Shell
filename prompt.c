#include "prompt.h"
#include "headers.h"

char *process_path(char *pwd, char *home_dir)
{
    // printf("PROMPT %s %s\n", pwd, home_dir);
    char temp_path[10000];
    memset(temp_path, '\0', sizeof(temp_path));
    char *processed_path = calloc(10000, sizeof(char));
    long long home_dir_length = strlen(home_dir);
    long long pwd_length = strlen(pwd);

    if (pwd_length < home_dir_length)
    {
        strcpy(processed_path, pwd);
        return processed_path;
    }

    strncpy(temp_path, pwd, home_dir_length);
    if (!(pwd[home_dir_length] == '\0' || pwd[home_dir_length] == '/'))
    {
        strcpy(processed_path, pwd);
        return processed_path;
    }
    if (strcmp(temp_path, home_dir) == 0)
    {
        strcat(processed_path, "~");
        strcat(processed_path, pwd + home_dir_length);
        char *return_string = calloc((strlen(processed_path) + 5), sizeof(char));
        strcpy(return_string, processed_path);
        free(processed_path);
        return return_string;
    }
    strcpy(processed_path, pwd);
    return processed_path;
}
void prompt(char *pwd, char *home_dir)
{
    int userid = getuid();
    struct passwd *userstruct = getpwuid(userid);

    char *path_name = process_path(pwd, home_dir);
    if (userstruct == NULL)
    {
        perror("Error in getting username\n");
        return;
    }
    char *username = userstruct->pw_name;
    char hostname[10000];
    int hostname_status = gethostname(hostname, 10000);
    if (hostname_status == -1)
    {
        perror("Hostname error\n");
        return;
    }
    printf("<%s@%s:%s>", username, hostname, path_name);
    free(path_name);
}
