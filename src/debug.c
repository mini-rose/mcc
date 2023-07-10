/* debug.c - additional debug tools
   Copyright (c) 2023 mini-rose */

#include "mcc.h"

#include <ctype.h>

void dump_bytes(void *addr, int len)
{
    unsigned char *p;
    int total_lines = len / 16;
    int spaces;
    int chunk = 16;

    total_lines = len / 16;
    total_lines += len % 16 ? 1 : 0;

    for (int i = 0; i < total_lines; i++) {
        p = addr + (i * 16);

        chunk = 16;
        if (total_lines - 1 == i)
            chunk = len - (i * 16);

        /* Print bytes */
        printf("%p : ", p);
        for (int j = 0; j < chunk; j++) {
            printf("%02hhx", p[j]);
            if (j % 2 != 0)
                fputc(' ', stdout);
        }

        /* Print spaces */
        spaces = (16 - chunk) * 2 + (16 - chunk) / 2 + (chunk % 2);
        for (int j = 0; j < spaces; j++)
            fputc(' ', stdout);

        /* Print ASCII chars */
        fputs(": ", stdout);
        for (int j = 0; j < chunk; j++) {
            printf("%c", isprint(p[j]) ? p[j] : '.');
        }

        fputc('\n', stdout);
    }
}

void token_list_dump(struct token_list *tokens)
{
    struct token_name
    {
        token_k kind;
        const char *name;
    };

#define TK(TOK)                                                                \
 {                                                                             \
  .kind = TK_##TOK, .name = #TOK                                               \
 }

    static const struct token_name token_names[] = {TK(EOF), TK(SYMBOL),
                                                    TK(STRING)};

#undef TK

    struct token *tok;

    for (int i = 0; i < tokens->len; i++) {
        tok = &tokens->tokens[i];

        for (size_t j = 0; j < TABLE_SIZE(token_names); j++) {
            if (tok->kind == token_names[j].kind) {
                printf("\033[1;39m%s\033[0m len=%d pos=`%.*s`\n",
                       token_names[j].name, tok->len, tok->len, tok->pos);
                break;
            }
        }
    }
}

static void node_dump_level(struct node *node, int level)
{
#define FIELD(NOD) [NOD_##NOD] = #NOD,
    static const char *node_names[] = {FIELD(MODULE)};
#undef FIELD

    for (int i = 0; i < level; i++)
        fputs("  ", stdout);

    /* Print this */

    printf("\033[1;39m%s\033[0m", node_names[node->kind]);

    switch (node->kind) {
    case NOD_MODULE:
        printf(" source_path=`%s`", NA_MODULE(node)->source_path);
        break;
    case NOD_FUNCTION:
        printf(" name=`%s`", NA_FUNCTION(node)->name);
        break;
    }

    fputc('\n', stdout);

    /* Print the children. */
    if (node->child)
        node_dump_level(node->child, level + 1);

    /* Print the next one. */
    if (node->next)
        node_dump_level(node->next, level);
}

void node_dump(struct node *node)
{
    node_dump_level(node, 0);
}
