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

/** @defgroup CORE Core API Interface Common to C and C++
  * @brief Core elements shared by all API's*/

/** @defgroup C-API Distrotech GUI Library C API
  * @brief Exported functions used for use in C code.
  * @remark Use of this API is not recomended from inside C++ use the native API.
  * @see __DTS_C_API
  * @see DTS_C_API
  * @see CORE
  * @see @ref wrapper.cpp
  * @see @ref cxxapi.cpp
  * @see @ref dtsgui.h*/

/** @defgroup CPP Native C++ API
  * @brief C++ Interface*/

/** @file
  * @brief Wrapper functions exported to C to allow interfacing with C++.
  * @ingroup C-API
  * @see DTS_C_API
  * @see __DTS_C_API
  * @see @ref C-API
  *
  * These functions should not be used in C++ applications.*/

/** @defgroup C-API-Menus Menu bar interface.
  * @brief Create and manage menus.
  * @ingroup C-API
  * @remark The menu bar at the top of the application contains menus,
  * these menus contain menuitems these are the primary control interface.
  * @remark Menu items when selected can call a function or place a pre created 
  * panel in the display area. If the function returns a panel it will be placed
  * in the display area.
  * @remark Menuitems can also be enabled / disabled programatically,
  * disabled menu items are shown greyed out.
  * @todo Define and implement Exit/Quit events*/

/** @defgroup C-API-Panel Display panel interface.
  * @brief Create and manage display panels shown in the application display area.
  * @ingroup C-API
  * @remark The application display area can display one panel at a time these panels
  * may be static or dynamic. static panels are created once and persist during execution.
  * dynamic panels are created and displayed each time the associated menuitem is selected.
  * @see panel_type
  * @warning The displayed dynamic panel will be removed when a new item is selected.
  * @todo Emit and allow panels to manage a event when a new panel is requested to allow saving
  * the displayed panel.*/

/** @defgroup C-API-Panel-Tree Tree view panel interface.
  * @brief A tree view is a split window with a tree in the left window and display area on right.
  * @ingroup C-API-Panel
  * @remark The tree can contain branches (folder containers) and leaf nodes each node when clicked will
  * create and place a panel in the display area.
  * @remark The tree window supports sorting and deletion via a menu displayed when right clicking.
  * @todo Allow for the deletion of branch/container/folder nodes.*/

/** @defgroup C-API-Panel-Tab Tab view panel interface.
  * @brief A tree view is a panel with tabs along the top and display area underneath.
  * @ingroup C-API-Panel
  * @todo Implement a right click action to allow for deletion via a confirmation dialog.
  * @remark The tab window allows grouping related panels together via the tab control
  * when the tab is clicked its display area displays the panel associated with this control.*/

/** @defgroup C-API-Panel-Elements Panel element interface.
  * @brief Interface for creating and placing elements on the panel.
  * @ingroup C-API-Panel
  * @todo Add a JSON API
  * @see @ref C-API-Panel-Items
  * @remark Element items include Text / Password / Check / List / Combo boxes.
  * @remark They are placed with the title on the left and element on the right.
  * @remark Elements are accessed via the item interface.
  * @remark There multiple API's for managing elements but a common interface for
  * controling list and combo boxes.*/

/** @defgroup C-API-Panel-Elements-STD Basic Elements.
  * @brief Elements consisting Name/Value/Data.
  * @ingroup C-API-Panel-Elements
  * @note The data is a reference to a referenced object.*/

/** @defgroup C-API-Panel-Elements-XML XML Elements
  * @brief Elements derived from a XML node.  
  * @ingroup C-API-Panel-Elements
  * @see DTSPanel::GetNode()
  * @todo Adding functions to work from a search path will be better than
  * the current interface that alloacates a new search for each element.
  * @remark XML elements require a xpath to the parent , node name , filter attribute , filter value
  * and attribute name to derive the value from.
  * @remark If a node name is supplied a XML node will be attempted to be created.
  * @remark If no XML node is found or can be created the element will be disabled.
  * @remark the resulting path will be
  * @verbatim XPATH/Node<[<Filter attribute|.> = 'Filter Value']>@endverbatim
  * @note node, filter attribute, filter value and attribute may be NULL.
  * @note If the node name is not supplied a node will not be created.
  * @note If the attribute is NULL the value of the node will be used for value.
  * @note If no filter value is supplied the filter attribute is ignored*/

/** @defgroup C-API-Panel-Items Panel item interface.
  * @brief Access and manage panel elements in a consistant way.
  * @ingroup C-API-Panel
  * @see @ref C-API-Panel-Elements
  * @remark Element items include Text / Password / Check / List / Combo boxes.
  * @remark Information of each element is stored in a hashed bucket to provide
  * a consistant interface to the values and references to data (XML).*/

/** @defgroup C-API-Dialog User interface dialogs.
  * @brief Common dialogs for user interaction Confirmation, File, Password.
  * @ingroup C-API
  * @remark Custom dialog boxes can be created using DTSDialog panels.
  * @remark All implementations of the wxFileDialog provide a wildcard filter.
  * Typing a filename containing wildcards (*, ?) in the filename text item, and clicking on Ok, will 
  * result in only those files matching the pattern being displayed.
  * The wildcard may be a specification for multiple types of file with a description for each, such as:
  * @verbatim"BMP and GIF files (*.bmp;*.gif)|*.bmp;*.gif|PNG files (*.png)|*.png"@endverbatim
  * Copied from wxFileDialog Documentation*/

/** @defgroup C-API-Wizard Wizard Interface.
  * @brief Multi panel pop up wizard user interface.
  * @ingroup C-API
  * @remark A wizard is a dialog box that allows for multiple panels to be dispalayed
  * in sequence via Back/Next controls.
  * @note It is important to process the panels on complition before un refrencing the
  * wizard.
  * @todo Add a hashed list to the wizard class storing the panels to no longer need to
  * keep a array of panels.*/

/** @defgroup C-API-Progress Progress dialog interface.
  * @brief Popup dialog containing a progress bar and posible cancel button.
  * @ingroup C-API
  * @todo Allow multiple instances of progress dialog not only the single interface at present.*/

/**
  * @ingroup C-API
  * @brief Defining __DTS_C_API allows access to C API from inside a C++ file
  * 
  * dtsgui.h will only include the definitions for the C API using C++ if this is defined.*/
#define __DTS_C_API

#include <wx/menu.h>
#include <wx/filedlg.h>
#include <wx/toolbar.h>

#include "dtsgui.h"

#include "DTSFrame.h"
#include "DTSApp.h"
#include "DTSTreeWindow.h"
#include "pitems.h"
#include "DTSWizard.h"
#include "DTSListView.h"

/**
  * These functions should not be used in C++ code use of the native API is recomended.
  * @namespace DTS_C_API
  * @ingroup C-API
  * @brief Namespace containing exported functions used by C.
  *
  * Allocate all exported fuctions for C to the namespace DTS_C_API
  * Functions in this namespace are wrappers and should not be used in C++ code.*/
