/* mcc_compile.c
   Copyright (c) 2023 bellrise */

#include <mcc/errmsg.h>
#include <mcc/file.h>
#include <mcc/mcc.h>
#include <mcc/parser.h>
#include <mcc/paths.h>
#include <mcc/settings.h>
#include <mcc/tokenize.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int mcc_compile(const char *input, const char *output)
{
	struct token_list *tokens;
	struct module *module;
	struct file *fil;

	infomsg("compile %s -> %s", input, output);

	fil = file_map(input);
	if (!fil)
		errmsg("failed to open source file `%s`", input);

	tokens = tokenize(fil);
	module = mcc_parse(tokens);

	if (settings_global()->x_tree)
		p_node_dump(module->ast);

	file_unmap(fil);
	return 0;
}

int mcc_compile_c(const char *input, const char *output)
{
	char cmd[BUFSIZ];
	FILE *proc;
	int c;

	infomsg("compile-c %s -> %s", input, output);

	snprintf(cmd, BUFSIZ, "%s -c -o %s %s", settings_global()->cc, output,
		 input);

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
