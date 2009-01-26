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

	fail_unless (node_get_children_no(father) == 1,
			"Children number returned doesn't match real children number");

	node_delete(father);
}
END_TEST

START_TEST (test_node_list_creation)
{
	struct node_list *nl = NULL;

	nl = node_list_create();

	fail_if (nl == NULL,
			"Node list hasn't been created");
}
END_TEST

START_TEST (test_node_list_add_siblings)
{
	struct node_list *nl, *tmpnl;
	struct node *n1, *n2, *n3;
	int i = 0;

	nl = node_list_create();

	n1 = node_create("Node 1", N_FILE);
	n2 = node_create("Node 2", N_FILE);
	n3 = node_create("Node 3", N_FILE);

	fail_unless (node_list_add_sibling(nl, n1) == n1,
			"Returned object mismatch from expected object");
	fail_unless (node_list_add_sibling(nl, n2) == n2,
			"Returned object mismatch from expected object");
	fail_unless (node_list_add_sibling(nl, n3) == n3,
			"Returned object mismatch from expected object");

	tmpnl = nl;
	while (tmpnl->next != NULL) {
		tmpnl = tmpnl->next;
		i++;
	}

	fail_unless(i == 3,
			"Unexpected number of siblings");
}
END_TEST

Suite *
inmemfs_suite(void) {
	Suite *s = suite_create("Master");
	TCase *tc_core = tcase_create("Tree tests");

	suite_add_tcase(s, tc_core);
	tcase_add_test(tc_core, test_node_creation);
	tcase_add_test(tc_core, test_node_child_addition);
	tcase_add_test(tc_core, test_node_list_creation);
	tcase_add_test(tc_core, test_node_list_add_siblings);

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
