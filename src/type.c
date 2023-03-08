/* type.c
   Copyright (c) 2023 bellrise */

#include <mcc/alloc.h>
#include <mcc/type.h>
#include <stdio.h>
#include <string.h>

char *type_str(struct type *ty_)
{
	struct type *ty = ty_;
	char *str;

	/*
	 * Convert a type struct into a readable type name.
	 * Pointers get a & in front.
	 */

	str = slab_alloc(BUFSIZ);

	/* Put enough & in front until we get to a non-pointer type. */
	while (ty->kind == TY_POINTER) {
		strcat(str, "&");
		ty = ty->t_base;
	}

	if (ty->kind == TY_INT) {
		snprintf(str + strlen(str), BUFSIZ, "i%d", ty->t_int.bitwidth);
	} else {
		strcat(str, "T");
	}

	return str;
}
