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

/*
 * Destroys the file pointer. Once this function has been
 * called, you can't reuse the file pointer again (unless
 * you don't reopen the node)
 */
void
kclose(KFILE kfile) {
	free(kfile);
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
	kfile = (KFILE)malloc(sizeof(struct _KFILE));

	kfile->node = node;
	kfile->node->first_chunk = NULL;
}

/*
 * Read from the current position the specified number of
 * bytes, and save the content in 'buffer'
 */
unsigned int
kread(KFILE kfile, unsigned int size, void *buffer) {
	return _raw_kread(kfile->node->first_chunk, size, buffer);
}

/*
 * Write starting from the current position the specified
 * number of bytes from 'buffer'
 */
unsigned int
kwrite(KFILE kfile, void *data, unsigned int size) {
	return _raw_kwrite(kfile->node->first_chunk, data, size);
}
