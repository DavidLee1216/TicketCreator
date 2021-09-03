// CPointLimitsDlg.cpp : implementation file
//

#include "pch.h"
#include "TicketCreator.h"
#include "CPointLimitsDlg.h"
#include "NewCellTypes/GridCellSpinEdit.h"

#include "afxdialogex.h"

#define DEFAULT_ROW_CNT 5
#define DEFAULT_COL_CNT 3

#define IDC_POINT_LIMIT_GRID 5003

#define WM_POINT_VAL_CHANGED WM_USER+5
#define WM_POINT_VAL_CHANGE_END WM_USER+6

IMPLEMENT_DYNAMIC(CPointLimitsDlg, CDialog)

int CStringToInt(CString str);

CPointLimitsDlg::CPointLimitsDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_POINT_LIMITS, pParent)
{
	m_pGrid = NULL;
	m_pParentWnd = NULL;
	m_nMin = 0;
	m_nMax = 0;
	bOnceSelectionChanged = false;
	for (int i = 0; i < 21; i++)
	{
		for (int j = 0; j< 31; j++)
		{
			m_nppPointMatrix[i][j] = 0;
		}
	}
}

CPointLimitsDlg::~CPointLimitsDlg()
{
	if (m_pGrid)
		delete m_pGrid;
}

void CPointLimitsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_MIN_COND, m_ctrlMin);
	DDX_Control(pDX, IDC_EDIT_MAX_COND, m_ctrlMax);
	DDX_Control(pDX, IDC_SPIN_MIN_COND, m_ctrlMinSpin);
	DDX_Control(pDX, IDC_SPIN_MAX_COND, m_ctrlMaxSpin);
	DDX_Control(pDX, IDC_STATIC_MIN, m_ctrlStaticMin);
	DDX_Control(pDX, IDC_STATIC_MAX, m_ctrlStaticMax);
	DDX_Control(pDX, IDC_MFCBUTTON_COPY, m_ctrlCopy);
	DDX_Control(pDX, IDC_MFCBUTTON_PASTE, m_ctrlPaste);
}


BEGIN_MESSAGE_MAP(CPointLimitsDlg, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_EN_CHANGE(IDC_EDIT_MIN_COND, &CPointLimitsDlg::OnEnChangeEditMinCond)
	ON_EN_CHANGE(IDC_EDIT_MAX_COND, &CPointLimitsDlg::OnEnChangeEditMaxCond)
	ON_COMMAND(IDOK, &CPointLimitsDlg::OnIdok)
	ON_BN_CLICKED(IDC_MFCBUTTON_COPY, &CPointLimitsDlg::OnBnClickedMfcbuttonCopy)
	ON_BN_CLICKED(IDC_MFCBUTTON_PASTE, &CPointLimitsDlg::OnBnClickedMfcbuttonPaste)
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MIN_COND, &CPointLimitsDlg::OnDeltaposSpinMinCond)
END_MESSAGE_MAP()


// CPointLimitsDlg message handlers


int CPointLimitsDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pParentWnd = FromHandle(lpCreateStruct->hwndParent);

	return 0;
}


void CPointLimitsDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (m_pGrid) {
		CRect rect, rect1, rect2;
		GetClientRect(rect);
		m_ctrlCopy.GetWindowRect(rect2);
		rect1.top = rect.top + 10; rect1.bottom = rect1.top + rect2.Height(); rect1.left = rect.left + 100; rect1.right = rect1.left + rect2.Width();
		m_ctrlCopy.MoveWindow(rect1);
		rect1.left = rect1.right + 100; rect1.right = rect1.left + rect2.Width();
		m_ctrlPaste.MoveWindow(rect1);
		int nGridHeight = m_pGrid->GetGridHeight();
		rect.top = rect.top + 50;
		rect.bottom = rect.top + nGridHeight + 5;
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
		rect.left = rect.right - 1; rect.right = rect.left + rect1.Width();
		m_ctrlMinSpin.MoveWindow(rect);
		m_ctrlStaticMax.GetWindowRect(rect1);
		rect.left = rect.right + 50; rect.right = rect.left + rect1.Width();
		rect2 = rect; rect2.top += 5; rect2.bottom += 5;
		m_ctrlStaticMax.MoveWindow(rect2);
		m_ctrlMax.GetWindowRect(rect1);
		rect.left = rect.right + 10; rect.right = rect.left + rect1.Width();
		m_ctrlMax.MoveWindow(rect);
		m_ctrlMaxSpin.GetWindowRect(rect1);
		rect.left = rect.right - 1; rect.right = rect.left + rect1.Width();
		m_ctrlMaxSpin.MoveWindow(rect);
	}
}


