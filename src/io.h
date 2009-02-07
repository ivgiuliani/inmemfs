#ifndef IO_H
#define IO_H

#include "kalloc.h"

typedef struct _KFILE {
	Chunk *first_chunk;
} KFILE;

/* these are used by kseek for relative seeking */
#define KF_SEEK_START 1
#define KF_SEEK_CURR  2
#define KF_SEEK_EOF   3

KFILE kopen(char *);
void kclose(KFILE *);
long ktell(KFILE *);
void krewind(KFILE *);
int kseek(KFILE *, int, short int);

#endif /* IO_H */
