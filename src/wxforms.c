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
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

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

void test_posturl(struct dtsgui *dtsgui, const char *user, const char *passwd, const char *url) {
	struct curlbuf *cbuf;
	struct basic_auth *auth;
	struct curl_post *post;

	if (user && passwd) {
		auth = dtsgui_pwdialog(user, passwd, dtsgui);
	} else {
		auth = NULL;
	}

	post = curl_newpost();
	curl_postitem(post, "firstname", "gregory hinton");
	curl_postitem(post, "lastname", "nietsky");
	curl_postitem(post, "test", "&<>@%");

	if (!(cbuf = curl_posturl(url, auth, post, dtsgui_pwdialog, dtsgui))) {
		objunref(auth);
		return;
	}
/*
	curl_ungzip(cbuf);

	if (cbuf && cbuf->c_type && !strcmp("application/xml", cbuf->c_type)) {
		xmldoc = xml_loadbuf(cbuf->body, cbuf->bsize, 1);
	}*/

	if (cbuf && cbuf->body) {
		dtsgui_alert(dtsgui, (char*)cbuf->body);
	}

	objunref(cbuf);
	objunref(auth);
}

void xml_config(struct xml_doc *xmldoc) {
	void *xmlbuf;

	xmlbuf = xml_doctobuffer(xmldoc);
	printf("%s\n", xml_getbuffer(xmlbuf));
	objunref(xmlbuf);
}

int post_test(struct dtsgui *dtsgui, void *data) {
	test_posturl(dtsgui, NULL, NULL, "https://sip1.speakezi.co.za:666/auth/test.php");

	return 1;
}

int open_config(struct dtsgui *dtsgui, void *data) {
	struct app_data *appdata;
	const char *filename;
	int i;

	appdata = dtsgui_userdata(dtsgui);

	if (!(filename = dtsgui_fileopen(dtsgui, "Select Customer Configuration To Open", NULL, "", "XML Configuration|*.xml"))) {
		return 0;
	}

	if (!(appdata->xmldoc = xml_loaddoc(filename, 1	))) {
		dtsgui_alert(dtsgui, "Configuration failed to load.\n");
		return 0;
	}

	dtsgui_reconfig(dtsgui);

	dtsgui_createdyn(dtsgui, appdata->dyn_cfg);

	for(i=0; i < 30;i++ ){
		if (appdata->dyn_cfg) {
			break;
		}
		usleep(200000);
	}

	dtsgui_menuitemenable(appdata->e_wiz, 0);
	dtsgui_menuitemenable(appdata->n_wiz, 0);
	dtsgui_menuitemenable(appdata->c_open, 0);
	dtsgui_menuenable(appdata->cfg_menu, 1);
	dtsgui_titleappend(dtsgui, filename);
	return 1;
}

int save_config(struct dtsgui *dtsgui, void *data) {
	struct app_data *appdata;

	appdata = dtsgui_userdata(dtsgui);
/*	dtsgui_reconfig(dtsgui);*/
	dtsgui_menuenable(appdata->cfg_menu, 0);
	dtsgui_menuitemenable(appdata->e_wiz, 1);
	dtsgui_menuitemenable(appdata->n_wiz, 1);
	dtsgui_menuitemenable(appdata->c_open, 1);
	dtsgui_titleappend(dtsgui, NULL);
	if (appdata->dyn_cfg) {
		dtsgui_closedyn(dtsgui, appdata->dyn_cfg);
	}
	objunref(appdata->xmldoc);
	return 1;
}

void file_menu(struct dtsgui *dtsgui) {
	dtsgui_menu file;
	struct app_data *appdata;

	appdata = dtsgui_userdata(dtsgui);

	file = dtsgui_newmenu(dtsgui, "&File");

	appdata->n_wiz = dtsgui_newmenucb(file, dtsgui, "&New System (Wizard)", "New System Configuration Wizard", newsys_wizard, NULL);
	appdata->e_wiz = dtsgui_newmenucb(file, dtsgui, "&Edit Saved System (Wizard)", "Reconfigure Saved System File With Wizard ", editsys_wizard, NULL);

	dtsgui_menusep(file);
	appdata->c_open = dtsgui_newmenucb(file, dtsgui, "&Open Config File", "Open System Config From A File", open_config, NULL);


	dtsgui_menusep(file);
	dtsgui_close(file, dtsgui);
	dtsgui_exit(file, dtsgui);
}

