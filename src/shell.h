#ifndef _SHELL_H
#define _SHELL_H

#include "node.h"

#define SHELL_N_FUNCS 10
#define MAX_CMD_LEN 20

void shell(void);
void shell_err_matcher(int);
void shell_hello(void);
void *shell_binsearch_cmd(char *);
int shell_parse_line(char *);
void shell_cleanup(void);
char **shell_completion(const char *, int, int);
char *shell_command_generator(const char *, int);
struct node_list *shell_get_root(void);
void shell_set_root(struct node_list *);

int cmd_cd(char *);
int cmd_copyto(char *);
int cmd_create_root(char *);
int cmd_delete_root(char *);
int cmd_list_root(char *);
int cmd_mkdir(char *);
int cmd_rmdir(char *);
int cmd_get_root(char *);
int cmd_ls(char *);
int cmd_set_root(char *);

/* private functions */
struct node_list *_node_from_num(unsigned int);

#endif /* _SHELL_H */
