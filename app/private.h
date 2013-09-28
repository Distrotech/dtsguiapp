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

#ifndef PRIVATE_H_INCLUDED
#define PRIVATE_H_INCLUDED

enum node_id {
	DTS_NODE_NETWORK_CONFIG,
	DTS_NODE_NETWORK_CONFIG_DNS,
	DTS_NODE_NETWORK_CONFIG_DNS_DYN,
	DTS_NODE_NETWORK_CONFIG_DNS_SERV,
	DTS_NODE_NETWORK_CONFIG_DNS_ZONE,
	DTS_NODE_NETWORK_CONFIG_DNS_HOST_NEW,
	DTS_NODE_NETWORK_CONFIG_DNS_HOST,
	DTS_NODE_NETWORK_CONFIG_DNS_DOMAIN_NEW,
	DTS_NODE_NETWORK_CONFIG_DNS_DOMAIN,
	DTS_NODE_NETWORK_CONFIG_DNS_DOMAIN_SERVER_NEW,
	DTS_NODE_NETWORK_CONFIG_DNS_DOMAIN_SERVER,
	DTS_NODE_NETWORK_IFACE_NEW,
	DTS_NODE_NETWORK_IFACE,
	DTS_NODE_NETWORK_WIFI_NEW,
	DTS_NODE_NETWORK_WIFI,
	DTS_NODE_NETWORK_WAN_NEW,
	DTS_NODE_NETWORK_WAN,
	DTS_NODE_NETWORK_ROUTE_NEW,
	DTS_NODE_NETWORK_ROUTE,
	DTS_NODE_NETWORK_MODEM,
	DTS_NODE_NETWORK_MODEM_ANA,
	DTS_NODE_NETWORK_ADSL_LINK_NEW,
	DTS_NODE_NETWORK_ADSL_LINK,
	DTS_NODE_NETWORK_ADSL_USER_NEW,
	DTS_NODE_NETWORK_ADSL_USER,
	DTS_NODE_NETWORK_TOS_NEW,
	DTS_NODE_NETWORK_TOS
};

struct app_data {
	struct dtsgui *dtsgui;
	struct xml_doc *xmldoc;
	const char *datadir;
	const char *openconf;
	dtsgui_menuitem n_wiz;
	dtsgui_menuitem e_wiz;
	dtsgui_menuitem c_open;
	dtsgui_menu cfg_menu;
	struct dynamic_panel *dyn_cfg;
};

struct listitem {
	const char *name;
	const char *value;
};

/*wizard.c*/
int reconfig_wizard(struct dtsgui *dtsgui, void *data);
int editsys_wizard(struct dtsgui *dtsgui, void *data);
int newsys_wizard(struct dtsgui *dtsgui, void *data);

/*pbxconfig.c*/
dtsgui_pane pbx_settings(struct dtsgui *dtsgui, const char *title, void *data);

/*testing.c*/
void handle_test(dtsgui_pane p, int type, int event, void *data);
int post_test(struct dtsgui *dtsgui, void *data);
void handle_test(dtsgui_pane p, int type, int event, void *data);
void test_menu(struct dtsgui *dtsgui);
void testpanel(dtsgui_pane p);

/*netconfig.c*/
dtsgui_treeview network_tree(struct dtsgui *dtsgui);

#endif /*PRIVATE_H_INCLUDED*/
