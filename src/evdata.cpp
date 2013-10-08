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
#include <wx/object.h>
#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <wx/wizard.h>
#include <dtsapp.h>

#include "dtsgui.h"
#include "dtsgui.hpp"
#include "evdata.h"
#include "pitems.h"
#include "DTSPanel.h"


dynamic_panel::dynamic_panel(const char *title, int blank, dtsgui_dynpanel cb, void *udata) {
	if (udata && objref(udata)) {
		data = udata;
	} else {
		data = NULL;
	}
	ALLOC_CONST(this->title, title);
	this->cb = cb;
	this->blank = blank;
}

dynamic_panel::~dynamic_panel() {
	if (title) {
		free((void*)title);
	}
	if (data) {
		objunref(data);
	}
}

bool dynamic_panel::HasCallback() {
	return (cb);
}

bool dynamic_panel::IsBlank() {
	return (blank);
}

bool dynamic_panel::operator==(wxWindow &rhs) {
	return (this->w == &rhs);
}

wxWindow *dynamic_panel::RunCallback(class dtsgui *dtsgui) {
	DTSObject *p;

	if (w) {
		delete w;
		w = NULL;
	}

	if (dtsgui && objref(dtsgui)) {
		if ((p = (DTSObject*)cb(dtsgui, title, data))) {
			w = p->GetPanel();
		}
		objunref(dtsgui);
	}
	return w;
}

evdata::evdata(void *userdata, int uref) {
	if (userdata && uref) {
		this->data = (objref(userdata)) ? userdata : NULL;
	} else {
		this->data = userdata;
	}
	this->unref = uref;
}

evdata::~evdata() {
	if (unref) {
		objunref(data);
	}
}
