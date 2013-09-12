#ifndef DTSLISTVIEW_HPP_INCLUDED
#define DTSLISTVIEW_HPP_INCLUDED

class DTSDVMListStore {
	public:
		DTSDVMListStore(DTSDVMListStore* parent, bool is_container, const wxString &title);
		~DTSDVMListStore();
		bool IsContainer() const;
		DTSDVMListStore* GetParent();
		int GetChildCount();
		void Append(DTSDVMListStore* child);
		void Insert(DTSDVMListStore* child, size_t idx);
		DTSDVMListStore* GetNthChild(size_t idx);
		int GetChildIndex(DTSDVMListStore* child);
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
	protected:
		wxString title;
	private:
		DTSDVMListStore *parent;
		std::vector<DTSDVMListStore*> children;
		bool is_container;
		bool expanded;
};

class DTSDVMListView :public wxDataViewModel {
	public:
		DTSDVMListView();
		~DTSDVMListView();
		virtual bool IsContainer( const wxDataViewItem& item) const;
		virtual wxDataViewItem GetParent(const wxDataViewItem &item) const;
		virtual unsigned int GetChildren(const wxDataViewItem &parent, wxDataViewItemArray &items) const;

		virtual unsigned int GetColumnCount() const;
		virtual wxString GetColumnType(unsigned int col) const;
		virtual void GetValue(wxVariant &variant, const wxDataViewItem &item, unsigned int col) const;
		virtual bool SetValue(const wxVariant &variant, const wxDataViewItem &item, unsigned int col);
		virtual bool HasContainerColumns(const wxDataViewItem& item) const;

		DTSDVMListStore* GetRoot();
		DTSDVMListStore* SetRoot(const wxString& title);
		void Delete(const wxDataViewItem& item);
		void DeleteAll(void);
		void SortChildren(const wxDataViewItem& parent);
		int GetChildCount(wxDataViewItem& parent);
		wxDataViewItem GetNthChild(wxDataViewItem& parent, size_t idx);
		void SetExpanded(const wxDataViewItem& node, bool expanded = true);
		virtual unsigned int GetContainers(const wxDataViewItem &parent, wxDataViewItemArray &items, bool exonly = false) const;
		void MoveChildUp(const wxDataViewItem& node);
		void MoveChildDown(const wxDataViewItem& node);
	protected:
		DTSDVMListStore* root;
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
		wxDataViewItem AppendItem(wxDataViewItem parent, const wxString& title);
		wxDataViewItem AppendContainer(wxDataViewItem parent, const wxString& title);
		void ReloadParent(const wxDataViewItem parent, bool do_expand, const wxDataViewItemArray cont, const wxDataViewItemArray sel);
	private:
		DTSDVMListView *model;
};

#endif // DTSLISTVIEW_HPP_INCLUDED
