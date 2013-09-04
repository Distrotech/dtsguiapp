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

#ifdef __WIN32__
#define UNICODE 1
#include <winsock2.h>
#include <windows.h>
#include <shlobj.h>
#endif

#include <stdint.h>

#include <wx/app.h>
#include <wx/frame.h>
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/scrolwin.h>
#include <wx/gbsizer.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/dialog.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/splitter.h>
#include <wx/dataview.h>
#include <wx/wizard.h>
#include <wx/filedlg.h>
#include <wx/notebook.h>

#include <dtsapp.h>
#include "dtsgui.hpp"

#include "evdata.h"
#include "DTSApp.h"
#include "DTSFrame.h"
#include "DTSPanel.h"
#include "DTSTreeWindow.h"

static int menuid = wxID_AUTO_LOWEST;

dtsgui *dtsgui_config(dtsgui_configcb confcallback_cb, void *userdata, struct point wsize, struct point wpos, const char *title, const char *status) {
	/*deleted on close*/
	DTSApp *guiapp = new DTSApp();

	return guiapp->CreateFrame(confcallback_cb, userdata, wsize, wpos, title, status);
}

int dtsgui_run(int argc, char **argv) {
	return wxEntry(argc, argv);
}

static void dtsgui_quit(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, int type) {
	wxMenu *m = (wxMenu *)dtsmenu;
	DTSFrame *frame = (DTSFrame *)dtsgui->appframe;

	switch (type) {
		case wxID_EXIT:
			m->Append(type, "&Quit", "Quit And Exit");
			frame->Bind(wxEVT_COMMAND_MENU_SELECTED, &DTSFrame::OnClose, frame, wxID_EXIT);
			break;

		case wxID_SAVE:
			m->Append(type, "&Exit", "Save And Exit");
			frame->Bind(wxEVT_COMMAND_MENU_SELECTED, &DTSFrame::OnClose, frame, wxID_SAVE);
			break;
	}
}

void dtsgui_exit(dtsgui_menu dtsmenu, struct dtsgui *dtsgui) {
	dtsgui_quit(dtsmenu, dtsgui, wxID_EXIT);
}

void dtsgui_close(dtsgui_menu dtsmenu, struct dtsgui *dtsgui) {
	dtsgui_quit(dtsmenu, dtsgui, wxID_SAVE);
}

wxWindow *getpanewindow(dtsgui_pane pane) {
	DTSObject *p = (DTSObject *)pane;
	return p->GetPanel();
}

wxString getpanename(dtsgui_pane pane) {
	DTSObject *p = (DTSObject *)pane;
	return p->GetName();
}

wxMenuBar *DTSFrame::GetMenuBar() {
	return menubar;
}

dtsgui_menu dtsgui_newmenu(struct dtsgui *dtsgui, const char *name) {
	/*deleted with menubar*/
	wxMenu *new_menu = NULL;
	wxMenuBar *menubar;
	DTSFrame *frame = (DTSFrame *)dtsgui->appframe;

	if ((menubar = frame->GetMenuBar())) {
		new_menu = new wxMenu;
		menubar->Append(new_menu, name);
	}

	return new_menu;
}

void dtsgui_about(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, const char *text) {
	wxMenu *m = (wxMenu *)dtsmenu;
	DTSFrame *frame = (DTSFrame *)dtsgui->appframe;


	frame->SetAbout(text);
	m->Append(wxID_ABOUT);
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED, &DTSFrame::OnAbout, frame, wxID_ABOUT);
}

void dtsgui_menusep(dtsgui_menu dtsmenu) {
	wxMenu *m = (wxMenu *)dtsmenu;
	m->AppendSeparator();
}

void dtsgui_newmenuitem(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, const char *hint, dtsgui_pane p) {
	wxMenu *m = (wxMenu *)dtsmenu;
	DTSFrame *frame = (DTSFrame *)dtsgui->appframe;

	wxWindow *w = (p) ? getpanewindow(p) : NULL;

	/*handed over to wx no need to delete*/
	evdata *ev_data = new evdata(w);

	m->Append(menuid, hint, (p) ? getpanename(p) : "");
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED, &DTSFrame::SwitchWindow, frame, menuid, menuid, (wxObject *)ev_data);
	menuid++;
}

