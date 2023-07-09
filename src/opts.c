/* opts.c - command line options
   Copyright (c) 2023 mini-rose */

#include "mcc.h"

#include <getopt.h>

static void short_usage();
static void usage();
static void version();
static void set_default_options(struct options *opts);

struct options *options_parse(int argc, char **argv)
{
    struct options *opts;
    int opt_index;
    int c;

    static struct option long_opts[] = {{"help", no_argument, 0, 'h'},
                                        {"version", no_argument, 0, 'v'},
                                        {0, 0, 0, 0}};

    /* Set default options */
    opt_index = 0;
    opts = malloc(sizeof(*opts));
    set_default_options(opts);

    if (argc == 1) {
        short_usage();
        exit(0);
    }

    while (1) {
        c = getopt_long(argc, argv, "hv", long_opts, &opt_index);
        if (c == -1)
            break;

        switch (c) {
        case 'h':
            usage();
            exit(0);
        case 'v':
            version();
            exit(0);
        }
    }

    if (optind < argc)
        opts->filename = strdup(argv[optind++]);

    return opts;
}

void options_free(struct options *opts)
{
    free(opts->filename);
}

void short_usage()
{
    puts("usage: mcc [-h] [opts]... [file]");
}

void usage()
{
    short_usage();
    puts("Compile, build & link mocha source code. Use '-' for the file\n"
         "path to read from stdin\n");
    puts("  -h, --help            help page\n"
         "  -v, --version         show version");
}

void version()
{
    printf("mcc %d.%d\n", MCC_MAJOR, MCC_MINOR);
}

void set_default_options(struct options *opts)
{
    opts->filename = strdup("-");
}
