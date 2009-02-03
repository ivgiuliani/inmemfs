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
	return 0;
}

void
node_delete(struct node *n) {
	struct node *subnode;
	struct node_list *nl, *next;

	if (n->childrens != NULL) {
		nl = n->childrens;

		while (nl != NULL) {
			subnode = nl->node;
			next = nl->next;

			node_delete(subnode);

			free(nl);
			nl = next;
		}
	}

	free(n);
	n = NULL;
}

void
node_delete_child(struct node *father, struct node *children) {
	struct node_list *nl = father->childrens, *delnl;
	struct node_list *prev = NULL;

	while (nl != NULL) {
		if (nl->node == children) {
			if (prev != NULL) {
				/* we're in the middle or at the end of the linked list */
				prev->next = nl->next;
			} else {
				/* we're at the first item of the linked list */
				father->childrens = nl->next;
			}

			delnl = nl;
			break;
		}
		prev = nl;
		nl = nl->next;
	}

	node_delete(children);
	free(delnl);
}

int
node_add_child(struct node *father, struct node *children) {
	struct node_list *tmp, *nl, *tmpnl;
	unsigned int children_no = node_get_children_no(father);

	if (father->type == N_FILE) {
		return E_FILE_CHILD;
	}

	if (children_no == 0) {
		/* initalizes the children list (we have deferred its initalization) */
		father->childrens = node_list_create();

		father->childrens->node = children;
		father->childrens->prev = NULL;
		father->childrens->next = NULL;
	} else {
		tmp = node_get_nth_children_nl(father, children_no);
		nl = node_list_create();
		nl->node = children;

		tmpnl = father->childrens;
		while (tmpnl->next != NULL)
			tmpnl = tmpnl->next;
		tmpnl->next = nl;
		nl->prev = tmpnl;
	}
	father->children_no += 1;

	return 0;
}

unsigned int
node_children_num(struct node *node) {
	unsigned int num = 0;
	struct node_list *nl = node->childrens;

	while (nl != NULL) {
		num++;
		nl = nl->next;
	}

	return num;
}

struct node *
node_find_children(struct node *father, char *name) {
	struct node *node = NULL;
	struct node_list *tmpnl;

	tmpnl = father->childrens;
	while (tmpnl != NULL) {
		if (!strcmp(tmpnl->node->name, name)) {
			node = tmpnl->node;
			break;
		}

		tmpnl = tmpnl->next;
	}

	return node;
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

