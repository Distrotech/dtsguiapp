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
#include <string.h>

#include <wx/combobox.h>
#include <wx/checkbox.h>

#include <dtsapp.h>
#include "dtsgui.h"

#include "pitems.h"

xml_element::~xml_element() {
	if (xsearch) {
		objunref(xsearch);
	}
	if (xpath) {
		free((void*)xpath);
	}
	if (attr) {
		free((void*)attr);
	}
}

xml_element::xml_element(const char *xp, struct xml_search *xs, const char *xattr) {
	xpath = xp;
	xsearch = xs;
	if (xattr) {
		attr = strdup(xattr);
	}
}

void xml_element::Modify(struct xml_doc *xmldoc, const char *value) {
	struct xml_node *xn;

	if ((xn = xml_getfirstnode(xsearch, NULL))) {
		if (attr) {
			xml_setattr(xmldoc, xn, attr, (value) ? value : "");
		} else {
			xml_modify(xmldoc, xn, (value) ? value : "");
		}
		objunref(xn);
	}
}

const char *xml_element::GetValue() {
	struct xml_node *xn;
	const char *ret = NULL;
	const char *tmp;

	if (!xsearch) {
		return NULL;
	}

	xn = xml_getfirstnode(xsearch, NULL);
	if (!attr) {
		if (xn->value) {
			ret = strdup(xn->value);
		}
	} else if ((tmp = xml_getattr(xn, attr))) {
		ret = strdup(tmp);
	}

	objunref(xn);
	return ret;
}

form_item::form_item(void *widget, enum widget_type type, const char *name, const char *value, const char *value2, void *data, enum form_data_type dtype) {
	this->widget.p = widget;
	this->type = type;
	this->data.ptr = data;
	this->dtype = dtype;
	ALLOC_CONST(this->name, name);
	if (value) {
		ALLOC_CONST(this->value, value);
	}
	if (value2) {
		ALLOC_CONST(this->value2, value2);
	}
	this->idx = -1;
}

form_item::~form_item() {
	switch(type) {
		case DTS_WIDGET_CHECKBOX:
		case DTS_WIDGET_TEXTBOX:
			break;
		case DTS_WIDGET_LISTBOX:
		case DTS_WIDGET_COMBOBOX:
			wxComboBox *cbox = widget.l;
			int cnt = cbox->GetCount();
			void *val;

			for(int i=0; i < cnt; i++) {
				if ((val = cbox->GetClientData(i))) {
					free(val);
				}
			}
			break;
	}

	if (name) {
		free((void *)name);
	}
	if (value) {
		free((void *)value);
	}
	if (data.ptr) {
		objunref(data.ptr);
	}
}

class xml_element *form_item::GetXMLData() {
	class xml_element *ret = NULL;
	objlock(this);
	if ((dtype == DTSGUI_FORM_DATA_XML) && data.xml && objref(data.xml)) {
		ret = data.xml;
	}
	objunlock(this);
	return ret;
}

void *form_item::GetData() {
	void *fp = NULL;

	objlock(this);
	if (data.ptr && objref(data.ptr)) {
		fp = data.ptr;
	}
	objunlock(this);
	return fp;
}

void form_item::Append(const char *text, const char *val) {
	int i;

	switch(type) {
		case DTS_WIDGET_COMBOBOX:
		case DTS_WIDGET_LISTBOX:
			break;
		default:
			return;
	}

	i = widget.l->Append(text, (val) ? (void*)strdup(val) : NULL);

	if ((idx == -1) && value && val && !strcmp(value, val)) {
		idx = i;
		SetSelection(idx);
	} else if (i == 0) {
		SetSelection(0);
	}
}

void form_item::SetSelection(int i) {
	switch(type) {
		case DTS_WIDGET_COMBOBOX:
		case DTS_WIDGET_LISTBOX:
			break;
		default:
			return;
	}

	widget.l->SetSelection(i);
}

const char *form_item::GetName() {
	return name;
}

const char *form_item::GetValue() {
	const char *val = NULL;
	void *tmp;

	switch(type) {
		case DTS_WIDGET_TEXTBOX:
			val = strdup(widget.t->GetValue().ToUTF8());
			break;
		case DTS_WIDGET_LISTBOX:
		case DTS_WIDGET_COMBOBOX:
			int pos;
			pos = widget.l->GetSelection();
			if ((pos !=  wxNOT_FOUND) && widget.l->HasClientData() && (tmp = widget.l->GetClientData(pos))) {
				val = strdup((char*)tmp);
			} else {
				val = strdup(widget.l->GetValue().ToUTF8());
			}
			break;
		case DTS_WIDGET_CHECKBOX:
			if (widget.c->IsChecked()) {
				val = (value) ? strdup(value) : NULL;
			} else {
				val = (value2) ? strdup(value2) : NULL;
			}
			break;
	}
	return val;
}

void *form_item::GetWidget() {
	return widget.p;
}

void form_item::AppendXML(struct xml_doc *xmldoc, const char *xpath, const char *nattr, const char *vattr) {
	struct xml_search *xs;
	struct xml_node *xn;
	void *iter;
	const char *name, *value;

	if (!(xs = xml_xpath(xmldoc, xpath, nattr))) {
		return;
	}

	for(xn = xml_getfirstnode(xs, &iter); xn ; xn = xml_getnextnode(iter)) {
		name = (nattr) ? xml_getattr(xn, nattr) : xn->value;
		value = (vattr) ? xml_getattr(xn, vattr) : xn->value;
		Append(name, value);
		objunref(xn);
	}
	objunref(xs);
	if (iter) {
		objunref(iter);
	}
}
