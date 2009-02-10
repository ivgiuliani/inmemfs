#include <check.h>
#include <stdlib.h>

#include "test_tree.h"
#include "test_memory.h"
#include "test_shell.h"

Suite *
inmemfs_suite(void) {
	Suite *s = suite_create("Master");

	suite_add_tcase(s, tcase_tree());
	suite_add_tcase(s, tcase_memory());
	suite_add_tcase(s, tcase_shell());

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
