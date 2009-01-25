#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "errors.h"
#include "node.h"

struct node *
node_create(char *name, enum node_type type) {
	struct node *n;

	n = (struct node *)malloc(sizeof(struct node));
	n->type = type;
	n->children_no = 0;

	/* defer initalizations of childrens until we're actually adding
	 * a new children
	 */
	n->childrens = NULL;

	if (node_set_name(n, name) < 0)
		return NULL;

	return n;
}

int
node_set_name(struct node *node, char *name) {
	if (strlen(name) > MAX_NAME_LENGTH)
		return E_CONSTRAINT_VIOLATED;

	strcpy(node->name, name);
	return 1;
}

void
node_delete(struct node *n) {
	struct node *subnode;
	struct node_list *nl, *next;

	if (n->childrens != NULL) {
		nl = n->childrens;

		while (nl != NULL) {
			subnode = nl->node;
			node_delete(subnode);

			next = nl->next;
			free(nl);
			nl = next;
		}
	}

	free(n);
}

int
node_add_child(struct node *father, struct node *children) {
	struct node_list *tmp, *nl, *tmpnl;
	unsigned int children_no = node_get_children_no(father);

	if (father->type == N_FILE) {
		return E_FILE_CHILD;
	}

	if (children_no == 0) {
		/* initalizes the children list (we have deferred its initalization */
		father->childrens = (struct node_list *)malloc(sizeof(struct node_list));
		memset(father->childrens, 0, sizeof(struct node_list));
		father->childrens->node = children;
	} else {
		tmp = node_get_nth_children_nl(father, children_no);
		nl = node_list_create();
		nl->node = children;

		tmpnl = father->childrens;
		while (tmpnl->next != NULL)
			tmpnl = tmpnl->next;
		tmpnl->next = nl;
	}
	father->children_no += 1;

	return 1;
}

struct node_list *
node_get_nth_children_nl(struct node *n, int no) {
	int i;
	struct node_list *nl;

	nl = n->childrens;
	for (i = 0; i < no; i++)
		nl = nl->next;
	return nl;
}

struct node *
node_get_nth_children(struct node *n, int no) {
	return node_get_nth_children_nl(n, no)->node;
}

unsigned int
node_get_children_no(struct node *n) {
	return n->children_no;
}

struct node_list *
node_list_create() {
	struct node_list *nl;
	nl = (struct node_list *)malloc(sizeof(struct node_list));
	nl->next = NULL;
	return nl;
}

struct node *
node_list_add_sibling(struct node_list *list, struct node *node) {
	struct node_list *nl = node_list_create();
	struct node_list *tmp = list;

	while (tmp->next != NULL)
		tmp = tmp->next;

	tmp->next = nl;
	nl->node = node;

	return node;
}
