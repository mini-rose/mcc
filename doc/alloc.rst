Memory allocation
=================

An important topic for any C program is memory allocations, which has become
a headache in mcc using basic malloc() and free() calls. This is why the slab
allocator was created, to have easy access to heap memory without the need to
worry about free'ing it. The slab allocators used in mcc do not allow for
free'ing memory, only acquiring. This means that once ``slab_deinit_global()``
has been called, all memory acquired by the slabs is free'd at once, removing
any worry about memory leaks.

Instead of::

        int *t = malloc(sizeof(int));

        // ...

        free(t);

Now, memory allocation is achieved using the ``slab_alloc`` function::

        int *t = slab_alloc(sizeof(int));

It will acquire a block in the slab with the best matching block size. For
example, if there are 4 slab allocators initialized with the following sizes:
8, 16, 32 and 64 - then an allocation of 7 bytes will get put into an 8 byte
block, and an allocation of over 64 bytes will get put into the oversize
section. For more information about all allocations done using slab_alloc,
call ``alloc_dump_stats()``.

The value acquired from slab_alloc is a pointer to a block, with a size of at
least n bytes, which is valid until the ``slab_deinit_global`` function gets
called.

Pointer arrays::

        struct object **array;
        int n;

        // Previous realloc calls:

        array = realloc(array, sizeof(struct object *) * (n + 1));

        // Should now use realloc_ptr_array instead.

                this will return a pointer to a void ** array with unallocated
                item blocks
                v
        array = realloc_ptr_array(array, n + 1);
                                         ^
                                         notice that we are not multiplying the
                                         elements by the item size

These will also get automatically free'd after calling slab_deinit_global.


Memory sanitizer
----------------

When passing ``-Xsanitize-alloc``, all global slab allocators will be created
with the sanitize option. This will use 3x as much memory, but will check for
buffer underflows and overflows. This is done by allocating 2 additional blocks
to the left and right of the user data block::

        |-underflow-|-user-block-|-overflow-|

Both the underflow and overflow block are filled with magic bytes, which are
then checked when calling ``slab_sanitize()``.


Slab allocator
--------------

Global functions:

* ``slab_init_global(bool sanitize)``
        Initialize global allocators.

* ``slab_deinit_global()``
        Free global allocators.

* ``slab_sanitize_global()``
        Run the sanitizer on the global allocators. This effectively calls
        slab_sanitize() on every slab.


Allocation routines. As you can see, there is no slab_free routine. This is
because the compiler isn't a long-lived program, so freeing memory continuously
in this day and age is not really needed, meaning the slabs will get freed at
the end of the program, all at once.

* ``void *slab_alloc(int n)``
        Actually a macro, it finds a matching slab allocator, which means
        finding the smallest block size which N bytes will fit in. This means
        that calling slab_alloc(4) can allocate a block in a slab8 allocator.
        If the fitting allocator cannot be found, it will create an oversized
        allocation on the heap which will fit N bytes and no more.

* ``void *slab_alloc_array(int n, int item_size)``
        Allocates a contiguous array of n * item_size bytes.

* ``void *slab_strdup(const char *str)``
* ``void *slab_strndup(const char *str, size_t len)``
        Same as strdup() and strndup(), but allocated memory on the slab
        allocator.


Functions for operating on the ``struct slab`` allocator type. You should know
what you're doing if you want to create custom slab allocators, as the global
ones have additional functionality attached to them. If you want a new block
size, add a new allocator in slab_init_global.

* ``slab_create(struct slab *, int block_size, bool sanitize)``
        Create a new slab with the given block size and sanitizer option. This
        will initially allocate SLAB_SIZE (default 16KB) bytes to prepare for
        storing blocks. The chosen block_size should not be larger than 4KB to
        allow for sanitizing.

* ``slab_destroy(struct slab *)``
        Free all memory associated with this slab.

* ``slab_sanitize(struct slab *)``
        Run the sanitizer on this slab. Requires that the slab is created with
        the `sanitize` option enabled.
