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

/** @defgroup DTS-APP-Test Application test panel
  * @ingroup DTS-APP
  * @brief Static window with standard elements that will post to a URL and pop up the result.
  *
  * This is for testing the basic features and curl integration.
  * @addtogroup DTS-APP-Test
  * @{
  * @file
  * @brief Test panel.*/

#include <unistd.h>

#include "dtsgui.h"

#include "private.h"

/** @brief Create a curl post object and submit to url.
  *
  * The panel data contains the URL
  * @param dtsgui Application data ptr.
  * @param p panel to be posted.
  * @return Nothing is returned a alert box is popped up on success.*/
void post_items(struct dtsgui *dtsgui, dtsgui_pane p) {
	struct curl_post *post;
	struct curlbuf *cbuf;
	const char *url;

	url = dtsgui_paneldata(p);
	post = dtsgui_pane2post(p);
	cbuf = curl_posturl(url, NULL, post, NULL, NULL);

	if (cbuf && cbuf->body) {
		dtsgui_alert(dtsgui, (char*)cbuf->body);
	}

	if (cbuf) {
		objunref(cbuf);
	}
	objunref((void*)url);
}

/** @brief panel event handler.
  *
  * Filter all but button events and on Apply/Yes post the panel to URL.
  * Other butons pressed will pop up a alert box with the button pressed.
  * @param dtsgui Application data ptr.
  * @param p The panel generating the events.
  * @param type Type of event generated.
  * @param event The event generated.
  * @param data Reference to userdata set with eventhandler.
  * @return 0 to mark the event as handled or non zero to pass handling on.*/
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

/** @brief Configure a panel with test elements.
    @param p Panel to configure*/
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


/** @brief Add test option to menu.
  *
  * @param dtsgui Application data ptr.
  * @param menu Menu to append new item too.
  * @param url URL to post the panel too.*/
void test_menu(struct dtsgui *dtsgui, dtsgui_menu menu, const char *url) {
	void *purl;
	dtsgui_pane p;

	purl = objchar(url);
	p = dtsgui_panel(dtsgui, "Test Panel", "Test Panel", wx_PANEL_BUTTON_ALL, 1, purl);
	objunref(purl);
	testpanel(p);

	dtsgui_setevcallback(p, handle_test, NULL);
	dtsgui_newmenuitem(menu, dtsgui, "&Test", p);
}

/** @}*/
