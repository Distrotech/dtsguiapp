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
#include <vector>

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
#include <wx/progdlg.h>
#include <wx/toolbar.h>

#include <dtsapp.h>
#include "dtsgui.hpp"

#include "evdata.h"
#include "DTSApp.h"
#include "DTSFrame.h"
#include "DTSPanel.h"
#include "DTSListView.h"
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
			m->Append(type, "E&xit", "Save And Exit");
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

dtsgui_menuitem dtsgui_newmenuitem(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, const char *hint, dtsgui_pane p) {
	wxMenu *m = (wxMenu *)dtsmenu;
	DTSFrame *frame = (DTSFrame *)dtsgui->appframe;
	wxMenuItem *mi;

	wxWindow *w = (p) ? getpanewindow(p) : NULL;

	/*handed over to wx no need to delete*/
	evdata *ev_data = new evdata(w);

	mi = m->Append(menuid, hint, (p) ? getpanename(p) : "");
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED, &DTSFrame::SwitchWindow, frame, menuid, menuid, (wxObject *)ev_data);
	menuid++;
	return mi;
}

extern dtsgui_menuitem dtsgui_newmenucb(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, const char *hint, const char *title, int blank, dtsgui_dynpanel cb,void *data) {
	struct dynamic_panel *p_dyn;
	wxMenu *m = (wxMenu *)dtsmenu;
	DTSFrame *frame = (DTSFrame *)dtsgui->appframe;
	wxMenuItem *mi;

	if (!(p_dyn = (struct dynamic_panel*)objalloc(sizeof(*p_dyn), NULL))) {
		return NULL;
	}
	p_dyn->data = data;
	ALLOC_CONST(p_dyn->title, title);
	p_dyn->cb = cb;
	p_dyn->blank = blank;

	/*handed over to wx no need to delete*/
	evdata *ev_data = new evdata(p_dyn, 1);
	objunref(p_dyn);

	mi = m->Append(menuid, hint, (title) ? title : "");
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED, &DTSFrame::DynamicPanelEvent, frame, menuid, menuid, (wxObject *)ev_data);
	menuid++;
	return mi;
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

int dtsgui_progress_start(struct dtsgui *dtsgui, const char *text, int maxval, int quit) {
	DTSFrame *f = (DTSFrame *)dtsgui->appframe;

	return f->StartProgress(text, maxval, quit);
}

int dtsgui_progress_update(struct dtsgui *dtsgui, int newval, const char* newtext) {
	DTSFrame *f = (DTSFrame *)dtsgui->appframe;

	return f->UpdateProgress(newval, newtext);
}

int dtsgui_progress_increment(struct dtsgui *dtsgui, int ival, const char* newtext) {
	DTSFrame *f = (DTSFrame *)dtsgui->appframe;

	return f->IncProgress(ival, newtext);
}

void dtsgui_progress_end(struct dtsgui *dtsgui) {
	DTSFrame *f = (DTSFrame *)dtsgui->appframe;
	f->EndProgress();
}

