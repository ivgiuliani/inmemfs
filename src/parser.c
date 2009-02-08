#include <string.h>

#include "errors.h"
#include "parser.h"

/*
 * Split a line using 'delimiter' as token.
 * Multiple successive occurrences of 'delimiter' are ignored
 * (i.e. a line like "split    this" with ' ' as delimiter
 * will be splitted as ['split', 'this']
 */
int
parser_split(char *line, const char delimiter, char **splitted, unsigned int max_values) {
	unsigned int i;
	unsigned int start;
	unsigned int arg_num = 0;

	if (!*line)
		return 0;

	/* skip white spaces before than the argline */
	while ((line[i] == delimiter) || (line[i] == '\0'))
		i++;

	start = i;
	while (i <= strlen(line)) {
		if ((line[i] == delimiter) || (line[i] == '\0')) {
			if (arg_num >= max_values)
				return E_TOO_MANY_ARGS;

			splitted[arg_num] = (char *)malloc(i - start + 1);
			strncpy(splitted[arg_num], &line[start], i - start);
			splitted[arg_num++][i - start] = '\0';

			/* skip trailing whitespaces */
			while (line[i] == ' ')
				i++;

			start = i + 1;
		}

		i++;
	}

	return arg_num;
}
