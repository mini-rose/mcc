/* debug.c - additional debug tools
   Copyright (c) 2023 mini-rose */

#include "mcc.h"

#include <ctype.h>

void dump_bytes(void *addr, int len)
{
    unsigned char *p;
    int total_lines = len / 16;
    int spaces;
    int chunk = 16;

    total_lines = len / 16;
    total_lines += len % 16 ? 1 : 0;

    for (int i = 0; i < total_lines; i++) {
        p = addr + (i * 16);

        chunk = 16;
        if (total_lines - 1 == i)
            chunk = len - (i * 16);

        /* Print bytes */
        printf("%p : ", p);
        for (int j = 0; j < chunk; j++) {
            printf("%02hhx", p[j]);
            if (j % 2 != 0)
                fputc(' ', stdout);
        }

        /* Print spaces */
        spaces = (16 - chunk) * 2 + (16 - chunk) / 2 + (chunk % 2);
        for (int j = 0; j < spaces; j++)
            fputc(' ', stdout);

        /* Print ASCII chars */
        fputs(": ", stdout);
        for (int j = 0; j < chunk; j++) {
            printf("%c", isprint(p[j]) ? p[j] : '.');
        }

        fputc('\n', stdout);
    }
}
