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

#include <wx/frame.h>

typedef class wxBoxSizer wxBoxSizer;
typedef class wxGauge wxGauge;
typedef class wxProgressDialog wxProgressDialog;
typedef class DTSPanel DTSPanel;
typedef class DTSObject DTSObject;
typedef class DTSWindow DTSWindow;

class DTSFrame: public wxFrame {
	public:
		DTSFrame(const wxString &title, const wxPoint &pos, const wxSize &size, class dtsgui *dtsgui);
		~DTSFrame();
		wxMenuBar *GetMenuBar(void);
		void CloseMenu(wxMenu *m, int type);
		wxMenu *NewMenu(const wxString &name);
		void SetAbout(wxMenu *m, const char *a_text);
		bool Confirm(wxString text);
		void Alert(wxString text);
		class basic_auth *Passwd(const char *user, const char *passwd);
		const char *FileDialog(const char *title, const char *path, const char *name, const char *filter, long style);
		/*progress*/
		int StartProgress(const wxString &text, int maxval, int quit);
		void EndProgress(void);
		int UpdateProgress(int cval, const wxString &msg);
		int IncProgress(int ival, const wxString &msg);

		void SetWindow(wxWindow *window);
		class dtsgui *GetDTSData(void);
		virtual wxToolBar *OnCreateToolBar(long style, wxWindowID id, const wxString& name);
		void SetupToolbar(dtsgui_toolbar_create cb, void *data);
		DTSPanel *CreatePane(const wxString &name, const wxString &title, int butmask, enum panel_type type, void *udata);
		DTSWindow *TextPanel(const wxString &title, const char *buf);
		wxMenuItem *NewMenuItem(wxMenu *m, DTSObject *panel, int menuid, const wxString &hint);
		wxMenuItem *NewMenuItem(wxMenu *m, int menuid, const wxString &name, const wxString &hint, int blank, dtsgui_dynpanel cb, void *data);
		/*events*/
		void SendDTSEvent(int eid, wxObject *evobj);
		void SwitchWindow(wxCommandEvent &event);
		void DynamicPanelEvent(wxCommandEvent &event);
		void OnClose(wxCommandEvent &event);
		void OnAbort(wxCloseEvent &event);
		void OnAbout(wxCommandEvent &event);
	private:
		void OnDTSEvent(wxCommandEvent &event);
		static int pwevent(struct dtsgui *dtsgui, dtsgui_pane p, int type, int event, void *data);
		wxBoxSizer *sizer;
		wxWindow *blank;
		wxWindow *a_window;
		wxToolBar *toolbar;
		wxString about;
		wxMenuBar *menubar;
		class dtsgui *dtsgui;
		bool abortconfirm;
		wxGauge *pbar;
		wxProgressDialog *pdia;
		dtsgui_toolbar_create tbcb;
		void *tb_data;
};

#endif // DTSFRAME_H
