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

START_TEST (mem_alloc_n_chunks)
{
	/* will allocate 10 chunks CHUNK_SIZE large and another
	 * chunk of one byte
	 */
	Chunk *first = kalloc((CHUNK_SIZE * 10) + 1);
	Chunk *chunk = first;
	unsigned short int i = 0;

	while (chunk->next != NULL) {
		i++;
		fail_unless (chunk->size == CHUNK_SIZE);
		chunk = chunk->next;
	}

	fail_unless (i == 10);

	fail_unless (chunk->size == 1);

	kfree(first);
}
END_TEST

START_TEST (mem_alloc_less_than_1_chunk)
{
	Chunk *c = kalloc(1);

	/* we have just one chunk */
	fail_unless (c->next == NULL);
	fail_unless (c->size == 1);

	kfree(c);
}
END_TEST

TCase *
tcase_memory(void) {
	TCase *tc_memory = tcase_create("Memory allocation tests");

	tcase_add_test(tc_memory, mem_alloc_1chunk);
	tcase_add_test(tc_memory, mem_alloc_n_chunks);
	tcase_add_test(tc_memory, mem_alloc_less_than_1_chunk);

	return tc_memory;
}

