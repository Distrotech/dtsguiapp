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

#ifndef DTSLISTVIEW_HPP_INCLUDED
#define DTSLISTVIEW_HPP_INCLUDED

class DTSDVMListStore {
	public:
		DTSDVMListStore(DTSDVMListStore* parent, bool is_container, const wxString &title, int nodeid = -1, dtsgui_treeviewpanel_cb p_cb = NULL, void *userdata = NULL);
		~DTSDVMListStore();
		bool IsContainer() const;
		DTSDVMListStore* GetParent();
		size_t GetChildCount();
		void Append(DTSDVMListStore* child);
		void Insert(DTSDVMListStore* child, size_t idx);
		DTSDVMListStore* GetNthChild(size_t idx);
		size_t GetChildIndex(DTSDVMListStore* child);
		void RemoveChildReference(DTSDVMListStore* child);
		const wxString GetTitle();
		void SetTitle(const wxString& value);
		void SortChildren();
		void SetExpanded(bool expanded = true);
		unsigned int GetChildren(wxDataViewItemArray& items);
		unsigned int GetContainers(wxDataViewItemArray& items, bool exonly = false);
		bool IsExpanded();
		bool MoveChildUp(size_t idx);
		bool MoveChildDown(size_t idx);
		void *GetUserData();
		bool can_edit;
		bool can_sort;
		bool can_delete;
		int GetNodeID();
		void SetXMLData(struct xml_node *xnode, const char *tattr);
		struct xml_node *GetXMLData(char **buff);
		void ConfigPanel(dtsgui_pane p, dtsgui_treeview tw);
	private:
		std::vector<DTSDVMListStore*> children;
		dtsgui_treeviewpanel_cb p_cb;
		DTSDVMListStore *parent;
		struct xml_node *xml;
		const char *tattr;
		bool is_container;
		wxString title;
		bool expanded;
		void *data;
		int nodeid;
};

class DTSDVMListView :public wxDataViewModel {
	public:
		DTSDVMListView(int cols=1, bool cont_cols = true);
		~DTSDVMListView();
		virtual bool IsContainer( const wxDataViewItem& item) const;
		virtual wxDataViewItem GetParent(const wxDataViewItem &item) const;
		virtual unsigned int GetChildren(const wxDataViewItem &parent, wxDataViewItemArray &items) const;
		virtual unsigned int GetColumnCount() const;
		virtual bool HasContainerColumns(const wxDataViewItem& item) const;

		virtual wxString GetColumnType(unsigned int col) const;
		virtual void GetValue(wxVariant &variant, const wxDataViewItem &item, unsigned int col) const;
		virtual bool SetValue(const wxVariant &variant, const wxDataViewItem &item, unsigned int col);

		DTSDVMListStore* GetRoot();
		DTSDVMListStore* SetRoot(const wxString& title, int nodeid = -1, dtsgui_treeviewpanel_cb p_cb = NULL, void *userdata = NULL);
		void Delete(const wxDataViewItem& item);
		void DeleteAll(void);
		void SortChildren(const wxDataViewItem& parent);
		int GetChildCount(wxDataViewItem& parent);
		wxDataViewItem GetNthChild(wxDataViewItem& parent, size_t idx);
		void SetExpanded(const wxDataViewItem& node, bool expanded = true);
		virtual unsigned int GetContainers(const wxDataViewItem &parent, wxDataViewItemArray &items, bool exonly = false) const;
		void MoveChildUp(const wxDataViewItem& node);
		void MoveChildDown(const wxDataViewItem& node);
		void *GetUserData(const wxDataViewItem& node);
		int GetNodeID(const wxDataViewItem& node);
		void SetXMLData(const wxDataViewItem& node, struct xml_node *xnode, const char *tattr);
		struct xml_node *GetXMLData(const wxDataViewItem& node, char **buff);
	protected:
		DTSDVMListStore* root;
		bool hascontcol;
		int colcnt;
};

class DTSDVMCtrl :public wxDataViewCtrl {
	public:
		DTSDVMCtrl();
		DTSDVMCtrl(wxWindow *parent, wxWindowID id = wxID_ANY, DTSDVMListView *model = NULL, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize,
						long style=wxDV_ROW_LINES, const wxValidator &validator=wxDefaultValidator);
		~DTSDVMCtrl();
		virtual bool AssociateModel(DTSDVMListView *model);
		void Sort(const wxDataViewItem& parent);
		DTSDVMListView *GetStore();
		wxDataViewItem AppendItem(wxDataViewItem parent, const wxString& title, bool can_edit = true, bool can_sort = true, bool can_del = false, int nodeid = -1, dtsgui_treeviewpanel_cb p_cb = NULL, void *userdata = NULL);
		wxDataViewItem AppendContainer(wxDataViewItem parent, const wxString& title, bool can_edit = true, bool can_sort = true, bool can_del = false, int nodeid = -1, dtsgui_treeviewpanel_cb p_cb = NULL, void *userdata = NULL);
	private:
		wxDataViewItem AppendNode(wxDataViewItem parent, const wxString& title, bool iscont, bool can_edit = true, bool can_sort = true, bool can_del = false, int nodeid = -1, dtsgui_treeviewpanel_cb p_cb = NULL, void *userdata = NULL);
		DTSDVMListView *model;
};

#endif // DTSLISTVIEW_HPP_INCLUDED
