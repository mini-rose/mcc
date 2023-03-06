/* errmsg.c
   Copyright (c) 2023 bellrise */

#include <mcc/errmsg.h>
#include <mcc/file.h>
#include <mcc/settings.h>
#include <mcc/term.h>
#include <mcc/tokenize.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static void errmsg_impl(char *fmt, va_list args)
{
	fprintf(stderr, "mcc: %serror: %s", color_err(), color_bold_white());
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "%s\n", color_end());
}

void errmsg(char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	errmsg_impl(fmt, args);

	va_end(args);
	exit(1);
}

void warnmsg(char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	fprintf(stderr, "mcc: %swarn: %s", color_warn(), color_end());
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "%s\n", color_end());

	va_end(args);
}

void infomsg(char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	if (settings_global()->verbose) {
		fprintf(stderr, "mcc: ");
		vfprintf(stderr, fmt, args);
		fprintf(stderr, "%s\n", color_end());
	}

	va_end(args);
}

const char *errstr(enum error_code e)
{
	struct err_entry
	{
		int id;
		const char *msg;
	};

	static const struct err_entry error_strs[] = {
	    {E_UNTERMSTR, "unterminated string"}};
	static const int n = sizeof(error_strs) / sizeof(*error_strs);

	if ((int) e >= n)
		return "???";
	return error_strs[e].msg;
}

void errsrc(struct file *source, struct token *tok, enum error_code e)
{
	char *p = source->source;
	int line = 0;

	while (p != tok->val) {
		if (*p == '\n')
			line++;
		p++;
	}

	fprintf(stderr, "mcc: %serror%s[%d]: %s%s\n", color_err(), color_end(),
		e, color_bold_white(), errstr(e));

	exit(1);
}
