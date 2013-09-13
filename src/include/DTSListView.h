#ifndef DTSLISTVIEW_HPP_INCLUDED
#define DTSLISTVIEW_HPP_INCLUDED

class DTSDVMListStore {
	public:
		DTSDVMListStore(DTSDVMListStore* parent, bool is_container, const wxString &title, void *userdata = NULL);
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
	protected:
		wxString title;
	private:
		std::vector<DTSDVMListStore*> children;
		void *data;
		DTSDVMListStore *parent;
		bool is_container;
		bool expanded;
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
		DTSDVMListStore* SetRoot(const wxString& title, void *userdata = NULL);
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
		wxDataViewItem AppendItem(wxDataViewItem parent, const wxString& title, void *userdata = NULL);
		wxDataViewItem AppendContainer(wxDataViewItem parent, const wxString& title, void *userdata = NULL);
	private:
		wxDataViewItem AppendNode(wxDataViewItem parent, bool iscont, const wxString& title, void *userdata);
		DTSDVMListView *model;
};

#endif // DTSLISTVIEW_HPP_INCLUDED
