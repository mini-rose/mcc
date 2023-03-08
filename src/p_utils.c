/* p_utils.c
   Copyright (c) 2023 bellrise */

#include <mcc/errmsg.h>
#include <mcc/parser.h>
#include <stdarg.h>

void p_err(struct p_context *p, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	errsrc_v(p->module->source, p_cur_tok(p), fmt, args);
}

struct token *p_cur_tok(struct p_context *p)
{
	if (p->tokens->iter == 0)
		return p->tokens->tokens[0];
	return p->tokens->tokens[p->tokens->iter - 1];
}

struct token *p_next_tok(struct p_context *p)
{
	static struct token end_tok = {.len = 0, .val = "", .type = T_END};

	if (p->tokens->iter >= p->tokens->len)
		return &end_tok;
	return p->tokens->tokens[p->tokens->iter++];
}

void p_skip_block(struct p_context *p)
{
	struct token *tok;
	int depth = 0;

	tok = p_cur_tok(p);
	while (tok->type != T_END) {
		if (tok->type == T_LBRACE)
			depth++;
		if (tok->type == T_RBRACE)
			depth--;

		if (!depth)
			break;

		tok = p_next_tok(p);
	}
}
