/* emit-llvm/module.c - operations on the top-level module
   Copyright (c) 2023 bellrise */

#include "llvm.h"

#include <mcc/mangle.h>
#include <string.h>

bool e_has_main(struct e_context *e)
{
	struct p_fn_decl sig;
	char *mangled_name;

	/* This is all we need to a mangled name. */
	sig.name = "main";
	sig.n_params = 0;

	mangled_name = mcc_mangle(&sig);

	return !!LLVMGetNamedFunction(e->llvm_mod, mangled_name);
}

void e_add_main(struct e_context *e)
{
	LLVMTypeRef fn_ty;
	LLVMTypeRef args_ty[2];
	LLVMBuilderRef builder;
	LLVMValueRef fn;
	LLVMBasicBlockRef block;
	LLVMValueRef main_fn;
	char *mangled_name;
	struct p_fn_decl sig;

	args_ty[0] = LLVMInt32Type();
	args_ty[1] = LLVMPointerType(LLVMPointerType(LLVMInt8Type(), 0), 0);

	fn_ty = LLVMFunctionType(LLVMInt32Type(), args_ty, 2, false);

	fn = LLVMAddFunction(e->llvm_mod, "main", fn_ty);
	builder = LLVMCreateBuilder();
	block = LLVMAppendBasicBlock(fn, "start");

	/* This is all we need to a mangled name. */
	memset(&sig, 0, sizeof(sig));
	sig.name = "main";
	sig.n_params = 0;

	mangled_name = mcc_mangle(&sig);
	main_fn = LLVMGetNamedFunction(e->llvm_mod, mangled_name);

	if (!main_fn) {
		errmsg(
		    "cannot generate call to Mocha main as it does not exist");
	}

	/*
	 * Generated code:
	 *
	 * int main(int, char **)
	 * {
	 *     _M4mainv();
	 * }
	 */

	LLVMPositionBuilderAtEnd(builder, block);
	LLVMBuildCall2(builder, e_gen_fn_type(&sig), main_fn, NULL, 0, "");

	LLVMDisposeBuilder(builder);
}
