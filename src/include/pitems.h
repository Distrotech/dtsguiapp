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


#ifndef PITEMS_H_INCLUDED
#define PITEMS_H_INCLUDED

typedef class wxComboBox wxComboBox;
typedef class wxCheckBox wxCheckBox;

class xml_element {
	public:
		DTS_OJBREF_CLASS(xml_element);
		xml_element(const char *xp, struct xml_search *xs, const char *xa);
		void Modify(struct xml_doc *xmldoc, const char *value);
		const char *GetValue();
	private:
		struct xml_search *xsearch;
		const char *xpath;
		const char  *attr;
};

union form_data {
	void *ptr;
	class xml_element *xml;
};

union widgets {
	wxTextCtrl *t;
	wxComboBox *l;
	wxCheckBox *c;
	void *p;
};

class form_item {
	public:
		DTS_OJBREF_CLASS(form_item);
		form_item(void *widget, enum widget_type type, const char *name, const char *value, const char *value2, void *data, enum form_data_type dtype);
		class xml_element *GetXMLData();
		void *GetData();
		void Append(const char *text, const char *val);
		void SetSelection(int i);
		const char *GetName();
		const char *GetValue();
		void *GetWidget();
		void AppendXML(struct xml_doc *xmldoc, const char *xpath, const char *nattr, const char *vattr);
	private:
		enum widget_type type;
		const char *name;
		int idx;
		union form_data data;
		union widgets widget;
		enum form_data_type dtype;
		const char *value;
		const char *value2;
};

#endif // PITEMS_H_INCLUDED
