/* emit-llvm/block.c - operations inside a block
   Copyright (c) 2023 bellrise */

#include "llvm.h"

void e_var_decl(struct e_blockcontext *block, struct node *var)
{
	LLVMValueRef alloca;

	alloca =
	    LLVMBuildAlloca(block->builder, e_gen_type(&var->var->type), "");

	block->local_names =
	    realloc_ptr_array(block->local_names, block->n_locals + 1);
	block->locals = realloc_ptr_array(block->locals, block->n_locals + 1);

	block->local_names[block->n_locals] = slab_strdup(var->var->name);
	block->locals[block->n_locals++] = alloca;
}

static LLVMValueRef gen_lvalue(struct e_blockcontext *block,
			       struct p_lvalue *val)
{
	LLVMValueRef lvalue;

	if (val->kind == LVAL_VAR) {
		lvalue = e_block_local(block, val->v_var->name);
		if (!lvalue)
			errmsg("could not find local `%s`", val->v_var->name);
		return lvalue;
	} else {
		errmsg("gen_lvalue only works for LVAL_VAR for now");
	}
}

static LLVMValueRef gen_rvalue(struct e_blockcontext *block,
			       struct p_rvalue *val)
{
	if (val->kind == RVAL_LVAL)
		return gen_lvalue(block, val->v_lval);
	if (val->kind == RVAL_NUM) {
		/* TODO: generate different types depending on v_num. */
		return LLVMConstInt(LLVMInt32Type(), val->v_num, false);
	}

	errmsg("gen_rvalue only works for RVAL_LVAL & RVAL_NUM");
}

void e_assign(struct e_blockcontext *block, struct node *assign)
{
	LLVMValueRef left;
	LLVMValueRef right;

	left = gen_lvalue(block, assign->assign->assignee);
	right = gen_rvalue(block, assign->assign->value);

	/* TODO: support different operators */

	LLVMBuildStore(block->builder, right, left);
}

LLVMValueRef e_block_local(struct e_blockcontext *block, char *name)
{
	for (int i = 0; i < block->n_locals; i++) {
		if (!strcmp(block->local_names[i], name))
			return block->locals[i];
	}

	return NULL;
}
