/*
    Distrotech Solutions wxWidgets Gui Interface
    Copyright (C) 2013 Gregory Hinton Nietsky <gregory@distrotech.co.za>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/** @defgroup DTS-APP Distrotech Server Configuration Application
  * @brief Multi platform GUI server management application.
  * @{
  * @file
  * @brief Main application file.*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#ifdef __WIN32
#include <winsock2.h>
#include <dirent.h>
#include <shlobj.h>
#endif

#include "dtsgui.h"

#include "private.h"

/** @brief Display formated XML Document on the console.
  * @param xmldoc XML Document to display*/
void xml_config(struct xml_doc *xmldoc) {
	void *xmlbuf;

	xmlbuf = xml_doctobuffer(xmldoc);
	printf("%s\n", xml_getbuffer(xmlbuf));
	objunref(xmlbuf);
}

/** @brief return reference to xmldoc stored in userdata.
  *
  * The user data is available from the application data
  * handle the correct referencing und un referencing of the data.
  * @param dtsgui Application data ptr.
  * @return XML Document.*/
struct xml_doc *app_getxmldoc(struct dtsgui *dtsgui) {
	struct app_data *appdata;
	struct xml_doc *xmldoc;

	if (!(appdata = dtsgui_userdata(dtsgui))) {
		return NULL;
	}

	objlock(appdata);
	if (appdata->xmldoc && objref(appdata->xmldoc)) {
		xmldoc = appdata->xmldoc;
	} else {
		objunlock(appdata);
		objunref(appdata);
		return NULL;
	}
	objunlock(appdata);
	objunref(appdata);
	return xmldoc;
}

/** @brief Create the File menu in the menu bar.
  * @param dtsgui Application data ptr.*/
void file_menu(struct dtsgui *dtsgui) {
	dtsgui_menu file;
	struct app_data *appdata;

	if (!(appdata = dtsgui_userdata(dtsgui))) {
		return;
	}

	file = dtsgui_newmenu(dtsgui, "&File");

	objlock(appdata);
	appdata->n_wiz = dtsgui_newmenucb(file, dtsgui, "&New System (Wizard)", "New System Configuration Wizard", 1, newsys_wizard, NULL);
	appdata->e_wiz = dtsgui_newmenucb(file, dtsgui, "&Edit Saved System (Wizard)", "Reconfigure Saved System File With Wizard ", 1, editsys_wizard, NULL);

	dtsgui_menusep(file);
	appdata->c_open = dtsgui_newmenucb(file, dtsgui, "&Open Config File", "Open System Config From A File", 1, open_config, NULL);
	objunlock(appdata);

	dtsgui_menusep(file);
	dtsgui_close(file, dtsgui);
	dtsgui_exit(file, dtsgui);

	objunref(appdata);
}

/** @brief Create the Help menu in the menu bar.
  * @param dtsgui Application data ptr.*/
void help_menu(struct dtsgui *dtsgui) {
	dtsgui_menu help;
	dtsgui_pane p;

	help = dtsgui_newmenu(dtsgui, "&Help");

	p = dtsgui_textpane(dtsgui, "TEST", "Hello This is a text box");
	dtsgui_newmenuitem(help, dtsgui, "&Hello...\tCtrl-H", p);

	dtsgui_menusep(help);
	test_menu(dtsgui, help, "https://sip1.speakezi.co.za:666/auth/test.php");

	dtsgui_menusep(help);
	dtsgui_about(help, dtsgui, "This is a test application!!!!");
}

/** @brief GUI application callback.
  *
  * This is called on GUI application creation returning 0 will end
  * the GUI returning non zero will pass control to the GUI.
  * @see dtsgui_configcb
  * @param dtsgui Application data ptr.
  * @param data Application userdata reference.
  * @return Non zero value to pass control to the GUI.*/
int guiconfig_cb(struct dtsgui *dtsgui, void *data) {
	if (!data) {
		return 0;
	}

	dtsgui_setuptoolbar(dtsgui, app_toolbar, NULL);

	/* menus*/
	file_menu(dtsgui);
	config_menu(dtsgui);
	help_menu(dtsgui);

	return 1;
}

/** @brief Callback called when reference count becomes 0
  *
  * Free datadir and un reference xmldoc as we going away.
  * @param data Application user data reference.*/
void free_appdata(void *data) {
	struct app_data *appdata = data;

	if (appdata->datadir) {
		free((void*)appdata->datadir);
	}

	if (appdata->xmldoc) {
		objunref(appdata->xmldoc);
	}
}

/** @brief Application entry point.
  *
  * The first argument (argv[0]) is the application name.
  * @param argc Number of arguments in array argv.
  * @param argv Array of argumements passed on the commandline.
  * @return Exit code.*/
int main(int argc, char **argv) {
	struct point wsize = {800, 600};
	struct point wpos = {50, 50};
	struct app_data *appdata;
	int res;
	char apppath[PATH_MAX];

	if (!(appdata = objalloc(sizeof(*appdata), free_appdata))) {
		return -1;
	}

#ifdef __WIN32
	getwin32folder(CSIDL_COMMON_APPDATA, apppath);
	appdata->datadir = malloc(strlen(apppath)+12);
	snprintf((char*)appdata->datadir, strlen(apppath)+12, "%s\\Distrotech", apppath);
	chdir(appdata->datadir);
	_putenv("XML_DEBUG_CATALOG=\"\"");
	_putenv("XML_CATALOG_FILES=catalog.xml");
#else
	appdata->datadir = strdup(DATA_DIR);
	snprintf(apppath, strlen(appdata->datadir)+13, "%s/catalog.xml", appdata->datadir);
	if (is_file(apppath)) {
		setenv("XML_CATALOG_FILES", apppath, 1);
	}
	setenv("XML_DEBUG_CATALOG", "", 0);
#endif

	startthreads();
	xml_init();
	xslt_init();

	dtsgui_config(guiconfig_cb, appdata, wsize, wpos, "Distrotech System App", "Welcome to Distrotech App!");
	res = dtsgui_run(argc, argv);
	objunref(appdata);

	xslt_close();
	xml_close();
	stopthreads();

	return res;
}

/**@}*/
