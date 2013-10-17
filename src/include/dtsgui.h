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

/** @file
  * @ingroup CORE C-API
  * @brief Main include file for the GUI Library
  *
  * This file needs to be included to use the defined functions and API.
  * Including it in a C++ file without defining _DTS_C_API will not
  * allow access to the namespace DTS_C_API.
  *
  * It is not recomended to use the C API from C++.
  * @addtogroup CORE
  * @{*/

#ifndef DTSGUI_H_INCLUDED
#define DTSGUI_H_INCLUDED

#include <stdint.h>
#include <dtsapp.h>

/** @brief Callback event for tree view.*/
enum tree_cbtype {
	/**@brief Treenods is selected*/
	DTSGUI_TREE_CB_SELECT,
	/**@brief Treenods is to be deleted*/
	DTSGUI_TREE_CB_DELETE,
	/**@brief Treenods is edited*/
	DTSGUI_TREE_CB_EDIT
};

/** @brief Data storage type on form element*/
enum form_data_type {
	/** @brief Reference to data.*/
	DTSGUI_FORM_DATA_PTR,
	/** @brief Reference to XML Node.*/
	DTSGUI_FORM_DATA_XML
};

/** @brief Forward decleration of the application data class*/
typedef struct dtsgui dtsgui;
/** @brief Forward decleration of wizard class*/
typedef struct dtsgui_wizard dtsgui_wizard;
/** @brief Forward definition of form_item*/
typedef struct form_item form_item;
/** @brief Forward definition of dynamic_panel*/
typedef struct dynamic_panel dynamic_panel;

/** @brief Define menu as void* it is not exported to API*/
typedef void *dtsgui_menu;
/** @brief Define menuitem as void* it is not exported to API*/
typedef void *dtsgui_menuitem;
/** @brief Define panel as void* it is not exported to API*/
typedef void *dtsgui_pane;
/** @brief Define notebook as void* it is not exported to API*/
typedef void *dtsgui_notebook;
/** @brief Define treeview as void* it is not exported to API*/
typedef void *dtsgui_treeview;
/** @brief Define tabview as void* it is not exported to API*/
typedef void *dtsgui_tabview;
/** @brief Define treenode as void* it is not exported to API*/
typedef void *dtsgui_treenode;
/** @brief Define progress as void* it is not exported to API*/
typedef void *dtsgui_progress;

/** @brief Callback called on application execution
  *
  * The callback is called with the application pointer and supplied userdata
  * @see DTSApp::DTSApp()
  * @see dtsgui_config()
  * @param dtsgui Application data ptr.
  * @param data Userdata reference.
  * @return Returning 0 will cause application execution to fail.*/
typedef int (*dtsgui_configcb)(struct dtsgui*, void*);

/** @brief Callback run on panel event
  *
  * When a panel event is triggered and a callback has been registered it will
  * be called.
  * @see panel_events
  * @see panel_buttons
  * @see DTSPanel::SetEventCallback()
  * @see dtsgui_setevcallback()
  * @param dtsgui Application data ptr.
  * @param p Panel that the event originated on.
  * @param type Event type.
  * @param eid Event ID Depends on the event type.
  * @param data Data attached to the event handler.
  * @return A non zero value to allow further processing of the event.*/
typedef int (*event_callback)(struct dtsgui*, dtsgui_pane, int type, int, void*);

/** @brief Calback to configure and return a wxToolbar objecct.
  *
  * @see DTSFrame::SetupToolbar()
  * @see dtsgui_setuptoolbar
  * @param dtsgui Application data ptr.
  * @param window Application frame (Parent window wxWindow)
  * @param style Style to be supplied to wxToolbar constructor.
  * @param wid Window id to be supplied.
  * @param name Name to be supplied.
  * @param data Reference to data supplied when setting upt the tool bar.
  * @return Must return a wxToolbar object.*/
typedef void* (*dtsgui_toolbar_create)(struct dtsgui*, void *, long, int, const char*, void *data);

/** @brief Callback called when a dynamic menu item is selected
  *
  * This function is called and allows returning a pane to be displayed.
  * @see DTSFrame::NewMenuItem()
  * @see dtsgui_newmenucb
  * @param dtsgui Application data ptr.
  * @param name Name of the menu item.
  * @param data Reference to data set when menu is created.
  * @return If a panel is returned it will be displayed.*/
typedef dtsgui_pane (*dtsgui_dynpanel)(struct dtsgui*, const char*, void*);

