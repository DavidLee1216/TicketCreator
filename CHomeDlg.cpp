// CHomeDlg.cpp : implementation file
//

#include "pch.h"
#include "TicketCreator.h"
#include "CHomeDlg.h"
#include "afxdialogex.h"

#define DEFAULT_ROW_CNT 5
#define DEFAULT_COL_CNT 3
#define DEFAULT_SCORE_BASE 0.952

#define IDC_HOME_GRID 5001
#define WM_TABLE_UPDATED WM_USER+2
#define WM_TABLE_ADD_COLUMN WM_USER+3
#define WM_TABLE_ADD_ROW WM_USER+4

IMPLEMENT_DYNAMIC(CHomeDlg, CDialog)

double g_dblScoreBase = 0;

CHomeDlg::CHomeDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_HOME, pParent)
	, m_nMatchCount(0)
	, m_nOutcomeCount(0)
	, m_dblScoreBase(0)
{
	m_pGrid = NULL;
	m_pParentWnd = NULL;
	m_nMatchCount = DEFAULT_ROW_CNT;
	m_nOutcomeCount = DEFAULT_COL_CNT;
	m_dblScoreBase = DEFAULT_SCORE_BASE;
	g_dblScoreBase = m_dblScoreBase;
}

CHomeDlg::~CHomeDlg()
{
	if (m_pGrid)
		delete m_pGrid;
}

void CHomeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MATCH_COUNT, m_nMatchCount);
	DDX_Text(pDX, IDC_EDIT_OUTCOME_COUNT, m_nOutcomeCount);
	DDX_Text(pDX, IDC_EDIT_SCORE_BASE, m_dblScoreBase);
}


BEGIN_MESSAGE_MAP(CHomeDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CHomeDlg::OnBnClickedOk)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_CREAT_TABLE, &CHomeDlg::OnBnClickedButtonCreatTable)
	ON_WM_CREATE()
	ON_WM_PAINT()
//	ON_WM_DRAWITEM()
ON_WM_CTLCOLOR()
ON_BN_CLICKED(IDC_BUTTON_ADD_COLUMN, &CHomeDlg::OnBnClickedButtonAddColumn)
ON_BN_CLICKED(IDC_BUTTON_ADD_ROW, &CHomeDlg::OnBnClickedButtonAddRow)
ON_EN_CHANGE(IDC_EDIT_SCORE_BASE, &CHomeDlg::OnEnChangeEditScoreBase)
END_MESSAGE_MAP()


// CHomeDlg message handlers


void CHomeDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
}


