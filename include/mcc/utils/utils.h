/* utils.h - random utilities
   Copyright (c) 2023 mini-rose */

#pragma once

#include <stdbool.h>

#define LEN(array) (sizeof(array) / sizeof(*array))

bool isfile(const char *path);
bool isdir(const char *path);
char *abspath(const char *path);
