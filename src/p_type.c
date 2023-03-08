/* p_type.c
   Copyright (c) 2023 bellrise */

#include <mcc/alloc.h>
#include <mcc/parser.h>
#include <string.h>

static bool parse_builtin_type_if_possible(struct token *tok, struct type *ty)
{
	if (!strncmp("i32", tok->val, tok->len)) {
		ty->kind = TY_INT;
		ty->t_int.bitwidth = 32;
		return true;
	}

	return false;
}

static void resolve_type(struct p_context *p, struct type *ty)
{
	struct token *tok = p_cur_tok(p);

	if (parse_builtin_type_if_possible(tok, ty))
		return;

	/* TODO: lookup other than builtin types here */

	p_err(p, "unknown type");
}

void p_parse_type(struct p_context *p, struct type *ty)
{
	struct token *tok = p_cur_tok(p);

	if (tok->type == T_AND) {
		p_next_tok(p);
		ty->kind = TY_POINTER;
		ty->t_base = slab_alloc(sizeof(struct type));
		return p_parse_type(p, ty->t_base);
	}

	if (tok->type != T_IDENT)
		p_err(p, "expected type name");

	resolve_type(p, ty);
}
