/* args.c
   Copyright (c) 2023 bellrise */

#include <getopt.h>
#include <mcc/alloc.h>
#include <mcc/args.h>
#include <mcc/emit.h>
#include <mcc/errmsg.h>
#include <mcc/help.h>
#include <mcc/mcc.h>
#include <mcc/settings.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void long_opt(const char *opt, const char *value);
static void extra_opt(const char *opt);
static void list_targets();
static void version();

void args_parse(int argc, char **argv)
{
	struct settings *settings;
	int opt_index;
	int c;

	static struct option long_opts[] = {
	    {"cc", required_argument, 0, 0},
	    {"color", optional_argument, 0, 0},
	    {"help", optional_argument, 0, 'h'},
	    {"helpdir", required_argument, 0, 0},
	    {"output", required_argument, 0, 'o'},
	    {"ld", required_argument, 0, 0},
	    {"ldd", required_argument, 0, 0},
	    {"shared", no_argument, 0, 's'},
	    {"target", required_argument, 0, 't'},
	    {"verbose", no_argument, 0, 'V'},
	    {"version", no_argument, 0, 'v'},
	    {0, 0, 0, 0}};

	settings = settings_global();
	opt_index = 0;

	if (argc == 1) {
		help_short();
		exit(0);
	}

	while (1) {
		c = getopt_long(argc, argv, "cho:st:VvX:", long_opts,
				&opt_index);
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
			settings->output = slab_strdup(optarg);
			break;
		case 's':
			settings->to_shared = true;
			break;
		case 't':
			if (!strcmp(optarg, "list"))
				list_targets();
			settings->target = slab_strdup(optarg);
			break;
		case 'V':
			settings->verbose = true;
			break;
		case 'v':
			version();
			exit(0);
			break;
		case 'X':
			extra_opt(optarg);
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

	if (!strcmp(opt, "cc")) {
		if (value)
			s->cc = strdup(value);
	}

	if (!strcmp(opt, "color")) {
		if (!value)
			s->use_colors = true;
		else {
			if (!strcmp(value, "never") || !strcmp(value, "no"))
				s->use_colors = false;
		}
	}

	if (!strcmp(opt, "helpdir")) {
		if (!value)
			errmsg("missing dirname after --helpdir");
		s->helpdir = slab_strdup(value);
	}

	if (!strcmp(opt, "ld")) {
		if (value)
			s->ld = strdup(value);
	}

	if (!strcmp(opt, "ldd")) {
		if (!value)
			errmsg("missing dynamic linker path after --ldd");
		s->ldd = slab_strdup(value);
	}
}

static void extra_opt(const char *opt)
{
	struct settings *s = settings_global();

	if (!strcmp(opt, "tree"))
		s->x_tree = true;
	else if (!strcmp(opt, "alloc-stat"))
		s->x_alloc_stat = true;
}

static void version()
{
	printf("mcc %d.%d\n", MCC_MAJOR, MCC_MINOR);
}

static void list_targets()
{
	const char **targets;
	int n;

	targets = emit_target_list(&n);

	printf("Possible targets for `--target <tgt>`:\n");
	for (int i = 0; i < n; i++)
		printf("  %s\n", targets[i]);

	exit(0);
}
