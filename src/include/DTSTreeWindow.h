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
	private:
		void MoveDown(wxDataViewItem p_cont);
		void MoveUp(wxDataViewItem p_cont);
		void Float(wxDataViewItemArray items, wxDataViewItem p_cont, wxDataViewItem f_item, int i);
		void Sort(wxDataViewItem p_cont);
		void *data;
		event_callback evcb;
		DTSTreeWindow *parent;
		wxDataViewTreeStore *vm;
		wxDataViewTreeCtrl *tree;
		wxDataViewItem a_item;
		wxDataViewItem a_cont;
};

class DTSTreeWindow: public DTSObject, public virtual wxSplitterWindow {
	public:
		DTSTreeWindow(wxWindow *parent = NULL, DTSFrame *frame = NULL, wxString stat_msg = wxEmptyString, int pos = 30);
		~DTSTreeWindow();
		void ShowRMenu(bool cont, int cnt, bool first, bool last);
		bool Show(bool = true);
		wxDataViewTreeCtrl *GetTreeCtrl();
		void TreeResize();
	protected:
		void SetWindow(wxWindow *window);
	private:
		wxScrolledWindow *t_pane;
		wxWindow *c_pane;
		wxDataViewTreeCtrl *tree;
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
