#if !defined(AFX_GRIDCELLCOMBO_H__ECD42822_16DF_11D1_992F_895E185F9C72__INCLUDED_)
#define AFX_GRIDCELLCOMBO_H__ECD42822_16DF_11D1_992F_895E185F9C72__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "../GridCtrl_src/GridCell.h"
#include "../NumSpinCtrl.h"
#include "../NumEdit.h"

#define IDC_CELL_SPIN 1011
#define IDC_CELL_EDIT 1010
class CGridCellSpinEdit;
class CGridCellEdit : public CNumEdit
{
public:
	CGridCellEdit();
	~CGridCellEdit();
	CGridCellSpinEdit* m_pGridCellSpinEdit;
	CWnd* m_pParentDlg;
	int m_nRow;
	int m_nCol;
	int m_nVal;
	int m_nPrevVal;
	int m_nMax;
	int m_nMin;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnEnChange();
	afx_msg void OnEnKillfocus();
};
class CGridCellSpin : public CNumSpinCtrl
{
public:
	CGridCellSpin();
	~CGridCellSpin();
};
class CGridCellSpinEdit : public CGridCell
{
    friend class CGridCtrl;
    DECLARE_DYNCREATE(CGridCellSpinEdit)

public:
	CGridCellSpinEdit();
	~CGridCellSpinEdit();
// editing cells
public:
    virtual BOOL  Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
    virtual CWnd* GetEditWnd() const;
    virtual void  EndEdit();

// Operations
public:
	virtual CSize GetCellExtent(CDC* pDC);

public:
	CGridCellSpin* m_pSpin;
	CGridCellEdit* m_pEdit;
	TCHAR m_str[10];
 	void  SetText(int val);

protected:
    virtual BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);

    
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};
#endif // !defined(AFX_GRIDCELLCOMBO_H__ECD42822_16DF_11D1_992F_895E185F9C72__INCLUDED_)
