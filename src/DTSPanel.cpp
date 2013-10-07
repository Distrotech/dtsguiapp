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

#include <stdint.h>
#include <stdlib.h>

#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <wx/wizard.h>

#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/notebook.h>

#include <dtsapp.h>
#include "dtsgui.h"
#include "dtsgui.hpp"

#include "pitems.h"
#include "DTSFrame.h"
#include "DTSPanel.h"

#define PADING	10

static const int def_buttons[6] = {wxID_FIRST, wxID_BACKWARD, wxID_FORWARD, wxID_LAST, wxID_APPLY, wxID_UNDO};

static int fitems_hash(const void *data, int key) {
	int ret = 0;

	class form_item *fi = (class form_item *)data;
	const char *hashkey = (key) ? (const char *)data : fi->GetName();

	if (hashkey) {
		ret = jenhash(hashkey, strlen(hashkey), 0);
	} else {
		ret = jenhash(fi, sizeof(*fi), 0);
	}

	return(ret);
}

DTSPanelEvent::DTSPanelEvent(DTSObject *win) {
	refobj = (void**)objalloc(sizeof(void*), NULL);
	*refobj = this;
	evcb = NULL;
	data = NULL;
	parent = win;
}

DTSPanelEvent::~DTSPanelEvent() {
	objlock(refobj);
	if (data) {
		objunref(data);
		data = NULL;
	}
	objunlock(refobj);
	objunref(refobj);
}

void DTSPanelEvent::SetCallback(event_callback ev_cb, void *userdata) {
	objlock(refobj);
	if (data) {
		objunref(data);
		data = NULL;
	}

	if (userdata && objref(userdata)) {
		data = userdata;
	}
	evcb = ev_cb;
	objunlock(refobj);
}

int DTSPanelEvent::RunCallBack(int etype, int eid, void *cb_data) {
	void *cbdata = NULL;
	DTSFrame *f;
	class dtsgui *dtsgui = NULL;
	int res;

	if (cb_data && objref(cb_data)) {
		cbdata = cb_data;
	}

	/*get app data*/
	if ((f = parent->GetFrame())) {
		dtsgui = f->GetDTSData();
	}

	/*pass fresh ref to callback*/
	res = evcb(dtsgui, (void *)parent, etype, eid, cbdata);

	if (dtsgui) {
		objunref(dtsgui);
	}
	if (cbdata) {
		objunref(cbdata);
	}
	return res;
}

void DTSPanelEvent::OnButton(wxCommandEvent &event) {
	int eid, i;

	if (!parent) {
		event.Skip(true);
		return;
	}

	eid=event.GetId();

	/*convert wxbutton info to own id's and pass this on except for dialog*/
	for(i = 0; i < 6; i++) {
		if (parent->buttons[i] == eid) {
			eid = 1 << i;
			break;
		}
	}
	if (parent->type != wx_DTSPANEL_DIALOG) {
		event.SetId(eid);
	}

	/*maybe oneday there will be a default*/
	parent->EventHandler(eid, &event);

	if (evcb) {
		if (RunCallBack(wx_PANEL_EVENT_BUTTON, eid, data)) {
			event.Skip(true);
		}
	} else {
		event.Skip(true);
	}
}

void DTSPanelEvent::OnDialog(wxCommandEvent &event) {
	/*translate enter into OK/Apply*/
	event.SetId(parent->buttons[4]);
	event.SetEventType(wxEVT_COMMAND_BUTTON_CLICKED);
	OnButton(event);
}

