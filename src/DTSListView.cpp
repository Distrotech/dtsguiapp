#include <vector>
#include <algorithm>
#include <stdlib.h>

#include <wx/dataview.h>
#include <wx/validate.h>

#include "DTSListView.h"

bool cmp_title(DTSDVMListStore *c1,DTSDVMListStore *c2) {
	wxString s1, s2;

	s1 = c1->GetTitle();
	s2 = c2->GetTitle();

	return (s1 < s2);
}

DTSDVMListStore::DTSDVMListStore(DTSDVMListStore* parent, bool is_container, const wxString& title, void *userdata) {
	this->parent = parent;
	this->title = title;
	this->is_container = is_container;
	this->data = userdata;
}

DTSDVMListStore::~DTSDVMListStore() {
	while (!children.empty()) {
		DTSDVMListStore* f_child = children.back();
		delete f_child;
		children.pop_back();
	}
}

bool DTSDVMListStore::IsContainer() const {
	return is_container;
}

DTSDVMListStore* DTSDVMListStore::GetParent() {
	return parent;
}

size_t DTSDVMListStore::GetChildCount() {
	return children.size();
}

void DTSDVMListStore::Append(DTSDVMListStore* child) {
	children.push_back(child);
}

void DTSDVMListStore::Insert(DTSDVMListStore* child, size_t idx) {
	std::vector<DTSDVMListStore*>::iterator iter = children.begin();

	if (idx >= children.size()) {
		return;
	}

	children.insert(iter+idx, child);
}

DTSDVMListStore* DTSDVMListStore::GetNthChild(size_t idx) {
	if (idx >= children.size()) {
		return NULL;
	}

	return children[idx];
}

size_t DTSDVMListStore::GetChildIndex(DTSDVMListStore* child) {
	size_t idx = 0, c_cnt;

	c_cnt = children.size();
	while ((idx < c_cnt) && (children[idx] != child)) {
		idx++;
	}

	return (children[idx] = child) ? idx : -1;
}

void DTSDVMListStore::RemoveChildReference(DTSDVMListStore* child) {
	std::vector<DTSDVMListStore*>::iterator iter = children.begin();
	size_t idx;

	if ((idx = GetChildIndex(child)) >= 0) {
		children.erase(iter+idx);
	}
}

const wxString DTSDVMListStore::GetTitle() {
	return (const wxString)title;
}

void DTSDVMListStore::SetTitle(const wxString& value) {
	title = value;
}

void DTSDVMListStore::SortChildren() {
	std::sort(children.begin(), children.end(), cmp_title);
}

void DTSDVMListStore::SetExpanded(bool expanded) {
	this->expanded = expanded;
}

unsigned int DTSDVMListStore::GetChildren(wxDataViewItemArray& items) {
	unsigned int cnt;
	size_t acnt;

	acnt = children.size();
	for(cnt=0; cnt < acnt;cnt++) {
		items.Add(wxDataViewItem((void*)children[cnt]));
	}
	return cnt;
}

unsigned int DTSDVMListStore::GetContainers(wxDataViewItemArray& items, bool exonly) {
	unsigned int cnt;
	size_t acnt, ccnt = 0;
	wxString s;

	acnt = children.size();
	for(cnt=0; cnt < acnt;cnt++) {
		if (children[cnt]->is_container && (!exonly || (exonly && children[cnt]->expanded))) {
			items.Add(wxDataViewItem((void*)children[cnt]));
			ccnt++;
		}
	}
	return ccnt;
}

bool DTSDVMListStore::IsExpanded() {
	return expanded;
}

bool DTSDVMListStore::MoveChildUp(size_t idx) {
	DTSDVMListStore *tmp;

	if ((idx <= 0) || (idx > children.size())) {
		return false;
	}
	tmp = children[idx-1];
	children[idx-1]=children[idx];
	children[idx]=tmp;
	return true;
}

bool DTSDVMListStore::MoveChildDown(size_t idx) {
	DTSDVMListStore *tmp;

	if ((idx < 0) || (idx > children.size()-1)) {
		return false;
	}
	tmp = children[idx];
	children[idx]=children[idx+1];
	children[idx+1]=tmp;
	return true;
}

void *DTSDVMListStore::GetUserData() {
	return data;
}

DTSDVMListView::DTSDVMListView(int cols, bool cont_cols) {
	hascontcol = cont_cols;
	root = NULL;
	colcnt = cols;
}

DTSDVMListView::~DTSDVMListView() {
	DeleteAll();
}

bool DTSDVMListView::IsContainer(const wxDataViewItem& item) const {
	if (!item.IsOk()) {
		return true;
	}

	DTSDVMListStore *node = (DTSDVMListStore*)item.GetID();
	return node->IsContainer();
}

