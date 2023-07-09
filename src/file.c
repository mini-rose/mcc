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
    mfile->is_mmaped = true;

    close(fd);

    return mfile;
}

struct mapped_file *map_stdin()
{
    /* In order to map stdin, we have to continuously allocate enough bytes to
       fit the whole thing, as we have no idea when it ends. */

    const size_t block_size = 4096;

    struct mapped_file *mfile;
    size_t bytes_read = 0;
    size_t chunk_size = 0;

    mfile = malloc(sizeof(*mfile));
    mfile->fd = STDIN_FILENO;
    mfile->source = strdup("<stdin>");
    mfile->is_mmaped = false;
    mfile->source = NULL;

    /* Read stdin in `block_size` chunks. */

    do {
        mfile->source = realloc(mfile->source, bytes_read + block_size);
        chunk_size = fread(&mfile->source[bytes_read], 1, block_size, stdin);
        if (chunk_size < block_size) {
            mfile->len += chunk_size;
            break;
        }

        bytes_read += block_size;
        mfile->len += block_size;
    } while (1);

    return mfile;
}

void unmap_file(struct mapped_file *file)
{
    free(file->path);

    if (file->is_mmaped)
        munmap(file->source, file->len);
    else
        free(file->source);
}
