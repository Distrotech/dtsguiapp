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

#include <vector>
#include <stdint.h>

#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <wx/wizard.h>

#include <wx/dataview.h>
#include <wx/splitter.h>
#include <wx/notebook.h>

#include <wx/menu.h>

#include <dtsapp.h>
#include "dtsgui.h"
#include "dtsgui.hpp"

#include "pitems.h"
#include "DTSFrame.h"
#include "DTSPanel.h"
#include "DTSListView.h"
#include "DTSTreeWindow.h"

enum treewinmenu {
	DTS_TREEWIN_MENU_SORT = 127,
	DTS_TREEWIN_MENU_MOVEUP,
	DTS_TREEWIN_MENU_MOVEDOWN,
	DTS_TREEWIN_MENU_DELETE
};

tree_newnode::~tree_newnode() {
	if (xpath) {
		free((void*)xpath);
	}
	if (node) {
		free((void*)node);
	}
	if (vitem) {
		free((void*)vitem);
	}
	if (tattr) {
		free((void*)tattr);
	}
	if (data) {
		objunref(data);
	}
}

tree_newnode::tree_newnode(dtsgui_treeview tree, dtsgui_treenode tn, const char *xpath, const char *node, const char *vitem, const char *tattr,
							int nid, int flags, dtsgui_xmltreenode_cb node_cb, void *data, dtsgui_treeviewpanel_cb p_cb) {
	if (data && objref(data)) {
		this->data = data;
	} else {
		this->data = NULL;
	}
	this->tv = tree;
	this->tn = tn;
	ALLOC_CONST(this->xpath, xpath);
	ALLOC_CONST(this->node, node);
	ALLOC_CONST(this->vitem, vitem);
	ALLOC_CONST(this->tattr, tattr);
	this->flags = flags;
	this->type = nid;
	this->node_cb = node_cb;
	this->p_cb = p_cb;
}

int tree_newnode::handle_newtreenode_cb(class dtsgui *dtsgui, dtsgui_pane p, int type, int event, void *data) {
	class tree_newnode *nn = (class tree_newnode*)data;
	DTSPanel *dp = (DTSPanel*)p;

	if (type != wx_PANEL_EVENT_BUTTON) {
		return 1;
	}

	switch(event) {
		case wx_PANEL_EVENT_BUTTON_YES:
			break;
		default:
			return 1;
	}

	return nn->handle_newtreenode(dtsgui, dp);
}

int tree_newnode::handle_newtreenode(class dtsgui *dtsgui, DTSPanel *dp) {
	DTSTreeWindow *tw = (DTSTreeWindow*)tv;
	DTSDVMCtrl *tree = tw->GetTreeCtrl();
	DTSDVMListStore *ls;
	wxDataViewItem item, root;
	struct xml_node *xn;
	const char *name;

	if (!(xn = dp->Panel2XML(xpath, node, vitem, tattr))) {
		return 1;
	}

	if (tattr) {
		name = xml_getattr(xn, tattr);
	} else {
		name = xn->value;
	}

	root = wxDataViewItem(this->tn);
	if (flags & DTS_TREE_NEW_NODE_CONTAINER) {
		ls = (DTSDVMListStore*)tree->AppendContainer(root, name, flags & DTS_TREE_NEW_NODE_EDIT, flags & DTS_TREE_NEW_NODE_SORT, flags & DTS_TREE_NEW_NODE_DELETE, type, p_cb, data).GetID();
	} else {
		ls = (DTSDVMListStore*)tree->AppendItem(root, name, flags & DTS_TREE_NEW_NODE_EDIT, flags & DTS_TREE_NEW_NODE_SORT, flags & DTS_TREE_NEW_NODE_DELETE, type, p_cb, data).GetID();
	}
	ls->SetXMLData(xn, tattr);

	if (!tree->IsExpanded(root)) {
		tree->Expand(root);
	}

	/*the panel event manager holds a ref for nn*/
	if (node_cb) {
		node_cb(tv, ls, xn, data);
	}
	objunref(xn);

	item = wxDataViewItem(ls);
	tw->Select(item);

	return 0;
}

