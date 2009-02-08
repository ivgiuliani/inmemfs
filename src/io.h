#ifndef IO_H
#define IO_H

#include "node.h"
#include "kalloc.h"

struct _KFILE {
	struct node *node;
};
typedef struct _KFILE *KFILE;

/* these are used by kseek for relative seeking */
#define KF_SEEK_START 1
#define KF_SEEK_CURR  2
#define KF_SEEK_EOF   3

KFILE kopen(struct node *, char *);
void kclose(KFILE);
long ktell(KFILE);
void krewind(KFILE);
int kseek(KFILE, int, short int);
unsigned int kread(KFILE, unsigned int, void *);
unsigned int kwrite(KFILE, void *, unsigned int);

KFILE _alloc_kfile(struct node *);

#endif /* IO_H */
