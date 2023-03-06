/* paths.c
   Copyright (c) 2023 bellrise */

#include <libgen.h>
#include <limits.h>
#include <mcc/paths.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool path_is_c_file(const char *path)
{
	return !strcmp(path + strlen(path) - 2, ".c");
}

bool path_is_mocha_file(const char *path)
{
	return !strcmp(path + strlen(path) - 3, ".ff");
}

bool path_is_object_file(const char *path)
{
	return !strcmp(path + strlen(path) - 2, ".o");
}

char *path_tmp_object(const char *tip)
{
	char *p = malloc(PATH_MAX);
	char *base = strdup(tip);
	int iter = 0;

	/*
	 * In order to generate a temporary name which does not clash with
	 * existing file names, we add a '.number' suffix onto the basename
	 * until we get a file name that does not exist yet.
	 */

	while (1) {
		snprintf(p, PATH_MAX, "/tmp/mcc.%s.%d.o", basename(base),
			 iter++);
		if (access(p, F_OK))
			break;
	}

	free(base);
	return p;
}

int path_copy(const char *to, const char *from)
{
	char buf[BUFSIZ];
	size_t n;
	FILE *rfile;
	FILE *wfile;

	rfile = fopen(from, "rb");
	if (!rfile)
		return 1;

	wfile = fopen(to, "wb");
	if (!wfile) {
		fclose(rfile);
		return 1;
	}

	while ((n = fread(buf, 1, BUFSIZ, rfile)))
		fwrite(buf, 1, n, wfile);

	fclose(rfile);
	fclose(wfile);
	return 0;
}

void path_dump_file(FILE *f)
{
	char buf[BUFSIZ];
	size_t n;

	while ((n = fread(buf, 1, BUFSIZ, f)))
		fwrite(buf, 1, n, stdout);
}
