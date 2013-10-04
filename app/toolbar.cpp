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

#

//typedef struct xml_post* (*getpost)(const wxString&);

class DTSXMLComboBox: public wxComboBox {
	public:
		DTSXMLComboBox(wxWindow *parent, wxString url, wxString xpath, struct curl_post*(*getpost)(const wxString&), size_t minlen);
		~DTSXMLComboBox();
		bool HasXMLList();
	private:
		void HandleEvent(wxCommandEvent& event);
		struct xml_doc *GetXMLDoc(const wxString& val, struct xml_node *xn);
		struct curl_post *GetPostInfo(const wxString& val);
		bool AppendXMLNodes(struct xml_doc *xmldoc);
		void EmptyServerList();
		int sflags;
		size_t minlen;
		wxString url;
		wxString xpath;
		struct curl_post*(*getpost)(const wxString&);
};


DTSXMLComboBox::DTSXMLComboBox(wxWindow *parent, wxString url, wxString xpath, struct curl_post*(*getpost)(const wxString&), size_t minlen) {
	sflags = 0;

	this->minlen = minlen;
	this->url = url;
	this->xpath = xpath;
	this->getpost = getpost;

	Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxTE_PROCESS_ENTER | wxCB_DROPDOWN);
	Append(wxEmptyString, (void*)NULL);

	Bind(wxEVT_TEXT_ENTER, &DTSXMLComboBox::HandleEvent, this);
	Bind(wxEVT_COMBOBOX, &DTSXMLComboBox::HandleEvent, this);
	Bind(wxEVT_COMBOBOX_DROPDOWN, &DTSXMLComboBox::HandleEvent, this);
	Bind(wxEVT_COMMAND_TEXT_UPDATED, &DTSXMLComboBox::HandleEvent, this);
}

struct xml_doc *DTSXMLComboBox::GetXMLDoc(const wxString& val, struct xml_node *xn) {
	struct xml_doc *xmldoc = NULL;
	struct curl_post *post;
	struct curlbuf *cbuf;

	if (getpost) {
		post = getpost(val);
	} else {
		return NULL;
	}

	if (!(cbuf = dtsgui_posturl(url, post))) {
		return NULL;
	}

	curl_ungzip(cbuf);
	if (cbuf && cbuf->c_type && !strcmp("application/xml", cbuf->c_type)) {
		xmldoc = xml_loadbuf(cbuf->body, cbuf->bsize, 0);
	}
	objunref(cbuf);

	return xmldoc;
}

bool DTSXMLComboBox::AppendXMLNodes(struct xml_doc *xmldoc) {
	struct xml_search *xp;
	struct xml_node *xn;
	void *iter;

	xp = xml_xpath(xmldoc, xpath, NULL);
	if (!xp || !xml_nodecount(xp)) {
		if (xp) {
			objunref(xp);
		}
		objunref(xmldoc);
		return false;
	}
	for(xn = xml_getfirstnode(xp, &iter); xn; xn = xml_getnextnode(iter)) {
		Append(xn->value, xn);
	}
	objunref(iter);
	objunref(xp);
	objunref(xmldoc);
	return true;
}

DTSXMLComboBox::~DTSXMLComboBox() {
	EmptyServerList();
}

bool DTSXMLComboBox::HasXMLList() {
	if (sflags & DTSAPPTB_SERVER_POP) {
		return true;
	} else {
		return false;
	}
}

