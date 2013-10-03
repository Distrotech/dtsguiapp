ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = -I$(srcdir)/../src/include $(DEVELOPER_CFLAGS) -I. -DDATA_DIR=\"$(datarootdir)/@PACKAGE@\"
AM_CXXFLAGS = -I$(srcdir)/../src/include $(DEVELOPER_CXXFLAGS) -I. -DDATA_DIR=\"$(datarootdir)/@PACKAGE@\"
EXTRA_DIST = include

bin_PROGRAMS = wxforms
wxforms_SOURCES = wxforms.c wizard.c pbxconfig.c testing.c netconfig.c interface.c toolbar.cpp
wxforms_CFLAGS = $(AM_CFLAGS) $(WX_CFLAGS) $(DTSAPPLIB_CFLAGS)
wxforms_CXXFLAGS = $(AM_CXXFLAGS) $(WX_CXXFLAGS) $(DTSAPPLIB_CFLAGS)
wxforms_LDADD = $(WX_LIBS) $(DTSAPPLIB_LIBS) ../src/libdtsapp_gui.la