wxDataViewItem DTSDVMListView::GetParent(const wxDataViewItem& item) const {
	DTSDVMListStore *node;

	if (!item.IsOk()) {
		return wxDataViewItem(NULL);
	}

	node = (DTSDVMListStore*)item.GetID();
	if (!node || (node == root)){
		return wxDataViewItem(NULL);
	}

	return wxDataViewItem((void*)node->GetParent());
}

unsigned int DTSDVMListView::GetChildren(const wxDataViewItem& parent, wxDataViewItemArray& items) const {
	DTSDVMListStore *node;
	bool nok;

	nok = parent.IsOk();
	node = (DTSDVMListStore*)parent.GetID();

	if (!nok && !node) {
		if (root) {
			items.Add(wxDataViewItem((void*)root));
			return 1;
		} else {
			return 0;
		}
	} else {
		return node->GetChildren(items);
	}
}

unsigned int DTSDVMListView::GetColumnCount() const {
	return colcnt;
}

bool DTSDVMListView::HasContainerColumns(const wxDataViewItem& item) const {
	return hascontcol;
}

wxString DTSDVMListView::GetColumnType(unsigned int col) const {
	return wxT("string");
}

void DTSDVMListView::GetValue(wxVariant &variant, const wxDataViewItem &item, unsigned int col) const {
	DTSDVMListStore *node = (DTSDVMListStore*)item.GetID();

	if (col == 0) {
		variant = node->GetTitle();
	}
}

bool DTSDVMListView::SetValue(const wxVariant &variant, const wxDataViewItem &item, unsigned int col) {
	assert(item.IsOk());
	DTSDVMListStore *node = (DTSDVMListStore*) item.GetID();

	if (col == 0) {
		node->SetTitle(variant.GetString());
		return true;
	}

	return false;
}

DTSDVMListStore* DTSDVMListView::GetRoot() {
	return root;
}

DTSDVMListStore* DTSDVMListView::SetRoot(const wxString& title, void *userdata) {
	root = new DTSDVMListStore(NULL, true, title, userdata);
	return root;
}

void DTSDVMListView::Delete(const wxDataViewItem& item) {
	DTSDVMListStore *node;
	wxDataViewItem parent;

	node = (DTSDVMListStore*)item.GetID();
	if (!node) {
		return;
	}

	parent = wxDataViewItem(node->GetParent());
	if (!parent.IsOk()) {
		DeleteAll();
		return;
	}

	node->GetParent()->RemoveChildReference(node);
	delete node;
	ItemDeleted(parent, item);
}

void DTSDVMListView::DeleteAll(void) {
	if (!root) {
		return;
	}

	delete root;
	root = NULL;
	Cleared();
}

void DTSDVMListView::SortChildren(const wxDataViewItem& parent) {
	DTSDVMListStore *node = (DTSDVMListStore*)parent.GetID();
	wxDataViewItemArray items;

	if (!node) {
		return;
	}

	node->SortChildren();
	GetChildren(parent, items);
	ItemsDeleted(parent, items);
	ItemsAdded(parent, items);
}

int DTSDVMListView::GetChildCount(wxDataViewItem& parent) {
	DTSDVMListStore *entry;
	bool nok;

	nok = parent.IsOk();
	if (nok && (entry = (DTSDVMListStore*)parent.GetID())) {
		return entry->GetChildCount();
	} else if (root && !nok) {
		return root->GetChildCount();
	} else {
		return 0;
	}
}

wxDataViewItem DTSDVMListView::GetNthChild(wxDataViewItem& parent, size_t idx) {
	DTSDVMListStore *entry, *li;
	bool nok;

	nok = parent.IsOk();
	entry = (DTSDVMListStore*)parent.GetID();

	if (nok && entry) {
		li = entry->GetNthChild(idx);
	} else if (!nok && root) {
		li = root->GetNthChild(idx);
	} else {
		li = NULL;
	}
	return wxDataViewItem(li);
}

void DTSDVMListView::SetExpanded(const wxDataViewItem& node, bool expanded) {
	DTSDVMListStore *entry;
	bool nok;

	nok = node.IsOk();
	entry = (DTSDVMListStore*)node.GetID();

	if (nok && entry) {
		entry->SetExpanded(expanded);
	} else if (!nok && root) {
		root->SetExpanded();
	}
}

