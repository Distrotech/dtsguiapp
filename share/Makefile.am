sharedir = $(datarootdir)/@PACKAGE@
share_DATA = netsentry.dtd default.xml default.conf xml2conf.xsl catalog.xml
xmlcatalog = /etc/xml/catalog

EXTRA_DIST = $(share_DATA)

install-data-hook:
	if [ -e $(xmlcatalog) ];then\
		xmlcatalog --noout --del "//Distrotech/Config" $(xmlcatalog); \
		xmlcatalog --noout --add delegatePublic "//Distrotech/Config" "file://$(sharedir)/catalog.xml" $(xmlcatalog); \
	fi;
