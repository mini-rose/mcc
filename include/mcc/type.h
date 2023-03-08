/* mcc/type.h
   Copyright (c) 2023 bellrise */

#pragma once

struct token;

enum type_kind
{
	TY_NULL,
	TY_INT,     /* iN */
	TY_POINTER, /* &T */
	TY_OBJECT,  /* type {} */
	TY_GENERIC, /* <T> */
};

struct type_int
{
	int bitwidth;
};

struct type_object_field
{
	struct type *field;
	char *name;
	struct token *place;
};

struct type_object
{
	struct type_object_field **fields;
	int n_fields;
	struct token *place;
};

struct type
{
	enum type_kind kind;
	union
	{
		struct type_int t_int;
		struct type_object t_object;
		struct type *t_base; /* base type of pointer or generic */
	};
};

char *type_str(struct type *ty);