BOOL CHomeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_pGrid == NULL) {
		m_pGrid = new CGridCtrl;
		if (!m_pGrid) return FALSE;
		CRect rect;
		GetClientRect(rect);
		rect.top = rect.top + 50;
		rect.bottom = rect.bottom - 10;
		rect.left = rect.left + 10;
		rect.right = rect.right - 10;
		m_pGrid->Create(rect, this, IDC_HOME_GRID);
		setDefaultGrid();
		MoveWindow(rect);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CHomeDlg::setDefaultGrid()
{
	m_pGrid->EnableDragAndDrop(FALSE);
	m_pGrid->GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
	m_pGrid->SetEditable(FALSE);
	m_pGrid->SetFixedRowCount(1);
	m_pGrid->SetFixedColumnCount(1);
	m_pGrid->SetColumnWidth(0, 30);
	m_pGrid->SetFixedBkColor(RGB(0, 25, 255));
	m_pGrid->SetBkColor(RGB(255, 255, 255));
	m_nMatchCount = 5;
	m_pGrid->SetRowCount(DEFAULT_ROW_CNT+1);
	m_pGrid->SetColumnCount(DEFAULT_COL_CNT+1);
	TCHAR temp[10];
	for (int i = 1; i <= DEFAULT_ROW_CNT; i++)
	{
		for (int j = 1; j <= DEFAULT_COL_CNT; j++)
		{
			_stprintf(temp, _T("%d"), j);
			m_pGrid->SetItemText(i, j, temp);
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

void CHomeDlg::OnSize(UINT nType, int cx, int cy)
{
// 	CDialog::OnSize(nType, cx, cy);

	if (m_pGrid) {
		CRect rect;
		GetClientRect(rect);
		int nGridHeight = m_pGrid->GetGridHeight();
		rect.top = rect.top + 50;
		rect.bottom = rect.top + nGridHeight+5;
		rect.left = rect.left + 10;
		rect.right = rect.right - 10;
		m_pGrid->MoveWindow(rect);
	}
}


void CHomeDlg::OnBnClickedButtonCreatTable()
{
	UpdateData(TRUE);
	if (m_nMatchCount > 20)
	{
		AfxMessageBox(_T("the count of matches must be less than 20")); return;
	}
	else if (m_nOutcomeCount > 20)
	{
		AfxMessageBox(_T("the count of outcomes must be less than 30")); return;
	}
	if (m_nMatchCount > 10 && m_nOutcomeCount > 6)
	{
		AfxMessageBox(_T("The count of outcomes or matches is too large")); return;
	}
	else if (m_nOutcomeCount > 10 && m_nOutcomeCount < 20 && m_nMatchCount>8)
	{
		AfxMessageBox(_T("The count of outcomes or matches is too large")); return;
	}
	int n = pow(m_nOutcomeCount, m_nMatchCount);
	if (n > 0xfffff || n < 0)
	{
		int yesno = AfxMessageBox(_T("It may cause unexpected results or take a long time. Will you do it anyway?"), MB_OKCANCEL);
		if (yesno == IDCANCEL)
			return;
	}
	m_pGrid->DeleteNonFixedRows();
	m_pGrid->SetRowCount(m_nMatchCount + 1);
	m_pGrid->SetColumnCount(m_nOutcomeCount + 1);
	TCHAR temp[10];
	for (int i = 1; i <= m_nMatchCount; i++)
	{
		for (int j = 1; j <= m_nOutcomeCount; j++)
		{
			_stprintf(temp, _T("%d"), j);
			m_pGrid->SetItemText(i, j, temp);
		}
	}
	for (int i = 1; i <= m_nOutcomeCount; i++)
	{
		_stprintf(temp, _T("%d"), i);
		m_pGrid->SetItemText(0, i, temp);
	}
	for (int i = 1; i <= m_nMatchCount; i++)
	{
		_stprintf(temp, _T("%d"), i);
		m_pGrid->SetItemText(i, 0, temp);
	}
	m_pGrid->SetFixedTextColor(RGB(255, 255, 255));
	SendMessage(WM_SIZE, 0, 0);
	SendMessage(WM_TABLE_UPDATED, m_nMatchCount, m_nOutcomeCount);
}


LRESULT CHomeDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_TABLE_UPDATED)
	{
		m_pParentWnd->SendMessage(WM_TABLE_UPDATED, wParam, lParam);
	}
	if (message == WM_TABLE_ADD_COLUMN)
		m_pParentWnd->SendMessage(WM_TABLE_ADD_COLUMN, wParam, lParam);
	if (message == WM_TABLE_ADD_ROW)
		m_pParentWnd->SendMessage(WM_TABLE_ADD_ROW, wParam, lParam);
	return CDialog::WindowProc(message, wParam, lParam);
}


int CHomeDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pParentWnd = FromHandle(lpCreateStruct->hwndParent);

	return 0;
}


void CHomeDlg::OnPaint()
{
	CPaintDC dc(this);
	HBITMAP hbm = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BACKGROUND));
	BITMAP bm;
	HDC hdcMem = CreateCompatibleDC(dc);
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbm);
	GetObject(hbm, sizeof(bm), &bm);
	BitBlt(dc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hbmOld);
	DeleteDC(hdcMem);
}


HBRUSH CHomeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkColor(RGB(190, 219, 255));
	}
	return hbr;
}


void CHomeDlg::ReloadTable(int nRow, int nCol)
{
	m_pGrid->DeleteNonFixedRows();
	m_nMatchCount = nRow;
	m_nOutcomeCount = nCol;
	m_pGrid->SetRowCount(m_nMatchCount + 1);
	m_pGrid->SetColumnCount(m_nOutcomeCount + 1);
	TCHAR temp[10];
	for (int i = 1; i <= m_nMatchCount; i++)
	{
		for (int j = 1; j <= m_nOutcomeCount; j++)
		{
			_stprintf(temp, _T("%d"), j);
			m_pGrid->SetItemText(i, j, temp);
		}
	}
	for (int i = 1; i <= m_nOutcomeCount; i++)
	{
		_stprintf(temp, _T("%d"), i);
		m_pGrid->SetItemText(0, i, temp);
	}
	for (int i = 1; i <= m_nMatchCount; i++)
	{
		_stprintf(temp, _T("%d"), i);
		m_pGrid->SetItemText(i, 0, temp);
	}
	m_pGrid->SetFixedTextColor(RGB(255, 255, 255));
	UpdateData(FALSE);
	SendMessage(WM_SIZE, 0, 0);
}


