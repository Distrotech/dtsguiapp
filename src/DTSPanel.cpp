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

#include <wx/scrolwin.h>
#include <wx/gbsizer.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/dialog.h>
#include <wx/frame.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/wizard.h>

#include <dtsapp.h>
#include "dtsgui.hpp"

#include "DTSFrame.h"
#include "DTSPanel.h"

#define PADING	10

static const int def_buttons[6] = {wxID_FIRST, wxID_BACKWARD, wxID_FORWARD, wxID_LAST, wxID_APPLY, wxID_REFRESH};

void free_fitem(void *data) {
	struct form_item *fi = (struct form_item *)data;

	if (fi->name) {
		free((void *)fi->name);
	}
	if (fi->value) {
		free((void *)fi->value);
	}
	if (fi->data.ptr) {
		objunref(fi->data.ptr);
	}
}

static int fitems_hash(const void *data, int key) {
	int ret = 0;

	const struct form_item *fi = (const struct form_item *)data;
	const char *hashkey = (key) ? (const char *)data : fi->name;

	if (hashkey) {
		ret = jenhash(hashkey, strlen(hashkey), 0);
	} else {
		ret = jenhash(fi, sizeof(*fi), 0);
	}

	return(ret);
}

DTSPanelEvent::DTSPanelEvent(DTSObject *win) {
	evcb = NULL;
	data = NULL;
	parent = win;
}

void DTSPanelEvent::SetCallback(event_callback ev_cb, void *userdata) {
	data = userdata;
	evcb = ev_cb;
}

void DTSPanelEvent::OnButton(wxCommandEvent &event) {
	int eid, i, etype;

	if (!parent) {
		event.Skip(true);
		return;
	}

	eid=event.GetId();

	for(i = 0; i < 6; i++) {
		if (parent->buttons[i] == eid) {
			eid = 1 << i;
			break;
		}
	}

	if (evcb) {
		etype = event.GetEventType();
		evcb((void *)parent, etype, eid, data);
	}

	parent->EventHandler(eid, &event);
	event.Skip();
}

void DTSPanelEvent::OnDialog(wxCommandEvent &event) {
	event.SetId(wxID_OK);
	event.SetEventType(wxEVT_COMMAND_BUTTON_CLICKED);
	OnButton(event);
}

void DTSPanelEvent::OnCombo(wxCommandEvent &event) {
	wxComboBox *cb;
	struct bucket_list *bl;
	struct bucket_loop *bloop;
	struct form_item *fi;
	int eid, etype;

	if (!parent) {
		event.Skip(true);
		return;
	}

	bl = parent->GetItems();
	cb = (wxComboBox *)event.GetEventObject();
	etype = event.GetEventType();

	bloop = init_bucket_loop(bl);

	while(bl && bloop && (fi = (struct form_item *)next_bucket_loop(bloop))) {
		if (fi->widget == cb) {
			break;
		}

		objunref(fi);
		fi = NULL;
	}

	stop_bucket_loop(bloop);

	if (fi) {
		eid=event.GetId();

		if (evcb) {
			evcb((void *)parent, etype, eid, fi);

			if (etype == wxEVT_COMMAND_TEXT_ENTER) {
				cb->Popup();
			}
		}

		objunref(fi);
	} else {
		event.Skip(true);
	}
}

