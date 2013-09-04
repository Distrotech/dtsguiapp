#ifndef DTSTREEWINDOW_H
#define DTSTREEWINDOW_H

class DTSTreeWindow: public DTSObject, public virtual wxSplitterWindow {
	public:
		DTSTreeWindow(wxWindow *parent = NULL, wxFrame *frame = NULL, wxString stat_msg = wxEmptyString, int pos = 30);
		~DTSTreeWindow();
		bool Show(bool = true);
	protected:
		void SetWindow(wxWindow *window);
	private:
		wxPanel *t_pane;
		wxWindow *c_pane;
		wxDataViewTreeCtrl *tree;
		wxBoxSizer *sizer;
		wxWindow *a_window;
};

class DTSTabWindow: public DTSObject, public virtual wxNotebook {
	public:
		DTSTabWindow(wxFrame *frame = NULL, wxString stat_msg = wxEmptyString);
		bool Show(bool = true);
};

#endif // DTSTREEWINDOW_H