/** @brief Callback used for tree view events
  *
  * @see DTSTreeWindowEvent::TreeCallback()
  * @see tree_cbtype
  * @param dtsgui Application data ptr.
  * @param tree Treeview.
  * @param node Tree node that generated the event.
  * @param type Callback type.
  * @param title Name of node.
  * @param data Userdata refernece of data passed too treeview.
  * @return If a panel is returned it will be placed int the display area.*/
typedef dtsgui_pane (*dtsgui_tree_cb)(struct dtsgui *, dtsgui_treeview, dtsgui_treenode, enum tree_cbtype, const char*, void*);

/** @brief Callback called after dtsgui_tree_cb to allow configuration of the panel.
  *
  * This callback is stored on the node and allows the panel to be configured per node allowing generic
  * tree callback.
  * @see DTSTreeWindowEvent::TreeCallback()
  * @see DTSDVMListStore::ConfigPanel()
  * @param pane Panel to be configured.
  * @param tree Treeview that contains the node/panel.
  * @param node Node that this panel represents.
  * @param data Reference to user data stored on node.*/
typedef void (*dtsgui_treeviewpanel_cb)(dtsgui_pane, dtsgui_treeview, dtsgui_treenode, void*);

/** @brief Callback called as part of the creation of a new node.
  *
  * This callback will allow manipulation of the newly created node if required.
  * @see dtsgui_newxmltreenode()
  * @see tree_newnode::tree_newnode()
  * @param tree Tree view that contains the node.
  * @param node Newly created tree node.
  * @param xn XML Node assigned to the node.
  * @param data Reference to user data.*/
typedef void (*dtsgui_xmltreenode_cb)(dtsgui_treeview, dtsgui_treenode, struct xml_node*, void*);

/** @brief Configure a tab view panel prior to display.
  *
  * Tab panels are dynanic and created as needed by calling this callback.
  * @see DTSTabPage::ConfigPane()
  * @param pane Panel been configured.
  * @param data Reference to the config data held by panel.*/
typedef void (*dtsgui_tabpanel_cb)(dtsgui_pane, void*);

/** @brief Allow setting the configuration data before tab pane is created.
  *
  * This callback is called as part of the new tab creation to allow setting callback
  * data.The position of the pane can also be set.
  * @see tab_newpane::handle_newtabpane()
  * @param xmldoc XML Document.
  * @param xn XML Node reference.
  * @param data Panel data reference.
  * @param cdata Pointer to reference of cdata can be replaced by setting cdata[0] to new reference.
  * @param pos Pointer to integer containing panel position can be overwritten setting *pos.*/
typedef void (*dtsgui_tabpane_newdata_cb)(struct xml_doc*, struct xml_node*, void*, void**, int*);

/** @brief A simple data structure to store a coordinate*/
struct point {
	/**@brief X axis*/
	int x;
	/**@brief Y axis*/
	int y;
};

/** @brief Define all buttons that are used in creating a panel.
  * @remark These buttons are placed in this order at the bottom of the panel
  * buttons not in the mask are not displayed and are blank spaces*/
enum panel_buttons {
	/**@brief A Rewind button used to indicate select first record.*/
	wx_PANEL_EVENT_BUTTON_FIRST		= 1 << 0,
	/**@brief A Back button used to go one record back.*/
	wx_PANEL_EVENT_BUTTON_BACK		= 1 << 1,
	/**@brief A Forward button indicating to proceed to next record.*/
	wx_PANEL_EVENT_BUTTON_FWD		= 1 << 2,
	/**@brief A Fast Forward button used to indicate select lasr record.*/
	wx_PANEL_EVENT_BUTTON_LAST		= 1 << 3,
	/**@brief A Affirmative button.*/
	wx_PANEL_EVENT_BUTTON_YES		= 1 << 4,
	/**@brief A Cancel/No/Undo button*/
	wx_PANEL_EVENT_BUTTON_NO		= 1 << 5
};

/** @brief Define all possible events that are passed to event handler.*/
enum panel_events {
	/**@brief Button press event.*/
	wx_PANEL_EVENT_BUTTON			= 1 << 0,
	/**@brief Enter was pressed in a Combo box.*/
	wx_PANEL_EVENT_COMBO_ENTER		= 1 << 1,
	/**@brief Combo box text has been updated.*/
	wx_PANEL_EVENT_COMBO_UPDATE		= 1 << 2,
	/**@brief DTS Application event..*/
	wx_PANEL_EVENT_DTS			= 1 << 3
};

/** @brief Options set on tree nodes
  * @remark Not all options apply to containers and leaf nodes.*/
