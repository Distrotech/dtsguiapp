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

#ifndef DTSXMLCOMBOBOX_H
#define DTSXMLCOMBOBOX_H

enum server_state {
	DTSAPPTB_SERVER_POP = 1 << 0,
	DTSAPPTB_SERVER_SET = 1 << 1
};

class DTSXMLComboBox: public wxComboBox {
	public:
		DTSXMLComboBox(wxWindow *parent, int id, wxString url, wxString xpath, struct curl_post*(*getpost)(const wxString&), size_t minlen);
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

#endif // DTSXMLCOMBOBOX_H
