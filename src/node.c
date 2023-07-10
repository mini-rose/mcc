/* node.c - implements the node API
   Copyright (c) 2023 mini-rose */

#include "mcc.h"

void *node_create(void *_parent, node_k kind)
{
    struct node *parent = _parent;
    struct node *walker;
    struct node *nod;
    size_t alloc_size;

    static const size_t sizes[] = {[NOD_MODULE] = sizeof(struct node_module),
                                   [NOD_FUNCTION] =
                                       sizeof(struct node_function)};
    alloc_size = sizes[kind];

    nod = calloc(1, alloc_size);
    nod->parent = parent;
    nod->kind = kind;

    if (!parent)
        return nod;

    if (!parent->child) {
        parent->child = nod;
        return nod;
    }

    /* Walk to the end of the linked-list. */
    walker = parent->child;
    while (walker->next)
        walker = walker->next;
    walker->next = nod;

    return nod;
}

static void node_free_chain(struct node *nod)
{
    struct node *this;
    struct node *next;

    /* Free the whole chain, meaning we need to walk the .next pointers. */

    next = nod;
    while (next) {
        this = next;
        next = next->next;
        node_free(this);
    }
}

static void node_free_kind(struct node *nod)
{
    struct node_module *module;
    struct node_function *function;

    switch (nod->kind) {
    case NOD_MODULE:
        module = NA_MODULE(nod);
        free(module->source_path);
        break;
    case NOD_FUNCTION:
        function = NA_FUNCTION(nod);
        free(function->name);
        break;
    }
}

void node_free(struct node *nod)
{
    if (nod->child)
        node_free_chain(nod->child);

    node_free_kind(nod);
    free(nod);
}
