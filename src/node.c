#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "errors.h"
#include "node.h"
#include "parser.h"

struct node *
node_create(char *name, enum node_type type) {
	struct node *n;

	n = (struct node *)malloc(sizeof(struct node));
	n->type = type;
	n->children_no = 0;
	n->father = NULL;

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
	unsigned int i;
	char c;

	if (strlen(name) > MAX_NAME_LENGTH)
		return E_CONSTRAINT_VIOLATED;

	for (i = 0; i < strlen(name); i++) {
		/* A"white list" approach in this case is easier
		 * to understand (for future reference)
		 */
		c = name[i];
		if ((c >= 'A' && c <= 'Z') ||
				(c >= 'a' && c <= 'z') ||
				(c >= '0' && c <= '9') ||
				(c == '.') ||
				(c == '-') ||
				(c == '_')) continue;
		else return E_INVALID_NAME;
	}

	strcpy(node->name, name);
	return 0;
}

/* Set a node's father */
void
node_set_father(struct node *node, struct node *father) {
	node->father = father;
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
	struct node_list *tmp, *nl, *tmpnl, *prev;
	unsigned int children_no = node_get_children_no(father);
	unsigned int inserted;
	short int name_comparison;

	if (father->type == N_FILE) {
		return E_FILE_CHILD;
	}

	if (children_no == 0) {
		/* this is the first child, so initalizes the children
		 * list (we have deferred its initalization) */
		father->childrens = node_list_create();

		father->childrens->node = children;
		father->childrens->prev = NULL;
		father->childrens->next = NULL;
	} else {
		/* there are childrens already, so insert this node in the
		 * proper place in alphabetical order
		 */

		nl = node_list_create();
		nl->node = children;

		tmpnl = father->childrens;
		prev = NULL;

		inserted = 0;
		while (tmpnl != NULL) {
			/* insert the node in the proper alphabetical place */
			name_comparison = strcmp(children->name, tmpnl->node->name);
			if (name_comparison < 0) {
				if (prev == NULL)
					father->childrens = nl;
				else prev->next = nl;

				nl->next = tmpnl;
				inserted = 1;
				break;
			} else if (name_comparison == 0) {
				/* a node with the same name already exists, exit
				 * with a proper error code
				 */
				return E_NAME_EXISTS;
			}

			prev = tmpnl;
			tmpnl = tmpnl->next;
		}

		if (!inserted)
			node_get_nth_children_nl(father, children_no - 1)->next = nl;
	}
	father->children_no += 1;
	node_set_father(children, father);

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

/* Returns a node's father or NULL if it's a root node */
struct node *
node_get_father(const struct node *children) {
	return children->father;
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
	/* Returns the Ith children (starts from 0) */
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

struct node *
node_path_find(struct node *root, char *path) {
	char *nodes[MAX_TREE_DEPTH];
	short int node_num, depth = 0;
	struct node *parent = root;

	node_num = parser_split(path, '/', nodes, MAX_TREE_DEPTH);
	if (node_num < 0)
		return NULL;

	while (depth < node_num) {
		parent = node_find_children(parent, nodes[depth++]);

		/* invalid path */
		if (parent == NULL) {
			parser_free_splitted(nodes, node_num);
			return NULL;
		}
	}

	parser_free_splitted(nodes, node_num);
	return parent;
}

