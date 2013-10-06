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

void post_items(struct dtsgui *dtsgui, dtsgui_pane p) {
	struct curl_post *post;
	struct curlbuf *cbuf;
	const char *url;

	url = dtsgui_paneldata(p);
	post = dtsgui_pane2post(p);
	cbuf = dtsgui_posturl(url, post);

	if (cbuf && cbuf->body) {
		dtsgui_alert(dtsgui, (char*)cbuf->body);
	}

	if (cbuf) {
		objunref(cbuf);
	}
	objunref((void*)url);
}

int handle_test(struct dtsgui *dtsgui, dtsgui_pane p, int type, int event, void *data) {
	if (type != wx_PANEL_EVENT_BUTTON) {
		return 1;
	}

	switch(event) {
		case wx_PANEL_EVENT_BUTTON_YES:
			post_items(dtsgui, p);
			break;
		case wx_PANEL_EVENT_BUTTON_NO:
			dtsgui_alert(dtsgui, "Undo");
			break;
		case wx_PANEL_EVENT_BUTTON_FIRST:
			dtsgui_alert(dtsgui, "First");
			break;
		case wx_PANEL_EVENT_BUTTON_LAST:
			dtsgui_alert(dtsgui, "Last");
			break;
		case wx_PANEL_EVENT_BUTTON_BACK:
			dtsgui_alert(dtsgui, "Back");
			break;
		case wx_PANEL_EVENT_BUTTON_FWD:
			dtsgui_alert(dtsgui, "Forward");
			break;
	}
	return 1;
}

void testpanel(dtsgui_pane p) {
	struct form_item *l;

	dtsgui_textbox(p, "TextBox", "text", "Text", NULL);
	dtsgui_textbox_multi(p, "Multi Line", "multi", "L\nL\n", NULL);
	dtsgui_checkbox(p, "Checkbox", "check", "1", "0", 1, NULL);

	l = dtsgui_combobox(p, "ComboBox", "combo", NULL);
	dtsgui_listbox_add(l, "Test1", NULL);
	dtsgui_listbox_add(l, "Test2", NULL);
	dtsgui_listbox_add(l, "Test3", NULL);
	dtsgui_listbox_add(l, "Alt Test3", NULL);
	objunref(l);

	l = dtsgui_listbox(p, "ListBox", "list", NULL);
	dtsgui_listbox_add(l, "Test1", NULL);
	dtsgui_listbox_add(l, "Test2", NULL);
	dtsgui_listbox_add(l, "Test3", NULL);
	dtsgui_listbox_add(l, "Alt Test3", NULL);
	objunref(l);
}

void test_menu(struct dtsgui *dtsgui, dtsgui_menu menu, const char *url) {
	void *purl;
	dtsgui_pane p;

	purl = dtsgui_char2obj(url);
	p = dtsgui_panel(dtsgui, "Test Panel", "Test Panel", wx_PANEL_BUTTON_ALL, 1, purl);
	objunref(purl);
	testpanel(p);

	dtsgui_setevcallback(p, handle_test, NULL);
	dtsgui_newmenuitem(menu, dtsgui, "&Test", p);
}

