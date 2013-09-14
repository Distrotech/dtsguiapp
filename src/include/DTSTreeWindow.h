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
		void TreeCallback(const wxDataViewItem item);
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
		DTSTreeWindow(wxWindow *parent = NULL, DTSFrame *frame = NULL, dtsgui_tree_cb tree_cb = NULL, wxString stat_msg = wxEmptyString, int pos = 30);
		~DTSTreeWindow();
		void ShowRMenu(bool cont, int cnt, bool first, bool last);
		bool Show(bool = true);
		DTSDVMCtrl *GetTreeCtrl();
		void TreeResize();
		void SetTreePaneSize();
		wxWindow *SetWindow(wxWindow *window);
		wxWindow *GetClientPane();
	private:
		wxScrolledWindow *t_pane;
		wxWindow *c_pane;
		DTSDVMCtrl *tree;
		DTSDVMListView *vm;
		wxBoxSizer *sizer;
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
