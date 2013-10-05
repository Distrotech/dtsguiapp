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

#ifdef __WIN32__
#define UNICODE 1
#include <winsock2.h>
#include <windows.h>
#endif

#include <wx/app.h>
#include <wx/wizard.h>

#include <dtsapp.h>

#include "dtsgui.hpp"
#include "DTSApp.h"

static void free_dtsgui(void *data) {
	struct dtsgui *dtsgui = (struct dtsgui *)data;

	if (dtsgui->userdata) {
		objunref(dtsgui->userdata);
	}
}

DTSApp::~DTSApp() {
	if (dtsgui && dtsgui->userdata && !objlock(dtsgui)) {
		objunref(dtsgui->userdata);
		dtsgui->userdata = NULL;
		objunlock(dtsgui);
	}
	if (dtsgui) {
		objunref((void *)dtsgui);
	}

	if (curl) {
		curlclose();
	}
}

void DTSApp::CreateFrame(dtsgui_configcb confcallback_cb,void *data, struct point wsize, struct point wpos, const char *title, const char *status) {
	if (!(dtsgui = (struct dtsgui *)objalloc(sizeof(struct dtsgui), free_dtsgui))) {
		return;
	}
	dtsgui->wsize = wsize;
	dtsgui->wpos = wpos;
	dtsgui->title = title;
	dtsgui->status = status;
	dtsgui->cb = confcallback_cb;
	if (data && objref(data)) {
		dtsgui->userdata = data;
	} else {
		dtsgui->userdata = NULL;
	}
}

bool DTSApp::OnInit() {
	void *data = NULL;
	bool res;

	/*start up curl and add progress bits*/
	curl = curlinit();
	curl_setprogress(curl_progress_function, curl_progress_ctrl, curl_startprogress, dtsgui);
	curl_setauth_cb(dtsgui_pwdialog, dtsgui);

	/*deleted on close*/
	newappframe(dtsgui);

	if (dtsgui->userdata && objref(dtsgui->userdata)) {
		data = dtsgui->userdata;
	}

	res = dtsgui->cb(dtsgui, data);
	if (data) {
		objunref(data);
	}
	return res;
}
