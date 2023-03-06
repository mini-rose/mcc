/* args.c
   Copyright (c) 2023 bellrise */

#include <getopt.h>
#include <mcc/args.h>
#include <mcc/errmsg.h>
#include <mcc/help.h>
#include <mcc/mcc.h>
#include <mcc/settings.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void long_opt(const char *opt, const char *value);
static void version();

void args_parse(int argc, char **argv)
{
	struct settings *settings;
	int opt_index;
	int c;

	static struct option long_opts[] = {
	    {"color", optional_argument, 0, 0},
	    {"help", optional_argument, 0, 'h'},
	    {"helpdir", required_argument, 0, 0},
	    {"output", required_argument, 0, 'o'},
	    {"shared", no_argument, 0, 's'},
	    {"verbose", no_argument, 0, 'V'},
	    {"version", no_argument, 0, 'v'},
	    {0, 0, 0, 0}};

	settings = settings_global();
	opt_index = 0;

	if (argc == 1) {
		help_short();
		return;
	}

	while (1) {
		c = getopt_long(argc, argv, "cho:sVv", long_opts, &opt_index);
		if (c == -1)
			break;

		switch (c) {
		case 0:
			long_opt(long_opts[opt_index].name, optarg);
			break;
		case 'c':
			settings->to_object = true;
			break;
		case 'h':
			optarg ? help_topic(optarg) : help();
			exit(0);
			break;
		case 'o':
			if (settings->output)
				free(settings->output);
			settings->output = strdup(optarg);
			break;
		case 's':
			settings->to_shared = true;
			break;
		case 'V':
			settings->verbose = true;
			break;
		case 'v':
			version();
			exit(0);
			break;
		}
	}

	while (optind < argc) {
		strlist_append(&settings->inputs, argv[optind++]);
	}
}

static void long_opt(const char *opt, const char *value)
{
	struct settings *s = settings_global();

	if (!strcmp(opt, "color")) {
		if (!value)
			s->use_colors = true;
		else {
			if (!strcmp(value, "never") || !strcmp(value, "no"))
				s->use_colors = false;
		}
	}

	if (!strcmp(opt, "help")) {
		value ? help_topic(value) : help();
		exit(0);
	}

	if (!strcmp(opt, "helpdir")) {
		if (!value)
			errmsg("missing dirname after --helpdir");
		if (s->helpdir)
			free(s->helpdir);
		s->helpdir = strdup(value);
	}

	if (!strcmp(opt, "output")) {
		if (!value)
			errmsg("missing filename after --output");
		s->output = strdup(value);
	}

	if (!strcmp(opt, "shared"))
		s->to_shared = true;

	if (!strcmp(opt, "verbose"))
		s->verbose = true;

	if (!strcmp(opt, "version")) {
		version();
		exit(0);
	}
}

static void version()
{
	printf("mcc %d.%d\n", MCC_MAJOR, MCC_MINOR);
}
