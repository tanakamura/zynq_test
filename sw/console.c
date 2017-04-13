#include "console.h"
#include "libc.h"

#define BUFSIZE 1024
static char buffer[BUFSIZE];

void
read_console(int *argc, char **argv, int max_argc)
{
    while (1) {
    retry:
        putchar('>');
        putchar(' ');

        for (int i=0; i<1024; i++) {
            buffer[i] = getchar();
            if (buffer[i] == '\r') {
                buffer[i] = '\0';
                putchar('\n');
                goto parse_cmdline;
            }

            putchar(buffer[i]);
            if (buffer[i] == '\b') {
                i-=2;
                if (i < 0) {
                    i = 0;
                }
            }

            if (buffer[i] == '\f') {
                goto retry;
            }
        }
    }

parse_cmdline:;

    int in_space = 1;
    int cur_argc = 0;

    for (int i=0; i<BUFSIZE; i++) {
        if (buffer[i] == '\0') {
            break;
        }

        if (isspace(buffer[i])) {
            if (in_space) {
                /* nop */
            } else {
                buffer[i] = '\0';
            }

            in_space = 1;
        } else {
            if (in_space) {
                if (cur_argc >= max_argc) {
                    break;
                }
                argv[cur_argc] = &buffer[i];
                cur_argc++;
            } else {
                /* nop */
            }

            in_space = 0;
        }
    }

    *argc = cur_argc;
}
