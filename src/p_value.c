/* p_value.c - parse value
   Copyright (c) 2023 bellrise */

#include <mcc/alloc.h>
#include <mcc/parser.h>
#include <stdlib.h>

void p_parse_lvalue(struct p_context *p, struct p_lvalue *into)
{
	struct token *tok = p_cur_tok(p);
	char *name;

	/* addr: &symbol */
	if (tok->type == T_AND) {
		tok = p_next_tok(p);

		into->kind = LVAL_ADDR;
		into->v_addr = slab_alloc(sizeof(*into->v_addr));

		return p_parse_lvalue(p, into->v_addr);
	}

	/* deref: *symbol */
	if (tok->type == T_MUL) {
		tok = p_next_tok(p);

		into->kind = LVAL_DEREF;
		into->v_deref = slab_alloc(sizeof(*into->v_deref));

		return p_parse_lvalue(p, into->v_deref);
	}

	if (tok->type != T_IDENT)
		p_err(p, "expected symbol in lvalue");

	/* Reference to a local var, find it. */
	into->kind = LVAL_VAR;
	name = slab_strndup(tok->val, tok->len);
	into->v_var = p_node_local(p->here, name);

	if (!into->v_var)
		p_err(p, "use of undeclared variable");

	/*
	 * If we find a [ or . after the symbol, we must convert the lvalue into
	 * a index or field type.
	 */

	tok = p_after_tok(p, tok);

	/* index: lvalue '[' rvalue ']' */
	if (tok->type == T_LBRACKET) {
		tok = p_next_tok(p);
		tok = p_next_tok(p);

		into->kind = LVAL_INDEX;
		into->v_index = slab_alloc(sizeof(*into->v_index));

		return p_parse_rvalue(p, into->v_index);
	}

	/* field: lvalue '.' symbol */
	if (tok->type == T_DOT) {
		tok = p_next_tok(p);
		tok = p_next_tok(p);

		if (tok->type != T_IDENT)
			p_err(p, "expected field name after dot");

		into->kind = LVAL_FIELD;
		into->v_field = slab_strndup(tok->val, tok->len);

		/* TODO: validate field */
		return;
	}

	p_next_tok(p);
}

void p_parse_rvalue(struct p_context *p, struct p_rvalue *into)
{
	struct token *tok = p_cur_tok(p);

	/* lvalue */
	if (p_match(p, 1, CLASS_LVALUE)) {
		into->kind = RVAL_LVAL;
		into->v_lval = slab_alloc(sizeof(*into->v_lval));
		return p_parse_lvalue(p, into->v_lval);
	}

	/* string */
	if (tok->type == T_STRING) {
		into->kind = RVAL_STR;
		into->v_str = slab_strndup(tok->val, tok->len);
		p_next_tok(p);
		return;
	}

	/* number */
	if (tok->type == T_NUMBER) {
		into->kind = RVAL_NUM;
		into->v_num = strtol(tok->val, NULL, 10);
		p_next_tok(p);
		return;
	}

	/* call */
	/* cond-block */
	/* expr-block */
	/* math op */
	/* cmp op */
	/* assign */
}
