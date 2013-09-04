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
	struct basic_auth *auth;


	if (event == wx_PANEL_BUTTON_YES) {
		if (!(auth = dtsgui_paneldata(p))) {
			return;
		}
		if (auth->user) {
			free((void*)auth->user);
			auth->user = NULL;
		}
		if (auth->passwd) {
			memset((void*)auth->passwd, 0, strlen(auth->passwd));
			free((void*)auth->passwd);
			auth->passwd = NULL;
		}

		auth->user = dtsgui_findvalue(p, "uname");
		auth->passwd = dtsgui_findvalue(p, "pwd");
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
	dtsgui_textbox(pwbox, "Username", "uname", bauth->user, NULL);
	dtsgui_passwdbox(pwbox, "Password", "pwd", bauth->passwd, NULL);
	objref(bauth);
	dtsgui_rundialog(pwbox, pwevent, NULL);

	return bauth;
}
