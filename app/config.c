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

/** @defgroup DTS-APP-Config Configuration menu functions
  * @ingroup DTS-APP
  * @brief Configuration menu functions.
  * @addtogroup DTS-APP-Config
  * @{
  * @file
  * @brief Configuration menu functions.*/

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

#include "dtsgui.h"

#include "private.h"

/** @brief Open XML configuration file callback.
  *
  * Present a file open dialog to the user let them select
  * a configuration file on success enable config menu options
  * and disable file menu options.
  * @param dtsgui Application data ptr.
  * @param title Name supplied to menuitem.
  * @param data Reference to data held by menu callback.
  * @return NULL as no pane is displayed.*/
dtsgui_pane open_config(struct dtsgui *dtsgui, const char *title, void *data) {
	struct app_data *appdata;
	const char *filename;


	if (!(filename = dtsgui_fileopen(dtsgui, "Select Customer Configuration To Open", NULL, "", "XML Configuration|*.xml"))) {
		return NULL;
	}

	if (!(appdata = dtsgui_userdata(dtsgui))) {
		objunref((void*)filename);
		return  NULL;
	}

	objlock(appdata);
	if (!(appdata->xmldoc = xml_loaddoc(filename, 1	))) {
		objunlock(appdata);
		objunref(appdata);
		objunref((void*)filename);
		dtsgui_alert(dtsgui, "Configuration failed to load.\n");
		return NULL;
	}
	objunlock(appdata);

	dtsgui_sendevent(dtsgui, 1);

	objlock(appdata);
	dtsgui_menuitemenable(appdata->e_wiz, 0);
	dtsgui_menuitemenable(appdata->n_wiz, 0);
	dtsgui_menuitemenable(appdata->c_open, 0);
	dtsgui_menuenable(appdata->cfg_menu, 1);
	objunlock(appdata);

	dtsgui_titleappend(dtsgui, filename);
	objunref((void*)filename);

	dtsgui_set_toolbar(dtsgui, 0);
	objunref(appdata);

	return NULL;
}

/** @brief Close/Save menu option call back.
  *
  * This function re enables the file menu items
  * And disables the config menu.
  * @todo Actually save the config
  * @param dtsgui Application data ptr.
  * @param title Name supplied to menuitem.
  * @param data Reference to data held by menu callback.
  * @return NULL as no pane is displayed.*/
dtsgui_pane save_config(struct dtsgui *dtsgui, const char *title, void *data) {
	struct app_data *appdata;

	if (!(appdata = dtsgui_userdata(dtsgui))) {
		return NULL;
	}
	/*	dtsgui_sendevent(dtsgui);*/
	dtsgui_titleappend(dtsgui, NULL);

	objlock(appdata);
	dtsgui_menuenable(appdata->cfg_menu, 0);
	dtsgui_menuitemenable(appdata->e_wiz, 1);
	dtsgui_menuitemenable(appdata->n_wiz, 1);
	dtsgui_menuitemenable(appdata->c_open, 1);

	objunref(appdata->xmldoc);
	appdata->xmldoc = NULL;
	objunlock(appdata);
	objunref(appdata);

	dtsgui_set_toolbar(dtsgui, 1);

	return NULL;
}

/** @brief Display XML document formated in text display.
  *
  * @param dtsgui Application data ptr.
  * @param title Name supplied to menuitem.
  * @param data Reference to data held by menu callback.
  * @return Text Panel.*/
dtsgui_pane view_config_xml(struct dtsgui *dtsgui, const char *title, void *data) {
	struct xml_doc *xmldoc = NULL;
	dtsgui_pane p;
	void *xmlbuf;

	if (!(xmldoc = app_getxmldoc(dtsgui))) {
		return NULL;
	}

	xmlbuf = xml_doctobuffer(xmldoc);
	p = dtsgui_textpane(dtsgui, "XML Configuration", xml_getbuffer(xmlbuf));

	objunref(xmlbuf);
	objunref(xmldoc);

	return p;
}

/** @brief Display XML document formated in text display as a dot conf.
  *
  * Open the XSLT file to transform to .conf and apply it to the XML
  * document placing the output in a text panel.
  * @param dtsgui Application data ptr.
  * @param title Name supplied to menuitem.
  * @param data Reference to data held by menu callback.
  * @return Text Panel.*/