void DTSXMLComboBox::HandleEvent(wxCommandEvent& event) {
	struct xml_doc *xmldoc;
	wxString newval;
	int etype = event.GetEventType();
	wxString val;
	struct xml_node *xn = NULL;

	val = GetValue();

	if ((sflags & DTSAPPTB_SERVER_POP) && !(sflags & DTSAPPTB_SERVER_SET) && (etype == wxEVT_COMMAND_TEXT_UPDATED)) {
		sflags &= ~DTSAPPTB_SERVER_POP;
		event.Skip();
	} else if ((sflags & DTSAPPTB_SERVER_SET) && (etype == wxEVT_COMMAND_TEXT_UPDATED)) {
		sflags &= ~DTSAPPTB_SERVER_SET;
		event.Skip();
	} else if (!(sflags & DTSAPPTB_SERVER_POP) && !(sflags & DTSAPPTB_SERVER_SET) && (etype == wxEVT_COMBOBOX_DROPDOWN) && (val.Len() < minlen)) {
		sflags &= ~DTSAPPTB_SERVER_SET;
		EmptyServerList();
		Append(wxEmptyString, (void*)NULL);
		Dismiss();
		event.Skip();
	} else if (!(sflags & DTSAPPTB_SERVER_POP) && (etype == wxEVT_COMBOBOX_DROPDOWN) && (val.Len() >= minlen)) {
		Dismiss();
		EmptyServerList();
		sflags &= ~(DTSAPPTB_SERVER_SET | DTSAPPTB_SERVER_POP);

		if ((xmldoc = GetXMLDoc(val, xn))) {
			xml_config(xmldoc);
			if (!AppendXMLNodes(xmldoc)) {
				Append(wxEmptyString, (void*)NULL);
				sflags &= ~(DTSAPPTB_SERVER_POP | DTSAPPTB_SERVER_SET);
			} else {
				sflags |= (DTSAPPTB_SERVER_POP | DTSAPPTB_SERVER_SET);
				event.Skip();
				Popup();
			}
		} else {
			Append(wxEmptyString, (void*)NULL);
			sflags &= ~(DTSAPPTB_SERVER_POP | DTSAPPTB_SERVER_SET);
		}
	} else if (!(sflags & DTSAPPTB_SERVER_POP) && (etype == wxEVT_COMBOBOX_DROPDOWN)) {
		EmptyServerList();
		Append(wxEmptyString, (void*)NULL);
		Dismiss();
	} else if ((sflags & DTSAPPTB_SERVER_POP) && etype == wxEVT_COMBOBOX) {
		/*Windows Barfs if you change the value*/
#ifdef __WIN332
		sflags |= (DTSAPPTB_SERVER_POP | DTSAPPTB_SERVER_SET);
#else
		sflags |= DTSAPPTB_SERVER_SET;
#endif // __WIN32
		event.Skip();
	} else if (etype != wxEVT_COMBOBOX_DROPDOWN) {
		event.Skip();
	}
}

void DTSXMLComboBox::EmptyServerList() {
	void *data;
	int i, cnt;

	cnt = GetCount();
	for(i=0; i < cnt;i++) {
		if ((data = GetClientData(i))) {
			objunref(data);
			SetClientData(i, NULL);
		}
	}
	Clear();
}

class DTSAPPToolBar: public wxToolBar {
	public:
		DTSAPPToolBar(struct dtsgui *dtsgui, wxWindow *parent, long style, wxWindowID id, wxString name, void *data);
		static struct curl_post *GetPostInfo(const wxString& val);
	private:
		void HandleEvent(wxCommandEvent &event);
		DTSXMLComboBox *server;
		wxComboBox *proto;
		struct dtsgui *dtsgui;
};


struct curl_post *DTSAPPToolBar::GetPostInfo(const wxString& val) {
	struct curl_post *post;

	post = curl_newpost();
	curl_postitem(post, "function", "getcust");
	curl_postitem(post, "search", val);

	return post;
}

DTSAPPToolBar::DTSAPPToolBar(struct dtsgui *dtsgui, wxWindow *parent, long style, wxWindowID id, wxString name, void *data) {
	if (dtsgui && objref(dtsgui)) {
		this->dtsgui = (struct dtsgui*)dtsgui;
	}

	Create(parent, id, wxDefaultPosition, wxDefaultSize, style, name);
	proto = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
	server = new DTSXMLComboBox(this, "https://sip1.speakezi.co.za:666/auth/test.php", "/servers/Server", &DTSAPPToolBar::GetPostInfo, 3);
	server->SetSize(wxSize(300,-1));
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
	AddStretchableSpace();

	Bind(wxEVT_TEXT_ENTER, &DTSAPPToolBar::HandleEvent, this);
	Bind(wxEVT_COMBOBOX, &DTSAPPToolBar::HandleEvent, this);
	Bind(wxEVT_COMBOBOX_DROPDOWN, &DTSAPPToolBar::HandleEvent, this);
}

void DTSAPPToolBar::HandleEvent(wxCommandEvent& event) {
	int etype = event.GetEventType(), idx;
	struct xml_node *xn;

	idx = server->GetSelection();

	if (etype == wxEVT_TEXT_ENTER) {
		dtsgui_alert(dtsgui, "Got Me Some URL ARRRRGh");
	} else if (etype == wxEVT_COMBOBOX) {
		xn = (struct xml_node*)server->GetClientData(idx);
		dtsgui_alert(dtsgui, wxString("Selected IP ").Append(xml_getattr(xn, "ipaddr")));
	} else if ((etype == wxEVT_COMBOBOX_DROPDOWN) && !server->HasXMLList()) {
		dtsgui_alert(dtsgui, "Please enter 3 or more characters to search !");
	}
}

void *app_toolbar(struct dtsgui *dtsgui, void *pw, long style, int id, const char *name, void *data) {
	DTSAPPToolBar *tb;

	tb = new DTSAPPToolBar(dtsgui, (wxWindow*)pw, style, id, name, data);

	return tb;
}
