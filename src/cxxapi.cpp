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

/** @file
  * @brief C++ Functions exported to C API
  * @ingroup C-API
  * @see DTS_C_API
  * @see __DTS_C_API
  * @see @ref C-API
  * 
  * These functions should not be used in C++ applications.*/


/** @ingroup C-API
  * @brief Defining __DTS_C_API allows access to C API from inside a C++ file
  * 
  * dtsgui.h will only include the definitions for the C API using C++ if this is defined.*/
#define __DTS_C_API

#ifdef __WIN32__
#define UNICODE 1
#include <winsock2.h>
#include <shlobj.h>
#endif

#include <wx/menu.h>

#include "dtsgui.h"

namespace DTS_C_API {

/** @ingroup C-API-Menus
  * @brief Enable or disable all items in menu.
  * @param dm Menu to enable/disable.
  * @param enable Enable all items if set to non zero.*/
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

/** @ingroup C-API
  * @fn void getwin32folder(int csidl, char *path)
  * @brief Wrapper to obtain "special" path for WIN32
  * @param csidl Special folder to obtain.
  * @param path Buffer that the path is placed in.*/
#ifdef __WIN32
void getwin32folder(int csidl, char *path) {
	SHGetFolderPathA(NULL, csidl, NULL, 0, path);
}
#endif
} /* END namespace*/
