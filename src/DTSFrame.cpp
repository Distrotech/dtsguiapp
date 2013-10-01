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

#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/wizard.h>
#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include <wx/scrolwin.h>
#include <wx/stattext.h>
#include <wx/combobox.h>
#include <wx/toolbar.h>
#include <wx/progdlg.h>

#include <dtsapp.h>

#include "dtsgui.hpp"

#include "evdata.h"
#include "DTSFrame.h"
#include "DTSPanel.h"

wxDEFINE_EVENT(DTS_APP_EVENT, wxCommandEvent);

DTSFrame::DTSFrame(const wxString &title, const wxPoint &pos, const wxSize &size, struct dtsgui *dtsgui)
	: wxFrame(NULL, wxID_ANY, title, pos, size) {
	/*deleted on close*/
	menubar = new wxMenuBar;
	SetMenuBar(menubar);

	status = dtsgui->status;
	CreateStatusBar();
	SetStatusText(status);

	SetupToolbar();

	/*deleted on close*/
	sizer = new wxBoxSizer(wxHORIZONTAL);
	this->SetSizer(sizer);

	SetMinSize(size);
	dtsgui->appframe = this;
	objref(dtsgui);
	this->dtsgui = dtsgui;

	blank = new wxWindow(this, -1);
	sizer->Add(blank, 1, wxALL | wxEXPAND);
	a_window = blank;
	blank->Show(true);

	abortconfirm = true;
	Bind(wxEVT_CLOSE_WINDOW, &DTSFrame::OnAbort, this);
	Bind(DTS_APP_EVENT, &DTSFrame::OnDTSEvent, this);

	Show(true);
	Raise();
}

DTSFrame::~DTSFrame() {
	objunref(dtsgui);
	delete blank;
}

void DTSFrame::SetAbout(const char *a_text) {
	about = a_text;
}

bool DTSFrame::Confirm(wxString text) {
	wxMessageDialog *confirm;
	int res;

	confirm = new wxMessageDialog(NULL, text, wxT("Confirm"),
								  wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
	res = confirm->ShowModal();
	confirm->Destroy();

	if (res == wxID_YES) {
		return true;
	} else {
		return false;
	}
}

void DTSFrame::Alert(wxString text) {
	wxMessageDialog *alert;

	alert = new wxMessageDialog(NULL, text, wxT("Alert"),
								  wxOK | wxICON_EXCLAMATION);
	alert->ShowModal();
	alert->Destroy();
}

wxProgressDialog *DTSFrame::StartProgress(wxString text, int maxval, int quit) {
	wxProgressDialog *pdlg;
	int flags;

	flags = wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxPD_ELAPSED_TIME;
	if (quit) {
		flags |= wxPD_CAN_ABORT;
	}

	pdlg = new wxProgressDialog("Progress", text, maxval, NULL, flags);
	pdlg->Show();
	return pdlg;
}

int DTSFrame::UpdateProgress(wxProgressDialog *pdlg, int cval, const wxString& newtext) {
	if (pdlg->Update(cval, newtext)) {
		return 1;
	} else {
		return 0;
	}
}

void DTSFrame::OnDTSEvent(wxCommandEvent &event) {
	event.Skip(true);
}

void DTSFrame::OnAbort(wxCloseEvent &event) {
	if (abortconfirm && !Confirm("Are You Sure You Want To Abort ?")) {
		event.Veto();
	} else {
		event.Skip(true);
	}
}

void DTSFrame::OnClose(wxCommandEvent &event) {
	bool confirm;
	int eid;

	eid=event.GetId();

	switch (eid) {
		case wxID_EXIT:
			confirm = Confirm("Quit Without Saving ?");
			break;

		case wxID_SAVE:
			confirm = Confirm("Save And Exit ?");
			break;

		default:
			confirm = true;
	}

	if (confirm) {
		abortconfirm = false;
		Close(true);
	} else {
		event.Skip();
	}
}

void DTSFrame::OnAbout(wxCommandEvent &event) {
	wxMessageBox(about, "About", wxOK | wxICON_INFORMATION );
}

void DTSFrame::SetWindow(wxWindow *window) {
	if (!window) {
		window = blank;
	}

	if (window == a_window) {
		return;
	}

	a_window->Show(false);
	sizer->Replace(a_window, window);

	if (window == blank) {
		SetStatusText(status);
	}

	window->Show(true);
	sizer->Layout();
	sizer->FitInside(this);
	a_window = window;
}

void DTSFrame::SwitchWindow(wxCommandEvent &event) {
	wxWindow *window;
	class evdata *evdat;

	evdat = (evdata*)event.m_callbackUserData;

	if (!evdat || (!(window = (wxWindow*)evdat->data))) {
		window = blank;
	}

	if (window != a_window) {
		SetWindow(window);
	}
}

void DTSFrame::DynamicPanelEvent(wxCommandEvent &event) {
	class evdata *evdat;
	struct dynamic_panel *p_dyn;
	DTSObject *p;

	if (!(evdat = (evdata *)event.m_callbackUserData)) {
		SetWindow(NULL);
		return;
	}

	if (!evdat->data || !(p_dyn = (struct dynamic_panel*)evdat->data) || !p_dyn->cb ) {
		SetWindow(NULL);
		return;
	}

	if (p_dyn->w == a_window) {
		return;
	}

	if (p_dyn->blank) {
		SetWindow(NULL);
	}

	if (p_dyn->w) {
		delete p_dyn->w;
		p_dyn->w = NULL;
	}

	if ((p = (DTSObject*)p_dyn->cb(dtsgui, p_dyn->title, p_dyn->data))) {
		p_dyn->w = p->GetPanel();
		SetWindow(p_dyn->w);
	}
}

void DTSFrame::SendDTSEvent(int eid, wxObject *evobj) {
	wxCommandEvent event(DTS_APP_EVENT, eid);
	event.SetEventObject(evobj);
	event.ShouldPropagate();
	ProcessWindowEvent(event);
}

struct dtsgui *DTSFrame::GetDTSData(void) {
	if (objref(dtsgui)) {
		return dtsgui;
	}
	return NULL;
}

void DTSFrame::SetupToolbar() {
	toolbar = CreateToolBar();

	wxComboBox *server = new wxComboBox(toolbar, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(300,-1) );
	wxComboBox *proto = new wxComboBox(toolbar, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
	wxStaticText *text = new wxStaticText(toolbar, wxID_ANY, "Server ");
	wxStaticText *text2 = new wxStaticText(toolbar, wxID_ANY, "://");

	toolbar->AddControl(text);

	proto->Append("http");
	proto->Append("https");
	proto->Append("https [:666]");
	proto->SetSelection(2);

	toolbar->AddControl(proto);
	toolbar->AddControl(text2);
	toolbar->AddControl(server);
	toolbar->AddStretchableSpace();
	toolbar->Realize();
}
