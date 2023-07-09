/* mcc - mocha compiler
   Copyright (c) 2023 mini-rose */

#include "mcc.h"

int main(int argc, char **argv)
{
    struct mapped_file *source;
    struct options *opts;

    opts = options_parse(argc, argv);

    if (!strcmp(opts->filename, "-"))
        source = map_stdin();
    else
        source = map_file(opts->filename);
}
