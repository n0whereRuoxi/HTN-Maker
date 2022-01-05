#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)

# Modified by me.
AC_INIT([ch-htn-tools], [1.1], [cmh204@lehigh.edu])

# Added by me.
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_CONFIG_SRCDIR([funcs.cpp])

AC_CONFIG_HEADER([config.h])

#Added by me.
#AC_SUBST([DEB_DEFS], [NO_STR_TABLE NO_TERM_TABLE NO_MACROS])
#AC_SUBST([REL_DEFS], [CATCH_EXCEPTS])
#AC_SUBST([DEB_CXXFLAGS], [-g])
#AC_SUBST([REL_CXXFLAGS], [-O3]

#Added by me.
AC_CONFIG_FILES([
  Makefile
])

# Checks for programs.
AC_PROG_CXX
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.

#Added by me.
AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([sstream])
AC_CHECK_HEADERS([strstream])
AC_LANG_POP

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE

#Added by me.
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN(test, doxygen.cfg)

AC_OUTPUT
