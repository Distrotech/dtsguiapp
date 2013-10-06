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
#include <wx/frame.h>
#include <wx/progdlg.h>
#include <wx/gauge.h>

#include <dtsapp.h>

#include "dtsgui.h"
#include "dtsgui.hpp"

#include "DTSApp.h"
#include "DTSFrame.h"

void dtsgui::Setup(const char *title, const char *stat, struct point w_size, struct point w_pos, dtsgui_configcb confcallback_cb , void *data) {
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

void dtsgui::UnRef(void *data) {
	struct dtsgui *dtsgui = (struct dtsgui *)data;

	if (dtsgui->userdata) {
		objunref(dtsgui->userdata);
		dtsgui->userdata = NULL;
	}
	if (dtsgui->status) {
		free((void*)dtsgui->status);
		dtsgui->status = NULL;
	}
	if (dtsgui->title) {
		free((void*)dtsgui->title);
		dtsgui->title = NULL;
	}
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

DTSApp::~DTSApp() {
	if (curl) {
		curlclose();
		curl = 0;
	}
	if (dtsgui) {
		objunref((void *)dtsgui);
	}
}

void DTSApp::CreateFrame(dtsgui_configcb confcallback_cb,void *data, struct point wsize, struct point wpos, const char *title, const char *status) {
	if (!(dtsgui = (struct dtsgui *)objalloc(sizeof(struct dtsgui), &dtsgui::UnRef))) {
		return;
	}
	dtsgui->Setup(title, status, wsize, wpos, confcallback_cb, data);
}

bool DTSApp::OnInit() {
	if (!dtsgui) {
		return false;
	}

	/*start up curl and add progress bits*/
	curl = curlinit();
	curl_setprogress(curl_progress_function, curl_progress_ctrl, curl_startprogress, dtsgui);
	curl_setauth_cb(dtsgui_pwdialog, dtsgui);

	return dtsgui->SetupAPPFrame();
}
