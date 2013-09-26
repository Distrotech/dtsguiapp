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
	data = userdata;
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
}

void DTSTreeWindowEvent::TreeEvent(wxDataViewEvent &event) {
	DTSTreeWindow *tw;
	wxDataViewItem p_cont, f_item, l_item;
	int evid, cnt;
	bool cont,first = true,last = true;

	tw = (DTSTreeWindow*)parent;
	evid = event.GetEventType();

	if (evid == wxEVT_DATAVIEW_SELECTION_CHANGED) {
		TreeCallback(event.GetItem(), DTSGUI_TREE_CB_SELECT);
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
		DTSDVMListStore *data = (DTSDVMListStore*)event.GetItem().GetID();
		if (!data->can_edit) {
			event.Veto();
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

void DTSTreeWindowEvent::MenuEvent(wxCommandEvent &event) {	enum treewinmenu eid;
	wxDataViewItem p_cont;
	DTSFrame *frame;

	p_cont = (a_cont == a_item) ? vm->GetParent(a_cont) : a_cont;

	eid=(treewinmenu)event.GetId();
	switch(eid) {
		case DTS_TREEWIN_MENU_MOVEDOWN:
		case DTS_TREEWIN_MENU_MOVEUP:
		case DTS_TREEWIN_MENU_SORT:
			SortParent(p_cont, eid);
			break;
		case DTS_TREEWIN_MENU_DELETE:
			frame = parent->GetFrame();
			if (frame->Confirm("Are you sure you want to delete this item")) {
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
	DTSPanel *sp;
	wxWindow *op, *w;

	if (item && treecb) {
		DTSDVMListStore *ndata = (item.IsOk()) ? (DTSDVMListStore*)item.GetID() : NULL;
		if (objref(data)) {
			tdata = data;
		}
		if ((sp = (DTSPanel*)treecb(dtsgui, parent, item, type, ndata->GetTitle().ToUTF8(), tdata))) {
			w = sp->GetPanel();
			op = parent->SetWindow(w);
			delete op;
		}

		if (tdata) {
			objunref(tdata);
		}
	}
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
	wxSplitterWindow *sw = static_cast<wxSplitterWindow*>(this);
	wxBoxSizer *p_sizer = new wxBoxSizer(wxHORIZONTAL);
	treesizer = new wxBoxSizer(wxVERTICAL);
	wxDataViewItem root, root4;

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

	userdata = u_data;
	this->frame = frame;

	p_sizer->Add(sw, 1,wxEXPAND,0);
	t_pane = new wxScrolledWindow(sw, wxID_ANY);
	a_window = new wxWindow(sw, wxID_ANY);

	panel = static_cast<wxWindow *>(sw);
	type = wx_DTSPANEL_TREE;

	SplitVertically(t_pane, a_window);
	SetSashGravity(0.5);
	SetMinimumPaneSize(20);

	t_pane->SetSizer(treesizer);
	t_pane->SetScrollRate(10, 10);
	vm = new DTSDVMListView(1, true);
	tree = new DTSDVMCtrl(t_pane, wxID_ANY, vm, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES|wxDV_NO_HEADER);

	dtsevthandler = new DTSTreeWindowEvent(userdata, tree_cb, (frame) ? frame->GetDTSData() : NULL, this);
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

	tree->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &DTSTreeWindowEvent::TreeEvent, dtsevthandler);
	tree->Bind(wxEVT_DATAVIEW_ITEM_EXPANDED, &DTSTreeWindowEvent::TreeEvent, dtsevthandler);
	tree->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &DTSTreeWindowEvent::TreeEvent, dtsevthandler);
	tree->Bind(wxEVT_DATAVIEW_ITEM_EDITING_DONE, &DTSTreeWindowEvent::TreeEvent, dtsevthandler);
	tree->Bind(wxEVT_DATAVIEW_ITEM_START_EDITING, &DTSTreeWindowEvent::TreeEvent, dtsevthandler);
	frame->Bind(wxEVT_DATAVIEW_ITEM_BEGIN_DRAG, &DTSTreeWindowEvent::TreeEvent, dtsevthandler);
	tree->Bind(wxEVT_COMMAND_MENU_SELECTED, &DTSTreeWindowEvent::MenuEvent, dtsevthandler);
	sw->Bind(wxEVT_SPLITTER_SASH_POS_CHANGED, &DTSTreeWindowEvent::SplitterEvent, dtsevthandler);

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
	tree->GetColumn(0)->SetWidth(psize);
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
		tree->GetColumn(0)->SetWidth(psize);
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

wxWindow *DTSTreeWindow::SetWindow(wxWindow *window) {
	wxWindow *oldwin;

	if (!window || (window == a_window)) {
		return NULL;
	}

	if ((oldwin = a_window)) {
		a_window->Show(false);
	}

	ReplaceWindow(a_window, window);
	a_window = window;
	window->Show(true);
	window->Layout();
	window->FitInside();
//	UpdateSize();

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
	delete dtsevthandler;
	objunref(rmenu);
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
			tree->Select(root);
			tree->Expand(root);
			dtsevthandler->TreeCallback(root, DTSGUI_TREE_CB_SELECT);
			beenshown = true;
		}

		if (frame) {
			frame->SetStatusText(status);
		}
	}
	return wxSplitterWindow::Show(show);
}

wxWindow *DTSTreeWindow::GetClientPane() {
	return a_window;
}

DTSTabWindow::DTSTabWindow(DTSFrame *frame, wxString stat_msg)
	:wxNotebook((wxWindow*)frame, -1),
	DTSObject(stat_msg) {

	wxNotebook *nb = static_cast<wxNotebook*>(this);

	panel = static_cast<wxWindow *>(nb);
	type = wx_DTSPANEL_TAB;
	this->frame = frame;
}

bool DTSTabWindow::Show(bool show) {
	if (show && frame) {
		frame->SetStatusText(status);
	}
	return wxNotebook::Show(show);
}
