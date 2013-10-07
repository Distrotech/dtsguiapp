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

#ifndef DTSWIZARD_H_INCLUDED
#define DTSWIZARD_H_INCLUDED

typedef class wxWizard wxWizard;
typedef class DTSWizardWindow DTSWizardWindow;

class dtsgui_wizard {
	public:
		DTS_OJBREF_CLASS(dtsgui_wizard);
		dtsgui_wizard(class dtsgui *dtsgui, wxWindow *f, const wxString &title);
		DTSWizardWindow *AddPage(const char *title, struct xml_doc *xmldoc, void *userdata);
		int RunWizard();
	private:
		wxWizard *wiz;
		wxWizardPageSimple *start;
		class dtsgui *dtsgui;
};

#endif // DTSWIZARD_H_INCLUDED
