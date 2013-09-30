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

int handle_test(dtsgui_pane p, int type, int event, void *data) {
	switch(event) {
		case wx_PANEL_BUTTON_YES:
			printf("Ok\n");
			break;

		case wx_PANEL_BUTTON_NO:
			printf("Cancel\n");
			break;

		default:
			printf("Other %i\n", event);
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

int progress_test(struct dtsgui *dtsgui, void *data) {
	dtsgui_progress pdlg;
	int i;

	pdlg = dtsgui_progress_start(dtsgui, "Testing Progress Bar", 1000);
	for(i=0; i <10;i++) {
		if (!dtsgui_progress_update(pdlg, ((1000/10) * (i+1)), NULL)) {
			break;
		}
#ifdef __WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
	}
	dtsgui_progress_end(pdlg);
	return 1;
}

void test_menu(struct dtsgui *dtsgui) {
	dtsgui_treeview tree;
	dtsgui_menu test;
	dtsgui_pane p;

	test = dtsgui_newmenu(dtsgui, "&Testing");

	p = dtsgui_panel(dtsgui, "Test Panel", wx_PANEL_BUTTON_ALL, 1, NULL);
	testpanel(p);
	dtsgui_setevcallback(p, handle_test, NULL);
	dtsgui_newmenuitem(test, dtsgui, "&Test", p);

	tree = advanced_config(dtsgui, "Tree Window", NULL);
	dtsgui_newmenuitem(test, dtsgui, "T&ree", tree);

	dtsgui_newmenucb(test, dtsgui, "Test &Post", "Send POST request to callshop server (requires callshop user)", post_test, NULL);
	dtsgui_newmenucb(test, dtsgui, "Progress Test", "Progress Test", progress_test, NULL);
}

