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

#include <wx/toolbar.h>
#include <wx/combobox.h>
#include <wx/stattext.h>

#include <dtsapp.h>

#include "dtsgui.h"
#include "private.h"

enum server_state {
	DTSAPPTB_SERVER_POP = 1 << 0,
	DTSAPPTB_SERVER_SET = 1 << 1
};


class DTSAPPToolBar: public wxToolBar {
	public:
		DTSAPPToolBar(struct dtsgui *dtsgui, wxWindow *parent, long style, wxWindowID id, wxString name, void *data);
	private:
		void OnServer(wxCommandEvent& event);
		void EmptyServerList();
		wxComboBox *server;
		wxComboBox *proto;
		struct dtsgui *dtsgui;
		int sflags;
};


DTSAPPToolBar::DTSAPPToolBar(struct dtsgui *dtsgui, wxWindow *parent, long style, wxWindowID id, wxString name, void *data) {
	if (dtsgui && objref(dtsgui)) {
		this->dtsgui = (struct dtsgui*)dtsgui;
	}
	sflags = 0;

	Create(parent, id, wxDefaultPosition, wxDefaultSize, style, name);
	proto = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
	server = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(300,-1), 0, NULL, wxTE_PROCESS_ENTER | wxCB_DROPDOWN);
	wxStaticText *text2 = new wxStaticText(this, wxID_ANY, "://");
	wxStaticText *text = new wxStaticText(this, wxID_ANY, "Server ");

	AddControl(text);
	proto->Append("http");
	proto->Append("https");
	proto->Append("https [:666]");
	proto->SetSelection(2);

	AddControl(proto);
	AddControl(text2);
	AddControl(server);
	server->Append(wxEmptyString, (void*)NULL);
	AddStretchableSpace();

	server->Bind(wxEVT_TEXT_ENTER, &DTSAPPToolBar::OnServer, this);
	server->Bind(wxEVT_COMBOBOX, &DTSAPPToolBar::OnServer, this);
	server->Bind(wxEVT_COMBOBOX_DROPDOWN, &DTSAPPToolBar::OnServer, this);
	server->Bind(wxEVT_COMMAND_TEXT_UPDATED, &DTSAPPToolBar::OnServer, this);
}

void DTSAPPToolBar::EmptyServerList() {
	void *data;
	int i, cnt;

	cnt = server->GetCount();
	for(i=0; i < cnt;i++) {
		if ((data = server->GetClientData(i))) {
			free(data);
			server->SetClientData(i, NULL);
		}
	}
	server->Clear();
}

void DTSAPPToolBar::OnServer(wxCommandEvent& event) {
	struct xml_doc *xmldoc;
	struct xml_search *xp;
	struct xml_node *xn;
	wxString newval;
	void *iter;
	int etype = event.GetEventType(), idx;
	wxString val;
	const char *ipaddr;

	val = server->GetValue();
	idx = server->GetSelection();

	if ((sflags & DTSAPPTB_SERVER_POP) && !(sflags & DTSAPPTB_SERVER_SET) && (etype == wxEVT_COMMAND_TEXT_UPDATED)) {
		sflags &= ~DTSAPPTB_SERVER_POP;
	} else if ((sflags & DTSAPPTB_SERVER_SET) && (etype == wxEVT_COMMAND_TEXT_UPDATED)) {
		sflags &= ~DTSAPPTB_SERVER_SET;
	} else if (!(sflags & DTSAPPTB_SERVER_POP) && !(sflags & DTSAPPTB_SERVER_SET) && (etype == wxEVT_COMBOBOX_DROPDOWN) && (val.Len() < 3)) {
		sflags &= ~DTSAPPTB_SERVER_SET;
		EmptyServerList();
		server->Append(wxEmptyString, (void*)NULL);
		server->Dismiss();
		event.SetEventType(wxEVT_COMBOBOX_CLOSEUP);
		dtsgui_alert(dtsgui, "Please enter 3 or more characters to search !");
	} else if (!(sflags & DTSAPPTB_SERVER_POP) && (etype == wxEVT_COMBOBOX_DROPDOWN) && (val.Len() >= 3)) {
		struct curl_post *post = curl_newpost();
		struct curlbuf *cbuf;

		EmptyServerList();
		server->Dismiss();

		curl_postitem(post, "function", "getcust");
		curl_postitem(post, "search", val);
		cbuf = test_posturl(dtsgui, NULL, NULL, "https://sip1.speakezi.co.za:666/auth/test.php" , post);

		sflags &= ~(DTSAPPTB_SERVER_SET | DTSAPPTB_SERVER_POP);
		curl_ungzip(cbuf);
		if (cbuf && cbuf->c_type && !strcmp("application/xml", cbuf->c_type)) {
			xmldoc = xml_loadbuf(cbuf->body, cbuf->bsize, 0);
			xml_config(xmldoc);
			xp = xml_xpath(xmldoc, "/servers/Server", "ipaddr");
			if (!xp || !xml_nodecount(xp)) {
				server->Append(wxEmptyString, (void*)NULL);
				sflags &= ~(DTSAPPTB_SERVER_POP | DTSAPPTB_SERVER_SET);
				event.SetEventType(wxEVT_COMBOBOX_CLOSEUP);
			} else {
				sflags |= (DTSAPPTB_SERVER_POP | DTSAPPTB_SERVER_SET);
				for(xn = xml_getfirstnode(xp, &iter); xn; xn = xml_getnextnode(iter)) {
					ipaddr = strdup(xml_getattr(xn, "ipaddr"));
					newval = wxString(xn->value).Append(" [").Append(ipaddr).Append("]");
					server->Append(newval, (void*)ipaddr);
					objunref(xn);
				}
				objunref(iter);
				server->Popup();
			}
			if (xp) {
				objunref(xp);
			}
			if (xmldoc) {
				objunref(xmldoc);
			}
		} else {
			event.SetEventType(wxEVT_COMBOBOX_CLOSEUP);
			server->Append(wxEmptyString, (void*)NULL);
			sflags &= ~(DTSAPPTB_SERVER_POP | DTSAPPTB_SERVER_SET);
		}

		if (cbuf) {
			objunref(cbuf);
		}
	} else if (!(sflags & DTSAPPTB_SERVER_POP) && (etype == wxEVT_COMBOBOX_DROPDOWN)) {
		EmptyServerList();
		server->Append(wxEmptyString, (void*)NULL);
		server->Dismiss();
		event.SetEventType(wxEVT_COMBOBOX_CLOSEUP);
	} else if (etype == wxEVT_TEXT_ENTER) {
		dtsgui_alert(dtsgui, "Got Me Some URL ARRRRGh");
	} else if ((sflags & DTSAPPTB_SERVER_POP) && etype == wxEVT_COMBOBOX) {
		ipaddr = (const char*)server->GetClientData(idx);
		dtsgui_alert(dtsgui, wxString("Selected IP ").Append(ipaddr));
		/*Windows Barfs if you change the value*/
#ifdef __WIN32
		sflags |= (DTSAPPTB_SERVER_POP | DTSAPPTB_SERVER_SET);
#else
		sflags |= DTSAPPTB_SERVER_POP;
#endif // __WIN32
	}
}

void *app_toolbar(struct dtsgui *dtsgui, void *pw, long style, int id, const char *name, void *data) {
	DTSAPPToolBar *tb;

	tb = new DTSAPPToolBar(dtsgui, (wxWindow*)pw, style, id, name, data);

	return tb;
}
