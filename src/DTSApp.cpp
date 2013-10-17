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
#include <wx/toolbar.h>

#include "dtsgui.h"

#include "DTSApp.h"
#include "DTSFrame.h"

/**@file
  *@brief Core Application Class implementation.
  *@ingroup CPP*/

/** @brief Define event type used for application events.
  *@ingroup CPP*/
wxDEFINE_EVENT(DTS_APP_EVENT, wxCommandEvent);

dtsgui::dtsgui(const char *title, const char *stat, struct point *w_size, struct point *w_pos, dtsgui_configcb confcallback_cb , void *data) {
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

	appframe = new DTSFrame(title, wxPoint(wpos->x, wpos->y), wxSize(wsize->x, wsize->y), this);
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

void dtsgui::ShowToolbar(bool show) {
	wxToolBar *tb;

	tb = appframe->GetToolBar();
	tb->Show(show);
	appframe->Layout();
}

class DTSFrame *dtsgui::GetFrame(void) {
	return appframe;
}

dtsgui_curl::dtsgui_curl(class dtsgui *dtsgui) {
	frame = dtsgui->GetFrame();

	curl = curlinit();
	curl_setprogress(curl_progress_function, curl_progress_ctrl, curl_startprogress, dtsgui);
	curl_setauth_cb(CurlPasswd, this);
}


dtsgui_curl::~dtsgui_curl() {
	Close();
}

void dtsgui_curl::Close() {
	if (curl) {
		curlclose();
		curl = 0;
	}
}

/*start and reset progress*/
void *dtsgui_curl::curl_startprogress(void *data) {
	class dtsgui_curl *dc = (class dtsgui_curl*)data;

	return new class curl_progress(dc);
}

int dtsgui_curl::curl_progress_function(void *data, double dltotal, double dlnow, double ultotal, double ulnow) {
	class curl_progress *cp = (class curl_progress*)data;
	DTSFrame *f;
	int val, tot, cur;

	if (!cp || cp->pause || !(f = cp->GetFrame())) {
		return 0;
	}

	tot = dltotal+ultotal;
	cur = ulnow+dlnow;

	val = (tot && cur) ? (1000/tot) * cur : 0;
	val = ceil(val);

	if (!cp->pd) {
		cp->pd = f->StartProgress("Web Transfer Progress", 1000, 1);
	};

	if (cp->pd && val && !f->UpdateProgress(val, wxEmptyString)) {
		return 1;
	}

	return 0;
}

void dtsgui_curl::curl_progress_ctrl(void *data, int pause) {
	class curl_progress *cp = (class curl_progress*)data;
	DTSFrame *f = cp->GetFrame();

	switch(pause) {
		case 0:
			cp->pause = pause;
			break;
		case 1:
			f->EndProgress();
			cp->pause = pause;
			cp->pd = 0;
			break;
		case -1:
			if (cp->pd) {
				f->EndProgress();
			}
			objunref(cp);
			break;
	}
}

struct basic_auth *dtsgui_curl::CurlPasswd(const char *user, const char *passwd, void *data) {
	class dtsgui_curl *dc = (class dtsgui_curl*)data;
	class DTSFrame *f = dc->frame;

	return f->Passwd(user, passwd);
}

DTSFrame *dtsgui_curl::GetFrame() {
	return frame;
}

curl_progress::curl_progress(class dtsgui_curl *dc) {
	if (dc && objref(dc)) {
		owner = dc;
	} else {
		owner = NULL;
	}
	pd = 0;
	pause = 0;
}

curl_progress::~curl_progress() {
	if (owner) {
		objunref(owner);
	}
}

DTSFrame *curl_progress::GetFrame() {
	return owner->GetFrame();
}

DTSApp::DTSApp(dtsgui_configcb confcallback_cb,void *data, struct point *wsize, struct point *wpos, const char *title, const char *status) {
	dtsgui = new class dtsgui(title, status, wsize, wpos, confcallback_cb, data);
}

DTSApp::~DTSApp() {
	if (dtsgui) {
		objunref((void *)dtsgui);
	}
	if (curl) {
		curl->Close();
		objunref(curl);
	}

}

bool DTSApp::OnInit() {
	if (dtsgui && dtsgui->SetupAPPFrame()) {
		curl = new class dtsgui_curl(dtsgui);
		return true;
	}
	return false;
}
