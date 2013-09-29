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

#include <dtsapp.h>

#include "dtsgui.h"
#include "evdata.h"

evdata::evdata(void *userdata, dtsgui_configcb cb, int uref) {
	if (userdata && uref) {
		data = (objref(userdata)) ? userdata : NULL;
	} else {
		data = userdata;
	}
	callback = cb;
	unref = uref;
}

evdata::~evdata() {
	if (unref) {
		objunref(data);
	}
}