void dtsgui_newmenucb(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, const char *hint, const char *label, dtsgui_configcb cb, void *data) {
	wxMenu *m = (wxMenu *)dtsmenu;
	DTSFrame *frame = (DTSFrame *)dtsgui->appframe;

	/*handed over to wx no need to delete*/
	evdata *ev_data = new evdata(data, cb);

	m->Append(menuid, hint, label);
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED, &DTSFrame::RunCommand, frame, menuid, menuid, (wxObject *)ev_data);
	menuid++;
}

void newappframe(struct dtsgui *dtsgui) {
	new DTSFrame(dtsgui->title, wxPoint(dtsgui->wpos.x, dtsgui->wpos.y),
				 wxSize(dtsgui->wsize.x, dtsgui->wsize.y), dtsgui);
}

int dtsgui_confirm(struct dtsgui *dtsgui, const char *text) {
	DTSFrame *f = (DTSFrame *)dtsgui->appframe;

	return f->Confirm(text);
}

void dtsgui_alert(struct dtsgui *dtsgui, const char *text) {
	DTSFrame *f = (DTSFrame *)dtsgui->appframe;

	f->Alert(text);
}

dtsgui_pane dtsgui_panel(struct dtsgui *dtsgui, const char *name, int butmask,
						 enum panel_type type, void *userdata) {
	wxFrame *frame = (wxFrame *)dtsgui->appframe;
	DTSPanel *dp = NULL;

	switch (type) {
		case wx_DTSPANEL_SCROLLPANEL:
			dp = new DTSScrollPanel(frame, frame, name, butmask);
			break;
		case wx_DTSPANEL_PANEL:
			dp = new DTSStaticPanel(frame, frame, name, butmask);
			break;
		case wx_DTSPANEL_WINDOW:
			dp = new DTSWindow(frame);
//			p = new DTSWindow(frame, frame, name);
			break;
		case wx_DTSPANEL_DIALOG:
			dp = new DTSDialog(frame, name, butmask);
			break;
		case wx_DTSPANEL_TAB:
		case wx_DTSPANEL_TREE:
			break;
		case wx_DTSPANEL_WIZARD:
			dp = new DTSWizardWindow(name);
			break;
	}

	dp->SetUserData(userdata);
	return dp;
}

extern void dtsgui_xmlpanel(dtsgui_pane pane, struct xml_doc *xmldoc) {
	DTSPanel *p = (DTSPanel *)pane;
	p->SetXMLDoc(xmldoc);
}

void dtsgui_delpane(dtsgui_pane pane) {
	DTSPanel *p = (DTSPanel *)pane;
	delete p;
}

extern dtsgui_treeview dtsgui_treewindow(struct dtsgui *dtsgui, const char *title) {
	DTSTreeWindow *tw;
	wxFrame *frame = (wxFrame *)dtsgui->appframe;

	tw = new DTSTreeWindow(frame, frame, title, 30);
	return tw;
}

extern dtsgui_tabview dtsgui_tabwindow(struct dtsgui *dtsgui, const char *title) {
	DTSTabWindow *tw;
	wxFrame *frame = (wxFrame *)dtsgui->appframe;

	tw = new DTSTabWindow(frame, title);
	return tw;
}

extern dtsgui_pane dtsgui_addpage(dtsgui_tabview tv, const char *name, int butmask, enum panel_type type, void *userdata) {
	DTSPanel *dp = NULL;
	DTSTabWindow *tw = (DTSTabWindow*)tv;
	wxNotebook *parent = dynamic_cast<wxNotebook*>(tw);
	wxPanel *np;

	switch (type) {
		case wx_DTSPANEL_SCROLLPANEL:
			dp = new DTSScrollPanel(parent, NULL, name, butmask);
			break;
		case wx_DTSPANEL_PANEL:
			dp = new DTSStaticPanel(parent, NULL, name, butmask);
			break;
		case wx_DTSPANEL_WINDOW:
		case wx_DTSPANEL_DIALOG:
		case wx_DTSPANEL_TAB:
		case wx_DTSPANEL_TREE:
		case wx_DTSPANEL_WIZARD:
			return NULL;
	}

	np = dynamic_cast<wxPanel*>(dp);
	parent->AddPage(np, name);
	return dp;
}

extern void dtsgui_showpanel(dtsgui_pane pane) {
	DTSPanel *dp = (DTSPanel*)pane;

	dp->ShowPanel(true);
	dp->Show(true);

}

extern void dtsgui_textbox(dtsgui_pane pane, const char *title, const char *name, const char *value, void *data) {
	DTSPanel *p = (DTSPanel *)pane;

	p->TextBox(title, name, value, wxTE_LEFT, 1, data, DTSGUI_FORM_DATA_PTR);
}

