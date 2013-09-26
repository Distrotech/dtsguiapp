
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


char *createxmlnode(struct xml_doc *xmldoc, const char *xpath, const char *node, const char *val, const char *attr, const char *aval) {
	struct xml_search *xp;
	char *xpth;
	int len;

	len = strlen(xpath) + strlen(node) + 10;

	if (attr) {
		len+=strlen(aval)+strlen(attr);
		xpth=malloc(len);
		snprintf(xpth, PATH_MAX, "%s/%s[@%s = '%s']", xpath, node, attr, aval);
	} else {
		len+=strlen(val);
		xpth=malloc(len);
		snprintf(xpth, len, "%s/%s[. = '%s']", xpath, node, val);
	}

	if (!(xp = xml_xpath(xmldoc, xpth, attr))) {
		xml_addnode(xmldoc, xpath, node, val, attr, aval);
	} else {
		objunref(xp);
	}
	return xpth;
}


void xmltextbox(dtsgui_pane p, const char *name, const char *xpath, const char *node, const char *val, const char *attr, const char *aval) {
	struct xml_doc *xmldoc;
	char *xpth;

	if (!(xmldoc = dtsgui_panelxml(p))) {
		return;
	}

	xpth = createxmlnode(xmldoc, xpath, node, val, attr, aval);

	dtsgui_xmltextbox(p, name, (attr && aval) ? aval : node, xpth, NULL);
	free(xpth);
}

struct form_item *xmllistbox(dtsgui_pane p, const char *name, const char *xpath, const char *node, const char *val, const char *attr, const char *aval) {
	struct xml_doc *xmldoc;
	struct form_item *lb;
	char *xpth;

	if (!(xmldoc = dtsgui_panelxml(p))) {
		return NULL;
	}

	xpth = createxmlnode(xmldoc, xpath, node, val, attr, aval);

	lb = dtsgui_xmllistbox(p, name, (attr && aval) ? aval : node, xpth, NULL);
	free(xpth);
	return lb;
}


dtsgui_pane network_dnsconfig(struct dtsgui *dtsgui, dtsgui_treeview self, const char *title, struct tree_data *td, struct xml_doc *xmldoc) {
	dtsgui_pane p;

	p = dtsgui_treepane(self, title, wx_PANEL_BUTTON_ACTION, td, xmldoc);

	xmltextbox(p, "Primary DNS", "/config/DNS/Config", "Option", "", "option", "");
	xmltextbox(p, "Secondary DNS", "/config/DNS/Config", "Option", "", "option", "NattedIP");
	xmltextbox(p, "Primary WINS", "/config/DNS/Config", "Option", "", "option", "L2TPNet");
	xmltextbox(p, "Secondary WINS", "/config/DNS/Config", "Option", "", "option", "OVPNNet");
/*	xmltextbox(p, "", "/config/IP/SysConf", "Option", "", "option", "");
	xmltextbox(p, "", "/config/IP/SysConf", "Option", "", "option", "");
	xmltextbox(p, "", "/config/IP/SysConf", "Option", "", "option", "");
	xmltextbox(p, "", "/config/IP/SysConf", "Option", "", "option", "");
	xmltextbox(p, "", "/config/IP/SysConf", "Option", "", "option", "");
	xmltextbox(p, "", "/config/IP/SysConf", "Option", "", "option", "");
	xmltextbox(p, "", "/config/IP/SysConf", "Option", "", "option", "");
	xmltextbox(p, "", "/config/IP/SysConf", "Option", "", "option", "");*/

	dtsgui_setevcallback(p, handle_test, NULL);
	return p;
}

