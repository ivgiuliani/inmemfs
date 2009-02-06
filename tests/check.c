#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/errors.h"
#include "../src/shell.h"
#include "../src/commands.h"
#include "../src/node.h"

START_TEST (node_creation)
{
	struct node *node;

	node = node_create("file-node", N_FILE);
	fail_if (strcmp(node->name, "file-node") != 0,
			"Node name is not set correctly");
	fail_if (node->type != N_FILE,
			"Node type is not set correctly");
	node_delete(node);

	node = node_create("directory-node", N_DIRECTORY);
	fail_if (strcmp(node->name, "directory-node") != 0,
			"Node name is not set correctly");
	fail_if (node->type != N_DIRECTORY,
			"Node type is not set correctly");
	node_delete(node);
}
END_TEST

START_TEST (node_child_addition)
{
	struct node *father, *children1, *children2;

	father = node_create("Father", N_DIRECTORY);
	children1 = node_create("Children_1", N_FILE);
	children2 = node_create("Children_2", N_FILE);

	fail_unless (node_add_child(father, children1) != E_FILE_CHILD,
			"Can't add a children to a father node");

	fail_if (node_add_child(children1, children2) != E_FILE_CHILD,
			"Added a file node to a directory node");

	fail_unless (node_get_children_no(father) == 1,
			"Children number returned doesn't match real children number");

	node_delete(father);
}
END_TEST

START_TEST (node_add_child_alphabetical_ordering)
{
	struct node *father = node_create("Father", N_DIRECTORY);
	struct node *alpha, *beta, *gamma;

	alpha = node_create("alpha", N_FILE);
	beta = node_create("beta", N_FILE);
	gamma = node_create("gamma", N_FILE);

	node_add_child(father, beta);
	node_add_child(father, alpha);
	node_add_child(father, gamma);

	fail_if (strcmp(node_get_nth_children(father, 0)->name, "alpha") != 0);
	fail_if (strcmp(node_get_nth_children(father, 1)->name, "beta") != 0);
	fail_if (strcmp(node_get_nth_children(father, 2)->name, "gamma") != 0);

	node_delete(father);
}
END_TEST

START_TEST (node_add_child_duplicate_name)
{
	struct node *father = node_create("Father", N_DIRECTORY);
	struct node *children1 = node_create("children", N_FILE);
	struct node *children2 = node_create("children", N_FILE);

	node_add_child(father, children1);
	fail_unless (node_add_child(father, children2) == E_NAME_EXISTS);

	node_delete(children2);
	node_delete(father);
}
END_TEST

START_TEST (node_count_childrens)
{
	struct node *father = node_create("Test-root-node", N_DIRECTORY);
	struct node *children1 = node_create("Children_1", N_FILE);
	struct node *children2 = node_create("Children_2", N_FILE);
	struct node *children3 = node_create("Children_3", N_FILE);

	fail_unless (node_children_num(father) == 0);

	node_add_child(father, children1);
	fail_unless (node_children_num(father) == 1);

	node_add_child(father, children2);
	fail_unless (node_children_num(father) == 2);

	node_add_child(father, children3);
	fail_unless (node_children_num(father) == 3);

	node_delete(father);
}
END_TEST

START_TEST (node_delete_children)
{
	struct node *father = node_create("Test-root-node", N_DIRECTORY);
	struct node *children1 = node_create("Children_1", N_FILE);
	struct node *children2 = node_create("Children_2", N_FILE);
	struct node *children3 = node_create("Children_3", N_FILE);

	node_add_child(father, children1);
	node_add_child(father, children2);
	node_add_child(father, children3);

	/* delete in the middle */
	node_delete_child(father, children2);
	fail_unless (node_children_num(father) == 2);

	/* delete the first item */
	node_delete_child(father, children1);
	fail_unless (node_children_num(father) == 1);

	/* and a unique element */
	node_delete_child(father, children3);
	fail_unless (node_children_num(father) == 0);
}
END_TEST

START_TEST (node_can_get_father)
{
	struct node *father = node_create("Father", N_DIRECTORY);
	struct node *children = node_create("Children", N_FILE);

	node_add_child(father, children);

	fail_unless (node_get_father(children) == father);

	node_delete(father);
}
END_TEST

START_TEST (node_check_valid_names)
{
	struct node *hyphens = node_create("n-o-d-e", N_FILE);
	struct node *underscores = node_create("n_o_d_e", N_FILE);
	struct node *mixedcase = node_create("AaBbZz", N_FILE);
	struct node *numbers = node_create("1234567890", N_FILE);
	struct node *mixed = node_create("abc-ABC_123", N_FILE);

	fail_if (hyphens == NULL);
	fail_if (underscores == NULL);
	fail_if (mixedcase == NULL);
	fail_if (numbers == NULL);
	fail_if (mixed == NULL);

	node_delete(hyphens);
	node_delete(underscores);
	node_delete(mixedcase);
	node_delete(numbers);
	node_delete(mixed);
}
END_TEST