void DTSPanelEvent::OnCombo(wxCommandEvent &event) {
	wxComboBox *cb;
	struct bucket_list *bl;
	struct bucket_loop *bloop;
	class form_item *fi;
	int eid, etype, dtype;

	if (!parent) {
		event.Skip(true);
		return;
	}

	bl = parent->GetItems();
	cb = (wxComboBox *)event.GetEventObject();
	etype = event.GetEventType();

	if (etype == wxEVT_COMMAND_TEXT_ENTER) {
		dtype = wx_PANEL_EVENT_COMBO_ENTER;
	} else if (etype == wxEVT_COMMAND_TEXT_UPDATED) {
		dtype = wx_PANEL_EVENT_COMBO_UPDATE;
	} else {
		dtype = 0;
	}
	bloop = init_bucket_loop(bl);

	while(bl && bloop && (fi = (class form_item *)next_bucket_loop(bloop))) {
		if (fi->GetWidget() == cb) {
			break;
		}

		objunref(fi);
		fi = NULL;
	}
	stop_bucket_loop(bloop);
	objunref(bl);

	if (fi) {
		eid=event.GetId();

		if (dtype && evcb) {
			if (RunCallBack(dtype, eid, fi)) {
				event.Skip(true);
			}
		}

		if (etype == wxEVT_COMMAND_TEXT_ENTER) {
			cb->Popup();
		}

		objunref(fi);
	} else {
		event.Skip(true);
	}
}

void DTSPanelEvent::OnDTSEvent(wxCommandEvent &event) {
	int  eid;

	if (evcb) {
		eid=event.GetId();
		RunCallBack(wx_PANEL_EVENT_DTS, eid, data);
	}
	event.Skip(true);
}

void DTSPanelEvent::BindDTSEvent(DTSFrame *frame) {
	frame->Bind(DTS_APP_EVENT, &DTSPanelEvent::OnDTSEvent, this);
}

void DTSPanelEvent::BindButton(wxWindow *win, int button) {
	win->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DTSPanelEvent::OnButton, this, button, button, NULL);
}

void DTSPanelEvent::BindCombo(wxWindow *win, int w_id) {
	win->Bind(wxEVT_COMMAND_TEXT_ENTER, &DTSPanelEvent::OnCombo, this, w_id, w_id, NULL);
	win->Bind(wxEVT_COMMAND_TEXT_UPDATED, &DTSPanelEvent::OnCombo, this, w_id, w_id, NULL);
}

DTSObject::DTSObject(wxString st) {
	refobj = (void**)objalloc(sizeof(void*), NULL);
	*refobj = this;
	status = st;
	panel = NULL;
	frame = NULL;
	xmldoc = NULL;
	userdata = NULL;
	dtsevthandler = NULL;
	beenshown = false;
	SetName(status);
}

DTSObject::~DTSObject() {
	objlock(refobj);
	if (userdata) {
		objunref(userdata);
		userdata = NULL;
	}
	if (xmldoc) {
		objunref(xmldoc);
		xmldoc = NULL;
	}
	if (dtsevthandler) {
		delete dtsevthandler;
		dtsevthandler = NULL;
	}
	objunlock(refobj);
	objunref(refobj);
}

wxString DTSObject::GetName() {
	wxString st;

	objlock(refobj);
	st = status;
	objunlock(refobj);

	return st;
}

wxWindow *DTSObject::GetPanel() {
	return panel;
}

DTSFrame *DTSObject::GetFrame() {
	return frame;
}

void DTSObject::SetXMLDoc(struct xml_doc *xd) {
	objlock(refobj);

	if (xmldoc) {
		objunref(xmldoc);
		xmldoc = NULL;
	}

	if (xd && objref(xd)) {
		xmldoc = xd;
	}

	objunlock(refobj);
}

struct xml_doc *DTSObject::GetXMLDoc(void) {
	struct xml_doc *xd = NULL;

	objlock(refobj);
	if (xmldoc && objref(xmldoc)) {
		xd = xmldoc;
	}
	objunlock(refobj);
	return xd;
}

struct bucket_list *DTSObject::GetItems(void) {
	if (fitems && objref(fitems)) {
		return fitems;
	}
	return NULL;
}

void DTSObject::EventHandler(int eid, wxCommandEvent *event) {
}

void DTSObject::SetUserData(void *data) {
	objlock(refobj);
	if (userdata) {
		objunref(userdata);
		userdata = NULL;
	}
	if (data && objref(data)) {
		userdata = data;
	}
	objunlock(refobj);
}

void *DTSObject::GetUserData(void) {
	void *ud = NULL;
	objlock(refobj);
	if (userdata && objref(userdata)) {
		ud = userdata;
	}
	objunlock(refobj);
	return ud;
}

