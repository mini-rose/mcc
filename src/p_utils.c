/* p_utils.c
   Copyright (c) 2023 bellrise */

#include <mcc/errmsg.h>
#include <mcc/parser.h>
#include <stdarg.h>

static struct token end_tok = {.len = 0, .val = "", .type = T_END};

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
	if (p->tokens->iter >= p->tokens->len)
		return &end_tok;
	return p->tokens->tokens[p->tokens->iter++];
}

struct token *p_after_tok(struct p_context *p, struct token *from)
{
	for (int i = 0; i < p->tokens->len - 1; i++) {
		if (p->tokens->tokens[i] == from)
			return p->tokens->tokens[i + 1];
	}

	return &end_tok;
}

struct token *p_before_tok(struct p_context *p, struct token *from)
{
	for (int i = 1; i < p->tokens->len; i++) {
		if (p->tokens->tokens[i] == from)
			return p->tokens->tokens[i - 1];
	}

	return &end_tok;
}

void p_set_pos_tok(struct p_context *p, struct token *at)
{
	for (int i = 0; i < p->tokens->len; i++) {
		if (p->tokens->tokens[i] != at)
			continue;

		p->tokens->iter = i + 1;
		break;
	}
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