void DTSPanelEvent::OnDTSEvent(wxCommandEvent &event) {
	int  eid, etype;
	printf("Got DTS EVent\n");

	eid=event.GetId();
	if (evcb) {
		etype = event.GetEventType();
		evcb((void *)parent, etype, eid, data);
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
	status = st;
	panel = NULL;
	frame = NULL;
	SetName(status);
}

wxString DTSObject::GetName() {
	return status;
}

wxWindow *DTSObject::GetPanel() {
	return panel;
}

DTSFrame *DTSObject::GetFrame() {
	return frame;
}

struct bucket_list *DTSObject::GetItems(void) {
	objref(fitems);
	return fitems;
}

void DTSObject::EventHandler(int eid, wxCommandEvent *event) {
}

void DTSObject::SetUserData(void *data) {
	userdata = data;
}

void *DTSObject::GetUserData(void) {
	return userdata;
}

DTSPanel::DTSPanel(DTSFrame *mainwin, wxString statusmsg, int butmask)
	:DTSObject(statusmsg) {
	button_mask = butmask;
	userdata = NULL;
	dtsevthandler = NULL;
	xmldoc = NULL;
	fgs = NULL;
	frame = mainwin;
	memcpy(buttons, def_buttons, sizeof(def_buttons));;
	fitems = (struct bucket_list *)create_bucketlist(0, fitems_hash);
	dtsevthandler = new DTSPanelEvent(this);
	if (dtsevthandler && frame) {
		dtsevthandler->BindDTSEvent(frame);
	}
}

DTSPanel::~DTSPanel() {
	if (dtsevthandler) {
		delete dtsevthandler;
	}

	if (userdata) {
		objunref(userdata);
	}

	objunref(fitems);

	if (xmldoc) {
		objunref(xmldoc);
	}
}

struct form_item *DTSPanel::create_new_fitem(void *widget, enum widget_type type, const char *name, const char *value, const char *value2, void *data, enum form_data_type dtype) {
	struct form_item *fi;

	if (!(fi = (struct form_item *)objalloc(sizeof(*fi),free_fitem))) {
		return NULL;
	}

	fi->widget = widget;
	fi->type = type;
	fi->data.ptr = data;
	fi->dtype = dtype;
	ALLOC_CONST(fi->name, name);
	if (value) {
		ALLOC_CONST(fi->value, value);
	}
	if (value2) {
		ALLOC_CONST(fi->value2, value2);
	}
	addtobucket(fitems, fi);
	return fi;
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
	w->SetName(status);
	w->Show(false);
}

void DTSPanel::SetEventCallback(event_callback evcb, void *userdata) {
	if (dtsevthandler) {
		dtsevthandler->SetCallback(evcb, userdata);
	}
}

void DTSPanel::SetConfigCallback(dtsgui_configcb cb, void *userdata) {
	configcb = cb;
	config_data = userdata;
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

//            panel->SetClientSize(fgs->GetSize());
			beenshown = true;
		}
	}

	if (dtsevthandler) {
		printf("Do Something\n");
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
	wxStaticText *text = new wxStaticText(panel, -1, title);

	font = text->GetFont();
	font.SetPointSize(font.GetPointSize()+2);
	font.SetWeight(wxFONTWEIGHT_BOLD);
	text->SetFont(font);
	AddItem(text, wxGBPosition(g_row, 0), wxGBSpan(1, 6), wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, PADING);

	g_row++;
}

void DTSPanel::SetXMLDoc(struct xml_doc *xd) {
	objref(xd);
	xmldoc = xd;
}

struct xml_doc *DTSPanel::GetXMLDoc(void) {
	objref(xmldoc);
	return xmldoc;
}

void free_xmlelement(void *data) {
	struct xml_element *xml = (struct xml_element*)data;

	if (xml->xsearch) {
		objunref(xml->xsearch);
	}

	if (xml->xpath) {
		free((void*)xml->xpath);
	}
}

struct xml_element *DTSPanel::GetNode(const char *xpath, const char *attr) {
	struct xml_element *xml = NULL;

	if (!xmldoc || !xpath || (!(xml = (struct xml_element*)objalloc(sizeof(*xml),free_xmlelement)))) {
		return NULL;
	}

	if (!(xml->xsearch = xml_xpath(xmldoc, xpath, attr))) {
		objunref(xml);
		return NULL;
	}

	ALLOC_CONST(xml->xpath, xpath);

	if (attr) {
		xml->attr = strdup(attr);
	}

	return xml;
}

