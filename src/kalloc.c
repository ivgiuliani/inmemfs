#include <stdlib.h>

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
	rest = size - (CHUNK_SIZE * chunk_num);

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
	Chunk *chunk = (Chunk *)malloc(size);
	chunk->size = size;
	return chunk;
}

/*
 * Frees the memory allocated from the list of chunks whose the
 * chunk parameter is the first chunk in the list
 */
void
kfree(Chunk *chunk) {
	Chunk *next = chunk;

	while (next != NULL) {
		next = chunk->next;
		free(chunk);
	}
}

