/* alloc.c - Allocation routines
   Copyright (c) 2023 bellrise */

#include <ctype.h>
#include <errno.h>
#include <mcc/alloc.h>
#include <mcc/errmsg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

struct alloc_entry
{
	void *block;
	int size;
};

struct alloc_table
{
	struct alloc_entry **allocs;
	int n_allocs;
};

struct array
{
	void **items;
	int n_items;
};

struct array_table
{
	struct array **arrays;
	int n_arrays;
};

struct slab_allocators
{
	struct slab *slabs;
	int n_slabs;
};

struct sanitizer_alloc
{
	void *block;
	const char *file;
	const char *func;
	int line;
	int requested;
};

struct sanitizer_info
{
	struct sanitizer_alloc **allocs;
	int n_allocs;
};

struct slab_allocators global_slabs;
struct sanitizer_info sanitizer_info;

struct alloc_table global_alloc;
struct array_table global_arrays;

static void *safe_malloc(size_t size)
{
	void *block = malloc(size);

	if (block == NULL)
		errmsg(strerror(errno));

	return block;
}

static void *safe_calloc(size_t nmemb, size_t size)
{
	void *block = calloc(nmemb, size);

	if (block == NULL)
		errmsg(strerror(errno));

	return block;
}

static void *safe_realloc(void *ptr, size_t size)
{
	void *block = realloc(ptr, size);

	if (block == NULL)
		errmsg(strerror(errno));

	return block;
}

void slab_init_global(bool sanitize)
{
	global_slabs.n_slabs = 6;
	global_slabs.slabs =
	    safe_calloc(global_slabs.n_slabs, sizeof(struct slab));

	slab_create(&global_slabs.slabs[0], 8, sanitize);
	slab_create(&global_slabs.slabs[1], 16, sanitize);
	slab_create(&global_slabs.slabs[2], 32, sanitize);
	slab_create(&global_slabs.slabs[3], 64, sanitize);
	slab_create(&global_slabs.slabs[4], 512, sanitize);
	slab_create(&global_slabs.slabs[5], 1024, sanitize);

	sanitizer_info.allocs = NULL;
	sanitizer_info.n_allocs = 0;
}

void slab_deinit_global()
{
	for (int i = 0; i < global_alloc.n_allocs; i++)
		free(global_alloc.allocs[i]->block);
	free(global_alloc.allocs);

	for (int i = 0; i < global_arrays.n_arrays; i++)
		free(global_arrays.arrays[i]->items);
	free(global_arrays.arrays);

	/* The slabs must be free'd at the very end. */
	for (int i = 0; i < global_slabs.n_slabs; i++)
		slab_destroy(&global_slabs.slabs[i]);
}

void slab_sanitize_global()
{
	for (int i = 0; i < global_slabs.n_slabs; i++)
		slab_sanitize(&global_slabs.slabs[i]);
}

void slab_create(struct slab *allocator, int block_size, bool sanitize)
{
	memset(allocator, 0, sizeof(*allocator));
	allocator->block_size = block_size;
	allocator->blocks_per_slab = SLAB_REGION_SIZE / block_size;
	allocator->regions = safe_calloc(1, sizeof(void *));
	allocator->regions[0] = safe_malloc(SLAB_REGION_SIZE);
	allocator->n_regions = 1;
	allocator->sanitize = sanitize;
}

void slab_destroy(struct slab *allocator)
{
	for (int i = 0; i < allocator->n_regions; i++)
		free(allocator->regions[i]);
	free(allocator->regions);
}

static struct sanitizer_alloc *sanitizer_get_alloc_details(void *block)
{
	for (int i = 0; i < sanitizer_info.n_allocs; i++) {
		if (sanitizer_info.allocs[i]->block == block)
			return sanitizer_info.allocs[i];
	}

	return NULL;
}

