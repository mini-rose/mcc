/* err.c - error functions
   Copyright (c) 2023 mini-rose */

#include "mcc.h"

#include <stdarg.h>

void err_at(struct mapped_file *file, char *pos, int len, const char *fmt, ...)
{
    va_list args;

    const char *p = file->source;
    const char *q;
    const char *r;
    int spaces = 0;
    int line = 1;

    va_start(args, fmt);

    while (p != pos) {
        if (*p == '\n')
            line++;
        p++;
    }

    p = pos;
    while (p != file->source) {
        if (*(p - 1) == '\n')
            break;
        p--;
    }

    q = p;
    while (*(q + 1) != EOF) {
        if (*(q + 1) == '\n')
            break;
        q++;
    }

    fprintf(stderr, "mcc: \033[1;31merror\033[0m in %s:\n", file->path);
    fprintf(stderr, "    |\n% 3d | %.*s", line, (int) (pos - p), p);
    fprintf(stderr, "\033[31m%.*s\033[0m%.*s\n    | ", len, pos,
            (int) ((q - p) - (q - pos + len)), pos + len);

    r = p;
    while (r != pos) {
        spaces += 1;
        if (*r == '\t')
            spaces += 3;
        r++;
    }

    for (int i = 0; i < spaces; i++)
        fputc(' ', stderr);

    fputs("\033[31m", stderr);
    for (int i = 0; i < len; i++)
        fputc('^', stderr);
    fputc('\n', stderr);

    for (int i = 0; i < spaces + 6; i++)
        fputc(' ', stderr);

    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n\n");

    va_end(args);
    exit(1);
}

void die(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    fprintf(stderr, "mcc: \033[1;31merror: \033[1;39m");
    vfprintf(stderr, fmt, args);
    fputs("\033[0m\n", stderr);

    va_end(args);

    exit(1);
}
