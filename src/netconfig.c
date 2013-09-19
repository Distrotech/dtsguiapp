
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#ifdef __WIN32
#define UNICODE 1
#include <winsock2.h>
#include <dirent.h>
#include <shlobj.h>
#endif

#include <dtsapp.h>

#include "dtsgui.h"
#include "private.h"

#ifndef DATA_DIR
#define DATA_DIR	"/usr/share/dtsguiapp"
#endif

void handle_newnet(dtsgui_pane p, int type, int event, void *data) {
	const char *attrs[] = {"bwout", "gateway", "macaddr", "dhcpend", "dhcpstart", "subnet", "ipaddr", "bwin"};
	struct xml_doc *xmldoc;
	struct tree_data *td = data, *n_td;
	struct xml_node *xn;
	const char *iface,*name, *tmp;
	int cnt;

	switch(event) {
		case wx_PANEL_BUTTON_YES:
			break;
		default:
			return;
	}

	if (!td || !(xmldoc = dtsgui_panelxml(p))) {
		return;
	}

	iface = dtsgui_findvalue(p, "iface");
	name = dtsgui_findvalue(p, "name");
	xn = xml_addnode(xmldoc, "/config/IP/Interfaces", "Interface", iface, "name", name);
	n_td = gettreedata(td->tree, xn, DTS_NODE_NETWORK_IFACE);
	n_td->treenode = dtsgui_treecont(td->tree, td->treenode, name, 1, 1, 1, n_td);
	for(cnt = 0; cnt < 8;cnt++) {
		if ((tmp = dtsgui_findvalue(p, attrs[cnt]))) {
			xml_setattr(xmldoc, xn, attrs[cnt], tmp);
			free((void*)tmp);
		}
	}
	free((void*)iface);
	free((void*)name);


	objunref(n_td);
	objunref(xmldoc);
}


dtsgui_pane network_newiface(struct dtsgui *dtsgui, dtsgui_treeview self, const char *title, struct tree_data *td, struct xml_doc *xmldoc) {
	dtsgui_pane p;

	p = dtsgui_treepane(self, title, wx_PANEL_BUTTON_ACTION, NULL, xmldoc);

	dtsgui_textbox(p, "Interface", "iface", "", NULL);
	dtsgui_textbox(p, "Interface", "name", "", NULL);
	dtsgui_textbox(p, "IP Address", "ipaddr", "0", NULL);
	dtsgui_textbox(p, "IP Subnet Bits", "subnet", "32", NULL);
	dtsgui_textbox(p, "DHCP Gateway", "gateway", "", NULL);
	dtsgui_textbox(p, "MAC Address", "macaddr", "00:00:00:00:00:00", NULL);
	dtsgui_textbox(p, "DHCP Start Address", "dhcpstart", "-", NULL);
	dtsgui_textbox(p, "DHCP End Address", "dhcpend", "-", NULL);
	dtsgui_textbox(p, "Bandwidth In", "bwin", "", NULL);
	dtsgui_textbox(p, "Bandwidth Out", "bwout", "", NULL);

	dtsgui_setevcallback(p, handle_newnet, td);
	return p;
}

dtsgui_pane network_iface(struct dtsgui *dtsgui, dtsgui_treeview self, const char *title, struct tree_data *td, struct xml_doc *xmldoc) {
	struct xml_node *xn = td->node;
	const char *xpre = "/config/IP/Interfaces/Interface";
	char xpath[PATH_MAX];
	dtsgui_pane p;

	snprintf(xpath, PATH_MAX, "%s[. = '%s']", xpre, xn->value);
	p = dtsgui_treepane(self, title, wx_PANEL_BUTTON_ACTION, td, xmldoc);

	dtsgui_xmltextbox(p, "Interface", "iface", xpath, NULL);
	dtsgui_xmltextbox(p, "IP Address", "ipaddr", xpath, "ipaddr");
	dtsgui_xmltextbox(p, "IP Subnet Bits", "subnet", xpath, "subnet");
	dtsgui_xmltextbox(p, "DHCP Gateway", "gateway", xpath, "gateway");
	dtsgui_xmltextbox(p, "MAC Address", "macaddr", xpath, "macaddr");
	dtsgui_xmltextbox(p, "DHCP Start Address", "dhcpstart", xpath, "dhcpstart");
	dtsgui_xmltextbox(p, "DHCP End Address", "dhcpend", xpath, "dhcpend");
	dtsgui_xmltextbox(p, "Bandwidth In", "bwin", xpath, "bwin");
	dtsgui_xmltextbox(p, "Bandwidth Out", "bwout", xpath, "bwout");

	dtsgui_setevcallback(p, handle_test, NULL);
	return p;
}

