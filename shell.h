#ifndef _SHELL_H
#define _SHELL_H

#include "node.h"

#define SHELL_N_FUNCS 4
#define MAX_CMD_LEN 20

void shell(void);
void *shell_binsearch_cmd(char *);
void shell_parse_line(char *);
void shell_cleanup(void);
char **shell_completion(const char *, int, int);
char *shell_command_generator(const char *, int);

int cmd_create_root(char *);
int cmd_delete_root(char *);
int cmd_list_root(char *);
int cmd_ls(char *);

/* private functions */
struct node_list *_node_from_num(int);

#endif /* _SHELL_H */