tab_newpane::tab_newpane(DTSTabWindow *tabv, const char *xpath, const char *node, const char *vitem, const char *tattr, dtsgui_tabpane_newdata_cb data_cb,
							dtsgui_tabpanel_cb cb, void *cdata, struct xml_doc *xmldoc, void *data) {
	DTSTabWindow *tv = (DTSTabWindow*)tabv;

	if (data && objref(data)) {
		this->data = data;
	}

	if (cdata && objref(cdata)) {
		this->cdata = cdata;
	}

	this->tabv = tabv;
	if (xmldoc && objref(xmldoc)) {
		this->xmldoc = xmldoc;
	}

	ALLOC_CONST(this->xpath, xpath);
	ALLOC_CONST(this->node, node);
	ALLOC_CONST(this->vitem, vitem);
	ALLOC_CONST(this->tattr, tattr);
	this->last = tv->GetPageCount() - 1;
	this->cb = cb;
	this->data_cb = data_cb;
}

tab_newpane::~tab_newpane() {
	if (data) {
		objunref(data);
	}

	if (cdata) {
		objunref(cdata);
	}

	if (xmldoc) {
		objunref(xmldoc);
	}

	if (xpath) {
		free((void*)xpath);
	}
	if (node) {
		free((void*)node);
	}
	if (vitem) {
		free((void*)vitem);
	}
	if (tattr) {
		free((void*)tattr);
	}
}

int tab_newpane::handle_newtabpane_cb(class dtsgui *dtsgui, dtsgui_pane p, int type, int event, void *data) {
	class tab_newpane *tp = (class tab_newpane*)data;
	DTSPanel *dp = (DTSPanel*)p;

	if (type != wx_PANEL_EVENT_BUTTON) {
		return 1;
	}

	switch(event) {
		case wx_PANEL_EVENT_BUTTON_YES:
			break;
		default:
			return 1;
	}

	return tp->handle_newtabpane(dtsgui, dp);
}

int tab_newpane::handle_newtabpane(class dtsgui *dtsgui, DTSPanel *dp) {
	DTSTabPage *np;
	struct xml_node *xn;
	const char *name;
	int pos = last;
	void *ndata = cdata;

	if (!(xn = dp->Panel2XML(xpath, node, vitem, tattr))) {
		return 1;
	}

	if (tattr) {
		name = xml_getattr(xn, tattr);
	} else {
		name = xn->value;
	}

	if (data_cb) {
		data_cb(xmldoc, xn, data, &ndata, &pos);
		if (cdata && (ndata != cdata)) {
			objunref(cdata);
			cdata = NULL;
		}
		if (ndata) {
			cdata = ndata;
		}
	}

	if ((np = tabv->CreateTab(name, wx_PANEL_BUTTON_ACTION, NULL, cb, cdata, xmldoc, false))) {
		tabv->InsertTab(np, pos);
		tabv->Undo(-1);
		tabv->SetSelection(pos);
		last++;
	}

	objunref(xn);
	return 0;
}

treemenu::treemenu() {
	menu = new wxMenu();
	msort = menu->Append(DTS_TREEWIN_MENU_SORT, "Sort");
	mup = menu->Append(DTS_TREEWIN_MENU_MOVEUP, "Move Up");
	mdown = menu->Append(DTS_TREEWIN_MENU_MOVEDOWN, "Move Down");
	menu->AppendSeparator();
	mdelete = menu->Append(DTS_TREEWIN_MENU_DELETE, "Delete");
}

treemenu::~treemenu() {
	delete menu;
}

