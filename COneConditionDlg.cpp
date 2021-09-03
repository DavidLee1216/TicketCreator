// COneConditionDlg.cpp : implementation file
//

#include "pch.h"
#include "TicketCreator.h"
#include "COneConditionDlg.h"
#include "NewCellTypes/GridCellCheck.h"
#include "afxdialogex.h"

#define DEFAULT_ROW_CNT 5
#define DEFAULT_COL_CNT 3

#define IDC_ONE_CONDITION_GRID 5002
#define VISIBLE_GRID_LINES 10

#define WM_DELETE_COND WM_USER+1

IMPLEMENT_DYNAMIC(COneConditionDlg, CDialog)

int CStringToInt(CString str);

COneConditionDlg::COneConditionDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_ONE_CONDITION, pParent)
{
	m_pGrid = NULL;
	m_pParentWnd = NULL;
	m_nId = 0;
	m_nMin = 0;
	m_nMax = 0;
}

COneConditionDlg::~COneConditionDlg()
{
	if (m_pGrid)
		delete m_pGrid;
}

void COneConditionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_ctrlDel);
	DDX_Control(pDX, IDC_EDIT_MIN_COND, m_ctrlMin);
	DDX_Control(pDX, IDC_EDIT_MAX_COND, m_ctrlMax);
	DDX_Control(pDX, IDC_SPIN_MIN_COND, m_ctrlMinSpin);
	DDX_Control(pDX, IDC_SPIN_MAX_COND, m_ctrlMaxSpin);
	DDX_Control(pDX, IDC_STATIC_STATE, m_ctrlState);
	DDX_Control(pDX, IDC_STATIC_MIN, m_ctrlStaticMin);
	DDX_Control(pDX, IDC_STATIC_MAX, m_ctrlStaticMax);
}


BEGIN_MESSAGE_MAP(COneConditionDlg, CDialog)
	ON_BN_CLICKED(IDOK, &COneConditionDlg::OnBnClickedOk)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MIN_COND, &COneConditionDlg::OnDeltaposSpinMinCond)
	ON_EN_CHANGE(IDC_EDIT_MIN_COND, &COneConditionDlg::OnEnChangeEditMinCond)
	ON_EN_CHANGE(IDC_EDIT_MAX_COND, &COneConditionDlg::OnEnChangeEditMaxCond)
	ON_NOTIFY(NM_CLICK, IDC_ONE_CONDITION_GRID, OnGridClick)
END_MESSAGE_MAP()


// COneConditionDlg message handlers


void COneConditionDlg::OnBnClickedOk()
{
	m_pParentWnd->SendMessage(WM_DELETE_COND, m_nId, 0);
}


