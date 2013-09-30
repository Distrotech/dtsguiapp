ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = -I$(srcdir)/include $(DEVELOPER_CFLAGS) -DDATA_DIR=\"$(datarootdir)/@PACKAGE@\"
AM_CXXFLAGS = -I$(srcdir)/include $(DEVELOPER_CXXFLAGS) -DDATA_DIR=\"$(datarootdir)/@PACKAGE@\"
EXTRA_DIST = include

lib_LTLIBRARIES = libdtsapp_gui.la
includeinstdir = $(includedir)/dtsapplib
includeinst_DATA = include/dtsgui.h

libdtsapp_gui_la_SOURCES = DTSApp.cpp DTSFrame.cpp DTSPanel.cpp DTSTreeWindow.cpp DTSListView.cpp evdata.cpp cxxapi.cpp dtsuserpw.c wxcurlprogress.c
libdtsapp_gui_la_LIBADD = $(WX_LIBS) $(DTSAPPLIB_LIBS)
libdtsapp_gui_la_CFLAGS = $(AM_CFLAGS) $(WX_CFLAGS) $(DTSAPPLIB_CFLAGS)
libdtsapp_gui_la_CXXFLAGS = $(AM_CXXFLAGS) $(WX_CXXFLAGS) $(DTSAPPLIB_CFLAGS)
libdtsapp_gui_la_LDFLAGS = -version-info $(LT_LIBVER) -release $(VERSION) -no-undefined

# Here are a set of rules to help you update your library version information:
#
#    Start with version information of `0:0:0' for each libtool library.
#    Update the version information only immediately before a public release of your software.
#    More frequent updates are unnecessary, and only guarantee that the current interface number gets larger faster.
#    If the library source code has changed at all since the last update, then increment revision (`c:r:a' becomes `c:r+1:a').
#    If any interfaces have been added, removed, or changed since the last update, increment current, and set revision to 0.
#    If any interfaces have been added since the last public release, then increment age.
#    If any interfaces have been removed since the last public release, then set age to 0.

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libdtsapp_gui.pc