bool treemenu::Show(wxWindow *w, bool cont, int cnt, bool first, bool last, bool c_sort, bool del) {
	int scnt = 0;

	if (!cont && del) {
		scnt++;
	}
	mdelete->Enable(!cont && del);

	if (!(cnt < 2) && c_sort) {
		scnt++;
	}
	msort->Enable(!(cnt < 2) && c_sort);

	if (!first || !last) {
		scnt++;
	}
	mup->Enable(!first);
	mdown->Enable(!last);

	if (scnt) {
		w->PopupMenu(menu);
		return true;
	} else {
		return false;
	}
}

DTSTreeWindowEvent::DTSTreeWindowEvent(void *userdata, dtsgui_tree_cb tree_cb, class dtsgui *dtsgui, DTSTreeWindow *win) {
	if (userdata && objref(userdata)) {
		data = userdata;
	} else {
		data = NULL;
	}
	treecb = tree_cb;
	parent = win;
	tree = win->GetTreeCtrl();
	vm = tree->GetStore();
	if (dtsgui && objref(dtsgui)) {
		this->dtsgui = dtsgui;
	} else {
		this->dtsgui = NULL;
	}
}

DTSTreeWindowEvent::~DTSTreeWindowEvent() {
	if (dtsgui) {
		objunref(dtsgui);
	}
	if (data) {
		objunref(data);
	}
}

void DTSTreeWindowEvent::TreeEvent(wxDataViewEvent &event) {
	DTSTreeWindow *tw;
	wxDataViewItem p_cont, f_item, l_item;
	int evid, cnt;
	bool cont,first = true,last = true;

	tw = (DTSTreeWindow*)parent;
	evid = event.GetEventType();

	if (evid == wxEVT_DATAVIEW_SELECTION_CHANGED) {
		a_item = event.GetItem();
		TreeCallback(a_item, DTSGUI_TREE_CB_SELECT);
		if ((vm->GetNodeID(a_item) == -1) && vm->IsContainer(a_item)) {
			tree->Expand(a_item);
		}
	} else if (evid == wxEVT_DATAVIEW_ITEM_EXPANDED) {
		parent->TreeResize();
	} else if (evid == wxEVT_DATAVIEW_ITEM_CONTEXT_MENU) {
		DTSDVMListStore *d_p, *d_i, *d_c;
		bool c_sort, del;

		if ((a_item = event.GetItem())) {
			if (vm->IsContainer(a_item)) {
				a_cont = a_item;
				p_cont = vm->GetParent(a_item);
				cont = true;
			} else {
				a_cont = vm->GetParent(a_item);
				p_cont = a_cont;
				cont = false;
			}

			d_p = (DTSDVMListStore*)p_cont.GetID();
			d_c = (DTSDVMListStore*)a_cont.GetID();
			cnt=vm->GetChildCount(p_cont);
			if (p_cont && d_p->can_sort && (cnt > 1)) {
				f_item = vm->GetNthChild(p_cont, 0);
				if (f_item != a_item) {
					first = false;
				}
				l_item = vm->GetNthChild(p_cont, cnt-1);
				if (l_item != a_item) {
					last = false;
				}
			}

			c_sort = (d_p && d_p->can_sort) || (d_c && d_c->can_sort);

			d_i = (DTSDVMListStore*)a_item.GetID();
			del = d_i->can_delete;
			tw->ShowRMenu(cont, vm->GetChildCount(a_cont), first, last, c_sort, del);
		}
	} else if (evid == wxEVT_DATAVIEW_ITEM_START_EDITING) {
		a_item = event.GetItem();
		DTSDVMListStore *data = (DTSDVMListStore*)event.GetItem().GetID();
		if (!data->can_edit) {
			event.Veto();
			if (data->IsContainer()) {
				if (tree->IsExpanded(a_item)) {
					tree->Collapse(a_item);
				} else {
					tree->Expand(a_item);
				}
			}
		}
	} else if (evid == wxEVT_DATAVIEW_ITEM_EDITING_DONE) {
#ifndef _WIN32
		DTSDVMListStore *data = (DTSDVMListStore*)event.GetItem().GetID();
		event.SetValue(data->GetTitle());
#endif
		parent->SetPaneTitle(event.GetValue());
		TreeCallback(event.GetItem(), DTSGUI_TREE_CB_EDIT);
	}
}

