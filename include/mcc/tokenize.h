/* mcc/tokenize.h
   Copyright (c) 2023 bellrise */

#pragma once

#include <mcc/file.h>
#include <stdbool.h>

enum token_type
{
	T_NEWLINE,
	T_FN,   // fn
	T_USE,  // use
	T_TYPE, // type
	T_RET,  // ret
	T_NUMBER,
	T_STRING,
	T_TRUE,  // true
	T_FALSE, // false
	T_IDENT,
	T_PUNCT,
	T_LPAREN,   // (
	T_RPAREN,   // )
	T_LBRACE,   // {
	T_RBRACE,   // }
	T_LBRACKET, // [
	T_RBRACKET, // ]
	T_LANGLE,   // <
	T_RANGLE,   // >
	T_COMMA,    // ,
	T_DOT,      // .
	T_COLON,    // :
	T_END,

	/* Operator types */
	T_EQ,    // ==
	T_NEQ,   // !=
	T_ASS,   // =
	T_ADD,   // +
	T_ADDA,  // +=
	T_ARROW, // ->
	T_DEC,   // --
	T_INC,   // ++
	T_SUBA,  // -=
	T_DIV,   // /
	T_MOD,   // %
	T_DIVA,  // /=
	T_MODA,  // %=
	T_MULA,  // *=
	T_MUL,   // *
	T_SUB,   // -
	T_AND,   // &
};

struct token
{
	enum token_type type;
	const char *val;
	int len;
};

struct token_list
{
	struct token **tokens;
	struct file *source;
	int len;
	int iter;
};

struct token_list *tokenize(struct file *source);
void token_list_dump(struct token_list *tokens);
const char *token_name(struct token *tok);
