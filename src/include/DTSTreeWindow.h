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
		DTSTreeWindowEvent(void *userdata = NULL, event_callback evcb = NULL, DTSTreeWindow *win = NULL);
		void TreeEvent(wxDataViewEvent &event);
		void MenuEvent(wxCommandEvent &event);
		void SplitterEvent(wxSplitterEvent& event);
	private:
		bool ParentConfig(const wxDataViewItem parent, wxDataViewItemArray &cont, wxDataViewItemArray &sel);
		void *data;
		event_callback evcb;
		DTSTreeWindow *parent;
		DTSDVMListView *vm;
		DTSDVMCtrl *tree;
		wxDataViewItem a_item;
		wxDataViewItem a_cont;
};

class DTSTreeWindow: public DTSObject, public virtual wxSplitterWindow {
	public:
		DTSTreeWindow(wxWindow *parent = NULL, DTSFrame *frame = NULL, wxString stat_msg = wxEmptyString, int pos = 30);
		~DTSTreeWindow();
		void ShowRMenu(bool cont, int cnt, bool first, bool last);
		bool Show(bool = true);
		DTSDVMCtrl *GetTreeCtrl();
		void TreeResize();
		int GetTreePaneSize();
	protected:
		void SetWindow(wxWindow *window);
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
