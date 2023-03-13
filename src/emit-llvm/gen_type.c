/* emit-llvm/gen_type.c - generate LLVM types from Mocha types
   Copyright (c) 2023 bellrise */

#include "llvm.h"

LLVMTypeRef gen_int_type(struct type_int *ty)
{
	switch (ty->bitwidth) {
	default:
		return LLVMInt32Type();
	}
}

LLVMTypeRef e_gen_type(struct type *ty)
{
	if (!ty)
		return LLVMVoidType();

	switch (ty->kind) {
	case TY_INT:
		return gen_int_type(&ty->t_int);
	case TY_POINTER:
		return LLVMPointerType(e_gen_type(ty->t_base), 0);
	default:
		return LLVMVoidType();
	}
}

LLVMTypeRef e_gen_fn_type(struct p_fn_decl *fn)
{
	LLVMTypeRef *params;

	if (fn->n_generics)
		errmsg("cannot emit generic function type for `%s`", fn->name);

	params = realloc_ptr_array(NULL, fn->n_params);
	for (int i = 0; i < fn->n_params; i++)
		params[i] = e_gen_type(&fn->params[i]->type);

	return LLVMFunctionType(e_gen_type(fn->return_type), params,
				fn->n_params, false);
}