DTSPanel::DTSPanel(DTSFrame *mainwin, wxString statusmsg, int butmask)
	:DTSObject(statusmsg) {
	DTSPanelEvent *dtsevt;

	button_mask = butmask;
	config_data = NULL;
	configcb = NULL;
	dtsevthandler = NULL;
	title = NULL;
	fgs = NULL;
	frame = mainwin;
	memcpy(buttons, def_buttons, sizeof(def_buttons));;
	fitems = (struct bucket_list *)create_bucketlist(0, fitems_hash);
	dtsevt = new DTSPanelEvent(this);
	dtsevthandler = dtsevt;

	if (dtsevt && frame) {
		dtsevt->BindDTSEvent(frame);
	}
}

DTSPanel::~DTSPanel() {
	objlock(refobj);
	if (fitems) {
		objunref(fitems);
		fitems = NULL;
	}
	if (config_data) {
		objunref(config_data);
		config_data = NULL;
	}
	objunlock(refobj);
}

void DTSPanel::SetSizerSize(wxSize minsize, wxWindow *parent) {
	if (parent) {
		fgs->FitInside(parent);
	}

	fgs->Layout();
}

void DTSPanel::SetupWin(void) {
	wxWindow *w = static_cast<wxWindow *>(this);

	if (panel && (type != wx_DTSPANEL_TREE)) {
		fgs = new wxGridBagSizer(9, 25);
		panel->SetSizer(fgs);
	}

	beenshown = false;
	g_row = 0;
	objlock(refobj);
	w->SetName(status);
	objunlock(refobj);
	w->Show(false);
}

void DTSPanel::SetEventCallback(event_callback evcb, void *userdata) {
	DTSPanelEvent *dtsevt = (DTSPanelEvent*)dtsevthandler;

	if (dtsevthandler) {
		dtsevt->SetCallback(evcb, userdata);
	}
}

void DTSPanel::SetConfigCallback(dtsgui_configcb cb, void *userdata) {
	objlock(refobj);

	if (config_data) {
		objunref(config_data);
		config_data = NULL;
	}
	configcb = cb;
	if (userdata && objref(userdata)) {
		config_data = userdata;
	}

	objunlock(refobj);
}

bool DTSPanel::ShowPanel(bool show) {
	if (show) {
		if (frame && (type != wx_DTSPANEL_TAB) && (type != wx_DTSPANEL_TREE)) {
			frame->SetStatusText(status);
		}

		if (!beenshown && panel && fgs) {
			Buttons();

			if (fgs->GetCols() >= 3) {
				fgs->AddGrowableCol(3, 1);
				fgs->AddGrowableCol(1, 1);
			}
			beenshown = true;
		}
	}

	return show;
}

void DTSPanel::AddItem(wxWindow *item, const wxGBPosition pos, const wxGBSpan span, int flags, int border, int growrow) {
	fgs->Add(item, pos, span, flags , border);

	if (growrow >= 0) {
		fgs->AddGrowableRow(g_row, growrow);
	}
}

void DTSPanel::Title(const char *title) {
	wxFont font;
	wxStaticText *tit;

	if (this->title) {
		return;
	}

	tit = new wxStaticText(panel, -1, title);
	this->title = tit;


	font = tit->GetFont();
	font.SetPointSize(font.GetPointSize()+2);
	font.SetWeight(wxFONTWEIGHT_BOLD);
	tit->SetFont(font);
	AddItem(tit, wxGBPosition(g_row, 0), wxGBSpan(1, 6), wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, PADING);


	g_row++;
}

void DTSPanel::SetTitle(const wxString new_title, bool create) {
	if (title) {
		title->SetLabel(new_title);
	} else if (create) {
		Title(new_title);
	}
}

void DTSPanel::SetStatus(const wxString new_status) {
	objlock(refobj);
	status = new_status;
	objunlock(refobj);
}

struct xml_element *DTSPanel::GetNode(const char *ppath, const char *node, const char *fattr, const char *fval, const char *attr) {
	class xml_element *xml = NULL;
	struct xml_doc *xd;
	struct xml_node *xn;
	const char *xpath;
	struct xml_search *xs;
	int len;