void DTSTreeWindowEvent::SortParent(const wxDataViewItem parent, int action) {
	wxDataViewItemArray expanded, selected;
	DTSDVMListStore *data;
	unsigned int cont_cnt, cnt;

	/* Get enabled/selection state of parent*/
	tree->GetSelections(selected);

	/*mark expanded containers*/
	vm->SetExpanded(parent, tree->IsExpanded(parent));
	cont_cnt = vm->GetContainers(parent, expanded, false);
	for (cnt=0; cnt < cont_cnt;cnt++) {
		vm->SetExpanded(expanded[cnt], tree->IsExpanded(expanded[cnt]));
	}

	switch(action) {
		case DTS_TREEWIN_MENU_MOVEDOWN:
			vm->MoveChildDown(a_item);
			break;
		case DTS_TREEWIN_MENU_MOVEUP:
			vm->MoveChildUp(a_item);
			break;
		case DTS_TREEWIN_MENU_SORT:
			vm->SortChildren(a_cont);
			break;
	}

	/*re select all selected*/
	for (cnt=0; cnt < selected.size();cnt++) {
		tree->Select(wxDataViewItem(selected[cnt]));
	}

	/*expand root if was expanded*/
	data = (DTSDVMListStore*)parent.GetID();
	if (data && data->IsExpanded()) {
		tree->Expand(parent);
		data->SetExpanded(false);
	}

	/*re expand containers colapsed*/
	for (cnt=0; cnt < expanded.size();cnt++) {
		data = (DTSDVMListStore*)expanded[cnt].GetID();
		if (data && data->IsExpanded()) {
			tree->Expand(expanded[cnt]);
			data->SetExpanded(false);
		}
	}
}

void DTSTreeWindowEvent::MenuEvent(wxCommandEvent &event) {
	enum treewinmenu eid;
	wxDataViewItem p_cont;
	DTSFrame *frame;

	eid=(treewinmenu)event.GetId();
	switch(eid) {
		case DTS_TREEWIN_MENU_MOVEDOWN:
		case DTS_TREEWIN_MENU_MOVEUP:
		case DTS_TREEWIN_MENU_SORT:
			p_cont = (a_cont == a_item) ? vm->GetParent(a_cont) : a_cont;
			SortParent(p_cont, eid);
			break;
		case DTS_TREEWIN_MENU_DELETE:
			frame = parent->GetFrame();
			if (frame->Confirm("Are you sure you want to delete this item")) {
				p_cont = vm->GetParent(a_item);
				tree->Select(p_cont);
				TreeCallback(p_cont, DTSGUI_TREE_CB_SELECT);
				TreeCallback(a_item, DTSGUI_TREE_CB_DELETE);
				vm->Delete(a_item);
			}
			break;
	}
}

void DTSTreeWindowEvent::SplitterEvent(wxSplitterEvent& event) {
	DTSTreeWindow *tw;
	int evid;

	tw = (DTSTreeWindow*)parent;
	evid = event.GetEventType();

	if (evid == wxEVT_SPLITTER_SASH_POS_CHANGED) {
		tw->SetTreePaneSize();
	}
}

