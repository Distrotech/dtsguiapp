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

/*XXX LDAP Settings server/DN*/
void network_config(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	struct xml_doc *xmldoc = dtsgui_panelxml(p);
	struct form_item *lb, *elb;
	const char *extint = NULL;

	/*XXX LDAP Settings server/DN*/
	if ((lb = dtsgui_xmllistbox(p, "Internal Interface", "Internal", "/config/IP/SysConf", "Option", "option", "Internal", NULL))) {
		dtsgui_listbox_addxml(lb, xmldoc, "/config/IP/Interfaces/Interface", "name", NULL);
		objunref(lb);
	}
	if ((elb = dtsgui_xmllistbox(p, "External Interface", "External", "/config/IP/SysConf", "Option", "option", "External", NULL))) {
		dtsgui_listbox_add(elb, "Modem", "Dialup");
		dtsgui_listbox_addxml(elb, xmldoc, "/config/IP/Interfaces/Interface", "name", NULL);
		extint = dtsgui_item_value(elb);
		objunref(elb);
	}
	if ((lb = dtsgui_xmllistbox(p, "External OVPN Interface", "OVPNNet", "/config/IP/SysConf", "Option", "option", "OVPNNet", NULL))) {
		dtsgui_listbox_add(lb, "Modem", "Dialup");
		dtsgui_listbox_addxml(lb, xmldoc, "/config/IP/Interfaces/Interface", "name", NULL);
		objunref(lb);
	}

	if (extint) {
		free((void*)extint);
	}

	dtsgui_xmltextbox(p, "Gateway Address", "Nexthop", "/config/IP/SysConf", "Option", "option", "Nexthop", NULL);
	dtsgui_xmltextbox(p, "External (Natted IP)", "NattedIP", "/config/IP/SysConf", "Option", "option", "NattedIP", NULL);
	dtsgui_xmltextbox(p, "IPSEC VPN Access", "VPNNet", "/config/IP/SysConf", "Option", "option", "VPNNet", NULL);
	dtsgui_xmltextbox(p, "Open VPN Access", "OVPNNet", "/config/IP/SysConf", "Option", "option", "OVPNNet", NULL);
	dtsgui_xmltextbox(p, "L2TP VPN Access", "L2TPNet", "/config/IP/SysConf", "Option", "option", "L2TPNet", NULL);
	dtsgui_xmltextbox(p, "NTP Server[s]", "NTPServer", "/config/IP/SysConf", "Option", "option", "NTPServer", NULL);
	dtsgui_xmltextbox(p, "Bridge Interfaces", "Bridge", "/config/IP/SysConf", "Option", "option", "Bridge", NULL);
	dtsgui_xmltextbox(p, "Incoming Traffic Limit", "Ingress","/config/IP/SysConf", "Option", "option", "Ingress", NULL);
	dtsgui_xmltextbox(p, "Outgoing Traffic Limit", "Egress", "/config/IP/SysConf", "Option", "option", "Egress", NULL);

	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_config_dns(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	dtsgui_xmltextbox(p, "Hostname", "Hostname", "/config/DNS/Config", "Option", "option", "Hostname", NULL);
	dtsgui_xmltextbox(p, "Domain", "Domain", "/config/DNS/Config", "Option", "option", "Domain", NULL);
	dtsgui_xmltextbox(p, "Aditional Search Domains", "Search", "/config/DNS/Config", "Option", "option", "Search", NULL);
	dtsgui_xmltextbox(p, "Primary DNS", "PrimaryDns", "/config/IP/SysConf", "Option", "option", "PrimaryDns", NULL);
	dtsgui_xmltextbox(p, "Secondary DNS", "SecondaryDns", "/config/IP/SysConf", "Option", "option", "SecondaryDns", NULL);
	dtsgui_xmltextbox(p, "Primary WINS", "PrimaryWins", "/config/IP/SysConf", "Option", "option", "PrimaryWins", NULL);
	dtsgui_xmltextbox(p, "Secondary WINS", "SecondaryWins", "/config/IP/SysConf", "Option", "option", "SecondaryWins", NULL);
	dtsgui_xmltextbox(p, "DHCP Lease", "DHCPLease", "/config/IP/SysConf", "Option", "option", "DHCPLease", NULL);
	dtsgui_xmltextbox(p, "DHCP Max Lease", "DHCPLease", "/config/IP/SysConf", "Option", "option", "DHCPMaxLease", NULL);
	dtsgui_xmlcheckbox(p, "Use Internal Interface For DNS First", "IntFirst", "true", "", "/config/DNS/Config", "Option", "option", "IntFirst", NULL);
	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_config_dns_dyn(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	dtsgui_xmltextbox(p, "Dynamic DNS Domain", "DynZone", "/config/DNS/Config", "Option", "option", "DynZone", NULL);
	dtsgui_xmltextbox(p, "Dynamic DNS Server", "DynServ", "/config/DNS/Config", "Option", "option", "DynServ", NULL);
	dtsgui_xmltextbox(p, "Dynamic DNS Secret", "DynKey", "/config/DNS/Config", "Option", "option", "DynKey", NULL);
	dtsgui_xmltextbox(p, "Dynamic Zone TTL", "DynamicTTL", "/config/DNS/Config", "Option", "option", "DynamicTTL", NULL);
	dtsgui_xmlcheckbox(p, "Add CNAME For host", "DynamicCNAME", "true", "", "/config/DNS/Config", "Option", "option", "DynamicCNAME", NULL);
	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_config_dns_serv(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {

	dtsgui_xmltextbox(p, "Shared Secret For Domain Updates", "SmartKey", "/config/DNS/Config", "Option", "option", "SmartKey", NULL);
	dtsgui_xmlcheckbox(p, "Backup Zone Files", "Backup", "true", "", "/config/DNS/Config", "Option", "option", "Backup", NULL);
	dtsgui_xmlcheckbox(p, "Authorotive [Internal]", "Auth", "true", "", "/config/DNS/Config", "Option", "option", "Auth", NULL);
	dtsgui_xmlcheckbox(p, "Authorotive [External]", "AuthX", "true", "","/config/DNS/Config", "Option", "option", "AuthX", NULL);
	dtsgui_xmlcheckbox(p, "Use DNS Servers Obtained From PPP", "Usepeer", "true", "", "/config/DNS/Config", "Option", "option", "Usepeer", NULL);
	dtsgui_xmlcheckbox(p, "Allow Recursive Lookup Externally", "ExtServ", "true", "", "/config/DNS/Config", "Option", "option", "ExtServ", NULL);
	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_config_dns_zone(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	dtsgui_xmltextbox(p, "SOA Serial", "Serial", "/config/DNS/Config", "Option", "option", "Serial", NULL);
	dtsgui_xmltextbox(p, "SOA Refresh", "Refresh", "/config/DNS/Config", "Option", "option", "Refresh", NULL);
	dtsgui_xmltextbox(p, "SOA Retry", "Retry", "/config/DNS/Config", "Option", "option", "Retry", NULL);
	dtsgui_xmltextbox(p, "SOA Expire", "Expire", "/config/DNS/Config", "Option", "option", "Expire", NULL);
	dtsgui_xmltextbox(p, "Default TTL", "DefaultTTL", "/config/DNS/Config", "Option", "option", "DefaultTTL", NULL);

	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_config_dns_host(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	const char *xpre = "/config/DNS/Hosts";
	struct xml_node *xn = dtsgui_treenodegetxml(self, node, NULL);

	dtsgui_xmltextbox(p, "IP Address", "ipaddr", xpre, "Host", NULL, xn->value, "ipaddr");
	dtsgui_xmltextbox(p, "MAC Address", "macaddr", xpre, "Host", NULL, xn->value, "macaddr");

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
	const char *xpre = "/config/DNS/Hosted";
	const char *domain;
	struct xml_node *xn = dtsgui_treenodegetxml(self, node, NULL);

	domain = xml_getattr(xn, "domain");

	dtsgui_xmltextbox(p, "TSIG Key [Master]", "key", xpre, "Domain", "domain", domain, "key");
	dtsgui_xmlcheckbox(p, "Internal Domain", "internal", "true", "", xpre, "Domain", "domain", domain, "internal");

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
	struct xml_node *xn = dtsgui_treenodegetxml(self, node, NULL);

	if (!(xpre = dtsgui_treenodegetdata(self, node))) {
		return;
	}

	dtsgui_xmltextbox(p, "Name Server IP Address", "master", xpre, "NameServer", NULL, xn->value, NULL);
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
	const char *xpre = "/config/IP/Interfaces";
	struct xml_node *xn = dtsgui_treenodegetxml(self, node, NULL);

	dtsgui_xmltextbox(p, "Interface", "iface", xpre, "Interface", NULL, xn->value, NULL);
	dtsgui_xmltextbox(p, "IP Address", "ipaddr", xpre, "Interface", NULL, xn->value, "ipaddr");
	dtsgui_xmltextbox(p, "IP Subnet Bits", "subnet", xpre, "Interface", NULL, xn->value, "subnet");
	dtsgui_xmltextbox(p, "DHCP Gateway", "gateway", xpre, "Interface", NULL, xn->value, "gateway");
	dtsgui_xmltextbox(p, "MAC Address", "macaddr", xpre, "Interface", NULL, xn->value, "macaddr");
	dtsgui_xmltextbox(p, "DHCP Start Address", "dhcpstart", xpre, "Interface", NULL, xn->value, "dhcpstart");
	dtsgui_xmltextbox(p, "DHCP End Address", "dhcpend", xpre, "Interface", NULL, xn->value, "dhcpend");
	dtsgui_xmltextbox(p, "Bandwidth In", "bwin", xpre, "Interface", NULL, xn->value, "bwin");
	dtsgui_xmltextbox(p, "Bandwidth Out", "bwout", xpre, "Interface", NULL, xn->value, "bwout");

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
	const char *xpre = "/config/IP";
	struct form_item *lb;
	struct xml_node *xn = dtsgui_treenodegetxml(self, node, NULL);

	lb = dtsgui_xmlcombobox(p, "WiFi Configuration", "type", xpre, "WiFi", NULL, xn->value, "type");
	dtsgui_listbox_add(lb, "Access Point", "AP");
	dtsgui_listbox_add(lb, "WiFi Client", "Client");
	dtsgui_listbox_add(lb, "Hotspot", "Hotspot");
	objunref(lb);

	lb = dtsgui_xmlcombobox(p, "WiFi Mode", "mode", xpre, "WiFi", NULL, xn->value, "mode");
	dtsgui_listbox_add(lb, "802.11a", "0");
	dtsgui_listbox_add(lb, "802.11b [ch 1/6/11/14]", "1");
	dtsgui_listbox_add(lb, "802.11g [ch 1/5/9/13]", "2");
	dtsgui_listbox_add(lb, "802.11n 20Mhz [ch 1/5/9/13]", "3");
	dtsgui_listbox_add(lb, "802.11n 40Mhz [ch 3/11]", "4");
	objunref(lb);

	lb = dtsgui_xmlcombobox(p, "WiFi Auth Type", "auth", xpre, "WiFi", NULL, xn->value, "auth");
	dtsgui_listbox_add(lb, "WiFi Protected Access", "WPA");
	dtsgui_listbox_add(lb, "Extensible Authentication Protocol", "EAP");
	dtsgui_listbox_add(lb, "None", "None");
	objunref(lb);

	dtsgui_xmltextbox(p, "Channel", "channel", xpre, "WiFi", NULL, xn->value, "channel");
	dtsgui_xmltextbox(p, "Key (WPA)", "key", xpre, "WiFi", NULL, xn->value, "key");
	dtsgui_xmltextbox(p, "Regulatory Domain", "regdom", xpre, "WiFi", NULL, xn->value, "regdom");
	dtsgui_xmltextbox(p, "TX Power", "txpower", xpre, "WiFi", NULL, xn->value, "txpower");

	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_newwifi(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	struct form_item *lb;
	struct xml_doc *xmldoc = dtsgui_panelxml(p);

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
	const char *xpre = "/config/IP/Routes";
	struct xml_node *xn = dtsgui_treenodegetxml(self, node, NULL);

	dtsgui_xmltextbox(p, "Network", "network", xpre, "Route", NULL, xn->value, "network");
	dtsgui_xmltextbox(p, "Subnet Bits", "subnet", xpre, "Route", NULL, xn->value, "subnet");
	dtsgui_xmltextbox(p, "Gateway [Local]", "gateway", xpre, "Route", NULL, xn->value, "gateway");
	dtsgui_xmltextbox(p, "DHCP Start Address", "dhcpstart", xpre, "Route", NULL, xn->value, "dhcpstart");
	dtsgui_xmltextbox(p, "DHCP End Address", "dhcpend", xpre, "Route", NULL, xn->value, "dhcpend");
	dtsgui_xmltextbox(p, "Gateway [Remote]", "remote", xpre, "Route", NULL, xn->value, "remote");

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
	const char *xpre = "/config/IP/GenRoutes";
	struct xml_node *xn = dtsgui_treenodegetxml(self, node, NULL);

	dtsgui_xmltextbox(p, "Network", "network", xpre, "Route", NULL, xn->value, "network");
	dtsgui_xmltextbox(p, "Subnet Bits", "subnet", xpre, "Route", NULL, xn->value, "subnet");
	dtsgui_xmltextbox(p, "Gateway", "gateway", xpre, "Route", NULL, xn->value, "gateway");

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
	if ((lb = dtsgui_xmllistbox(p, "Connection Type", "Connection", "/config/IP/Dialup", "Option", "option", "Connection", NULL))) {
		dtsgui_listbox_add(lb, "Dialup", "Dialup");
		dtsgui_listbox_add(lb, "Leased", "Leased");
		dtsgui_listbox_add(lb, "ADSL", "ADSL");
		dtsgui_listbox_add(lb, "3G", "3G");
		dtsgui_listbox_add(lb, "3GIPW", "3GIPW");
		objunref(lb);
	}

	dtsgui_xmltextbox(p, "Holdoff Time", "Holdoff", "/config/IP/Dialup", "Option", "option", "Holdoff", NULL);
	dtsgui_xmltextbox(p, "MTU/MRU", "MTU", "/config/IP/Dialup", "Option", "option", "MTU", NULL);
	dtsgui_xmltextbox(p, "Idle Timeout", "IdleTimeout", "/config/IP/Dialup", "Option", "option", "IdleTimeout", NULL);
	dtsgui_xmltextbox(p, "Fail Limit", "MaxFail", "/config/IP/Dialup", "Option", "option", "MaxFail", NULL);
	dtsgui_xmltextbox(p, "Connect Dely", "ConnectDelay", "/config/IP/Dialup", "Option", "option", "ConnectDelay", NULL);

	dtsgui_xmltextbox(p, "Number/Service ID/APN", "Number", "/config/IP/Dialup", "Option", "option", "Number", NULL);
	dtsgui_xmltextbox(p, "Username", "Username", "/config/IP/Dialup", "Option", "option", "Username", NULL);
	dtsgui_xmltextbox(p, "Password", "Password", "/config/IP/Dialup", "Option", "option", "Password", NULL);

	dtsgui_xmlcheckbox(p, "BSD Compression", "BSD", "true", "", "/config/IP/Dialup", "Option", "option", "BSD", NULL);
	dtsgui_xmlcheckbox(p, "Deflate Compression", "Deflate", "true", "", "/config/IP/Dialup", "Option", "option", "Deflate", NULL);

	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_modem_ana(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	struct form_item *lb;

	if ((lb = dtsgui_xmllistbox(p, "Comm Port ", "ComPort", "/config/IP/Dialup", "Option", "option", "ComPort", NULL))) {
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

	if ((lb = dtsgui_xmllistbox(p, "Port Speed", "Speed", "/config/IP/Dialup", "Option", "option", "Speed", NULL))) {
		dtsgui_listbox_add(lb, "115200", "115200");
		dtsgui_listbox_add(lb, "57600", "57600");
		dtsgui_listbox_add(lb, "38400", "38400");
		dtsgui_listbox_add(lb, "19200", "19200");
		dtsgui_listbox_add(lb, "9600", "9600");
		objunref(lb);
	}

	if ((lb = dtsgui_xmllistbox(p, "Flow Control ", "FlowControl", "/config/IP/Dialup", "Option", "option", "FlowControl", NULL))) {
		dtsgui_listbox_add(lb, "Hardware (RTS/CTS)", "crtscts");
		dtsgui_listbox_add(lb, "Software (Xon/Xoff)", "xonxoff");
		dtsgui_listbox_add(lb, "Hardware (DTR/CTS)", "cdtrcts");
		dtsgui_listbox_add(lb, "None", "");
		objunref(lb);
	}

	dtsgui_xmltextbox(p, "Local Address", "Address", "/config/IP/Dialup", "Option", "option", "Address", NULL);
	dtsgui_xmltextbox(p, "Remote Address", "Gateway", "/config/IP/Dialup", "Option", "option", "Gateway", NULL);

	dtsgui_xmltextbox(p, "Init String 1", "Init1", "/config/IP/Dialup", "Option", "option", "Init1", NULL);
	dtsgui_xmltextbox(p, "Init String 2", "Init2", "/config/IP/Dialup", "Option", "option", "Init2", NULL);
	dtsgui_xmltextbox(p, "Dial String", "DialString", "/config/IP/Dialup", "Option", "option", "DialString", NULL);

	dtsgui_xmlcheckbox(p, "Abort On No Carrier", "NoCarrier", "true", "","/config/IP/Dialup", "Option", "option", "NoCarrier", NULL);
	dtsgui_xmlcheckbox(p, "Abort On No Dialtone", "NoDialtone", "true", "", "/config/IP/Dialup", "Option", "option", "NoDialtone", NULL);
	dtsgui_xmlcheckbox(p, "Abort On Error", "Error", "true", "", "/config/IP/Dialup", "Option", "option", "Error", NULL);
	dtsgui_xmlcheckbox(p, "Abort On Busy", "Busy", "true", "", "/config/IP/Dialup", "Option", "option", "Busy", NULL);

	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_adsl_link(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	const char *xpre = "/config/IP/ADSL/Links";
	struct xml_node *xn = dtsgui_treenodegetxml(self, node, NULL);
	struct xml_doc *xmldoc = dtsgui_panelxml(p);
	struct form_item *elb;

	if ((elb = dtsgui_xmllistbox(p, "Interface", "interface", xpre, "Link", NULL, xn->value, "interface"))) {
		dtsgui_listbox_addxml(elb, xmldoc, "/config/IP/Interfaces/Interface", "name", NULL);
		objunref(elb);
	}

	dtsgui_xmltextbox(p, "PPPoE Service (AC/Service)", "service", xpre, "Link", NULL, xn->value, "service");
	dtsgui_xmltextbox(p, "Username", "username", xpre, "Link", NULL, xn->value, "username");
	dtsgui_xmltextbox(p, "Password", "password", xpre, "Link", NULL, xn->value, "password");
	dtsgui_xmltextbox(p, "Bandwidth In", "bwin", xpre, "Link", NULL, xn->value, "bwin");
	dtsgui_xmltextbox(p, "Bandwidth Out", "bwout", xpre, "Link", NULL, xn->value, "bwout");
	dtsgui_xmltextbox(p, "TOS Match", "tos", xpre, "Link", NULL, xn->value, "tos");
	dtsgui_xmltextbox(p, "Virtual IP", "virtip", xpre, "Link", NULL, xn->value, "virtip");
	dtsgui_xmltextbox(p, "Remote IP", "remip", xpre, "Link", NULL, xn->value, "remip");


	dtsgui_setevcallback(p, handle_test, NULL);
}

void network_adsl_link_new(dtsgui_pane p, dtsgui_treeview self, dtsgui_treenode node, void *data) {
	struct xml_doc *xmldoc = dtsgui_panelxml(p);
	struct form_item *elb;

	dtsgui_textbox(p, "Link Name", "name", "", NULL);
	if ((elb = dtsgui_listbox(p, "Interface", "interface", NULL))) {
		dtsgui_listbox_addxml(elb, xmldoc, "/config/IP/Interfaces/Interface", "name", NULL);
		objunref(elb);
	}
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
	const char *xpre = "/config/IP/ADSL/Users";
	struct xml_node *xn = dtsgui_treenodegetxml(self, node, NULL);

	dtsgui_xmltextbox(p, "Password", "password", xpre, "User", NULL, xn->value , "password");

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
	const char *xpre = "/config/IP/QOS";
	const char *name;
	struct xml_node *xn = dtsgui_treenodegetxml(self, node, NULL);

	name = xml_getattr(xn, "name");

	dtsgui_xmltextbox(p, "Destination Address", "ipaddr", xpre, "TOS", "name", name, "ipaddr");

	lb = dtsgui_xmllistbox(p, "Protocol", "protocol", xpre, "TOS", "name", name, "protocol");
	pcnt = sizeof(protocols)/sizeof(protocols[0]);
	for(i = 0;i < pcnt;i ++) {
		dtsgui_listbox_add(lb, protocols[i], protocols[i]);
	}
	objunref(lb);

	dtsgui_xmltextbox(p, "Destination Port", "dport", xpre, "TOS", "name", name, "dport");
	dtsgui_xmltextbox(p, "Source Port", "sport", xpre, "TOS", "name", name, "sport");

	lb = dtsgui_xmllistbox(p, "TOS Setting", "tos", xpre, "TOS", "name", name, NULL);
	dtsgui_listbox_add(lb, "Normal-Service", "Normal-Service");
	dtsgui_listbox_add(lb, "Minimize-Cost", "Minimize-Cost");
	dtsgui_listbox_add(lb, "Maximize-Reliability", "Maximize-Reliability");
	dtsgui_listbox_add(lb, "Maximize-Throughput", "Maximize-Throughput");
	dtsgui_listbox_add(lb, "Minimize-Delay", "Minimize-Delay");
	objunref(lb);

	lb = dtsgui_xmllistbox(p, "Priority", "priority", xpre, "TOS", "name", name, "priority");
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

void network_tree_setup(dtsgui_treeview tree, struct xml_doc *xmldoc) {
	dtsgui_treenode root, tmp, tmp2, tln/*, tmp3*/;
	struct xml_search *xp;
	struct xml_node *xn;
	void *iter = NULL;

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
}

dtsgui_treeview network_tree(struct dtsgui *dtsgui) {
	dtsgui_treeview tree;
	struct app_data *appdata;
	struct xml_doc *xmldoc;
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

	tree = dtsgui_treewindow(dtsgui, "Tree Window", NULL, NULL, xmldoc);
	network_tree_setup(tree, xmldoc);
	return tree;
}
