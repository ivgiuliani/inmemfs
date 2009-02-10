#include <check.h>

#include "../src/common.h"
#include "../src/kalloc.h"

START_TEST (mem_alloc_1chunk)
{
	Chunk *c = kalloc(CHUNK_SIZE);

	/* we have just one chunk */
	fail_unless (c->next == NULL);
	fail_unless (c->size == CHUNK_SIZE);

	kfree(c);
}
END_TEST

TCase *
tcase_memory(void) {
	TCase *tc_memory = tcase_create("Memory allocation tests");

	tcase_add_test(tc_memory, mem_alloc_1chunk);

	return tc_memory;
}

