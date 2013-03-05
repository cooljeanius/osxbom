#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([osxbom],[0.0.3])
AC_CONFIG_SRCDIR([bom.h])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE
AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CPP
AC_PATH_PROG([RM],[rm])
AC_SUBST(RM)

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h ctype.h stdio.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([getopt])

AC_CONFIG_FILES([Makefile Makefile.orig])
AC_OUTPUT
