/* emit-llvm/fn_decl.c - function declaration
   Copyright (c) 2023 bellrise */

#include "llvm.h"

#include <mcc/mangle.h>

void e_fn_decl(struct e_context *e, struct node *decl)
{
	char *mangled_name;

	mangled_name = mcc_mangle(decl->fn_decl);

	LLVMAddFunction(e->llvm_mod, mangled_name,
			e_gen_fn_type(decl->fn_decl));
}
