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

#include <stdint.h>
#include <dtsapp.h>
#include "dtsgui.hpp"

#include "DTSPanel.h"
#include "DTSTreeWindow.h"

DTSTreeWindow::DTSTreeWindow(wxWindow *parent, wxFrame *frame, wxString stat_msg, int pos)
	:wxSplitterWindow(parent, -1, wxDefaultPosition, wxDefaultSize),
	 DTSObject(stat_msg) {

	int w, h, p;

	wxSplitterWindow *sw = static_cast<wxSplitterWindow*>(this);
	wxBoxSizer *p_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *treesizer = new wxBoxSizer(wxVERTICAL);

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
//	tree->AppendItem(NULL, wxT("Test Root"));

	tree->AppendContainer(wxDataViewItem(0), "The Root", 0 );

	Show(false);
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

DTSTreeWindow::~DTSTreeWindow() {
	delete t_pane;
	delete c_pane;
}

bool DTSTreeWindow::Show(bool show) {
	if (show && frame) {
		frame->SetStatusText(status);
	}
	return wxSplitterWindow::Show(show);
}

DTSTabWindow::DTSTabWindow(wxFrame *frame, wxString stat_msg)
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