dtsgui_pane dtsgui_panel(struct dtsgui *dtsgui, const char *name, int butmask,
						 enum panel_type type, void *userdata) {
	DTSFrame *frame = (DTSFrame *)dtsgui->appframe;
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

struct xml_doc *dtsgui_panelxml(dtsgui_pane pane) {
	DTSPanel *p = (DTSPanel *)pane;
	return p->GetXMLDoc();
}

void dtsgui_delpane(dtsgui_pane pane) {
	DTSPanel *p = (DTSPanel *)pane;
	delete p;
}

extern dtsgui_treeview dtsgui_treewindow(struct dtsgui *dtsgui, const char *title, dtsgui_tree_cb tree_cb, void *userdata, struct xml_doc *xmldoc) {
	DTSTreeWindow *tw;
	DTSFrame *frame = (DTSFrame *)dtsgui->appframe;

	tw = new DTSTreeWindow(frame, frame, tree_cb, title, 25);
	tw->SetXMLDoc(xmldoc);
	return tw;
}

extern dtsgui_tabview dtsgui_tabwindow(struct dtsgui *dtsgui, const char *title, void *data) {
	DTSTabWindow *tw;
	DTSFrame *frame = (DTSFrame *)dtsgui->appframe;
	tw = new DTSTabWindow(frame, title, data);
	return tw;
}

extern dtsgui_pane dtsgui_newtabpage(dtsgui_tabview tv, const char *name, int butmask, void *userdata, struct xml_doc *xmldoc, dtsgui_tabpanel_cb cb, void *cdata) {
	DTSTabPage *dp = NULL;
	DTSTabWindow *tw = (DTSTabWindow*)tv;
	wxBookCtrlBase *nb = static_cast<wxBookCtrlBase*>(tw);
	DTSFrame *f = tw->GetFrame();

	dp = new DTSTabPage(nb, f, name, true, butmask, cb, cdata, xmldoc);

	if (userdata) {
		dp->SetUserData(userdata);
	}

	return dp;
}

extern dtsgui_pane dtsgui_tabpage_insert(dtsgui_tabview tv, const char *name, int butmask, void *userdata, struct xml_doc *xmldoc, dtsgui_tabpanel_cb cb, void *cdata, int pos, int undo) {
	DTSTabPage *dp = NULL;
	DTSTabWindow *tw = (DTSTabWindow*)tv;
	wxBookCtrlBase *nb = static_cast<wxBookCtrlBase*>(tw);
	DTSFrame *f = tw->GetFrame();

	if (!(dp = new DTSTabPage(nb, f, name, false, butmask, cb, cdata, xmldoc))) {
		return NULL;
	}
	dp->InsertPage(pos);

	if (undo) {
		tw->Undo(undo);
		tw->SetSelection(pos);
	}

	return dp;
}

extern dtsgui_pane dtsgui_treepane(dtsgui_treeview tv, const char *name, int butmask, void *userdata, struct xml_doc *xmldoc) {
	DTSScrollPanel *dp;
	DTSTreeWindow *tw = (DTSTreeWindow*)tv;
	wxWindow *parent;

	parent = tw->GetPanel();
	dp = new DTSScrollPanel(parent, tw->GetFrame(), name, butmask);
	dp->type = wx_DTSPANEL_TREE;

	if (name) {
		dp->Title(name);
	}

	if (xmldoc) {
		dp->SetXMLDoc(xmldoc);
	}

	return dp;
}

dtsgui_pane dtsgui_treepane_defalt(dtsgui_treeview self, dtsgui_treenode node) {
	dtsgui_pane p = NULL;
	DTSDVMListStore *ls = (DTSDVMListStore*)node;
	struct xml_doc *xmldoc;
	int nodeid;
	const char *title = strdup(ls->GetTitle().ToUTF8());

	nodeid = ls->GetNodeID();

	if (nodeid == -1) {
		p = dtsgui_treepane(self, NULL, 0, NULL, NULL);
	} else {
		xmldoc = dtsgui_panelxml(self);
		p = dtsgui_treepane(self, title, wx_PANEL_BUTTON_ACTION, NULL, xmldoc);
		if (xmldoc) {
			objunref(xmldoc);
		};
	}
	if (title) {
		free((void*)title);
	}
	return p;
}

extern void dtsgui_textbox(dtsgui_pane pane, const char *title, const char *name, const char *value, void *data) {
	DTSPanel *p = (DTSPanel *)pane;

	p->TextBox(title, name, value, wxTE_LEFT | wxTE_PROCESS_ENTER, 1, data, DTSGUI_FORM_DATA_PTR);
}

extern void dtsgui_textbox_multi(dtsgui_pane pane, const char *title, const char *name, const char *value, void *data) {
	DTSPanel *p = (DTSPanel *)pane;

	p->TextBox(title, name, value, wxTE_MULTILINE, 5, data,  DTSGUI_FORM_DATA_PTR);
}

extern void dtsgui_passwdbox(dtsgui_pane pane, const char *title, const char *name, const char *value, void *data) {
	DTSPanel *p = (DTSPanel *)pane;

	p->TextBox(title, name, value, wxTE_PASSWORD | wxTE_PROCESS_ENTER, 1, data,  DTSGUI_FORM_DATA_PTR);
}

extern void dtsgui_checkbox(dtsgui_pane pane, const char *title, const char *name, const char *checkval, const char *uncheck, int ischecked, void *data) {
	DTSPanel *p = (DTSPanel *)pane;

	p->CheckBox(title, name, ischecked, checkval, uncheck, data,  DTSGUI_FORM_DATA_PTR);
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

extern void dtsgui_xmltextbox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	struct xml_element *xml;
	const char *value = NULL;

	if ((xml = p->GetNode(xpath, node, fattr, fval,attr))) {
		value = getxmlvalue(xml);
	}

	p->TextBox(title, name, value, wxTE_LEFT | wxTE_PROCESS_ENTER, 1, xml,  DTSGUI_FORM_DATA_XML);

	if (value) {
		free((void*)value);
	}
}

extern void dtsgui_xmltextbox_multi(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	struct xml_element *xml;
	const char *value = NULL;

 	if ((xml = p->GetNode(xpath, node, fattr, fval, attr))) {
		value = getxmlvalue(xml);
	}

	p->TextBox(title, name, value, wxTE_MULTILINE, 5, xml, DTSGUI_FORM_DATA_XML);

	if (value) {
		free((void*)value);
	}
}

extern void dtsgui_xmlpasswdbox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	struct xml_element *xml;
	const char *value = NULL;

	if ((xml = p->GetNode(xpath, node, fattr, fval, attr))) {
		value = getxmlvalue(xml);
	}

	p->TextBox(title, name, value, wxTE_PASSWORD | wxTE_PROCESS_ENTER, 1, xml, DTSGUI_FORM_DATA_XML);

	if (value) {
		free((void*)value);
	}
}