void DTSTreeWindowEvent::TreeCallback(const wxDataViewItem item, enum tree_cbtype type) {
	void *tdata = NULL;
	DTSPanel *sp = NULL;
	wxWindow *op;
	DTSDVMListStore *ndata = (item.IsOk()) ? (DTSDVMListStore*)item.GetID() : NULL;

	if (!ndata) {
		return;
	}

	if (objref(data)) {
		tdata = data;
	}

	if (treecb) {
		sp = (DTSPanel*)treecb(dtsgui, parent, ndata, type, ndata->GetTitle().ToUTF8(), tdata);
	} else {
		switch(type) {
			case DTSGUI_TREE_CB_SELECT:
				sp = parent->CreatePane(ndata);
				break;
			case DTSGUI_TREE_CB_EDIT:
				dtsgui_nodesetxml(parent, ndata, ndata->GetTitle().ToUTF8());
				break;
			case DTSGUI_TREE_CB_DELETE:
				break;
		}
	}

	if (sp) {
		ndata->ConfigPanel(sp, parent);
		op = parent->SetWindow(sp, item);
		delete op;
	}

	if (tdata) {
		objunref(tdata);
	}

}

void DTSTreeWindowEvent::OnButton(wxCommandEvent &event) {
	dtsgui_treenode tn = parent->GetActiveNode();
	wxDataViewItem item;
	int eid = event.GetId();
	DTSPanel *pane = parent->GetClientPane();

	switch(eid) {
		case wx_PANEL_EVENT_BUTTON_YES:
			pane->Update_XML();
			break;
		case wx_PANEL_EVENT_BUTTON_NO:
			item = wxDataViewItem(tn);
			parent->Select(item);
			return;
		default:
			break;
	}
	event.Skip(true);
}

DTSTreeWindow::DTSTreeWindow(wxWindow *parent, DTSFrame *frame, dtsgui_tree_cb tree_cb, wxString stat_msg, int pos, void *u_data)
	:wxSplitterWindow(parent, -1, wxDefaultPosition, wxDefaultSize),
	 DTSObject(stat_msg) {

	int w, h, p, psize;
	class dtsgui *udata;
	a_window = NULL;
	DTSTreeWindowEvent *dtsevt;
	wxSplitterWindow *sw = static_cast<wxSplitterWindow*>(this);
	wxBoxSizer *p_sizer = new wxBoxSizer(wxHORIZONTAL);
	treesizer = new wxBoxSizer(wxVERTICAL);
	wxDataViewItem root, root4;
	wxWindow *aw;

	rmenu = new treemenu();

	if (u_data && objref(u_data)) {
		userdata = u_data;
	} else {
		userdata = NULL;
	}
	this->frame = frame;

	p_sizer->Add(sw, 1,wxEXPAND,0);
#ifdef __WIN32
	t_pane = new wxScrolledWindow(sw, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
#else
	t_pane = new wxScrolledWindow(sw, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL);
#endif // _WIN32
	panel = static_cast<wxWindow *>(sw);

	a_window = new DTSWindow(panel, frame);
	a_window->type = wx_DTSPANEL_TREE;
	aw = a_window->GetPanel();

	SplitVertically(t_pane, aw);
	SetSashGravity(0.5);
	SetMinimumPaneSize(20);

	t_pane->SetSizer(treesizer);
#ifdef __WIN32
	t_pane->SetScrollRate(0, 10);
#else
	t_pane->SetScrollRate(10, 10);
#endif // _WIN32

	vm = new DTSDVMListView(1, true);
	tree = new DTSDVMCtrl(t_pane, wxID_ANY, vm, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES|wxDV_NO_HEADER);

	udata = (frame) ? frame->GetDTSData() : NULL;
	dtsevt = new DTSTreeWindowEvent(userdata, tree_cb, udata, this);
	if (udata) {
		objunref(udata);
	}
	dtsevthandler = dtsevt;
	treesizer->Add(tree, 1,wxEXPAND,0);

	p_sizer->FitInside(parent);
	p_sizer->Layout();
	p_sizer->SetSizeHints(sw);

	parent->GetSize(&w, &h);
	p = (w * pos) / 100;
	SetSashPosition(p, true);

	treesizer->SetSizeHints(t_pane);
	treesizer->FitInside(t_pane);
	treesizer->Layout();

	tree->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &DTSTreeWindowEvent::TreeEvent, dtsevt);
	tree->Bind(wxEVT_DATAVIEW_ITEM_EXPANDED, &DTSTreeWindowEvent::TreeEvent, dtsevt);
	tree->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &DTSTreeWindowEvent::TreeEvent, dtsevt);
	tree->Bind(wxEVT_DATAVIEW_ITEM_EDITING_DONE, &DTSTreeWindowEvent::TreeEvent, dtsevt);
	tree->Bind(wxEVT_DATAVIEW_ITEM_START_EDITING, &DTSTreeWindowEvent::TreeEvent, dtsevt);
	frame->Bind(wxEVT_DATAVIEW_ITEM_BEGIN_DRAG, &DTSTreeWindowEvent::TreeEvent, dtsevt);
	tree->Bind(wxEVT_COMMAND_MENU_SELECTED, &DTSTreeWindowEvent::MenuEvent, dtsevt);
	sw->Bind(wxEVT_SPLITTER_SASH_POS_CHANGED, &DTSTreeWindowEvent::SplitterEvent, dtsevt);
	sw->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DTSTreeWindowEvent::OnButton, dtsevt);
	tree->EnableDragSource(wxDF_UNICODETEXT);
	tree->EnableDropTarget(wxDF_UNICODETEXT);

	Show(false);