static void sanitizer_buffer_error(struct slab *allocator, const char *problem,
				   int i_slab, int i_pos)
{
	struct sanitizer_alloc *info;
	void *base;

	printf("alloc: Buffer sanitizer: \e[1;91m%s\e[0m in slab%d[%d,%d] "
	       "allocator\n",
	       problem, allocator->block_size, i_slab, i_pos);

	base = allocator->regions[i_slab] + (allocator->block_size * i_pos);

	printf(
	    "alloc:   addressable range of \e[92m%d\e[0m bytes from %p to %p\n",
	    allocator->block_size, base, base + allocator->block_size);

	info = sanitizer_get_alloc_details(base);
	if (!info) {
		printf("alloc:   no alloc details, compile with "
		       "`-DOPT_ALLOC_SLAB_INFO=1`\n");
	} else {
		printf("alloc:   allocated \e[92m%d\e[0m bytes \e[34min %s at "
		       "%s:%d\e[0m\n",
		       info->requested, info->func, info->file, info->line);
	}

	if (info->file) {
		FILE *src = fopen(info->file, "r");
		if (!src) {
			printf("alloc:   \e[90m<could not open source "
			       "file>\e[0m\n");
			goto skip;
		}

		char buf[512];
		int line = 0;

		while (line != info->line) {
			fgets(buf, 512, src);
			line++;
		}

		/* right-strip */
		int i = strlen(buf) - 1;

		while (i >= 0) {
			if (isspace(buf[i]))
				buf[i] = 0;
			else
				break;
			i--;
		}

		printf("alloc:   at\nalloc:   \e[1;98m%s\e[0m\n", buf);

		fclose(src);
	}

skip:
	printf("alloc:\n");
	printf("alloc:             addressable data\n");
	printf("alloc:             v~~~~~~~\n");
	printf(
	    "alloc:   "
	    "|\e[31mxxxxxxxx\e[0m|\e[32m--user--\e[0m|\e[31mxxxxxxxx\e[0m|\n");

	if (!strcmp(problem, "buffer-underflow")) {
		printf("alloc:         ~~^\n");
		printf("alloc:         bytes written here\n");
	} else if (!strcmp(problem, "buffer-overflow")) {
		printf("alloc:                      ^~~\n");
		printf("alloc:                      bytes written here\n");
	}

	printf("alloc:\n");

	const int MAX_DUMP = 256;
	int to_dump = allocator->block_size;

	if (to_dump > MAX_DUMP)
		to_dump = MAX_DUMP;

	printf("alloc: user data dump:\n");
	dump_memory_prefixed(base, to_dump, "alloc:   ");

	if (allocator->block_size > MAX_DUMP) {
		printf("alloc:   ... %d more bytes ...\n",
		       allocator->block_size - MAX_DUMP);
	}

	printf("alloc:\n");
}

void slab_sanitize(struct slab *allocator)
{
	const unsigned char magic[] = {SLAB_MAGIC};

	int blocks_to_check;
	void *base_ptr;
	void *block;
	bool clean;

	/*
	 * Check for buffer underflows and overflows, by comparing the blocks
	 * before and after the user block for changed bytes.
	 */

	for (int i_slab = 0; i_slab < allocator->n_regions; i_slab++) {
		base_ptr = allocator->regions[i_slab];
		blocks_to_check = allocator->blocks_per_slab;

		/* If we're at the last block, limit the amount of blocks to
		   check to the current position. */
		if (i_slab == allocator->i_slab)
			blocks_to_check = allocator->i_pos;

		for (int i = 0; i < blocks_to_check - 2; i += 3) {

			/* Check buffer underflow */
			block = base_ptr + (allocator->block_size * i);
			clean = true;

			for (int i = 0; i < allocator->block_size; i += 2) {
				if (memcmp(block + i, magic, 2))
					clean = false;
			}

			if (!clean) {
				sanitizer_buffer_error(allocator,
						       "buffer-underflow",
						       i_slab, i + 1);
			}

			/* Check buffer overflow */
			block = base_ptr + (allocator->block_size * (i + 2));
			clean = true;

			for (int i = 0; i < allocator->block_size; i += 2) {
				if (memcmp(block + i, magic, 2))
					clean = false;
			}

			if (!clean) {
				sanitizer_buffer_error(allocator,
						       "buffer-overflow",
						       i_slab, i + 1);
			}
		}
	}
}

static void *mmap_region(size_t size)
{
	return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON,
		    -1, 0);
}

