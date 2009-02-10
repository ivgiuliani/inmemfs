#include <check.h>
#include <stdlib.h>

#include "../src/errors.h"
#include "../src/shell.h"

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

START_TEST (shell_argline)
{
	char *res[10];
	short int no;

	no = shell_parse_argline("two arguments", res);
	fail_unless (no == 2);
	shell_free_parsed_argline(res, no);

	no = shell_parse_argline("", res);
	fail_unless (no == 0);
	shell_free_parsed_argline(res, no);

	no = shell_parse_argline("       ", res);
	fail_unless (no == 0);
	shell_free_parsed_argline(res, no);

	no = shell_parse_argline("arguments    with     a    lot   of   space", res);
	fail_unless (no == 6);
	shell_free_parsed_argline(res, no);
}
END_TEST

START_TEST (shell_invalid_chars_in_root)
{
	int ret = shell_parse_line("createroot i!have\\invalid?characters");
	fail_unless (ret == E_INVALID_NAME);
}
END_TEST

START_TEST (shell_make_file)
{
	int ret;

	shell_parse_line("createroot root");
	shell_parse_line("setroot 1");

	ret = shell_parse_line("mkfile");
	fail_unless (ret == E_INVALID_SYNTAX);

	ret = shell_parse_line("mkfile test");
	fail_unless (ret == EXIT_SUCCESS);
}
END_TEST

TCase *
tcase_shell(void) {
	TCase *tc_shell = tcase_create("Shell tests");

	tcase_add_test(tc_shell, shell_invalid_command);
	tcase_add_test(tc_shell, shell_invalid_syntax);
	tcase_add_test(tc_shell, shell_root_limits);
	tcase_add_test(tc_shell, shell_no_root_selected);
	tcase_add_test(tc_shell, shell_delete_dir);
	tcase_add_test(tc_shell, shell_change_dir);
	tcase_add_test(tc_shell, shell_argline);
	tcase_add_test(tc_shell, shell_invalid_chars_in_root);
	tcase_add_test(tc_shell, shell_make_file);

	return tc_shell;
}

