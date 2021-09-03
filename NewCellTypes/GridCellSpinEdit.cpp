
#include "../framework.h"
#include "../GridCtrl_src/GridCell.h"
#include "../GridCtrl_src/GridCtrl.h"
#include "../NumEdit.h"
#include "../GridCtrl_src/InPlaceEdit.h"
#include "GridCellSpinEdit.h"

#define WM_POINT_VAL_CHANGED WM_USER+5
#define WM_POINT_VAL_CHANGE_END WM_USER+6

IMPLEMENT_DYNCREATE(CGridCellSpinEdit, CGridCell)

int CStringToInt(CString str);

CGridCellSpin::CGridCellSpin() : CNumSpinCtrl()
{

}
CGridCellSpin::~CGridCellSpin()
{

}


CGridCellEdit::CGridCellEdit() : CNumEdit()
{
	m_nRow = 0;
	m_nCol = 0;
	m_nVal = 0;
	m_nPrevVal = 0;
	m_nMin = 0;
	m_nMax = 1000;
	m_pParentDlg = NULL;
	m_pGridCellSpinEdit = NULL;
}
CGridCellEdit::~CGridCellEdit()
{

}
BEGIN_MESSAGE_MAP(CGridCellEdit, CNumEdit)
	ON_CONTROL_REFLECT(EN_CHANGE, &CGridCellEdit::OnEnChange)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, &CGridCellEdit::OnEnKillfocus)
END_MESSAGE_MAP()


void CGridCellEdit::OnEnChange()
{
	TCHAR str[10];
	GetWindowText(str, 10);
	int val = CStringToInt(str);
	UINT row_col = m_nRow;
	row_col <<= 16;
	row_col |= m_nCol;
	if (val > m_nMax)
	{
		val = m_nMax;
		_stprintf(str, _T("%d"), val);
		SetWindowText(str);
		return;
	}
	else if (val < m_nMin)
	{
		val = m_nMin;
		_stprintf(str, _T("%d"), val);
		SetWindowText(str);
		return;
	}
	m_nVal = val;
	if (m_nVal == m_nPrevVal)
		return;
	m_nPrevVal = m_nVal;
	_tcscpy_s(m_pGridCellSpinEdit->m_str, 10, str);
	if(m_pParentDlg)
		m_pParentDlg->SendMessage(WM_POINT_VAL_CHANGED, row_col, val);
}

void CGridCellEdit::OnEnKillfocus()
{
	TCHAR str[10];
	GetWindowText(str, 10);
	int val = CStringToInt(str);
	UINT row_col = m_nRow;
	row_col <<= 16;
	row_col |= m_nCol;
	if (val > m_nMax)
	{
		val = m_nMax;
		_stprintf(str, _T("%d"), val);
		SetWindowText(str);
		return;
	}
	else if (val < m_nMin)
	{
		val = m_nMin;
		_stprintf(str, _T("%d"), val);
		SetWindowText(str);
		return;
	}
	m_nVal = val;
// 	if (m_nVal == m_nPrevVal)
// 		return;
// 	m_nPrevVal = m_nVal;
	_tcscpy_s(m_pGridCellSpinEdit->m_str, 10, str);
	if (m_pParentDlg)
		m_pParentDlg->SendMessage(WM_POINT_VAL_CHANGE_END, row_col, val);
}

CGridCellSpinEdit::CGridCellSpinEdit() : CGridCell()
{
	m_pSpin = new CGridCellSpin;
	m_pEdit = new CGridCellEdit;
	m_pEdit->m_pGridCellSpinEdit = this;
}

CGridCellSpinEdit::~CGridCellSpinEdit()
{
	delete m_pSpin;
	delete m_pEdit;
}

// Create a control to do the editing
BOOL CGridCellSpinEdit::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, UINT nID, UINT nChar)
{
    return TRUE;
}

CWnd* CGridCellSpinEdit::GetEditWnd() const
{
	if (m_pEditWnd)
		return (CInPlaceEdit*)m_pEditWnd;

	return NULL;
}