/*	root = tree->AppendContainer(wxDataViewItem(NULL), "The Root");
	tree->AppendItem(root, "Child E");
	tree->AppendContainer(root, "Root2");
	tree->AppendItem(root, "Child D");
	tree->AppendContainer(root, "Root3");
	tree->AppendItem(root, "Child C");
	root4 = tree->AppendContainer(root, "Root4");
	tree->AppendItem(root, "Child B");
	tree->AppendItem(root, "Child A");

	tree->AppendItem(root4, "Child E", true, true, true);
	tree->AppendItem(root4, "Child D", true, true, true);
	tree->AppendItem(root4, "Child C", true, true, true);
	tree->AppendItem(root4, "Child B", true, true, true);
	tree->AppendItem(root4, "Child A", true, true, true);*/

	psize = p - GetSashSize();
#ifdef _WIN32
	tree->GetColumn(0)->SetWidth(psize - wxSYS_VSCROLL_X);
#else
	tree->GetColumn(0)->SetMinWidth(psize);
#endif // _WIN32
}

DTSDVMCtrl *DTSTreeWindow::GetTreeCtrl() {
	return tree;
}

void DTSTreeWindow::SetTreePaneSize() {
	int psize = GetSashPosition() - GetSashSize();
#ifdef _WIN32
		tree->GetColumn(0)->SetWidth(psize - wxSYS_VSCROLL_X);
#else
		tree->GetColumn(0)->SetMinWidth(psize);
#endif // _WIN32
}

void DTSTreeWindow::TreeResize() {
	tree->GetColumn(0)->SetHidden(true);
	tree->GetColumn(0)->SetHidden(false);
}

void DTSTreeWindow::SetPaneTitle(const wxString value) {
	DTSPanel *p;

	if ((p = dynamic_cast<DTSPanel*>(a_window))) {
		p->SetTitle(value);
	}

	TreeResize();
}

dtsgui_treenode DTSTreeWindow::GetActiveNode() {
	return a_node;
}

wxWindow *DTSTreeWindow::SetWindow(DTSPanel *window, const wxDataViewItem& item) {
	DTSPanel *oldwin;
	wxWindow *nw, *cw;

	if (!window || (window == a_window)) {
		return NULL;
	}

	if ((oldwin = a_window)) {
		a_window->Show(false);
	}

	nw = window->GetPanel();
	cw = a_window->GetPanel();

	ReplaceWindow(cw, nw);
	a_window = window;
	a_node = item.GetID();
	nw->Show(true);
	nw->Layout();
	nw->FitInside();
	return oldwin;
}

void DTSTreeWindow::ShowRMenu(bool cont, int cnt, bool first, bool last, bool c_sort, bool del) {
	rmenu->Show(tree, cont, cnt, first, last, c_sort, del);
}