extern void dtsgui_textbox_multi(dtsgui_pane pane, const char *title, const char *name, const char *value, void *data) {
	DTSPanel *p = (DTSPanel *)pane;

	p->TextBox(title, name, value, wxTE_MULTILINE, 5, data,  DTSGUI_FORM_DATA_PTR);
}

extern void dtsgui_passwdbox(dtsgui_pane pane, const char *title, const char *name, const char *value, void *data) {
	DTSPanel *p = (DTSPanel *)pane;

	p->TextBox(title, name, value, wxTE_PASSWORD, 1, data,  DTSGUI_FORM_DATA_PTR);
}

extern void dtsgui_checkbox(dtsgui_pane pane, const char *title, const char *name, int ischecked, void *data) {
	DTSPanel *p = (DTSPanel *)pane;

	p->CheckBox(title, name, ischecked, NULL, NULL, data,  DTSGUI_FORM_DATA_PTR);
}

extern struct form_item *dtsgui_listbox(dtsgui_pane pane, const char *title, const char *name, void *data) {
	DTSPanel *p = (DTSPanel *)pane;

	return p->ListBox(title, name, NULL, data,  DTSGUI_FORM_DATA_PTR);
}

extern struct form_item *dtsgui_combobox(dtsgui_pane pane, const char *title, const char *name, void *data) {
	DTSPanel *p = (DTSPanel *)pane;

	return p->ComboBox(title, name, NULL, data, DTSGUI_FORM_DATA_PTR);
}

const char *getxmlvalue(struct xml_element *xml) {
	struct xml_node *xn;
	const char *ret = NULL;
	const char *tmp;

	if (!xml || !xml->xsearch) {
		return NULL;
	}

	xn = xml_getfirstnode(xml->xsearch, NULL);
	if (!xml->attr) {
		if (xn->value) {
			ret = strdup(xn->value);
		}
	} else if ((tmp = xml_getattr(xn, xml->attr))) {
			ret = strdup(tmp);
	}

	objunref(xn);
	return ret;
}

extern void dtsgui_xmltextbox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	struct xml_element *xml;
	const char *value = NULL;

	if ((xml = p->GetNode(xpath, attr))) {
		value = getxmlvalue(xml);
	}

	p->TextBox(title, name, value, wxTE_LEFT, 1, xml,  DTSGUI_FORM_DATA_XML);

	if (value) {
		free((void*)value);
	}
}

extern void dtsgui_xmltextbox_multi(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	struct xml_element *xml;
	const char *value = NULL;

	if ((xml = p->GetNode(xpath, attr))) {
		value = getxmlvalue(xml);
	}

	p->TextBox(title, name, value, wxTE_MULTILINE, 5, xml, DTSGUI_FORM_DATA_XML);

	if (value) {
		free((void*)value);
	}
}

extern void dtsgui_xmlpasswdbox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	struct xml_element *xml;
	const char *value = NULL;

	if ((xml = p->GetNode(xpath, attr))) {
		value = getxmlvalue(xml);
	}

	p->TextBox(title, name, value, wxTE_PASSWORD, 1, xml, DTSGUI_FORM_DATA_XML);

	if (value) {
		free((void*)value);
	}
}

extern void dtsgui_xmlcheckbox(dtsgui_pane pane, const char *title, const char *name, const char *checkval, const char *uncheckval, const char *xpath, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	struct xml_element *xml;
	int ischecked = 0;
	const char *value = NULL;

	if ((xml = p->GetNode(xpath, attr))) {
		value = getxmlvalue(xml);
	}

	if (value && checkval && !strcmp(value, checkval)) {
		ischecked = 1;
	}

	p->CheckBox(title, name, ischecked, checkval, uncheckval, xml, DTSGUI_FORM_DATA_XML);

	if (value) {
		free((void*)value);
	}
}

struct form_item *dtsgui_xmllistbox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	const char *value = NULL;
	struct xml_element *xml;
	struct form_item *fi;

	if ((xml = p->GetNode(xpath, attr))) {
		value = getxmlvalue(xml);
	}
	fi = p->ListBox(title, name, value, xml, DTSGUI_FORM_DATA_XML);

	if (value) {
		free((void*)value);
	}

	return fi;
}

