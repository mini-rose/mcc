/* mcc_compile.c
   Copyright (c) 2023 bellrise */

#include <mcc/errmsg.h>
#include <mcc/mcc.h>
#include <mcc/paths.h>
#include <mcc/settings.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int mcc_compile(const char *input, const char *output)
{
	infomsg("compile %s -> %s", input, output);
	return 1;
}

int mcc_compile_c(const char *input, const char *output)
{
	char cmd[BUFSIZ];
	FILE *proc;
	int c;

	infomsg("compile-c %s -> %s", input, output);

	snprintf(cmd, BUFSIZ, "%s -c -o %s %s", MCC_CC, output, input);

	if (settings_global()->verbose)
		infomsg("issuing %s", cmd);

	proc = popen(cmd, "r");
	if ((c = fgetc(proc)) != EOF) {
		fputc(c, stdout);
		path_dump_file(proc);
		return 1;
	}

	pclose(proc);
	return 0;
}
