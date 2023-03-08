/* mcc_link.c
   Copyright (c) 2023 bellrise */

#include <mcc/errmsg.h>
#include <mcc/mcc.h>
#include <mcc/paths.h>
#include <mcc/settings.h>
#include <stdio.h>
#include <string.h>

int mcc_link(enum mcc_link_mode mode, struct strlist *inputs,
	     const char *output)
{
	char cmd[MCC_CMDSIZ];
	FILE *proc;
	int c;

	if (settings_global()->verbose) {
		for (int i = 0; i < inputs->len; i++)
			infomsg("linking %s", inputs->strs[i]);
	}

	memset(cmd, 0, MCC_CMDSIZ);
	snprintf(cmd, MCC_CMDSIZ, "%s -o %s -dynamic-linker %s %s",
		 settings_global()->ld, output, settings_global()->ldd,
		 settings_global()->to_shared ? "-shared " : "");

	/*
	 * For an executable, we have to link some libc-related runtime objects,
	 * namely crt1, ctri and ctrn.
	 */
	if (mode == MCC_LINK_EXEC)
		strcat(cmd, "/lib/crt1.o /lib/crti.o ");

	for (int i = 0; i < inputs->len; i++) {
		strcat(cmd, inputs->strs[i]);
		strcat(cmd, " ");
	}

	if (mode == MCC_LINK_EXEC)
		strcat(cmd, "/lib/crtn.o ");

	/* Link against libc, and dump everything into stdout. */
	strcat(cmd, "-lc 2>&1");

	if (settings_global()->verbose)
		infomsg("issuing %s", cmd);

	/* Call the actual linker. */
	proc = popen(cmd, "r");
	if ((c = fgetc(proc)) != EOF) {
		fputc(c, stdout);
		path_dump_file(proc);
	}

	pclose(proc);
	return 0;
}
