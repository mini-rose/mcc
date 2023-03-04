/* mangle.h - function for mangling names
   Copyright (c) 2023 mini-rose */

#pragma once
#include <mcc/parser.h>

char *mcc_mangle(const fn_expr_t *func);
char *mcc_mangle_type(type_t *ty, char *buf);
