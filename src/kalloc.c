#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "kalloc.h"

/*
 * TODO: actually the whole kalloc/kfree mechanism is just a wrapper
 * to malloc(). The idea here is that we should build our own memory
 * manager on the top of brk()/sbrk().
 */

int _mem_count = 0;

/*
 * Allocates a set of memory chunks for the specified size.
 * Memory allocated with kalloc() can be freed with kfree()
 */
Chunk *
kalloc(int size) {
	Chunk *chunk = _alloc_chunk(size);
	_mem_count += size;
	return chunk;
}

/*
 * Allocate a chunk of the specified size. You usually don't need
 * to call this function directly, unless you're doing something
 * very dirty.
 */
Chunk *
_alloc_chunk(const int size) {
	Chunk *chunk = (Chunk *)malloc(sizeof(Chunk));
	chunk->size = size;
	chunk->memory = (void *)malloc(size);
	return chunk;
}

/*
 * Frees the memory allocated from the list of chunks whose the
 * chunk parameter is the first chunk in the list
 */
void
kfree(Chunk *chunk) {
	_mem_count -= chunk->size;
	free(chunk->memory);
	free(chunk);

	/* invalidate the pointer to the chunk */
	chunk = NULL;
}


/*
 * Read the specified size from chunk.
 * It returns the effective number of read bytes (i.e. if 'size' is greater
 * than the actual allocated memory, the whole allocated memory).
 * Do not use this function directly, we provided you kread which
 * will make your life much easier.
 */
unsigned int
_raw_kread(Chunk *c, unsigned int size, void *buffer) {
	unsigned int read_bytes = 0, copy_size;
	Chunk *chunk = c;

	copy_size = (size <= chunk->size) ? size : chunk->size;
	memcpy(buffer, chunk->memory, copy_size);

	return read_bytes;
}

/*
 * Writes the specified 'size' number of bytes in the allocated chunk.
 * Returns the actual number of written bytes (i.e. if 'size' is greater
 * than the actual allocated memory, the whole allocated memory).
 * Do not use this function directly, we provided you kwrite which
 * will make your life much easier.
 * Please note that this function does NOT allocate the needed memory.
 */
unsigned int
_raw_kwrite(Chunk *c, void *data, unsigned int size) {
	Chunk *chunk = c;
	unsigned int written_bytes = 0, copy_size;

	copy_size = (size <= chunk->size) ? size : chunk->size;
	memcpy(chunk->memory, data, copy_size);

	return written_bytes;
}
