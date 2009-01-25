# stack makefile

CC	= gcc
CFLAGS	= -O2 -Wall -ggdb
LIB = -lreadline
OUT = inmemfs

FILES = \
	node.o		\
	shell.o		\
	main.o

all:	$(FILES)
	$(CC) $(CFLAGS) $(LIB) $(FILES) -o $(OUT)

clean:
	rm -f *.o $(OUT)
