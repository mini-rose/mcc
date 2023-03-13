/* mcc/settings.h
   Copyright (c) 2023 bellrise */

#pragma once

#include <mcc/strlist.h>
#include <stdbool.h>

struct settings
{
	struct strlist inputs;
	char *output;
	char *helpdir;
	char *cc;
	char *ld;
	bool verbose;
	bool use_colors;
	bool to_object;
	bool to_shared;

	/* extra settings */
	bool x_tree;
	bool x_alloc_stat;

	/* link settings */
	char *ldd;
};

void settings_defaults();
struct settings *settings_global();
