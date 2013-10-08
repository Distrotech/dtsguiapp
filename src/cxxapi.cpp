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

#define __DTS_C_API

#ifdef __WIN32__
#define UNICODE 1
#include <winsock2.h>
#include <shlobj.h>
#endif

#include <wx/menu.h>

#include "dtsgui.h"

namespace DTS_C_API {

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

#ifdef __WIN32
void getwin32folder(int csidl, char *path) {
	SHGetFolderPathA(NULL, csidl, NULL, 0, path);
}
#endif
} /* END namespace*/
