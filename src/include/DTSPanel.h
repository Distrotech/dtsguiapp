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

typedef class DTSObject DTSObject;

struct xml_element {
	struct xml_search *xsearch;
	const char *xpath;
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
	const char *value;
	const char *value2;
	void *widget;
};

class DTSPanelEvent: public wxEvtHandler {
	public:
		DTSPanelEvent(DTSObject *win = NULL);
		void BindButton(wxWindow *win, int button);
		void BindDTSEvent(DTSFrame *frame);
		void BindCombo(wxWindow *win, int button);
		void OnButton(wxCommandEvent &event);
		void OnCombo(wxCommandEvent &event);
		void OnDTSEvent(wxCommandEvent &event);
		void SetCallback(event_callback evcb = NULL, void *userdata = NULL);
		void OnDialog(wxCommandEvent &event);
	private:
		void *data;
		event_callback evcb;
		DTSObject *parent;
};

class DTSObject: public virtual wxWindow {
	public:
		DTSObject(wxString = wxEmptyString);
		~DTSObject();
		wxString GetName();
		wxWindow *GetPanel();
		panel_type type;
		DTSFrame *GetFrame();
		int buttons[6];
		void EventHandler(int eid, wxCommandEvent *event);
		struct bucket_list *GetItems(void);
		void SetUserData(void *data);
		void *GetUserData(void);
		void SetXMLDoc(struct xml_doc *xmldoc);
		struct xml_doc *GetXMLDoc(void);
	protected:
		DTSFrame *frame;
		wxString status;
		wxWindow *panel;
		DTSPanelEvent *dtsevthandler;
		struct bucket_list *fitems;
		void *userdata;
		struct xml_doc  *xmldoc;
		bool beenshown;
};

class DTSPanel: public DTSObject {
	public:
		DTSPanel(DTSFrame* = NULL, wxString = wxEmptyString, int = 0);
		~DTSPanel();
		void Title(const char *title);
		void TextBox(const char *, const char *, wxString = wxEmptyString, int flags = wxTE_LEFT, int rows = 1, void *data = NULL, enum form_data_type dtype = DTSGUI_FORM_DATA_PTR);
		void PasswdBox(const char *, const char *, wxString = wxEmptyString, int flags = wxTE_LEFT, void *data = NULL, enum form_data_type dtype = DTSGUI_FORM_DATA_PTR);
		void CheckBox(const char *title, const char *name, int ischecked, const char *checkval, const char *uncheckval, void *data = NULL, enum form_data_type dtype = DTSGUI_FORM_DATA_PTR);
		struct form_item *ListBox(const char *title, const char *name, const char *value, void *data = NULL, enum form_data_type dtype = DTSGUI_FORM_DATA_PTR);
		struct form_item *ComboBox(const char *title, const char *name, const char *value, void *data = NULL, enum form_data_type dtype = DTSGUI_FORM_DATA_PTR);
		void AddItem(wxWindow *item, const wxGBPosition pos, const wxGBSpan span = wxDefaultSpan, int flags = 0, int border = 0,	int growrow = -1);
		void SetEventCallback(event_callback evcb, void *userdata = NULL);
		void SetConfigCallback(dtsgui_configcb cb, void *userdata = NULL);
		void Update_XML();
		struct xml_element *GetNode(const char *xpath, const char *attr);
		bool ShowPanel(bool = true);
		void SetTitle(const wxString new_title, bool create = false);
	protected:
		void SetSizerSize(wxSize, wxWindow*);
		void SetupWin();
		void Buttons(void);
		int button_mask;
	private:
		wxStaticText *title;
		struct form_item *create_new_fitem(void *widget, enum widget_type type, const char *name, const char *value = NULL, const char *value2 = NULL, void *data = NULL, enum form_data_type dtype = DTSGUI_FORM_DATA_PTR);
		wxGridBagSizer *fgs;
		int g_row;
		dtsgui_configcb configcb;
		void *config_data;
};

class DTSStaticPanel: public DTSPanel, public virtual wxPanel  {
	public:
		DTSStaticPanel(wxWindow *,DTSFrame* = NULL, wxString = wxEmptyString, int = 0);
		DTSStaticPanel();
		virtual bool Show(bool = true);
};

class DTSScrollPanel: public DTSPanel, public virtual wxScrolledWindow {
	public:
		DTSScrollPanel(wxWindow *, DTSFrame* = NULL, wxString = wxEmptyString, int = 0);
		virtual bool Show(bool);
};

class DTSWindow: public DTSPanel, public virtual wxWindow {
	public:
		DTSWindow(wxWindow *, DTSFrame* = NULL, wxString = wxEmptyString);
		bool Show(bool = true);
};

class DTSDialog: public DTSStaticPanel {
	public:
		DTSDialog(DTSFrame* = NULL, wxString = wxEmptyString, int = wx_PANEL_BUTTON_ACTION);
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
