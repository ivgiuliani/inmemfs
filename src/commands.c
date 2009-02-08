#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "shell.h"
#include "errors.h"

int
cmd_mkdir(char *argline) {
	struct node *n;

	if (!*argline)
		return E_INVALID_SYNTAX;

	if (shell_get_root() == NULL)
		return E_NO_ROOT;

	if (shell_get_curr_node() == NULL)
		return E_NO_DIR;

	n = node_create(argline, N_DIRECTORY);
	if (n == NULL) {
		return E_INVALID_NAME;
	} else return node_add_child(shell_get_curr_node(), n);
}

int
cmd_rmdir(char *argline) {
	struct node *node;

	if (!*argline)
		return E_INVALID_SYNTAX;

	if (shell_get_root() == NULL)
		return E_NO_ROOT;

	if (shell_get_curr_node() == NULL)
		return E_NO_DIR;

	node = node_find_children(shell_get_curr_node(), argline);
	if (node == NULL)
		return E_DIR_NOT_FOUND;
	if (node->type != N_DIRECTORY)
		return E_INVALID_TYPE;

	node_delete_child(shell_get_curr_node(), node);
	return EXIT_SUCCESS;
}

int
cmd_ls(char *argline) {
	struct node_list *nl;

	if (shell_get_root() == NULL)
		return E_NO_ROOT;

	if (shell_get_curr_node() == NULL)
		return E_NO_DIR;

	if (shell_get_curr_node()->children_no != 0) {
		nl = shell_get_curr_node()->childrens;
		while (nl != NULL) {
			printf("%s\n", nl->node->name);
			nl = nl->next;
		}
	}

	return EXIT_SUCCESS;
}

int
cmd_cd(char *argline) {
	struct node *node;

	if (shell_get_root() == NULL)
		return E_NO_ROOT;

	if (shell_get_curr_node() == NULL)
		return E_NO_DIR;

	if (!strcmp(argline, NODE_SELF))
		return EXIT_SUCCESS;

	if (!*argline)
		return E_INVALID_SYNTAX;

	if (!strcmp(argline, NODE_PARENT)) {
		node = node_get_father(shell_get_curr_node());
	} else {
		node = node_find_children(shell_get_curr_node(), argline);
	}

	if (node == NULL)
		return E_DIR_NOT_FOUND;

	if (node->type == N_FILE)
		return E_INVALID_TYPE;

	shell_set_curr_node(node);

	return EXIT_SUCCESS;
}

int
cmd_copyto(char *argline) {
	struct node *node;
	char *arguments[MAX_ARG_NUM], input_file_path[255];
	int arg_no, i;
	FILE *input_fd;

	if (shell_get_root() == NULL)
		return E_NO_ROOT;

	if (shell_get_curr_node() == NULL)
		return E_NO_DIR;

	if (!*argline)
		return E_INVALID_SYNTAX;

	arg_no = shell_parse_argline(argline, arguments);
	if (arg_no < 0)
		return arg_no;
	else if (arg_no != 2) {
		shell_free_parsed_argline(arguments, arg_no);
		return E_TOO_MANY_ARGS;
	}

	node = node_find_children(shell_get_curr_node(), arguments[0]);
	if (strlen(arguments[1]) > 255)
		return E_CANT_GET_EXT_FILE;

	strcpy(input_file_path, arguments[1]);
	shell_free_parsed_argline(arguments, arg_no);

	if (node == NULL)
		return E_DIR_NOT_FOUND;

	if (node->type != N_FILE)
		return E_INVALID_TYPE;

	input_fd = fopen(input_file_path, "r");
	printf("opening %s (%s)\n", input_file_path, input_fd);
	if (input_fd == NULL)
		return E_CANT_GET_EXT_FILE;

	fclose(input_fd);

	return EXIT_SUCCESS;
}

int
cmd_create_root(char *argline) {
	struct node *n;
	struct node_list *nl;

	if (strlen(argline) == 0)
		return E_INVALID_SYNTAX;
	
	if (shell_get_roots_num() >= MAX_ROOT_NODES)
		return E_CANNOT_PROCEED;
	
	n = node_create(argline, N_DIRECTORY);
	if (n == NULL)
		return E_INVALID_NAME;

	if (shell_get_root_reference() == NULL) {
		nl = node_list_create();
		nl->node = n;
		shell_set_root_reference(nl);
	} else node_list_add_sibling(shell_get_root_reference(), n);

	shell_set_roots_num(shell_get_roots_num() + 1);

	return EXIT_SUCCESS;
}

int
cmd_list_root(char *argline) {
	unsigned int i = 0;
	struct node_list *tmp;

	if (shell_get_roots_num() == 0)
		printf("No root nodes\n");
	else {
		tmp = shell_get_root_reference();
		while (tmp != NULL) {
			printf("%2d: %s\n", ++i, tmp->node->name);
			tmp = tmp->next;
		}
	}

	return EXIT_SUCCESS;
}

int
cmd_delete_root(char *argline) {
	unsigned int rootnum;
	struct node_list *prev = shell_get_root_reference(), *next;
	struct node_list *deletion;

	rootnum = atoi(argline);
	if (rootnum == 0)
		return E_INVALID_SYNTAX;

	deletion = _node_from_num(rootnum);
	if (deletion == NULL)
		return E_OUT_OF_BOUNDS;

	/* determine previous and next list item (se we relink stuff together */
	next = deletion->next;
	if (shell_get_root_reference() == deletion)
		/* if we're deleting the head of the list... */
		shell_set_root_reference(next);
	else {
		while (prev->next != NULL) {
			if (prev->next == deletion) {
				prev->next = next;
				break;
			} else prev = prev->next;
		}
	}
	shell_set_roots_num(shell_get_roots_num() - 1);

	if (shell_get_root() == deletion) {
		shell_set_root(NULL);
		shell_set_curr_node(NULL);
	}

	node_delete(deletion->node);
	free(deletion);

	return EXIT_SUCCESS;
}

int
cmd_set_root(char *argline) {
	unsigned int rootnum;
	struct node_list *root;

	rootnum = atoi(argline);
	if (rootnum == 0)
		return E_INVALID_SYNTAX;

	root = _node_from_num(rootnum);
	if (root == NULL)
		return E_OUT_OF_BOUNDS;

	shell_set_root(root);

	return EXIT_SUCCESS;
}

int
cmd_get_root(char *argline) {
	if (*argline)
		return E_INVALID_SYNTAX;

	if (shell_get_root() == NULL)
		printf("No current root node set\n");
	else printf("%s\n", shell_get_root()->node->name);

	return EXIT_SUCCESS;
}

int
cmd_mkfile(char *argline) {
	char *args[1];
	int arg_no;
	struct node *n;

	if (shell_get_root() == NULL)
		return E_NO_ROOT;

	if (shell_get_curr_node() == NULL)
		return E_NO_DIR;

	arg_no = shell_parse_argline(argline, args);

	if (arg_no < 0)
		return arg_no;
	else if (arg_no != 1) {
		shell_free_parsed_argline(args, arg_no);
		return E_INVALID_SYNTAX;
	}

	n = node_create(args[0], N_FILE);
	shell_free_parsed_argline(args, arg_no);
	if (n == NULL)
		return E_INVALID_NAME;
	else return node_add_child(shell_get_curr_node(), n);

	return EXIT_SUCCESS;
}

