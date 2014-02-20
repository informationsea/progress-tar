#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([progresstar], [0.1], [okamura@informationsea.info])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall])

AC_CONFIG_SRCDIR([src/progresstar.c])
AC_CONFIG_HEADERS([config.h])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

CPPFLAGS+=" -Wall -Wextra "

# Checks for programs.
AM_PROG_AR
AC_CONFIG_MACRO_DIR([m4])
AM_SILENT_RULES([yes])
AM_PROG_LIBTOOL([shared])

m4_ifdef([AC_CHECK_GCUTTER], [AC_CHECK_GCUTTER], [ac_cv_use_cutter="no"])
AM_CONDITIONAL([WITH_CUTTER], [test "$ac_cv_use_cutter" != "no"])

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
LT_INIT

# Checks for libraries.
PKG_CHECK_MODULES(zlib, zlib >= 0.28)
PKG_CHECK_MODULES(libarchive, libarchive >= 3.0)
#PKG_CHECK_MODULES(libbz2, libbz2 >= 1.0)
PKG_CHECK_MODULES(liblzma, liblzma >= 5.0)

# Checks for header files.
AC_CHECK_HEADER_STDBOOL
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT64_T

AC_CHECK_FUNCS([strchr strndup strrchr mkdir memmove memset])

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 test/Makefile
		 samples/Makefile])
AC_OUTPUT