struct form_item *dtsgui_xmlcombobox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	const char *value = NULL;
	struct xml_element *xml;
	struct form_item *fi;

	if ((xml = p->GetNode(xpath, attr))) {
		value = getxmlvalue(xml);
	}

	fi = p->ComboBox(title, name, value, xml, DTSGUI_FORM_DATA_XML);

	if (value) {
		free((void*)value);
	}

	return fi;
}

void dtsgui_listbox_add(struct form_item *listbox, const char *text, const char *value) {
	wxComboBox *lbox = (wxComboBox *)listbox->widget;
	lbox->Append(text, (void*)value);

	if (lbox->GetSelection() == wxNOT_FOUND) {
		lbox->SetSelection(0);
	} else if (listbox->value && value && !strcmp(listbox->value, value)) {
		lbox->SetSelection(lbox->GetCount()-1);
	}
}

void dtsgui_setevcallback(dtsgui_pane pane,event_callback evcb, void *data) {
	DTSPanel *p = (DTSPanel *)pane;

	return p->SetEventCallback(evcb, data);
}

void dtsgui_rundialog(dtsgui_pane pane, event_callback evcb, void *data) {
	DTSDialog *p = (DTSDialog *)pane;

	p->SetEventCallback(evcb, data);
	p->RunDialog();
	delete p;
}

dtsgui_pane dtsgui_textpane(struct dtsgui *dtsgui, const char *title, const char *buf) {
	wxFrame *f = (wxFrame *)dtsgui->appframe;
	DTSStaticPanel *p = new DTSStaticPanel(f, f, title);
	wxWindow *w = p->GetPanel();

	wxTextCtrl *eb = new wxTextCtrl(w, -1, buf, wxDefaultPosition, wxDefaultSize,
									wxTE_MULTILINE | wxTE_RICH | wxTE_READONLY, wxDefaultValidator, wxTextCtrlNameStr);

	p->AddItem(eb, wxGBPosition(0,0), wxGBSpan(10, 6), wxEXPAND|wxGROW, 0, 0);

	return p;
}

void *dtsgui_userdata(struct dtsgui *dtsgui) {
	return dtsgui->userdata;
}

struct bucket_list *dtsgui_panel_items(dtsgui_pane pane) {
	DTSDialog *p = (DTSDialog *)pane;
	return p->GetItems();
}

struct form_item *dtsgui_panel_getname(dtsgui_pane pane, const char *name) {
	DTSDialog *p = (DTSDialog *)pane;
	struct bucket_list *bl = p->GetItems();
	return (struct form_item *)bucket_list_find_key(bl, name);
}

extern void *dtsgui_item_data(struct form_item *fi) {
	if (fi && fi->data.ptr) {
		return fi->data.ptr;
	} else {
		return NULL;
	}
}

extern const char *dtsgui_item_name(struct form_item *fi) {
	return fi->name;
}

extern struct form_item *dtsgui_finditem(dtsgui_pane p, const char *name) {
	struct form_item *fi;
	struct bucket_list *il;

	il = dtsgui_panel_items(p);
	fi = (struct form_item*)bucket_list_find_key(il, (void*)name);
	objunref(il);

	return fi;
}

extern 	const char *dtsgui_item_value(struct form_item *fi) {
	const char *value = NULL;
	union widgets {
		wxTextCtrl *t;
	} w;

	switch(fi->type) {
		case DTS_WIDGET_TEXTBOX:
			w.t = (wxTextCtrl *)fi->widget;
			value = strdup(w.t->GetValue().ToUTF8());
			break;
		case DTS_WIDGET_CHECKBOX:
		case DTS_WIDGET_LISTBOX:
		case DTS_WIDGET_COMBOBOX:
			break;
	}
	return value;
}

extern const char *dtsgui_findvalue(dtsgui_pane p, const char *name) {
	struct form_item *fi;
	const char *val;

	fi = dtsgui_finditem(p, name);
	val = dtsgui_item_value(fi);
	objunref(fi);
	return val;
}

void dtsgui_wizard_free(void *data) {
	struct dtsgui_wizard *dtswiz = (struct dtsgui_wizard*)data;

	if (dtswiz->wiz) {
		delete dtswiz->wiz;
	}
}