void config_menu(struct dtsgui *dtsgui) {
	struct app_data *appdata;
	struct dynamic_panel *dyn_cfg = NULL;

	appdata = dtsgui_userdata(dtsgui);
	appdata->cfg_menu = dtsgui_newmenu(dtsgui, "&Config");

	dtsgui_newmenucb(appdata->cfg_menu, dtsgui, "Reconfigure &Wizard", "Run System Reconfigure Wizard.", reconfig_wizard, NULL);
	dtsgui_newmenudyn(appdata->cfg_menu, dtsgui, "PBX Setup", "P&BX Configuration", pbx_settings, NULL, &dyn_cfg);
	if (dyn_cfg) {
		appdata->dyn_cfg = dyn_cfg;
	}

	dtsgui_menusep(appdata->cfg_menu);
	dtsgui_newmenucb(appdata->cfg_menu, dtsgui, "&Save Config", "Save/Close System Config (File/URL)", save_config, NULL);

	dtsgui_menuenable(appdata->cfg_menu, 0);
}

void help_menu(struct dtsgui *dtsgui) {
	dtsgui_menu help;
	dtsgui_pane p;

	help = dtsgui_newmenu(dtsgui, "&Help");

	p = dtsgui_textpane(dtsgui, "TEST", "Hello This is a text box");
	dtsgui_newmenuitem(help, dtsgui, "&Hello...\tCtrl-H", p);
	dtsgui_menusep(help);
	dtsgui_about(help, dtsgui, "This is a test application!!!!");
}

void free_treedata(void *data) {
	struct tree_data *td = data;

	if (td->node) {
		objunref(td->node);
	}
	if (td->tattr) {
		free((void*)td->tattr);
	}
}

struct tree_data *gettreedata(dtsgui_treeview tree, struct xml_node *xn, const char *tattr, enum node_id nid) {
	struct tree_data *td;

	if (!(td = objalloc(sizeof(*td), free_treedata))) {
		return NULL;
	}
	td->nodeid = nid;
	if (xn && objref(xn)) {
		td->node = xn;
		if (tattr) {
			ALLOC_CONST(td->tattr, tattr);
		}
	}
	if (tree) {
		td->tree = tree;
	}
	return td;
}

int guiconfig_cb(struct dtsgui *dtsgui, void *data) {
	struct app_data *appdata = data;

	if (!data || !objref(appdata)) {
		return 0;
	}

	/* menus*/
	file_menu(dtsgui);
	config_menu(dtsgui);
	help_menu(dtsgui);
	test_menu(dtsgui);

	objunref(appdata);
	return 1;

	/*load xml config via http*/
	if (!(appdata->xmldoc = dtsgui_loadxmlurl(dtsgui, "admin", "", "https://callshop.distrotech.co.za:666/cshop"))) {
		objunref(appdata);
		dtsgui_confirm(dtsgui, "Config DL Failed (as expected after 3 tries)\nHello Dave\n\nWould You Like To Play .... Thermo Nuclear War ?");
		return 1;
	}

	objunref(appdata);
	return 1;
}

void free_appdata(void *data) {
	struct app_data *appdata = data;

	if (appdata->datadir) {
		free((void*)appdata->datadir);
	}

	if (appdata->dtsgui) {
		objunref(appdata->dtsgui);
	}

	if (appdata->xmldoc) {
		objunref(appdata->xmldoc);
	}

	if (appdata->dyn_cfg) {
		objunref(appdata->dyn_cfg);
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

	appdata->dtsgui = dtsgui_config(guiconfig_cb, appdata, wsize, wpos, "Distrotech System App", "Welcome to Distrotech App!");
	res = dtsgui_run(argc, argv);
	objunref(appdata);

	xslt_close();
	xml_close();
	stopthreads();

	return res;
}
