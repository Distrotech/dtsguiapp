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

#include <dtsapp.h>

#include "dtsgui.h"


void pwevent(dtsgui_pane p, int type, int event, void *data) {
	struct bucket_loop *bloop;
	struct bucket_list *il;
	struct form_item *fi;
	struct basic_auth *auth;
	const char *name;


	if (event == wx_PANEL_BUTTON_YES) {
		if (!(auth = dtsgui_paneldata(p))) {
			return;
		}

		il = dtsgui_panel_items(p);
		bloop = init_bucket_loop(il);

		while (bloop && (fi = next_bucket_loop(bloop))) {
			name = dtsgui_item_name(fi);

			if (!strcmp("Username", name)) {
				if (auth->user) {
					free((void*)auth->user);
				}
				auth->user = dtsgui_item_value(fi);
			} else if (!strcmp("Password", name)) {
				if (auth->passwd) {
					free((void*)auth->passwd);
				}
				auth->passwd = dtsgui_item_value(fi);
			}

			objunref(fi);
		}

		stop_bucket_loop(bloop);
		objunref(il);
	}
}

struct basic_auth *dtsgui_pwdialog(const char *user, const char *passwd, void *data) {
	struct dtsgui *dtsgui = (struct dtsgui *)data;
	struct basic_auth *bauth;
	dtsgui_pane pwbox;

	if (!(bauth = curl_newauth(user, passwd))) {
		return NULL;
	}

	pwbox = dtsgui_panel(dtsgui, "Athentification", wx_PANEL_BUTTON_ACTION, wx_DTSPANEL_DIALOG, bauth);
	dtsgui_textbox(pwbox, "Username", bauth->user, NULL);
	dtsgui_passwdbox(pwbox, "Password", bauth->passwd, NULL);
	objref(bauth);
	dtsgui_rundialog(pwbox, pwevent, NULL);

	return bauth;
}