void CHomeDlg::OnBnClickedButtonAddColumn()
{
	m_nOutcomeCount++;
	UpdateData(false);
	if (m_nMatchCount > 20)
	{
		AfxMessageBox(_T("the count of matches must be less than 20")); return;
	}
	else if (m_nOutcomeCount > 20)
	{
		AfxMessageBox(_T("the count of outcomes must be less than 30")); return;
	}
	if (m_nMatchCount > 10 && m_nOutcomeCount > 6)
	{
		AfxMessageBox(_T("The count of outcomes or matches is too large")); return;
	}
	else if (m_nOutcomeCount > 10 && m_nOutcomeCount < 20 && m_nMatchCount>8)
	{
		AfxMessageBox(_T("The count of outcomes or matches is too large")); return;
	}
	int n = pow(m_nOutcomeCount, m_nMatchCount);
	if (n > 0xfffff || n < 0)
	{
		int yesno = AfxMessageBox(_T("It may cause unexpected results or take a long time. Will you do it anyway?"), MB_OKCANCEL);
		if (yesno == IDCANCEL)
			return;
	}
	m_pGrid->DeleteNonFixedRows();
	m_pGrid->SetRowCount(m_nMatchCount + 1);
	m_pGrid->SetColumnCount(m_nOutcomeCount + 1);
	TCHAR temp[10];
	for (int i = 1; i <= m_nMatchCount; i++)
	{
		for (int j = 1; j <= m_nOutcomeCount; j++)
		{
			_stprintf(temp, _T("%d"), j);
			m_pGrid->SetItemText(i, j, temp);
		}
	}
	for (int i = 1; i <= m_nOutcomeCount; i++)
	{
		_stprintf(temp, _T("%d"), i);
		m_pGrid->SetItemText(0, i, temp);
	}
	for (int i = 1; i <= m_nMatchCount; i++)
	{
		_stprintf(temp, _T("%d"), i);
		m_pGrid->SetItemText(i, 0, temp);
	}
	m_pGrid->SetFixedTextColor(RGB(255, 255, 255));
	SendMessage(WM_SIZE, 0, 0);
	SendMessage(WM_TABLE_ADD_COLUMN, m_nMatchCount, m_nOutcomeCount);
}


void CHomeDlg::OnBnClickedButtonAddRow()
{
	m_nMatchCount++;
	UpdateData(false);
	if (m_nMatchCount > 20)
	{
		AfxMessageBox(_T("the count of matches must be less than 20")); return;
	}
	else if (m_nOutcomeCount > 20)
	{
		AfxMessageBox(_T("the count of outcomes must be less than 30")); return;
	}
	if (m_nMatchCount > 10 && m_nOutcomeCount > 6)
	{
		AfxMessageBox(_T("The count of outcomes or matches is too large")); return;
	}
	else if (m_nOutcomeCount > 10 && m_nOutcomeCount < 20 && m_nMatchCount>8)
	{
		AfxMessageBox(_T("The count of outcomes or matches is too large")); return;
	}
	int n = pow(m_nOutcomeCount, m_nMatchCount);
	if (n > 0xfffff || n < 0)
	{
		int yesno = AfxMessageBox(_T("It may cause unexpected results or take a long time. Will you do it anyway?"), MB_OKCANCEL);
		if (yesno == IDCANCEL)
			return;
	}
	m_pGrid->DeleteNonFixedRows();
	m_pGrid->SetRowCount(m_nMatchCount + 1);
	m_pGrid->SetColumnCount(m_nOutcomeCount + 1);
	TCHAR temp[10];
	for (int i = 1; i <= m_nMatchCount; i++)
	{
		for (int j = 1; j <= m_nOutcomeCount; j++)
		{
			_stprintf(temp, _T("%d"), j);
			m_pGrid->SetItemText(i, j, temp);
		}
	}
	for (int i = 1; i <= m_nOutcomeCount; i++)
	{
		_stprintf(temp, _T("%d"), i);
		m_pGrid->SetItemText(0, i, temp);
	}
	for (int i = 1; i <= m_nMatchCount; i++)
	{
		_stprintf(temp, _T("%d"), i);
		m_pGrid->SetItemText(i, 0, temp);
	}
	m_pGrid->SetFixedTextColor(RGB(255, 255, 255));
	SendMessage(WM_SIZE, 0, 0);
	SendMessage(WM_TABLE_ADD_ROW, m_nMatchCount, m_nOutcomeCount);
}


void CHomeDlg::OnEnChangeEditScoreBase()
{
	UpdateData(TRUE);
	g_dblScoreBase = m_dblScoreBase;

}
