/* mcc/parser.h
   Copyright (c) 2023 bellrise */

#pragma once

#include <mcc/tokenize.h>
#include <mcc/type.h>

enum node_kind
{
	NODE_INV = 0,
	NODE_MODULE,
	NODE_FN_DECL,
	NODE_FN_DEF,
	NODE_USE,
};

struct module
{
	struct node *ast;
	struct file *source;
};

struct p_type_decl;
struct p_fn_decl;
struct p_fn_def;

/*
 * Represents a single node in the abstract syntax tree, which can have a child
 * or another node in the chain.
 */
struct node
{
	enum node_kind kind;
	struct node *child;
	struct node *next;
	struct node *parent;
	union
	{
		struct p_fn_decl *fn_decl;
		struct p_fn_def *fn_def;
		struct module *module;
	};
};

/* Parse the token list into an abstract syntax tree. */
struct module *mcc_parse(struct token_list *tokens);

struct p_context
{
	struct module *module;
	struct token_list *tokens;
	struct node *here;
};

struct p_var
{
	struct type type;
	char *name;
	struct token *place;
};

struct p_generic
{
	char *name;
	struct token *place;
};

struct p_type_decl
{
	struct type *type;
	char *name;
};

/* Node types */
struct p_fn_decl
{
	char *name;
	struct p_var **params;
	int n_params;
	struct p_generic **generics;
	int n_generics;
	struct token *place;
	struct token *block_start;
};

/* parse expressions */
void p_parse_module(struct p_context *p);
void p_parse_fn_decl(struct p_context *p);
void p_parse_fn_def(struct p_context *p);
void p_parse_use(struct p_context *p);
void p_parse_block(struct p_context *p, struct node *block);
void p_parse_type(struct p_context *p, struct type *ty);

/* utils */
void p_err(struct p_context *p, const char *fmt, ...);
void p_skip_block(struct p_context *p);
struct token *p_cur_tok(struct p_context *p);
struct token *p_next_tok(struct p_context *p);

/* module */
struct module *p_module_create();

/* node */
struct node *p_node_create(struct node *parent);
struct node *p_node_add_child(struct node *inside);
struct node *p_node_add_next(struct node *after);
void p_node_dump(struct node *node);
