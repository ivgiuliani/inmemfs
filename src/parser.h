#ifndef PARSER_H
#define PARSER_H

int parser_split(char *, const char, char **, unsigned int);
void parser_free_splitted(char **, int);

#endif /* PARSER_H */
