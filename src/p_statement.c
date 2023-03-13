/* p_statement.c
   Copyright (c) 2023 bellrise */

#include <mcc/alloc.h>
#include <mcc/parser.h>

/*
 * Parses a list of tokens based on the language grammar. For more details and a
 * better reference of what might be going on, see the grammar.rst doc file.
 */

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

static void ps_var_decl(struct p_context *p)
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

static enum p_assign_kind ps_assign_kind(struct token *tok)
{
	switch (tok->type) {
	case T_ASS:
		return PA_SET;
	case T_ADDA:
		return PA_ADD;
	case T_SUBA:
		return PA_SUB;
	case T_MULA:
		return PA_MUL;
	case T_DIVA:
		return PA_DIV;
	case T_MODA:
		return PA_MOD;
	default:
		return 0;
	}
}

static void ps_assign(struct p_context *p)
{
	struct token *tok = p_cur_tok(p);
	struct node *node;

	node = p_node_add_child(p->here);
	node->kind = NODE_ASSIGN;
	node->assign = slab_alloc(sizeof(*node->assign));

	node->assign->place = tok;

	/* Left side of assignment. */
	node->assign->assignee = slab_alloc(sizeof(*node->assign->assignee));
	p_parse_lvalue(p, node->assign->assignee);

	/* Assignment operator. */
	tok = p_cur_tok(p);
	node->assign->kind = ps_assign_kind(tok);

	/* Right side of assignment. */
	tok = p_next_tok(p);
	node->assign->value = slab_alloc(sizeof(*node->assign->value));
	p_parse_rvalue(p, node->assign->value);
}

st_parse match_statement(struct p_context *p)
{
	/* var-decl */
	if (p_match(p, 2, T_IDENT, T_COLON))
		return ps_var_decl;

	/* assignment */
	if (p_match(p, 3, CLASS_LVALUE, CLASS_ASSIGN_OP, CLASS_RVALUE))
		return ps_assign;

	return NULL;
}