void DTSPanel::TextBox(const char *title, const char *name, wxString defval, int flags, int rows, void *data,enum form_data_type dtype) {
	wxStaticText *text = new wxStaticText(panel, -1, title);
	wxTextCtrl *tbox = new wxTextCtrl(panel, -1, defval, wxPoint(-1, -1), wxSize(-1, -1), flags);
	struct form_item *fi;

	AddItem(text, wxGBPosition(g_row, 0), wxGBSpan(rows, 3), wxLEFT | wxRIGHT, PADING);
	AddItem(tbox, wxGBPosition(g_row, 3), wxGBSpan(rows,3), wxEXPAND | wxGROW | wxLEFT | wxRIGHT, PADING,	(rows > 1) ? 1 : -1);
	g_row += rows;

	if ((dtype == DTSGUI_FORM_DATA_XML) && !data) {
		tbox->Disable();
	}

	fi = create_new_fitem(tbox, DTS_WIDGET_TEXTBOX, name, NULL, NULL, data, dtype);
	objunref(fi);
}

void DTSPanel::PasswdBox(const char *title, const char *name, wxString defval, int flags, void *data, enum form_data_type) {
	TextBox(title, name, defval, flags | wxTE_PASSWORD | wxTE_PROCESS_ENTER, 1, data);
}

