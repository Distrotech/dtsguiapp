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

DTSTreeWindowEvent::DTSTreeWindowEvent(void *userdata, event_callback ev_cb, DTSTreeWindow *win) {
	data = userdata;
	evcb = ev_cb;
	parent = win;
	tree = win->GetTreeCtrl();
	vm = tree->GetStore();
}

void DTSTreeWindowEvent::TreeEvent(wxDataViewEvent &event) {
	DTSTreeWindow *tw;
	wxDataViewItem p_cont, f_item, l_item;
	int evid, cnt;
	bool cont,first = true,last = true;

	tw = (DTSTreeWindow*)parent;
	evid = event.GetEventType();

	if (evid == wxEVT_DATAVIEW_SELECTION_CHANGED) {
		printf("Got ya\n");
	} else if (evid == wxEVT_DATAVIEW_ITEM_EXPANDED) {
		parent->TreeResize();
	} else if (evid == wxEVT_DATAVIEW_ITEM_CONTEXT_MENU) {
		if ((a_item = event.GetItem())) {
			if (vm->IsContainer(a_item)) {
				a_cont = a_item;
				p_cont = vm->GetParent(a_cont);
				cont = true;
			} else {
				a_cont = vm->GetParent(a_item);
				p_cont = a_cont;
				cont = false;
			}

			cnt=vm->GetChildCount(p_cont);
			if (p_cont && (cnt > 1)) {
				f_item = vm->GetNthChild(p_cont, 0);
				if (f_item != a_item) {
					first = false;
				}
				l_item = vm->GetNthChild(p_cont, cnt-1);
				if (l_item != a_item) {
					last = false;
				}
			}
			tw->ShowRMenu(cont, vm->GetChildCount(a_cont), first, last);
		} else if (evid == wxEVT_DATAVIEW_ITEM_BEGIN_DRAG) {
			printf("DRAG\n");
		}
	} else if (evid == wxEVT_DATAVIEW_ITEM_EDITING_DONE) {
		parent->TreeResize();
	}
}

bool DTSTreeWindowEvent::ParentConfig(const wxDataViewItem parent, wxDataViewItemArray &cont, wxDataViewItemArray &sel) {
	bool expand;
	unsigned int cont_cnt, cnt;

	/* Get enabled/selection state of parent*/
	expand = tree->IsExpanded(parent);
	tree->GetSelections(sel);

	/*mark expanded containers*/
	cont_cnt = vm->GetContainers(parent, cont, false);
	for (cnt=0; cnt < cont_cnt;cnt++) {
		vm->SetExpanded(cont[cnt], tree->IsExpanded(cont[cnt]));
	}
	return expand;
}

void DTSTreeWindowEvent::MenuEvent(wxCommandEvent &event) {	enum treewinmenu eid;
	wxDataViewItemArray items, sel;
	wxDataViewItem p_cont;
	DTSFrame *frame;
	bool expand;

	p_cont = (a_cont == a_item) ? vm->GetParent(a_cont) : a_cont;

	eid=(treewinmenu)event.GetId();
	switch(eid) {
		case DTS_TREEWIN_MENU_MOVEDOWN:
			expand = ParentConfig(p_cont, items, sel);
			vm->MoveChildDown(a_item);
			tree->ReloadParent(p_cont, expand, items, sel);
			break;
		case DTS_TREEWIN_MENU_MOVEUP:
			expand = ParentConfig(p_cont, items, sel);
			vm->MoveChildUp(a_item);
			tree->ReloadParent(p_cont, expand, items, sel);
			break;
		case DTS_TREEWIN_MENU_DELETE:
			frame = parent->GetFrame();
			if (frame->Confirm("Are you sure you want to delete this item")) {
				vm->Delete(a_item);
			}
			break;
		case DTS_TREEWIN_MENU_SORT:
			expand = ParentConfig(p_cont, items, sel);
			vm->SortChildren(a_cont);
			tree->ReloadParent(a_cont, expand, items, sel);
			break;
	}
}

void DTSTreeWindowEvent::SplitterEvent(wxSplitterEvent& event) {
	DTSTreeWindow *tw;
	int psize;
	int evid;

	tw = (DTSTreeWindow*)parent;
	evid = event.GetEventType();

	if (evid == wxEVT_SPLITTER_SASH_POS_CHANGED) {
		psize = tw->GetTreePaneSize();
#ifdef _WIN32
		tree->GetColumn(0)->SetWidth(psize);
#else
		tree->GetColumn(0)->SetMinWidth(psize);
#endif // _WIN32
	}
}

void free_menu(void *data) {
	struct treemenu *rmenu = (struct treemenu*)data;

	if (rmenu->menu) {
		delete rmenu->menu;
	}
}

