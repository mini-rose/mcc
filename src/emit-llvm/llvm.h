/* emit-llvm/llvm.h - functions for the LLVM IR emitter
   Copyright (c) 2023 bellrise */

#pragma once

#include <llvm-c/Core.h>
#include <mcc/alloc.h>
#include <mcc/errmsg.h>
#include <mcc/parser.h>

struct e_context
{
	struct module *module;
	LLVMModuleRef llvm_mod;
};

/* module */
bool e_has_main(struct e_context *e);
void e_add_main(struct e_context *e);

/* fn_decl */
void e_fn_decl(struct e_context *e, struct node *decl);

/* fn_def */
void e_fn_def(struct e_context *e, struct node *def);

/* gen_type */
LLVMTypeRef e_gen_type(struct type *ty);
LLVMTypeRef e_gen_fn_type(struct p_fn_decl *fn);
