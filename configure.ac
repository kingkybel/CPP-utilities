# automake input file
# Note: automake for this project is no longer maintained
#       use 'cmake' instead
#       Last tested 01/09/2023
#       
#       BOOST - path hard-coded and not auto-detected
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([DKyb-Cxx-Utilities], [1.0], [github@kybelksties.com])
AC_LANG_SOURCE()
AC_CONFIG_MACRO_DIR([/usr/local/share/aclocal])
AC_CONFIG_SRCDIR([src/anyutil.cc])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([subdir-objects -Wall -Werror foreign])

# Checks for programs.
AM_PROG_AR
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB

# Checks for libraries.
# AX_BOOST_BASE
# AX_BOOST_FILESYSTEM

# Checks for header files.
AC_CHECK_HEADERS([string.h sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([floor memset])

PKG_CHECK_MODULES([CHECK], [check >= 0.9.10])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
