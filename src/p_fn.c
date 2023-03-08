/* p_fn.c
   Copyright (c) 2023 bellrise */

#include <mcc/alloc.h>
#include <mcc/parser.h>

static void parse_generic_params(struct p_context *p, struct p_fn_decl *decl)
{
	struct token *tok = p_cur_tok(p);
	struct p_generic *generic;

	if (tok->type != T_LANGLE)
		p_err(p, "expected `<`");

	while (1) {
		tok = p_next_tok(p);
		if (tok->type != T_IDENT)
			p_err(p, "expected generic name, like `T`");

		/* Add a generic to the declaration. */
		decl->generics =
		    realloc_ptr_array(decl->generics, decl->n_generics + 1);
		generic = decl->generics[decl->n_generics++] =
		    slab_alloc(sizeof(*generic));

		generic->name = slab_strndup(tok->val, tok->len);
		generic->place = tok;

		tok = p_next_tok(p);
		if (tok->type == T_RANGLE) {
			tok = p_next_tok(p);
			break;
		}

		if (tok->type != T_COMMA)
			p_err(p, "expected comma after generic type name");
	}
}

static void parse_params(struct p_context *p, struct p_fn_decl *decl)
{
	struct token *tok = p_cur_tok(p);
	struct p_var *param;

	if (tok->type != T_LPAREN)
		p_err(p, "expected `(`");

	while (1) {
		tok = p_next_tok(p);

		if (tok->type == T_RPAREN)
			break;

		if (tok->type != T_IDENT)
			p_err(p, "expected parameter name");

		/* Add a parameter to the declaration. */
		decl->params =
		    realloc_ptr_array(decl->params, decl->n_params + 1);
		param = decl->params[decl->n_params++] =
		    slab_alloc(sizeof(*param));

		param->place = tok;
		param->name = slab_strndup(tok->val, tok->len);

		tok = p_next_tok(p);
		if (tok->type != T_COLON)
			p_err(p, "expected colon between the name and type");

		/* Parameter type */
		tok = p_next_tok(p);

		p_parse_type(p, &param->type);
	}

	/* Move after ) */
	p_next_tok(p);
}

void p_parse_fn_decl(struct p_context *p)
{
	struct token *tok;
	struct node *node;

	/* 'fn' keyword */
	tok = p_next_tok(p);
	if (tok->type != T_FN)
		p_err(p, "expected `fn` keyword");

	/* function name */
	tok = p_next_tok(p);
	if (tok->type != T_IDENT)
		p_err(p, "expected function name");

	node = p_node_add_child(p->module->ast);
	node->kind = NODE_FN_DECL;
	node->fn_decl = slab_alloc(sizeof(*node->fn_decl));

	node->fn_decl->name = slab_strndup(tok->val, tok->len);
	node->fn_decl->place = tok;

	tok = p_next_tok(p);

	p->here = node;

	/* optionally, generic type parameters */
	if (tok->type == T_LANGLE)
		parse_generic_params(p, node->fn_decl);

	tok = p_cur_tok(p);

	/* optionally, regular parameters */
	if (tok->type == T_LPAREN)
		parse_params(p, node->fn_decl);

	tok = p_cur_tok(p);
	node->fn_decl->block_start = tok;

	p_skip_block(p);

	p->here = node->parent;
}

void p_parse_fn_def(struct p_context *p) { }
