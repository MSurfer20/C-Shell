#include "baywatch.h"
#include "headers.h"

void diez(const char *s)
{
    // perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawModez()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        diez("tcsetattr");
}

/**
 * Enable row mode for the terminal
 * The ECHO feature causes each key you type to be printed to the terminal, so you can see what you’re typing.
 * Terminal attributes can be read into a termios struct by tcgetattr().
 * After modifying them, you can then apply them to the terminal using tcsetattr().
 * The TCSAFLUSH argument specifies when to apply the change: in this case, it waits for all pending output to be written to the terminal, and also discards any input that hasn’t been read.
 * The c_lflag field is for “local flags”
 */
void enableRawModez()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        diez("tcgetattr");
    atexit(disableRawModez);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        diez("tcsetattr");
}

/**
 * stdout and stdin are buffered we disable buffering on that
 * After entering in raw mode we read characters one by one
 * Up arrow keys and down arrow keys are represented by 3 byte escape codes
 * starting with ascii number 27 i.e. ESC key
 * This way we interpret arrow keys
 * Tabs are usually handled by the term, but here we are simulating tabs for the sake of simplicity
 * Backspace move the cursor one control character to the left
 * @return
 */

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

    int pid = fork();
    if (pid < 0)
    {
        perror("Error in parallelizing");
        return;
    }
    else if (pid == 0)
    {
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
            sleep(time);
            fclose(fil);
        }
    }
    else
    {
        char c;
        setbuf(stdout, NULL);
        enableRawModez();
        while (read(STDIN_FILENO, &c, 1) == 1)
        {
            if (iscntrl(c))
            {
            }
            else
            {
                if (c == 113)
                {
                    if (kill(pid, SIGTERM) < 0)
                        kill(pid, SIGKILL);
                    return;
                }
                break;
            }
        }
        disableRawModez();
    }
}

void baywatch(int time, int command_id)
{
    if (command_id == 0)
        interrupt(time);
}