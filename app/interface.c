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
#include <stdint.h>

#include <dtsapp.h>

#include "dtsgui.h"
#include "private.h"

dtsgui_pane iface_config(struct dtsgui *dtsgui, const char *title, void *data) {
	dtsgui_tabview tabv;
	struct app_data *appdata;
	struct xml_doc *xmldoc;
	struct xml_search *xp;
	struct xml_node *xn;
	const char *name;
	dtsgui_pane *p;
	void *iter = NULL;

	appdata = dtsgui_userdata(dtsgui);
	xmldoc = appdata->xmldoc;
	if (!(xp = xml_xpath(xmldoc, "/config/IP/Interfaces/Interface", "name"))) {
		return NULL;
	}

	tabv = dtsgui_tabwindow(dtsgui, title, NULL);

	for(xn = xml_getfirstnode(xp, &iter); xn; xn = xml_getnextnode(iter)) {
		name = xml_getattr(xn, "name");
		p = dtsgui_newtabpage(tabv, name, wx_PANEL_BUTTON_ACTION, NULL, xmldoc);
		network_iface_pane(p, xn->value);
		dtsgui_addtabpage(tabv, p);
		objunref(xn);
	}

	if (iter) {
		objunref(iter);
	}

	return tabv;
}
