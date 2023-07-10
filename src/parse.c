/* parse.c - turn a list of tokens into an AST
   Copyright (c) 2023 mini-rose */

#include "mcc.h"

struct node *parse(struct mapped_file *source,
                   struct token_list *__unused tokens)
{
    struct node_module *parent_node;

    parent_node = node_create(NULL, NOD_MODULE);
    parent_node->source_path = strdup(source->path);

    return (void *) parent_node;
}
