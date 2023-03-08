/* p_block.c
   Copyright (c) 2023 bellrise */

#include <mcc/parser.h>

void p_parse_block(struct p_context *p, struct node *block)
{
	struct token *tok;

	tok = p_cur_tok(p);
	if (tok->type != T_LBRACE)
		p_err(p, "expected opening brace");

	tok = p_next_tok(p);
}