DTSScrollPanel *DTSTreeWindow::CreatePane(const wxString &name, int butmask, void *userdata, struct xml_doc *xmldoc) {
	DTSScrollPanel *dp;
	wxWindow *parent;
	DTSFrame *f;

	parent = GetPanel();
	f = GetFrame();

	dp = new DTSScrollPanel(parent, f, name, butmask);
	dp->type = wx_DTSPANEL_TREE;

	if (name.Len() > 0) {
		dp->SetTitle(name, true);
	}

	if (xmldoc) {
		dp->SetXMLDoc(xmldoc);
	}
	return dp;
}

DTSScrollPanel *DTSTreeWindow::CreatePane(DTSDVMListStore *ls) {
	DTSScrollPanel *p;
	int nodeid;
	wxString name;

	nodeid = ls->GetNodeID();

	if (nodeid == -1) {
		p = CreatePane();
	} else {
		name = ls->GetTitle();
		p = CreatePane(name, wx_PANEL_BUTTON_ACTION, NULL, xmldoc);
	}
	return p;

}

DTSTreeWindow::~DTSTreeWindow() {
	delete t_pane;
	delete a_window;
	objunref(rmenu);
	if (userdata) {
		objunref(userdata);
	}
}

bool DTSTreeWindow::Show(bool show) {
	if (show) {
		TreeResize();

		if (!beenshown) {
			wxDataViewItem root;
			DTSDVMListStore *ds;
			DTSDVMListView *model;

			model = tree->GetStore();
			ds = model->GetRoot();
			root = wxDataViewItem(ds);
			Select(root);
			beenshown = true;
		}

		if (frame) {
			frame->SetStatusText(status);
		}
	}
	return wxSplitterWindow::Show(show);
}

DTSPanel *DTSTreeWindow::GetClientPane() {
	return a_window;
}

void DTSTreeWindow::Select(const wxDataViewItem& item) {
	if (vm->IsContainer(item)) {
		tree->Expand(item);
	}
	tree->Select(item);

	wxDataViewEvent event(wxEVT_DATAVIEW_SELECTION_CHANGED, -1);
	event.SetItem(item);
	tree->ProcessWindowEvent(event);
}

DTSTabWindowEvent::DTSTabWindowEvent(void *userdata, DTSTabWindow *win) {
	if (userdata && objref(userdata)) {
		data = userdata;
	} else {
		data = NULL;
	}
	tw = win;
}

DTSTabWindowEvent::~DTSTabWindowEvent() {
	if (data) {
		objunref(data);
	}
}

void DTSTabWindowEvent::OnButton(wxCommandEvent &event) {
	DTSTabPage *pane = dynamic_cast<DTSTabPage*>(tw->GetCurrentPage());
	int eid = event.GetId();
	int pg = tw->GetSelection();

	switch(eid) {
		case wx_PANEL_EVENT_BUTTON_YES:
			pane->Update_XML();
		/*no break here will reload tab*/
		case wx_PANEL_EVENT_BUTTON_NO:
			pg++;
			tw->Undo(pg);
			return;
		default:
			break;
	}
	event.Skip(true);
}


void DTSTabWindowEvent::RightMenu(wxCommandEvent &event) {
	printf("MENU\n");
}

void DTSTabWindowEvent::PageChange(wxBookCtrlEvent &event) {
	wxWindow *w;
	DTSTabPage *tp;
	int p = event.GetSelection();

	if (p != wxNOT_FOUND) {
		w = tw->GetPage(p);
		tp = dynamic_cast<DTSTabPage*>(w);
		tp->ConfigPane();
	}
}

void DTSTabWindowEvent::PageChanged(wxBookCtrlEvent &event) {
	wxWindow *w;
	int p = event.GetSelection();

	if (p != wxNOT_FOUND) {
		w = tw->GetPage(p);
		w->FitInside();
		w->Layout();
		w->Refresh();
	}
}

