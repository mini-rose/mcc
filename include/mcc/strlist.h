/* mcc/strlist.h
   Copyright (c) 2023 bellrise */

#pragma once

struct strlist
{
	char **strs;
	int len;
};

void strlist_append(struct strlist *list, char *str);