namespace DTS_C_API {

/** @ingroup C-API
  * @brief Static variable containing menuid incremented each time a menu is created.*/
static int menuid = wxID_AUTO_LOWEST;

/** @ingroup C-API
  * @brief Configure the GUI and create a application frame.
  *
  * This is the first process required the application is created and the callback
  * is executed.
  *
  * Application processing continues until dtsgui_run is executed at this point
  * the call back is executed and proccessing will be passed to the GUI App.
  *
  * The userdata is returned in the callback.
  *
  * The Callback must return a non zero value to indicate success should
  * the application fail returning 0 will close the application.
  *
  * @see DTSApp
  * @see dtsgui_configcb
  *
  * @param confcallback_cb The callback function where processing will continue.
  * @param userdata a refrenced object passed back to the config callback.
  * @param wsize Minimum/Initial size of the application window.
  * @param wpos Application position [Top Left Corner] of application placed on screen.
  * @param title Title displayed on the application bar.
  * @param status Default text displayed on the status bar.
  * @return The fuction does not return.*/
void dtsgui_config(dtsgui_configcb confcallback_cb, void *userdata, struct point wsize, struct point wpos, const char *title, const char *status) {
	new DTSApp(confcallback_cb, userdata, wsize, wpos, title, status);
}

/** @ingroup C-API
  * @brief Pass execution to the GUI and activate the frame.
  *
  * Pass application processing to the GUI after calling the configure callback.
  *
  * @see dtsgui_config
  * @see DTSApp
  *
  * @param argc Number of paramaters contained in argv.
  * @param argv Array of paramaters to be passed.
  * @return Return value from callback function.*/
int dtsgui_run(int argc, char **argv) {
	return wxEntry(argc, argv);
}

/** @ingroup C-API
  * @brief Obtain a reference for the application userdata.
  *
  * @note This reference must be un refferenced.
  *
  * @see dtsgui::GetUserData
  *
  * @param dtsgui Application data ptr.
  * @return Reference to application user data*/
void *dtsgui_userdata(struct dtsgui *dtsgui) {
	return dtsgui->GetUserData();
}

/** @ingroup C-API
  * @brief Set the displayed panel to pane provided.
  *
  * Switch the curent pane with the one provided this
  * should not be done and rather use menu callbacks.
  *
  * @see DTSFrame::SetWindow()
  *
  * @param dtsgui Application data ptr
  * @param p Panel to display*/
void dtsgui_setwindow(struct dtsgui *dtsgui, dtsgui_pane p) {
	DTSFrame *f = dtsgui->GetFrame();
	f->SetWindow((DTSPanel*)p);
}

/** @ingroup C-API
  * @brief Emit a event handled by event callback's.
  *
  * A event is emited and recived by all active handlers
  * The ID can be determined as required.
  * @see DTSFrame::SendDTSEvent()
  * @param dtsgui Application data ptr
  * @param eid Event ID to send*/
void dtsgui_sendevent(struct dtsgui *dtsgui, int eid) {
	DTSFrame *f =  dtsgui->GetFrame();
	f->SendDTSEvent(eid, NULL);
}

/** @ingroup C-API
  * @brief Specify callback to run to create and activate the toolbar.
  *
  * The toolbar has of yet not been wrapped into the C API
  * Its recomended that a C++ object be linked that contains a derived
  * class of wxToolbar.
  * @see DTSFrame::SetupToolbar
  * @warning This needs to be done in the application configuration callback.
  * @param dtsgui Application data ptr.
  * @param cb Toolbar create callback.
  * @param data Userdata to be passed to the toolbar*/
void dtsgui_setuptoolbar(struct dtsgui *dtsgui, dtsgui_toolbar_create cb, void *data) {
	DTSFrame *f = dtsgui->GetFrame();
	f->SetupToolbar(cb, data);
}

/** @ingroup C-API
  * @brief Disable or enable the toolbar.
  *
  * @see dtsgui::ShowToolbar()
  * @param dtsgui Application data ptr.
  * @param show Display the toolbar if non zero.*/
void dtsgui_set_toolbar(struct dtsgui *dtsgui, int show) {
	dtsgui->ShowToolbar(show);
}

/** @ingroup C-API
  * @brief Append text to the application frame title enclosed in square brackets.
  *
  * Update the main application title bar text.
  * @see dtsgui::AppendTitle()
  * @param dtsgui Application Data ptr.
  * @param text Text to append*/
void dtsgui_titleappend(struct dtsgui *dtsgui, const char *text) {
	dtsgui->AppendTitle(text);
}

/** @ingroup C-API-Menus
  * @brief Create a new menu and append it to the menu bar.
  *
  * A new menu is created and appended to the menu bar the
  * returned value will be passed to menu functions to append
  * items to.
  *
  * @see DTSFrame::NewMenu()
  *
  * @param dtsgui Application data ptr
  * @param name Name of the menu.
  * @return The fuction does not return.*/
dtsgui_menu dtsgui_newmenu(struct dtsgui *dtsgui, const char *name) {
	DTSFrame *frame = dtsgui->GetFrame();
	return frame->NewMenu(name);
}

/** @ingroup C-API-Menus
  * @brief Create a menu item that will activate the pane provided.
  *
  * this pane needs to be created and not deleted use of this function is discouraged.
  * the callback menu function is recomended where a panel can be dynamically created
  * and returned.
  *
  * @see DTSFrame::NewMenuItem()
  * @see dtsgui_newmenucb()
  * @see dtsgui_newmenu()
  * @see DTSFrame::NewMenu()
  *
  * @param dtsmenu Menu where to put append this item.
  * @param dtsgui Application data created on application startup and returned in most callbacks.
  * @param hint Menu hint with a & indicating the highlighted short key.
  * @param p Panel must be a derived window of DTSObject.
  * @return Created menu item.*/
dtsgui_menuitem dtsgui_newmenuitem(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, const char *hint, dtsgui_pane p) {
	DTSFrame *frame = dtsgui->GetFrame();
	return frame->NewMenuItem((wxMenu *)dtsmenu, (DTSObject*)p, menuid++, hint);
}

/** @ingroup C-API-Menus
  * @brief Create a menu item that will call a function with supplied data to execute when selected.
  *
  * The specified call back will be called passing the application pointer (dtsgui),
  * the name and data ptr supplied. if this function returns a DTSObject pane it
  * will be displayed.
  *
  *
  * @see DTSFrame::NewMenuItem()
  * @see dtsgui_newmenu()
  * @see dtsgui_dynpanel
  * @see dtsgui_newmenu()
  * @see DTSFrame::NewMenu()
  *
  * @param dtsmenu Menu where to put append this item.
  * @param dtsgui Application data created on application startup and returned in most callbacks.
  * @param hint Menu hint with a & indicating the highlighted short key.
  * @param name displayed on status bar and returned in callback.
  * @param blank if not zero will blank the display before executing callback.
  * @param cb Callback function to execute when selected.
  * @param data A ptr to a referenced object.
  * @return Created menu item.*/
extern dtsgui_menuitem dtsgui_newmenucb(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, const char *hint, const char *name, int blank, dtsgui_dynpanel cb,void *data) {
	DTSFrame *frame = dtsgui->GetFrame();
	return frame->NewMenuItem((wxMenu *)dtsmenu, menuid++, name, hint, blank, cb, data);
}

/** @ingroup C-API-Menus
  * @brief Add Menu item that will abort the app without save event been generated.
  *
  * A menu item will be appended to the supplied menu that when selected
  * will generate a abort event [Alt-F4] confirmation will be requested.
  *
  * @see DTSFrame::CloseMenu()
  * @see dtsgui_newmenu()
  * @see DTSFrame::NewMenu()
  *
  * @param dtsmenu Menu to append item to
  * @param dtsgui Application data ptr
  * @return The fuction does not return.*/
void dtsgui_exit(dtsgui_menu dtsmenu, struct dtsgui *dtsgui) {
	DTSFrame *frame = dtsgui->GetFrame();
	frame->CloseMenu((wxMenu *)dtsmenu, wxID_EXIT);
}

/** @ingroup C-API-Menus
  * @brief Add Menu item that will close the app after sending a save event.
  *
  * A menu item will be appended to the supplied menu that when selected
  * will generate a close/save event confirmation will be requested.
  *
  * @see DTSFrame::CloseMenu()
  * @see dtsgui_newmenu()
  * @see DTSFrame::NewMenu()
  *
  * @param dtsmenu Menu to append item to
  * @param dtsgui Application data ptr
  * @return The fuction does not return.*/
void dtsgui_close(dtsgui_menu dtsmenu, struct dtsgui *dtsgui) {
	DTSFrame *frame = dtsgui->GetFrame();
	frame->CloseMenu((wxMenu *)dtsmenu, wxID_SAVE);
}

/** @ingroup C-API-Menus
  * @brief Add Menu item will pop up a "about" box.
  *
  * Open an "about" box using the text supplied.
  *
  * @see dtsgui_newmenu()
  * @see DTSFrame::NewMenu()
  *
  * @param dtsmenu Menu to append about item to
  * @param dtsgui Application data ptr
  * @param text Text displayed in box when menu is selected
  * @return The fuction does not return.*/
void dtsgui_about(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, const char *text) {
	DTSFrame *frame = dtsgui->GetFrame();
	frame->SetAbout((wxMenu *)dtsmenu, text);
}

/** @ingroup C-API-Menus
  * @brief Add Menu seperator to the menu.
  *
  * A menu seperation line is appened to the menu.
  *
  * @see dtsgui_newmenu()
  * @see DTSFrame::NewMenu()
  *
  * @param dtsmenu Menu to append about item to
  * @return The fuction does not return.*/
void dtsgui_menusep(dtsgui_menu dtsmenu) {
	wxMenu *m = (wxMenu *)dtsmenu;
	m->AppendSeparator();
}

/** @ingroup C-API-Menus
  * @brief Enable/Disable an menu item.
  *
  * @param dmi Menuitem
  * @param enable Disable the menu if set to 0.*/
void dtsgui_menuitemenable(dtsgui_menuitem dmi, int enable) {
	wxMenuItem *mi = (wxMenuItem*)dmi;
	mi->Enable((enable) ? true : false);
}

/** @ingroup C-API-Panel
  * @brief Create a pannel to be displayed.
  *
  * This function is able to create and return various panels excluding Tab/Tree view panels.
  *
  * @see DTSFrame::CreatePane()
  * @see panel_type
  * @see panel_buttons
  *
  * @param dtsgui Application data ptr.
  * @param name Text used in the status bar when panel is displayed.
  * @param title Title set in the window Top/Middle.
  * @param butmask Buttons to be displayed values of panel_buttons or'd together.
  * @param type Type of panel to create [Excludes Tab/Tree Views]
  * @param userdata Referenced Obect passed stored in panel and available in event handlers.
  * @return Newly created panel.*/
dtsgui_pane dtsgui_panel(struct dtsgui *dtsgui, const char *name, const char *title, int butmask, enum panel_type type, void *userdata) {
	DTSFrame *frame = dtsgui->GetFrame();
	return frame->CreatePane(name, title, butmask, type, userdata);
}

/** @ingroup C-API-Panel
  * @brief Create a text display box with the text in the buffer been displayed.
  *
  * This creates a panel that only contains the supplied buffer
  * Scrollbars are used as required.
  *
  * This window is read only.
  *
  * @see DTSFrame::TextPanel()
  *
  * @param dtsgui Application data ptr.
  * @param title Text displayed on the status bar.
  * @param buf The text displayed in the pane.
  * @return New text panel.*/
dtsgui_pane dtsgui_textpane(struct dtsgui *dtsgui, const char *title, const char *buf) {
	DTSFrame *f = dtsgui->GetFrame();
	return f->TextPanel(title, buf);
}

/** @ingroup C-API-Panel
  * @brief Create a treepane view.
  *
  * Create a split window with a tree control on the left and display
  * area on right.
  *
  * @see DTSTreeWindow
  * @see dtsgui_tree_cb
  * @see tree_cbtype
  *
  * @param dtsgui Aplication data ptr.
  * @param title Text displayed on the status bar.
  * @param tree_cb Callback called when a event of tree_cbtype is handled.
  * @param userdata Referenced object made available in callback.
  * @param xmldoc Optional XML doc struct available in callback's and for managing XML nodes/panels.*/
extern dtsgui_treeview dtsgui_treewindow(struct dtsgui *dtsgui, const char *title, dtsgui_tree_cb tree_cb, void *userdata, struct xml_doc *xmldoc) {
	DTSFrame *frame = dtsgui->GetFrame();
	return new DTSTreeWindow(frame, frame, tree_cb, title, 25, NULL, xmldoc);
}

/** @ingroup C-API-Panel
  * @brief Create a tab view panel.
  *
  * Create a panel that will place pages as tabs along the top of the
  * display. Clicking on the tab opens the panel in the display area.
  *
  * @see DTSTabWindow
  *
  * @param dtsgui Application data ptr.
  * @param title Text displayed on status bar.
  * @param data Referenced object made available in the event callback.
  * @return Tab window.*/
extern dtsgui_tabview dtsgui_tabwindow(struct dtsgui *dtsgui, const char *title, void *data) {
	DTSFrame *frame = dtsgui->GetFrame();
	return  new DTSTabWindow(frame, title, data);
}

/** @ingroup C-API-Panel
  * @brief Pass a reference to a XML Doc Structure to the panel.
  *
  * Assign a reference to a XML Doc to the panel for use latter and with XML controls.
  *
  * @see DTSObject::SetXMLDoc()
  *
  * @param pane Panel to be assigned the ref.
  * @param xmldoc Document to be assigned.*/
extern void dtsgui_panel_setxml(dtsgui_pane pane, struct xml_doc *xmldoc) {
	DTSPanel *p = (DTSPanel *)pane;
	p->SetXMLDoc(xmldoc);
}

/** @ingroup C-API-Panel
  * @brief Get reference to previously allocated XML Doc.
  *
  * Obtain a new reference to the xmldoc stored in the panel.
  *
  * @note This must be un refferenced.
  *
  * @see DTSObject::GetXMLDoc()
  *
  * @param pane Panel to obtain the XML Info.
  * @return Reference to XML Doc.*/
struct xml_doc *dtsgui_panelxml(dtsgui_pane pane) {
	DTSPanel *p = (DTSPanel *)pane;
	return p->GetXMLDoc();
}

/** @ingroup C-API-Panel
  * @brief Delete a panel.
  *
  * @note This is unlikly too be needed as the GUI manages panels.
  *
  * @param pane Panel To Delete.*/
void dtsgui_delpane(dtsgui_pane pane) {
	delete (DTSPanel *)pane;
}

/** @ingroup C-API-Panel-Tab
  * @brief Create and append a tab to the view.
  *
  * Append a tab to the tab view the tab will be configured
  * when it is selected the callback will run to configure it.
  *
  * @note Its also possible to configure the returned panel.
  * @note On win32 all tabs are configured when the view is activated.
  *
  * @see dtsgui_tabpage_insert
  * @see dtsgui_tabwindow()
  * @see DTSTabWindow
  * @see DTSTabWindow::CreateTab()
  * @see panel_buttons
  * @see dtsgui_tabpanel_cb
  *
  * @param tv Tabview previously created.
  * @param name Title placed on the pane Top/Center.
  * @param butmask Mask of buttons to be placed on the panel and trigger events passed to the callback.
  * @param userdata Referenced object stored and made available in callbacks.
  * @param xmldoc Optional XML document stored and made available to XML resources.
  * @param cb Callback called to configure the panel.
  * @param cdata Referenced object passed to the configuration callback.*/
extern dtsgui_pane dtsgui_newtabpage(dtsgui_tabview tv, const char *name, int butmask, void *userdata, struct xml_doc *xmldoc, dtsgui_tabpanel_cb cb, void *cdata) {
	DTSTabWindow *tw = (DTSTabWindow*)tv;
	return tw->CreateTab(name, butmask, userdata, cb, cdata, xmldoc);
}


/** @ingroup C-API-Panel-Tab
  * @brief Create and insert a tab in the view.
  *
  * Insert a tab at the position specified and recreate the panel at
  * position undo.
  *
  * @see dtsgui_tabpage_newtabpage
  * @see dtsgui_tabwindow()
  * @see DTSTabWindow
  * @see DTSTabWindow::CreateTab()
  * @see panel_buttons
  * @see dtsgui_tabpanel_cb
  *
  * @param tv Tabview previously created.
  * @param name Title placed on the pane Top/Center.
  * @param butmask Mask of buttons to be placed on the panel and trigger events passed to the callback.
  * @param userdata Referenced object stored and made available in callbacks.
  * @param xmldoc Optional XML document stored and made available to XML resources.
  * @param cb Callback called to configure the panel.
  * @param cdata Referenced object passed to the configuration callback.
  * @param pos Position to insert into
  * @param undo Delete and recreate the panel at this position
  * @return New Tab Pane*/
extern dtsgui_pane dtsgui_tabpage_insert(dtsgui_tabview tv, const char *name, int butmask, void *userdata, struct xml_doc *xmldoc, dtsgui_tabpanel_cb cb, void *cdata, int pos, int undo) {
	DTSTabWindow *tw = (DTSTabWindow*)tv;
	return tw->CreateTab(name, butmask, userdata, cb, cdata, xmldoc, pos, undo);
}

/** @ingroup C-API-Panel-Tab
  * @brief Helper function to handle "Adding" a new tab from a existing tab.
  *
  * This implements the concept of the New button a new panel will be created
  * from the data filled in on the pane this callback is attached too.
  * A new XML node is created and the panel created from this node.
  *
  * @note this callback helper is rather complex.
  *
  * @see tab_newpane
  *
  * @param tabv Table view the new panel will be added too.
  * @param p The panel this callback is attached too.
  * @param xpath the path of the new node.
  * @param node the name of the XML node to create in the xpath.
  * @param vitem the item in the panel to be used to create the value of the new node.
  * @param tattr the attribute to be used too create the title in the new pane.
  * @param data_cb Callback to allow setting the data used in pane creation.
  * @param cb Callback to configure the pane.
  * @param cdata Referrenced object containing initial callback data.
  * @param xmldoc XML doc reference for creating the node / panel.
  * @param data Referenced object made available in callbacks.
  * @return Nothing is returned the callback is registered on the panel.*/
void dtsgui_newxmltabpane(dtsgui_tabview tabv, dtsgui_pane p, const char *xpath, const char *node, const char *vitem, const char *tattr,  dtsgui_tabpane_newdata_cb data_cb,
			  dtsgui_tabpanel_cb cb, void *cdata, struct xml_doc *xmldoc, void *data) {
	class tab_newpane *tn = new tab_newpane((DTSTabWindow*)tabv, xpath, node, vitem, tattr, data_cb, cb, cdata, xmldoc, data);
	static_cast<DTSPanel*>(p)->SetEventCallback(&tab_newpane::handle_newtabpane_cb, tn, true);
}

/** @ingroup C-API-Panel-Tree
  * @brief Create a panel owned by the tree.
  *
  * This will seldom be needed as panels are created and supplied in callbacks.
  *
  * @see DTSTreeWindow::CreatePane()
  * @see panel_buttons
  * @note Panels used in tree views must be created as children of the tree.
  * @param tv Tree view.
  * @param name Title placed on the panel top/center
  * @param butmask A mask of buttons to be placed on the panel.
  * @param userdata Referenced object available in event callbacks.
  * @param xmldoc XML doc reference used for updating XML elements.
  * @return New Tree Pane.*/
extern dtsgui_pane dtsgui_treepane(dtsgui_treeview tv, const char *name, int butmask, void *userdata, struct xml_doc *xmldoc) {
	DTSTreeWindow *tw = (DTSTreeWindow*)tv;
	return tw->CreatePane(name, butmask, userdata, xmldoc);
}

/** @ingroup C-API-Panel-Tree
  * @brief Create a panel owned by the tree configured from the specified node.
  *
  * Create a pane from information obtained from node.
  * Name of node is used for the panel title and a Accept/Undo button.
  *
  * @note if node id is -1 no name/buttons are added the page is blank.
  *
  * @see DTSTreeWindow::CreatePane()
  *
  * @param tv Tree view.
  * @param node used as template for panel.
  * @return New Tree Pane.*/
dtsgui_pane dtsgui_treepane_default(dtsgui_treeview tv, dtsgui_treenode node) {
	DTSTreeWindow *tw = (DTSTreeWindow*)tv;
	return tw->CreatePane(wxDataViewItem(node));
}

/** @ingroup C-API-Panel-Tree
  * @brief Create a container in the tree view.
  *
  * Create and insert a new folder/container in the tree view with properties / call back.
  * A panel will be created and supplied to the callback too implement on selection.
  *
  * @note The root node is NULL this is the initial container.
  *
  * @see DTSTreeWindow::AppendContainer
  * @see dtsgui_treeviewpanel_cb
  *
  * @param tree The tree view to add the node too.
  * @param node The parent node or NULL to create root node.
  * @param title The label shown on the tree.
  * @param can_edit Allow editing of this node the XML node will be updated.
  * @param can_sort Allow sorting of this node the XML node will be updated.
   * @param can_del Not yet implemented for containers but will allow right click menu to delete.
  * @param nodeid A value passed to the callback as convinence can be any value -1 will cause a blank panel.
  * @param p_cb Callback to pass panel for configuration on selection of the item.
  * @param data Referenced object passed too callbacks.*/
dtsgui_treenode dtsgui_treecont(dtsgui_treeview tree, dtsgui_treenode node, const char *title, int can_edit, int can_sort, int can_del,
				int nodeid, dtsgui_treeviewpanel_cb p_cb, void *data) {
	DTSTreeWindow *tw = (DTSTreeWindow*)tree;
	return tw->GetTreeCtrl()->AppendContainer(wxDataViewItem(node), title, can_edit, can_sort, can_del, nodeid, p_cb, data).GetID();
}

/** @ingroup C-API-Panel-Tree
  * @brief Create a item in a container in the tree view.
  *
  * Create and insert a new item in the tree view in the node specified with properties / call back.
  * A panel will be created and supplied to the callback too implement on selection.
  *
  * @see DTSTreeWindow::AppendItem
  * @see dtsgui_treeviewpanel_cb
  *
  * @param tree The tree view to add the node too.
  * @param node The parent node or NULL to create root node.
  * @param title The label shown on the tree.
  * @param can_edit Allow editing of this node the XML node will be updated.
  * @param can_sort Allow sorting of this node the XML node will be updated.
  * @param can_del Allow right click menu to delete.
  * @param nodeid A value passed to the callback as convinence can be any value -1 will cause a blank panel.
  * @param p_cb Callback to pass panel for configuration on selection of the item.
  * @param data Referenced object passed too callbacks.*/
dtsgui_treenode dtsgui_treeitem(dtsgui_treeview tree, dtsgui_treenode node, const char *title, int can_edit, int can_sort, int can_del, int nodeid,
				dtsgui_treeviewpanel_cb p_cb, void *data) {
	DTSTreeWindow *tw = (DTSTreeWindow*)tree;
	return tw->GetTreeCtrl()->AppendItem(wxDataViewItem(node), title, can_edit, can_sort, can_del, nodeid, p_cb, data).GetID();
}

/** @ingroup C-API-Panel-Tree
  * @brief Helper function to handle "Adding" a new node from a existing node.
  *
  * This implements the concept of the New button a new node will be created
  * from the data filled in on the pane this callback is attached too.
  * A new XML node is created and the panel created from this node.
  *
  * @see tree_newnode_flags
  *
  * @note this callback helper is rather complex.
  *
  * @param tree Tree view the new panel will be added too.
  * @param p The panel this callback is attached too.
  * @param tn Root treenode to add too.
  * @param xpath the path of the new node.
  * @param node the name of the XML node to create in the xpath.
  * @param vitem the item in the panel to be used to create the value of the new node.
  * @param tattr the attribute to be used too create the title in the new pane.
  * @param nid Node id for newly  created node.
  * @param flags Option flags combined from tree_newnode_flags.
  * @param node_cb Callback called to configure the node.
  * @param data Referenced Object passed to the callbacks.
  * @param p_cb Callback called to configure the panel when it is selected.
  * @return Nothing is returned the callback is registered on the panel.*/
void dtsgui_newxmltreenode(dtsgui_treeview tree, dtsgui_pane p, dtsgui_treenode tn, const char *xpath, const char *node, const char *vitem, const char *tattr,
							int nid, int flags, dtsgui_xmltreenode_cb node_cb, void *data, dtsgui_treeviewpanel_cb p_cb) {
	class tree_newnode *nn = new tree_newnode(tree, tn, xpath, node, vitem, tattr, nid, flags, node_cb, data, p_cb);
	static_cast<DTSPanel*>(p)->SetEventCallback(&tree_newnode::handle_newtreenode_cb, nn, true);
}

/** @ingroup C-API-Panel-Tree
  * @brief Get the node id of a tree node.
  *
  * @see DTSDVMListStore::GetNodeID()
  *
  * @param tn Tree Node to get the node of.
  * @return the user defined tree node ID*/
int dtsgui_treenodeid(dtsgui_treenode tn) {
	DTSDVMListStore *ls = (DTSDVMListStore*)tn;
	return ls->GetNodeID();
}

/** @ingroup C-API-Panel-Tree
  * @brief Get the name of the nodes parent
  * @see DTSDVMListStore::GetParentTitle()
  * @param tn Tree node
  * @return Name og the nodes parentI*/
const char *dtsgui_treenodeparent(dtsgui_treenode tn) {
	DTSDVMListStore *entry = (DTSDVMListStore*)tn;
	return strdup(entry->GetParentTitle().ToUTF8());
}

/** @ingroup C-API-Panel-Tree
  * @brief Add a XML node to the tree node
  * @see DTSDVMListStore::SetXMLData
  * @param tn Tree node to set.
  * @param xn XML Node to reference.
  * @param tattr Attribute in the XN that represents the title NULL if the value is the title.*/
void dtsgui_treenodesetxml(dtsgui_treenode tn,struct xml_node *xn, const char *tattr) {
	DTSDVMListStore *ls = (DTSDVMListStore*)tn;
	return ls->SetXMLData(xn, tattr);
}

/** @ingroup C-API-Panel-Tree
  * @brief Return the XML node reference held by the tree node.
  *
  * Returns the XML node of the node and optionally the title attribute.
  *
  * @note The reference to the buffer and node needs to be droped.
  * @param tn Tree node to obtain XML node from.
  * @param buf a pointer to a buffer that will be set to the title attribute (Optional).
  * @return Reference to the XML node stored in the tree node.*/
struct xml_node *dtsgui_treenodegetxml(dtsgui_treenode tn, char **buf) {
	DTSDVMListStore *ls = (DTSDVMListStore*)tn;
	return ls->GetXMLData(buf);
}

/** @ingroup C-API-Panel-Tree
  * @brief Update the XML Node or title attribute with a new name.
  *
  * This helper function will update the node's XML after a edit event.
  *
  * @see DTSTreeWindow::UpdateNodeXML()
  *
  * @param tree Treeview node is part of.
  * @param node TreeNode to update.
  * @param newname Name to update XML node with.*/
void dtsgui_nodesetxml(dtsgui_treeview tree, dtsgui_treenode node, const char *newname) {
	DTSTreeWindow *tw = (DTSTreeWindow*)tree;
	tw->UpdateNodeXML(wxDataViewItem(node), newname);
}

/** @ingroup C-API-Panel-Tree
  * @brief Get reference for the user data of node
  * @see DTSDVMListStore::GetUserData()
  * @note This needs to be unreferenced.
  * @param tn Tree node.
  * @return Reference to user data*/
void *dtsgui_treenodegetdata(dtsgui_treenode tn) {
	DTSDVMListStore *ls = (DTSDVMListStore*)tn;
	return ls->GetUserData();
}

/** @ingroup C-API-Panel
  * @brief Set event callback function for panel.
  *
  * @see DTSPanel::SetEventCallback()
  * @see event_callback
  *
  * When a event happens the callback is called with the userdata
  * to allow actioning the event.
  *
  * @param pane Panel to attach callback too.
  * @param evcb Event callback.
  * @param data Data supplied in the callback (panel userdata is available in the panel).*/
void dtsgui_setevcallback(dtsgui_pane pane,event_callback evcb, void *data) {
	DTSPanel *p = (DTSPanel *)pane;
	return p->SetEventCallback(evcb, data);
}

/** @ingroup C-API-Panel
  * @brief update all XML items in the panel from current element values.
  *
  * All elements on the panel that are XML will have there nodes updated
  * Based on the value of the elements.
  *
  * @see DTSPanel::Update_XML()
  * @see dtsgui_pane2post
  * @see DTSPanel::Panel2Post()
  *
  * @param pane Panel to update.*/
extern void dtsgui_xmlpanel_update(dtsgui_pane pane) {
	DTSPanel *p = (DTSPanel*)pane;
	p->Update_XML();
}

/** @ingroup C-API-Panel
  * @brief Prepare a HTTP POST structure from panel.
  *
  * Create a HTTP Post structure from the values and names
  * Of elements on a panel this can then be used with curl_post
  * to post to a URL.
  *
  * @see DTSPanel::Update_XML()
  * @see dtsgui_xmlpane_update()
  * @see DTSPanel::Panel2Post()
  *
  * @param p Panel to create from.
  * @return A CURL HTTP POST Structure.*/
struct curl_post *dtsgui_pane2post(dtsgui_pane p) {
	DTSPanel *dp = (DTSPanel*)p;
	return dp->Panel2Post();
}

/** @ingroup C-API-Panel
  * @brief Obtain a reference to the userdata of panel.
  *
  * @see DTSObject::GetUserData().
  *
  * @note this must be un refferenced.
  *
  * @param pane Panel to obtain reference from.
  * @return Reference to userdata.*/
extern void *dtsgui_paneldata(dtsgui_pane pane) {
	DTSPanel *p = (DTSPanel*)pane;
	return p->GetUserData();
}

/** @ingroup C-API-Panel
  * @brief Update the title on the panel if it exists.
  * @see DTSPanel::SetTitle()
  * @param pane Panel to update title.
  * @param title New title to place on panel.*/
void dtsgui_settitle(dtsgui_pane pane, const char *title) {
	DTSPanel *p = (DTSPanel*)pane;
	return p->SetTitle(title);
}


/** @ingroup C-API-Panel
  * @brief Update the status bar message of panel.
  * @see DTSPanel::SetStatus()
  * @param pane Panel to update title.
  * @param status New status bar message.*/
void dtsgui_setstatus(dtsgui_pane pane, const char *status) {
	DTSPanel *p = (DTSPanel*)pane;
	return p->SetStatus(status);
}

/** @ingroup C-API-Panel
  * @brief Set config callback function for panel (UNUSED).
  *
  * @see DTSPanel::SetConfigCallback()
  * @see dtsgui_configcb
  *
  * @note Not yet implemted who knows maybe it will maybe it wont.
  *
  * @param pane Panel to attach callback too.
  * @param cb Config callback.
  * @param data Data supplied in the callback (panel userdata is available in the panel).*/
void dtsgui_configcallback(dtsgui_pane pane,dtsgui_configcb cb, void *data) {
	DTSPanel *p = (DTSPanel *)pane;
	p->SetConfigCallback(cb, data);
}

/** @ingroup C-API-Panel
  * @brief Create a XML node from the elements on the panel.
  *
  * Using the path information a node is created and elements
  * added as attributes.
  *
  * @see DTSPanel::Panel2XML()
  *
  * @param p Panel to create nodes from.
  * @param xpath Base path to create the nodes in.
  * @param node Name of the node to add.
  * @param nodeval Name of the element to use as the value of node.
  * @param attrkey Use this item to set the initial attribute and use as key.
  * @return New XML Node.*/
struct xml_node *dtsgui_panetoxml(dtsgui_pane p, const char *xpath, const char *node, const char *nodeval, const char *attrkey) {
	DTSPanel *dp = (DTSPanel*)p;
	return dp->Panel2XML(xpath, node, nodeval, attrkey);
}

/** @ingroup C-API-Panel
  * @brief Execute a dialog panel.
  * Pop up and run a dialog pane the result will be obtained in the event handler.
  * @see DTSDialog::RunDialog()
  * @param pane Dialog panel to execute.
  * @param evcb Event callback to handle the results in.
  * @param data Data passed to the event handler.*/
void dtsgui_rundialog(dtsgui_pane pane, event_callback evcb, void *data) {
	DTSDialog *p = (DTSDialog *)pane;
	p->RunDialog(evcb, data);
}

/** @ingroup C-API-Wizard
  * @brief Create a new wizard.
  *
  * A wizard consists of multiple panels linked with back/next controls each panel
  * will need to be procesed on completion.
  *
  * @see dtsgui_wizard
  *
  * @param dtsgui Aplication data ptr.
  * @param title Title shown on title bar of wizard pop up.
  * @return New wizard that needs pages added too and run.*/
extern struct dtsgui_wizard *dtsgui_newwizard(struct dtsgui *dtsgui, const char *title) {
	return new dtsgui_wizard(dtsgui, dtsgui->GetFrame(), title);
}

/** @ingroup C-API-Wizard
  * @brief Create a panel and append it to the wizard.
  *
  * @see dtsgui_wizard::AddPage()
  *
  * @param dtswiz Wizard to add page too.
  * @param title Title of the new panel.
  * @param userdata Userdata to be referenced and stored on the panel.
  * @param xmldoc Optional XML Doc to be referenced and stored on the panel.
  * @return New panel that will need to be configured and processed after completion.*/
extern dtsgui_pane dtsgui_wizard_addpage(struct dtsgui_wizard *dtswiz, const char *title, void *userdata, struct xml_doc *xmldoc) {
	return dtswiz->AddPage(title, xmldoc, userdata);
}

/** @ingroup C-API-Wizard
  * @brief Run the wizard
  *
  * Pop up the wizard window and open first page on success its
  * required that the panels information be processed.
  * When the wizard is un refferenced the panels are deleted.
  *
  * @see dtsgui_wizard::RunWizard()
  *
  * @param dtswiz Wizard too run.
  * @return non Zero on success.*/
extern int dtsgui_runwizard(struct dtsgui_wizard *dtswiz) {
	return dtswiz->RunWizard();
}

/** @ingroup C-API-Progress
  * @brief Initialise the progress dialog
  * Pop up a dialog box that can be updated to showprogress with a optional cancel button.
  * @see DTSFrame::StartProgress()
  * @param dtsgui Application data ptr.
  * @param text Text displayed in the popup box.
  * @param maxval the maximum expected value to update.
  * @param quit Disable the cancel button if quit is zero.
  * @return zero on failure*/
int dtsgui_progress_start(struct dtsgui *dtsgui, const char *text, int maxval, int quit) {
	DTSFrame *f = dtsgui->GetFrame();
	return f->StartProgress(text, maxval, quit);
}


/** @ingroup C-API-Progress
  * @brief Update the progress bar counter
  * This function updates the current progress value and optionally allows setting
  * new displayed text.
  * @see DTSFrame::UpdateProgress()
  * @param dtsgui Application data ptr.
  * @param newval Update the counter with this value.
  * @param newtext Text displayed in the popup box.
  * @return 0 on success may fail when the user presses cancel.*/
int dtsgui_progress_update(struct dtsgui *dtsgui, int newval, const char* newtext) {
	DTSFrame *f = dtsgui->GetFrame();
	return f->UpdateProgress(newval, newtext);
}

/** @ingroup C-API-Progress
  * @brief Increments the counter by adding to it not setting it.
  * @see DTSFrame::IncProgress()
  * @param dtsgui Application data ptr.
  * @param ival Value to increment progress bar.
  * @param newtext Text displayed in the popup box.
  * @return 0 on success may fail when the user presses cancel.*/
int dtsgui_progress_increment(struct dtsgui *dtsgui, int ival, const char* newtext) {
	DTSFrame *f = dtsgui->GetFrame();
	return f->IncProgress(ival, newtext);
}

/** @ingroup C-API-Progress
  * @brief Disable the progress bar.
  * @see DTSFrame::EndProgress()
  * @param dtsgui Application data ptr.*/
void dtsgui_progress_end(struct dtsgui *dtsgui) {
	DTSFrame *f = dtsgui->GetFrame();
	f->EndProgress();
}

/** @ingroup C-API-Panel-Items
  * @brief Return reference to hashed bucket list of items.
  *
  * The bucket list API in dtsapplib contains more information.
  *
  * @see DTSObject::GetItems()
  * @param pane Panel to obtain list from.
  * @return Reference to hashed bucket list.*/
struct bucket_list *dtsgui_panel_items(dtsgui_pane pane) {
	DTSObject *p = (DTSObject*)pane;
	return p->GetItems();
}

/** @ingroup C-API-Panel-Items
  * @brief Return refernece to a form element.
  * Find a item and return it searcing the bucket list for the
  * specified name.
  * @see DTSPanel::FindItem()
  * @param p panel who's item list is to be searched.
  * @param  name Return item with this name.
  * @return Form item if found matching the name.*/
extern struct form_item *dtsgui_finditem(dtsgui_pane p, const char *name) {
	DTSPanel *dp = (DTSPanel*)p;
	return dp->FindItem(name);
}

/** @ingroup C-API-Panel-Items
  * @brief Get refernece to data stoed on element.
  *
  * Each element is capable of storing user supplied data
  * this returns a reference to this data.
  * @see form_item::GetData()
  * @param fi Form item to obtain data from.
  * @return Reference to data stored in item.*/
extern void *dtsgui_item_data(struct form_item *fi) {
	return (fi) ? fi->GetData() : NULL;
}

/** @ingroup C-API-Panel-Items
  * @brief Return value of element.
  * The value of the panel element is returned fo the item.
  * @note The return value must be freed.
  * @see form_item::GetValue()
  * @param fi Form item to obtain value of.
  * @return Value of the element.*/
extern 	const char *dtsgui_item_value(struct form_item *fi) {
	return (fi) ? fi->GetValue() : NULL;
}

/** @ingroup C-API-Panel-Items
  * @brief Return name of element.
  * @note the name must not be freed and the form itme must not
  * be un referenced
  * @see form_item::GetName()
  * @param fi Form item.
  * @return Name of item.*/
extern const char *dtsgui_item_name(struct form_item *fi) {
	return fi->GetName();
}

/** @ingroup C-API-Panel-Items
  * @brief Helper function to return a value of a item.
  * This function finds a value by name returning the value
  * @note the returned value must be freeed.
  * @see dtsgui_finditem()
  * @see dtsgui_item_value()
  * @see DTSPanel::FindValue()
  * @see DTSPanel::FindItem()
  * @see form_item::GetValue()
  * @param p Panel to search element.
  * @param name Return value of element with this name.
  * @return Value of element.*/
extern const char *dtsgui_findvalue(dtsgui_pane p, const char *name) {
	DTSPanel *dp = (DTSPanel*)p;
	return dp->FindValue(name);
}


/** @ingroup C-API-Dialog
  * @brief Alert the user and expect a confirmation.
  * @see DTSFrame::Alert()
  * @param dtsgui Application data ptr.
  * @param text Text to supply in the alert.*/
void dtsgui_alert(struct dtsgui *dtsgui, const char *text) {
	DTSFrame *f = dtsgui->GetFrame();
	f->Alert(text);
}

/** @ingroup C-API-Dialog
  * @brief Request Yes/No Confirmation from the user.
  * @see DTSFrame::Confirm()
  * @param dtsgui Application data ptr.
  * @param text Text to supply in the alert.
  * @return Non zero if the user affirmed.*/
int dtsgui_confirm(struct dtsgui *dtsgui, const char *text) {
	DTSFrame *f = dtsgui->GetFrame();
	return f->Confirm(text);
}

/** @ingroup C-API-Dialog
  * @brief Open file save dialog box.
  * @see DTSFrame::FileDialog()
  * @param dtsgui Application data ptr.
  * @param title Title of the file dialog.
  * @param path Original path to open dialog in.
  * @param name Suggested name to save as.
  * @param filter File namr filter.
  * @return Reference to the filename that must be unreferenced.*/
extern const char *dtsgui_filesave(struct dtsgui *dtsgui, const char *title, const char *path, const char *name, const char *filter) {
	DTSFrame *f = dtsgui->GetFrame();
	return f->FileDialog(title, path, name, filter, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
}

/** @ingroup C-API-Dialog
  * @brief Open file open dialog box.
  * 
  * @see DTSFrame::FileDialog()
  * @see dtsgui_fileopen() 
  * @param dtsgui Application data ptr.
  * @param title Title of the file dialog.
  * @param path Original path to open dialog in.
  * @param name Suggested name to open.
  * @param filter File namr filter.
  * @return Reference to the filename that must be unreferenced.*/
extern const char *dtsgui_fileopen(struct dtsgui *dtsgui, const char *title, const char *path, const char *name, const char *filter) {
	DTSFrame *f = dtsgui->GetFrame();
	return f->FileDialog(title, path, name, filter, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
}

/** @ingroup C-API-Dialog
  * @brief Return basic auth reference from a password dialog box.
  *
  * A dialog box requesting the user name and password is presented to the user.
  * The initial values may be supplied.
  * @remark This function is used as a callback for CURL.
  * @see DTSFrame::Passwd()
  * @param user initial username entered into the box.
  * @param passwd the initial password.
  * @param data this will always be the application data ptr.
  * @return Basic auth reference.*/
struct basic_auth *dtsgui_pwdialog(const char *user, const char *passwd, void *data) {
	DTSFrame *f = static_cast<class dtsgui*>(data)->GetFrame();
	return f->Passwd(user, passwd);
}

/** @ingroup C-API-Panel-Elements
  * @brief Add a name / value to the list box for selection.
  * @see form_item::Append()
  * @param listbox A form item containing a listbox or combobox.
  * @param text The text appearing on the drop down box.
  * @param value The value set when this item is selected.*/
void dtsgui_listbox_add(struct form_item *listbox, const char *text, const char *value) {
	listbox->Append(text, value);
}

/** @ingroup C-API-Panel-Elements
  * @brief Set the selected item on a list box or combobox to the index.
  * @param listbox A form item containing a listbox or combobox.
  * @param idx the index to set as the selected value.*/
void dtsgui_listbox_set(struct form_item *listbox, int idx) {
	listbox->SetSelection(idx);
}

/** @ingroup C-API-Panel-Elements
  * @brief Populate a listbox or combobox from a XML path.
  *
  * A entry is added for each node found using the supplied attributes for displayed text and value
  * @remark passing NULL as either or both of the attributes will use the value of the node.
  * @param lb A form item containing a listbox or combobox.
  * @param xmldoc XML Document to search.
  * @param xpath XML search path.
  * @param nattr Attribute whoose value will be used for name on list.
  * @param vattr Attribute whoose value will be used for value.*/
void dtsgui_listbox_addxml(struct form_item *lb, struct xml_doc *xmldoc, const char *xpath, const char *nattr, const char *vattr) {
	lb->AppendXML(xmldoc, xpath, nattr, vattr);
}

/** @ingroup C-API-Panel-Elements-STD
  * @brief Place a text element on the panel.
  * @see DTSPanel::TextBox()
  * @param pane Panel to place element on.
  * @param title Title of element.
  * @param name Name of element.
  * @param value Initial value of element.
  * @param data User data to hold a reference of.*/
extern void dtsgui_textbox(dtsgui_pane pane, const char *title, const char *name, const char *value, void *data) {
	DTSPanel *p = (DTSPanel *)pane;
	p->TextBox(title, name, value, wxTE_LEFT | wxTE_PROCESS_ENTER, 1, data, DTSGUI_FORM_DATA_PTR);
}

/** @ingroup C-API-Panel-Elements-STD
  * @brief Place a multi line text element on the panel.
  * @see DTSPanel::TextBox()
  * @param pane Panel to place element on.
  * @param title Title of element.
  * @param name Name of element.
  * @param value Initial value of element.
  * @param data User data to hold a reference of.*/
extern void dtsgui_textbox_multi(dtsgui_pane pane, const char *title, const char *name, const char *value, void *data) {
	DTSPanel *p = (DTSPanel *)pane;
	p->TextBox(title, name, value, wxTE_MULTILINE, 5, data,  DTSGUI_FORM_DATA_PTR);
}

/** @ingroup C-API-Panel-Elements-STD
  * @brief Place a password element on the panel.
  * @see DTSPanel::TextBox()
  * @param pane Panel to place element on.
  * @param title Title of element.
  * @param name Name of element.
  * @param value Initial value of element.
  * @param data User data to hold a reference of.*/
extern void dtsgui_passwdbox(dtsgui_pane pane, const char *title, const char *name, const char *value, void *data) {
	DTSPanel *p = (DTSPanel *)pane;
	p->TextBox(title, name, value, wxTE_PASSWORD | wxTE_PROCESS_ENTER, 1, data,  DTSGUI_FORM_DATA_PTR);
}

/** @ingroup C-API-Panel-Elements-STD
  * @brief Place a checkbox element on the panel.
  * @see DTSPanel::CheckBox()
  * @param pane Panel to place element on.
  * @param title Title of element.
  * @param name Name of element.
  * @param checkval Value of item when checked.
  * @param uncheck Value of item when unchecked.
  * @param ischecked Set to non zero if item is initially checked.
  * @param data User data to hold a reference of.*/
extern void dtsgui_checkbox(dtsgui_pane pane, const char *title, const char *name, const char *checkval, const char *uncheck, int ischecked, void *data) {
	DTSPanel *p = (DTSPanel *)pane;
	p->CheckBox(title, name, ischecked, checkval, uncheck, data,  DTSGUI_FORM_DATA_PTR);
}

/** @ingroup C-API-Panel-Elements-STD
  * @brief Place a listbox element on the panel.
  * @see DTSPanel::ListBox()
  * @param pane Panel to place element on.
  * @param title Title of element.
  * @param name Name of element.
  * @param data User data to hold a reference of.
  * @return Form item that can be used to add values this must be unreferenced*/
extern struct form_item *dtsgui_listbox(dtsgui_pane pane, const char *title, const char *name, void *data) {
	DTSPanel *p = (DTSPanel *)pane;
	return p->ListBox(title, name, NULL, data,  DTSGUI_FORM_DATA_PTR);
}

/** @ingroup C-API-Panel-Elements-STD
  * @brief Place a combobox element on the panel.
  * @see DTSPanel::ComboBox()
  * @param pane Panel to place element on.
  * @param title Title of element.
  * @param name Name of element.
  * @param data User data to hold a reference of.
  * @return Form item that can be used to add values this must be unreferenced*/
extern struct form_item *dtsgui_combobox(dtsgui_pane pane, const char *title, const char *name, void *data) {
	DTSPanel *p = (DTSPanel *)pane;
	return p->ComboBox(title, name, NULL, data, DTSGUI_FORM_DATA_PTR);
}

/** @ingroup C-API-Panel-Elements-XML
  * @brief Place a XML text element on the panel.
  * @see DTSPanel::XMLTextBox()
  * @param pane Panel to place element on.
  * @param title Title of element.
  * @param name Name of element.
  * @param xpath Xpath of the element.
  * @param node Node to create in path if it does not exist.
  * @param fattr Attribute to create a filter on.
  * @param fval Value to equate to filter attr in filter.
  * @param attr Attribute in the node to assign value of.*/
extern void dtsgui_xmltextbox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	p->XMLTextBox(title, name, xpath, node, fattr, fval, attr, wxTE_LEFT | wxTE_PROCESS_ENTER, 1);
}

/** @ingroup C-API-Panel-Elements-XML
  * @brief Place a XML multi line text element on the panel.
  * @see DTSPanel::XMLTextBox()
  * @param pane Panel to place element on.
  * @param title Title of element.
  * @param name Name of element.
  * @param xpath Xpath of the element.
  * @param node Node to create in path if it does not exist.
  * @param fattr Attribute to create a filter on.
  * @param fval Value to equate to filter attr in filter.
  * @param attr Attribute in the node to assign value of.*/
extern void dtsgui_xmltextbox_multi(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	p->XMLTextBox(title, name, xpath, node, fattr, fval, attr, wxTE_MULTILINE, 5);
}

/** @ingroup C-API-Panel-Elements-XML
  * @brief Place a XML password element on the panel.
  * @see DTSPanel::XMLPasswdBox()
  * @param pane Panel to place element on.
  * @param title Title of element.
  * @param name Name of element.
  * @param xpath Xpath of the element.
  * @param node Node to create in path if it does not exist.
  * @param fattr Attribute to create a filter on.
  * @param fval Value to equate to filter attr in filter.
  * @param attr Attribute in the node to assign value of.*/
extern void dtsgui_xmlpasswdbox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	p->XMLPasswdBox(title, name, xpath, node, fattr, fval, attr, wxTE_LEFT | wxTE_PROCESS_ENTER);
}

/** @ingroup C-API-Panel-Elements-XML
  * @brief Place a XML checkbox element on the panel.
  * @see DTSPanel::XMLCheckBox()
  * @param pane Panel to place element on.
  * @param title Title of element.
  * @param name Name of element.
  * @param checkval The value will be set to this if checked.
  * @param uncheckval The value will be set to this if unchecked.
  * @param xpath Xpath of the element.
  * @param node Node to create in path if it does not exist.
  * @param fattr Attribute to create a filter on.
  * @param fval Value to equate to filter attr in filter.
  * @param attr Attribute in the node to assign value of.*/
extern void dtsgui_xmlcheckbox(dtsgui_pane pane, const char *title, const char *name, const char *checkval, const char *uncheckval, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	p->XMLCheckBox(title, name, checkval, uncheckval, xpath, node, fattr, fval, attr);
}

/** @ingroup C-API-Panel-Elements-XML
  * @brief Place a XML listbox element on the panel.
  * @see DTSPanel::XMLListBox()
  * @param pane Panel to place element on.
  * @param title Title of element.
  * @param name Name of element.
  * @param xpath Xpath of the element.
  * @param node Node to create in path if it does not exist.
  * @param fattr Attribute to create a filter on.
  * @param fval Value to equate to filter attr in filter.
  * @param attr Attribute in the node to assign value of.
  * @return Form item that can be used to add values this must be unreferenced*/
struct form_item *dtsgui_xmllistbox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	return p->XMLListBox(title, name, xpath, node, fattr, fval, attr);
}

/** @ingroup C-API-Panel-Elements-XML
  * @brief Place a XML combobox element on the panel.
  * @see DTSPanel::XMLComboBox()
  * @param pane Panel to place element on.
  * @param title Title of element.
  * @param name Name of element.
  * @param xpath Xpath of the element.
  * @param node Node to create in path if it does not exist.
  * @param fattr Attribute to create a filter on.
  * @param fval Value to equate to filter attr in filter.
  * @param attr Attribute in the node to assign value of.
  * @return Form item that can be used to add values this must be unreferenced*/
struct form_item *dtsgui_xmlcombobox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	return p->XMLComboBox(title, name, xpath, node, fattr, fval, attr);
}

} /*END Namespace*/
