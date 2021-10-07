#include "baywatch.h"
#include "headers.h"

void interrupt(int time)
{
    FILE *fil = fopen("/proc/interrupts", "r");
    if (fil == NULL)
    {
        perror("Error opening interrupts file");
        return;
    }
    char line[1000];
    fgets(line, 1000, fil);
    printf("%s\n", line);
    while (true)
    {
        FILE *fil = fopen("/proc/interrupts", "r");
        if (fil == NULL)
        {
            perror("Error opening interrupts file");
            return;
        }
        char line[1000];
        fgets(line, 1000, fil);
        fgets(line, 1000, fil);
        fgets(line, 1000, fil);
        // printf("%s\n", line);
        int line_len = strlen(line);
        bool flag = false;
        for (int x = 0; x < line_len; x++)
        {
            if (!flag)
            {
                if (line[x] == ':')
                    flag = true;
                printf(" ");
            }
            else if (line[x] >= '0' && line[x] <= '9')
                printf("%c", line[x]);
            else if (line[x] == '\t' || line[x] == ' ')
                printf("%c", line[x]);
            else
            {
                // printf("%c", line[x]);
                printf("\n");
                break;
            }
        }

        fclose(fil);
        clock_t start_time = clock();
        while (1.0 * (clock() - start_time) / CLOCKS_PER_SEC < time)
        {
            fd_set rfds;
            struct timeval tv;
            int retval;
            /* Watch stdin (fd 0) to see when it has input. */
            FD_ZERO(&rfds);
            FD_SET(0, &rfds);

            /* Wait up to five seconds. */
            tv.tv_sec = 0.8;
            tv.tv_usec = 0;

            retval = select(1, &rfds, NULL, NULL, &tv);
            if (retval == -1)
            {
                perror("select()");
                return;
            }
            else if (retval)
            {
                printf("YAAAAYY");
                char ch = getchar();
                printf("YAY%c\n", ch);
                char dump[1000];
                fgets(dump, 1000, stdin);
                if (ch == 'q')
                    return;
            }
        }
    }
}

void baywatch(int time, int command_id)
{
    if (command_id == 0)
        interrupt(time);
}