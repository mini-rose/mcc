/* lex.c - convert a raw text stream into a list of tokens
   Copyright (c) 2023 mini-rose */

#include "mcc.h"

/**
 * Allocate a new token and add it to the list. This is better than allocating
 * each token as a seperate region in memory using malloc, as it will use way
 * more memory and there may be a _lot_ of tokens.
 */
static struct token *tok_new(struct token_list *toks)
{
    const int bump_size = 512;
    struct token *tok;

    if (toks->len >= toks->space) {
        /* Allocate space for `bump_size` more tokens. */
        toks->space += bump_size;
        toks->tokens =
            realloc(toks->tokens, sizeof(struct token) * toks->space);
    }

    tok = &toks->tokens[toks->len++];
    tok->len = 0;
    tok->pos = 0;
    tok->kind = TK_EOF;

    return tok;
}

static inline struct token *tok_new_kind(struct token_list *toks, token_k kind)
{
    struct token *t;
    t = tok_new(toks);
    t->kind = kind;
    return t;
}

struct token_list *lex(struct mapped_file *file)
{
    struct token_list *toks;
    struct token *tok;

    /* Make the last token a EOF. */
    tok = tok_new_kind(toks, TK_EOF);

    return toks;
}
