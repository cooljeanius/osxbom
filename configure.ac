dnl#                                               -*- Autoconf -*-
dnl# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])dnl
AC_INIT([osxbom],[0.0.5],[https://github.com/cooljeanius/osxbom/issues])dnl
AC_CONFIG_SRCDIR([bom.h])dnl
AC_CONFIG_AUX_DIR([build-aux])dnl
AC_CONFIG_HEADERS([config.h])dnl

dnl# Set up Automake.
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([1.11.6 gnits dist-zip])dnl
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_C_O
AC_PROG_CPP
AM_PROG_AS
AC_LANG([C++])dnl
AC_PROG_CXX
AC_PROG_CXX_C_O
AC_PROG_CXXCPP
AC_PATH_PROGS([RM_PROG],[rm grm])dnl
AC_SUBST([RM_PROG])dnl
AC_PATH_PROGS([ROFF_PROG],[nroff groff roff])dnl
AC_SUBST([ROFF_PROG])dnl

# Checks for header files.
AC_DEFUN([AC_REQUIRE_HEADER_STDC],[
  AC_REQUIRE([AC_HEADER_STDC])dnl
])dnl
AC_REQUIRE_HEADER_STDC
AC_HEADER_STDBOOL dnl# also "_CHECK"s it
AC_CHECK_HEADERS([arpa/inet.h ctype.h getopt.h mach-o/arch.h])dnl

if test "x${ac_cv_header_stdint_h}" = "x"; then
  test -z "${ac_cv_header_stdint_h}"
  AC_CHECK_HEADERS([stdint.h])
fi
if test "x${ac_cv_header_stdio_h}" = "x"; then
  test -z "${ac_cv_header_stdio_h}"
  AC_CHECK_HEADERS([stdio.h])
fi
if test "x${ac_cv_header_unistd_h}" = "x"; then
  test -z "${ac_cv_header_unistd_h}"
  AC_CHECK_HEADERS([unistd.h])
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_C_BIGENDIAN
AC_C_CONST
AC_C_FLEXIBLE_ARRAY_MEMBER
AC_C_PROTOTYPES
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_CHECK_TYPES([NXArchInfo],[],[],[
#ifdef HAVE_MACH_O_ARCH_H
# include <mach-o/arch.h>
#else
# if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#  warning "this conftest for NXArchInfo wants to include <mach-o/arch.h>"
# endif /* __GNUC__ && !__STRICT_ANSI__ */
#endif /* MACH_O_ARCH_H */
])dnl

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
dnl# some of these are only used by the system implementation, and NOT by
dnl# this one:
AC_CHECK_FUNCS([NXGetArchInfoFromName atoi cerr cout close exit \
                fflush fprintf fwrite getopt getopt_long getopt_long_only \
                getprogname isprint ntohl ntohs printf puts read \
                setfill setw strcmp strcpy strlen what])dnl
AC_CHECK_DECLS([ntohl, ntohs])dnl

# Output.
AC_CONFIG_FILES([Makefile Makefile_orig])dnl
AC_OUTPUT
