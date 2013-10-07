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

#ifndef EVDATA_H
#define EVDATA_H

typedef class wxWindow wxWindow;

class dynamic_panel {
	public:
		DTS_OJBREF_CLASS(dynamic_panel);
		dynamic_panel(const char *title, int blank, dtsgui_dynpanel cb, void *udata);
		bool HasCallback();
		bool IsBlank();
		bool operator==(wxWindow &rhs);
		wxWindow *RunCallback(class dtsgui *dtsgui);
	private:
		wxWindow *w;
		dtsgui_dynpanel cb;
		const char *title;
		void *data;
		int blank;
};

class evdata :public wxObject {
	public:
		evdata(void*, int uref = 0);
		~evdata();
		void *data;
	private:
		int unref;
};

#endif // EVDATA_H
