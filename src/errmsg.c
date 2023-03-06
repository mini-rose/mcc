/* errmsg.c
   Copyright (c) 2023 bellrise */

#include <mcc/errmsg.h>
#include <mcc/settings.h>
#include <mcc/term.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void errmsg(char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	fprintf(stderr, "mcc: %serror: %s", color_err(), color_bold_white());
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "%s\n", color_end());

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
