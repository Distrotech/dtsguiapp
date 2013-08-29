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

#ifndef DTSFRAME_H
#define DTSFRAME_H

class DTSFrame: public wxFrame {
	public:
		DTSFrame(const wxString &title, const wxPoint &pos, const wxSize &size, struct dtsgui *dtsgui);
		wxMenuBar *GetMenuBar(void);
		~DTSFrame();
		void SetAbout(const char *a_text);
		void SetWindow(wxWindow *window);
		void SwitchWindow(wxCommandEvent &event);
		bool Confirm(wxString text);
		void OnClose(wxCommandEvent &event);
		void OnAbort(wxCloseEvent &event);
		void OnAbout(wxCommandEvent &event);
	private:
		wxBoxSizer *sizer;
		wxWindow *blank;
		wxWindow *a_window;
		wxString status;
		wxString about;
		wxMenuBar *menubar;
		bool abortconfirm;
};

#endif // DTSFRAME_H