dtsgui_pane view_config_conf(struct dtsgui *dtsgui, const char *title, void *data) {
	struct app_data *appdata;
	struct xml_doc *xmldoc;
	struct xslt_doc *xsltdoc;
	dtsgui_pane p = NULL;
	char xsltfile[PATH_MAX];
	void *xmlbuf;

	if (!(appdata = dtsgui_userdata(dtsgui))) {
		return NULL;
	}
	snprintf(xsltfile, PATH_MAX-1, "%s/xml2conf.xsl", appdata->datadir);
	objunref(appdata);

	if (!is_file(xsltfile)) {
		dtsgui_alert(dtsgui, "XSLT Transform Not Found");
		return NULL;
	}

	if (!(xmldoc = app_getxmldoc(dtsgui))) {
		return NULL;
	}

	if (!(xsltdoc = xslt_open(xsltfile))) {
		dtsgui_alert(dtsgui, "XSLT Open Failed");
		objunref(xmldoc);
		return NULL;
	}

	xmlbuf = xslt_apply_buffer(xmldoc, xsltdoc);
	p = dtsgui_textpane(dtsgui, "Configuration in .conf format", xml_getbuffer(xmlbuf));

	objunref(xsltdoc);
	objunref(xmlbuf);
	objunref(xmldoc);
	return p;
}

/** @brief Export XML document as a .conf file.
  *
  * Openand apply the XSLT xml2conf.xsl and save the result to a file
  * selected in a file dialog.
  * @param dtsgui Application data ptr.
  * @param title Name supplied to menuitem.
  * @param data Reference to data held by menu callback.
  * @return NULL as no panel is displayed.*/
dtsgui_pane export_config(struct dtsgui *dtsgui, const char *title, void *data) {
	struct app_data *appdata;
	struct xml_doc *xmldoc;
	struct xslt_doc *xsltdoc;
	char xsltfile[PATH_MAX];
	const char *newfile;

	if (!(appdata = dtsgui_userdata(dtsgui))) {
		return NULL;
	}
	snprintf(xsltfile, PATH_MAX-1, "%s/xml2conf.xsl", appdata->datadir);
	objunref(appdata);

	if (!is_file(xsltfile)) {
		dtsgui_alert(dtsgui, "XSLT Transform Not Found");
		return NULL;
	}

	if (!(xsltdoc = xslt_open(xsltfile))) {
		dtsgui_alert(dtsgui, "XSLT Open Failed");
		return NULL;
	}

	if (!(xmldoc = app_getxmldoc(dtsgui))) {
		objunref(xsltdoc);
		return NULL;
	}

	newfile = dtsgui_filesave(dtsgui, "Export Config To File", NULL, "firewall.conf", "System Configuration (.conf)|*.conf");
	if (newfile) {
		xslt_apply(xmldoc, xsltdoc, newfile, 0);
		objunref((void*)newfile);
	}

	objunref(xsltdoc);
	objunref(xmldoc);
	return NULL;
}


/** @brief Add the configure menu to the menu bar and add options.
    @param dtsgui Application data ptr.*/
void config_menu(struct dtsgui *dtsgui) {
	struct app_data *appdata;

	if (!(appdata = dtsgui_userdata(dtsgui))) {
		return;
	}

	objlock(appdata);
	appdata->cfg_menu = dtsgui_newmenu(dtsgui, "&Config");

	dtsgui_newmenucb(appdata->cfg_menu, dtsgui, "Reconfigure &Wizard", "Run System Reconfigure Wizard.", 1, reconfig_wizard, NULL);

	dtsgui_newmenucb(appdata->cfg_menu, dtsgui, "P&BX Configuration", "PBX Setup", 0, pbx_settings, NULL);
	dtsgui_newmenucb(appdata->cfg_menu, dtsgui, "&Interface Configuration", "Inteface Configuration", 0, iface_config, NULL);
	dtsgui_newmenucb(appdata->cfg_menu, dtsgui, "&Advanced Configuration", "Advanced Config", 0, advanced_config, NULL);

	dtsgui_menusep(appdata->cfg_menu);
	dtsgui_newmenucb(appdata->cfg_menu, dtsgui, "&Save And Close Config", "Save/Close System Config (File/URL)", 1, save_config, NULL);
	dtsgui_newmenucb(appdata->cfg_menu, dtsgui, "E&xport Config", "Export Configuration as a .conf file", 0, export_config, NULL);

	dtsgui_menusep(appdata->cfg_menu);
	dtsgui_newmenucb(appdata->cfg_menu, dtsgui, "&View Config (XML)", "View Current Config File In XML Format.", 0, view_config_xml, NULL);
	dtsgui_newmenucb(appdata->cfg_menu, dtsgui, "View &Config (CONF)", "View Current Config File Converted to .conf Format.", 0, view_config_conf, NULL);

	/*initially greyed out*/
	dtsgui_menuenable(appdata->cfg_menu, 0);
	objunlock(appdata);
	objunref(appdata);
}

/** @}*/