dtsgui_pane network_config(struct dtsgui *dtsgui, dtsgui_treeview self, const char *title, struct tree_data *td, struct xml_doc *xmldoc) {
	dtsgui_pane p;
	struct form_item *lb, *elb;
	const char *extint = NULL;

	p = dtsgui_treepane(self, title, wx_PANEL_BUTTON_ACTION, td, xmldoc);


	/*XXX LDAP Settings server/DN*/
	if ((lb = xmllistbox(p, "Internal Interface", "/config/IP/SysConf", "Option", "", "option", "Internal"))) {
		dtsgui_listbox_addxml(lb, xmldoc, "/config/IP/Interfaces/Interface", "name", NULL);
		objunref(lb);
	}
	if ((elb = xmllistbox(p, "External Interface", "/config/IP/SysConf", "Option", "", "option", "External"))) {
		dtsgui_listbox_add(elb, "Modem", "Dialup");
		dtsgui_listbox_addxml(elb, xmldoc, "/config/IP/Interfaces/Interface", "name", NULL);
		extint = dtsgui_item_value(elb);
		objunref(elb);
	}
	if ((lb = xmllistbox(p, "External OVPN Interface", "/config/IP/SysConf", "Option", (extint) ? extint : "", "option", "OVPNNet"))) {
		dtsgui_listbox_add(lb, "Modem", "Dialup");
		dtsgui_listbox_addxml(lb, xmldoc, "/config/IP/Interfaces/Interface", "name", NULL);
		objunref(lb);
	}

	if (extint) {
		free((void*)extint);
	}

	xmltextbox(p, "Primary DNS", "/config/IP/SysConf", "Option", "", "option", "PrimaryDns");
	xmltextbox(p, "Secondary DNS", "/config/IP/SysConf", "Option", "", "option", "SecondaryDns");
	xmltextbox(p, "Primary WINS", "/config/IP/SysConf", "Option", "", "option", "PrimaryWins");
	xmltextbox(p, "Secondary WINS", "/config/IP/SysConf", "Option", "", "option", "SecondaryWins");
	xmltextbox(p, "Gateway Address", "/config/IP/SysConf", "Option", "", "option", "Nexthop");
	xmltextbox(p, "External (Natted IP)", "/config/IP/SysConf", "Option", "", "option", "NattedIP");
	xmltextbox(p, "IPSEC VPN Access", "/config/IP/SysConf", "Option", "", "option", "VPNNet");
	xmltextbox(p, "Open VPN Access", "/config/IP/SysConf", "Option", "", "option", "OVPNNet");
	xmltextbox(p, "L2TP VPN Access", "/config/IP/SysConf", "Option", "", "option", "L2TPNet");
	xmltextbox(p, "DHCP Lease", "/config/IP/SysConf", "Option", "43200", "option", "DHCPLease");
	xmltextbox(p, "DHCP Max Lease", "/config/IP/SysConf", "Option", "86400", "option", "DHCPMaxLease");
	xmltextbox(p, "NTP Server[s]", "/config/IP/SysConf", "Option", "196.25.1.1 196.25.1.9", "option", "NTPServer");
	xmltextbox(p, "Bridge Interfaces", "/config/IP/SysConf", "Option", "ethA", "option", "Bridge");
	xmltextbox(p, "Incoming Traffic Limit", "/config/IP/SysConf", "Option", "", "option", "Ingress");
	xmltextbox(p, "Outgoing Traffic Limit", "/config/IP/SysConf", "Option", "", "option", "Egress");

	dtsgui_setevcallback(p, handle_test, NULL);
	return p;
}

void handle_newnet(dtsgui_pane p, int type, int event, void *data) {
	struct tree_data *td = data, *n_td;
	struct xml_node *xn;
	const char *name;

	switch(event) {
		case wx_PANEL_BUTTON_YES:
			break;
		default:
			return;
	}

	if (!td || !(xn = dtsgui_panetoxml(p, "/config/IP/Interfaces", "Interface", "iface", "name"))) {
		return;
	}

	n_td = gettreedata(td->tree, xn, "name", DTS_NODE_NETWORK_IFACE);
	name = xml_getattr(xn, "name");
	n_td->treenode = dtsgui_treecont(td->tree, td->treenode, name, 1, 1, 1, n_td);

	objunref(n_td);
}


