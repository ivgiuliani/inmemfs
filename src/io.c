#include <stdlib.h>

#include "node.h"
#include "io.h"

KFILE
kopen(char *path) {
	KFILE file;

	return file;
}

void
kclose(KFILE kfile) {
}

long
ktell(KFILE kfile) {
}

void
krewind(KFILE kfile) {
}

int
kseek(KFILE kfile, int offset, short int relative_to) {
}

KFILE
_alloc_kfile(struct node *node) {
	KFILE kfile;
	kfile = (KFILE *)malloc(sizeof(KFILE));

	kfile->node = node;
	kfile->first_chunk = NULL;
}