void DTSPanel::CheckBox(const char *title, const char *name, int ischecked, const char *checkval, const char *uncheckval, void *data, enum form_data_type dtype) {
	wxStaticText *text = new wxStaticText(panel, -1, title);
	wxCheckBox *cbox = new wxCheckBox(panel, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	struct form_item *fi;

	AddItem(text, wxGBPosition(g_row, 0), wxGBSpan(1, 3), wxLEFT | wxRIGHT, PADING);
	AddItem(cbox, wxGBPosition(g_row, 3), wxGBSpan(1, 3), wxLEFT | wxRIGHT, PADING);
	g_row++;

	if ((dtype == DTSGUI_FORM_DATA_XML) && !data) {
		cbox->Disable();
	}

	cbox->SetValue((ischecked) ? true : false);

	fi = create_new_fitem(cbox, DTS_WIDGET_CHECKBOX, name, checkval, uncheckval, data, dtype);
	objunref(fi);
}

struct form_item *DTSPanel::ListBox(const char *title, const char *name, const char *value, void *data, enum form_data_type dtype) {
	wxStaticText *text = new wxStaticText(panel, -1, title);
	wxChoice *lbox = new wxComboBox(panel, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);

	AddItem(text, wxGBPosition(g_row, 0), wxGBSpan(1, 3), wxLEFT | wxRIGHT, PADING);
	AddItem(lbox, wxGBPosition(g_row, 3), wxGBSpan(1, 3), wxEXPAND | wxGROW | wxLEFT | wxRIGHT, PADING);
	g_row++;

	if ((dtype == DTSGUI_FORM_DATA_XML) && !data) {
		lbox->Disable();
	}

	return create_new_fitem(lbox, DTS_WIDGET_LISTBOX, name, value, NULL, data, dtype);
}

struct form_item *DTSPanel::ComboBox(const char *title, const char *name, const char *value, void *data, enum form_data_type dtype) {
	wxStaticText *text = new wxStaticText(panel, -1, title);
	wxChoice *lbox = new wxComboBox(panel, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxTE_PROCESS_ENTER);

	AddItem(text, wxGBPosition(g_row, 0), wxGBSpan(1, 3), wxLEFT | wxRIGHT, PADING);
	AddItem(lbox, wxGBPosition(g_row, 3), wxGBSpan(1, 3), wxEXPAND | wxGROW | wxLEFT | wxRIGHT, PADING);
	g_row++;

	if (dtsevthandler) {
		dtsevthandler->BindCombo(panel, lbox->GetId());
	}

	if ((dtype == DTSGUI_FORM_DATA_XML) && !data) {
		lbox->Disable();
	}

	return create_new_fitem(lbox, DTS_WIDGET_COMBOBOX, name, value, NULL, data, dtype);
}

void DTSPanel::Buttons(void) {
	int i, b;

	if (!button_mask || !dtsevthandler) {
		return;
	}

	for(i = 0; i < 6; i++) {
		b = buttons[i];

		if (button_mask & (1 << i)) {
			wxButton *button = new wxButton(panel, b);
			dtsevthandler->BindButton(panel, b);
			AddItem(button, wxGBPosition(g_row, i), wxDefaultSpan, wxALIGN_BOTTOM | wxALL, PADING);
		}
	}

	fgs->AddGrowableRow(g_row, 0);
	g_row++;
}

void DTSPanel::Update_XML() {
	struct bucket_loop *bloop;
	struct form_item *fi;
	struct xml_element *xml;
	struct xml_node *xn;
	const char *value, *tmp;
	wxTextCtrl *tbox;
	wxCheckBox *cbox;
	wxComboBox *lbox;

	objref(fitems);
	bloop = init_bucket_loop(fitems);
	while (bloop && (fi = (struct form_item*)next_bucket_loop(bloop))) {
		if (!fi->data.ptr || (fi->dtype != DTSGUI_FORM_DATA_XML)) {
			objunref(fi);
			continue;
		}

		xml = fi->data.xml;

		switch (fi->type) {
			case DTS_WIDGET_TEXTBOX:
				tbox = (wxTextCtrl*)fi->widget;
				value = strdup(tbox->GetValue().ToUTF8());
				break;
			case DTS_WIDGET_CHECKBOX:
				cbox = (wxCheckBox*)fi->widget;
				tmp = (cbox->GetValue()) ? fi->value : fi->value2;
				if (tmp) {
					value = strdup(tmp);
				} else {
					value = NULL;
				}
				break;
			case DTS_WIDGET_LISTBOX:
			case DTS_WIDGET_COMBOBOX:
				lbox = (wxComboBox*)fi->widget;
				if (!(value = strdup((const char*)lbox->GetClientData(lbox->GetSelection())))) {
					value = strdup(lbox->GetValue().ToUTF8());
				}
				break;
			default:
				value = NULL;
		}

		if ((xn = xml_getfirstnode(xml->xsearch, NULL))) {
			if (xml->attr) {
				xml_setattr(xmldoc, xn, xml->attr, (value) ? value : "");
			} else {
				xml_modify(xmldoc, xn, (value) ? value : "");
			}
			objunref(xn);
		}

		if (value) {
			free((void*)value);
		}
		objunref(fi);
	}
	stop_bucket_loop(bloop);
	objunref(fitems);
}

DTSScrollPanel::DTSScrollPanel(wxWindow *parent,DTSFrame *frame, wxString status, int butmask)
	:wxScrolledWindow(parent, wxID_ANY),
	 DTSPanel(frame, status, butmask) {
	type = wx_DTSPANEL_SCROLLPANEL;
	SetScrollRate(10, 10);
	panel = dynamic_cast<wxPanel *>(this);
	SetupWin();
}

bool DTSScrollPanel::Show(bool show) {
	ShowPanel(show);
	return wxScrolledWindow::Show(show);
}

DTSStaticPanel::DTSStaticPanel(wxWindow *parent,DTSFrame *frame, wxString status, int butmask)
	:wxPanel(parent, wxID_ANY),
	 DTSPanel(frame, status, butmask) {
	type = wx_DTSPANEL_PANEL;
	panel = dynamic_cast<wxPanel *>(this);
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
	SetupWin();
}

bool DTSWindow::Show(bool show) {
	ShowPanel(show);
	return wxWindow::Show(show);
}

DTSDialog::DTSDialog(DTSFrame *frame, wxString name, int butmask) {
	type = wx_DTSPANEL_DIALOG;

	button_mask = butmask;
	buttons[4] = wxID_OK;
	buttons[5] = wxID_CANCEL;

	dialog = new wxDialog(frame, -1, name);
	sizer = new wxBoxSizer(wxHORIZONTAL);
	dialog->SetSizer(sizer);
	dialog->SetAffirmativeId(buttons[4]);

	panel = dynamic_cast<wxPanel *>(this);
	panel->Create(dialog, -1);
	panel->SetName(name);
	panel->Bind(wxEVT_TEXT_ENTER, &DTSPanelEvent::OnDialog, dtsevthandler);

	SetupWin();
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
