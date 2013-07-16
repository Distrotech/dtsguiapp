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

#include "dtsgui.hpp"

#include "evdata.h"
#include "DTSFrame.h"

DTSFrame::DTSFrame(const wxString &title, const wxPoint &pos, const wxSize &size, struct dtsgui *dtsgui)
	: wxFrame(NULL, wxID_ANY, title, pos, size) {
	/*deleted on close*/
	menubar = new wxMenuBar;
	SetMenuBar(menubar);

	status = dtsgui->status;
	CreateStatusBar();
	SetStatusText(status);

	/*deleted on close*/
	sizer = new wxBoxSizer(wxHORIZONTAL);
	this->SetSizer(sizer);

	SetMinSize(size);
	dtsgui->appframe = this;

	blank = new wxWindow(this, -1);
	sizer->Add(blank, 1, wxALL | wxEXPAND);
	a_window = blank;

	blank->Show(true);

	abortconfirm = true;
	Bind(wxEVT_CLOSE_WINDOW, &DTSFrame::OnAbort, this);
	Show(true);
}

DTSFrame::~DTSFrame() {
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
		default
				:
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
	if (window == a_window) {
		return;
	}

	sizer->Detach(0);
	a_window->Show(false);
	sizer->Prepend(window, 1, wxALL | wxEXPAND);
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

	evdat = (evdata *)event.m_callbackUserData;
	if (!evdat || (!(window = (wxWindow *)evdat->data))) {
		window = blank;
	}
	if (window != a_window) {
		SetWindow(window);
	}
}