LRESULT CPointLimitsDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_POINT_VAL_CHANGED)
	{
		int row = HIWORD(wParam);
		int col = LOWORD(wParam);
		int val = lParam;
		m_nppPointMatrix[row][col] = val;

		if (bOnceSelectionChanged == false)
		{
			CCellRange Selection = m_pGrid->GetSelectedCellRange();
			int minRow = Selection.GetMinRow();
			int maxRow = Selection.GetMaxRow();
			int minCol = Selection.GetMinCol();
			int maxCol = Selection.GetMaxCol();
			if (!m_pGrid->IsValid(Selection))
				return NULL;
			if (row >= minRow && row <= maxRow && col >= minCol && col <= maxCol)
			{
				CGridCellSpinEdit* pCell;
				TCHAR str[10];
				bOnceSelectionChanged = true;
				for (int i = minRow; i <= maxRow; i++)
				{
					for (int j = minCol; j <= maxCol; j++)
					{
						if(i==row && j==col)
							continue;
						pCell = (CGridCellSpinEdit*)m_pGrid->GetCell(i, j);
						_stprintf(str, _T("%d"), val);
						pCell->m_pEdit->SetWindowText(str);
					}
				}
			}
		}
	}
	if (message == WM_POINT_VAL_CHANGE_END)
	{
		int row = HIWORD(wParam);
		int col = LOWORD(wParam);
		int val = lParam;
		m_nppPointMatrix[row][col] = val;

		CCellRange Selection = m_pGrid->GetSelectedCellRange();
		int minRow = Selection.GetMinRow();
		int maxRow = Selection.GetMaxRow();
		int minCol = Selection.GetMinCol();
		int maxCol = Selection.GetMaxCol();
		if (!m_pGrid->IsValid(Selection))
			return NULL;
		if (row >= minRow && row <= maxRow && col >= minCol && col <= maxCol)
		{
			CGridCellSpinEdit* pCell;
			TCHAR str[10];
			bOnceSelectionChanged = true;
			for (int i = minRow; i <= maxRow; i++)
			{
				for (int j = minCol; j <= maxCol; j++)
				{
					if (i == row && j == col)
						continue;
					pCell = (CGridCellSpinEdit*)m_pGrid->GetCell(i, j);
					_stprintf(str, _T("%d"), val);
					pCell->m_pEdit->SetWindowText(str);
				}
			}
		}

	}
	if (message == WM_KEYDOWN)
	{
		int kk = 0;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}


