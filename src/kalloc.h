#ifndef KMALLOC_H
#define KMALLOC_H

typedef struct _chunk {
	unsigned int size;
	void *memory;
} Chunk;

Chunk *kalloc(int);
void kfree(Chunk *);
unsigned int _raw_kread(Chunk *, unsigned int, void *);
unsigned int _raw_kwrite(Chunk *, void *, unsigned int);

Chunk *_alloc_chunk(const int);

#endif /* KMALLOC_H */