CSize CGridCellSpinEdit::GetCellExtent(CDC* pDC)
{    
    CSize sizeScroll (GetSystemMetrics(SM_CXVSCROLL), GetSystemMetrics(SM_CYHSCROLL));    
    CSize sizeCell (CGridCell::GetCellExtent(pDC));    
    sizeCell.cx += sizeScroll.cx;    
    sizeCell.cy = max(sizeCell.cy,sizeScroll.cy);    
    return sizeCell;
}

// Cancel the editing.
void CGridCellSpinEdit::EndEdit()
{
	if (m_pEditWnd)
		((CInPlaceEdit*)m_pEditWnd)->EndEdit();
}

// Override draw so that when the cell is selected, a drop arrow is shown in the RHS.
BOOL CGridCellSpinEdit::Draw(CDC* pDC, int nRow, int nCol, CRect rect,  BOOL bEraseBkgnd /*=TRUE*/)
{
	CRect rect1;
	if (nRow == 0 && nCol == 0)
		int kk = 0;
	rect1.top = rect.top + 1; rect1.bottom = rect.bottom - 1; rect1.left = rect.left + rect.Width()/3; rect1.right = rect1.left + 20;
	if (m_pSpin && m_pSpin->m_hWnd)
	{
		if (rect1.left < 10)
			int kk = 0;
		m_pSpin->MoveWindow(rect1);
	}
	rect1.left = rect1.right + 1; rect1.right = rect.right - 5;
	if (m_pEdit && m_pEdit->m_hWnd)
	{
		m_pEdit->MoveWindow(rect1);
		m_pEdit->SetWindowText(m_str);
	}
// 	CCellRange Selection = m_pGrid->GetSelectedCellRange();
// 	int minRow = Selection.GetMinRow();
// 	int maxRow = Selection.GetMaxRow();
// 	int minCol = Selection.GetMinCol();
// 	int maxCol = Selection.GetMaxCol();
// 	if (minRow == maxRow && minCol == maxCol && nRow == minRow && nCol == minCol)
// 	{
// 		m_pEdit->SendMessage(WM_LBUTTONDOWN);
// 		m_pEdit->SendMessage(WM_LBUTTONUP);
// 	}
// 	else
// 		m_pEdit->SendMessage(WM_KILLFOCUS);
	BOOL bResult = CGridCell::Draw(pDC, nRow, nCol, rect, bEraseBkgnd);
	return bResult;
	return TRUE;
}

void CGridCellSpinEdit::SetText(int val)
{
	_stprintf(m_str, _T("%d"), val);
}

BEGIN_MESSAGE_MAP(CGridCellSpinEdit, CGridCell)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


void CGridCellSpinEdit::OnSize(UINT nType, int cx, int cy)
{
	CGridCell::OnSize(nType, cx, cy);
/*
	CRect rect, rect1;
	GetClientRect(rect);
	rect1.top = rect.top + 1; rect1.bottom = rect.bottom - 1; rect1.left = rect.left + rect.Width() / 3; rect1.right = rect1.left + 20;
	if (m_pSpin && m_pSpin->m_hWnd)
	{
		if (rect1.left < 10)
			int kk = 0;
		m_pSpin->MoveWindow(rect1);
		m_pSpin->ShowWindow(SW_SHOW);
	}
	rect1.left = rect1.right + 1; rect1.right = rect.right - 5;
	if (m_pEdit && m_pEdit->m_hWnd)
	{
		m_pEdit->MoveWindow(rect1);
		m_pEdit->SetWindowText(m_str);
		m_pEdit->ShowWindow(SW_SHOW);
	}
*/
}


int CGridCellSpinEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CGridCell::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}




void CGridCellSpinEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (nChar >= 0x30 && nChar <= 0x39)
	{
		TCHAR str[10];
		m_pEdit->GetWindowText(str, 10);
		int val = CStringToInt(str);
// 		if (val != 0)
		{
			_stprintf(str, _T("%s%c"), str, nChar);
			m_pEdit->SetWindowText(str);
		}
	}
	CGridCell::OnKeyDown(nChar, nRepCnt, nFlags);
}
