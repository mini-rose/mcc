/* file.c
   Copyright (c) 2023 bellrise */

#include <errno.h>
#include <fcntl.h>
#include <mcc/alloc.h>
#include <mcc/errmsg.h>
#include <mcc/file.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

struct file *file_map(const char *path)
{
	struct stat info;
	struct file *f;
	int fd;

	f = slab_alloc(sizeof(*f));
	fd = open(path, O_RDONLY);

	if (fd == -1)
		return NULL;

	if (fstat(fd, &info) == -1)
		return NULL;

	f->path = slab_strdup(path);
	f->source = mmap(NULL, info.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	f->len = info.st_size;

	if (f->source == MAP_FAILED)
		return NULL;

	return f;
}

void file_unmap(struct file *fil)
{
	if (munmap(fil->source, fil->len) == -1)
		errmsg(strerror(errno));
}
