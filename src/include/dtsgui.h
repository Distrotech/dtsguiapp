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

#ifndef DTSGUI_H_INCLUDED
#define DTSGUI_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#else
/*application struct*/
typedef struct dtsgui dtsgui;
typedef struct dtsgui_wizard dtsgui_wizard;
#endif

/*menu controls*/
typedef void *dtsgui_menu;
typedef void *dtsgui_menuitem;

/*viewable windows*/
typedef void *dtsgui_pane;
typedef void *dtsgui_notebook;
typedef void *dtsgui_treeview;
typedef void *dtsgui_tabview;

/*forward def*/
typedef struct form_item form_item;
typedef struct dynamic_panel dynamic_panel;

/*callbacks*/
typedef int (*dtsgui_configcb)(struct dtsgui*, void*);
typedef void (*event_callback)(dtsgui_pane, int, int, void *);
typedef dtsgui_pane (*dtsgui_dynpanel)(struct dtsgui*, const char*, void*);
typedef void (*dtsgui_tree_cb)(struct dtsgui *, dtsgui_treeview, const char*, void*, void*);

struct point {
	int x;
	int y;
};

/*buttons YES / No may be YES/NO APPLY/REFRESH OK/CANCEL*/
enum panel_buttons {
	wx_PANEL_BUTTON_FIRST	= 1 << 0,
	wx_PANEL_BUTTON_BACK	= 1 << 1,
	wx_PANEL_BUTTON_FWD		= 1 << 2,
	wx_PANEL_BUTTON_LAST	= 1 << 3,
	wx_PANEL_BUTTON_YES		= 1 << 4,
	wx_PANEL_BUTTON_NO		= 1 << 5
};

enum panel_type {
	wx_DTSPANEL_WINDOW,
	wx_DTSPANEL_PANEL,
	wx_DTSPANEL_SCROLLPANEL,
	wx_DTSPANEL_DIALOG,
	wx_DTSPANEL_TREE,
	wx_DTSPANEL_WIZARD,
	wx_DTSPANEL_TAB
};

enum widget_type {
	DTS_WIDGET_TEXTBOX,
	DTS_WIDGET_CHECKBOX,
	DTS_WIDGET_LISTBOX,
	DTS_WIDGET_COMBOBOX
};

#define wx_PANEL_BUTTON_NAV		wx_PANEL_BUTTON_FIRST | wx_PANEL_BUTTON_BACK | wx_PANEL_BUTTON_FWD | wx_PANEL_BUTTON_LAST
#define wx_PANEL_BUTTON_DIR		wx_PANEL_BUTTON_BACK | wx_PANEL_BUTTON_FWD
#define wx_PANEL_BUTTON_ACTION	wx_PANEL_BUTTON_YES | wx_PANEL_BUTTON_NO
#define wx_PANEL_BUTTON_ALL		wx_PANEL_BUTTON_ACTION | wx_PANEL_BUTTON_NAV
#define wx_PANEL_BUTTON_NONE	0

/*application config and control*/
dtsgui *dtsgui_config(dtsgui_configcb confcallback_cb, void *userdata, struct point wsize,
					  struct point wpos, const char *title, const char *status);
int dtsgui_run(int argc, char **argv);
void *dtsgui_userdata(struct dtsgui *dtsgui);
void dtsgui_titleappend(struct dtsgui *dtsgui, const char *text);
void dtsgui_reconfig(struct dtsgui *dtsgui);
void dtsgui_setblank(struct dtsgui *dtsgui);

/*menu configuration*/
dtsgui_menu dtsgui_newmenu(struct dtsgui *dtsgui, const char *name);
dtsgui_menuitem dtsgui_newmenucb(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, const char *hint, const char *label, dtsgui_configcb, void *data);
dtsgui_menuitem dtsgui_newmenudyn(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, const char *title, const char *hint, dtsgui_dynpanel cb,void *data, struct dynamic_panel **d_pane);
dtsgui_menuitem dtsgui_newmenuitem(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, const char *hint, dtsgui_pane pane);
void dtsgui_menuitemenable(dtsgui_menuitem dmi, int enable);
void dtsgui_menuenable(dtsgui_menu dm, int enable);
void dtsgui_menusep(dtsgui_menu dtsmenu);
void dtsgui_about(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, const char *text);
void dtsgui_close(dtsgui_menu dtsmenu, struct dtsgui *dtsgui);
void dtsgui_exit(dtsgui_menu dtsmenu, struct dtsgui *dtsgui);