enum tree_newnode_flags {
	/**@brief Allow editing of the node.*/
	DTS_TREE_NEW_NODE_EDIT = 1 << 0,
	/**@brief Allow deleteing the node from right click menu.*/
	DTS_TREE_NEW_NODE_DELETE = 1 << 1,
	/**@brief Node is a Container.*/
	DTS_TREE_NEW_NODE_CONTAINER = 1 << 2,
	/**@brief Container node allows sorting.*/
	DTS_TREE_NEW_NODE_SORT = 1 << 3
};

/** @brief Panel Type.*/
enum panel_type {
	/** @brief DTSWindow (wx_DTSPANEL_WINDOW) is a basic panel without tab traversal controls and is rarely used (DTSFrame::TextPanel).*/
	wx_DTSPANEL_WINDOW,
	/** @brief DTSStaticPanel (wx_DTSPANEL_PANEL) includes tab traversal.*/
	wx_DTSPANEL_PANEL,
	/** @brief DTSScrollPanel (wx_DTSPANEL_SCROLLPANEL) adds scrollbar support.*/
	wx_DTSPANEL_SCROLLPANEL,
	/** @brief DTSDialog (wx_DTSPANEL_DIALOG) is a wx_DTSPANEL_PANEL displayed in a pop up dialog box.*/
	wx_DTSPANEL_DIALOG,
	/** @brief DTSTreeWindow (wx_DTSPANEL_TREE) see @ref C-API-Panel-Tree.*/
	wx_DTSPANEL_TREE,
	/** @brief DTSTabWindow (wx_DTSPANEL_TAB) see @ref C-API-Panel-Tab.*/
	wx_DTSPANEL_TAB,
	/** @brief DTSWizardWindow (wx_DTSPANEL_WIZARD) see @ref C-API-Wizard.*/
	wx_DTSPANEL_WIZARD
};

/** @brief Type of list item.*/
enum widget_type {
	DTS_WIDGET_TEXTBOX,
	DTS_WIDGET_CHECKBOX,
	DTS_WIDGET_LISTBOX,
	DTS_WIDGET_COMBOBOX
};

/** @brief Shortcut flags for navigation buttons.*/
#define wx_PANEL_BUTTON_NAV		wx_PANEL_EVENT_BUTTON_FIRST | wx_PANEL_EVENT_BUTTON_BACK | wx_PANEL_EVENT_BUTTON_FWD | wx_PANEL_EVENT_BUTTON_LAST

/** @brief Shortcut flags for direction buttons.*/
#define wx_PANEL_BUTTON_DIR		wx_PANEL_EVENT_BUTTON_BACK | wx_PANEL_EVENT_BUTTON_FWD

/** @brief Shortcut flags for action buttons.*/
#define wx_PANEL_BUTTON_ACTION wx_PANEL_EVENT_BUTTON_YES | wx_PANEL_EVENT_BUTTON_NO

/** @brief Shortcut flags for all buttons.*/
#define wx_PANEL_BUTTON_ALL wx_PANEL_BUTTON_ACTION | wx_PANEL_BUTTON_NAV

/** @brief Shortcut flags for no buttons.*/
#define wx_PANEL_EVENT_BUTTON_NONE 0

/**@}*/

/*
 * These are used in C only or when __DTS_C_API is defined in the DTS_C_API namespace
 */

