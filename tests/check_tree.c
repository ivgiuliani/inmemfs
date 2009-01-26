#include <check.h>
#include <stdlib.h>

#include "../src/errors.h"
#include "../src/node.h"

START_TEST (test_node_creation)
{
	struct node *node;

	node = node_create("File node", N_FILE);
	fail_if (strcmp(node->name, "File node") != 0,
			"Node name is not set correctly");
	fail_if (node->type != N_FILE,
			"Node type is not set correctly");
	node_delete(node);

	node = node_create("Directory node", N_DIRECTORY);
	fail_if (strcmp(node->name, "Directory node") != 0,
			"Node name is not set correctly");
	fail_if (node->type != N_DIRECTORY,
			"Node type is not set correctly");
	node_delete(node);
}
END_TEST

START_TEST (test_node_child_addition)
{
	struct node *father, *children1, *children2;

	father = node_create("Test node", N_DIRECTORY);
	children1 = node_create("Children 1", N_FILE);
	children2 = node_create("Children 2", N_FILE);

	fail_unless (node_add_child(father, children1) != E_FILE_CHILD,
			"Can't add a children to a father node");

	fail_if (node_add_child(children1, children2) != E_FILE_CHILD,
			"Added a file node to a directory node");

	node_delete(father);
}
END_TEST

Suite *
inmemfs_suite(void) {
	Suite *s = suite_create("Master");
	TCase *tc_core = tcase_create("Tree tests");

	suite_add_tcase(s, tc_core);
	tcase_add_test(tc_core, test_node_creation);
	tcase_add_test(tc_core, test_node_child_addition);

	return s;
}

int
main(int argc, char **argv) {
	int number_failed;

	Suite *s = inmemfs_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_VERBOSE);

	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
