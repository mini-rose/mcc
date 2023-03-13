/* emit-llvm/llvm.h - functions for the LLVM IR emitter
   Copyright (c) 2023 bellrise */

#pragma once

#include <llvm-c/Core.h>
#include <mcc/alloc.h>
#include <mcc/errmsg.h>
#include <mcc/mangle.h>
#include <mcc/parser.h>
#include <string.h>

struct e_context
{
	struct module *module;
	LLVMModuleRef llvm_mod;
};

struct e_blockcontext
{
	struct e_context *e;
	LLVMBuilderRef builder;
	LLVMValueRef *locals;
	char **local_names;
	int n_locals;
};

/* module */
bool e_has_main(struct e_context *e);
void e_add_main(struct e_context *e);

/* fn_decl */
void e_fn_decl(struct e_context *e, struct node *decl);

/* fn_def */
void e_fn_def(struct e_context *e, struct node *def);

/* block */
void e_var_decl(struct e_blockcontext *block, struct node *var);
void e_assign(struct e_blockcontext *block, struct node *assign);
LLVMValueRef e_block_local(struct e_blockcontext *block, char *name);

/* gen_type */
LLVMTypeRef e_gen_type(struct type *ty);
LLVMTypeRef e_gen_fn_type(struct p_fn_decl *fn);
