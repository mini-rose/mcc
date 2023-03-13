/* p_node.c
   Copyright (c) 2023 bellrise */

#include <mcc/alloc.h>
#include <mcc/parser.h>
#include <stdio.h>
#include <string.h>

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

struct p_var *p_node_local(struct node *node, char *name)
{
	for (int i = 0; i < node->n_locals; i++) {
		if (!strcmp(node->locals[i]->name, name))
			return node->locals[i];
	}

	return NULL;
}

struct p_var *p_node_add_local(struct node *node)
{
	node->locals = realloc_ptr_array(node->locals, node->n_locals + 1);
	return (node->locals[node->n_locals++] =
		    slab_alloc(sizeof(struct p_var)));
}

static const char *node_names[] = {"<node>", "module",   "fn-decl", "fn-def",
				   "use",    "var-decl", "assign"};

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

static void print_lvalue(struct p_lvalue *lvalue, int level);

static void print_rvalue(struct p_rvalue *rvalue, int level)
{
	for (int i = 0; i < (level * 2); i++)
		fputc(' ', stdout);

	printf("rvalue: ");
	switch (rvalue->kind) {
	case RVAL_LVAL:
		printf("lvalue\n");
		print_lvalue(rvalue->v_lval, level + 1);
		break;
	case RVAL_NUM:
		printf("number: %ld\n", rvalue->v_num);
		break;
	case RVAL_STR:
		printf("string: %s\n", rvalue->v_str);
		break;
	default:
		printf("\n");
		break;
	}
}

static void print_lvalue(struct p_lvalue *lvalue, int level)
{
	for (int i = 0; i < (level * 2); i++)
		fputc(' ', stdout);

	printf("lvalue: ");
	switch (lvalue->kind) {
	case LVAL_VAR:
		printf("var `%s`\n", lvalue->v_var->name);
		break;
	case LVAL_FIELD:
		printf("field `%s.%s`\n", lvalue->v_var->name, lvalue->v_field);
		break;
	case LVAL_INDEX:
		printf("index `%s`[rvalue]\n", lvalue->v_var->name);
		print_rvalue(lvalue->v_index, level + 1);
		break;
	case LVAL_ADDR:
		printf("addr\n");
		print_lvalue(lvalue->v_addr, level + 1);
		break;
	default:
		printf("\n");
		break;
	}
}

static const char *assign_op_str(enum p_assign_kind kind)
{
	switch (kind) {
	case PA_SET:
		return "=";
	case PA_ADD:
		return "+=";
	case PA_SUB:
		return "-=";
	case PA_MUL:
		return "*=";
	case PA_DIV:
		return "/=";
	case PA_MOD:
		return "%=";
	}

	return "=";
}

static void p_node_dump_level(struct node *node, int level)
{
	for (int i = 0; i < (level * 2); i++)
		fputc(' ', stdout);

	printf("%s ", node_names[node->kind]);

	/* Additional info */
	switch (node->kind) {
	case NODE_MODULE:
		printf("src=`%s`\n", node->module->source->path);
		break;
	case NODE_FN_DECL:
		print_fn_sig(node->fn_decl);
		fputc('\n', stdout);
		break;
	case NODE_FN_DEF:
		printf("for `%s`\n", node->fn_def->decl->name);
		break;
	case NODE_VAR_DECL:
		printf("`%s`: %s\n", node->var->name,
		       type_str(&node->var->type));
		break;
	case NODE_ASSIGN:
		printf("L %s R\n", assign_op_str(node->assign->kind));
		print_lvalue(node->assign->assignee, level + 1);
		print_rvalue(node->assign->value, level + 1);
		break;
	default:
		fputc('\n', stdout);
		break;
	}

	if (node->child)
		p_node_dump_level(node->child, level + 1);

	if (node->next)
		p_node_dump_level(node->next, level);
}

void p_node_dump(struct node *node)
{
	p_node_dump_level(node, 0);
}
