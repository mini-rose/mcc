/* errmsg.c
   Copyright (c) 2023 bellrise */

#include <ctype.h>
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

void errsrc(struct file *source, struct token *tok, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	errsrc_v(source, tok, fmt, args);
}

void errsrc_v(struct file *source, struct token *tok, const char *fmt,
	      va_list args)
{
	const char *p = source->source;
	const char *start;
	const char *end;
	int line = 1;
	int spaces = 0;
	int tabs = 0;
	int offset;

	/* Find the line number */
	while (p != tok->val) {
		if (*p == '\n')
			line++;
		p++;
	}

	/* Walk back to the start of the line */
	start = tok->val;
	while (start >= source->source) {
		if (*start == '\n') {
			start++;
			break;
		}

		start--;
	}

	/* Walk to the end of the line */
	end = tok->val;
	while (*end) {
		if (*end == '\n')
			break;
		end++;
	}

	/* Count the whitespace before the beginning of the line */
	p = start;
	while (p != end) {
		if (!isspace(*p))
			break;

		(*p == '\t') ? tabs++ : spaces++;
		p++;
	}

	fprintf(stderr, "mcc: %serror%s in %s: %s", color_err(), color_end(),
		source->path, color_bold_white());
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "%s\n", color_end());

	/* Print the source, highlighting the error part */
	offset = fprintf(stderr, "% 4d", line);
	fprintf(stderr, " | %.*s%s%.*s%s%.*s\n", (int) (tok->val - start),
		start, color_err(), (int) (tok->len), tok->val, color_end(),
		(int) (end - (tok->val + tok->len)), tok->val + tok->len);

	for (int i = 0; i < offset; i++)
		fputc(' ', stderr);

	offset = tok->val - start;
	fprintf(stderr, " | %s", color_err());

	for (int i = 0; i < tabs; i++)
		fputc('\t', stderr);
	for (int i = 0; i < spaces + offset; i++)
		fputc(' ', stderr);

	fputc('^', stderr);
	for (int i = 0; i < tok->len - 1; i++)
		fputc('~', stderr);

	fprintf(stderr, "%s\n", color_end());

	exit(1);
}
