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
	NODE_VAR_DECL,
	NODE_ASSIGN,
};

struct module
{
	struct node *ast;
	struct file *source;
};

struct p_type_decl;
struct p_fn_decl;
struct p_fn_def;
struct p_assign;
struct p_var;

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
		struct module *module;
		struct p_fn_decl *fn_decl;
		struct p_fn_def *fn_def;
		struct p_assign *assign;
		struct p_var *var;
	};

	struct p_var **locals;
	int n_locals;
};

/* Parse the token list into an abstract syntax tree. */
struct module *mcc_parse(struct token_list *tokens);

struct p_context
{
	struct module *module;
	struct token_list *tokens;
	struct node *here;
};

/* Node types */
struct p_var
{
	struct type type;
	char *name;
	struct token *place;
};

enum p_assign_kind
{
	PA_SET, /* = */
	PA_ADD, /* += */
	PA_SUB, /* -= */
	PA_MUL, /* *= */
	PA_DIV, /* /= */
	PA_MOD, /* %= */
};

struct p_assign
{
	enum p_assign_kind kind;
	struct p_lvalue *assignee;
	struct p_rvalue *value;
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

struct p_fn_decl
{
	char *name;
	struct p_var **params;
	int n_params;
	struct p_generic **generics;
	int n_generics;
	struct type *return_type;
	struct token *place;
	struct token *block_start;
};

struct p_fn_def
{
	struct p_fn_decl *decl;
};

enum p_value_kind
{
	/* lvalue */
	LVAL_VAR,
	LVAL_INDEX,
	LVAL_FIELD,
	LVAL_ADDR,
	LVAL_DEREF,

	/* rvalue */
	RVAL_LVAL,
	RVAL_STR,
	RVAL_NUM,
	RVAL_CALL,
	RVAL_COND,
	RVAL_BLOCK,
	RVAL_MATH,
	RVAL_CMP,
	RVAL_ASSIGN
};

struct p_lvalue
{
	enum p_value_kind kind;
	union
	{
		struct p_var *v_var;
		struct p_lvalue *v_addr;
		struct p_lvalue *v_deref;
	};

	union
	{
		struct p_rvalue *v_index;
		char *v_field;
	};
};

struct p_rvalue
{
	enum p_value_kind kind;
	union
	{
		struct p_lvalue *v_lval;
		char *v_str;
		long v_num;
		/* v_call */
		/* v_cond */
		/* v_block */
		/* v_math */
		/* v_cmp */
		struct p_assign *v_assign;
	};
};

/* parse expressions */
void p_parse_module(struct p_context *p);
void p_parse_fn_decl(struct p_context *p);
void p_parse_fn_def(struct p_context *p, struct p_fn_decl *decl);
void p_parse_use(struct p_context *p);
void p_parse_block(struct p_context *p, struct node *block);
void p_parse_type(struct p_context *p, struct type *ty);
void p_parse_statement(struct p_context *p);
void p_parse_lvalue(struct p_context *p, struct p_lvalue *into);
void p_parse_rvalue(struct p_context *p, struct p_rvalue *into);

/* Note that these classes _must_ be negative, to allow mixing with regular
   token types which are positive. */
enum match_class
{
	CLASS_LVALUE = -1,
	CLASS_RVALUE = -2,
	CLASS_ASSIGN_OP = -3,
};

/*
 * Match a range of tokens. Depending on the passed arguments, it will match a
 * literal token or a group which match_class defines. For example::
 *
 * 	lvalue '=' rvalue
 *
 * Could be matched using 2 classes and a token, like so:
 *
 * 	match_st(p, 3, CL_LVALUE, T_ASS, CL_RVALUE);
 */
bool p_match(struct p_context *p, int n, ...);

/* utils */
void p_err(struct p_context *p, const char *fmt, ...);
void p_skip_block(struct p_context *p);
struct token *p_cur_tok(struct p_context *p);
struct token *p_next_tok(struct p_context *p);
struct token *p_after_tok(struct p_context *p, struct token *from);
struct token *p_before_tok(struct p_context *p, struct token *from);
void p_set_pos_tok(struct p_context *p, struct token *at);

/* module */
struct module *p_module_create();

/* node */
struct node *p_node_create(struct node *parent);
struct node *p_node_add_child(struct node *inside);
struct node *p_node_add_next(struct node *after);
struct p_var *p_node_add_local(struct node *node);
struct p_var *p_node_local(struct node *node, char *name);
const char *p_node_name(enum node_kind kind);
void p_node_dump(struct node *node);
