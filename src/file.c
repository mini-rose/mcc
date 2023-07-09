/* file.c - file operations
   Copyright (c) 2023 mini-rose */

#include "mcc.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

struct mapped_file *map_file(const char *path)
{
    struct mapped_file *mfile;
    struct stat stat;
    int fd;

    if ((fd = open(path, O_RDONLY)) == -1)
        return NULL;

    if (fstat(fd, &stat) == -1)
        return NULL;

    mfile = malloc(sizeof(*mfile));
    mfile->source = mmap(NULL, stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    if (mfile->source == MAP_FAILED) {
        free(mfile);
        return NULL;
    }

    mfile->len = stat.st_size;
    mfile->path = strdup(path);

    close(fd);

    return mfile;
}

void unmap_file(struct mapped_file *file)
{
    free(file->path);
    munmap(file->source, file->len);
}
