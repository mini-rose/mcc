/* mcc/file.h
   Copyright (c) 2023 bellrise */

#pragma once

#include <stddef.h>

struct file
{
	const char *path;
	char *source;
	size_t len;
};

/* mmap() the file into memory, may return NULL on failure. */
struct file *file_map(const char *path);

void file_unmap(struct file *fil);
