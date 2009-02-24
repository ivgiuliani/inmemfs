/*
 * Include tests on memory allocation and on the I/O system
 */

#include <check.h>
#include <string.h>
#include <stdlib.h>

#include "../src/common.h"
#include "../src/kalloc.h"
#include "../src/node.h"
#include "../src/io.h"
#include "../src/errors.h"

START_TEST (mem_alloc_1byte)
{
	Chunk *c = kalloc(1);

	/* we have just one chunk */
	fail_unless (c->size == 1);

	kfree(c);
}
END_TEST

START_TEST (mem_write_node)
{
	struct node *root = node_create("root", N_DIRECTORY);
	struct node *node = node_create("node", N_FILE);
	KFILE kfile;
	char *data = "I am writing something on a node";
	char *buffer = (char *)malloc(strlen(data) + 1);

	node_add_child(root, node);
	kfile = kopen(root, "node");

	kwrite(kfile, data, strlen(data) + 1);
	kread(kfile, strlen(data) + 1, buffer);

	fail_unless (strcmp(buffer, data) == 0);

	free(buffer);
	kclose(kfile);
	node_delete(root);
}
END_TEST

START_TEST (mem_cant_write_directory)
{
	/* Ensure we can't write anything over a N_DIRECTORY node */
	struct node *root = node_create("root", N_DIRECTORY);
	struct node *node = node_create("node", N_DIRECTORY);
	KFILE kfile;
	char *data = "I can't write anything here!";
	char *buffer = (char *)malloc(strlen(data));

	node_add_child(root, node);
	kfile = kopen(root, "node");

	kwrite(kfile, data, strlen(data));
	fail_unless (kread(kfile, strlen(data), buffer) == E_INVALID_TYPE);

	free(buffer);
	kclose(kfile);
	node_delete(root);
}
END_TEST

START_TEST (mem_multiple_reads)
{
	struct node *root = node_create("root", N_DIRECTORY);
	struct node *node = node_create("node", N_FILE);
	KFILE kfile;
	char *orig = "I am the walrus";
	char *buffer = (char *)malloc(strlen(orig));

	node_add_child(root, node);
	kfile = kopen(root, "node");
	kwrite(kfile, orig, strlen(orig));

	kread(kfile, 5, buffer);
	fail_unless (strcmp(buffer, "I am ") == 0);

	kread(kfile, 4, buffer);
	fail_unless (strcmp(buffer, "the ") == 0);

	kread(kfile, 6, buffer);
	fail_unless (strcmp(buffer, "walrus") == 0);

	free(buffer);
	kclose(kfile);
	node_delete(root);
}
END_TEST

TCase *
tcase_memory(void) {
	TCase *tc_memory = tcase_create("Memory allocation tests");

	tcase_add_test(tc_memory, mem_alloc_1byte);
	tcase_add_test(tc_memory, mem_write_node);
	tcase_add_test(tc_memory, mem_cant_write_directory);
	tcase_add_test(tc_memory, mem_multiple_reads);

	return tc_memory;
}

