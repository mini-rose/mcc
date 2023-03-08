/* p_block.c
   Copyright (c) 2023 bellrise */

#include <mcc/parser.h>

void p_parse_block(struct p_context *p, struct node *block)
{
	struct token *tok;

	tok = p_cur_tok(p);
	if (tok->type != T_LBRACE)
		p_err(p, "expected opening brace in block");

	tok = p_next_tok(p);

	/*
	 * expr-block ::= '{' { statement }* '}'
	 * statement ::= var-decl
	 *             | assignment
	 *             | builtin-call
	 *             | call
	 *             | expr-block
	 *             | rvalue
	 *             | return
	 */

	while (tok->type != T_RBRACE && tok->type != T_END) {
		tok = p_next_tok(p);
		/* TODO: use & implement p_parse_statement */
	}
}
