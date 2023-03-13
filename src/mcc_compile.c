/* mcc_compile.c
   Copyright (c) 2023 bellrise */

#include <mcc/alloc.h>
#include <mcc/emit.h>
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
	const struct emit_target *emitter;
	struct token_list *tokens;
	struct module *module;
	struct settings *s;
	struct file *fil;
	char *emitted;

	s = settings_global();

	infomsg("compile %s -> %s", input, output);

	fil = file_map(input);
	if (!fil)
		errmsg("failed to open source file `%s`", input);

	tokens = tokenize(fil);
	module = mcc_parse(tokens);

	if (s->x_tree)
		p_node_dump(module->ast);

	/* Select the proper emitter. */
	emitter = emit_for_target(s->target);
	if (!emitter) {
		errmsg("invalid target `%s`, try `--target list`", s->target);
	}

	emitted = emitter->fn(module);

	/* Only compile, do not assemble. */
	if (s->to_target) {
		char *outname = s->output;
		if (!outname) {
			outname = slab_alloc(32);
			snprintf(outname, 32, "a.%s", emitter->file_suffix);
		}

		path_copy(outname, emitted);
		file_unmap(fil);
		remove(emitted);
		exit(0);
	}

	file_unmap(fil);
	remove(emitted);
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
