#ifndef _SHELL_H
#define _SHELL_H

#include "node.h"

#define SHELL_N_FUNCS 12
#define MAX_CMD_LEN 20

void shell(void);
void shell_err_matcher(int);
void shell_hello(void);
void *shell_binsearch_cmd(char *);
int shell_parse_line(char *);
int shell_parse_argline(char *, char **);
void shell_free_parsed_argline(char **, int);
void shell_cleanup(void);
char **shell_completion(const char *, int, int);
char *shell_command_generator(const char *, int);
struct node_list *shell_get_root(void);
void shell_set_root(struct node_list *);
unsigned int shell_get_roots_num(void);
void shell_set_roots_num(unsigned int);
struct node_list *shell_get_root_reference();
void shell_set_root_reference(struct node_list *);
struct node *shell_get_curr_node(void);
void shell_set_curr_node(struct node *);

/* private functions */
struct node_list *_node_from_num(unsigned int);

#endif /* _SHELL_H */
