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

void handle_test(dtsgui_pane p, int type, int event, void *data) {
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
}

void testpanel(dtsgui_pane p) {
	struct form_item *l;

	dtsgui_textbox(p, "TextBox", "text", "Text", NULL);
	dtsgui_textbox_multi(p, "Multi Line", "multi", "L\nL\n", NULL);
	dtsgui_checkbox(p, "Checkbox", "check", 1, NULL);

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

void test_menu(struct dtsgui *dtsgui) {
	dtsgui_treeview tree;
	dtsgui_menu test;
	dtsgui_pane p;

	test = dtsgui_newmenu(dtsgui, "&Testing");

	p = dtsgui_panel(dtsgui, "Test Panel", wx_PANEL_BUTTON_ALL, 1, NULL);
	testpanel(p);
	dtsgui_setevcallback(p, handle_test, NULL);
	dtsgui_newmenuitem(test, dtsgui, "&Test", p);

	tree = network_tree(dtsgui);
	dtsgui_newmenuitem(test, dtsgui, "T&ree", tree);

	dtsgui_newmenucb(test, dtsgui, "Test &Post", "Send POST request to callshop server (requires callshop user)", post_test, NULL);
}

