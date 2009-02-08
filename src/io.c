#include <stdlib.h>

#include "node.h"
#include "io.h"

KFILE
kopen(struct node *root, char *path) {
	KFILE file;
	struct node *node;

	node = node_path_find(root, path);
	if (node == NULL)
		return NULL;

	file = _alloc_kfile(node);

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
