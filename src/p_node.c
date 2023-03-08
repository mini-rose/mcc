/* p_node.c
   Copyright (c) 2023 bellrise */

#include <mcc/alloc.h>
#include <mcc/parser.h>
#include <stdio.h>

struct node *p_node_create(struct node *parent)
{
	struct node *n;

	n = slab_alloc(sizeof(*n));
	n->kind = NODE_INV;
	n->parent = parent;

	return n;
}

struct node *p_node_add_child(struct node *inside)
{
	if (!inside->child)
		return (inside->child = p_node_create(inside));
	return p_node_add_next(inside->child);
}

struct node *p_node_add_next(struct node *after)
{
	struct node *walker;

	if (!after->next)
		return (after->next = p_node_create(after->parent));

	walker = after->next;
	while (walker && walker->next)
		walker = walker->next;

	return (walker->next = p_node_create(after->parent));
}

static const char *node_names[] = {
    "<node>", "module", "fn-decl", "fn-def", "use",
};

static void print_fn_sig(struct p_fn_decl *fn)
{
	printf("%s", fn->name);
	if (fn->n_generics) {
		fputc('<', stdout);

		for (int i = 0; i < fn->n_generics - 1; i++)
			printf("%s, ", fn->generics[i]->name);

		printf("%s>", fn->generics[fn->n_generics - 1]->name);
	}

	fputc('(', stdout);

	for (int i = 0; i < fn->n_params - 1; i++) {
		printf("%s: %s, ", fn->params[i]->name,
		       type_str(&fn->params[i]->type));
	}

	if (fn->n_params) {
		printf("%s: %s)", fn->params[fn->n_params - 1]->name,
		       type_str(&fn->params[fn->n_params - 1]->type));
	} else {
		fputc(')', stdout);
	}

	if (fn->return_type && fn->return_type->kind != TY_NULL)
		printf(" -> %s", type_str(fn->return_type));
}

static void p_node_dump_level(struct node *node, int level)
{
	for (int i = 0; i < (level * 2); i++)
		fputc(' ', stdout);

	printf("%s ", node_names[node->kind]);

	/* Additional info */
	switch (node->kind) {
	case NODE_MODULE:
		printf("src=`%s`", node->module->source->path);
		break;
	case NODE_FN_DECL:
		print_fn_sig(node->fn_decl);
		break;
	case NODE_FN_DEF:
		printf("for `%s`", node->fn_def->decl->name);
		break;
	default:
		break;
	}

	fputc('\n', stdout);

	if (node->child)
		p_node_dump_level(node->child, level + 1);

	if (node->next)
		p_node_dump_level(node->next, level);
}

void p_node_dump(struct node *node)
{
	p_node_dump_level(node, 0);
}
