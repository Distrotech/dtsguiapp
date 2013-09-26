
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

void xmlcheckbox(dtsgui_pane p, const char *name, const char *xpath, const char *node, const char *val, const char *attr, const char *aval) {
	struct xml_doc *xmldoc;
	char *xpth;

	if (!(xmldoc = dtsgui_panelxml(p))) {
		return;
	}

	xpth = createxmlnode(xmldoc, xpath, node, val, attr, aval);

	dtsgui_xmlcheckbox(p, name, (attr && aval) ? aval : node, "true", "", xpth, NULL);
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

void network_dnsconfig(	dtsgui_pane p) {

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
}

void network_config(dtsgui_pane p, struct xml_doc *xmldoc) {
	struct form_item *lb, *elb;
	const char *extint = NULL;

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
}

void network_newiface(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node) {

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


	dtsgui_newxmltreenode(self, p, node, "/config/IP/Interfaces", "Interface", "iface", "name", DTS_NODE_NETWORK_IFACE,
							DTS_TREE_NEW_NODE_CONTAINER | DTS_TREE_NEW_NODE_DELETE | DTS_TREE_NEW_NODE_EDIT | DTS_TREE_NEW_NODE_SORT, NULL);
}

void network_iface(dtsgui_pane p, struct xml_node *xn) {
	const char *xpre = "/config/IP/Interfaces/Interface";
	char xpath[PATH_MAX];

	snprintf(xpath, PATH_MAX, "%s[. = '%s']", xpre, xn->value);

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
}

void network_wifi(dtsgui_pane p, struct xml_node *xn) {
	const char *xpre = "/config/IP/WiFi";
	char xpath[PATH_MAX];
	struct form_item *lb;

	snprintf(xpath, PATH_MAX, "%s[. = '%s']", xpre, xn->value);

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
}

void network_newwifi(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, struct xml_doc *xmldoc) {
	struct form_item *lb;

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

	dtsgui_newxmltreenode(self, p, node, "/config/IP", "WiFi", "iface", NULL, DTS_NODE_NETWORK_WIFI,
							DTS_TREE_NEW_NODE_DELETE | DTS_TREE_NEW_NODE_EDIT | DTS_TREE_NEW_NODE_SORT, NULL);
}

void network_newwan(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node) {

	dtsgui_textbox(p, "Description", "descrip", "", NULL);
	dtsgui_textbox(p, "Network", "network", "", NULL);
	dtsgui_textbox(p, "Subnet Bits", "subnet", "", NULL);
	dtsgui_textbox(p, "Gateway [Local]", "gateway", "", NULL);
	dtsgui_textbox(p, "DHCP Start Address", "dhcpstart", "", NULL);
	dtsgui_textbox(p, "DHCP End Address", "dhcpend", "", NULL);
	dtsgui_textbox(p, "Gateway [Remote]", "remote", "", NULL);

	dtsgui_newxmltreenode(self, p, node, "/config/IP/Routes", "Route", "descrip", NULL, DTS_NODE_NETWORK_WAN,
							DTS_TREE_NEW_NODE_DELETE | DTS_TREE_NEW_NODE_EDIT | DTS_TREE_NEW_NODE_SORT, NULL);
}

void network_wan(dtsgui_pane p, struct xml_node *xn) {
	const char *xpre = "/config/IP/Routes/Route";
	char xpath[PATH_MAX];

	snprintf(xpath, PATH_MAX, "%s[. = '%s']", xpre, xn->value);

	dtsgui_xmltextbox(p, "Network", "network", xpath, "network");
	dtsgui_xmltextbox(p, "Subnet Bits", "subnet", xpath, "subnet");
	dtsgui_xmltextbox(p, "Gateway [Local]", "gateway", xpath, "gateway");
	dtsgui_xmltextbox(p, "DHCP Start Address", "dhcpstart", xpath, "dhcpstart");
	dtsgui_xmltextbox(p, "DHCP End Address", "dhcpend", xpath, "dhcpend");
	dtsgui_xmltextbox(p, "Gateway [Remote]", "remote", xpath, "remote");

	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_newroute(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node) {
	dtsgui_textbox(p, "Description", "descrip", "", NULL);
	dtsgui_textbox(p, "Network", "network", "", NULL);
	dtsgui_textbox(p, "Subnet Bits", "subnet", "", NULL);
	dtsgui_textbox(p, "Gateway", "gateway", "", NULL);

	dtsgui_newxmltreenode(self, p, node, "/config/IP/GenRoutes", "Route", "descrip", NULL, DTS_NODE_NETWORK_ROUTE,
								DTS_TREE_NEW_NODE_DELETE | DTS_TREE_NEW_NODE_EDIT | DTS_TREE_NEW_NODE_SORT, NULL);
}

void network_route(dtsgui_pane p, struct xml_node *xn) {
	const char *xpre = "/config/IP/GenRoutes/Route";
	char xpath[PATH_MAX];

	snprintf(xpath, PATH_MAX, "%s[. = '%s']", xpre, xn->value);

	dtsgui_xmltextbox(p, "Network", "network", xpath, "network");
	dtsgui_xmltextbox(p, "Subnet Bits", "subnet", xpath, "subnet");
	dtsgui_xmltextbox(p, "Gateway", "gateway", xpath, "gateway");

	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_modem(dtsgui_pane p) {
	struct form_item *lb;

	/*XXX LDAP Settings server/DN*/
	if ((lb = xmllistbox(p, "Connection Type", "/config/IP/Dialup", "Option", "Dialup", "option", "Connection"))) {
		dtsgui_listbox_add(lb, "Dialup", "Dialup");
		dtsgui_listbox_add(lb, "Leased", "Leased");
		dtsgui_listbox_add(lb, "ADSL", "ADSL");
		dtsgui_listbox_add(lb, "3G", "3G");
		dtsgui_listbox_add(lb, "3GIPW", "3GIPW");
		objunref(lb);
	}

	if ((lb = xmllistbox(p, "Comm Port ", "/config/IP/Dialup", "Option", "COM1", "option", "ComPort"))) {
		dtsgui_listbox_add(lb, "COM1", "COM1");
		dtsgui_listbox_add(lb, "COM2", "COM2");
		dtsgui_listbox_add(lb, "COM3", "COM3");
		dtsgui_listbox_add(lb, "COM4", "COM4");
		dtsgui_listbox_add(lb, "USB0", "USB0");
		dtsgui_listbox_add(lb, "USB1", "USB1");
		dtsgui_listbox_add(lb, "USB2", "USB2");
		dtsgui_listbox_add(lb, "USB3", "USB3");
		objunref(lb);
	}

	xmltextbox(p, "Holdoff Time", "/config/IP/Dialup", "Option", "", "option", "Holdoff");
	xmltextbox(p, "MTU/MRU", "/config/IP/Dialup", "Option", "", "option", "MTU");

	xmltextbox(p, "Number/Service ID/APN", "/config/IP/Dialup", "Option", "TelkomSA", "opFlowControltion", "Number");
	xmltextbox(p, "Username", "/config/IP/Dialup", "Option", "", "option", "Username");
	xmltextbox(p, "Password", "/config/IP/Dialup", "Option", "", "option", "Password");

	xmlcheckbox(p, "BSD Compression", "/config/IP/Dialup", "Option", "", "option", "BSD");
	xmlcheckbox(p, "Deflate Compression", "/config/IP/Dialup", "Option", "", "option", "Deflate");

	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_modem_ana(dtsgui_pane p) {
	struct form_item *lb;

	/*XXX LDAP Settings server/DN*/
	if ((lb = xmllistbox(p, "Port Speed", "/config/IP/Dialup", "Option", "38400", "option", "Speed"))) {
		dtsgui_listbox_add(lb, "115200", "115200");
		dtsgui_listbox_add(lb, "57600", "57600");
		dtsgui_listbox_add(lb, "38400", "38400");
		dtsgui_listbox_add(lb, "19200", "19200");
		dtsgui_listbox_add(lb, "9600", "9600");
		objunref(lb);
	}

	if ((lb = xmllistbox(p, "Flow Control ", "/config/IP/Dialup", "Option", "crtscts", "option", "FlowControl"))) {
		dtsgui_listbox_add(lb, "Hardware (RTS/CTS)", "crtscts");
		dtsgui_listbox_add(lb, "Software (Xon/Xoff)", "xonxoff");
		dtsgui_listbox_add(lb, "Hardware (DTR/CTS)", "cdtrcts");
		dtsgui_listbox_add(lb, "None", "");
		objunref(lb);
	}

	xmltextbox(p, "Local Address", "/config/IP/Dialup", "Option", "10.0.0.1", "option", "Address");
	xmltextbox(p, "Remote Address", "/config/IP/Dialup", "Option", "10.0.0.2", "option", "Gateway");

	xmltextbox(p, "Init String 1", "/config/IP/Dialup", "Option", "AT&F", "option", "Init1");
	xmltextbox(p, "Init String 2", "/config/IP/Dialup", "Option", "ATL1M1", "option", "Init2");
	xmltextbox(p, "Dial String", "/config/IP/Dialup", "Option", "ATDT", "option", "DialString");
	xmltextbox(p, "Idle Timeout", "/config/IP/Dialup", "Option", "120", "option", "IdleTimeout");
	xmltextbox(p, "Fail Limit", "/config/IP/Dialup", "Option", "5", "option", "MaxFail");
	xmltextbox(p, "Connect Dely", "/config/IP/Dialup", "Option", "", "option", "ConnectDelay");

	xmlcheckbox(p, "Abort On No Carrier", "/config/IP/Dialup", "Option", "true", "option", "NoCarrier");
	xmlcheckbox(p, "Abort On No Dialtone", "/config/IP/Dialup", "Option", "true", "option", "NoDialtone");
	xmlcheckbox(p, "Abort On Error", "/config/IP/Dialup", "Option", "true", "option", "Error");
	xmlcheckbox(p, "Abort On Busy", "/config/IP/Dialup", "Option", "true", "option", "Busy");

	dtsgui_setevcallback(p, handle_test, NULL);
}

void node_edit(dtsgui_treeview tree, dtsgui_treenode node, const char *title) {
	struct xml_doc *xmldoc;
	struct xml_node *xn;
	char *buff = NULL;

	if (!(xmldoc = dtsgui_panelxml(tree))) {
		return;
	}

	if (!(xn = dtsgui_treenodegetxml(tree, node, &buff))) {
		return;
	}

	if (buff) {
		xml_setattr(xmldoc, xn, buff, title);
		objunref(buff);
	} else {
		xml_modify(xmldoc, xn, title);
	}
}

dtsgui_pane tree_do_shit(struct dtsgui *dtsgui, dtsgui_treeview self, dtsgui_treenode node, enum tree_cbtype cb_type, const char *title, void *td) {
	dtsgui_pane p = NULL;
	struct xml_doc *xmldoc;
	struct xml_node *xn = NULL;
	enum node_id nodeid;

	switch(cb_type) {
		case DTSGUI_TREE_CB_SELECT:
			break;
		case DTSGUI_TREE_CB_EDIT:
			node_edit(self, node, title);
			return NULL;
		default:
			return NULL;
	}

	xmldoc = dtsgui_panelxml(self);
	nodeid = dtsgui_treenodeid(self, node);
	xn = dtsgui_treenodegetxml(self, node, NULL);

	if (nodeid < 0) {
		p = dtsgui_treepane(self, title, wx_PANEL_BUTTON_ACTION, NULL, NULL);
		testpanel(p);
		dtsgui_setevcallback(p, handle_test, NULL);
		return p;
	}

	if (!(p = dtsgui_treepane(self, title, wx_PANEL_BUTTON_ACTION, NULL, xmldoc))) {
		return NULL;
	}

	switch(nodeid) {
		case DTS_NODE_DNS_CONFIG:
			network_dnsconfig(p);
			break;
		case DTS_NODE_NETWORK_CONFIG:
			network_config(p, xmldoc);
			break;
		case DTS_NODE_NETWORK_IFACE_NEW:
			network_newiface(p, self, node);
			break;
		case DTS_NODE_NETWORK_IFACE:
			network_iface(p, xn);
			break;
		case DTS_NODE_NETWORK_WIFI_NEW:
			network_newwifi(p, self, node, xmldoc);
			break;
		case DTS_NODE_NETWORK_WIFI:
			network_wifi(p, xn);
			break;
		case DTS_NODE_NETWORK_WAN_NEW:
			network_newwan(p, self, node);
			break;
		case DTS_NODE_NETWORK_WAN:
			network_wan(p, xn);
			break;
		case DTS_NODE_NETWORK_ROUTE_NEW:
			network_newroute(p, self, node);
			break;
		case DTS_NODE_NETWORK_ROUTE:
			network_route(p, xn);
			break;
		case DTS_NODE_NETWORK_MODEM:
			network_modem(p);
			break;
		case DTS_NODE_NETWORK_MODEM_ANA:
			network_modem_ana(p);
			break;
		default:
/*			dtsgui_delpane(p);
			p = NULL;*/
			break;
	}

	if (xmldoc) {
		objunref(xmldoc);
	}

	if (xn) {
		objunref(xn);
	}

	return p;
}


dtsgui_treeview network_tree(struct dtsgui *dtsgui) {
	dtsgui_treeview tree;
	dtsgui_treenode root, tmp, tmp2;
	struct app_data *appdata;
	struct xml_doc *xmldoc;
	struct xml_search *xp;
	struct xml_node *xn;
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

	root = dtsgui_treecont(tree, NULL, "Global IP Settings", 0, 0, 0, DTS_NODE_NETWORK_CONFIG, NULL);
	tmp = dtsgui_treecont(tree, root, "Network Interface", 0, 1, 0, DTS_NODE_NETWORK_IFACE_NEW, NULL);

	xp = xml_xpath(xmldoc, "/config/IP/Interfaces/Interface", "name");
	for(xn = xml_getfirstnode(xp, &iter); xn; xn = xml_getnextnode(iter)) {
		tmp2 = dtsgui_treecont(tree, tmp, xml_getattr(xn, "name"), 1, 1, 1, DTS_NODE_NETWORK_IFACE, NULL);
		dtsgui_treenodesetxml(tree, tmp2, xn, "name");
		objunref(xn);
	}
	objunref(xp);

	tmp = dtsgui_treecont(tree, root, "Wireless Config", 0, 1, 0, DTS_NODE_NETWORK_WIFI_NEW, NULL);

	xp = xml_xpath(xmldoc, "/config/IP/WiFi", NULL);
	for(xn = xml_getfirstnode(xp, &iter); xn; xn = xml_getnextnode(iter)) {
		tmp2 = dtsgui_treecont(tree, tmp, xn->value, 1, 1, 1, DTS_NODE_NETWORK_WIFI, NULL);
		dtsgui_treenodesetxml(tree, tmp2, xn, NULL);
		objunref(xn);
	}
	objunref(xp);

	tmp = dtsgui_treecont(tree, root, "Wan Routing/Nodes", 0, 1, 0, DTS_NODE_NETWORK_WAN_NEW, NULL);
	xp = xml_xpath(xmldoc, "/config/IP/Routes/Route", NULL);
	for(xn = xml_getfirstnode(xp, &iter); xn; xn = xml_getnextnode(iter)) {
		tmp2 = dtsgui_treecont(tree, tmp, xn->value, 1, 1, 1, DTS_NODE_NETWORK_WAN, NULL);
		dtsgui_treenodesetxml(tree, tmp2, xn, NULL);
		objunref(xn);
	}
	objunref(xp);

	tmp = dtsgui_treecont(tree, root, "Other Routes", 0, 1, 0, DTS_NODE_NETWORK_ROUTE_NEW, NULL);
	xp = xml_xpath(xmldoc, "/config/IP/GenRoutes/Route", NULL);
	for(xn = xml_getfirstnode(xp, &iter); xn; xn = xml_getnextnode(iter)) {
		tmp2 = dtsgui_treecont(tree, tmp, xn->value, 1, 1, 1, DTS_NODE_NETWORK_WAN, NULL);
		dtsgui_treenodesetxml(tree, tmp2, xn, NULL);
		objunref(xn);
	}
	objunref(xp);

	tmp = dtsgui_treecont(tree, root, "Modem Config", 0, 0, 0, DTS_NODE_NETWORK_MODEM, NULL);
	dtsgui_treecont(tree, tmp, "Dialup/Leased", 0, 0, 0, DTS_NODE_NETWORK_MODEM_ANA, NULL);
	dtsgui_treecont(tree, root, "Modem Firewall Rules", 0, 1, 0, -1, NULL);
	dtsgui_treecont(tree, root, "Additional ADSL Links", 0, 1, 0, -1, NULL);
	dtsgui_treecont(tree, root, "ADSL Accounts", 0, 1, 0, -1, NULL);
	dtsgui_treecont(tree, root, "Default TOS", 0, 1, 0, -1, NULL);
	dtsgui_treecont(tree, root, "FAX Config", 0, 0, 0, -1, NULL);
	dtsgui_treecont(tree, root, "GRE VPN Tunnels", 0, 1, 0, -1, NULL);
	dtsgui_treecont(tree, root, "ESP VPN Tunnels", 0, 1, 0, -1, NULL);
	dtsgui_treecont(tree, root, "ESP Remote Access", 0, 1, 0, -1, NULL);
	dtsgui_treecont(tree, root, "VOIP Registrations", 0, 0, 0, -1, NULL);
	dtsgui_treecont(tree, root, "Secuity Certificate Config", 0, 0, 0, -1, NULL);

	return tree;
}
