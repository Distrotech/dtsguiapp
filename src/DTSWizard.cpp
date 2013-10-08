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
#endif

#include "dtsgui.h"

#include "DTSWizard.h"
#include "DTSFrame.h"
#include "DTSApp.h"
#include "DTSPanel.h"

dtsgui_wizard::dtsgui_wizard(class dtsgui *dgui, wxWindow *f, const wxString &title) {
	if (dgui && objref(dgui)) {
		dtsgui = dgui;
		wiz = new wxWizard(f, wxID_ANY, title, wxNullBitmap, wxDefaultPosition, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	}

	start = NULL;
}

dtsgui_wizard::~dtsgui_wizard() {
	if (wiz) {
		delete wiz;
	}
	if (dtsgui) {
		objunref(dtsgui);
	}
}

DTSWizardWindow *dtsgui_wizard::AddPage(const char *title, struct xml_doc *xmldoc, void *userdata) {
	DTSWizardWindow *dww;
	wxWizardPageSimple *wp, *tmp;
	wxWizardPage *last;
	DTSFrame *f = dtsgui->GetFrame();

	dww = (DTSWizardWindow*)f->CreatePane(title, wxEmptyString, 0, wx_DTSPANEL_WIZARD, userdata);

	wp = dynamic_cast<wxWizardPageSimple *>(dww);
	wp->Create(wiz);

	if (title) {
		dww->SetTitle(title, true);
	}

	if (xmldoc) {
		dww->SetXMLDoc(xmldoc);
	}

	objlock(this);
	if (!start) {
		start = wp;
	} else {
		for(last = start; last->GetNext(); last=last->GetNext());

		tmp = dynamic_cast<wxWizardPageSimple*>(last);
		tmp->SetNext(wp);
		wp->SetPrev(tmp);
	}
	objunlock(this);
	return dww;
}

int dtsgui_wizard::RunWizard() {
	wiz->GetPageAreaSizer()->Add(start);
	wiz->Center();

	return wiz->RunWizard(start);
}
