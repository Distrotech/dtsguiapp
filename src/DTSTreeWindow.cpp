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
	wxDataViewItemArray items;
	int evid, cnt;
	bool cont,first = true,last = true;

	tw = (DTSTreeWindow*)parent;
	evid = event.GetEventType();

	if (evid == wxEVT_DATAVIEW_SELECTION_CHANGED) {
		printf("Got ya\n");
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

			cnt=tree->GetChildCount(p_cont);
			if (p_cont && (cnt > 1)) {
				f_item = tree->GetNthChild(p_cont, 0);
				if (f_item != a_item) {
					first = false;
				}
				l_item = tree->GetNthChild(p_cont, cnt-1);
				if (l_item != a_item) {
					last = false;
				}
			}
			tw->ShowRMenu(cont, vm->GetChildCount(a_cont), first, last);
		} else if (evid == wxEVT_DATAVIEW_ITEM_BEGIN_DRAG) {
			printf("DRAG\n");
		}
		printf("Right Click\n");
	}
}

void DTSTreeWindowEvent::MenuEvent(wxCommandEvent &event) {
	enum treewinmenu eid;
	wxDataViewItem p_cont;
	DTSFrame *frame;

	p_cont = (a_cont == a_item) ? vm->GetParent(a_cont) : a_cont;

	eid=(treewinmenu)event.GetId();
	switch(eid) {
		case DTS_TREEWIN_MENU_MOVEDOWN:
			MoveDown(p_cont);
			break;
		case DTS_TREEWIN_MENU_MOVEUP:
			MoveUp(p_cont);
			break;
		case DTS_TREEWIN_MENU_DELETE:
			frame = parent->GetFrame();
			if (frame->Confirm("Are you sure you want to delete this item")) {
				tree->DeleteItem(a_item);
			}
			break;
		case DTS_TREEWIN_MENU_SORT:
			Sort(p_cont);
			break;
	}
}

void DTSTreeWindowEvent::MoveDown(wxDataViewItem p_cont) {
	wxDataViewItemArray items;
	int cnt,i;

	cnt = vm->GetChildren(p_cont, items);
	if (a_item == items[cnt-1]) {
		return;
	}
	for(i=0; i < cnt;i++) {
		if (items[i] == a_item) {
			break;
		}
	}
	i+=2;
	if (i > cnt) {
		return;
	}
	if (i == cnt) {
		if (a_cont == a_item) {
			tree->AppendContainer(p_cont, vm->GetItemText(a_item), -1, tree->IsExpanded(a_item), vm->GetItemData(a_item));
		} else {
			tree->AppendItem(p_cont, vm->GetItemText(a_item), -1, vm->GetItemData(a_item));
		}
	} else {
		if (a_cont == a_item) {
			tree->InsertContainer(p_cont, items[i], vm->GetItemText(a_item), -1, tree->IsExpanded(a_item), vm->GetItemData(a_item));
		} else {
			tree->InsertItem(p_cont, items[i], vm->GetItemText(a_item), -1, vm->GetItemData(a_item));
		}
	}
	tree->DeleteItem(a_item);
}

void DTSTreeWindowEvent::Float(wxDataViewItemArray items, wxDataViewItem p_cont, wxDataViewItem f_item, int i) {
	bool iscontainer;

	iscontainer = vm->IsContainer(f_item);
	if (i == 0) {
		if (iscontainer) {
			tree->PrependContainer(p_cont, vm->GetItemText(f_item), -1, tree->IsExpanded(f_item), vm->GetItemData(f_item));
		} else {
			tree->PrependItem(p_cont, vm->GetItemText(f_item), -1, vm->GetItemData(f_item));
		}
	} else {
		if (iscontainer) {
			tree->InsertContainer(p_cont, items[i], vm->GetItemText(f_item), -1, tree->IsExpanded(f_item), vm->GetItemData(f_item));
		} else {
			tree->InsertItem(p_cont, items[i], vm->GetItemText(f_item), -1, vm->GetItemData(f_item));
		}
	}
	tree->DeleteItem(f_item);
}

