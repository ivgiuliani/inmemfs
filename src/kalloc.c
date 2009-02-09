#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "kalloc.h"

int _mem_count = 0;

/*
 * Allocates a set of memory chunks for the specified size.
 * Memory is divided in chunks of CHUNK_SIZE bytes.
 * You should NEVER access the allocated memory directly,
 * rather you should use the kread and kwrite functions.
 *
 * Memory allocated with kalloc() can be freed with kfree()
 */
Chunk *
kalloc(int size) {
	/* Probably we can give some optimization here */
	Chunk *chunk, *first = NULL, *prev = NULL;
	unsigned int chunk_num, rest;

	chunk_num = size / CHUNK_SIZE;
	rest = size % CHUNK_SIZE;

	while (chunk_num-- > 0) {
		chunk = _alloc_chunk(CHUNK_SIZE);
		
		if (prev != NULL)
			prev->next = chunk;

		prev = chunk;
		if (first == NULL)
			first = chunk;
	}

	if (rest != 0) {
		chunk = _alloc_chunk(rest);
		if (first == NULL)
			first = chunk;
	}

	_mem_count += size;
	return first;
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
	Chunk *next = chunk;
	Chunk *curr;

	while (next != NULL) {
		curr = next;
		next = curr->next;

		_mem_count -= curr->size;
		free(curr->memory);
		free(curr);
	}
}


/*
 * Read the specified size starting from the first chunk 'chunk'.
 * It returns the effective number of read bytes (i.e. if 'size' is greater
 * than the actual allocated memory, the whole allocated memory).
 * Do not use this function directly, we provided you kread which
 * will make your life much easier.
 */
unsigned int
_raw_kread(Chunk *c, unsigned int size, void *buffer) {
	unsigned int read_bytes = 0, copy_size;
	Chunk *chunk = c;

	while ((read_bytes < size) && (chunk != NULL)) {
		copy_size = (size <= chunk->size) ? size : chunk->size;

		memcpy(buffer + read_bytes, chunk->memory, copy_size);
		size -= copy_size;
		read_bytes += copy_size;
		chunk = chunk->next;
	}

	return read_bytes;
}

/*
 * Writes the specified 'size' number of bytes in the allocated chunk list
 * starting from 'chunk'
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

	while ((written_bytes < size) && (chunk != NULL)) {
		copy_size = (size <= chunk->size) ? size : chunk->size;

		memcpy(chunk->memory, data + written_bytes, copy_size);
		size -= copy_size;
		written_bytes += copy_size;
		chunk = chunk->next;
	}

	return written_bytes;
}
