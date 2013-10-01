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

#include <wx/splitter.h>
#include <wx/panel.h>
#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include <wx/dialog.h>
#include <wx/scrolwin.h>
#include <wx/dataview.h>
#include <wx/wizard.h>
#include <wx/notebook.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/stattext.h>
#include <wx/progdlg.h>
#include <wx/bookctrl.h>

#include <stdint.h>
#include <dtsapp.h>
#include "dtsgui.hpp"

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

DTSTreeWindowEvent::DTSTreeWindowEvent(void *userdata, dtsgui_tree_cb tree_cb, struct dtsgui *dtsgui, DTSTreeWindow *win) {
	if (userdata && objref(userdata)) {
		data = userdata;
	} else {
		data = NULL;
	}
	treecb = tree_cb;
	parent = win;
	tree = win->GetTreeCtrl();
	vm = tree->GetStore();
	this->dtsgui = dtsgui;
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
				sp = (DTSPanel*)dtsgui_treepane_defalt(parent, ndata);
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
		case wx_PANEL_BUTTON_YES:
			pane->Update_XML();
			break;
		case wx_PANEL_BUTTON_NO:
			item = wxDataViewItem(tn);
			parent->Select(item);
			return;
		default:
			break;
	}
	event.Skip(true);
}

void free_menu(void *data) {
	struct treemenu *rmenu = (struct treemenu*)data;

	if (rmenu->menu) {
		delete rmenu->menu;
	}
}

DTSTreeWindow::DTSTreeWindow(wxWindow *parent, DTSFrame *frame, dtsgui_tree_cb tree_cb, wxString stat_msg, int pos, void *u_data)
	:wxSplitterWindow(parent, -1, wxDefaultPosition, wxDefaultSize),
	 DTSObject(stat_msg) {

	int w, h, p, psize;
	a_window = NULL;
	DTSTreeWindowEvent *dtsevt;
	wxSplitterWindow *sw = static_cast<wxSplitterWindow*>(this);
	wxBoxSizer *p_sizer = new wxBoxSizer(wxHORIZONTAL);
	treesizer = new wxBoxSizer(wxVERTICAL);
	wxDataViewItem root, root4;
	wxWindow *aw;

	if ((rmenu = (struct treemenu*)objalloc(sizeof(*rmenu), free_menu))) {
		wxMenu *menu;
		rmenu->menu = new wxMenu();
		menu = rmenu->menu;
		rmenu->msort = menu->Append(DTS_TREEWIN_MENU_SORT, "Sort");
		rmenu->mup = menu->Append(DTS_TREEWIN_MENU_MOVEUP, "Move Up");
		rmenu->mdown = menu->Append(DTS_TREEWIN_MENU_MOVEDOWN, "Move Down");
		menu->AppendSeparator();
		rmenu->mdelete = menu->Append(DTS_TREEWIN_MENU_DELETE, "Delete");
	}

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
	aw = a_window->GetPanel();
	type = wx_DTSPANEL_TREE;

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

	dtsevt = new DTSTreeWindowEvent(userdata, tree_cb, (frame) ? frame->GetDTSData() : NULL, this);
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
	wxMenu *menu;
	int scnt = 0;

	if (!rmenu || !(menu = rmenu->menu)) {
		return;
	}
	if (!cont && del) {
		scnt++;
	}
	rmenu->mdelete->Enable(!cont && del);

	if (!(cnt < 2) && c_sort) {
		scnt++;
	}
	rmenu->msort->Enable(!(cnt < 2) && c_sort);

	if (!first || !last) {
		scnt++;
	}
	rmenu->mup->Enable(!first);
	rmenu->mdown->Enable(!last);

	if (scnt) {
		tree->PopupMenu(menu);
	}
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

void DTSTabWindowEvent::RightMenu(wxCommandEvent &event) {
	printf("MENU\n");
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

	nb->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, &DTSTabWindowEvent::PageChanged, dtsevt);
	nb->Bind(wxEVT_CONTEXT_MENU, &DTSTabWindowEvent::RightMenu, dtsevt);

	Show(false);
}

DTSTabWindow::~DTSTabWindow() {
	if (userdata) {
		objunref(userdata);
	}
	DeleteAllPages();
}

bool DTSTabWindow::Show(bool show) {
	wxWindow *w;
	int i, cnt;
	bool res;

	if (show && frame) {
		frame->SetStatusText(status);
	}

	res = wxNotebook::Show(show);

	if (!beenshown && show && res) {
		cnt = GetPageCount();
		for(i = 0; i < cnt;i++) {
			w = GetPage(i);
			w->Show(true);
#ifdef __WIN32
			if (i) {
				w->Show(false);
			}
#endif // __WIN32
		}
		beenshown = true;
	}
	return res;
}
