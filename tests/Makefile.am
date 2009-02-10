TESTS = check_inmemfs
check_PROGRAMS = check_inmemfs
check_inmemfs_SOURCES = check.c                            \
												test_memory.c                      \
												$(top_builddir)/src/node.h         \
												$(top_builddir)/src/node.c         \
												$(top_builddir)/src/shell.h        \
												$(top_builddir)/src/shell.c        \
												$(top_builddir)/src/commands.h     \
												$(top_builddir)/src/commands.c     \
												$(top_builddir)/src/parser.h       \
												$(top_builddir)/src/parser.c       \
												$(top_builddir)/src/io.h           \
												$(top_builddir)/src/io.c           \
												$(top_builddir)/src/kalloc.h       \
												$(top_builddir)/src/kalloc.c

check_inmemfs_CFLAGS = @CHECK_CFLAGS@
check_inmemfs_LDADD = @CHECK_LIBS@

