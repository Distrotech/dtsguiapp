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

#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <wx/wizard.h>

#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/gauge.h>
#include <wx/progdlg.h>
#include <wx/toolbar.h>

#include <dtsapp.h>
#include <wx/app.h>

#include "dtsgui.h"
#include "dtsgui.hpp"

#include "pitems.h"
#include "evdata.h"
#include "DTSFrame.h"
#include "DTSPanel.h"
#include "DTSApp.h"

wxDEFINE_EVENT(DTS_APP_EVENT, wxCommandEvent);

DTSFrame::DTSFrame(const wxString &title, const wxPoint &pos, const wxSize &size, class dtsgui *dtsgui)
	: wxFrame(NULL, wxID_ANY, title, pos, size) {

	if (!dtsgui || !objref(dtsgui)) {
		return;
	}

	this->dtsgui = dtsgui;

	/*deleted on close*/
	menubar = new wxMenuBar;
	SetMenuBar(menubar);

	CreateStatusBar();

	tbcb = NULL;
	tb_data = NULL;

//	pbar = new wxGauge(toolbar, wxID_ANY, 1000);
	pbar = NULL;
	pdia = NULL;

	/*deleted on close*/
	sizer = new wxBoxSizer(wxHORIZONTAL);
	this->SetSizer(sizer);

	SetMinSize(size);

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
	if (tb_data) {
		objunref(tb_data);
	}
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

int DTSFrame::StartProgress(const wxString &text, int maxval, int quit) {
	int flags;

	if (!quit && pbar) {
		pbar->SetRange(maxval);
		pbar->Show();
		pbar->Pulse();
		return 1;
	} else {
		flags = wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxPD_ELAPSED_TIME;
		if (quit) {
			flags |= wxPD_CAN_ABORT;
		}
		if ((pdia = new wxProgressDialog("Progress", text, maxval, NULL, flags))) {
			pdia->Show();
			pdia->Pulse();
			return 1;
		}
	}
	return 0;
}

int DTSFrame::UpdateProgress(int cval, const wxString &text) {
	if (pbar) {
		pbar->SetValue(cval);
	}
	if (pdia) {
		if (pdia->Update(cval, text)) {
			return 1;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

int DTSFrame::IncProgress(int ival, const wxString &text) {
	if (pbar) {
		pbar->SetValue(pbar->GetValue()+ival);
	}
	if (pdia) {
		if (pdia->Update(pdia->GetValue()+ival, text)) {
			return 1;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

void DTSFrame::EndProgress(void) {
	if (pdia) {
		pdia->Hide();
		delete pdia;
		pdia = NULL;
	}
	if (pbar) {
		pbar->Hide();
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
		dtsgui->SetStatusText();
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

	if (objref(dtsgui)) {
		if ((p = (DTSObject*)p_dyn->cb(dtsgui, p_dyn->title, p_dyn->data))) {
			p_dyn->w = p->GetPanel();
			SetWindow(p_dyn->w);
		}
		objunref(dtsgui);
	}
}

void DTSFrame::SendDTSEvent(int eid, wxObject *evobj) {
	wxCommandEvent event(DTS_APP_EVENT, eid);
	event.SetEventObject(evobj);
	event.ShouldPropagate();
	ProcessWindowEvent(event);
}

class dtsgui *DTSFrame::GetDTSData(void) {
	if (dtsgui && objref(dtsgui)) {
		return dtsgui;
	}
	return NULL;
}

wxToolBar *DTSFrame::OnCreateToolBar(long style, wxWindowID id, const wxString& name) {
	wxToolBar *tb;

	if (tbcb) {
		objref(dtsgui);
		tb = (wxToolBar*)tbcb(dtsgui, this, style, id, name, tb_data);
		objunref(dtsgui);
	} else {
		tb = new wxToolBar(this, id, wxDefaultPosition, wxDefaultSize, style, name);
	}

	return tb;
}

void DTSFrame::SetupToolbar(dtsgui_toolbar_create cb, void *data) {
	tbcb = cb;

	if (tb_data) {
		objunref(tb_data);
		tb_data = NULL;
	}

	if (data && objref(data)) {
		tb_data = data;
	}

	toolbar = CreateToolBar();

	if (pbar) {
		toolbar->AddControl(pbar);
		pbar->Hide();
	}
	toolbar->Realize();
}

wxMenuBar *DTSFrame::GetMenuBar() {
	return menubar;
}
