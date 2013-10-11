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

/** @addtogroup DTS-APP
  * @{
  * @file
  * @brief Main application private includes.*/

#ifndef PRIVATE_H_INCLUDED
#define PRIVATE_H_INCLUDED


/** @brief Shared application folder.
  *
  * This should be set correctly at build time and this value ignored.*/
#ifndef DATA_DIR
#define DATA_DIR	"/usr/share/dtsguiapp"
#endif

/** @brief Node ID's used in application treeview*/
enum node_id {
	DTS_NODE_CUSTOMER,
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
	DTS_NODE_NETWORK_MODEM_ADV,
	DTS_NODE_NETWORK_MODEM_ANA,
	DTS_NODE_NETWORK_ADSL_LINK_NEW,
	DTS_NODE_NETWORK_ADSL_LINK,
	DTS_NODE_NETWORK_ADSL_USER_NEW,
	DTS_NODE_NETWORK_ADSL_USER,
	DTS_NODE_NETWORK_TOS_NEW,
	DTS_NODE_NETWORK_TOS
};

/** @brief Application data structure*/
struct app_data {
	/** @brief XML Document been edited.*/
	struct xml_doc *xmldoc;
	/** @brief Shared datadir.*/
	const char *datadir;
	/** @brief Active configuration.*/
	const char *openconf;
	/** @brief New system configuration menu item.*/
	dtsgui_menuitem n_wiz;
	/** @brief Existing system configuration menu item.*/
	dtsgui_menuitem e_wiz;
	/** @brief Open configuration menu item.*/
	dtsgui_menuitem c_open;
	/** @brief Configuration menu.*/
	dtsgui_menu cfg_menu;
};

/** @brief Name value pair sturucture.
  *
  * This is used to create a static array to to populate listbox.*/
struct listitem {
	/** @brief Name of the pair.*/
	const char *name;
	/** @brief Value of the pair.*/
	const char *value;
};

/** @}*/

#ifdef __cplusplus
extern "C" {
#endif
struct xml_doc *app_getxmldoc(struct dtsgui *dtsgui);

/*toolbar.cpp*/
void *app_toolbar(struct dtsgui *dtsgui, void *pw, long style, int id, const char *name, void *data);

/*wizard.c*/
dtsgui_pane reconfig_wizard(struct dtsgui *dtsgui, const char *title, void *data);
dtsgui_pane editsys_wizard(struct dtsgui *dtsgui, const char *title, void *data);
dtsgui_pane newsys_wizard(struct dtsgui *dtsgui, const char *title, void *data);
void wizz_custinfo(dtsgui_pane *pg);

/*pbxconfig.c*/
dtsgui_pane pbx_settings(struct dtsgui *dtsgui, const char *title, void *data);

/*testing.c*/
void test_menu(struct dtsgui *dtsgui, dtsgui_menu menu, const char *url);

/*netconfig.c*/
dtsgui_pane advanced_config(struct dtsgui *dtsgui, const char *title, void *data);
void network_iface_pane(dtsgui_pane p, const char *xpre, const char *iface);
void network_iface_new_pane(dtsgui_pane p);

/*iface*/
dtsgui_pane iface_config(struct dtsgui *dtsgui, const char *title, void *data);

/*config.c*/
dtsgui_pane open_config(struct dtsgui *dtsgui, const char *title, void *data);
dtsgui_pane save_config(struct dtsgui *dtsgui, const char *title, void *data);
dtsgui_pane export_config(struct dtsgui *dtsgui, const char *title, void *data);
dtsgui_pane view_config_conf(struct dtsgui *dtsgui, const char *title, void *data);
dtsgui_pane view_config_xml(struct dtsgui *dtsgui, const char *title, void *data);
void config_menu(struct dtsgui *dtsgui);

#ifdef __cplusplus
}
#endif
#endif /*PRIVATE_H_INCLUDED*/
