#pragma once


#include "GridCtrl_src/GridCtrl.h"
#include "NumSpinCtrl.h"
#include "NumEdit.h"

struct CheckedOneItem
{
	int row;
	int col;
};
class CheckedItems
{
public:
	CheckedItems() {};
	~CheckedItems() {};
	CArray<CheckedOneItem, CheckedOneItem> m_arrCheckedItems;
	void addItem(int row, int col)
	{
		CheckedOneItem newItem;
		newItem.row = row;
		newItem.col = col;
		int idx = findItem(row, col);
		if(idx == -1)
			m_arrCheckedItems.Add(newItem);
	}
	void removeItem(int row, int col)
	{
		int idx = findItem(row, col);
		if(idx != -1)
			m_arrCheckedItems.RemoveAt(idx);
	}
	int findItem(int row, int col)
	{
		int idx = 0;
		for (int i = 0; i < m_arrCheckedItems.GetCount(); i++)
		{
			CheckedOneItem& item = m_arrCheckedItems.ElementAt(i);
			if (item.row == row && item.col == col)
			{
				idx = i;
				return idx;
			}
		}
		return -1;
	}
};

class COneConditionDlg : public CDialog
{
	DECLARE_DYNAMIC(COneConditionDlg)

public:
	COneConditionDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~COneConditionDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_ONE_CONDITION };
	CWnd* m_pParentWnd;
	CGridCtrl* m_pGrid;
	int m_nId;
	CheckedItems m_checkedItems;
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnGridClick(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void setDefaultGrid();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void CreateTable(int nRow, int nCol);
	CButton m_ctrlDel;
	CStatic m_ctrlState;
	CStatic m_ctrlStaticMin;
	CStatic m_ctrlStaticMax;
	int m_nMin;
	int m_nMax;
	CNumEdit m_ctrlMin;
	CNumEdit m_ctrlMax;
	CNumSpinCtrl m_ctrlMinSpin;
	CNumSpinCtrl m_ctrlMaxSpin;
	afx_msg void OnDeltaposSpinMinCond(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnChangeEditMinCond();
	afx_msg void OnEnChangeEditMaxCond();
	void AddColumn(int nRow, int nCol);
	void AddRow(int nRow, int nCol);
};
