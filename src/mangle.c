/* mangle.c - mangle functions and types
   Copyright (c) 2023 bellrise */

#include <mcc/alloc.h>
#include <mcc/errmsg.h>
#include <mcc/mangle.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MANGLE_BUF_SIZ 1024

char mangled_int(int bitwidth)
{
	switch (bitwidth) {
	case 1:
		return 'b';
	case 8:
		return 'a';
	case 16:
		return 's';
	case 32:
		return 'i';
	case 64:
		return 'l';
	default:
		return 'i';
	}
}

static inline char mangled_void()
{
	return 'v';
}

char *mcc_mangle_type(struct type *ty, char *buf)
{
	if (!buf)
		buf = slab_alloc(MANGLE_BUF_SIZ);

	if (ty->kind == TY_INT) {
		buf[0] = mangled_int(ty->t_int.bitwidth);
	} else if (ty->kind == TY_POINTER) {
		buf[0] = 'P';
		mcc_mangle_type(ty->t_base, &buf[1]);
	} else if (ty->kind == TY_OBJECT) {
		/* Parse this as the fully qualified name of the type */
		sprintf(buf, "%zu%s", strlen(ty->t_object.name),
			ty->t_object.name);
	} else {
		errmsg("cannot mangle %s type", type_str(ty));
	}

	return buf;
}

char *mcc_mangle(struct p_fn_decl *fn)
{
	char *name = slab_alloc(MANGLE_BUF_SIZ);

	snprintf(name, MANGLE_BUF_SIZ, "_M%d%s", (int) strlen(fn->name),
		 fn->name);

	for (int i = 0; i < fn->n_params; i++)
		mcc_mangle_type(&fn->params[i]->type, &name[strlen(name)]);

	if (!fn->n_params)
		name[strlen(name)] = mangled_void();

	return name;
}