unsigned int DTSDVMListView::GetContainers(const wxDataViewItem& parent, wxDataViewItemArray& items, bool exonly) const {
	DTSDVMListStore *node;
	bool nok;

	nok = parent.IsOk();
	node = (DTSDVMListStore*)parent.GetID();

	if (!nok && !node) {
		if (root) {
			items.Add(wxDataViewItem((void*)root));
			return 1;
		} else {
			return 0;
		}
	} else {
		return node->GetContainers(items, exonly);
	}
}

void DTSDVMListView::MoveChildUp(const wxDataViewItem& node) {
	DTSDVMListStore *data, *parent;
	wxDataViewItem pnode;
	wxDataViewItemArray items;

	data = (DTSDVMListStore*)node.GetID();
	if (!data || !node.IsOk()) {
		return;
	}
	parent = data->GetParent();
	pnode = wxDataViewItem(parent);

	if (parent) {
		parent->MoveChildUp(parent->GetChildIndex(data));
		GetChildren(pnode, items);
		ItemsDeleted(pnode, items);
		ItemsAdded(pnode, items);
	}
}

void DTSDVMListView::MoveChildDown(const wxDataViewItem& node) {
	DTSDVMListStore *data, *parent;
	wxDataViewItem pnode;
	wxDataViewItemArray items;

	data = (DTSDVMListStore*)node.GetID();
	if (!data || !node.IsOk()) {
		return;
	}
	parent = data->GetParent();
	pnode = wxDataViewItem(parent);

	if (parent) {
		parent->MoveChildDown(parent->GetChildIndex(data));
		GetChildren(pnode, items);
		ItemsDeleted(pnode, items);
		ItemsAdded(pnode, items);
	}
}

void *DTSDVMListView::GetUserData(const wxDataViewItem& node) {
	DTSDVMListStore *data;

	if (!node.IsOk() && root) {
		return root->GetUserData();
	} else if ((data = (DTSDVMListStore*)node.GetID())) {
		return data->GetUserData();
	} else {
		return NULL;
	}
}

DTSDVMCtrl::DTSDVMCtrl() {
	model = NULL;
}

DTSDVMCtrl::DTSDVMCtrl(wxWindow *parent, wxWindowID id, DTSDVMListView *model, const wxPoint &pos, const wxSize &size, long style, const wxValidator &validator) {
	wxDataViewTextRenderer *col0_r;
	wxDataViewColumn *col0;

	this->model = NULL;
	Create(parent, id, pos, size, style, validator);
	if (model) {
		AssociateModel(model);
	}

	col0_r = new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_EDITABLE);
#ifndef _WIN32
	col0_r->DisableEllipsize();
#endif //_WIN32
	col0 = new wxDataViewColumn("Title", col0_r, 0);
#ifndef _WIN32
	col0->SetWidth(wxCOL_WIDTH_AUTOSIZE);
#endif // _WIN32
	col0->SetFlags(wxCOL_RESIZABLE | wxCOL_REORDERABLE);
	col0->SetReorderable(true);
	col0->SetAlignment(wxALIGN_LEFT);
	AppendColumn(col0);
#ifdef _WIN32
	SetRowHeight(17);
#endif // _WIN32
}

DTSDVMCtrl::~DTSDVMCtrl() {
	model->DeleteAll();
}

bool DTSDVMCtrl::AssociateModel(DTSDVMListView *model) {
	bool res;

	res = wxDataViewCtrl::AssociateModel(model);
	if (res) {
		if (this->model) {
			model->DeleteAll();
		}
		this->model = model;
		model->DecRef();
	}
	return res;
}

void DTSDVMCtrl::Sort(const wxDataViewItem& parent) {
	model->SortChildren(parent);
}

DTSDVMListView *DTSDVMCtrl::GetStore() {
	return model;
}

wxDataViewItem DTSDVMCtrl::AppendItem(wxDataViewItem parent, const wxString& title, void *userdata) {
	return AppendNode(parent, false, title, userdata);
}

wxDataViewItem DTSDVMCtrl::AppendContainer(wxDataViewItem parent, const wxString& title, void *userdata) {
	return AppendNode(parent, true, title, userdata);
}

wxDataViewItem DTSDVMCtrl::AppendNode(wxDataViewItem parent, bool iscont, const wxString& title, void *userdata) {
	DTSDVMListStore *li, *node;
	wxDataViewItem dvi;


	if (!parent.IsOk() && !model->GetRoot()) {
		li = model->SetRoot(title, userdata);
	} else if (!(node = (DTSDVMListStore*)parent.GetID())) {
		return wxDataViewItem(NULL);
	} else {
		li= new DTSDVMListStore(node, iscont, title, userdata);
		node->Append(li);
	}

	dvi = wxDataViewItem(li);
	model->ItemAdded(parent, dvi);
	return dvi;
}