#ifdef __cplusplus
extern "C" {
namespace DTS_C_API {
#endif /* __cplusplus*/

#if !defined(__cplusplus) || defined(__DTS_C_API)
/*app frame config and control*/
void dtsgui_config(dtsgui_configcb confcallback_cb, void *userdata, struct point *wsize,
					  struct point *wpos, const char *title, const char *status);
int dtsgui_run(int argc, char **argv);
void *dtsgui_userdata(struct dtsgui *dtsgui);
void dtsgui_titleappend(struct dtsgui *dtsgui, const char *text);
void dtsgui_sendevent(struct dtsgui *dtsgui, int eid);
void dtsgui_setwindow(struct dtsgui *dtsgui, dtsgui_pane p);
void dtsgui_setuptoolbar(struct dtsgui *dtsgui, dtsgui_toolbar_create cb, void *data);
void dtsgui_set_toolbar(struct dtsgui *dtsgui, int show);
void dtsgui_setstatus(dtsgui_pane pane, const char *status);

/*menu configuration*/
dtsgui_menu dtsgui_newmenu(struct dtsgui *dtsgui, const char *name);
dtsgui_menuitem dtsgui_newmenucb(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, const char *hint, const char *title, int blank, dtsgui_dynpanel cb, void *data);
dtsgui_menuitem dtsgui_newmenuitem(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, const char *hint, dtsgui_pane pane);

/*menu enable/disable*/
void dtsgui_menuitemenable(dtsgui_menuitem dmi, int enable);

/*predefined menus*/
void dtsgui_menusep(dtsgui_menu dtsmenu);
void dtsgui_about(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, const char *text);
void dtsgui_close(dtsgui_menu dtsmenu, struct dtsgui *dtsgui);
void dtsgui_exit(dtsgui_menu dtsmenu, struct dtsgui *dtsgui);

/*panel config*/
dtsgui_pane dtsgui_panel(struct dtsgui *dtsgui, const char *name, const char *title, int butmask, enum panel_type type, void *userdata);
dtsgui_pane dtsgui_textpane(struct dtsgui *dtsgui, const char *title, const char *buf);
void dtsgui_settitle(dtsgui_pane pane, const char *title); /*unused*/
void dtsgui_delpane(dtsgui_pane pane);
void *dtsgui_paneldata(dtsgui_pane pane);
void dtsgui_setevcallback(dtsgui_pane pane,event_callback evcb, void *data);
void dtsgui_configcallback(dtsgui_pane pane,dtsgui_configcb cb, void *data); /*unused*/

/*panel xml set/get*/
void dtsgui_panel_setxml(dtsgui_pane pane, struct xml_doc *xmldoc);
struct xml_doc *dtsgui_panelxml(dtsgui_pane pane);
void dtsgui_xmlpanel_update(dtsgui_pane pane);

/*tab page*/
dtsgui_tabview dtsgui_tabwindow(struct dtsgui *dtsgui, const char *title, void *data);
dtsgui_pane dtsgui_newtabpage(dtsgui_tabview tv, const char *name, int butmask, void *userdata, struct xml_doc *xmldoc, dtsgui_tabpanel_cb cb, void *cdata);
dtsgui_pane dtsgui_tabpage_insert(dtsgui_tabview tv, const char *name, int butmask, void *userdata, struct xml_doc *xmldoc, dtsgui_tabpanel_cb cb, void *cdata, int pos, int undo);
void dtsgui_newxmltabpane(dtsgui_tabview tabv, dtsgui_pane p, const char *xpath, const char *node, const char *vitem, const char *tattr, dtsgui_tabpane_newdata_cb data_cb, dtsgui_tabpanel_cb cb, void *cdata, struct xml_doc *xmldoc, void *data);

/*tree pane*/
dtsgui_treeview dtsgui_treewindow(struct dtsgui *dtsgui, const char *title, dtsgui_tree_cb tree_cb, void *userdata, struct xml_doc *xmldoc);
dtsgui_pane dtsgui_treepane(dtsgui_treeview tv, const char *name, int butmask, void *userdata, struct xml_doc *xmldoc);
dtsgui_pane dtsgui_treepane_default(dtsgui_treeview self, dtsgui_treenode node);
void dtsgui_nodesetxml(dtsgui_treeview tree, dtsgui_treenode node, const char *title);
dtsgui_treenode dtsgui_treecont(dtsgui_treeview tree, dtsgui_treenode node, const char *title, int can_edit, int can_sort, int can_del, int nodeid, dtsgui_treeviewpanel_cb p_cb,void *data);
dtsgui_treenode dtsgui_treeitem(dtsgui_treeview tree, dtsgui_treenode node, const char *title, int can_edit, int can_sort, int can_del, int nodeid, dtsgui_treeviewpanel_cb p_cb, void *data);
void dtsgui_treenodesetxml(dtsgui_treenode tn,struct xml_node *xn, const char *tattr);
struct xml_node *dtsgui_treenodegetxml(dtsgui_treenode tn, char **buf);
void *dtsgui_treenodegetdata(dtsgui_treenode tn);
const char *dtsgui_treenodeparent(dtsgui_treenode tn);
void dtsgui_newxmltreenode(dtsgui_treeview tree, dtsgui_pane p, dtsgui_treenode tn, const char *xpath, const char *node, const char *vitem, const char *tattr,
								int nid, int flags, dtsgui_xmltreenode_cb node_cb, void *data, dtsgui_treeviewpanel_cb p_cb);
int dtsgui_treenodeid(dtsgui_treenode tn);

/*convert panel elements to a xml node with attributes*/
struct xml_node *dtsgui_panetoxml(dtsgui_pane p, const char *xpath, const char *node, const char *nodeval, const char *attrkey);


/*run modal dialog*/
void dtsgui_rundialog(dtsgui_pane pane, event_callback evcb, void *data);

/*form items
 *list/combo box must be unrefed when all items added
 */
extern void dtsgui_textbox(dtsgui_pane pane, const char *title, const char *name, const char *value, void *data);
extern void dtsgui_textbox_multi(dtsgui_pane pane, const char *title, const char *name, const char *value, void *data);
extern void dtsgui_passwdbox(dtsgui_pane pane, const char *title, const char *name, const char *value, void *data);
extern void dtsgui_checkbox(dtsgui_pane pane, const char *title, const char *name, const char *checkval, const char *uncheck, int ischecked, void *data);
struct form_item *dtsgui_listbox(dtsgui_pane pane, const char *title, const char *name, void *data);
struct form_item *dtsgui_combobox(dtsgui_pane pane, const char *title, const char *name, void *data);

/*XML form items - see above for need to unref list/combo boxes*/
extern void dtsgui_xmltextbox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr);
extern void dtsgui_xmltextbox_multi(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr);
extern void dtsgui_xmlpasswdbox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr);
extern void dtsgui_xmlcheckbox(dtsgui_pane pane, const char *title, const char *name, const char *checkval, const char *uncheckval, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr);
struct form_item *dtsgui_xmllistbox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr);
struct form_item *dtsgui_xmlcombobox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr);

