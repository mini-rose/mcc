/* tokenize.c
   Copyright (c) 2023 bellrise */

#include <ctype.h>
#include <mcc/alloc.h>
#include <mcc/errmsg.h>
#include <mcc/tokenize.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct token *token_new(enum token_type type, const char *p, int len)
{
	struct token *tok;

	tok = slab_alloc(sizeof(*tok));
	tok->type = type;
	tok->val = p;
	tok->len = len;

	return tok;
}

static void token_list_append(struct token_list *tokens, struct token *tok)
{
	tokens->tokens = realloc_ptr_array(tokens->tokens, tokens->len + 1);
	tokens->tokens[tokens->len++] = tok;
}

static char *push_str(const char *start, const char *end)
{
	size_t size = end - start + 1;
	char *buf = slab_alloc(size);
	snprintf(buf, size, "%.*s", (int) (end - start), start);
	return buf;
}

static char *strend(char *p)
{
	char *q = p++;

	for (; *p != *q; p++) {
		if (*p == '\n' || *p == '\0')
			return NULL;

		if (*p == '\\')
			p++;
	}

	return p;
}

static void token_convert_kw(struct token *tok)
{
	static const struct
	{
		const char *kw;
		enum token_type type;
	} kws[] = {
	    {"use", T_USE}, {"fn", T_FN}, {"type", T_TYPE}, {"ret", T_RET}};
	static const int n_kws = sizeof(kws) / sizeof(*kws);

	/*
	 * If the content in tok matches a keyword, assign the proper keyword
	 * type, instead of the regular T_IDENT.
	 */

	for (int i = 0; i < n_kws; i++) {
		if (!strncmp(kws[i].kw, tok->val, tok->len)) {
			tok->type = kws[i].type;
			break;
		}
	}
}

struct token_list *tokenize(struct file *source)
{
	struct token_list *list;
	enum token_type last;
	struct file *f;
	char *p;

	list = slab_alloc(sizeof(*list));
	list->source = source;
	last = T_NEWLINE;
	f = source;
	p = f->source;

	while (*p) {
		/* Skip long comment */
		if (!strncmp(p, "/*", 2)) {
			char *q = strstr(p + 2, "*/");

			if (!q) {
				struct token t = {.val = p, .len = 2};
				errsrc(f, &t, "unterminated string");
			}

			p += q - p + 2;
			continue;
		}

		/* Skip oneline comment */
		if (!strncmp(p, "//", 2)) {
			char *q = strstr(p + 2, "\n");
			p += q - p;
		}

		/* Skip unix interpreter */
		if (!strncmp(p, "#!", 2)) {
			while (*p != '\n')
				p++;
		}

		/* Parse newline token,
		   but skip when previous token is newline */
		if (*p == '\n') {
			if (last == T_NEWLINE) {
				p++;
				continue;
			} else {
				struct token *tok =
				    token_new(last = T_NEWLINE, p, 0);
				token_list_append(list, tok);
				p++;
				continue;
			}
		}

		/* Skip space */
		if (isspace(*p)) {
			p++;
			continue;
		}

		/* Tokenize quoted string */
		if (*p == '\'' || *p == '"') {
			struct token *tok;
			char *q = strend(p++);

			if (!q) {
				struct token t = {.val = p - 1, .len = 1};
				errsrc(f, &t, "unterminated string");
			}

			tok = token_new(last = T_STRING, p, q - p);
			token_list_append(list, tok);

			p += q - p + 1;
			continue;
		}

		/* Tokenize string and guess it type */
		if (isalpha(*p) || *p == '_') {
			struct token *tok = NULL;
			char *str;
			char *q = p;

			while (isalnum(*q) || *q == '_')
				q++;

			str = push_str(p, q);

			if (!strcmp("true", str)) {
				tok = token_new(last = T_TRUE, p, p - q);
			} else if (!strcmp("false", str)) {
				tok = token_new(last = T_FALSE, p, p - q);
			} else if (!strcmp("__LINE__", str)) {
				int line = 1;
				static char buf[12];

				for (char *c = f->source; c < p; c++)
					if (*c == '\n')
						line++;

				snprintf(buf, sizeof(buf), "%i", line);

				tok = token_new(last = T_NUMBER, buf,
						strlen(buf));

				token_list_append(list, tok);

				p += 8;
				continue;
			} else if (!strcmp("stdin", str)) {
				char *n = "0";
				tok = token_new(last = T_NUMBER, n, 1);
				token_list_append(list, tok);
				p += 5;
				continue;
			} else if (!strcmp("stdout", str)) {
				char *n = "1";
				tok = token_new(last = T_NUMBER, n, 1);
				token_list_append(list, tok);
				p += 6;
				continue;
			} else if (!strcmp("stderr", str)) {
				char *n = "2";
				tok = token_new(last = T_NUMBER, n, 1);
				token_list_append(list, tok);
				p += 6;
				continue;
			} else {
				tok = token_new(last = T_IDENT, p, q - p);
				token_convert_kw(tok);
			}

			token_list_append(list, tok);
			p += q - p;
			continue;
		}

		/* Tokenize number */
		if (isdigit(*p) || (*p == '.' && isdigit(*(p + 1)))
		    || (*p == '-' && isdigit(*(p + 1)))) {
			struct token *tok;
			char *q = p;

			if (*q == '.' || *q == '-')
				q++;

			while (isdigit(*q) || *q == '.')
				q++;

			tok = token_new(last = T_NUMBER, p, q - p);
			token_list_append(list, tok);

			p += q - p;
			continue;
		}

		/* Tokenize punct and quess it type */
		if (ispunct(*p)) {
			struct token *tok = NULL;

			static char *operators[] = {
			    [T_ASS] = "=",   [T_EQ] = "==",    [T_NEQ] = "!=",
			    [T_ADDA] = "+=", [T_ARROW] = "->", [T_DEC] = "--",
			    [T_INC] = "++",  [T_SUBA] = "-=",  [T_ADD] = "+",
			    [T_DIV] = "/",   [T_MOD] = "%",    [T_DIVA] = "/=",
			    [T_MODA] = "%=", [T_MULA] = "*=",  [T_MUL] = "*",
			    [T_SUB] = "-"};
			static size_t n_operators =
			    sizeof(operators) / sizeof(*operators);

			switch (*p) {
			case '(':
				tok = token_new(last = T_LPAREN, p, 1);
				token_list_append(list, tok);
				p++;
				break;
			case ')':
				tok = token_new(last = T_RPAREN, p, 1);
				token_list_append(list, tok);
				p++;
				break;
			case '{':
				tok = token_new(last = T_LBRACE, p, 1);
				token_list_append(list, tok);
				p++;
				break;
			case '}':
				tok = token_new(last = T_RBRACE, p, 1);
				token_list_append(list, tok);
				p++;
				break;
			case '[':
				tok = token_new(last = T_LBRACKET, p, 1);
				token_list_append(list, tok);
				p++;
				break;
			case ']':
				tok = token_new(last = T_RBRACKET, p, 1);
				token_list_append(list, tok);
				p++;
				break;
			case ',':
				tok = token_new(last = T_COMMA, p, 1);
				token_list_append(list, tok);
				p++;
				break;
			case '.':
				tok = token_new(last = T_DOT, p, 1);
				token_list_append(list, tok);
				p++;
				break;
			case ':':
				tok = token_new(last = T_COLON, p, 1);
				token_list_append(list, tok);
				p++;
				break;
			case '<':
				tok = token_new(last = T_LANGLE, p, 1);
				token_list_append(list, tok);
				p++;
				break;
			case '>':
				tok = token_new(last = T_RANGLE, p, 1);
				token_list_append(list, tok);
				p++;
				break;
			case '&':
				tok = token_new(last = T_AND, p, 1);
				token_list_append(list, tok);
				p++;
				break;
			case ';':
				p++;
				break;
			default:
				break;
			}

			if (!ispunct(*p))
				continue;

			for (size_t i = T_EQ; i < n_operators; i++) {
				if (!strncmp(p, operators[i],
					     strlen(operators[i]))) {

					tok = token_new(
					    last = (enum token_type) i, p,
					    strlen(operators[i]));

					token_list_append(list, tok);
					p += strlen(operators[i]);
					break;
				}
			}

			if (tok)
				continue;

			while (isspace(*p))
				p++;

			if (!ispunct(*p) || *p == '"')
				continue;

			tok = token_new(last = T_PUNCT, p, 1);
			token_list_append(list, tok);

			p++;
			continue;
		}

		p++;
	}

