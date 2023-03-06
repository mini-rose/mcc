/* mcc.c
   Copyright (c) 2023 bellrise */

#include <mcc/alloc.h>
#include <mcc/args.h>
#include <mcc/mcc.h>
#include <mcc/settings.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	slab_init_global(false);

	settings_defaults();
	args_parse(argc, argv);
	mcc_run();

	slab_deinit_global();
}
