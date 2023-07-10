/* err.c - error functions
   Copyright (c) 2023 mini-rose */

#include "mcc.h"

#include <stdarg.h>

void err_at(struct mapped_file *file, char *pos, int len, const char *fmt, ...)
{
    const char *p = file->source;
    int line = 1;
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