	/* Add end token */
	struct token *tok = token_new(T_END, p, 0);
	token_list_append(list, tok);

	return list;
}

struct token_name
{
	enum token_type type;
	const char *name;
};

static const struct token_name toknames[] = {
    {T_NEWLINE, "newline"},   {T_FN, "fn"},          {T_TYPE, "type"},
    {T_RET, "ret"},           {T_NUMBER, "number"},  {T_STRING, "string"},
    {T_TRUE, "true"},         {T_FALSE, "false"},    {T_IDENT, "ident"},
    {T_PUNCT, "punct"},       {T_LPAREN, "lparen"},  {T_RPAREN, "rparen"},
    {T_LBRACE, "lbrace"},     {T_RBRACE, "rbrace"},  {T_LBRACKET, "lbracket"},
    {T_RBRACKET, "rbracket"}, {T_LANGLE, "langle"},  {T_RANGLE, "rangle"},
    {T_COMMA, "comma"},       {T_DOT, "dot"},        {T_COLON, "colon"},
    {T_END, "end"},           {T_EQ, "eq"},          {T_NEQ, "neq"},
    {T_ASS, "assign"},        {T_ADD, "add"},        {T_ADDA, "addassign"},
    {T_ARROW, "arrow"},       {T_DEC, "decrement"},  {T_INC, "increment"},
    {T_SUBA, "subassign"},    {T_DIV, "divide"},     {T_MOD, "modulo"},
    {T_DIVA, "divassign"},    {T_MODA, "modassign"}, {T_MULA, "mulassign"},
    {T_MUL, "multiply"},      {T_SUB, "subtract"},   {T_AND, "addr"}};

static const int n_toknames = sizeof(toknames) / sizeof(*toknames);

const char *token_name(struct token *tok)
{
	for (int j = 0; j < n_toknames; j++) {
		if (tok->type == toknames[j].type) {
			return toknames[j].name;
		}
	}

	return "<token>";
}

void token_list_dump(struct token_list *tokens)
{
	const char *name = "token";

	for (int i = 0; i < tokens->len; i++) {
		name = token_name(tokens->tokens[i]);
		infomsg("%s `%.*s`", name, tokens->tokens[i]->len,
			tokens->tokens[i]->val);
	}
}
