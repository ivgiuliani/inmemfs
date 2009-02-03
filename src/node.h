#ifndef _NODE_H
#define _NODE_H

#include "common.h"

enum node_type { N_FILE, N_DIRECTORY };

struct node_list {
	struct node *node;
	struct node_list *next;
	struct node_list *prev;
};

struct node {
	char name[MAX_NAME_LENGTH];
	enum node_type type;

	unsigned int children_no;
	struct node_list *childrens;
};

struct node *node_create(char *, enum node_type);
int node_set_name(struct node *, char *);
void node_delete(struct node *);
void node_delete_child(struct node *, struct node *);
int node_add_child(struct node *, struct node *);
unsigned int node_children_num(struct node *);
struct node *node_find_children(struct node *, char *);
struct node_list *node_get_nth_children_nl(struct node *, int);
struct node *node_get_nth_children(struct node *, int);
unsigned int node_get_children_no(struct node *);
struct node_list *node_list_create(void);
struct node *node_list_add_sibling(struct node_list *, struct node *);

#endif /* _NODE_H */
