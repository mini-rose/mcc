/* mcc - mocha compiler
   Copyright (c) 2023 mini-rose */

#include "mcc.h"

int main(int argc, char **argv)
{
    struct mapped_file *source;
    struct token_list *tokens;
    struct options *opts;

    opts = options_parse(argc, argv);

    /*
     * 1. Load the source file
     * 2. Tokenize (lex)
     * 3. Parse
     */

    if (!strcmp(opts->filename, "-"))
        source = file_from_stdin();
    else
        source = file_map(opts->filename);

    if (!source)
        die("no file named '%s'", opts->filename);

    tokens = lex(source);

    file_free(source);
    options_free(opts);
}
