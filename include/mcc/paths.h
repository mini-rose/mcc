/* mcc/paths.h
   Copyright (c) 2023 bellrise */

#pragma once

#include <stdbool.h>
#include <stdio.h>

bool path_is_c_file(const char *path);
bool path_is_mocha_file(const char *path);
bool path_is_object_file(const char *path);

char *path_tmp_object(const char *tip);
char *path_tmp(const char *tip, const char *suffix);
int path_copy(const char *to, const char *from);

void path_dump_file(FILE *f);
