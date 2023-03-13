/* targets.c - targets for emit
   Copyright (c) 2023 bellrise */

#include <mcc/alloc.h>
#include <mcc/emit.h>
#include <string.h>

int emit_llvm(struct module *mod);

static const struct emit_target targets[] = {
    {"llvm", emit_llvm},
};
static const int n_targets = sizeof(targets) / sizeof(*targets);

const char **emit_target_list(int *len)
{
	const char **list;

	*len = n_targets;
	list = realloc_ptr_array(NULL, n_targets);

	for (int i = 0; i < n_targets; i++)
		list[i] = targets[i].id;

	return list;
}

const struct emit_target *emit_for_target(const char *id)
{
	for (int i = 0; i < n_targets; i++) {
		if (!strcmp(id, targets[i].id))
			return &targets[i];
	}
	return NULL;
}