	if (!(xd = GetXMLDoc()) || !ppath) {
		return NULL;
	}

	if (node) {
		len = strlen(ppath) + strlen(node) + 2;
		if (fval) {
			if (fattr) {
				len+= strlen(fattr) + strlen(fval)+8;
				xpath = (const char*)malloc(len);
				snprintf((char*)xpath, len, "%s/%s[@%s = '%s']", ppath, node, fattr, fval);
			} else {
				len+= strlen(fval)+8;
				xpath = (const char*)malloc(len);
				snprintf((char*)xpath, len, "%s/%s[. = '%s']", ppath, node, fval);
			}
		} else {
			xpath = (const char*)malloc(len);
			snprintf((char*)xpath, len, "%s/%s", ppath, node);
		}
	} else {
		len = strlen(ppath) + 1;
		if (fval) {
			if (fattr) {
				len+= strlen(fattr) + strlen(fval)+8;
				xpath = (const char*)malloc(len);
				snprintf((char*)xpath, len, "%s[@%s = '%s']", ppath, fattr, fval);
			} else {
				len+= strlen(fval)+8;
				xpath = (const char*)malloc(len);
				snprintf((char*)xpath, len, "%s[. = '%s']", ppath, fval);
			}
		} else {
			ALLOC_CONST(xpath, ppath);
		}
	}

	if (!(xs = xml_xpath(xd, xpath, attr))) {
		if (ppath && node && fval) {
			xml_createpath(xd, ppath);
			if ((xn = xml_addnode(xd, ppath, node, (fattr) ? "" : fval, fattr, (fattr) ? fval : NULL))) {
				xs = xml_xpath(xd, xpath, attr);
				objunref(xn);
			}
		}
	}

	if (!xs) {
		free((void*)xpath);
		return NULL;
	}

	if (!(xml = new xml_element(xpath, xs, attr))) {
		free((void*)xpath);
		objunref(xs);
	}
	objunref(xd);

	return xml;
}

void DTSPanel::TextBox(const char *title, const char *name, wxString defval, int flags, int rows, void *data,enum form_data_type dtype) {
	wxStaticText *text = new wxStaticText(panel, -1, title);
	wxTextCtrl *tbox = new wxTextCtrl(panel, -1, defval, wxPoint(-1, -1), wxSize(-1, -1), flags);
	class form_item *fi;

	AddItem(text, wxGBPosition(g_row, 0), wxGBSpan(rows, 3), wxLEFT | wxRIGHT, PADING);
	AddItem(tbox, wxGBPosition(g_row, 3), wxGBSpan(rows,3), wxEXPAND | wxGROW | wxLEFT | wxRIGHT, PADING,	(rows > 1) ? 1 : -1);
	g_row += rows;

	if ((dtype == DTSGUI_FORM_DATA_XML) && !data) {
		tbox->Disable();
	}

	fi = new form_item(tbox, DTS_WIDGET_TEXTBOX, name, NULL, NULL, data, dtype);
	addtobucket(fitems, fi);
	objunref(fi);
}

void DTSPanel::PasswdBox(const char *title, const char *name, wxString defval, int flags, void *data, enum form_data_type) {
	TextBox(title, name, defval, flags | wxTE_PASSWORD | wxTE_PROCESS_ENTER, 1, data);
}

