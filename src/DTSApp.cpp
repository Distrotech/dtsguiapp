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

#include <wx/app.h>

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
	objunref((void *)dtsgui);
	if (curl) {
		curlclose();
	}
}

struct dtsgui *DTSApp::CreateFrame(dtsgui_configcb confcallback_cb,void *data, struct point wsize, struct point wpos, const char *title, const char *status) {
	dtsgui = (struct dtsgui *)objalloc(sizeof(struct dtsgui), free_dtsgui);
	dtsgui->wsize = wsize;
	dtsgui->wpos = wpos;
	dtsgui->title = title;
	dtsgui->status = status;
	dtsgui->cb = confcallback_cb;
	dtsgui->userdata = data;
	objref((void *)dtsgui);
	objref(data);
	return dtsgui;
}

bool DTSApp::OnInit() {
	curl = curlinit();
	/*deleted on close*/
	newappframe(dtsgui);
	return (dtsgui->cb(dtsgui, dtsgui->userdata));
}