START_TEST (node_check_invalid_names)
{
	struct node *spaces = node_create("s p a c e", N_FILE);
	struct node *apostrophe = node_create("I can't have this", N_FILE);
	struct node *strange = node_create("/!)!=", N_FILE);

	fail_if (spaces != NULL);
	fail_if (apostrophe != NULL);
	fail_if (strange != NULL);
}
END_TEST

START_TEST (node_list_creation)
{
	struct node_list *nl = NULL;

	nl = node_list_create();

	fail_if (nl == NULL,
			"Node list hasn't been created");
}
END_TEST

START_TEST (node_list_add_siblings)
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

START_TEST (shell_invalid_command)
{
	int ret = shell_parse_line("Invalid command");
	fail_unless (ret == E_CMD_NOT_FOUND,
			"Invalid command produces unexpected output");
}
END_TEST

START_TEST (shell_invalid_syntax)
{
	int ret;

	ret = shell_parse_line("createroot");
	fail_unless (ret == E_INVALID_SYNTAX,
			"Invalid syntax is not recognized on createroot");

	ret = shell_parse_line("setroot a");
	fail_unless (ret == E_INVALID_SYNTAX,
			"Invalid syntax is not recognized on setroot");
}
END_TEST

START_TEST (shell_root_limits)
{
	int i, ret;

	for(i = 0; i < MAX_ROOT_NODES; i++)
		shell_parse_line("createroot xxx");

	ret = shell_parse_line("createroot over_limit");

	/* revert changes back */
	for(i = 0; i < MAX_ROOT_NODES; i++)
		shell_parse_line("deleteroot 1");

	fail_unless(ret == E_CANNOT_PROCEED,
			"Resource limits are not hitted");
}
END_TEST

START_TEST (shell_no_root_selected)
{
	int ret = shell_parse_line("mkdir testdir");
	fail_unless(ret == E_NO_ROOT,
			"A root node is found even if none is selected");
}
END_TEST

START_TEST (shell_delete_dir)
{
	int ret;

	shell_parse_line("createroot testroot");
	shell_parse_line("setroot 1");
	shell_parse_line("mkdir test1");
	shell_parse_line("mkdir test2");
	shell_parse_line("mkdir test3");

	ret = shell_parse_line("rmdir test1");
	fail_unless(ret == EXIT_SUCCESS);

	ret = shell_parse_line("rmdir test2");
	fail_unless(ret == EXIT_SUCCESS);

	ret = shell_parse_line("rmdir test3");
	fail_unless(ret == EXIT_SUCCESS);

}
END_TEST

START_TEST (shell_add_duplicate_name)
{
	int ret;

	shell_parse_line("createroot testroot");
	shell_parse_line("setroot 1");
	shell_parse_line("mkdir test");
	ret = shell_parse_line("mkdir test");

	fail_unless (ret == E_NAME_EXISTS);
}
END_TEST

START_TEST (shell_change_dir)
{
	int ret;

	shell_parse_line("createroot testroot");
	shell_parse_line("setroot 1");
	shell_parse_line("mkdir test");

	ret = shell_parse_line("cd unknown");
	fail_unless (ret == E_DIR_NOT_FOUND);

	ret = shell_parse_line("cd test");
	fail_unless (ret == EXIT_SUCCESS);

	shell_parse_line("mkdir subtest");
	ret = shell_parse_line("cd subtest");
	fail_unless (ret == EXIT_SUCCESS);

	ret = shell_parse_line("cd ..");
	fail_unless (ret == EXIT_SUCCESS);

	ret = shell_parse_line("cd ..");
	fail_unless (ret == EXIT_SUCCESS);

	ret = shell_parse_line("cd ..");
	fail_unless (ret == E_DIR_NOT_FOUND);
}
END_TEST

Suite *
inmemfs_suite(void) {
	Suite *s = suite_create("Master");

	TCase *tc_tree = tcase_create("Tree tests");
	TCase *tc_shell = tcase_create("Shell tests");

	suite_add_tcase(s, tc_tree);
	tcase_add_test(tc_tree, node_creation);
	tcase_add_test(tc_tree, node_child_addition);
	tcase_add_test(tc_tree, node_add_child_alphabetical_ordering);
	tcase_add_test(tc_tree, node_add_child_duplicate_name);
	tcase_add_test(tc_tree, node_list_creation);
	tcase_add_test(tc_tree, node_list_add_siblings);
	tcase_add_test(tc_tree, node_count_childrens);
	tcase_add_test(tc_tree, node_delete_children);
	tcase_add_test(tc_tree, node_can_get_father);
	tcase_add_test(tc_tree, node_check_valid_names);
	tcase_add_test(tc_tree, node_check_invalid_names);

	suite_add_tcase(s, tc_shell);
	tcase_add_test(tc_shell, shell_invalid_command);
	tcase_add_test(tc_shell, shell_invalid_syntax);
	tcase_add_test(tc_shell, shell_root_limits);
	tcase_add_test(tc_shell, shell_no_root_selected);
	tcase_add_test(tc_shell, shell_delete_dir);
	tcase_add_test(tc_shell, shell_change_dir);

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
