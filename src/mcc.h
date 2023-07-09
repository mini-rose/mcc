/* mcc.h - main header
   Copyright (c) 2023 mini-rose */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum token_k
{
    TK_EOF = -1,
    TK_NEWLINE = 0, // \n
    TK_SYMBOL,      // <symbol>
};

enum node_k
{
    NOD_MODULE,
    NOD_FUNCTION,
    NOD_VARDECL,
    NOD_ASSIGN,
    NOD_RET,
};

typedef enum token_k token_k;
typedef enum node_k node_k;

struct node;
struct mapped_file;

struct token
{
    char *pos;
    int len;
    token_k kind;
};

struct token_list
{
    struct token *tokens;
    int len;
    int space;
};

/**
 * Convert a raw text stream into a list of tokens.
 */
struct token_list *lex(struct mapped_file *file);

/**
 * Anything that can be named as a single object is a node. For example, a
 * function, if statement, variable declaration are all nodes with possible
 * child nodes. The whole tokenized file gets turned into a node tree, each
 * node representing a single expression.
 *
 * Structure
 *
 * Each node apart from the top-level module has a pointer to the parent node.
 * For example, a variable declaration in a function will point to the function
 * node.
 *
 * 	+----------+
 * 	|  module  |
 * 	+----------+
 *    | .child
 *    v
 * 	+----------+ .next  +----------+
 * 	| function |------->| function |
 * 	+----------+        +----------+
 *    | .child
 *    v
 * 	+----------+ .next  +----------+
 * 	| var decl |------->|   call   |
 * 	+----------+        +----------+
 *
 * As you can see on this graph, each node holds a pointer to the child and next
 * node. This means that parent nodes do not hold a array of child nodes, but
 * rather only to the first one in a linked list. The next node can be simply
 * accessed using .next.
 *
 * Data
 *
 * Some nodes may have custom data, like the name of the function or an argument
 * list. This kind of information is allocated in a seperate region pointed by
 * the .data field.
 */
struct node
{
    struct node *parent;
    struct node *child;
    struct node *next;

    node_k kind;

    void *data;
};

/**
 * Convert the token list into an AST, validating the whole thing. The returned
 * tree is assumed to be correct.
 */
struct node *parse(struct token_list *tokens);

struct mapped_file
{
    char *path;
    char *source;
    size_t len;
    int fd;
};

/**
 * Maps the file into memory returning a mapped_file struct, or NULL if
 * something fails.
 */
struct mapped_file *map_file(const char *path);

/**
 * Unmap and free the file.
 */
void unmap_file(struct mapped_file *file);
