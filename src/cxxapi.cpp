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
#include <vector>

#ifdef __WIN32__
#define UNICODE 1
#include <winsock2.h>
#include <windows.h>
#include <shlobj.h>
#endif

#include <wx/dataview.h>
#include <wx/menu.h>

#include <dtsapp.h>
#include "dtsgui.h"
#include "DTSListView.h"

void dtsgui_menuenable(dtsgui_menu dm, int enable) {
	bool state =  (enable) ? true : false;
	wxMenuItemList items;
	wxMenuItem *mi;
	int cnt,i;

	wxMenu *m = (wxMenu*)dm;

	items = m->GetMenuItems();
	cnt = m->GetMenuItemCount();

	for(i=0; i < cnt;i++) {
		mi = items[i];
		mi->Enable(state);
	}
}

const char *dtsgui_treenodeparent(dtsgui_treenode tn) {
	DTSDVMListStore *entry, *parent;
	const char *val;

	entry = (DTSDVMListStore*)tn;
	if (!entry || !(parent = entry->GetParent())) {
		return NULL;
	}
	val = strdup(parent->GetTitle().ToUTF8());
	return val;
}

void dtsgui_nodesetxml(dtsgui_treeview tree, dtsgui_treenode node, const char *title) {
	DTSDVMListStore *ls = (DTSDVMListStore*)node;
	struct xml_doc *xmldoc;
	struct xml_node *xn;
	char *buff = NULL;

	if (!(xmldoc = dtsgui_panelxml(tree))) {
		return;
	}

	if (!(xn = ls->GetXMLData(&buff))) {
		objunref(xmldoc);
		return;
	}

	if (buff) {
		xml_setattr(xmldoc, xn, buff, title);
		objunref(buff);
	} else {
		xml_modify(xmldoc, xn, title);
	}
	objunref(xn);
	objunref(xmldoc);
}

extern struct xml_doc *dtsgui_buf2xml(struct curlbuf *cbuf) {
	struct xml_doc *xmldoc = NULL;

	if (cbuf && cbuf->c_type && !strcmp("application/xml", cbuf->c_type)) {
		curl_ungzip(cbuf);
		xmldoc = xml_loadbuf(cbuf->body, cbuf->bsize, 1);
	}
	return xmldoc;
}

void *dtsgui_char2obj(const char *orig) {
	int len = strlen(orig) + 1;
	void *nobj;

	if ((nobj = objalloc(len, NULL))) {
		memcpy(nobj, orig, len);
	}
	return nobj;
}

#ifdef __WIN32
void getwin32folder(int csidl, char *path) {
	SHGetFolderPathA(NULL, csidl, NULL, 0, path);
}
#endif