dtsgui_pane network_newiface(struct dtsgui *dtsgui, dtsgui_treeview self, const char *title, struct tree_data *td, struct xml_doc *xmldoc) {
	dtsgui_pane p;

	p = dtsgui_treepane(self, title, wx_PANEL_BUTTON_ACTION, NULL, xmldoc);

	dtsgui_textbox(p, "Interface", "iface", "", NULL);
	dtsgui_textbox(p, "Name", "name", "", NULL);
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

dtsgui_pane network_wifi(struct dtsgui *dtsgui, dtsgui_treeview self, const char *title, struct tree_data *td, struct xml_doc *xmldoc) {
	struct xml_node *xn = td->node;
	const char *xpre = "/config/IP/WiFi";
	char xpath[PATH_MAX];
	struct form_item *lb;
	dtsgui_pane p;

	snprintf(xpath, PATH_MAX, "%s[. = '%s']", xpre, xn->value);
	p = dtsgui_treepane(self, title, wx_PANEL_BUTTON_ACTION, td, xmldoc);

	lb = dtsgui_xmlcombobox(p, "WiFi Configuration", "type", xpath, "type");
	dtsgui_listbox_add(lb, "Access Point", "AP");
	dtsgui_listbox_add(lb, "WiFi Client", "Client");
	dtsgui_listbox_add(lb, "Hotspot", "Hotspot");
	objunref(lb);

	lb = dtsgui_xmlcombobox(p, "WiFi Mode", "mode", xpath, "mode");
	dtsgui_listbox_add(lb, "802.11a", "0");
	dtsgui_listbox_add(lb, "802.11b [ch 1/6/11/14]", "1");
	dtsgui_listbox_add(lb, "802.11g [ch 1/5/9/13]", "2");
	dtsgui_listbox_add(lb, "802.11n 20Mhz [ch 1/5/9/13]", "3");
	dtsgui_listbox_add(lb, "802.11n 40Mhz [ch 3/11]", "4");
	objunref(lb);

	lb = dtsgui_xmlcombobox(p, "WiFi Auth Type", "auth", xpath, "auth");
	dtsgui_listbox_add(lb, "WiFi Protected Access", "WPA");
	dtsgui_listbox_add(lb, "Extensible Authentication Protocol", "EAP");
	dtsgui_listbox_add(lb, "None", "None");
	objunref(lb);

	dtsgui_xmltextbox(p, "Channel", "channel", xpath, "channel");
	dtsgui_xmltextbox(p, "Key (WPA)", "key", xpath, "key");
	dtsgui_xmltextbox(p, "Regulatory Domain", "regdom", xpath, "regdom");
	dtsgui_xmltextbox(p, "TX Power", "txpower", xpath, "txpower");

	dtsgui_setevcallback(p, handle_test, NULL);
	return p;
}

void handle_newwifi(dtsgui_pane p, int type, int event, void *data) {
	struct tree_data *td = data, *n_td;
	struct xml_node *xn;

	switch(event) {
		case wx_PANEL_BUTTON_YES:
			break;
		default:
			return;
	}

	if (!td || !(xn = dtsgui_panetoxml(p, "/config/IP", "WiFi", "iface", NULL))) {
		return;
	}

	n_td = gettreedata(td->tree, xn, NULL, DTS_NODE_NETWORK_WIFI);
	n_td->treenode = dtsgui_treecont(td->tree, td->treenode, xn->value, 1, 1, 1, n_td);

	objunref(n_td);
}

dtsgui_pane network_newwifi(struct dtsgui *dtsgui, dtsgui_treeview self, const char *title, struct tree_data *td, struct xml_doc *xmldoc) {
	dtsgui_pane p;
	struct form_item *lb;

	p = dtsgui_treepane(self, title, wx_PANEL_BUTTON_ACTION, NULL, xmldoc);

	lb = dtsgui_listbox(p, "WiFi Interface", "iface", NULL);
	dtsgui_listbox_addxml(lb, xmldoc, "/config/IP/Interfaces/Interface", "name", NULL);
	objunref(lb);

	lb = dtsgui_listbox(p, "WiFi Configuration", "type", NULL);
	dtsgui_listbox_add(lb, "Access Point", "AP");
	dtsgui_listbox_add(lb, "WiFi Client", "Client");
	dtsgui_listbox_add(lb, "Hotspot", "Hotspot");
	dtsgui_listbox_set(lb, 0);
	objunref(lb);

	lb = dtsgui_listbox(p, "WiFi Mode", "mode", NULL);
	dtsgui_listbox_add(lb, "802.11a", "0");
	dtsgui_listbox_add(lb, "802.11b [ch 1/6/11/14]", "1");
	dtsgui_listbox_add(lb, "802.11g [ch 1/5/9/13]", "2");
	dtsgui_listbox_add(lb, "802.11n 20Mhz [ch 1/5/9/13]", "3");
	dtsgui_listbox_add(lb, "802.11n 40Mhz [ch 3/11]", "4");
	dtsgui_listbox_set(lb, 2);
	objunref(lb);

	lb = dtsgui_listbox(p, "WiFi Auth Type", "auth", NULL);
	dtsgui_listbox_add(lb, "WiFi Protected Access", "WPA");
	dtsgui_listbox_add(lb, "Extensible Authentication Protocol", "EAP");
	dtsgui_listbox_add(lb, "None", "None");
	dtsgui_listbox_set(lb, 0);
	objunref(lb);

	dtsgui_textbox(p, "Channel", "channel", "", NULL);
	dtsgui_textbox(p, "Key (WPA)", "key", "", NULL);
	dtsgui_textbox(p, "Regulatory Domain", "regdom", "ZA", NULL);
	dtsgui_textbox(p, "TX Power", "txpower", "25", NULL);

	dtsgui_setevcallback(p, handle_newwifi, td);
	return p;
}

void node_edit(dtsgui_treeview tree, struct tree_data *td, const char *title) {
	struct xml_doc *xmldoc;

	if (!td->node) {
		return;
	}

	xmldoc = dtsgui_panelxml(tree);

	if (td->tattr) {
		xml_setattr(xmldoc, td->node, td->tattr, title);
	} else {
		xml_modify(xmldoc, td->node, title);
	}
}

void tree_do_shit(struct dtsgui *dtsgui, dtsgui_treeview self, enum tree_cbtype cb_type, const char *title, void *tdata, void *ndata) {
	dtsgui_pane p = NULL;
	struct xml_doc *xmldoc;
	struct tree_data *td = ndata;


	switch(cb_type) {
		case DTSGUI_TREE_CB_SELECT:
			break;
		case DTSGUI_TREE_CB_EDIT:
			node_edit(self, td, title);
			return;
		default:
			return;
	}

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
		case DTS_NODE_DNS_CONFIG:
			p = network_dnsconfig(dtsgui, self, title, td, xmldoc);
			break;
		case DTS_NODE_NETWORK_CONFIG:
			p = network_config(dtsgui, self, title, td, xmldoc);
			break;
		case DTS_NODE_NETWORK_IFACE_NEW:
			p = network_newiface(dtsgui, self, title, td, xmldoc);
			break;
		case DTS_NODE_NETWORK_IFACE:
			p = network_iface(dtsgui, self, title, td, xmldoc);
			break;
		case DTS_NODE_NETWORK_WIFI_NEW:
			p = network_newwifi(dtsgui, self, title, td, xmldoc);
			break;
		case DTS_NODE_NETWORK_WIFI:
			p = network_wifi(dtsgui, self, title, td, xmldoc);
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

/*	td = gettreedata(tree, NULL, DTS_NODE_DNS_CONFIG);
	root = dtsgui_treecont(tree, NULL, "Global DNS Settings", 0, 0, 0, td);*/

	td = gettreedata(tree, NULL, NULL, DTS_NODE_NETWORK_CONFIG);
	root = dtsgui_treecont(tree, NULL, "Global IP Settings", 0, 0, 0, td);

	td = gettreedata(tree, NULL, NULL, DTS_NODE_NETWORK_IFACE_NEW);
	tmp = dtsgui_treecont(tree, root, "Network Interface", 0, 1, 0, td);
	td->treenode = tmp;
	objunref(td);

	xp = xml_xpath(xmldoc, "/config/IP/Interfaces/Interface", "name");
	for(xn = xml_getfirstnode(xp, &iter); xn; xn = xml_getnextnode(iter)) {
		td = gettreedata(tree, xn, "name", DTS_NODE_NETWORK_IFACE);
		td->treenode = dtsgui_treecont(tree, tmp, xml_getattr(xn, "name"), 1, 1, 1, td);
		objunref(td);
		objunref(xn);
	}
	objunref(xp);


	td = gettreedata(tree, NULL, NULL, DTS_NODE_NETWORK_WIFI_NEW);
	tmp = dtsgui_treecont(tree, root, "Wireless Config", 0, 1, 0, td);
	td->treenode = tmp;
	objunref(td);

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