extern struct dtsgui_wizard* dtsgui_newwizard(struct dtsgui *dtsgui, const char *title) {
	struct dtsgui_wizard *dtswiz;
	wxFrame *f = (wxFrame *)dtsgui->appframe;

	if (!(dtswiz = (dtsgui_wizard*)objalloc(sizeof(*dtswiz),dtsgui_wizard_free))) {
		return NULL;
	}

	dtswiz->wiz = new wxWizard();
	dtswiz->start = NULL;
	dtswiz->dtsgui = dtsgui;

	if (!dtswiz->wiz || !dtswiz->wiz->Create(f, wxID_ANY, title, wxNullBitmap, wxDefaultPosition, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)) {
		if (dtswiz->wiz) {
			delete dtswiz->wiz;
		}

		return NULL;
	}

	return dtswiz;
}

extern dtsgui_pane dtsgui_wizard_addpage(struct dtsgui_wizard *dtswiz, const char *title, void *userdata, struct xml_doc *xmldoc) {
	dtsgui_pane page;
	DTSWizardWindow *dww;
	wxWizardPageSimple *wp, *tmp;
	wxWizardPage *last;

	page = dtsgui_panel(dtswiz->dtsgui, title, 0, wx_DTSPANEL_WIZARD, userdata);
	dww = (DTSWizardWindow*)page;
	wp = dynamic_cast<wxWizardPageSimple *>(dww);

	wp->Create(dtswiz->wiz);

	if (!dtswiz->start) {
		dtswiz->start = wp;
	} else {
		for(last = dtswiz->start; last->GetNext(); last=last->GetNext());

		tmp = dynamic_cast<wxWizardPageSimple*>(last);
		tmp->SetNext(wp);
		wp->SetPrev(tmp);
	}

	if (title) {
		dww->Title(title);
	}

	if (xmldoc) {
		dww->SetXMLDoc(xmldoc);
	}

	return page;
}

extern int dtsgui_runwizard(struct dtsgui_wizard *dtswiz) {
	dtswiz->wiz->GetPageAreaSizer()->Add(dtswiz->start);
	dtswiz->wiz->Center();

	return dtswiz->wiz->RunWizard(dtswiz->start);
}

const char *dtsgui_filedialog(struct dtsgui *dtsgui, const char *title, const char *path, const char *name, const char *filter, long style) {
	wxFrame *f = (wxFrame *)dtsgui->appframe;
	const char *filename = NULL;
	int len;

	wxFileDialog *fd = new wxFileDialog(f, title, (path) ? path : "", (name) ? name : "",
										(filter) ? filter : wxFileSelectorDefaultWildcardStr, style);
	if (fd->ShowModal() != wxID_CANCEL) {
		len = strlen(fd->GetPath());
		filename = (const char*)objalloc(len+1, NULL);
		strcpy((char*)filename, fd->GetPath());
	}

	delete fd;
	return filename;
}

extern const char *dtsgui_filesave(struct dtsgui *dtsgui, const char *title, const char *path, const char *name, const char *filter) {
	return dtsgui_filedialog(dtsgui, title, path, name, filter, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
}

extern const char *dtsgui_fileopen(struct dtsgui *dtsgui, const char *title, const char *path, const char *name, const char *filter) {
	return dtsgui_filedialog(dtsgui, title, path, name, filter, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
}

extern void dtsgui_xmlpanel_update(dtsgui_pane pane) {
	DTSPanel *p = (DTSPanel*)pane;
	p->Update_XML();
}


extern void *dtsgui_paneldata(dtsgui_pane pane) {
	DTSPanel *p = (DTSPanel*)pane;
	return p->GetUserData();
}

extern struct xml_doc *dtsgui_loadxmlurl(struct dtsgui *dtsgui, const char *user, const char *passwd, const char *url) {
	struct curlbuf *cbuf;
	struct xml_doc *xmldoc = NULL;
	struct basic_auth *auth;

	if (user && passwd) {
		auth = dtsgui_pwdialog(user, passwd, dtsgui);
	} else {
		auth = NULL;
	}

	auth = dtsgui_pwdialog(user, passwd, dtsgui);
	if (!(cbuf = curl_posturl(url, auth, NULL, dtsgui_pwdialog, dtsgui))) {
		objunref(auth);
		return NULL;
	}

	curl_ungzip(cbuf);

	if (cbuf && cbuf->c_type && !strcmp("application/xml", cbuf->c_type)) {
		xmldoc = xml_loadbuf(cbuf->body, cbuf->bsize, 1);
	}

	objunref(cbuf);
	objunref(auth);
	return xmldoc;
}

#ifdef __WIN32
void getwin32folder(int csidl, char *path) {
	SHGetFolderPathA(NULL, csidl, NULL, 0, path);
}
#endif