static void *slab_acquire_block(struct slab *self, int requested)
{
	void *base_ptr;
	void *block_ptr;
	int blocks;

	if (self->i_slab == self->n_regions) {
		self->regions = safe_realloc(
		    self->regions, sizeof(void *) * (self->n_regions + 1));
		self->regions[self->n_regions++] =
		    mmap_region(SLAB_REGION_SIZE);
	}

	base_ptr = self->regions[self->i_slab];
	block_ptr = base_ptr + (self->block_size * self->i_pos);

	self->i_pos++;
	if (self->i_pos == self->blocks_per_slab) {
		self->i_slab++;
		self->i_pos = 0;
	}

	/* Stats */
	blocks = self->i_slab * self->blocks_per_slab + self->i_pos;

	self->unused_padding += self->block_size - requested;
	self->average_block = (self->average_block * (blocks - 1)) + requested;
	self->average_block /= blocks;

	memset(block_ptr, 0, self->block_size);
	return block_ptr;
}

static void *slab_acquire_sanitized_block(struct slab *self, int requested)
{
	void *before_block;
	void *after_block;
	void *block_ptr;

	/*
	 * Sanitizing blocks in this slab allocator protects against buffer
	 * underflows and overflows, by allocating 2 additional blocks before
	 * and after the block the user gets for writing.
	 *
	 * In normal mode, each block is a user block with no space in between.
	 *
	 * |aaaa|bbbb|cccc|....
	 *
	 * In sanitized mode, each block gets 2 blocks before and after it
	 * filled with special bytes to check if the user might have overwritten
	 * some by accident.
	 *
	 * |----|aaaa|----|----|bbbb|----|...
	 *
	 * Then, calling a sanitizer function should check all the magic bytes
	 * before and after allocated blocks for invalid bytes.
	 */

	/* Check if we have enough space for 3 blocks. */
	if (self->i_pos + 3 > self->blocks_per_slab) {
		self->i_slab++;
		self->i_pos = 0;
	}

	before_block = slab_acquire_block(self, self->block_size);
	block_ptr = slab_acquire_block(self, requested);
	after_block = slab_acquire_block(self, self->block_size);

	const unsigned char magic[] = {SLAB_MAGIC};

	for (int i = 0; i < self->block_size; i += 2) {
		memcpy(before_block + i, magic, 2);
		memcpy(after_block + i, magic, 2);
	}

	return block_ptr;
}

static void *acquire_oversized_block(int n)
{
	void *block;

	block = safe_calloc(1, n);

	global_alloc.allocs = safe_realloc(global_alloc.allocs,
					   sizeof(struct alloc_entry *)
					       * (global_alloc.n_allocs + 1));

	global_alloc.allocs[global_alloc.n_allocs] =
	    slab_alloc(sizeof(struct alloc_entry));
	global_alloc.allocs[global_alloc.n_allocs]->block = block;
	global_alloc.allocs[global_alloc.n_allocs]->size = n;

	global_alloc.n_allocs++;

	return block;
}

void *slab_alloc_simple(int n)
{
	struct slab *allocator = NULL;

	for (int i = 0; i < global_slabs.n_slabs; i++) {
		if (n <= global_slabs.slabs[i].block_size) {
			allocator = &global_slabs.slabs[i];
			break;
		}
	}

	if (!allocator)
		return acquire_oversized_block(n);

	return allocator->sanitize ? slab_acquire_sanitized_block(allocator, n)
				   : slab_acquire_block(allocator, n);
}

void *slab_alloc_info(int n, const char *file, const char *func, int line)
{
	struct sanitizer_alloc *alloc;
	void *block;

	block = slab_alloc_simple(n);

	alloc = safe_calloc(1, sizeof(*alloc));

	sanitizer_info.allocs = safe_realloc(
	    sanitizer_info.allocs,
	    (sanitizer_info.n_allocs + 1) * sizeof(struct sanitizer_alloc *));
	sanitizer_info.allocs[sanitizer_info.n_allocs++] = alloc;

	alloc->requested = n;
	alloc->block = block;
	alloc->file = file;
	alloc->func = func;
	alloc->line = line;

	return block;
}

void *slab_alloc_array(int n, int item_size)
{
	return slab_alloc(n * item_size);
}

void *slab_strdup(const char *str)
{
	return slab_strndup(str, strlen(str));
}

void *slab_strndup(const char *str, size_t len)
{
	char *ptr;

	ptr = slab_alloc(len + 1);
	memcpy(ptr, str, len);
	ptr[len] = 0;

	return ptr;
}

