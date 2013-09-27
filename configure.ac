#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([dtsguiapp], [0.1], [gregory@distrotech.co.za])
AC_CONFIG_SRCDIR([src/cxxapi.cpp])
AC_CONFIG_HEADERS([config.h])

#x:y:z z <= x
#so.x.z.y

LT_LIBVER=0:1:0
AC_SUBST(LT_LIBVER)

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for libraries.
AM_INIT_AUTOMAKE
AC_CONFIG_MACRO_DIR([m4])
AC_LIBTOOL_WIN32_DLL
LT_INIT()
AM_PROG_CC_C_O

PKG_CHECK_MODULES(DTSAPPLIB, libdtsapp >= 0.2)

AC_ARG_ENABLE([dev-mode], AS_HELP_STRING([--enable-dev-mode], [Developer mode -g -Wall -Werror -pedantic-errors]))
AC_ARG_ENABLE([world-peace], AS_HELP_STRING([--enable-world-peace], [Yeah right .....]))

if test "x$enable_dev_mode" = "xyes" ; then
 DEVELOPER_CFLAGS="-g -Wall -Werror -pedantic-errors -Wno-long-long -Wno-unused-local-typedefs -Wno-variadic-macros"
 DEVELOPER_CXXFLAGS="-g -Wall -Werror -pedantic-errors -Wno-long-long -Wno-unused-local-typedefs -Wno-variadic-macros -Wno-conversion-null"
fi;
AC_SUBST(DEVELOPER_CFLAGS)
AC_SUBST(DEVELOPER_CXXFLAGS)

WX_CONFIG_OPTIONS
WX_CONFIG_CHECK([2.9.4], [wxWin=1])
	if test "$wxWin" != 1; then
	AC_MSG_ERROR([
		wxWidgets must be installed on your system
		but wx-config script couldn't be found.
		Please check that wx-config is in path, the directory
		where wxWidgets libraries are installed (returned by
		'wx-config --libs' command) is in LD_LIBRARY_PATH or
		equivalent variable and wxWidgets version is 2.9.4 or above.
	])
fi
AC_SUBST(WX_CPPFLAGS)
AC_SUBST(WX_CXXFLAGS_ONLY)
AC_SUBST(WX_CFLAGS_ONLY)
AC_SUBST(WX_LIBS)

# Checks for header files.
AC_CHECK_HEADERS([stdint.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 app/Makefile
                 share/Makefile
                 src/libdtsapp_gui.pc])
AC_OUTPUT