BOOL COneConditionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_pGrid == NULL) {
		m_pGrid = new CGridCtrl;
		if (!m_pGrid) return FALSE;
		CRect rect;
		GetClientRect(rect);
		m_pGrid->Create(rect, this, IDC_ONE_CONDITION_GRID);
		setDefaultGrid();
		m_ctrlMinSpin.SetBuddy(&m_ctrlMin);
		m_ctrlMinSpin.SetRange(0, 20);
		m_ctrlMinSpin.SetPos(0);
		m_ctrlMinSpin.SetDelta(1);
		m_ctrlMaxSpin.SetBuddy(&m_ctrlMax);
		m_ctrlMaxSpin.SetRange(0, 20);
		m_ctrlMaxSpin.SetPos(0);
		m_ctrlMaxSpin.SetDelta(1);
		m_ctrlDel.SetWindowText(_T("DELETE"));
		SendMessage(WM_SIZE, 0, 0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void COneConditionDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (m_pGrid) {
		CRect rect;
		GetClientRect(rect);
		CRect rect1, rect2;
		if (m_ctrlState) {
			TCHAR szTemp[50];
			_stprintf(szTemp, _T("Condition %d"), m_nId+1);
			m_ctrlState.SetWindowText(szTemp);
		}
		m_ctrlState.GetWindowRect(rect1);
		rect2.top = rect.top + 5;
		rect2.bottom = rect2.top + rect1.Height();
		rect2.left = rect.left + 100;
		rect2.right = rect2.left + rect1.Width();
		m_ctrlState.MoveWindow(rect2);
		m_ctrlDel.GetWindowRect(rect1);
		rect2.top = rect.top + 5;
		rect2.bottom = rect2.top + rect1.Height();
		rect2.left = rect.right/2 + rect1.Width();
		rect2.right = rect2.left + rect1.Width();
		m_ctrlDel.MoveWindow(rect2);
		rect.top = rect2.bottom + 5;
		rect.bottom = rect.top + m_pGrid->GetGridHeight(VISIBLE_GRID_LINES);
		rect.left = rect.left + 10;
		rect.right = rect.right - 10;
		m_pGrid->MoveWindow(rect);
		m_ctrlStaticMin.GetWindowRect(rect1);
		rect.top = rect.bottom + 5; rect.bottom = rect.top + rect1.Height(); rect.left = rect.left + 50; rect.right = rect.left + rect1.Width();
		rect2 = rect; rect2.top += 5; rect2.bottom += 5;
		m_ctrlStaticMin.MoveWindow(rect2);
		m_ctrlMin.GetWindowRect(rect1);
		rect.left = rect.right + 10; rect.right = rect.left + rect1.Width();
		m_ctrlMin.MoveWindow(rect);
		m_ctrlMinSpin.GetWindowRect(rect1);
		rect.left = rect.right-1; rect.right = rect.left + rect1.Width();
		m_ctrlMinSpin.MoveWindow(rect);
		m_ctrlStaticMax.GetWindowRect(rect1);
		rect.left = rect.right + 50; rect.right = rect.left + rect1.Width();
		rect2 = rect; rect2.top += 5; rect2.bottom += 5;
		m_ctrlStaticMax.MoveWindow(rect2);
		m_ctrlMax.GetWindowRect(rect1);
		rect.left = rect.right + 10; rect.right = rect.left + rect1.Width();
		m_ctrlMax.MoveWindow(rect);
		m_ctrlMaxSpin.GetWindowRect(rect1);
		rect.left = rect.right-1; rect.right = rect.left + rect1.Width();
		m_ctrlMaxSpin.MoveWindow(rect);

	}
}

void COneConditionDlg::setDefaultGrid()
{
	m_pGrid->EnableDragAndDrop(TRUE);
	m_pGrid->GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
	m_pGrid->SetEditable(TRUE);
	m_pGrid->SetFixedRowCount(1);
	m_pGrid->SetFixedColumnCount(1);
	m_pGrid->SetColumnWidth(0, 30);
	m_pGrid->SetFixedBkColor(RGB(0, 25, 255));
	m_pGrid->SetBkColor(RGB(255, 255, 255));
	m_pGrid->SetRowCount(DEFAULT_ROW_CNT+1);
	m_pGrid->SetColumnCount(DEFAULT_COL_CNT+1);
	TCHAR temp[10];
	for (int i = 1; i <= DEFAULT_ROW_CNT; i++)
	{
		for (int j = 1; j <= DEFAULT_COL_CNT; j++)
		{
			_stprintf(temp, _T("%d"), j);
			m_pGrid->SetItemText(i, j, temp);
			m_pGrid->SetCellType(i, j, RUNTIME_CLASS(CGridCell));
			m_pGrid->SetItemState(i, j, m_pGrid->GetItemState(i, j) & ~GVIS_READONLY);
			m_pGrid->SetCellType(i, j, RUNTIME_CLASS(CGridCellCheck));
		}
	}
	for (int i = 1; i <= DEFAULT_COL_CNT; i++)
	{
		_stprintf(temp, _T("%d"), i);
		m_pGrid->SetItemText(0, i, temp);
	}
	for (int i = 1; i <= DEFAULT_ROW_CNT; i++)
	{
		_stprintf(temp, _T("%d"), i);
		m_pGrid->SetItemText(i, 0, temp);
	}
	m_pGrid->SetFixedTextColor(RGB(255, 255, 255));
}


int COneConditionDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pParentWnd = FromHandle(lpCreateStruct->hwndParent);

	return 0;
}

void COneConditionDlg::OnGridClick(NMHDR* pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	if (pItem->iRow < 1 || pItem->iColumn < 1)
		return;
	CGridCellCheck* clickedCell = (CGridCellCheck*)m_pGrid->GetCell(pItem->iRow, pItem->iColumn);
	bool checkState = clickedCell->GetCheck();
	CCellRange selectedRange = m_pGrid->GetSelectedCellRange();
	int nMinRow = selectedRange.GetMinRow();
	int nMaxRow = selectedRange.GetMaxRow();
	int nMinCol = selectedRange.GetMinCol();
	int nMaxCol = selectedRange.GetMaxCol();
	for (int i = nMinRow; i <= nMaxRow; i++)
	{
		for (int j = nMinCol; j <= nMaxCol; j++)
		{
			CGridCellCheck* cell = (CGridCellCheck*)m_pGrid->GetCell(i, j);
			if (checkState == true)
			{
				cell->SetCheck(checkState);
				m_checkedItems.addItem(i, j);
			}
			else
			{
				cell->SetCheck(checkState);
				m_checkedItems.removeItem(i, j);
			}
		}
	}
	SendMessage(WM_PAINT, 0, 0);
}