extern void dtsgui_xmlcheckbox(dtsgui_pane pane, const char *title, const char *name, const char *checkval, const char *uncheckval, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	struct xml_element *xml;
	int ischecked = 0;
	const char *value = NULL;

	if ((xml = p->GetNode(xpath, node, fattr, fval, attr))) {
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

struct form_item *dtsgui_xmllistbox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	const char *value = NULL;
	struct xml_element *xml;
	struct form_item *fi;

	if ((xml = p->GetNode(xpath, node, fattr, fval, attr))) {
		value = getxmlvalue(xml);
	}
	fi = p->ListBox(title, name, value, xml, DTSGUI_FORM_DATA_XML);

	if (value) {
		free((void*)value);
	}

	return fi;
}

struct form_item *dtsgui_xmlcombobox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	const char *value = NULL;
	struct xml_element *xml;
	struct form_item *fi;

	if ((xml = p->GetNode(xpath, node, fattr, fval, attr))) {
		value = getxmlvalue(xml);
	}

	fi = p->ComboBox(title, name, value, xml, DTSGUI_FORM_DATA_XML);

	if (value) {
		free((void*)value);
	}

	return fi;
}

void dtsgui_listbox_addxml(struct form_item *lb, struct xml_doc *xmldoc, const char *xpath, const char *nattr, const char *vattr) {
	struct xml_search *xs;
	struct xml_node *xn;
	void *iter;
	const char *name, *value;

	xs = xml_xpath(xmldoc, xpath, nattr);
	for(xn = xml_getfirstnode(xs, &iter); xn ; xn = xml_getnextnode(iter)) {
		name = (nattr) ? xml_getattr(xn, nattr) : xn->value;
		value = (vattr) ? xml_getattr(xn, vattr) : xn->value;
		dtsgui_listbox_add(lb, name, value);
		objunref(xn);
	}
}

void dtsgui_listbox_add(struct form_item *listbox, const char *text, const char *value) {
	int idx;

	wxComboBox *lbox = (wxComboBox *)listbox->widget;
	idx = lbox->Append(text, (value) ? (void*)strdup(value) : NULL);

	if ((listbox->idx == -1) && listbox->value && value && !strcmp(listbox->value, value)) {
		listbox->idx = idx;
		lbox->SetSelection(idx);
	} else if (idx == 0) {
		lbox->SetSelection(0);
	}
}

