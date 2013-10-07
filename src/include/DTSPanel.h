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

typedef class wxComboBox wxComboBox;
typedef class wxCheckBox wxCheckBox;
typedef class wxStaticText wxStaticText;
typedef class wxBookCtrlBase wxBookCtrlBase;
typedef class wxDialog wxDialog;
typedef class DTSFrame DTSFrame;

typedef class DTSObject DTSObject;

class DTSPanelEvent: public wxEvtHandler {
	public:
		DTSPanelEvent(DTSObject *win = NULL);
		~DTSPanelEvent();
		void BindButton(wxWindow *win, int button);
		void BindDTSEvent(DTSFrame *frame);
		void BindCombo(wxWindow *win, int button);
		void OnButton(wxCommandEvent &event);
		void OnCombo(wxCommandEvent &event);
		void OnDTSEvent(wxCommandEvent &event);
		void SetCallback(event_callback evcb = NULL, void *userdata = NULL);
		void OnDialog(wxCommandEvent &event);
	private:
		void **refobj;
		int RunCallBack(int etype, int eid, void *cb_data);
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
		DTSFrame *GetFrame();
		void EventHandler(int eid, wxCommandEvent *event);
		struct bucket_list *GetItems(void);
		void SetUserData(void *data);
		void *GetUserData(void);
		void SetXMLDoc(struct xml_doc *xmldoc);
		struct xml_doc *GetXMLDoc(void);
		panel_type type;
		int buttons[6];
	protected:
		void **refobj;
		DTSFrame *frame;
		wxString status;
		wxWindow *panel;
		wxEvtHandler *dtsevthandler;
		struct bucket_list *fitems;
		void *userdata;
		struct xml_doc  *xmldoc;
		bool beenshown;
};

class DTSPanel: public DTSObject {
	public:
		DTSPanel(DTSFrame* = NULL, wxString = wxEmptyString, int = 0);
		~DTSPanel();
		void TextBox(const char *, const char *, wxString = wxEmptyString, int flags = wxTE_LEFT, int rows = 1, void *data = NULL, enum form_data_type dtype = DTSGUI_FORM_DATA_PTR);
		void PasswdBox(const char *, const char *, wxString = wxEmptyString, int flags = wxTE_LEFT, void *data = NULL, enum form_data_type dtype = DTSGUI_FORM_DATA_PTR);
		void CheckBox(const char *title, const char *name, int ischecked, const char *checkval, const char *uncheckval, void *data = NULL, enum form_data_type dtype = DTSGUI_FORM_DATA_PTR);
		class form_item *ListBox(const char *title, const char *name, const char *value, void *data = NULL, enum form_data_type dtype = DTSGUI_FORM_DATA_PTR);
		class form_item *ComboBox(const char *title, const char *name, const char *value, void *data = NULL, enum form_data_type dtype = DTSGUI_FORM_DATA_PTR);
		void AddItem(wxWindow *item, const wxGBPosition pos, const wxGBSpan span = wxDefaultSpan, int flags = 0, int border = 0,	int growrow = -1);
		void SetEventCallback(event_callback evcb, void *userdata = NULL);
		void SetConfigCallback(dtsgui_configcb cb, void *userdata = NULL);
		void Update_XML();
		struct xml_node *Panel2XML(const char *xpath, const char *node, const char *nodeval, const char *attrkey);
		class xml_element *GetNode(const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr);
		bool ShowPanel(bool = true);
		void SetTitle(const wxString new_title, bool create = false);
		void SetStatus(const wxString new_status);
	protected:
		void Title(const char *title);
		void SetSizerSize(wxSize, wxWindow*);
		void SetupWin();
		void Buttons(void);
		int button_mask;
	private:
		wxStaticText *title;
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

class DTSTabPage: public DTSScrollPanel {
	public:
		DTSTabPage(wxBookCtrlBase*, DTSFrame* = NULL, wxString = wxEmptyString, bool = false, int = 0, dtsgui_tabpanel_cb  = NULL, void* = NULL, struct xml_doc* = NULL);
		~DTSTabPage();
		void ConfigPane();
		void InsertPage(int pos);
		virtual bool Show(bool);
		DTSTabPage &operator=(const DTSTabPage &orig);
	private:
		dtsgui_tabpanel_cb cb;
		void *cdata;
		bool hasconfig;
};

class DTSWindow: public DTSPanel, public virtual wxWindow {
	public:
		DTSWindow(wxWindow *, DTSFrame* = NULL, wxString = wxEmptyString);
		bool Show(bool = true);
};

class DTSDialog: public DTSStaticPanel {
	public:
		DTSDialog(DTSFrame* = NULL, wxString = wxEmptyString, int = wx_PANEL_BUTTON_ACTION);
		~DTSDialog();
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
