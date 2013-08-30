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

#ifndef DTSPANEL_H
#define DTSPANEL_H

typedef class DTSPanel DTSPanel;

struct xml_element {
	struct xml_search *xsearch;
	const char  *attr;
};

union form_data {
	void *ptr;
	struct xml_element *xml;
};

enum form_data_type {
	DTSGUI_FORM_DATA_PTR,
	DTSGUI_FORM_DATA_XML
};

struct form_item {
	union form_data data;
	enum form_data_type dtype;
	enum widget_type type;
	const char *name;
	const char *value2;
	void *widget;
};

class DTSPanelEvent: public wxEvtHandler {
	public:
		DTSPanelEvent(void *userdata = NULL, event_callback evcb = NULL, DTSPanel *win = NULL);
		void BindButton(wxWindow *win, int button);
		void BindCombo(wxWindow *win, int button);
		void OnButton(wxCommandEvent &event);
		void OnCombo(wxCommandEvent &event);
	private:
		void *data;
		event_callback evcb;
		DTSPanel *parent;
};

class DTSPanel: public virtual wxWindow {
	public:
		DTSPanel(wxFrame* = NULL, wxString = wxEmptyString, int = 0);
		~DTSPanel();
		void Title(const char *title);
		void TextBox(const char *, wxString = wxEmptyString, int flags = wxTE_LEFT, int rows = 1, void *data = NULL, enum form_data_type dtype = DTSGUI_FORM_DATA_PTR);
		void PasswdBox(const char *, wxString = wxEmptyString, int flags = wxTE_LEFT, void *data = NULL, enum form_data_type dtype = DTSGUI_FORM_DATA_PTR);
		void CheckBox(const char *title, int ischecked, void *data = NULL, enum form_data_type dtype = DTSGUI_FORM_DATA_PTR);
		struct form_item *ListBox(const char *title, void *data = NULL, enum form_data_type dtype = DTSGUI_FORM_DATA_PTR);
		struct form_item *ComboBox(const char *title, void *data = NULL, enum form_data_type dtype = DTSGUI_FORM_DATA_PTR);
		void AddItem(wxWindow *item, const wxGBPosition pos, const wxGBSpan span = wxDefaultSpan, int flags = 0, int border = 0,	int growrow = -1);
		void SetEventCallback(event_callback evcb, void *userdata = NULL);
		void EventHandler(int eid, wxCommandEvent *event);
		void SetUserData(void *data);
		void *GetUserData(void);
		struct bucket_list *GetItems(void);
		void SetXMLDoc(struct xml_doc *xmldoc);
		struct xml_element *GetNode(const char *xpath, const char *attr);
		wxString GetName();
		wxWindow *GetPanel();
		panel_type type;
		int buttons[6];
	protected:
		void SetSizerSize(wxSize, wxWindow*);
		bool ShowPanel(bool = true);
		void SetupWin();
		DTSPanelEvent *dtsevthandler;
		int button_mask;
		wxString status;
		wxFrame *frame;
		void Buttons(void);
		wxWindow *panel;
	private:
		struct form_item *create_new_fitem(void *widget, enum widget_type type, const char *name, const char *value, void *data = NULL, enum form_data_type dtype = DTSGUI_FORM_DATA_PTR);
		wxGridBagSizer *fgs;
		bool beenshown;
		int g_row;
		void *userdata;
		struct xml_doc  *xmldoc;
		struct bucket_list *fitems;
};

class DTSStaticPanel: public DTSPanel, public virtual wxPanel  {
	public:
		DTSStaticPanel(wxWindow *,wxFrame* = NULL, wxString = wxEmptyString, int = 0);
		DTSStaticPanel();
		virtual bool Show(bool = true);
};

class DTSScrollPanel: public DTSPanel, public virtual wxScrolledWindow {
	public:
		DTSScrollPanel(wxWindow *, wxFrame* = NULL, wxString = wxEmptyString, int = 0);
		virtual bool Show(bool);
};

class DTSWindow: public DTSPanel, public virtual wxWindow {
	public:
		DTSWindow(wxWindow *, wxFrame* = NULL, wxString = wxEmptyString);
		bool Show(bool = true);
};

class DTSDialog: public DTSStaticPanel {
	public:
		DTSDialog(wxFrame* = NULL, wxString = wxEmptyString, int = wx_PANEL_BUTTON_ACTION);
		bool Show(bool = true);
		void RunDialog(void);
	private:
		wxBoxSizer *sizer;
		wxDialog *dialog;
};

class DTSWizardWindow: public DTSPanel, public virtual wxWizardPageSimple {
	public:
		DTSWizardWindow(wxString = wxEmptyString);
		virtual bool Show(bool = true);
};

#endif // DTSPANEL_H
