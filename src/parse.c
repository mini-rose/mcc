/* parse.c
   Copyright (c) 2023 bellrise */

#include <mcc/alloc.h>
#include <mcc/parser.h>

struct module *mcc_parse(struct token_list *tokens)
{
	struct p_context context;
	struct module *module;

	module = p_module_create();
	module->source = tokens->source;

	context.module = module;
	context.tokens = tokens;

	p_parse_module(&context);

	return module;
}
