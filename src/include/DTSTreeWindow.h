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

#ifndef DTSTREEWINDOW_H
#define DTSTREEWINDOW_H

typedef class DTSTreeWindow DTSTreeWindow;

struct treemenu {
	wxMenu *menu;
	wxMenuItem *msort;
	wxMenuItem *mup;
	wxMenuItem *mdown;
	wxMenuItem *mdelete;
};

class DTSTreeWindowEvent: public wxEvtHandler {
	public:
		DTSTreeWindowEvent(void *userdata = NULL, dtsgui_tree_cb tree_cb = NULL, struct dtsgui *dtsgui = NULL, DTSTreeWindow *win = NULL);
		~DTSTreeWindowEvent();
		void TreeEvent(wxDataViewEvent &event);
		void MenuEvent(wxCommandEvent &event);
		void SplitterEvent(wxSplitterEvent& event);
		void TreeCallback(const wxDataViewItem item, enum tree_cbtype cbtype);
	private:
		void SortParent(const wxDataViewItem parent, int action);
		void *data;
		dtsgui_tree_cb treecb;
		DTSTreeWindow *parent;
		DTSDVMListView *vm;
		DTSDVMCtrl *tree;
		wxDataViewItem a_item;
		wxDataViewItem a_cont;
		struct dtsgui *dtsgui;
};

class DTSTreeWindow: public DTSObject, public virtual wxSplitterWindow {
	public:
		DTSTreeWindow(wxWindow *parent = NULL, DTSFrame *frame = NULL, dtsgui_tree_cb tree_cb = NULL, wxString stat_msg = wxEmptyString, int pos = 30, void *u_data = NULL);
		~DTSTreeWindow();
		void ShowRMenu(bool cont, int cnt, bool first, bool last, bool c_sort, bool del);
		bool Show(bool = true);
		DTSDVMCtrl *GetTreeCtrl();
		void TreeResize();
		void SetTreePaneSize();
		wxWindow *SetWindow(wxWindow *window);
		wxWindow *GetClientPane();
		void SetPaneTitle(const wxString value);
	private:
		wxScrolledWindow *t_pane;
		wxWindow *c_pane;
		DTSDVMCtrl *tree;
		DTSDVMListView *vm;
//		wxBoxSizer *sizer;
		wxWindow *a_window;
		DTSTreeWindowEvent *dtsevthandler;
		struct treemenu *rmenu;
		wxBoxSizer *treesizer;
};

class DTSTabWindow: public DTSObject, public virtual wxNotebook {
	public:
		DTSTabWindow(DTSFrame *frame = NULL, wxString stat_msg = wxEmptyString);
		bool Show(bool = true);
};

#endif // DTSTREEWINDOW_H
