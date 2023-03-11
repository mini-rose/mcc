/* p_statement.c
   Copyright (c) 2023 bellrise */

#include <mcc/alloc.h>
#include <mcc/parser.h>

typedef void (*st_parse)(struct p_context *p);

static st_parse match_statement(struct p_context *p);

void p_parse_statement(struct p_context *p)
{
	st_parse fn;

	if (!(fn = match_statement(p)))
		p_err(p, "cannot parse statement");

	fn(p);
	p_next_tok(p);
}

void ps_var_decl(struct p_context *p)
{
	struct token *tok = p_cur_tok(p);
	struct p_var *local;
	struct node *node;

	local = p_node_add_local(p->here);
	node = p_node_add_child(p->here);

	node->var = local;
	node->kind = NODE_VAR_DECL;

	local->name = slab_strndup(tok->val, tok->len);
	local->place = tok;

	tok = p_next_tok(p);
	if (tok->type != T_COLON)
		p_err(p, "expected : after variable name in declaration");

	tok = p_next_tok(p);
	p_parse_type(p, &local->type);
}

st_parse match_statement(struct p_context *p)
{
	struct token *tok = p_cur_tok(p);

	/* vardecl -> symbol ':' ... */
	if (tok->type == T_IDENT && p_after_tok(p, tok)->type == T_COLON)
		return ps_var_decl;

	return NULL;
}