/*XML form items - see above for need to unref list/combo boxes*/
/*
extern void dtsgui_xnode_textbox(dtsgui_pane pane, const char *title, const char *attr);
extern void dtsgui_xnode_textbox_multi(dtsgui_pane pane, const char *title, const char *attr);
extern void dtsgui_xnode_passwdbox(dtsgui_pane pane, const char *title, const char *attr);
extern void dtsgui_xnode_checkbox(dtsgui_pane pane, const char *title, const char *checkval, const char *uncheckval, const char *attr);
struct form_item *dtsgui_xnode_listbox(dtsgui_pane pane, const char *title, const char *attr);
struct form_item *dtsgui_xnode_combobox(dtsgui_pane pane, const char *title, const char *attr);*/

/*add item to list*/
void dtsgui_listbox_add(struct form_item *lbox, const char *text, const char *value);
void dtsgui_listbox_addxml(struct form_item *lb, struct xml_doc *xmldoc, const char *xpath, const char *nattr, const char *vattr);
void dtsgui_listbox_set(struct form_item *listbox, int idx);

/*message box's*/
int dtsgui_confirm(struct dtsgui *dtsgui, const char *text);
void dtsgui_alert(struct dtsgui *dtsgui, const char *text);

/*progress bar*/
int dtsgui_progress_start(struct dtsgui *dtsgui, const char *text, int maxval, int quit);
int dtsgui_progress_update(struct dtsgui *dtsgui, int newval, const char* newtext);
int dtsgui_progress_increment(struct dtsgui *dtsgui, int ival, const char* newtext);
void dtsgui_progress_end(struct dtsgui *dtsgui);

/* get bucket list of form items*/
struct bucket_list *dtsgui_panel_items(dtsgui_pane pane);
void *dtsgui_item_data(struct form_item *fi);
const char *dtsgui_item_name(struct form_item *fi);
const char *dtsgui_item_value(struct form_item *fi);
struct form_item *dtsgui_finditem(dtsgui_pane p, const char *name);
const char *dtsgui_findvalue(dtsgui_pane p, const char *name);

/*Wizards*/
struct dtsgui_wizard *dtsgui_newwizard(struct dtsgui *dtsgui, const char *title);
dtsgui_pane dtsgui_wizard_addpage(struct dtsgui_wizard *dtswiz, const char *title, void *userdata, struct xml_doc *xmldoc);
int dtsgui_runwizard(struct dtsgui_wizard *dtswiz);

/* returned values need to be unref'd*/
const char *dtsgui_filesave(struct dtsgui *dtsgui, const char *title, const char *path, const char *name, const char *filter);
const char *dtsgui_fileopen(struct dtsgui *dtsgui, const char *title, const char *path, const char *name, const char *filter);

/*utils*/
void dtsgui_menuenable(dtsgui_menu dm, int enable);
struct curl_post *dtsgui_pane2post(dtsgui_pane p);

/* returns auth struct needs to be un-ref'd*/
struct basic_auth *dtsgui_pwdialog(const char *user, const char *passwd,void *data);

#ifdef __WIN32
void getwin32folder(int csidl, char *path);
#endif /* __WIN32*/

#endif /* __cplusplus || __DTS_C_API*/
#ifdef __cplusplus
}}

#include <wx/app.h>

wxDECLARE_EVENT(DTS_APP_EVENT, wxCommandEvent);
#endif /* __cplusplus extern and namespace*/
#endif /* DTSGUI_H_INCLUDED*/
