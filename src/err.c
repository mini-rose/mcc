#include "mcc.h"

void err_at(struct mapped_file *file, char *pos, int len, char *fmt, ...)
{
    const char *p = file->source;
    int line = 1;
}
