#ifndef PRIVATE_H_INCLUDED
#define PRIVATE_H_INCLUDED

enum node_id {
	DTS_NODE_DNS_CONFIG,
	DTS_NODE_NETWORK_CONFIG,
	DTS_NODE_NETWORK_IFACE_NEW,
	DTS_NODE_NETWORK_IFACE,
	DTS_NODE_NETWORK_WIFI_NEW,
	DTS_NODE_NETWORK_WIFI
};

struct tree_data {
	int nodeid;
	const char *xpath;
	const char *tattr;
	struct xml_node *node;
	dtsgui_treeview tree;
	dtsgui_treenode treenode;
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


int reconfig_wizard(struct dtsgui *dtsgui, void *data);
int editsys_wizard(struct dtsgui *dtsgui, void *data);
int newsys_wizard(struct dtsgui *dtsgui, void *data);

dtsgui_pane pbx_settings(struct dtsgui *dtsgui, const char *title, void *data);

void handle_test(dtsgui_pane p, int type, int event, void *data);
int post_test(struct dtsgui *dtsgui, void *data);
void handle_test(dtsgui_pane p, int type, int event, void *data);
void test_menu(struct dtsgui *dtsgui);
void testpanel(dtsgui_pane p);

struct tree_data *gettreedata(dtsgui_treeview tree, struct xml_node *xn, const char *tattr, enum node_id nid);

dtsgui_treeview network_tree(struct dtsgui *dtsgui);

#endif /*PRIVATE_H_INCLUDED*/
