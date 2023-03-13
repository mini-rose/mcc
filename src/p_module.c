/* p_module.c
   Copyright (c) 2023 bellrise */

#include <mcc/alloc.h>
#include <mcc/errmsg.h>
#include <mcc/parser.h>

void p_parse_module(struct p_context *p)
{
	struct token *tok;

	/*
	 * Parse the top-level module, where we kind find:
	 *  - function
	 *  - use
	 *  - built-in call
	 */

	p->here = p->module->ast;
	tok = p_next_tok(p);

	do {
		tok = p_cur_tok(p);

		if (tok->type == T_FN) {
			/* function-expr */
			p_parse_fn_decl(p);

		} else if (tok->type == T_USE) {
			/* use-expr */
			p_parse_use(p);

		} else {
			p_err(p,
			      "invalid expression, expected function or use");
		}

		if ((tok = p_next_tok(p))->type == T_NEWLINE)
			tok = p_next_tok(p);

	} while (tok->type != T_END);

	/*
	 * After parsing the top-level declarations, we want to return to parse
	 * the function implementations. This is done because we want to be able
	 * to access all the top-level types and functions without the need to
	 * define them before.
	 */

	struct node *nod = p->module->ast->child;
	while (nod) {
		if (nod->kind == NODE_FN_DECL)
			p_parse_fn_def(p, nod->fn_decl);
		nod = nod->next;
	}
}

struct module *p_module_create()
{
	struct module *m;

	m = slab_alloc(sizeof(*m));
	m->ast = p_node_create(NULL);
	m->ast->kind = NODE_MODULE;
	m->ast->module = m;

	return m;
}
