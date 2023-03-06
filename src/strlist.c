/* strlist.c
   Copyright (c) 2023 bellrise */

#include <mcc/strlist.h>
#include <stdlib.h>
#include <string.h>

void strlist_append(struct strlist *list, char *str)
{
	list->strs = realloc(list->strs, sizeof(char *) * (list->len + 1));
	list->strs[list->len++] = strdup(str);
}

void strlist_destroy(struct strlist *list)
{
	for (int i = 0; i < list->len; i++)
		free(list->strs[i]);
	free(list->strs);
}
