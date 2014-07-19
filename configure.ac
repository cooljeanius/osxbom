dnl#                                               -*- Autoconf -*-
dnl# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([osxbom],[0.0.3],[])
AC_CONFIG_SRCDIR([bom.h])
AC_CONFIG_HEADER([config.h])

dnl# Set up Automake.
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([1.10 foreign])
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_C_O
AC_PROG_CPP
AC_LANG([C++])dnl
AC_PROG_CXX
AC_PROG_CXX_C_O
AC_PROG_CXXCPP
AC_PATH_PROGS([RM_PROG],[rm grm])dnl
AC_SUBST([RM_PROG])dnl

# Checks for header files.
AC_DEFUN([AC_REQUIRE_HEADER_STDC],[
  AC_REQUIRE([AC_HEADER_STDC])dnl
])dnl
AC_REQUIRE_HEADER_STDC
AC_HEADER_STDBOOL dnl# also "_CHECK"s it
AC_CHECK_HEADERS([arpa/inet.h ctype.h stdio.h])dnl

# Checks for typedefs, structures, and compiler characteristics.
AC_C_BIGENDIAN
AC_C_CONST
AC_C_PROTOTYPES
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([atoi getopt exit ntohl strlen what])dnl
AC_CHECK_DECLS([ntohl])dnl

# Output.
AC_CONFIG_FILES([Makefile Makefile.orig])dnl
AC_OUTPUT
