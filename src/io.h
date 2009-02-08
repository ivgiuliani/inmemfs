#ifndef IO_H
#define IO_H

#include "node.h"
#include "kalloc.h"

struct _KFILE {
	struct node *node;
	Chunk *first_chunk;
};
typedef struct _KFILE *KFILE;

/* these are used by kseek for relative seeking */
#define KF_SEEK_START 1
#define KF_SEEK_CURR  2
#define KF_SEEK_EOF   3

KFILE kopen(char *);
void kclose(KFILE);
long ktell(KFILE);
void krewind(KFILE);
int kseek(KFILE, int, short int);

KFILE _alloc_kfile(struct node *);

#endif /* IO_H */