void COneConditionDlg::CreateTable(int nRow, int nCol)
{
	m_pGrid->DeleteNonFixedRows();
	m_pGrid->SetRowCount(nRow + 1);
	m_pGrid->SetColumnCount(nCol + 1);
	TCHAR temp[10];
	for (int i = 1; i <= nRow; i++)
	{
		for (int j = 1; j <= nCol; j++)
		{
			_stprintf(temp, _T("%d"), j);
			m_pGrid->SetItemText(i, j, temp);
			m_pGrid->SetCellType(i, j, RUNTIME_CLASS(CGridCell));
			m_pGrid->SetItemState(i, j, m_pGrid->GetItemState(i, j) & ~GVIS_READONLY);
			m_pGrid->SetCellType(i, j, RUNTIME_CLASS(CGridCellCheck));
		}
	}
	for (int i = 1; i <= nCol; i++)
	{
		_stprintf(temp, _T("%d"), i);
		m_pGrid->SetItemText(0, i, temp);
	}
	for (int i = 1; i <= nRow; i++)
	{
		_stprintf(temp, _T("%d"), i);
		m_pGrid->SetItemText(i, 0, temp);
	}
	m_pGrid->SetFixedTextColor(RGB(255, 255, 255));
	SendMessage(WM_SIZE, 0, 0);
}


void COneConditionDlg::OnDeltaposSpinMinCond(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;
}


void COneConditionDlg::OnEnChangeEditMinCond()
{
	TCHAR str[5];
	if (m_ctrlMin.m_hWnd)
	{
		m_ctrlMin.GetWindowText(str, 3);
		m_nMin = CStringToInt((CString)str);
	}
}


void COneConditionDlg::OnEnChangeEditMaxCond()
{
	TCHAR str[5];
	if (m_ctrlMax.m_hWnd)
	{
		m_ctrlMax.GetWindowText(str, 3);
		m_nMax = CStringToInt((CString)str);
	}
}


void COneConditionDlg::AddColumn(int nRow, int nCol)
{
	m_pGrid->SetRowCount(nRow + 1);
	m_pGrid->SetColumnCount(nCol + 1);
	TCHAR temp[10];
	for (int i = 1; i <= nRow; i++)
	{
//		for (int j = 1; j <= nCol; j++)
//		{
			_stprintf(temp, _T("%d"), nCol);
			m_pGrid->SetItemText(i, nCol, temp);
			m_pGrid->SetCellType(i, nCol, RUNTIME_CLASS(CGridCell));
			m_pGrid->SetItemState(i, nCol, m_pGrid->GetItemState(i, nCol) & ~GVIS_READONLY);
			m_pGrid->SetCellType(i, nCol, RUNTIME_CLASS(CGridCellCheck));
//		}
	}
//	for (int i = 1; i <= nCol; i++)
//	{
		_stprintf(temp, _T("%d"), nCol);
		m_pGrid->SetItemText(0, nCol, temp);
//	}
/*	for (int i = 1; i <= nRow; i++)
	{
		_stprintf(temp, _T("%d"), i);
		m_pGrid->SetItemText(i, 0, temp);
	}*/
	m_pGrid->SetFixedTextColor(RGB(255, 255, 255));
	SendMessage(WM_SIZE, 0, 0);
}


void COneConditionDlg::AddRow(int nRow, int nCol)
{
	m_pGrid->SetRowCount(nRow + 1);
	TCHAR temp[10];
	for (int j = 1; j <= nCol; j++)
	{
		_stprintf(temp, _T("%d"), j);
		m_pGrid->SetItemText(nRow, j, temp);
		m_pGrid->SetCellType(nRow, j, RUNTIME_CLASS(CGridCell));
		m_pGrid->SetItemState(nRow, j, m_pGrid->GetItemState(nRow, j) & ~GVIS_READONLY);
		m_pGrid->SetCellType(nRow, j, RUNTIME_CLASS(CGridCellCheck));
	}
	_stprintf(temp, _T("%d"), nRow);
	m_pGrid->SetItemText(nRow, 0, temp);
	m_pGrid->SetFixedTextColor(RGB(255, 255, 255));
	SendMessage(WM_SIZE, 0, 0);
}
