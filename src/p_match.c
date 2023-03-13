/* p_match.c - matching rules for token groups
   Copyright (c) 2023 bellrise */

#include <mcc/parser.h>
#include <stdarg.h>

static struct token *match_class(struct p_context *p, struct token *tok,
				 enum match_class class)
{
	struct token *tmp_tok;

	if (class == CLASS_LVALUE) {
		if (tok->type == T_IDENT)
			return p_after_tok(p, tok);

		if (tok->type != T_MUL && tok->type != T_AND)
			return NULL;

		tok = p_after_tok(p, tok);
		if (tok->type == T_IDENT)
			return p_after_tok(p, tok);

		return NULL;
	}

	if (class == CLASS_RVALUE) {
		tmp_tok = match_class(p, tok, CLASS_LVALUE);
		if (tmp_tok)
			return tmp_tok;

		if (tok->type == T_NUMBER || tok->type == T_STRING)
			return p_after_tok(p, tok);

		return NULL;
	}

	if (class == CLASS_ASSIGN_OP) {
		switch (tok->type) {
		case T_ASS:
		case T_ADDA:
		case T_SUBA:
		case T_MULA:
		case T_DIVA:
		case T_MODA:
			return p_after_tok(p, tok);
		default:
			return NULL;
		}
	}

	return NULL;
}

bool p_match(struct p_context *p, int n, ...)
{
	struct token *tok = p_cur_tok(p);
	va_list args;
	int class;

	va_start(args, n);

	for (int i = 0; i < n; i++) {
		class = va_arg(args, int);

		if (class >= 0) {
			/* Exact token type */
			if ((int) tok->type != class)
				return false;
			tok = p_after_tok(p, tok);
		} else {
			/* Match class */
			tok = match_class(p, tok, class);
			if (!tok)
				return false;
		}
	}

	va_end(args);

	return true;
}
