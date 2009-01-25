#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "common.h"
#include "errors.h"
#include "shell.h"
#include "node.h"

/* handle multiple root nodes */
struct node_list *_nodes = NULL;
unsigned int _nodenum = 0;

/* current root node */
struct node_list *_current = NULL;
unsigned int _current_num = 0;

struct _commands {
	char *command;
	void *func;
} commands[SHELL_N_FUNCS] = {
	{ "createroot", cmd_create_root },
	{ "deleteroot",	cmd_delete_root },
	{ "getroot",		cmd_get_root },
	{ "listroot",		cmd_list_root },
	{ "ls",					cmd_ls },
	{ "setroot",		cmd_set_root },
};

void
shell(void) {
	unsigned int exit = 0;
	char *line;

	/* set up completion */
	rl_attempted_completion_function = shell_completion;

	do {
		line = readline(RL_PROMPT);

		if ((line == NULL) || (!strcmp(line, "exit"))) {
			printf("\n");
			exit = 1;
		} else {
			if (*line)
				add_history(line);
			shell_parse_line(line);
		}

		if (line)
			free(line);
	} while (!exit);

	shell_cleanup();
}

/* Attempt to complete on the contents of TEXT.  START and END show the
 * region of TEXT that contains the word to complete.  We can use the
 * entire line in case we want to do some simple parsing.  Return the
 * array of matches, or NULL if there aren't any. */
char **
shell_completion(const char *text, int start, int end) {
	char **matches;
	matches = (char **)NULL;

	/* If this word is at the start of the line, then it is a command
	 * to complete.  Otherwise it is the name of a file in the current
	 * directory. */
	if (start == 0)
		matches = rl_completion_matches(text, shell_command_generator);

  return (matches);
}

/* Generator function for command completion.  STATE lets us know whether
 * to start from scratch; without any state (i.e. STATE == 0), then we
 * start at the top of the list. */
char *
shell_command_generator (const char *text, int state) {
	static int list_index, len;
	char *name;

	/* If this is a new word to complete, initialize now.  This includes
	 * saving the length of TEXT for efficiency, and initializing the index
	 * variable to 0. */
	if (!state) {
		list_index = 0;
		len = strlen(text);
	}

	/* Return the next name which partially matches from the command list. */
	while ((name = commands[list_index++].command) != NULL && list_index <= SHELL_N_FUNCS) {
		if (strncmp (name, text, len) == 0)
			return (strdup(name));
	}

	/* If no names matched, then return NULL. */
	return ((char *)NULL);
}


void
shell_parse_line(char *line) {
	unsigned int i = 0, j = 0;
	char *cmd = (char *)NULL;
	char *tmp;
	int (*call)(char *);
	int ret;

	/* skip white spaces */
	while (line[i] == ' ')
		i++;

	while (line[i] && line[i] != ' ') {
		j++;
		i++;
	}

	cmd = (char *)malloc(i + 1);
	memset(cmd, '\0', i + 1);
	strncpy(cmd, &line[i - j], i);
	call = shell_binsearch_cmd(cmd);

	if (!call) {
		printf("Command not found\n");
	} else {
		while (line[i] == ' ')
			i++;

		/* dup the string so we can eventually modify that
		 * within the various cmd_* functions
		 */
		tmp = strdup(&line[i]);
		ret = call(tmp);
		if (tmp)
			free(tmp);

		switch(ret) {
			case E_INVALID_SYNTAX:
				printf("Invalid syntax\n");
				break;
			case E_CANNOT_PROCEED:
				printf("Hit resource limits\n");
				break;
			case E_OUT_OF_BOUNDS:
				printf("Invalid range (out of bounds)\n");
				break;
		}
	}

	if (cmd)
		free(cmd);
}

void *
shell_binsearch_cmd(char *cmd) {
	int low, high, mid, cond = 0;

	if (!cmd || !*cmd)
		return NULL;

	low = 0;
	high = SHELL_N_FUNCS - 1;

	while (low <= high) {
		mid = (low + high) / 2;

		cond = strcmp(commands[mid].command, cmd);
		if (cond > 0)
			high = mid - 1;
		else if (cond < 0)
			low = mid + 1;
		else
			return commands[mid].func;
	}

	return NULL;
}

void
shell_cleanup(void) {
	/* Cleans data structures used in the shell
	 * (mostly reference to root nodes
	 */
	struct node_list *tmp;
	tmp = _nodes;

	while (tmp != NULL) {
		node_delete(tmp->node);
		tmp = tmp->next;
	}
}

int
cmd_ls(char *argline) {
	if (!strcmp(argline, "."))
		return E_INVALID_SYNTAX;

	printf("ls OK\n");
	return 1;
}

int
cmd_create_root(char *argline) {
	struct node *n;

	if (strlen(argline) == 0)
		return E_INVALID_SYNTAX;
	
	if (_nodenum >= MAX_ROOT_NODES)
		return E_CANNOT_PROCEED;
	
	n = node_create(argline, N_DIRECTORY);

	if (_nodes == NULL) {
		_nodes = node_list_create();
		_nodes->node = n;
	} else
		node_list_add_sibling(_nodes, n);

	_nodenum++;

	return 1;
}

int
cmd_list_root(char *argline) {
	unsigned int i = 0;
	struct node_list *tmp;

	if (_nodenum == 0)
		printf("No root nodes\n");
	else {
		tmp = _nodes;
		while (tmp != NULL) {
			printf("%2d: %s\n", ++i, tmp->node->name);
			tmp = tmp->next;
		}
	}

	return 1;
}

int
cmd_delete_root(char *argline) {
	unsigned int rootnum;
	struct node_list *prev = _nodes, *next;
	struct node_list *deletion;

	rootnum = atoi(argline);
	if (rootnum == 0)
		return E_INVALID_SYNTAX;

	deletion = _node_from_num(rootnum);
	if (deletion == NULL)
		return E_OUT_OF_BOUNDS;

	/* determine previous and next list item (se we relink stuff together */
	next = deletion->next;
	if (_nodes == deletion)
		/* if we're deleting the head of the list... */
		_nodes = next;
	else {
		while (prev->next != NULL) {
			if (prev->next == deletion) {
				prev->next = next;
				break;
			} else prev = prev->next;
		}
	}
	_nodenum--;

	if (_current == deletion) {
		_current = NULL;
		_current_num = 0;
	}

	node_delete(deletion->node);
	free(deletion);

	return 1;
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

	_set_current_root(root, rootnum);

	return 1;
}

int
cmd_get_root(char *argline) {
	if (*argline)
		return E_INVALID_SYNTAX;

	if (_current == NULL)
		printf("No current root node set\n");
	else printf("%0d: %s\n", _current_num, _current->node->name);

	return 1;
}

struct node_list *
_node_from_num(unsigned int num) {
	/* Given the ordinal number returned from listroot, returns
	 * the specified root node
	 */
	unsigned int i = 0;
	struct node_list *tmp = _nodes;

	if (num > _nodenum)
		return NULL;

	while (tmp != NULL && i <= num) {
		if (++i == num)
			break;
		tmp = tmp->next;
	}

	return tmp;
}

void
_set_current_root(struct node_list *root, unsigned int rootnum) {
	_current = root;
	_current_num = rootnum;
}

unsigned int
_get_current_root(void) {
	return _current_num;
}