/*view config*/
dtsgui_pane dtsgui_panel(struct dtsgui *dtsgui, const char *name, int butmask, enum panel_type type, void *userdata);
void dtsgui_xmlpanel(dtsgui_pane pane, struct xml_doc *xmldoc);
void dtsgui_delpane(dtsgui_pane pane);
dtsgui_pane dtsgui_textpane(struct dtsgui *dtsgui, const char *title, const char *buf);
dtsgui_treeview dtsgui_treewindow(struct dtsgui *dtsgui, const char *title, dtsgui_tree_cb tree_cb);
dtsgui_tabview dtsgui_tabwindow(struct dtsgui *dtsgui, const char *title);
dtsgui_pane dtsgui_addpage(dtsgui_tabview tv, const char *name, int butmask, void *userdata, struct xml_doc *xmldoc);
dtsgui_pane dtsgui_treepane(dtsgui_treeview tv, const char *name, int butmask, void *userdata, struct xml_doc *xmldoc);
void dtsgui_treeshow(dtsgui_treeview tv, dtsgui_pane p);

void dtsgui_showpanel(dtsgui_pane pane, int act);
void dtsgui_rundialog(dtsgui_pane pane, event_callback evcb, void *data);
void dtsgui_xmlpanel_update(dtsgui_pane pane);
void *dtsgui_paneldata(dtsgui_pane pane);
void dtsgui_closedyn(struct dtsgui *dtsgui, struct dynamic_panel *dpane);
void dtsgui_createdyn(struct dtsgui *dtsgui, struct dynamic_panel *dpane);

/*form items
 *list/combo box must be unrefed when all items added
 */
extern void dtsgui_textbox(dtsgui_pane pane, const char *title, const char *name, const char *value, void *data);
extern void dtsgui_textbox_multi(dtsgui_pane pane, const char *title, const char *name, const char *value, void *data);
extern void dtsgui_passwdbox(dtsgui_pane pane, const char *title, const char *name, const char *value, void *data);
extern void dtsgui_checkbox(dtsgui_pane pane, const char *title, const char *name, int ischecked, void *data);
struct form_item *dtsgui_listbox(dtsgui_pane pane, const char *title, const char *name, void *data);
struct form_item *dtsgui_combobox(dtsgui_pane pane, const char *title, const char *name, void *data);

/*XML form items - see above for need to unref list/combo boxes*/
extern void dtsgui_xmltextbox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *attr);
extern void dtsgui_xmltextbox_multi(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *attr);
extern void dtsgui_xmlpasswdbox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *attr);
extern void dtsgui_xmlcheckbox(dtsgui_pane pane, const char *title, const char *name, const char *checkval, const char *uncheckval, const char *xpath, const char *attr);
struct form_item *dtsgui_xmllistbox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *attr);
struct form_item *dtsgui_xmlcombobox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *attr);
void dtsgui_item_xmlcreate(dtsgui_pane pane, const char *path, const char *node, const char *attr);

/*add item to list*/
void dtsgui_listbox_add(struct form_item *lbox, const char *text, const char *value);

/* returns auth struct needs to be un-ref'd*/
struct basic_auth *dtsgui_pwdialog(const char *user, const char *passwd,void *data);

/*message box's*/
int dtsgui_confirm(struct dtsgui *dtsgui, const char *text);
void dtsgui_alert(struct dtsgui *dtsgui, const char *text);

/*set callback for a pane*/
void dtsgui_setevcallback(dtsgui_pane pane,event_callback evcb, void *data);
void dtsgui_configcallback(dtsgui_pane pane,dtsgui_configcb cb, void *data);

/* get bucket list of form items*/
struct bucket_list *dtsgui_panel_items(dtsgui_pane pane);
void *dtsgui_item_data(struct form_item *fi);
const char *dtsgui_item_name(struct form_item *fi);
const char *dtsgui_item_value(struct form_item *fi);
struct form_item *dtsgui_finditem(dtsgui_pane p, const char *name);
const char *dtsgui_findvalue(dtsgui_pane p, const char *name);

/*Wizards*/
struct dtsgui_wizard* dtsgui_newwizard(struct dtsgui *dtsgui, const char *title);
dtsgui_pane dtsgui_wizard_addpage(struct dtsgui_wizard *dtswiz, const char *title, void *userdata, struct xml_doc *xmldoc);
int dtsgui_runwizard(struct dtsgui_wizard *dtswiz);

/* returned values need to be unref'd*/
const char *dtsgui_filesave(struct dtsgui *dtsgui, const char *title, const char *path, const char *name, const char *filter);
const char *dtsgui_fileopen(struct dtsgui *dtsgui, const char *title, const char *path, const char *name, const char *filter);

struct xml_doc *dtsgui_loadxmlurl(struct dtsgui *dtsgui, const char *user, const char *passwd, const char *url);

#ifdef __WIN32
void getwin32folder(int csidl, char *path);
#endif

#ifdef __cplusplus
}
#endif
#endif
