#include "io.h"

KFILE
kopen(char *path) {
	KFILE file;

	return file;
}

void
kclose(KFILE *kfile) {
}

long
ktell(KFILE *kfile) {
}

void
krewind(KFILE *kfile) {
}

int
kseek(KFILE *kfile, int offset, short int relative_to) {
}
