/* llvm/emit.c - emit for the LLVM IR target
   Copyright (c) 2023 bellrise */

#include "llvm.h"

#include <mcc/emit.h>
#include <mcc/errmsg.h>
#include <mcc/paths.h>
#include <mcc/settings.h>
#include <stdlib.h>

int emit_llvm(struct module *module)
{
	struct e_context e;
	struct node *node;
	char *out;

	infomsg("emit target=%s %s", settings_global()->target,
		module->source->path);

	e.module = module;
	e.llvm_mod = LLVMModuleCreateWithName(module->source->path);

	/*
	 * For LLVM IR, this is a basic plan of how things should be generated:
	 *
	 * - global types
	 * - function declarations
	 * - function definitions
	 *   - statements & blocks
	 */

	node = module->ast->child;
	while (node) {
		if (node->kind == NODE_FN_DECL) {
			e_fn_decl(&e, node);
		} else if (node->kind == NODE_FN_DEF) {
			e_fn_def(&e, node);
		} else {
			errmsg("cannot emit %s as a top-level node",
			       p_node_name(node->kind));
		}

		node = node->next;
	}

	/*
	 * If the module has a main function, emit a helper main(int, char**)
	 * function which just calls the Mocha main.
	 */
	if (e_has_main(&e))
		e_add_main(&e);

	out = path_tmp(module->source->path, "ll");
	LLVMPrintModuleToFile(e.llvm_mod, out, NULL);

	infomsg("emit to %s", out);

	return 0;
}
