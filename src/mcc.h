/* mcc.h - main header
   Copyright (c) 2023 mini-rose */

#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MCC_MAJOR 0
#define MCC_MINOR 4

#define STRINGIFY(X)  _STRINGIFY(X)
#define _STRINGIFY(X) #X

#define TABLE_SIZE(X) (sizeof(X) / sizeof(*X))

#if DEBUG
# define IFDEBUG(CODE) CODE
#else
# define IFDEBUG(CODE)
#endif

#define __unused      __attribute__((unused))
#define __warn_unused __attribute__((warn_unused_result))

enum token_k
{
    TK_EOF = -1,
    TK_SYMBOL, // <symbol>
    TK_STRING, // "string"
};

/* Any change to this changes a lot in node.c */
enum node_k
{
    NOD_MODULE,
    NOD_FUNCTION
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

/* Token list API */
void token_list_free(struct token_list *);
void token_list_dump(struct token_list *);

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
 * list. This kind of information is placed right after the node header.
 */
struct node
{
    struct node *parent;
    struct node *child;
    struct node *next;
    node_k kind;
};

struct node_module
{
    struct node head;
    char *source_path;
};

struct node_function
{
    struct node head;
    char *name;
};

#define NA_MODULE(PTR)   ((struct node_module *) PTR)
#define NA_FUNCTION(PTR) ((struct node_function *) PTR)

/**
 * Convert the token list into an AST, validating the whole thing. The returned
 * tree is assumed to be correct, otherwise throws an error.
 */
struct node *parse(struct mapped_file *source, struct token_list *);

/* Node API */

/**
 * Returns a struct node *. Creates a new node of the given kind. If parent is
 * NULL, a new tree is created, otherwise the new node is added as a child to
 * the parent node.
 */
void *node_create(void *parent, node_k kind) __warn_unused;

/**
 * Free this node and all its children. Note, this does _not_ free the next
 * node, which means that it will become a dangling pointer unless you keep
 * it.
 */
void node_free(struct node *);

void node_dump(struct node *);

struct mapped_file
{
    char *path;
    char *source;
    size_t len;
    int fd;
    bool is_mmaped;
};

/**
 * Maps the file into memory returning a mapped_file struct, or NULL if
 * something fails. Note that mapped files have an EOF char at the end
 * of the buffer at source[len - 1]. The stdin version will map the whole
 * stdin into a buffer.
 */
struct mapped_file *file_map(const char *path);
struct mapped_file *file_from_stdin();

/**
 * Unmap and free the file.
 */
void file_free(struct mapped_file *file);

struct options
{
    char *filename;
};

struct options *options_parse(int argc, char **argv);

void options_free(struct options *opts);

/**
 * Report error at a specific position and exit.
 */
void err_at(struct mapped_file *file, char *pos, int len, const char *fmt, ...);

/**
 * Immediately die and exit the program.
 */
void die(const char *fmt, ...);

/**
 * Dump `len` bytes starting from `addr` to stdout.
 */
void dump_bytes(void *addr, int len);
