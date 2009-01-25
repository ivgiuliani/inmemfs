AC_INIT([inmemfs], [0.1], ['giuliani.v@gmail.com'])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/main.c])

AC_CHECK_LIB([readline], [readline])
AC_CHECK_HEADERS([readline/readline.h])

AM_INIT_AUTOMAKE([-Wall -Werror foreign dist-bzip2])
AC_PROG_CC
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
