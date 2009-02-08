#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "config.h"
#include "common.h"
#include "errors.h"
#include "shell.h"
#include "commands.h"
#include "node.h"
#include "parser.h"

/* handle multiple root nodes */
struct node_list *_nodes = NULL;
unsigned int _nodenum = 0;

/* current root node */
struct node_list *_current_root = NULL;

/* current node */
struct node *_current = NULL;

struct _commands {
	char *command;
	void *func;
} commands[SHELL_N_FUNCS] = {
	{ "cd",         cmd_cd },
	{ "copyto",     cmd_copyto },
	{ "createroot", cmd_create_root },
	{ "deleteroot", cmd_delete_root },
	{ "getroot",    cmd_get_root },
	{ "listroot",   cmd_list_root },
	{ "ls",         cmd_ls },
	{ "mkdir",      cmd_mkdir },
	{ "mkfile",     cmd_mkfile },
	{ "rmdir",      cmd_rmdir },
	{ "setroot",    cmd_set_root },
};

void
shell(void) {
	unsigned int exit = 0;
	char *line;

	/* set up completion */
	rl_attempted_completion_function = shell_completion;

	shell_hello();

	do {
		line = readline(RL_PROMPT);

		if ((line == NULL) || (!strcmp(line, "exit"))) {
			printf("\n");
			exit = 1;
		} else {
			if (!*line) {
				if (line)
					free(line);
				continue;
			}

			shell_err_matcher(shell_parse_line(line));
		}

		if (line) {
			add_history(line);
			free(line);
		}
	} while (!exit);

	shell_cleanup();
}

void
shell_err_matcher(int return_code) {
	switch(return_code) {
		case E_CMD_NOT_FOUND:
			printf("Command not found\n");
			break;
		case E_FILE_CHILD:
			printf("Can't add a child to a FILE node\n");
			break;
		case E_INVALID_SYNTAX:
			printf("Invalid syntax\n");
			break;
		case E_CANNOT_PROCEED:
			printf("Hit resource limits\n");
			break;
		case E_OUT_OF_BOUNDS:
			printf("Invalid range (out of bounds)\n");
			break;
		case E_NO_ROOT:
			printf("No root node selected\n");
			break;
		case E_DIR_NOT_FOUND:
		case E_FILE_NOT_FOUND:
			printf("File or directory not found\n");
			break;
		case E_INVALID_TYPE:
			printf("Invalid node type\n");
			break;
		case E_NAME_EXISTS:
			printf("A file or a directory with this name already exists\n");
			break;
		case E_INVALID_NAME:
			printf("The argument contains invalid characters\n");
			break;
		case E_TOO_MANY_ARGS:
			printf("Too many arguments passed to the command\n");
			break;
		case E_CANT_GET_EXT_FILE:
			printf("Can't access to the specified file\n");
			break;
		}
}

void
shell_hello(void) {
	printf("-- "PACKAGE_NAME" shell ("PACKAGE_VERSION") --\n\n");
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
shell_command_generator(const char *text, int state) {
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

int
shell_parse_line(char *line) {
	unsigned int i = 0, j = 0;
	char *cmd = (char *)NULL;
	char *tmp;
	int (*call)(char *);
	int ret;

	ret = EXIT_SUCCESS;

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
		ret = E_CMD_NOT_FOUND;
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
	}

	if (cmd)
		free(cmd);

	return ret;
}

/*
 * Parse the argument line for shell commands
 */
int
shell_parse_argline(char *argline, char **arguments) {
	return parser_split(argline, ' ', arguments, MAX_ARG_NUM);
}

/*
 * Cleanup the parsed argline.
 * This function MUST be called ALWAYS after you're done
 * with the argline
 */
void
shell_free_parsed_argline(char **argline, int arg_no) {
	parser_free_splitted(argline, arg_no);
}

/*
 * Search for an existing command by using binary search
 */
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
	 * (mostly references to root nodes)
	 */
	struct node_list *tmp;
	tmp = _nodes;

	shell_set_curr_node(NULL);

	while (tmp != NULL) {
		node_delete(tmp->node);
		tmp = tmp->next;
	}
}

/*
 * Get the current root node or NULL if none
 */
struct node_list *
shell_get_root() {
	return _current_root;
}

/*
 * Set the current root node
 */
void
shell_set_root(struct node_list *root) {
	_current_root = root;
	shell_set_curr_node(root->node);
}

/*
 * Get the number of current root nodes
 */
unsigned int
shell_get_roots_num() {
	return _nodenum;
}

/*
 * Set the number of current root nodes
 */
void
shell_set_roots_num(unsigned int num) {
	_nodenum = num;
}

/*
 * Returns a reference to the root's linked list
 */
struct node_list *
shell_get_root_reference() {
	return _nodes;
}

/*
 * Set a reference to the root's linked list
 */
void
shell_set_root_reference(struct node_list *root) {
	_nodes = root;
}

/*
 * Get the current node
 */
struct node *
shell_get_curr_node() {
	return _current;
}

/*
 * Set the current node
 */
void
shell_set_curr_node(struct node *node) {
	_current = node;
}

/* Given the ordinal number returned from listroot, returns
 * the specified root nodo
 */
struct node_list *
_node_from_num(unsigned int num) {
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

