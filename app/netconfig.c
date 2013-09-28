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


void network_config_dns_domain_server_new(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data);

static const char *protocols[] = {"TCP", "UDP", "GRE", "ESP", "AH", "OSPF", "ALL"};

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

void network_config(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	struct xml_doc *xmldoc;
	struct form_item *lb, *elb;
	const char *extint = NULL;

	xmldoc = dtsgui_panelxml(p);

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

	xmltextbox(p, "Gateway Address", "/config/IP/SysConf", "Option", "", "option", "Nexthop");
	xmltextbox(p, "External (Natted IP)", "/config/IP/SysConf", "Option", "", "option", "NattedIP");
	xmltextbox(p, "IPSEC VPN Access", "/config/IP/SysConf", "Option", "", "option", "VPNNet");
	xmltextbox(p, "Open VPN Access", "/config/IP/SysConf", "Option", "", "option", "OVPNNet");
	xmltextbox(p, "L2TP VPN Access", "/config/IP/SysConf", "Option", "", "option", "L2TPNet");
	xmltextbox(p, "NTP Server[s]", "/config/IP/SysConf", "Option", "196.25.1.1 196.25.1.9", "option", "NTPServer");
	xmltextbox(p, "Bridge Interfaces", "/config/IP/SysConf", "Option", "ethA", "option", "Bridge");
	xmltextbox(p, "Incoming Traffic Limit", "/config/IP/SysConf", "Option", "", "option", "Ingress");
	xmltextbox(p, "Outgoing Traffic Limit", "/config/IP/SysConf", "Option", "", "option", "Egress");

	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_config_dns(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	xmltextbox(p, "Hostname", "/config/DNS/Config", "Option", "netsentry", "option", "Hostname");
	xmltextbox(p, "Domain", "/config/DNS/Config", "Option", "company.co.za", "option", "Domain");
	xmltextbox(p, "Aditional Search Domains", "/config/DNS/Config", "Option", "", "option", "Search");
	xmltextbox(p, "Primary DNS", "/config/IP/SysConf", "Option", "", "option", "PrimaryDns");
	xmltextbox(p, "Secondary DNS", "/config/IP/SysConf", "Option", "", "option", "SecondaryDns");
	xmltextbox(p, "Primary WINS", "/config/IP/SysConf", "Option", "", "option", "PrimaryWins");
	xmltextbox(p, "Secondary WINS", "/config/IP/SysConf", "Option", "", "option", "SecondaryWins");
	xmltextbox(p, "DHCP Lease", "/config/IP/SysConf", "Option", "43200", "option", "DHCPLease");
	xmltextbox(p, "DHCP Max Lease", "/config/IP/SysConf", "Option", "86400", "option", "DHCPMaxLease");
	xmlcheckbox(p, "Use Internal Interface For DNS First", "/config/DNS/Config", "Option", "true", "option", "IntFirst");

	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_config_dns_dyn(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	xmltextbox(p, "Dynamic DNS Domain", "/config/DNS/Config", "Option", "", "option", "DynZone");
	xmltextbox(p, "Dynamic DNS Server", "/config/DNS/Config", "Option", "", "option", "DynServ");
	xmltextbox(p, "Dynamic DNS Secret", "/config/DNS/Config", "Option", "", "option", "DynKey");
	xmltextbox(p, "Dynamic Zone TTL", "/config/DNS/Config", "Option", "180", "option", "DynamicTTL");
	xmlcheckbox(p, "Add CNAME For host", "/config/DNS/Config", "Option", "true", "option", "DynamicCNAME");
	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_config_dns_serv(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	xmltextbox(p, "Shared Secret For Domain Updates", "/config/DNS/Config", "Option", "", "option", "SmartKey");

	xmlcheckbox(p, "Backup Zone Files", "/config/DNS/Config", "Option", "true", "option", "Backup");
	xmlcheckbox(p, "Authorotive [Internal]", "/config/DNS/Config", "Option", "true", "option", "Auth");
	xmlcheckbox(p, "Authorotive [External]", "/config/DNS/Config", "Option", "", "option", "AuthX");
	xmlcheckbox(p, "Use DNS Servers Obtained From PPP", "/config/DNS/Config", "Option", "true", "option", "Usepeer");
	xmlcheckbox(p, "Allow Recursive Lookup Externally", "/config/DNS/Config", "Option", "", "option", "ExtServ");
	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_config_dns_zone(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	xmltextbox(p, "SOA Serial", "/config/DNS/Config", "Option", "1", "option", "Serial");
	xmltextbox(p, "SOA Refresh", "/config/DNS/Config", "Option", "3600", "option", "Refresh");
	xmltextbox(p, "SOA Retry", "/config/DNS/Config", "Option", "1800", "option", "Retry");
	xmltextbox(p, "SOA Expire", "/config/DNS/Config", "Option", "604800", "option", "Expire");
	xmltextbox(p, "Default TTL", "/config/DNS/Config", "Option", "3600", "option", "DefaultTTL");

	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_config_dns_host(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	const char *xpre = "/config/DNS/Hosts/Host";
	char xpath[PATH_MAX];
	struct xml_node *xn;

	xn = dtsgui_treenodegetxml(self, node, NULL);

	snprintf(xpath, PATH_MAX, "%s[. = '%s']", xpre, xn->value);

	dtsgui_xmltextbox(p, "IP Address", "ipaddr", xpath, "ipaddr");
	dtsgui_xmltextbox(p, "MAC Address", "macaddr", xpath, "macaddr");

	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_config_dns_host_new(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	dtsgui_textbox(p, "Hostame", "host", "", NULL);
	dtsgui_textbox(p, "IP Address", "ipaddr", "", NULL);
	dtsgui_textbox(p, "Mac Address", "macaddr", "", NULL);

 	dtsgui_newxmltreenode(self, p, node, "/config/DNS/Hosts", "Host", "host", "host", DTS_NODE_NETWORK_CONFIG_DNS_HOST,
							DTS_TREE_NEW_NODE_DELETE | DTS_TREE_NEW_NODE_EDIT | DTS_TREE_NEW_NODE_SORT, NULL, NULL, network_config_dns_host);
}

void network_config_dns_domain_node(dtsgui_treeview tv, dtsgui_treenode tn, struct xml_node *xn, void *data) {
	dtsgui_treecont(tv, tn, "Servers", 0, 1, 0, DTS_NODE_NETWORK_CONFIG_DNS_DOMAIN_SERVER_NEW, network_config_dns_domain_server_new, NULL);
}

void network_config_dns_domain(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	const char *xpre = "/config/DNS/Hosted/Domain";
	char xpath[PATH_MAX];
	const char *domain;
	struct xml_node *xn;

	xn = dtsgui_treenodegetxml(self, node, NULL);
	domain = xml_getattr(xn, "domain");
	snprintf(xpath, PATH_MAX, "%s[@domain = '%s']", xpre, domain);

	dtsgui_xmltextbox(p, "TSIG Key [Master]", "key", xpath, "key");
	dtsgui_xmlcheckbox(p, "Internal Domain", "internal", "true", "", xpath, "internal");

	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_config_dns_domain_new(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	dtsgui_textbox(p, "Domain", "domain", "", NULL);
	dtsgui_textbox(p, "TSIG Key [Master]", "key", "", NULL);
	dtsgui_checkbox(p, "Internal Domain", "internal", "true", "", 0, NULL);

	dtsgui_newxmltreenode(self, p, node, "/config/DNS/Hosted", "Domain", NULL, "domain", DTS_NODE_NETWORK_CONFIG_DNS_DOMAIN,
							DTS_TREE_NEW_NODE_CONTAINER | DTS_TREE_NEW_NODE_DELETE | DTS_TREE_NEW_NODE_EDIT | DTS_TREE_NEW_NODE_SORT, network_config_dns_domain_node,
							NULL, network_config_dns_domain);
}

void network_config_dns_domain_server(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	const char *xpre;
	char xpath[PATH_MAX];
	struct xml_node *xn;

	xn = dtsgui_treenodegetxml(self, node, NULL);

	if (!(xpre = dtsgui_treenodegetdata(self, node))) {
		return;
	}

	snprintf(xpath, PATH_MAX, "%s/NameServer[. = '%s']", xpre, xn->value);
	dtsgui_xmltextbox(p, "Name Server IP Address", "master", xpath, NULL);
	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_config_dns_domain_server_new(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	const char *domain;
	char xpath[PATH_MAX];
	int len;

	if (!(domain = dtsgui_treenodeparent(node))) {
		return;
	}

	snprintf(xpath, PATH_MAX, "/config/DNS/Hosted/Domain[@domain = '%s']", domain);
	free((void*)domain);

	len = strlen(xpath)+1;
	domain = objalloc(len, NULL);
	memcpy((void*)domain, xpath, len);

	dtsgui_textbox(p, "Name Server IP Address", "master", "", NULL);

	dtsgui_newxmltreenode(self, p, node, xpath, "NameServer", "master", "master", DTS_NODE_NETWORK_CONFIG_DNS_DOMAIN_SERVER,
							DTS_TREE_NEW_NODE_DELETE | DTS_TREE_NEW_NODE_SORT, NULL, (void*)domain, network_config_dns_domain_server);
}

void network_iface(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	const char *xpre = "/config/IP/Interfaces/Interface";
	char xpath[PATH_MAX];
	struct xml_node *xn;

	xn = dtsgui_treenodegetxml(self, node, NULL);
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

void network_newiface(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {

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
							DTS_TREE_NEW_NODE_CONTAINER | DTS_TREE_NEW_NODE_DELETE | DTS_TREE_NEW_NODE_EDIT | DTS_TREE_NEW_NODE_SORT, NULL, NULL, network_iface);
}

void network_wifi(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	const char *xpre = "/config/IP/WiFi";
	char xpath[PATH_MAX];
	struct form_item *lb;
	struct xml_node *xn;

	xn = dtsgui_treenodegetxml(self, node, NULL);

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

void network_newwifi(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	struct form_item *lb;
	struct xml_doc *xmldoc;

	xmldoc = dtsgui_panelxml(p);

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
							DTS_TREE_NEW_NODE_DELETE | DTS_TREE_NEW_NODE_SORT, NULL, NULL, network_wifi);
}

void network_wan(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	const char *xpre = "/config/IP/Routes/Route";
	char xpath[PATH_MAX];
	struct xml_node *xn;

	xn = dtsgui_treenodegetxml(self, node, NULL);

	snprintf(xpath, PATH_MAX, "%s[. = '%s']", xpre, xn->value);

	dtsgui_xmltextbox(p, "Network", "network", xpath, "network");
	dtsgui_xmltextbox(p, "Subnet Bits", "subnet", xpath, "subnet");
	dtsgui_xmltextbox(p, "Gateway [Local]", "gateway", xpath, "gateway");
	dtsgui_xmltextbox(p, "DHCP Start Address", "dhcpstart", xpath, "dhcpstart");
	dtsgui_xmltextbox(p, "DHCP End Address", "dhcpend", xpath, "dhcpend");
	dtsgui_xmltextbox(p, "Gateway [Remote]", "remote", xpath, "remote");

	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_newwan(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {

	dtsgui_textbox(p, "Description", "descrip", "", NULL);
	dtsgui_textbox(p, "Network", "network", "", NULL);
	dtsgui_textbox(p, "Subnet Bits", "subnet", "", NULL);
	dtsgui_textbox(p, "Gateway [Local]", "gateway", "", NULL);
	dtsgui_textbox(p, "DHCP Start Address", "dhcpstart", "", NULL);
	dtsgui_textbox(p, "DHCP End Address", "dhcpend", "", NULL);
	dtsgui_textbox(p, "Gateway [Remote]", "remote", "", NULL);

	dtsgui_newxmltreenode(self, p, node, "/config/IP/Routes", "Route", "descrip", NULL, DTS_NODE_NETWORK_WAN,
							DTS_TREE_NEW_NODE_DELETE | DTS_TREE_NEW_NODE_EDIT | DTS_TREE_NEW_NODE_SORT, NULL, NULL, network_wan);
}

void network_route(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	const char *xpre = "/config/IP/GenRoutes/Route";
	char xpath[PATH_MAX];
	struct xml_node *xn;

	xn = dtsgui_treenodegetxml(self, node, NULL);

	snprintf(xpath, PATH_MAX, "%s[. = '%s']", xpre, xn->value);

	dtsgui_xmltextbox(p, "Network", "network", xpath, "network");
	dtsgui_xmltextbox(p, "Subnet Bits", "subnet", xpath, "subnet");
	dtsgui_xmltextbox(p, "Gateway", "gateway", xpath, "gateway");

	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_newroute(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	dtsgui_textbox(p, "Description", "descrip", "", NULL);
	dtsgui_textbox(p, "Network", "network", "", NULL);
	dtsgui_textbox(p, "Subnet Bits", "subnet", "", NULL);
	dtsgui_textbox(p, "Gateway", "gateway", "", NULL);

	dtsgui_newxmltreenode(self, p, node, "/config/IP/GenRoutes", "Route", "descrip", NULL, DTS_NODE_NETWORK_ROUTE,
								DTS_TREE_NEW_NODE_DELETE | DTS_TREE_NEW_NODE_EDIT | DTS_TREE_NEW_NODE_SORT, NULL, NULL, network_route);
}

void network_modem(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
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
	xmltextbox(p, "Idle Timeout", "/config/IP/Dialup", "Option", "120", "option", "IdleTimeout");
	xmltextbox(p, "Fail Limit", "/config/IP/Dialup", "Option", "5", "option", "MaxFail");
	xmltextbox(p, "Connect Dely", "/config/IP/Dialup", "Option", "", "option", "ConnectDelay");

	xmltextbox(p, "Number/Service ID/APN", "/config/IP/Dialup", "Option", "TelkomSA", "opFlowControltion", "Number");
	xmltextbox(p, "Username", "/config/IP/Dialup", "Option", "", "option", "Username");
	xmltextbox(p, "Password", "/config/IP/Dialup", "Option", "", "option", "Password");

	xmlcheckbox(p, "BSD Compression", "/config/IP/Dialup", "Option", "", "option", "BSD");
	xmlcheckbox(p, "Deflate Compression", "/config/IP/Dialup", "Option", "", "option", "Deflate");

	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_modem_ana(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
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

	xmlcheckbox(p, "Abort On No Carrier", "/config/IP/Dialup", "Option", "true", "option", "NoCarrier");
	xmlcheckbox(p, "Abort On No Dialtone", "/config/IP/Dialup", "Option", "true", "option", "NoDialtone");
	xmlcheckbox(p, "Abort On Error", "/config/IP/Dialup", "Option", "true", "option", "Error");
	xmlcheckbox(p, "Abort On Busy", "/config/IP/Dialup", "Option", "true", "option", "Busy");

	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_adsl_link(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	const char *xpre = "/config/IP/ADSL/Links/Link";
	char xpath[PATH_MAX];
	struct xml_node *xn;

	xn = dtsgui_treenodegetxml(self, node, NULL);

	snprintf(xpath, PATH_MAX, "%s[. = '%s']", xpre, xn->value);

	dtsgui_xmltextbox(p, "Interface", "interface", xpath, "interface");
	dtsgui_xmltextbox(p, "PPPoE Service (AC/Service)", "service", xpath, "service");
	dtsgui_xmltextbox(p, "Username", "username", xpath, "username");
	dtsgui_xmltextbox(p, "Password", "password", xpath, "password");
	dtsgui_xmltextbox(p, "Bandwidth In", "bwin", xpath, "bwin");
	dtsgui_xmltextbox(p, "Bandwidth Out", "bwout", xpath, "bwout");
	dtsgui_xmltextbox(p, "TOS Match", "tos", xpath, "tos");
	dtsgui_xmltextbox(p, "Virtual IP", "virtip", xpath, "virtip");
	dtsgui_xmltextbox(p, "Remote IP", "remip", xpath, "remip");


	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_adsl_link_new(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	dtsgui_textbox(p, "Link Name", "name", "", NULL);
	dtsgui_textbox(p, "Interface", "interface", "", NULL);
	dtsgui_textbox(p, "PPPoE Service (AC/Service)", "service", "", NULL);
	dtsgui_textbox(p, "Username", "username", "", NULL);
	dtsgui_textbox(p, "Password", "password", "", NULL);
	dtsgui_textbox(p, "Bandwidth In", "bwin", "", NULL);
	dtsgui_textbox(p, "Bandwidth Out", "bwout", "", NULL);
	dtsgui_textbox(p, "TOS Match", "tos", "", NULL);
	dtsgui_textbox(p, "Virtual IP", "virtip", "", NULL);
	dtsgui_textbox(p, "Remote IP", "remip", "", NULL);

	dtsgui_newxmltreenode(self, p, node, "/config/IP/ADSL/Links", "Link", "name", NULL, DTS_NODE_NETWORK_ADSL_LINK,
								DTS_TREE_NEW_NODE_DELETE | DTS_TREE_NEW_NODE_EDIT | DTS_TREE_NEW_NODE_SORT, NULL, NULL, network_adsl_link);
}

void network_adsl_user(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	const char *xpre = "/config/IP/ADSL/Users/User";
	char xpath[PATH_MAX];
	struct xml_node *xn;

	xn = dtsgui_treenodegetxml(self, node, NULL);

	snprintf(xpath, PATH_MAX, "%s[. = '%s']", xpre, xn->value);

	dtsgui_xmltextbox(p, "Password", "password", xpath, "password");

	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_adsl_user_new(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	dtsgui_textbox(p, "Username", "username", "", NULL);
	dtsgui_textbox(p, "Password", "password", "", NULL);

	dtsgui_newxmltreenode(self, p, node, "/config/IP/ADSL/Users", "User", "username", NULL, DTS_NODE_NETWORK_ADSL_USER,
								DTS_TREE_NEW_NODE_DELETE | DTS_TREE_NEW_NODE_EDIT | DTS_TREE_NEW_NODE_SORT, NULL, NULL, network_adsl_user);
}

void network_tos(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	struct form_item *lb;
	int pcnt, i;
	const char *xpre = "/config/IP/QOS/TOS";
	char xpath[PATH_MAX];
	struct xml_node *xn;

	xn = dtsgui_treenodegetxml(self, node, NULL);

	snprintf(xpath, PATH_MAX, "%s[@name = '%s']", xpre, xml_getattr(xn, "name"));

	dtsgui_xmltextbox(p, "Destination Address", "ipaddr", xpath, "ipaddr");

	lb = dtsgui_xmllistbox(p, "Protocol", "protocol", xpath, "protocol");
	pcnt = sizeof(protocols)/sizeof(protocols[0]);
	for(i = 0;i < pcnt;i ++) {
		dtsgui_listbox_add(lb, protocols[i], protocols[i]);
	}
	objunref(lb);

	dtsgui_xmltextbox(p, "Destination Port", "dport", xpath, "dport");
	dtsgui_xmltextbox(p, "Source Port", "sport", xpath, "sport");

	lb = dtsgui_xmllistbox(p, "TOS Setting", "tos", xpath, NULL);
	dtsgui_listbox_add(lb, "Normal-Service", "Normal-Service");
	dtsgui_listbox_add(lb, "Minimize-Cost", "Minimize-Cost");
	dtsgui_listbox_add(lb, "Maximize-Reliability", "Maximize-Reliability");
	dtsgui_listbox_add(lb, "Maximize-Throughput", "Maximize-Throughput");
	dtsgui_listbox_add(lb, "Minimize-Delay", "Minimize-Delay");
	objunref(lb);

	lb = dtsgui_xmllistbox(p, "Priority", "priority", xpath, "priority");
	dtsgui_listbox_add(lb, "High", "High");
	dtsgui_listbox_add(lb, "Med", "Med");
	dtsgui_listbox_add(lb, "Low", "Low");
	objunref(lb);

	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_tos_new(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	struct form_item *lb;
	int pcnt, i;

	dtsgui_textbox(p, "Description", "name", "", NULL);
	dtsgui_textbox(p, "Destination Address", "ipaddr", "", NULL);

	lb = dtsgui_listbox(p, "Protocol", "protocol", NULL);
	pcnt = sizeof(protocols)/sizeof(protocols[0]);
	for(i = 0;i < pcnt;i ++) {
		dtsgui_listbox_add(lb, protocols[i], protocols[i]);
	}
	objunref(lb);

	dtsgui_textbox(p, "Destination Address", "ipaddr", "", NULL);
	dtsgui_textbox(p, "Destination Port", "dport", "", NULL);
	dtsgui_textbox(p, "Source Port", "sport", "1024:65535", NULL);

	lb = dtsgui_listbox(p, "TOS Setting", "tos", NULL);
	dtsgui_listbox_add(lb, "Normal-Service", "Normal-Service");
	dtsgui_listbox_add(lb, "Minimize-Cost", "Minimize-Cost");
	dtsgui_listbox_add(lb, "Maximize-Reliability", "Maximize-Reliability");
	dtsgui_listbox_add(lb, "Maximize-Throughput", "Maximize-Throughput");
	dtsgui_listbox_add(lb, "Minimize-Delay", "Minimize-Delay");
	objunref(lb);

	lb = dtsgui_listbox(p, "Priority", "priority", NULL);
	dtsgui_listbox_add(lb, "High", "High");
	dtsgui_listbox_add(lb, "Med", "Med");
	dtsgui_listbox_add(lb, "Low", "Low");
	objunref(lb);

	dtsgui_newxmltreenode(self, p, node, "/config/IP/QOS", "TOS", "name", NULL, DTS_NODE_NETWORK_TOS,
								DTS_TREE_NEW_NODE_DELETE | DTS_TREE_NEW_NODE_EDIT | DTS_TREE_NEW_NODE_SORT, NULL, NULL, network_tos);
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

dtsgui_pane newtreepane(dtsgui_treeview self, dtsgui_treenode node, const char *title) {
	dtsgui_pane p = NULL;
	struct xml_doc *xmldoc;
	enum node_id nodeid;

	nodeid = dtsgui_treenodeid(self, node);

	if (nodeid == -1) {
		p = dtsgui_treepane(self, NULL, 0, NULL, NULL);
		dtsgui_setevcallback(p, handle_test, NULL);
		return p;
	}

	xmldoc = dtsgui_panelxml(self);
	if (!(p = dtsgui_treepane(self, title, wx_PANEL_BUTTON_ACTION, NULL, xmldoc))) {
		return NULL;
	}
	if (xmldoc) {
		objunref(xmldoc);
	}
	return p;
}

dtsgui_pane tree_do_shit(struct dtsgui *dtsgui, dtsgui_treeview self, dtsgui_treenode node, enum tree_cbtype cb_type, const char *title, void *td) {
	switch(cb_type) {
		case DTSGUI_TREE_CB_SELECT:
			return newtreepane(self, node, title);
		case DTSGUI_TREE_CB_EDIT:
			node_edit(self, node, title);
			return NULL;
		case DTSGUI_TREE_CB_DELETE:
			return NULL;
	}
	return NULL;
}


dtsgui_treeview network_tree(struct dtsgui *dtsgui) {
	dtsgui_treeview tree;
	dtsgui_treenode root, tmp, tmp2, tln/*, tmp3*/;
	struct app_data *appdata;
	struct xml_doc *xmldoc;
	struct xml_search *xp;
	struct xml_node *xn;
	void *iter = NULL;
	char defconf[PATH_MAX];

	appdata = dtsgui_userdata(dtsgui);
	snprintf(defconf, PATH_MAX-1, "%s/default.xml", appdata->datadir);
	if (!is_file(defconf)) {
		dtsgui_alert(dtsgui, "Default configuration not found.\nCheck Installation.");
		return 0;
	}

	if (!(xmldoc = xml_loaddoc(defconf, 1))) {
		dtsgui_alert(dtsgui, "Default configuration failed to loDynamicTTLad.\nCheck Installation.");
		return 0;
	}

	tree = dtsgui_treewindow(dtsgui, "Tree Window", tree_do_shit, NULL, xmldoc);

	root = dtsgui_treecont(tree, NULL, "Global IP Settings", 0, 0, 0, DTS_NODE_NETWORK_CONFIG, network_config, NULL);
	tln = dtsgui_treecont(tree, root, "DNS/DHCP Settings", 0, 0, 0, DTS_NODE_NETWORK_CONFIG_DNS, network_config_dns, NULL);
	dtsgui_treeitem(tree, tln, "Dynamic DNS", 0, 0, 0, DTS_NODE_NETWORK_CONFIG_DNS_DYN, network_config_dns_dyn, NULL);
	dtsgui_treeitem(tree, tln, "DNS Server Options", 0, 0, 0, DTS_NODE_NETWORK_CONFIG_DNS_SERV, network_config_dns_serv, NULL);
	dtsgui_treeitem(tree, tln, "DNS Zone Defaults", 0, 0, 0, DTS_NODE_NETWORK_CONFIG_DNS_ZONE, network_config_dns_zone, NULL);

	tmp = dtsgui_treecont(tree, tln, "Static Hosts", 0, 1, 0, DTS_NODE_NETWORK_CONFIG_DNS_HOST_NEW, network_config_dns_host_new, NULL);
	xp = xml_xpath(xmldoc, "/config/DNS/Hosts/Host", NULL);
	for(xn = xml_getfirstnode(xp, &iter); xn; xn = xml_getnextnode(iter)) {
		tmp2 = dtsgui_treeitem(tree, tmp, xn->value, 1, 1, 1, DTS_NODE_NETWORK_CONFIG_DNS_HOST, network_config_dns_host, NULL);
		dtsgui_treenodesetxml(tree, tmp2, xn, NULL);
		objunref(xn);
	}
	objunref(xp);
	objunref(iter);
	iter = NULL;

	dtsgui_treecont(tree, tln, "Hosted Domains", 0, 1, 0, DTS_NODE_NETWORK_CONFIG_DNS_DOMAIN_NEW, network_config_dns_domain_new, NULL);
	xp = xml_xpath(xmldoc, "/config/DNS/Hosted/Domain", NULL);
	for(xn = xml_getfirstnode(xp, &iter); xn; xn = xml_getnextnode(iter)) {
		tmp2 = dtsgui_treeitem(tree, tmp, xn->value, 1, 1, 1, DTS_NODE_NETWORK_CONFIG_DNS_DOMAIN, network_config_dns_domain, NULL);
		dtsgui_treenodesetxml(tree, tmp2, xn, NULL);
/*		xp2 = xml_xpath(xmldoc, "/config/DNS/Hosted/Domain", NULL);
		for(xn = xml_getfirstnode(xp2, &iter); xn2; xn2 = xml_getnextnode(iter2)) {
			tmp3 = dtsgui_treeitem(tree, tmp2, xn2->value, 1, 1, 1, DTS_NODE_NETWORK_CONFIG_DNS_HOST, NULL);
			dtsgui_treenodesetxml(tree, tmp3, xn2, NULL);
			objunref(xn2);
		}
		objunref(xp2);*/
		objunref(xn);
	}
	objunref(xp);
	objunref(iter);
	iter = NULL;

	tln = dtsgui_treecont(tree, root, "Network Interfaces", 0, 0, 0, -1, NULL, NULL);
	tmp = dtsgui_treecont(tree, tln, "Ethernet Interfaces", 0, 1, 0, DTS_NODE_NETWORK_IFACE_NEW, network_newiface, NULL);

	xp = xml_xpath(xmldoc, "/config/IP/Interfaces/Interface", "name");
	for(xn = xml_getfirstnode(xp, &iter); xn; xn = xml_getnextnode(iter)) {
		tmp2 = dtsgui_treecont(tree, tmp, xml_getattr(xn, "name"), 1, 1, 1, DTS_NODE_NETWORK_IFACE, network_iface, NULL);
		dtsgui_treenodesetxml(tree, tmp2, xn, "name");
		objunref(xn);
	}
	objunref(xp);
	objunref(iter);
	iter = NULL;

	tmp = dtsgui_treecont(tree, tln, "Wireless Config", 0, 1, 0, DTS_NODE_NETWORK_WIFI_NEW, network_newwifi, NULL);
	xp = xml_xpath(xmldoc, "/config/IP/WiFi", NULL);
	for(xn = xml_getfirstnode(xp, &iter); xn; xn = xml_getnextnode(iter)) {
		tmp2 = dtsgui_treeitem(tree, tmp, xn->value, 0, 1, 1, DTS_NODE_NETWORK_WIFI, network_wifi, NULL);
		dtsgui_treenodesetxml(tree, tmp2, xn, NULL);
		objunref(xn);
	}
	objunref(xp);
	objunref(iter);
	iter = NULL;

	tln = dtsgui_treecont(tree, root, "Static Routes", 0, 0, 0, -1, NULL, NULL);
	tmp = dtsgui_treecont(tree, tln, "Wan Routing/Nodes", 0, 1, 0, DTS_NODE_NETWORK_WAN_NEW, network_newwan, NULL);
	xp = xml_xpath(xmldoc, "/config/IP/Routes/Route", NULL);
	for(xn = xml_getfirstnode(xp, &iter); xn; xn = xml_getnextnode(iter)) {
		tmp2 = dtsgui_treeitem(tree, tmp, xn->value, 1, 1, 1, DTS_NODE_NETWORK_WAN, network_wan, NULL);
		dtsgui_treenodesetxml(tree, tmp2, xn, NULL);
		objunref(xn);
	}
	objunref(xp);
	objunref(iter);
	iter = NULL;

	tmp = dtsgui_treecont(tree, tln, "Other Routes", 0, 1, 0, DTS_NODE_NETWORK_ROUTE_NEW, network_newroute, NULL);
	xp = xml_xpath(xmldoc, "/config/IP/GenRoutes/Route", NULL);
	for(xn = xml_getfirstnode(xp, &iter); xn; xn = xml_getnextnode(iter)) {
		tmp2 = dtsgui_treeitem(tree, tmp, xn->value, 1, 1, 1, DTS_NODE_NETWORK_WAN, network_route, NULL);
		dtsgui_treenodesetxml(tree, tmp2, xn, NULL);
		objunref(xn);
	}
	objunref(xp);
	objunref(iter);
	iter = NULL;

	tln = dtsgui_treecont(tree, root, "PPP Connection Config", 0, 0, 0, DTS_NODE_NETWORK_MODEM, network_modem, NULL);
	dtsgui_treeitem(tree, tln, "Dialup/Leased", 0, 0, 0, DTS_NODE_NETWORK_MODEM_ANA, network_modem_ana, NULL);
	dtsgui_treecont(tree, tln, "Modem Firewall Rules", 0, 1, 0, -1, NULL, NULL);

	tmp = dtsgui_treecont(tree, tln, "Additional ADSL Links", 0, 1, 0, DTS_NODE_NETWORK_ADSL_LINK_NEW, network_adsl_link_new, NULL);
	xp = xml_xpath(xmldoc, "/config/IP/ADSL/Links/Link", NULL);
	for(xn = xml_getfirstnode(xp, &iter); xn; xn = xml_getnextnode(iter)) {
		tmp2 = dtsgui_treeitem(tree, tmp, xn->value, 1, 1, 1, DTS_NODE_NETWORK_ADSL_LINK, network_adsl_link, NULL);
		dtsgui_treenodesetxml(tree, tmp2, xn, NULL);
		objunref(xn);
	}
	objunref(xp);
	objunref(iter);
	iter = NULL;

	tmp = dtsgui_treecont(tree, tln, "ADSL Accounts", 0, 1, 0, DTS_NODE_NETWORK_ADSL_USER_NEW, network_adsl_user_new, NULL);
	xp = xml_xpath(xmldoc, "/config/IP/ADSL/Users/User", NULL);
	for(xn = xml_getfirstnode(xp, &iter); xn; xn = xml_getnextnode(iter)) {
		tmp2 = dtsgui_treeitem(tree, tmp, xn->value, 1, 1, 1, DTS_NODE_NETWORK_ADSL_USER, network_adsl_user, NULL);
		dtsgui_treenodesetxml(tree, tmp2, xn, NULL);
		objunref(xn);
	}
	objunref(xp);
	objunref(iter);
	iter = NULL;

	tmp = dtsgui_treecont(tree, root, "Default TOS", 0, 1, 0, DTS_NODE_NETWORK_TOS_NEW, network_tos_new, NULL);
	xp = xml_xpath(xmldoc, "/config/IP/QOS/TOS", NULL);
	for(xn = xml_getfirstnode(xp, &iter); xn; xn = xml_getnextnode(iter)) {
		tmp2 = dtsgui_treeitem(tree, tmp, xml_getattr(xn, "name"), 1, 1, 1, DTS_NODE_NETWORK_TOS, network_tos, NULL);
		dtsgui_treenodesetxml(tree, tmp2, xn, NULL);
		objunref(xn);
	}
	objunref(xp);
	objunref(iter);
	iter = NULL;


	tln = dtsgui_treecont(tree, root, "VPN Configuration", 0, 0, 0, -1, NULL, NULL);
	dtsgui_treecont(tree, tln, "GRE VPN Tunnels", 0, 1, 0, -1, NULL, NULL);
	dtsgui_treecont(tree, tln, "ESP VPN Tunnels", 0, 1, 0, -1, NULL, NULL);
	dtsgui_treecont(tree, tln, "ESP Remote Access", 0, 1, 0, -1, NULL, NULL);

	tln = dtsgui_treecont(tree, root, "Voice & FAX Over IP", 0, 0, 0, -1, NULL, NULL);
	dtsgui_treecont(tree, tln, "VOIP Registrations", 0, 0, 0, -1, NULL, NULL);
	dtsgui_treecont(tree, tln, "FAX Config", 0, 0, 0, -1, NULL, NULL);
	dtsgui_treecont(tree, root, "Secuity Certificate Config", 0, 0, 0, -1, NULL, NULL);

	return tree;
}
