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

#include <stdlib.h>

#ifdef __WIN32__
#define UNICODE 1
#include <winsock2.h>
#include <windows.h>
#endif

#include "dtsgui.h"

#include "DTSApp.h"
#include "DTSFrame.h"

dtsgui::dtsgui(const char *title, const char *stat, struct point w_size, struct point w_pos, dtsgui_configcb confcallback_cb , void *data) {
	wsize = w_size;
	wpos = w_pos;

	cb = confcallback_cb;
	if (data && objref(data)) {
		userdata = data;
	} else {
		userdata = NULL;
	}
	ALLOC_CONST(this->status, stat);
	ALLOC_CONST(this->title, title);
}

dtsgui::~dtsgui() {
	if (userdata) {
		objunref(userdata);
		userdata = NULL;
	}
	if (status) {
		free((void*)status);
		status = NULL;
	}
	if (title) {
		free((void*)title);
		title = NULL;
	}
}

int dtsgui::SetupAPPFrame() {
	void *data = NULL;
	int res;

	appframe = new DTSFrame(title, wxPoint(wpos.x, wpos.y), wxSize(wsize.x, wsize.y), this);
	SetStatusText();

	objlock(this);
	if (userdata && objref(userdata)) {
		data = userdata;
	}
	objunlock(this);

	res = cb(this, data);

	if (data) {
		objunref(data);
	}
	return res;
}

void *dtsgui::GetUserData() {
	void *ud = NULL;

	objlock(this);
	if (userdata && objref(userdata)) {
		ud = userdata;
	}
	objunlock(this);

	return ud;
}

void dtsgui::SetStatusText(void) {
	objlock(this);
	if (appframe && status) {
		appframe->SetStatusText(status);
	}
	objunlock(this);
}

void dtsgui::AppendTitle(const char *text) {
	char *newtitle;
	int len;

	objlock(this);
	if (text) {
		len = strlen(title)+strlen(text)+4;
		if ((newtitle=(char*)malloc(len))) {
			snprintf(newtitle, len, "%s [%s]", title, text);
		} else {
			newtitle = (char*)title;
		}
	} else {
		newtitle = (char*)title;
	}
	objunlock(this);

	appframe->SetTitle(newtitle);

	if (text && newtitle) {
		free(newtitle);
	}
}

class DTSFrame *dtsgui::GetFrame(void) {
	return appframe;
}

DTSApp::DTSApp(dtsgui_configcb confcallback_cb,void *data, struct point wsize, struct point wpos, const char *title, const char *status) {
	dtsgui = new class dtsgui(title, status, wsize, wpos, confcallback_cb, data);

	/*start up curl and add progress bits*/
	curl = curlinit();
	curl_setprogress(DTS_C_API::curl_progress_function, DTS_C_API::curl_progress_ctrl, DTS_C_API::curl_startprogress, dtsgui);
	curl_setauth_cb(DTS_C_API::dtsgui_pwdialog, dtsgui);
}

DTSApp::~DTSApp() {
	if (curl) {
		curlclose();
		curl = 0;
	}
	if (dtsgui) {
		objunref((void *)dtsgui);
	}
}

bool DTSApp::OnInit() {
	if (!dtsgui) {
		return false;
	}

	return dtsgui->SetupAPPFrame();
}
