#include "prompt.h"
#include "headers.h"

char *process_path(char *pwd, char *home_dir)
{
    // printf("PROMPT %s %s\n", pwd, home_dir);
    char temp_path[10000];
    char *processed_path = calloc(10000, sizeof(char));
    long long home_dir_length = strlen(home_dir);
    long long pwd_length = strlen(pwd);

    if (pwd_length < home_dir_length)
    {
        return pwd;
    }

    strncpy(temp_path, pwd, home_dir_length);
    if (strcmp(temp_path, home_dir) == 0)
    {
        strcat(processed_path, "~");
        strcat(processed_path, pwd + home_dir_length);
        char *return_string = calloc((strlen(processed_path) + 5), sizeof(char));
        strcpy(return_string, processed_path);
        return return_string;
    }
    return pwd;
}
void prompt(char *pwd, char *home_dir)
{
    int userid = getuid();
    struct passwd *userstruct = getpwuid(userid);

    char *path_name = process_path(pwd, home_dir);
    if (userstruct == NULL)
    {
        perror(("Error in pwid"));
    }
    char *username = userstruct->pw_name;
    char hostname[10000];
    int hostname_status = gethostname(hostname, 10000);
    if (hostname_status == -1)
    {
        perror("Hostname error");
    }
    printf("<%s@%s:%s>", username, hostname, path_name);
}