void tree_do_shit(struct dtsgui *dtsgui, dtsgui_treeview self, const char *title, void *tdata, void *ndata) {
	dtsgui_pane p = NULL;
	struct xml_doc *xmldoc;
	struct tree_data *td = ndata;


	if (!td) {
		p = dtsgui_treepane(self, title, wx_PANEL_BUTTON_ACTION, NULL, NULL);
		testpanel(p);
		dtsgui_setevcallback(p, handle_test, NULL);
		dtsgui_treeshow(self, p);
		if (ndata) {
			objunref(ndata);
		}
		return;
	}

	xmldoc = dtsgui_panelxml(self);

	switch(td->nodeid) {
		case DTS_NODE_NETWORK_NEW:
			p = network_newiface(dtsgui, self, title, td, xmldoc);
			break;
		case DTS_NODE_NETWORK_IFACE:
			p = network_iface(dtsgui, self, title, td, xmldoc);
			break;
		default:
			break;
	}

	if (xmldoc) {
		objunref(xmldoc);
	}
	if (ndata) {
		objunref(ndata);
	}
	if (tdata) {
		objunref(tdata);
	}
	if (p) {
		dtsgui_treeshow(self, p);
	}
}


dtsgui_treeview network_tree(struct dtsgui *dtsgui) {
	dtsgui_treeview tree;
	dtsgui_treenode root, tmp;
	struct app_data *appdata;
	struct xml_doc *xmldoc;
	struct xml_search *xp;
	struct xml_node *xn;
	struct tree_data *td;
	void *iter;
	char defconf[PATH_MAX];

	appdata = dtsgui_userdata(dtsgui);
	snprintf(defconf, PATH_MAX-1, "%s/default.xml", appdata->datadir);
	if (!is_file(defconf)) {
		dtsgui_alert(dtsgui, "Default configuration not found.\nCheck Installation.");
		return 0;
	}

	if (!(xmldoc = xml_loaddoc(defconf, 1))) {
		dtsgui_alert(dtsgui, "Default configuration failed to load.\nCheck Installation.");
		return 0;
	}

	tree = dtsgui_treewindow(dtsgui, "Tree Window", tree_do_shit, NULL, xmldoc);
	root = dtsgui_treecont(tree, NULL, "Global Settings", 0, 0, 0, NULL);

	td = gettreedata(tree, NULL, DTS_NODE_NETWORK_NEW);
	tmp = dtsgui_treecont(tree, root, "Network Interface", 0, 1, 0, td);
	td->treenode = tmp;
	objunref(td);

	xp = xml_xpath(xmldoc, "/config/IP/Interfaces/Interface", "name");
	for(xn = xml_getfirstnode(xp, &iter); xn; xn = xml_getnextnode(iter)) {
		td = gettreedata(tree, xn, DTS_NODE_NETWORK_IFACE);
		td->treenode = dtsgui_treecont(tree, tmp, xml_getattr(xn, "name"), 1, 1, 1, td);
		objunref(td);
		objunref(xn);
	}
	objunref(xp);

	dtsgui_treecont(tree, root, "Wireless Config", 0, 1, 0, NULL);
	dtsgui_treecont(tree, root, "Wan Routing/Nodes", 0, 1, 0, NULL);
	dtsgui_treecont(tree, root, "Other Routes", 0, 1, 0, NULL);
	dtsgui_treecont(tree, root, "Modem Config", 0, 0, 0, NULL);
	dtsgui_treecont(tree, root, "Modem Firewall Rules", 0, 1, 0, NULL);
	dtsgui_treecont(tree, root, "Additional ADSL Links", 0, 1, 0, NULL);
	dtsgui_treecont(tree, root, "ADSL Accounts", 0, 1, 0, NULL);
	dtsgui_treecont(tree, root, "Default TOS", 0, 1, 0, NULL);
	dtsgui_treecont(tree, root, "FAX Config", 0, 0, 0, NULL);
	dtsgui_treecont(tree, root, "GRE VPN Tunnels", 0, 1, 0, NULL);
	dtsgui_treecont(tree, root, "ESP VPN Tunnels", 0, 1, 0, NULL);
	dtsgui_treecont(tree, root, "ESP Remote Access", 0, 1, 0, NULL);
	dtsgui_treecont(tree, root, "VOIP Registrations", 0, 0, 0, NULL);
	dtsgui_treecont(tree, root, "Secuity Certificate Config", 0, 0, 0, NULL);

	return tree;
}