BOOL CPointLimitsDlg::OnInitDialog()
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
		m_pGrid->Create(rect, this, IDC_POINT_LIMIT_GRID);
		setDefaultGrid();

		m_ctrlMinSpin.SetBuddy(&m_ctrlMin);
		m_ctrlMinSpin.SetRange(0, 1000);
		m_ctrlMinSpin.SetPos(0);
		m_ctrlMinSpin.SetDelta(1);
		m_ctrlMaxSpin.SetBuddy(&m_ctrlMax);
		m_ctrlMaxSpin.SetRange(0, 1000);
		m_ctrlMaxSpin.SetPos(0);
		m_ctrlMaxSpin.SetDelta(1);
		SendMessage(WM_SIZE, 0, 0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CPointLimitsDlg::OnEnChangeEditMinCond()
{
	TCHAR str[10];
	m_ctrlMin.GetWindowText(str, 10);
	m_nMin = CStringToInt((CString)str);
}


void CPointLimitsDlg::OnEnChangeEditMaxCond()
{
	TCHAR str[10];
	m_ctrlMax.GetWindowText(str, 10);
	m_nMax = CStringToInt((CString)str);
}


void CPointLimitsDlg::setDefaultGrid()
{
	m_pGrid->EnableDragAndDrop(FALSE);
	m_pGrid->GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
	m_pGrid->SetEditable(FALSE);
	m_pGrid->SetFixedRowCount(1);
	m_pGrid->SetFixedColumnCount(1);
	m_pGrid->SetColumnWidth(0, 30);
	m_pGrid->SetFixedBkColor(RGB(0, 25, 255));
	m_pGrid->SetBkColor(RGB(255, 255, 255));
	m_pGrid->SetRowCount(DEFAULT_ROW_CNT +1);
	m_pGrid->SetColumnCount(DEFAULT_COL_CNT +1);
	m_pGrid->SetDoubleBuffering(TRUE);
	TCHAR temp[10];
	for (int i = 1; i <= DEFAULT_ROW_CNT; i++)
	{
		for (int j = 1; j <= DEFAULT_COL_CNT; j++)
		{
			_stprintf(temp, _T("%d"), j);
			m_pGrid->SetItemText(i, j, temp);
			m_pGrid->SetCellType(i, j, RUNTIME_CLASS(CGridCell));
			m_pGrid->SetItemState(i, j, m_pGrid->GetItemState(i, j) & ~GVIS_READONLY);
			m_pGrid->SetCellType(i, j, RUNTIME_CLASS(CGridCellSpinEdit));
		}
	}
	for (int i = 1; i <= DEFAULT_ROW_CNT; i++)
	{
		for (int j = 1; j <= DEFAULT_COL_CNT; j++)
		{
			CGridCellSpinEdit * cell = (CGridCellSpinEdit* )m_pGrid->GetCell(i, j);
			CRect rect;
			rect.top = 0; rect.bottom = 0; rect.left = 0; rect.right = 0;
			cell->m_pEdit->Create(ES_RIGHT | ES_NUMBER |/* ES_READONLY |*/ WS_VISIBLE, rect, m_pGrid, IDC_CELL_EDIT);
			cell->m_pEdit->m_pParentDlg = this;
			cell->m_pEdit->m_nRow = i;
			cell->m_pEdit->m_nCol = j;

			cell->m_pSpin->Create(UDS_ALIGNLEFT | UDS_ARROWKEYS | UDS_SETBUDDYINT | WS_VISIBLE, rect, m_pGrid, IDC_CELL_SPIN);
			cell->m_pSpin->SetBuddy(cell->m_pEdit);
			cell->m_pSpin->SetRange(0, 1000);
			cell->m_pSpin->SetDelta(1);
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


void CPointLimitsDlg::ReloadTable(int nRow, int nCol)
{
	m_pGrid->DeleteNonFixedRows();
	m_pGrid->SetRowCount(nRow + 1);
	m_pGrid->SetColumnCount(nCol + 1);
	m_ctrlMin.SetWindowText(_T("0"));
	m_ctrlMax.SetWindowText(_T("0"));
	TCHAR temp[10];
	for (int i = 1; i <= nRow; i++)
	{
		for (int j = 1; j <= nCol; j++)
		{
			_stprintf(temp, _T("%d"), j);
			m_pGrid->SetItemText(i, j, temp);
			m_pGrid->SetCellType(i, j, RUNTIME_CLASS(CGridCell));
			m_pGrid->SetItemState(i, j, m_pGrid->GetItemState(i, j) & ~GVIS_READONLY);
			m_pGrid->SetCellType(i, j, RUNTIME_CLASS(CGridCellSpinEdit));
		}
	}
	for (int i = 1; i <= nRow; i++)
	{
		for (int j = 1; j <= nCol; j++)
		{
			CGridCellSpinEdit* cell = (CGridCellSpinEdit*)m_pGrid->GetCell(i, j);
			CRect rect;
			rect.top = 0; rect.bottom = 0; rect.left = 0; rect.right = 0;
			cell->m_pSpin->Create(UDS_ALIGNLEFT | UDS_ARROWKEYS | UDS_SETBUDDYINT | WS_VISIBLE, rect, m_pGrid, IDC_CELL_SPIN);
			cell->m_pEdit->Create(ES_RIGHT | ES_NUMBER | /*ES_READONLY |*/ WS_VISIBLE, rect, m_pGrid, IDC_CELL_EDIT);
			cell->m_pSpin->SetBuddy(cell->m_pEdit);
			cell->m_pSpin->SetRange(0, 1000);
			cell->m_pSpin->SetDelta(1);
			cell->m_pEdit->m_pParentDlg = this;
			cell->m_pEdit->m_nRow = i;
			cell->m_pEdit->m_nCol = j;
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


void CPointLimitsDlg::OnIdok()
{
	// TODO: Add your command handler code here
}


void CPointLimitsDlg::OnBnClickedMfcbuttonCopy()
{
	OnEditCopy();
}


void CPointLimitsDlg::OnBnClickedMfcbuttonPaste()
{
	OnEditPaste();
}


void CPointLimitsDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{

	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CPointLimitsDlg::OnEditCopy()
{
	COleDataSource* pSource = CopyTextFromGrid();
	if (!pSource)
		return;

	pSource->SetClipboard();
}


void CPointLimitsDlg::OnEditPaste()
{
	CCellRange cellRange = m_pGrid->GetSelectedCellRange();

	// Get the top-left selected cell, or the Focus cell, or the topleft (non-fixed) cell
	CCellID cell;
	if (cellRange.IsValid())
	{
		cell.row = cellRange.GetMinRow();
		cell.col = cellRange.GetMinCol();
	}
	else
	{
		return;
	}

	CGridCellBase* pCell = m_pGrid->GetCell(cell.row, cell.col);
	if (!pCell) return;

	// Attach a COleDataObject to the clipboard and paste the data to the grid
	COleDataObject obj;
	if (obj.AttachClipboard())
		PasteTextToGrid(cell, &obj);
}

COleDataSource* CPointLimitsDlg::CopyTextFromGrid()
{
	USES_CONVERSION;

	CCellRange Selection = m_pGrid->GetSelectedCellRange();
	if (!m_pGrid->IsValid(Selection))
		return NULL;

	// Write to shared file (REMEBER: CF_TEXT is ANSI, not UNICODE, so we need to convert)
	CSharedFile sf(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);

	// Get a tab delimited string to copy to cache
	CString str;
	CGridCellSpinEdit* pCell;
	for (int row = Selection.GetMinRow(); row <= Selection.GetMaxRow(); row++)
	{
		str.Empty();
		for (int col = Selection.GetMinCol(); col <= Selection.GetMaxCol(); col++)
		{
			pCell = (CGridCellSpinEdit *)m_pGrid->GetCell(row, col);
			if (pCell && (pCell->GetState() & GVIS_SELECTED))
			{
				str += (CString)pCell->m_str;
			}
			if (col != Selection.GetMaxCol())
				str += _T("\t");
		}

		if (row != Selection.GetMaxRow())
			str += _T("\r\n");

		sf.Write(T2A(str.GetBuffer(1)), str.GetLength());
		str.ReleaseBuffer();
	}

	char c = '\0';
	sf.Write(&c, 1);

	DWORD dwLen = (DWORD)sf.GetLength();
	HGLOBAL hMem = sf.Detach();
	if (!hMem)
		return NULL;

	hMem = ::GlobalReAlloc(hMem, dwLen, GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);
	if (!hMem)
		return NULL;

	// Cache data
	COleDataSource* pSource = new COleDataSource();
	pSource->CacheGlobalData(CF_TEXT, hMem);

	return pSource;
}

// Pastes text from the clipboard to the selected cells
BOOL CPointLimitsDlg::PasteTextToGrid(CCellID cell, COleDataObject* pDataObject,
	BOOL bSelectPastedCells /*=TRUE*/)
{
	// Get the text from the COleDataObject
	HGLOBAL hmem = pDataObject->GetGlobalData(CF_TEXT);
	CMemFile sf((BYTE*) ::GlobalLock(hmem), (UINT)::GlobalSize(hmem));

	// CF_TEXT is ANSI text, so we need to allocate a char* buffer
	// to hold this.
	LPSTR szBuffer = new char[::GlobalSize(hmem)]; // FIX: Use LPSTR char here
	if (!szBuffer)
		return FALSE;

	sf.Read(szBuffer, (UINT)::GlobalSize(hmem));
	::GlobalUnlock(hmem);

	// Now store in generic TCHAR form so we no longer have to deal with
	// ANSI/UNICODE problems
	CString strText(szBuffer);
	delete szBuffer;

	// Parse text data and set in cells...
	strText.LockBuffer();
	CString strLine = strText;
	int nLine = 0;

	// Find the end of the first line
	CCellRange PasteRange(cell.row, cell.col, -1, -1);
	int nIndex;
	do
	{
		int nColumn = 0;
		nIndex = strLine.Find(_T("\n"));

		// Store the remaining chars after the newline
		CString strNext = (nIndex < 0) ? _T("") : strLine.Mid(nIndex + 1);

		// Remove all chars after the newline
		if (nIndex >= 0)
			strLine = strLine.Left(nIndex);

		int nLineIndex = strLine.FindOneOf(_T("\t,"));
		CString strCellText = (nLineIndex >= 0) ? strLine.Left(nLineIndex) : strLine;

		// skip hidden rows
		int iRowVis = cell.row + nLine;
		while (iRowVis < m_pGrid->GetRowCount())
		{
			if (m_pGrid->GetRowHeight(iRowVis) > 0)
				break;
			nLine++;
			iRowVis++;
		}

		while (!strLine.IsEmpty())
		{
			// skip hidden columns
			int iColVis = cell.col + nColumn;
			while (iColVis < m_pGrid->GetColumnCount())
			{
				if (m_pGrid->GetColumnWidth(iColVis) > 0)
					break;
				nColumn++;
				iColVis++;
			}

			CCellID TargetCell(iRowVis, iColVis);
			if (m_pGrid->IsValid(TargetCell))
			{
				strCellText.TrimLeft();
				strCellText.TrimRight();

				ValidateAndModifyCellContents(TargetCell.row, TargetCell.col, strCellText);

				m_pGrid->SetItemState(TargetCell.row, TargetCell.col,
					m_pGrid->GetItemState(TargetCell.row, TargetCell.col) & ~GVIS_SELECTED);

				if (iRowVis > PasteRange.GetMaxRow()) PasteRange.SetMaxRow(iRowVis);
				if (iColVis > PasteRange.GetMaxCol()) PasteRange.SetMaxCol(iColVis);
			}

			strLine = (nLineIndex >= 0) ? strLine.Mid(nLineIndex + 1) : _T("");
			nLineIndex = strLine.FindOneOf(_T("\t,"));
			strCellText = (nLineIndex >= 0) ? strLine.Left(nLineIndex) : strLine;

			nColumn++;
		}

		strLine = strNext;
		nLine++;
	} while (nIndex >= 0);

	strText.UnlockBuffer();

	if (bSelectPastedCells)
		m_pGrid->SetSelectedRange(PasteRange, TRUE);
	m_pGrid->Refresh();

	return TRUE;
}

void CPointLimitsDlg::ValidateAndModifyCellContents(int nRow, int nCol, LPCTSTR strText)
{
	CGridCellSpinEdit* cell = (CGridCellSpinEdit*)m_pGrid->GetCell(nRow, nCol);
	CString strCurrentText = (CString)cell->m_str;
	if (strCurrentText != strText)
	{
		cell->m_pEdit->SetWindowText(strText);
		cell->m_pEdit->m_nVal = CStringToInt(strText);
	}
}



BOOL CPointLimitsDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (IsCTRLpressed())
		{
			switch (pMsg->wParam)
			{
			case 'C':
				OnEditCopy();
				return TRUE;
			case 'V':
				OnEditPaste();
				return TRUE;
			}
		}
	}
	else if (pMsg->message == WM_LBUTTONDOWN)
	{
		bOnceSelectionChanged = false;
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CPointLimitsDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	HBITMAP hbm = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BACKGROUND));
	BITMAP bm;
	HDC hdcMem = CreateCompatibleDC(dc);
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbm);
	GetObject(hbm, sizeof(bm), &bm);
	BitBlt(dc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hbmOld);
	DeleteDC(hdcMem);
}


HBRUSH CPointLimitsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkColor(RGB(190, 219, 255));
	}
	return hbr;
}


void CPointLimitsDlg::OnDeltaposSpinMinCond(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CPointLimitsDlg::AddColumn(int nRow, int nCol)
{
//	m_pGrid->SetRowCount(nRow + 1);
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
			m_pGrid->SetCellType(i, nCol, RUNTIME_CLASS(CGridCellSpinEdit));
//		}
	}
	for (int i = 1; i <= nRow; i++)
	{
//		for (int j = 1; j <= nCol; j++)
//		{
			CGridCellSpinEdit* cell = (CGridCellSpinEdit*)m_pGrid->GetCell(i, nCol);
			CRect rect;
			rect.top = 0; rect.bottom = 0; rect.left = 0; rect.right = 0;
			cell->m_pSpin->Create(UDS_ALIGNLEFT | UDS_ARROWKEYS | UDS_SETBUDDYINT | WS_VISIBLE, rect, m_pGrid, IDC_CELL_SPIN);
			cell->m_pEdit->Create(ES_RIGHT | ES_NUMBER | /*ES_READONLY |*/ WS_VISIBLE, rect, m_pGrid, IDC_CELL_EDIT);
			cell->m_pSpin->SetBuddy(cell->m_pEdit);
			cell->m_pSpin->SetRange(0, 1000);
			cell->m_pSpin->SetDelta(1);
			cell->m_pEdit->m_pParentDlg = this;
			cell->m_pEdit->m_nRow = i;
			cell->m_pEdit->m_nCol = nCol;
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


void CPointLimitsDlg::AddRow(int nRow, int nCol)
{
	m_pGrid->SetRowCount(nRow + 1);
	TCHAR temp[10];
	for (int j = 1; j <= nCol; j++)
	{
		_stprintf(temp, _T("%d"), j);
		m_pGrid->SetItemText(nRow, j, temp);
		m_pGrid->SetCellType(nRow, j, RUNTIME_CLASS(CGridCell));
		m_pGrid->SetItemState(nRow, j, m_pGrid->GetItemState(nRow, nCol) & ~GVIS_READONLY);
		m_pGrid->SetCellType(nRow, j, RUNTIME_CLASS(CGridCellSpinEdit));
	}
	for (int j = 1; j <= nCol; j++)
	{
		CGridCellSpinEdit* cell = (CGridCellSpinEdit*)m_pGrid->GetCell(nRow, j);
		CRect rect;
		rect.top = 0; rect.bottom = 0; rect.left = 0; rect.right = 0;
		cell->m_pSpin->Create(UDS_ALIGNLEFT | UDS_ARROWKEYS | UDS_SETBUDDYINT | WS_VISIBLE, rect, m_pGrid, IDC_CELL_SPIN);
		cell->m_pEdit->Create(ES_RIGHT | ES_NUMBER | /*ES_READONLY |*/ WS_VISIBLE, rect, m_pGrid, IDC_CELL_EDIT);
		cell->m_pSpin->SetBuddy(cell->m_pEdit);
		cell->m_pSpin->SetRange(0, 1000);
		cell->m_pSpin->SetDelta(1);
		cell->m_pEdit->m_pParentDlg = this;
		cell->m_pEdit->m_nRow = nRow;
		cell->m_pEdit->m_nCol = nCol;
	}
	_stprintf(temp, _T("%d"), nRow);
	m_pGrid->SetItemText(nRow, 0, temp);
	m_pGrid->SetFixedTextColor(RGB(255, 255, 255));
	SendMessage(WM_SIZE, 0, 0);
}
