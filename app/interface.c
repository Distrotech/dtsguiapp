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

/** @defgroup DTS-APP-Iface Interface configuration tab view
  * @ingroup DTS-APP
  * @brief Tab view panel allowing configuration and addition of interfaces.
  *
  * @addtogroup DTS-APP-Iface
  * @{
  * @file
  * @brief Interface configuration panel.*/


#include <stdlib.h>
#include <string.h>

#include "dtsgui.h"

#include "private.h"

/** @brief Structure containing treeview and xmldoc for creating new tab.*/
struct new_iface_data {
	/** @brief Tabview to add new tab too.*/
	dtsgui_tabview tv;
	/** @brief XML Document to insert new node into.*/
	struct xml_doc *xmldoc;
};

/** @brief Pretty pointless structure holding interface name*/
struct iface_cdata {
	/** @brief Name of new interface.*/
	const char *name;
};

/** @brief Relase references held by structure before free.
  * @param data Reference to iface_cdata that is about to be freed*/
void free_iface_cdata(void *data) {
	struct iface_cdata *cdata = data;

	if (cdata->name) {
		free((void*)cdata->name);
	}
}

/** @brief Allocate new iface_cdata structure
  * @param val Name of interface.*/
struct iface_cdata *get_iface_cdata(const char *val) {
	struct iface_cdata *cd;

	if (!(cd = objalloc(sizeof(*cd), free_iface_cdata))) {
		return NULL;
	}

	ALLOC_CONST(cd->name, val);
	return cd;
}

/** @brief Release references held by structure before free.
  * @param data Reference to new_iface_data that is about to be freed*/
void free_newiface_data(void *data) {
	struct new_iface_data *nd = data;

	if (nd->xmldoc) {
		objunref(nd->xmldoc);
	}

}

/** @brief Allocate and return a new_iface_data structure.
  *
  * This structure is used to create and insert a new tab in the treeview.
  * @param tv Tabview to allocate new tab too.
  * @param xmldoc XML Document to create new node in.
  * @return Reference to new new_iface_data structure*/
struct new_iface_data *get_newiface_data(dtsgui_tabview tv, struct xml_doc *xmldoc) {
	struct new_iface_data *nd;

	if (!(nd = objalloc(sizeof(*nd), free_newiface_data))) {
		return NULL;
	}

	if (xmldoc && objref(xmldoc)) {
		nd->xmldoc = xmldoc;
	}
	nd->tv = tv;
	return nd;
}


/** @brief Configure panel configuration data
  *
  * As the new interface name is not known till creation time the configuration data needs to be configured.
  * @see dtsgui_tabpane_newdata_cb*/
extern void handle_newxmltabpane(struct xml_doc *xmldoc, struct xml_node *xn, void *tdata, void **cdata, int *last) {
	struct iface_cdata *ndat = NULL;

	if (xn->value && (ndat = get_iface_cdata(xn->value))) {
		cdata[0] = ndat;
	}
}

/** @brief Event handler callback that will only check the interface name and pass handling to default handler
  * @param dtsgui Application data ptr.
  * @param p Panel generating the event.
  * @param type Event type.
  * @param event Event id.
  * @param data Reference to data passed to event handler.
  * @return 0 on error 1 will allow the event to be handled by default handler.*/
extern int handle_updatetabpane(struct dtsgui *dtsgui, dtsgui_pane p, int type, int event, void *data) {
	struct iface_cdata *cdata = data;
	const char *name;

	if (type != wx_PANEL_EVENT_BUTTON) {
		return 1;
	}

	switch(event) {
		case wx_PANEL_EVENT_BUTTON_YES:
			break;
		case wx_PANEL_EVENT_BUTTON_NO:
			return 1;
		default:
			return 1;
	}

	if (!cdata || !(name = dtsgui_findvalue(p, "iface"))) {
		return 0;
	}

	objlock(cdata);
	if (cdata->name) {
		free((void*)cdata->name);
		cdata->name = NULL;
	}
	ALLOC_CONST(cdata->name, name);
	objunlock(cdata);

	if ((name = dtsgui_findvalue(p, "name"))) {
		dtsgui_setstatus(p, name);
		free((void*)name);
	}

	return 1;
}

/** @brief Callback called to configure a interface pane.
  * @param p Panel to configure.
  * @param data Reference to configuration data iface_cdata.*/
void network_iface_pane_cb(dtsgui_pane p, void *data) {
	struct iface_cdata *cdata = data;
	const char *iface;
	const char *xpre = "/config/IP/Interfaces";

	objlock(cdata);
	iface = cdata->name;
	objunlock(cdata);

	dtsgui_xmltextbox(p, "Name", "name", xpre, "Interface", NULL, iface, "name");
	network_iface_pane(p, xpre, iface);
	dtsgui_setevcallback(p, handle_updatetabpane, cdata);
}

/** @brief Callback called for the "New" interface pane.
  * @param p Panel to configure.
  * @param data Reference to configuration data new_iface_data.*/
void network_iface_new_pane_cb(dtsgui_pane p, void *data) {
	struct new_iface_data *nd = data;

	network_iface_new_pane(p);
	/** @remark use dtsgui_newxmltabpane() helper to atach a callback that will create a new panel
	  * @see handle_newxmltabpane()
	  * @see network_iface_pane_cb()*/
	dtsgui_newxmltabpane(nd->tv, p, "/config/IP/Interfaces", "Interface", "iface", "name", handle_newxmltabpane, network_iface_pane_cb, NULL, nd->xmldoc, nd);
}


/** @brief Callback called when the interface configuration link is selected.
  * @param dtsgui Application data ptr.
  * @param title Name assigned to the menu item (not name in menubar).
  * @param data Userdata referenced in menuitem.
  * @return Tabview panel to display to configure interface cards.*/
dtsgui_pane iface_config(struct dtsgui *dtsgui, const char *title, void *data) {
	dtsgui_tabview tabv;
	struct new_iface_data *nd;
	struct xml_doc *xmldoc;
	struct xml_search *xp;
	struct xml_node *xn;
	const char *name;
	struct iface_cdata *cdata;
	void *iter = NULL;

	if (!(xmldoc = app_getxmldoc(dtsgui))) {
		return NULL;
	}

	if (!(xp = xml_xpath(xmldoc, "/config/IP/Interfaces/Interface", "name"))) {
		objunref(xmldoc);
		return NULL;
	}

	tabv = dtsgui_tabwindow(dtsgui, title, NULL);

	for(xn = xml_getfirstnode(xp, &iter); xn; xn = xml_getnextnode(iter)) {
		name = xml_getattr(xn, "name");
		cdata = get_iface_cdata(xn->value);
		dtsgui_newtabpage(tabv, name, wx_PANEL_BUTTON_ACTION, NULL, xmldoc, network_iface_pane_cb, cdata);
		objunref(cdata);
		objunref(xn);
	}

	nd = get_newiface_data(tabv, xmldoc);
	dtsgui_newtabpage(tabv, "Add", wx_PANEL_BUTTON_ACTION, NULL, xmldoc, network_iface_new_pane_cb, nd);
	objunref(nd);

	if (iter) {
		objunref(iter);
	}
	objunref(xp);
	objunref(xmldoc);
	return tabv;
}

/** @}*/