void dtsgui_listbox_set(struct form_item *listbox, int idx) {
	wxComboBox *lbox = (wxComboBox*)listbox->widget;
	lbox->SetSelection(idx);
}

void dtsgui_setevcallback(dtsgui_pane pane,event_callback evcb, void *data) {
	DTSPanel *p = (DTSPanel *)pane;

	return p->SetEventCallback(evcb, data);
}

void dtsgui_configcallback(dtsgui_pane pane,dtsgui_configcb cb, void *data) {
	DTSPanel *p = (DTSPanel *)pane;

	p->SetConfigCallback(cb, data);
}

void dtsgui_rundialog(dtsgui_pane pane, event_callback evcb, void *data) {
	DTSDialog *p = (DTSDialog *)pane;

	p->SetEventCallback(evcb, data);
	p->RunDialog();
	delete p;
}

dtsgui_pane dtsgui_textpane(struct dtsgui *dtsgui, const char *title, const char *buf) {
	DTSFrame *f = (DTSFrame *)dtsgui->appframe;
	DTSStaticPanel *p = new DTSStaticPanel(f, f, title);
	wxPanel *wxp = static_cast<wxPanel*>(p);

	wxTextCtrl *eb = new wxTextCtrl(wxp, -1, buf, wxDefaultPosition, wxDefaultSize,wxTE_MULTILINE | wxHSCROLL | wxTE_READONLY);

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
		wxComboBox *l;
		wxCheckBox *c;
	} w;

	switch(fi->type) {
		case DTS_WIDGET_TEXTBOX:
			w.t = (wxTextCtrl *)fi->widget;
			value = strdup(w.t->GetValue().ToUTF8());
			break;
		case DTS_WIDGET_LISTBOX:
		case DTS_WIDGET_COMBOBOX:
			int pos;
			w.l = (wxComboBox *)fi->widget;
			pos = w.l->GetSelection();
			value = strdup((char*)w.l->GetClientData(pos));
			break;
		case DTS_WIDGET_CHECKBOX:
			w.c = (wxCheckBox *)fi->widget;
			if (w.c->IsChecked()) {
				value = (fi->value) ? strdup(fi->value) : NULL;
			} else {
				value = (fi->value2) ? strdup(fi->value2) : NULL;
			}
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
	DTSFrame *f = (DTSFrame *)dtsgui->appframe;

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
	DTSFrame *f = (DTSFrame *)dtsgui->appframe;
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

void dtsgui_titleappend(struct dtsgui *dtsgui, const char *text) {
	DTSFrame *f = (DTSFrame *)dtsgui->appframe;
	char *newtitle;
	int len;

	if (text) {
		len = strlen(dtsgui->title)+strlen(text)+4;
		newtitle=(char*)malloc(len);
		snprintf(newtitle, len, "%s [%s]", dtsgui->title, text);
	} else {
		newtitle = (char*)dtsgui->title;
	}
	f->SetTitle(newtitle);
	if (text) {
		free(newtitle);
	}
}

void dtsgui_menuitemenable(dtsgui_menuitem dmi, int enable) {
	wxMenuItem *mi = (wxMenuItem*)dmi;
	mi->Enable((enable) ? true : false);
}

void dtsgui_menuenable(dtsgui_menu dm, int enable) {
	bool state =  (enable) ? true : false;
	wxMenuItemList items;
	wxMenuItem *mi;
	int cnt,i;

	wxMenu *m = (wxMenu*)dm;

	items = m->GetMenuItems();
	cnt = m->GetMenuItemCount();

	for(i=0; i < cnt;i++) {
		mi = items[i];
		mi->Enable(state);
	}
}

void dtsgui_setwindow(struct dtsgui *dtsgui, dtsgui_pane p) {
	DTSFrame *f = (DTSFrame *)dtsgui->appframe;
	f->SetWindow(NULL);
}

void dtsgui_reconfig(struct dtsgui *dtsgui) {
	DTSFrame *f = (DTSFrame *)dtsgui->appframe;
	f->SendDTSEvent(1, NULL);
}

void dtsgui_settitle(dtsgui_pane pane, const char *title) {
	DTSPanel *p = (DTSPanel*)pane;
	return p->SetTitle(title);
}

void dtsgui_setstatus(dtsgui_pane pane, const char *status) {
	DTSPanel *p = (DTSPanel*)pane;
	return p->SetStatus(status);
}

dtsgui_treenode dtsgui_treecont(dtsgui_treeview tree, dtsgui_treenode node, const char *title, int can_edit, int can_sort, int can_del, int nodeid, dtsgui_treeviewpanel_cb p_cb, void *data) {
	DTSTreeWindow *tw = (DTSTreeWindow*)tree;
	DTSDVMCtrl *tc = tw->GetTreeCtrl();
	wxDataViewItem root = wxDataViewItem(node);

	return tc->AppendContainer(root, title, can_edit, can_sort, can_del, nodeid, p_cb, data).GetID();
}

dtsgui_treenode dtsgui_treeitem(dtsgui_treeview tree, dtsgui_treenode node, const char *title, int can_edit, int can_sort, int can_del, int nodeid, dtsgui_treeviewpanel_cb p_cb, void *data) {
	DTSTreeWindow *tw = (DTSTreeWindow*)tree;
	DTSDVMCtrl *tc = tw->GetTreeCtrl();
	wxDataViewItem root = wxDataViewItem(node);

	return tc->AppendItem(root, title, can_edit, can_sort, can_del, nodeid, p_cb, data).GetID();
}

struct xml_node *dtsgui_panetoxml(dtsgui_pane p, const char *xpath, const char *node, const char *nodeval, const char *attrkey) {
	struct xml_node *xn;
	struct xml_doc *xmldoc;
	const char *val, *name, *aval = NULL;
	struct form_item *fi;
	struct bucket_list *il;
	struct bucket_loop *bl;

	if (!(xmldoc = dtsgui_panelxml(p))) {
		return NULL;
	}

	if (nodeval) {
		val = dtsgui_findvalue(p , nodeval);
	} else {
		val = NULL;
	}

	if (attrkey) {
		aval = dtsgui_findvalue(p , attrkey);
	}

	xml_createpath(xmldoc, xpath);
	xn = xml_addnode(xmldoc, xpath, node, (val) ? val : "", attrkey, aval);

	if (val) {
		free((void*)val);
	}
	if (aval) {
		free((void*)aval);
	}

	if (!xn) {
		return NULL;
	}

	il = dtsgui_panel_items(p);
	bl = init_bucket_loop(il);
	while(il && bl && (fi = (struct form_item *)next_bucket_loop(bl))) {
		if (!(name = dtsgui_item_name(fi))) {
			objunref(fi);
			continue;
		}
		if (!(val = dtsgui_item_value(fi))) {
			objunref(fi);
			continue;
		}

		if ((!nodeval || strcmp(name, nodeval)) && (!attrkey || strcmp(name, attrkey))) {
			xml_setattr(xmldoc, xn, name, val);
		}
		free((void*)val);
		objunref(fi);
	}
	stop_bucket_loop(bl);
	objunref(il);
	objunref(xmldoc);

	return xn;
}

int dtsgui_treenodeid(dtsgui_treenode tn) {
	DTSDVMListStore *ls = (DTSDVMListStore*)tn;

	return ls->GetNodeID();
}

void dtsgui_treenodesetxml(dtsgui_treenode tn,struct xml_node *xn, const char *tattr) {
	DTSDVMListStore *ls = (DTSDVMListStore*)tn;

	return ls->SetXMLData(xn, tattr);
}

struct xml_node *dtsgui_treenodegetxml(dtsgui_treenode tn, char **buf) {
	DTSDVMListStore *ls = (DTSDVMListStore*)tn;

	return ls->GetXMLData(buf);
}

void *dtsgui_treenodegetdata(dtsgui_treenode tn) {
	DTSDVMListStore *ls = (DTSDVMListStore*)tn;

	return ls->GetUserData();
}

const char *dtsgui_treenodeparent(dtsgui_treenode tn) {
	wxDataViewItem item = wxDataViewItem(tn);
	DTSDVMListStore *entry, *parent;
	const char *val;
	bool nok;

	nok = item.IsOk();
	entry = (DTSDVMListStore*)item.GetID();

	if (!nok || !entry) {
		return NULL;
	}

	if (!(parent = entry->GetParent())) {
		return NULL;
	}
	val = strdup(parent->GetTitle().ToUTF8());
	return val;
}

static int dtsgui_handle_newtreenode(dtsgui_pane p, int type, int event, void *data) {
	struct tree_newnode *nn = (struct tree_newnode*)data;
	DTSTreeWindow *tw = (DTSTreeWindow*)nn->tv;
	DTSDVMCtrl *tree = tw->GetTreeCtrl();
	wxDataViewItem parent;
	wxDataViewItem item;
	struct xml_node *xn;
	const char *name;
	dtsgui_treenode tn;

	switch(event) {
		case wx_PANEL_BUTTON_YES:
			break;
		default:
			return 1;
	}

	if (!nn || !(xn = dtsgui_panetoxml(p, nn->xpath, nn->node, nn->vitem, nn->tattr))) {
		return 1;
	}

	if (nn->tattr) {
		name = xml_getattr(xn, nn->tattr);
	} else {
		name = xn->value;
	}
	if (nn->flags & DTS_TREE_NEW_NODE_CONTAINER) {
		tn = dtsgui_treecont(nn->tv, nn->tn, name, nn->flags & DTS_TREE_NEW_NODE_EDIT, nn->flags & DTS_TREE_NEW_NODE_SORT, nn->flags & DTS_TREE_NEW_NODE_DELETE, nn->type, nn->p_cb, nn->data);
	} else {
		tn = dtsgui_treeitem(nn->tv, nn->tn, name, nn->flags & DTS_TREE_NEW_NODE_EDIT, nn->flags & DTS_TREE_NEW_NODE_SORT, nn->flags & DTS_TREE_NEW_NODE_DELETE, nn->type, nn->p_cb, nn->data);
	}
	dtsgui_treenodesetxml(tn, xn, nn->tattr);

	parent = wxDataViewItem(nn->tn);
	if (!tree->IsExpanded(parent)) {
		tree->Expand(parent);
	}

	if (nn->node_cb) {
		nn->node_cb(nn->tv, tn, xn, nn->data);
	}
	item = wxDataViewItem(tn);
	tw->Select(item);
	return 0;
}

static void free_tree_newnode(void *data) {
	struct tree_newnode *nn = (struct tree_newnode*)data;

	if (nn->xpath) {
		free((void*)nn->xpath);
	}
	if (nn->node) {
		free((void*)nn->node);
	}
	if (nn->vitem) {
		free((void*)nn->vitem);
	}
	if (nn->tattr) {
		free((void*)nn->tattr);
	}
	if (nn->data) {
		objunref(nn->data);
	}
}

void dtsgui_newxmltreenode(dtsgui_treeview tree, dtsgui_pane p, dtsgui_treenode tn, const char *xpath, const char *node, const char *vitem, const char *tattr,
								int nid, int flags, dtsgui_xmltreenode_cb node_cb, void *data, dtsgui_treeviewpanel_cb p_cb) {
	struct tree_newnode *nn;

	if (!(nn = (struct tree_newnode*)objalloc(sizeof(*nn), free_tree_newnode))) {
		return;
	}

	if (data && objref(data)) {
		nn->data = data;
	}
	nn->tv = tree;
	nn->tn = tn;
	ALLOC_CONST(nn->xpath, xpath);
	ALLOC_CONST(nn->node, node);
	ALLOC_CONST(nn->vitem, vitem);
	ALLOC_CONST(nn->tattr, tattr);
	nn->flags = flags;
	nn->type = nid;
	nn->node_cb = node_cb;
	nn->p_cb = p_cb;

	dtsgui_setevcallback(p, dtsgui_handle_newtreenode, nn);
	objunref(nn);
}

void dtsgui_nodesetxml(dtsgui_treeview tree, dtsgui_treenode node, const char *title) {
	DTSDVMListStore *ls = (DTSDVMListStore*)node;
	struct xml_doc *xmldoc;
	struct xml_node *xn;
	char *buff = NULL;

	if (!(xmldoc = dtsgui_panelxml(tree))) {
		return;
	}

	if (!(xn = ls->GetXMLData(&buff))) {
		objunref(xmldoc);
		return;
	}

	if (buff) {
		xml_setattr(xmldoc, xn, buff, title);
		objunref(buff);
	} else {
		xml_modify(xmldoc, xn, title);
	}
	objunref(xn);
	objunref(xmldoc);
}

extern int dtsgui_handle_newxmltabpane(dtsgui_pane p, int type, int event, void *data) {
	struct tab_newpane *tn = (struct tab_newpane*)data;
	struct xml_node *xn;
	const char *name;

	switch(event) {
		case wx_PANEL_BUTTON_YES:
			break;
		default:
			return 1;
	}

	if (!tn || !(xn = dtsgui_panetoxml(p, tn->xpath, tn->node, tn->vitem, tn->tattr))) {
		return 1;
	}

	if (tn->tattr) {
		name = xml_getattr(xn, tn->tattr);
	} else {
		name = xn->value;
	}

	if (name) {
		dtsgui_setstatus(p, name);
	}

	dtsgui_newtabpage(tn->tabv, name, wx_PANEL_BUTTON_ACTION, tn->data, tn->xmldoc, tn->cb, tn->cdata);
	return 0;
}

void free_newtpane(void *data) {
	struct tab_newpane *tn = (struct tab_newpane*)data;

	if (tn->data) {
		objunref(tn->data);
	}

	if (tn->cdata) {
		objunref(tn->cdata);
	}

	if (tn->xmldoc) {
		objunref(tn->xmldoc);
	}

	if (tn->xpath) {
		free((void*)tn->xpath);
	}
	if (tn->node) {
		free((void*)tn->node);
	}
	if (tn->vitem) {
		free((void*)tn->vitem);
	}
	if (tn->tattr) {
		free((void*)tn->tattr);
	}
}

void dtsgui_newxmltabpane(dtsgui_tabview tabv, dtsgui_pane p, const char *xpath, const char *node, const char *vitem, const char *tattr, event_callback evcb, dtsgui_tabpanel_cb cb, void *cdata, struct xml_doc *xmldoc, void *data) {
	struct tab_newpane *tn;
	DTSTabWindow *nb = (DTSTabWindow*)tabv;

	if (!(tn = (struct tab_newpane*)objalloc(sizeof(*tn), free_newtpane))) {
		return;
	}

	if (data && objref(data)) {
		tn->data = data;
	}

	if (cdata && objref(cdata)) {
		tn->cdata = cdata;
	}

	tn->tabv = tabv;
	if (xmldoc && objref(xmldoc)) {
		tn->xmldoc = xmldoc;
	}

	ALLOC_CONST(tn->xpath, xpath);
	ALLOC_CONST(tn->node, node);
	ALLOC_CONST(tn->vitem, vitem);
	ALLOC_CONST(tn->tattr, tattr);

	tn->last = nb->GetPageCount() -1;

	tn->cb = cb;
	dtsgui_setevcallback(p, evcb, tn);
	objunref(tn);
}

void dtsgui_set_toolbar(struct dtsgui *dtsgui, int show) {
	DTSFrame *f = (DTSFrame *)dtsgui->appframe;
	wxToolBar *tb = f->GetToolBar();

	tb->Show((show) ? true : false);
}

void dtsgui_setuptoolbar(struct dtsgui *dtsgui, dtsgui_toolbar_create cb) {
	DTSFrame *f = (DTSFrame*)dtsgui->appframe;

	f->SetupToolbar(cb);
}

#ifdef __WIN32
void getwin32folder(int csidl, char *path) {
	SHGetFolderPathA(NULL, csidl, NULL, 0, path);
}
#endif