DTSTabWindow::DTSTabWindow(DTSFrame *frame, wxString stat_msg, void *u_data)
	:wxNotebook((wxWindow*)frame, -1),
	DTSObject(stat_msg) {

	DTSTabWindowEvent *dtsevt;
	wxNotebook *nb = (wxNotebook*)this;

	panel = dynamic_cast<wxBookCtrlBase*>(this);

	type = wx_DTSPANEL_TAB;
	this->frame = frame;

	if (u_data && objref(u_data)) {
		userdata = u_data;
	}

	dtsevt = new DTSTabWindowEvent(userdata, this);
	dtsevthandler = dtsevt;

	nb->Bind(wxEVT_CONTEXT_MENU, &DTSTabWindowEvent::RightMenu, dtsevt);

	Show(false);
}

DTSTabWindow::~DTSTabWindow() {
	if (userdata) {
		objunref(userdata);
	}
}


void DTSTabWindow::Undo(int pg) {
	DTSTabPage *pane, *newp;
	int idx;

	if (pg == 0) {
		return;
	} else if (pg > 0) {
		idx = pg-1;
	} else {
		idx = GetPageCount() + pg;
	}

	pane = dynamic_cast<DTSTabPage*>(GetPage(idx));
	RemovePage(idx);

	newp = new DTSTabPage(this);
	*newp = *pane;
	delete pane;

	InsertTab(newp, idx);
}

bool DTSTabWindow::Show(bool show) {
	wxNotebook *nb = (wxNotebook*)this;
	DTSTabWindowEvent *dtsevt = (DTSTabWindowEvent*)dtsevthandler;
	wxWindow *w;
	int i, cnt;
	bool res;
#ifdef __WIN32
	DTSTabPage *tp;
#endif // __WIN32

	if (show && frame) {
		frame->SetStatusText(status);
	}

	res = wxNotebook::Show(show);

	if (!beenshown && show && res) {
		cnt = GetPageCount();
		for(i = 0; i < cnt;i++) {
			w = GetPage(i);
#ifdef __WIN32
			tp = dynamic_cast<DTSTabPage*>(w);
			tp->ConfigPane();
			w->Show(true);
			if (i) {
				w->Show(false);
			}
#else
			w->Show(true);
#endif // __WIN32
		}
		beenshown = true;
		nb->Bind(wxEVT_NOTEBOOK_PAGE_CHANGING, &DTSTabWindowEvent::PageChange, dtsevt);
		nb->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, &DTSTabWindowEvent::PageChanged, dtsevt);
		nb->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DTSTabWindowEvent::OnButton, dtsevt);

	}
	return res;
}

DTSTabPage *DTSTabWindow::CreateTab(const wxString &name, int butmask, void *userdata, dtsgui_tabpanel_cb cb, void *cdata, struct xml_doc *xmldoc, bool addpage) {
	DTSTabPage *dp;
	DTSFrame *f;
	wxBookCtrlBase *nb;

	f = GetFrame();
	nb = static_cast<wxBookCtrlBase*>(this);

	if (!(dp = new DTSTabPage(nb, f, name, butmask, cb, cdata, xmldoc))) {
		return NULL;
	}

	if (userdata) {
		dp->SetUserData(userdata);
	}

	if (addpage) {
		nb->AddPage(dp->GetPanel(), name);
	}

	return dp;
}

void DTSTabWindow::InsertTab(DTSTabPage *panel, int pos, bool sel, int undo) {
	wxBookCtrlBase *nb = dynamic_cast<wxBookCtrlBase*>(this);
	DTSObject *dp = static_cast<DTSObject*>(panel);
	wxWindow *w = panel->GetPanel();

	objlock(refobj);
	nb->InsertPage(pos, w, dp->GetName());
	objunlock(refobj);
	w->Show(true);

	if (sel) {
		SetSelection(pos);
	}
	if (undo) {
		Undo(undo);
	}
}
