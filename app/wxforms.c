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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#ifdef __WIN32
#define UNICODE 1
#include <winsock2.h>
#include <dirent.h>
#include <shlobj.h>
#endif

#include <dtsapp.h>

#include "dtsgui.h"
#include "private.h"

#ifndef DATA_DIR
#define DATA_DIR	"/usr/share/dtsguiapp"
#endif

void xml_config(struct xml_doc *xmldoc) {
	void *xmlbuf;

	xmlbuf = xml_doctobuffer(xmldoc);
	printf("%s\n", xml_getbuffer(xmlbuf));
	objunref(xmlbuf);
}

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

void config_menu(struct dtsgui *dtsgui) {
	struct app_data *appdata;

	if (!(appdata = dtsgui_userdata(dtsgui))) {
		return;
	}

	objlock(appdata);
	appdata->cfg_menu = dtsgui_newmenu(dtsgui, "&Config");

	dtsgui_newmenucb(appdata->cfg_menu, dtsgui, "Reconfigure &Wizard", "Run System Reconfigure Wizard.", 1, reconfig_wizard, NULL);

	dtsgui_newmenucb(appdata->cfg_menu, dtsgui, "P&BX Configuration", "PBX Setup", 0, pbx_settings, NULL);
	dtsgui_newmenucb(appdata->cfg_menu, dtsgui, "&Interface Configuration", "Inteface Configuration", 0, iface_config, NULL);
	dtsgui_newmenucb(appdata->cfg_menu, dtsgui, "&Advanced Configuration", "Advanced Config", 0, advanced_config, NULL);

	dtsgui_menusep(appdata->cfg_menu);
	dtsgui_newmenucb(appdata->cfg_menu, dtsgui, "&Save And Close Config", "Save/Close System Config (File/URL)", 1, save_config, NULL);
	dtsgui_newmenucb(appdata->cfg_menu, dtsgui, "E&xport Config", "Export Configuration as a .conf file", 0, export_config, NULL);

	dtsgui_menusep(appdata->cfg_menu);
	dtsgui_newmenucb(appdata->cfg_menu, dtsgui, "&View Config (XML)", "View Current Config File In XML Format.", 0, view_config_xml, NULL);
	dtsgui_newmenucb(appdata->cfg_menu, dtsgui, "View &Config (CONF)", "View Current Config File Converted to .conf Format.", 0, view_config_conf, NULL);

	/*initially greyed out*/
	dtsgui_menuenable(appdata->cfg_menu, 0);
	objunlock(appdata);
	objunref(appdata);
}

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

void free_appdata(void *data) {
	struct app_data *appdata = data;

	if (appdata->datadir) {
		free((void*)appdata->datadir);
	}

	if (appdata->xmldoc) {
		objunref(appdata->xmldoc);
	}
}

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