static void *array_create(int n)
{
	struct array *arr;

	arr = slab_alloc(sizeof(*arr));

	global_arrays.arrays =
	    safe_realloc(global_arrays.arrays,
			 (global_arrays.n_arrays + 1) * sizeof(struct array *));

	global_arrays.arrays[global_arrays.n_arrays++] = arr;

	arr->items = safe_calloc(n, sizeof(void *));
	arr->n_items = n;

	return arr->items;
}

static struct array *array_find(void *ptr)
{
	for (int i = 0; i < global_arrays.n_arrays; i++) {
		if (global_arrays.arrays[i]->items == ptr)
			return global_arrays.arrays[i];
	}

	return NULL;
}

void *realloc_ptr_array(void *array, int n)
{
	struct array *self;

	if (array == NULL)
		return array_create(n);

	/* Re-allocate the array */
	self = array_find(array);
	self->items = safe_realloc(self->items, n * sizeof(void *));
	self->n_items = n;

	return self->items;
}

static int slab_dump(struct slab *self)
{
	float used;
	float padding;
	int blocks;
	int total;

	blocks = self->i_slab * self->blocks_per_slab + self->i_pos;
	used = (float) blocks / (self->n_regions * self->blocks_per_slab) * 100;
	total = (self->n_regions * SLAB_REGION_SIZE) / 1024;

	if (blocks) {
		padding =
		    ((float) self->unused_padding / (blocks * self->block_size))
		    * 100;
	} else {
		padding = 0;
	}

	printf("alloc: % 10d % 8d % 8d % 5.0f%% % 5.0f%% % 6.0f % 6d KB\n",
	       self->block_size, self->n_regions, blocks, used, padding,
	       self->average_block, total);

	return total;
}

void alloc_dump_stats()
{
	int total = 0;
	int total_ptrs = 0;
	float avg;

	printf("alloc: Slab allocators:\n");
	printf("alloc:   region size: %d (%d kB)\n", SLAB_REGION_SIZE,
	       SLAB_REGION_SIZE / 1024);
	printf("alloc:   allocators in use: %d\n", global_slabs.n_slabs);
	printf("alloc:\nalloc: %10s %8s %8s %6s %6s %6s %9s\n", "block-size",
	       "regions", "blocks", "used", "pad", "avg", "total");

	for (int i = 0; i < global_slabs.n_slabs; i++)
		total += slab_dump(&global_slabs.slabs[i]);

	printf("alloc:\nalloc: % 56d KB\n", total);

	avg = 0;
	for (int i = 0; i < global_alloc.n_allocs; i++)
		avg += global_alloc.allocs[i]->size;
	avg /= global_alloc.n_allocs;

	printf("alloc:\nalloc: Oversized allocations:\n");
	printf("alloc:   %d instance(s), with an average of %.0f B\n",
	       global_alloc.n_allocs, avg);

	for (int i = 0; i < global_arrays.n_arrays; i++)
		total_ptrs += global_arrays.arrays[i]->n_items;

	printf("alloc:\nalloc: Pointer arrays:\n");
	printf("alloc:   %d instance(s), with a total of %d pointer(s)\n",
	       global_arrays.n_arrays, total_ptrs);
}

static void dump_line(void *addr, int amount, const char *prefix)
{
	int byte;

	printf("%s0x%08zx : ", prefix, (size_t) addr);

	for (int i = 0; i < 16; i++) {
		if (i > amount) {
			printf("  ");
			if (i % 2 != 0)
				fputc(' ', stdout);
			continue;
		}

		printf("%02hhx", ((char *) addr)[i]);
		if (i % 2 != 0)
			fputc(' ', stdout);
	}

	printf(": ");
	for (int i = 0; i < amount; i++) {
		byte = ((char *) addr)[i];
		fputc(byte >= 0x20 && byte <= 0x7e ? byte : '.', stdout);
	}

	fputc('\n', stdout);
}

void dump_memory_prefixed(void *addr, int amount, const char *prefix)
{
	int lines = 0;
	int rest;

	lines = amount >> 4;
	for (int i = 0; i < lines; i++)
		dump_line(addr + (i * 16), 16, prefix);

	rest = (lines << 4) ^ amount;
	if (rest)
		dump_line(addr + (lines << 4), rest, prefix);
}

void dump_memory(void *addr, int amount)
{
	dump_memory_prefixed(addr, amount, "");
}
