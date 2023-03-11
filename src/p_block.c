/* p_block.c
   Copyright (c) 2023 bellrise */

#include <mcc/errmsg.h>
#include <mcc/parser.h>

void p_parse_block(struct p_context *p, struct node *block)
{
	struct token *tok;
	struct node *parent;

	tok = p_cur_tok(p);
	if (tok->type != T_LBRACE)
		p_err(p, "expected opening brace in block");

	parent = p->here;
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

	/* Set the current block to our block. */
	p->here = block;

	while (tok->type != T_RBRACE && tok->type != T_END) {
		if (tok->type == T_NEWLINE) {
			tok = p_next_tok(p);
			continue;
		}

		p_parse_statement(p);
		tok = p_cur_tok(p);
	}

	p->here = parent;
}
