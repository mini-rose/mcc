/* mcc.c - mocha compiler
   Copyright (c) 2023 bellrise */

#include <mcc/alloc.h>
#include <mcc/args.h>
#include <mcc/mcc.h>
#include <mcc/settings.h>
#include <stdio.h>
#include <stdlib.h>

void exit_routines()
{
	if (settings_global()->x_alloc_stat)
		alloc_dump_stats();
	slab_deinit_global();
}

int main(int argc, char **argv)
{
	slab_init_global(false);
	atexit(exit_routines);

	settings_defaults();
	args_parse(argc, argv);
	mcc_run();
}
