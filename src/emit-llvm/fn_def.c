/* emit-llvm/fn_def.c - function definition
   Copyright (c) 2023 bellrise */

#include "llvm.h"

void e_fn_def(struct e_context *e, struct node *decl)
{
	LLVMBasicBlockRef start;
	LLVMBuilderRef builder;
	LLVMValueRef fn;
	char *mangled_name;

	struct e_blockcontext block = {0};
	struct node *walker;

	mangled_name = mcc_mangle(decl->fn_def->decl);
	fn = LLVMGetNamedFunction(e->llvm_mod, mangled_name);

	if (!fn)
		errmsg("could not find `%s` in module", mangled_name);

	builder = LLVMCreateBuilder();
	start = LLVMAppendBasicBlock(fn, "start");

	LLVMPositionBuilderAtEnd(builder, start);
	block.builder = builder;
	block.e = e;

	walker = decl->child;
	while (walker) {
		if (walker->kind == NODE_VAR_DECL)
			e_var_decl(&block, walker);

		if (walker->kind == NODE_ASSIGN)
			e_assign(&block, walker);

		walker = walker->next;
	}

	LLVMDisposeBuilder(builder);
}
