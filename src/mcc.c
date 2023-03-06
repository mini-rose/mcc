/* mcc.c
   Copyright (c) 2023 bellrise */

#include <mcc/args.h>
#include <mcc/mcc.h>
#include <mcc/settings.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	settings_defaults();
	args_parse(argc, argv);
	mcc_run();
}
