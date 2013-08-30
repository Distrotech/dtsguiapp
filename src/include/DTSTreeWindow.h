#ifndef DTSTREEWINDOW_H
#define DTSTREEWINDOW_H

class DTSTreeWindow: public DTSPanel, public virtual wxSplitterWindow {
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

#endif // DTSTREEWINDOW_H
