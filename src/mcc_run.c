/* mcc_run.c
   Copyright (c) 2023 bellrise */

#include <mcc/errmsg.h>
#include <mcc/mcc.h>
#include <mcc/paths.h>
#include <mcc/settings.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int mcc_run()
{
	struct settings *s = settings_global();
	struct strlist objects = {0};
	char *p;

	if (!s->inputs.len) {
		errmsg("missing input filename");
	}

	if (s->to_object && s->inputs.len > 1)
		errmsg("cannot compile a single object from multiple sources");

	/* Compile files to objects. */
	for (int i = 0; i < s->inputs.len; i++) {
		if (path_is_mocha_file(s->inputs.strs[i])) {
			/* Regular mocha source file */
			p = path_tmp_object(s->inputs.strs[i]);

			if (mcc_compile(s->inputs.strs[i], p)) {
				errmsg("failed to compile %s",
				       s->inputs.strs[i]);
			}

			strlist_append(&objects, p);

		} else if (path_is_object_file(s->inputs.strs[i])) {
			/* Object file */
			strlist_append(&objects, s->inputs.strs[i]);

		} else if (path_is_c_file(s->inputs.strs[i])) {
			/* C source file */
			p = path_tmp_object(s->inputs.strs[i]);

			if (mcc_compile_c(s->inputs.strs[i], p)) {
				errmsg("failed to compile %s",
				       s->inputs.strs[i]);
			}

			strlist_append(&objects, p);

		} else {
			warnmsg("don't know what to do with '%s'",
				s->inputs.strs[i]);
		}
	}

	if (s->to_object) {
		/*
		 * Move the compiled object from the temporary directory into
		 * the working directory or the destination which the user
		 * chose.
		 */
		path_copy(s->output ? s->output : "a.o", objects.strs[0]);
		goto end;
	}

	mcc_link(s->to_shared ? MCC_LINK_SHARED : MCC_LINK_EXEC, &objects,
		 s->output ? s->output : "a.out");

end:
	for (int i = 0; i < objects.len; i++)
		remove(objects.strs[i]);
	return 0;
}