DTSTreeWindow::DTSTreeWindow(wxWindow *parent, DTSFrame *frame, wxString stat_msg, int pos)
	:wxSplitterWindow(parent, -1, wxDefaultPosition, wxDefaultSize),
	 DTSObject(stat_msg) {

	int w, h, p;
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

	this->frame = frame;

	p_sizer->Add(sw, 1,wxEXPAND,0);
	t_pane = new wxScrolledWindow(sw, wxID_ANY);
	c_pane = new wxWindow(sw, wxID_ANY);

	panel = static_cast<wxWindow *>(sw);
	type = wx_DTSPANEL_TREE;

	SplitVertically(t_pane, c_pane);
	SetSashGravity(0.5);
	SetMinimumPaneSize(20);

	t_pane->SetSizer(treesizer);
	t_pane->SetScrollRate(10, 10);
	vm = new DTSDVMListView(1, true);
	tree = new DTSDVMCtrl(t_pane, wxID_ANY, vm, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES|wxDV_NO_HEADER);

	dtsevthandler = new DTSTreeWindowEvent(NULL, NULL, this);
	treesizer->Add(tree, 1,wxEXPAND,0);

	sizer = new wxBoxSizer(wxHORIZONTAL);
	c_pane->SetSizer(sizer);

	p_sizer->FitInside(parent);
	p_sizer->Layout();
	p_sizer->SetSizeHints(sw);

	parent->GetSize(&w, &h);
	p = (w * pos) / 100;
	SetSashPosition(p, true);

	tree->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &DTSTreeWindowEvent::TreeEvent, dtsevthandler);
	tree->Bind(wxEVT_DATAVIEW_ITEM_EXPANDED, &DTSTreeWindowEvent::TreeEvent, dtsevthandler);
	tree->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &DTSTreeWindowEvent::TreeEvent, dtsevthandler);
	tree->Bind(wxEVT_DATAVIEW_ITEM_EDITING_DONE, &DTSTreeWindowEvent::TreeEvent, dtsevthandler);
	frame->Bind(wxEVT_DATAVIEW_ITEM_BEGIN_DRAG, &DTSTreeWindowEvent::TreeEvent, dtsevthandler);
	tree->Bind(wxEVT_COMMAND_MENU_SELECTED, &DTSTreeWindowEvent::MenuEvent, dtsevthandler);
	sw->Bind(wxEVT_SPLITTER_SASH_POS_CHANGED, &DTSTreeWindowEvent::SplitterEvent, dtsevthandler);

	treesizer->SetSizeHints(t_pane);
	treesizer->FitInside(t_pane);
	treesizer->Layout();

	tree->EnableDragSource(wxDF_UNICODETEXT);
	tree->EnableDropTarget(wxDF_UNICODETEXT);

	Show(false);

	root = tree->AppendContainer(wxDataViewItem(NULL), "The Root");
	tree->Select(root);

	tree->AppendItem(root, "Child E");
	tree->AppendContainer(root, "Root2");
	tree->AppendItem(root, "Child D");
	tree->AppendContainer(root, "Root3");
	tree->AppendItem(root, "Child C");
	root4 = tree->AppendContainer(root, "Root4");
	tree->AppendItem(root, "Child B");
	tree->AppendItem(root, "Child A");

	tree->AppendItem(root4, "Child E");
	tree->AppendItem(root4, "Child D");
	tree->AppendItem(root4, "Child C");
	tree->AppendItem(root4, "Child B");
	tree->AppendItem(root4, "Child A");

	/*can only expand when not empty*/
	tree->Expand(root);
}

DTSDVMCtrl *DTSTreeWindow::GetTreeCtrl() {
	return tree;
}

int DTSTreeWindow::GetTreePaneSize() {
	return GetSashPosition() - GetSashSize();
}

void DTSTreeWindow::TreeResize() {
	tree->GetColumn(0)->SetHidden(true);
	tree->GetColumn(0)->SetHidden(false);
}

void DTSTreeWindow::SetWindow(wxWindow *window) {
	if (!window || (window == a_window)) {
		return;
	}

	if (a_window) {
		sizer->Detach(0);
		a_window->Show(false);
		sizer->Prepend(window, 1, wxALL | wxEXPAND);
	} else {
		sizer->Add(window, 1, wxALL | wxEXPAND);
	}

	window->Show(true);
	sizer->Layout();
	sizer->FitInside(c_pane);
	a_window = window;
}

void DTSTreeWindow::ShowRMenu(bool cont, int cnt, bool first, bool last) {
	wxMenu *menu;

	if (!rmenu || !(menu = rmenu->menu)) {
		return;
	}
	rmenu->mdelete->Enable(!cont);
	rmenu->mup->Enable(!first);
	rmenu->mdown->Enable(!last);
	rmenu->msort->Enable(!(cnt < 2));
	tree->PopupMenu(menu);
}

DTSTreeWindow::~DTSTreeWindow() {
	delete t_pane;
	delete c_pane;
	delete dtsevthandler;
	objunref(rmenu);
}

bool DTSTreeWindow::Show(bool show) {
	if (show) {
		TreeResize();
		if (frame) {
			frame->SetStatusText(status);
		}

	}
	return wxSplitterWindow::Show(show);
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
