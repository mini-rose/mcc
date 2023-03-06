/* strlist.c
   Copyright (c) 2023 bellrise */

#include <mcc/alloc.h>
#include <mcc/strlist.h>
#include <stdlib.h>
#include <string.h>

void strlist_append(struct strlist *list, char *str)
{
	list->strs = realloc_ptr_array(list->strs, list->len + 1);
	list->strs[list->len++] = slab_strdup(str);
}
