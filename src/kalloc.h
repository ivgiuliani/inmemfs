#ifndef KMALLOC_H
#define KMALLOC_H

typedef struct _chunk {
	unsigned int size;
	struct _chunk *next;
	void *memory;
} Chunk;

Chunk *kalloc(int);
void kfree(Chunk *);

Chunk *_alloc_chunk(const int);

#endif /* KMALLOC_H */