void DTSTreeWindowEvent::MoveUp(wxDataViewItem p_cont) {
	wxDataViewItemArray items;
	int cnt,i;

	cnt = vm->GetChildren(p_cont, items);
	if (a_item == items[0]) {
		return;
	}
	for(i=0; i < cnt;i++) {
		if (items[i] == a_item) {
			break;
		}
	}
	i--;
	if (i < 0) {
		return;
	}
	Float(items, p_cont, a_item, i);
}

void DTSTreeWindowEvent::Sort(wxDataViewItem p_cont) {
	wxDataViewItemArray items;
	wxDataViewItem tmp;
	wxString tocmp[2];
	int cnt,i,j;

	cnt = vm->GetChildren(p_cont, items);

	/*Sort Items*/
	do {
		j = 0;
		for(i=0; i < cnt-1;i++) {
			if (vm->IsContainer(items[i])) {
				continue;
			}
			tocmp[0] = vm->GetItemText(items[i]);
			tocmp[1] = vm->GetItemText(items[i+1]);
			if (tocmp[0].Cmp(tocmp[1]) > 0) {
				j = 1;
				Float(items, p_cont, items[i+1], i);
				items[i] = tree->GetNthChild(p_cont, i);
				items[i+1] = tree->GetNthChild(p_cont, i+1);
			}
		}
	} while (j);

	/*Sort Containers*/
	do {
		j = 0;
		for(i=0; i < cnt-1;i++) {
			if (!vm->IsContainer(items[i])) {
				continue;
			}
			tocmp[0] = vm->GetItemText(items[i]);
			tocmp[1] = vm->GetItemText(items[i+1]);
			if (tocmp[0].Cmp(tocmp[1]) > 0) {
				j = 1;
				Float(items, p_cont, items[i+1], i);
				items[i] = tree->GetNthChild(p_cont, i);
				items[i+1] = tree->GetNthChild(p_cont, i+1);
			}
		}
	} while (j);
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
	wxBoxSizer *treesizer = new wxBoxSizer(wxVERTICAL);
	wxDataViewItem root;

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
	t_pane = new wxPanel(sw, wxID_ANY);
	c_pane = new wxWindow(sw, wxID_ANY);

	panel = static_cast<wxWindow *>(sw);
	type = wx_DTSPANEL_TREE;

	SplitVertically(t_pane, c_pane);
	SetSashGravity(0.5);
	SetMinimumPaneSize(20);

	t_pane->SetSizer(treesizer);
	tree = new wxDataViewTreeCtrl(t_pane, wxID_ANY);
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

	tree->GetParent();

	root = tree->AppendContainer(wxDataViewItem(0), "The Root");

	tree->AppendContainer(root, "Root2");
	tree->AppendContainer(root, "Root3");
	tree->AppendContainer(root, "Root4");
	tree->AppendItem(root, "Child E");
	tree->AppendItem(root, "Child D");
	tree->AppendItem(root, "Child C");
	tree->AppendItem(root, "Child B");
	tree->AppendItem(root, "Child A");

	tree->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &DTSTreeWindowEvent::TreeEvent, dtsevthandler);
	tree->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &DTSTreeWindowEvent::TreeEvent, dtsevthandler);
	frame->Bind(wxEVT_DATAVIEW_ITEM_BEGIN_DRAG, &DTSTreeWindowEvent::TreeEvent, dtsevthandler);
	tree->Bind(wxEVT_COMMAND_MENU_SELECTED, &DTSTreeWindowEvent::MenuEvent, dtsevthandler);

	tree->Expand(root);
	tree->Select(root);

	tree->EnableDragSource(wxDF_UNICODETEXT);
	tree->EnableDropTarget(wxDF_UNICODETEXT);

	Show(false);
}

wxDataViewTreeCtrl *DTSTreeWindow::GetTreeCtrl() {
	return tree;
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
	if (show && frame) {
		frame->SetStatusText(status);
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