void DTSPanel::CheckBox(const char *title, const char *name, int ischecked, const char *checkval, const char *uncheckval, void *data, enum form_data_type dtype) {
	wxStaticText *text = new wxStaticText(panel, -1, title);
	wxCheckBox *cbox = new wxCheckBox(panel, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	class form_item *fi;

	AddItem(text, wxGBPosition(g_row, 0), wxGBSpan(1, 3), wxLEFT | wxRIGHT, PADING);
	AddItem(cbox, wxGBPosition(g_row, 3), wxGBSpan(1, 3), wxLEFT | wxRIGHT, PADING);
	g_row++;

	if ((dtype == DTSGUI_FORM_DATA_XML) && !data) {
		cbox->Disable();
	}

	cbox->SetValue((ischecked) ? true : false);

	fi = new form_item(cbox, DTS_WIDGET_CHECKBOX, name, checkval, uncheckval, data, dtype);
	addtobucket(fitems, fi);
	objunref(fi);
}

class form_item *DTSPanel::ListBox(const char *title, const char *name, const char *value, void *data, enum form_data_type dtype) {
	class form_item *fi;

	wxStaticText *text = new wxStaticText(panel, -1, title);
	wxChoice *lbox = new wxComboBox(panel, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);

	AddItem(text, wxGBPosition(g_row, 0), wxGBSpan(1, 3), wxLEFT | wxRIGHT, PADING);
	AddItem(lbox, wxGBPosition(g_row, 3), wxGBSpan(1, 3), wxEXPAND | wxGROW | wxLEFT | wxRIGHT, PADING);
	g_row++;

	if ((dtype == DTSGUI_FORM_DATA_XML) && !data) {
		lbox->Disable();
	}

	fi = new form_item(lbox, DTS_WIDGET_LISTBOX, name, value, NULL, data, dtype);
	addtobucket(fitems, fi);
	return fi;
}

class form_item *DTSPanel::ComboBox(const char *title, const char *name, const char *value, void *data, enum form_data_type dtype) {
	class form_item *fi;
	DTSPanelEvent *dtsevt = (DTSPanelEvent*)dtsevthandler;
	wxStaticText *text = new wxStaticText(panel, -1, title);
	wxChoice *lbox = new wxComboBox(panel, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxTE_PROCESS_ENTER);

	AddItem(text, wxGBPosition(g_row, 0), wxGBSpan(1, 3), wxLEFT | wxRIGHT, PADING);
	AddItem(lbox, wxGBPosition(g_row, 3), wxGBSpan(1, 3), wxEXPAND | wxGROW | wxLEFT | wxRIGHT, PADING);
	g_row++;

	if (dtsevthandler) {
		dtsevt->BindCombo(panel, lbox->GetId());
	}

	if ((dtype == DTSGUI_FORM_DATA_XML) && !data) {
		lbox->Disable();
	}

	fi = new form_item(lbox, DTS_WIDGET_COMBOBOX, name, value, NULL, data, dtype);
	addtobucket(fitems, fi);
	return fi;
}

void DTSPanel::Buttons(void) {
	DTSPanelEvent *dtsevt = (DTSPanelEvent*)dtsevthandler;
	int i, b;

	if (!button_mask || !dtsevthandler) {
		return;
	}

	for(i = 0; i < 6; i++) {
		b = buttons[i];

		if (button_mask & (1 << i)) {
			wxButton *button = new wxButton(panel, b);
			dtsevt->BindButton(panel, b);
			AddItem(button, wxGBPosition(g_row, i), wxDefaultSpan, wxALIGN_BOTTOM | wxALL, PADING);
		}
	}

	fgs->AddGrowableRow(g_row, 0);
	g_row++;
}

void DTSPanel::Update_XML() {
	struct bucket_loop *bloop;
	class form_item *fi;
	class xml_element *xml;
	const char *value;

	if (!xmldoc) {
		return;
	}

	objref(fitems);
	bloop = init_bucket_loop(fitems);
	while (bloop && (fi = (class form_item*)next_bucket_loop(bloop))) {
		if (!(xml = fi->GetXMLData())) {
			objunref(fi);
			continue;
		}

		value = fi->GetValue();
		xml->Modify(xmldoc, value);
		objunref(xml);
		if (value) {
			free((void*)value);
		}

		objunref(fi);
	}
	stop_bucket_loop(bloop);
	objunref(fitems);
}

struct xml_node *DTSPanel::Panel2XML(const char *xpath, const char *node, const char *nodeval, const char *attrkey) {
	struct xml_node *xn;
	const char *val = NULL, *name, *aval = NULL;
	class form_item *fi;
	struct bucket_loop *bl;

	if (!xmldoc || !objref(xmldoc)) {
		return NULL;
	}

	objref(fitems);
	if (nodeval && (fi = (class form_item*)bucket_list_find_key(fitems, (void*)nodeval))) {
		val = fi->GetValue();
		objunref(fi);
		fi = NULL;
	}

	if (attrkey && (fi = (class form_item*)bucket_list_find_key(fitems, (void*)attrkey))) {
		aval = fi->GetValue();
		objunref(fi);
		fi = NULL;
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
		objunref(xmldoc);
		objunref(fitems);
		return NULL;
	}

	bl = init_bucket_loop(fitems);
	while(fitems && bl && (fi = (class form_item *)next_bucket_loop(bl))) {
		if (!(name = fi->GetName())) {
			objunref(fi);
			continue;
		}
		if (!(val = fi->GetValue())) {
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
	objunref(fitems);
	objunref(xmldoc);

	return xn;
}

struct curl_post *DTSPanel::Panel2Post() {
	struct bucket_loop *bloop;
	struct curl_post *post;
	struct form_item *fi;
	const char *name;
	const char *val;

	if (!objref(fitems)) {
		return NULL;
	}
	if (!(bloop = init_bucket_loop(fitems))) {
		objunlock(fitems);
		return NULL;
	}

	if (!(post = curl_newpost())) {
		stop_bucket_loop(bloop);
		objunlock(fitems);
		return NULL;
	}

	while((fi = (struct form_item*)next_bucket_loop(bloop))) {
		if (!(name = fi->GetName())) {
			objunref(fi);
			continue;
		}
		val = fi->GetValue();
		if (val) {
			curl_postitem(post, name, val);
			free((void*)val);
		} else {
			curl_postitem(post, name, "");
		}
		objunref(fi);
	}
	stop_bucket_loop(bloop);
	objunref(fitems);

	return post;
}

DTSScrollPanel::DTSScrollPanel(wxWindow *parent,DTSFrame *frame, wxString status, int butmask)
	:wxScrolledWindow(parent, wxID_ANY),
	 DTSPanel(frame, status, butmask) {
	DTSPanelEvent *dtsevt = (DTSPanelEvent*)dtsevthandler;

	type = wx_DTSPANEL_SCROLLPANEL;
	SetScrollRate(10, 10);
	panel = dynamic_cast<wxPanel *>(this);
	panel->Bind(wxEVT_TEXT_ENTER, &DTSPanelEvent::OnDialog, dtsevt);
	SetupWin();
}

bool DTSScrollPanel::Show(bool show) {
	ShowPanel(show);
	return wxScrolledWindow::Show(show);
}

DTSTabPage::DTSTabPage(wxBookCtrlBase *parent, DTSFrame *frame, wxString status, bool addpage, int butmask, dtsgui_tabpanel_cb c_cb, void *c_data, struct xml_doc *xmldoc)
	:wxScrolledWindow(parent, wxID_ANY),
	DTSScrollPanel(parent,frame, status, butmask) {
	type = wx_DTSPANEL_TAB;
	cb = c_cb;
	if (c_data && objref(c_data)) {
		cdata = c_data;
	} else {
		cdata = NULL;
	}
	if (xmldoc) {
		SetXMLDoc(xmldoc);
	}
	hasconfig = false;
	SetTitle(status, 1);
	if (addpage) {
		parent->AddPage(panel, status);
	}
}

DTSTabPage::~DTSTabPage() {
	objlock(refobj);
	if (cdata) {
		objunref(cdata);
	}
	objunlock(refobj);
}

bool DTSTabPage::Show(bool show) {
	return wxScrolledWindow::Show(show);
}

void DTSTabPage::ConfigPane() {
	if (!hasconfig) {
		if (cb) {
			void *dat = NULL;
			if (cdata && objref(cdata)) {
				dat = cdata;
			}
			cb(this, dat);
			if (dat) {
				objunref(dat);
			}
		}
		hasconfig = true;
		ShowPanel();
	}
}

void DTSTabPage::InsertPage(int pos) {
	wxBookCtrlBase *nb = dynamic_cast<wxBookCtrlBase*>(panel->GetParent());

	objlock(refobj);
	nb->InsertPage(pos, panel, status);
	objunlock(refobj);
	panel->Show();
	nb->SetSelection(pos);
}

DTSTabPage &DTSTabPage::operator=(const DTSTabPage &orig) {
	DTSPanelEvent *dtsevt = (DTSPanelEvent*)dtsevthandler;

	if (this == &orig) {
		return *this;
	}

	/*deadlock avoidance*/
	objlock(orig.refobj);
	while(!objtrylock(this->refobj)) {
		objunlock(orig.refobj);
#ifdef __WIN32
		Sleep(1);
#else
		usleep(1000);
#endif
		objlock(orig.refobj);
	}
	button_mask = orig.button_mask;
	if (cdata) {
		objunref(cdata);
		cdata = NULL;
	}
	if (orig.cdata && objref(orig.cdata)) {
		cdata = orig.cdata;
	}
	cb = orig.cb;

	if (xmldoc) {
		objunref(xmldoc);
		xmldoc = NULL;
	}
	if (orig.xmldoc && objref(orig.xmldoc)) {
		xmldoc = orig.xmldoc;
	} else {
		xmldoc = NULL;
	}
	status = orig.status;
	SetTitle(status);
	panel->SetName(status);

	frame = orig.frame;
	if (dtsevt && frame) {
		dtsevt->BindDTSEvent(frame);
	}
	objunlock(this->refobj);
	objunlock(orig.refobj);
	return *this;
}

DTSStaticPanel::DTSStaticPanel(wxWindow *parent,DTSFrame *frame, wxString status, int butmask)
	:wxPanel(parent, wxID_ANY),
	 DTSPanel(frame, status, butmask) {
	DTSPanelEvent *dtsevt = (DTSPanelEvent*)dtsevthandler;
	type = wx_DTSPANEL_PANEL;
	panel = dynamic_cast<wxPanel *>(this);
	panel->Bind(wxEVT_TEXT_ENTER, &DTSPanelEvent::OnDialog, dtsevt);
	SetupWin();
}

DTSStaticPanel::DTSStaticPanel() {
	type = wx_DTSPANEL_PANEL;
}

bool DTSStaticPanel::Show(bool show) {
	ShowPanel(show);
	return wxPanel::Show(show);
}

DTSWindow::DTSWindow(wxWindow *parent, DTSFrame *frame, wxString status)
	:wxWindow(parent, -1),
	 DTSPanel(frame, status) {
	type = wx_DTSPANEL_WINDOW;
	panel = dynamic_cast<wxWindow *>(this);
	SetupWin();
}

bool DTSWindow::Show(bool show) {
	ShowPanel(show);
	return wxWindow::Show(show);
}

DTSDialog::DTSDialog(DTSFrame *frame, wxString name, int butmask) {
	DTSPanelEvent *dtsevt = (DTSPanelEvent*)dtsevthandler;
	type = wx_DTSPANEL_DIALOG;

	button_mask = butmask;
	buttons[4] = wxID_OK;
	buttons[5] = wxID_CANCEL;

	/*this is deleted by app*/
	dialog = new wxDialog(frame, -1, name);
	sizer = new wxBoxSizer(wxHORIZONTAL);
	dialog->SetSizer(sizer);
	dialog->SetAffirmativeId(buttons[4]);

	panel = dynamic_cast<wxPanel *>(this);
	panel->Create(dialog, -1);
	panel->SetName(name);
	panel->Bind(wxEVT_TEXT_ENTER, &DTSPanelEvent::OnDialog, dtsevt);
	SetupWin();
}

DTSDialog::~DTSDialog() {
}

bool DTSDialog::Show(bool show) {
	ShowPanel(show);
	return wxPanel::Show(show);
}

void DTSDialog::RunDialog(void) {
	Show(true);

	sizer->Add(panel, 1, wxALL | wxEXPAND, 15);
	dialog->Fit();
	dialog->Center();
	dialog->Layout();
	dialog->SetClientSize(sizer->GetSize());
	dialog->ShowModal();
}

DTSWizardWindow::DTSWizardWindow(wxString title) {
	type = wx_DTSPANEL_WIZARD;
	button_mask = 0;
	panel = dynamic_cast<wxPanel *>(this);
	SetupWin();
}

bool DTSWizardWindow::Show(bool show) {
	ShowPanel(show);

	if (show) {
		SetSizerSize(panel->GetSize(), NULL);
	}

	return wxWizardPageSimple::Show(show);
}
