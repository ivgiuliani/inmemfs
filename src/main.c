#include <stdio.h>
#include <stdlib.h>

#include "node.h"
#include "shell.h"

int
main(int argc, char **argv) {
	struct node *n_father = node_create("test node", N_DIRECTORY);
	struct node *n_child = node_create("first child", N_DIRECTORY);
	struct node *n_child2 = node_create("child of child", N_FILE);
	struct node *n_child3 = node_create("second child", N_FILE);

	printf("Node name: %s\n", n_father->name);

	node_add_child(n_father, n_child);
	node_add_child(n_father, n_child3);
	printf("Node child numbers: %d\n", node_get_children_no(n_father));
	printf("Child name: %s\n", node_get_nth_children(n_father, 0)->name);

	node_add_child(n_child, n_child2);
	printf("Second child: %s\n", node_get_nth_children(n_child, 0)->name);

	node_delete(n_father);

	shell();

	return EXIT_SUCCESS;
}
