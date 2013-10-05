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

#ifndef DTSGUI_HPP_INCLUDED
#define DTSGUI_HPP_INCLUDED

#include "dtsgui.h"

wxDECLARE_EVENT(DTS_APP_EVENT, wxCommandEvent);

struct dtsgui {
	public:
		int SetupAPPFrame(void);
		void Setup(const char *title, const char *stat, struct point w_size, struct point w_pos, dtsgui_configcb confcallback_cb , void *userdata);
		void *GetUserData(void);
		void SetStatusText(void);
		void AppendTitle(const char *text);
		void UnRef(void);
		class DTSFrame *GetFrame(void);
	private:
		class DTSFrame *appframe;
		const char *title;
		const char *status;
		dtsgui_configcb cb;
		void *userdata;
		struct point wsize;
		struct point wpos;
};

struct dtsgui_wizard {
	wxWizard *wiz;
	wxWizardPageSimple *start;
	struct dtsgui *dtsgui;
};

#endif // DTSGUI_HPP_INCLUDED
