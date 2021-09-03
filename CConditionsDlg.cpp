// CConditionsDlg.cpp : implementation file
//

#include "pch.h"
#include "TicketCreator.h"
#include "CConditionsDlg.h"
#include "TicketCreatorDlg.h"
#include "afxdialogex.h"


#define TOP_SPACE 5
#define LINE_SPACING 10
#define LEFT_MARGIN 10
#define RIGHT_MARGIN 10
#define VISIBLE_GRID_LINES 10

#define WM_DELETE_COND WM_USER+1
#define WM_TABLE_ADD_COLUMN WM_USER+3
#define WM_TABLE_ADD_ROW WM_USER+4


IMPLEMENT_DYNAMIC(CConditionsDlg, CDialog)

CConditionsDlg::CConditionsDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_COUPONS, pParent)
{
	m_nCondCnt = 1;
	for (int i = 0; i < 10; i++)
	{
		m_pOneCond[i] = NULL;
	}
	m_pParentWnd = NULL;
	m_nVScrollMax = 0;
	m_bScrollBar = false;
}

CConditionsDlg::~CConditionsDlg()
{
	for (int i=0;i<10;i++)
	{
		if (m_pOneCond[i])
			delete m_pOneCond[i];
	}

}

void CConditionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_ADD_CONDITION, m_ctrlAdd);
}


BEGIN_MESSAGE_MAP(CConditionsDlg, CDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_ADD_CONDITION, &CConditionsDlg::OnBnClickedButtonAddCondition)
	ON_WM_CREATE()
	ON_WM_VSCROLL()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CConditionsDlg message handlers


void CConditionsDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(rect);
	int rectHeight = rect.Height();
	CRect rect1, rect2;
	m_nCondHeight = 0;
	m_nOneCondHeight = 0;
	if (m_ctrlAdd)
	{
		m_ctrlAdd.GetClientRect(rect1);
		rect2.top = rect.top + TOP_SPACE;
		rect2.bottom = rect2.top + rect1.Height();
		rect2.left = rect.right/2 + rect1.Width();
		rect2.right = rect2.left + rect1.Width();
		m_ctrlAdd.MoveWindow(rect2);
		m_nCondHeight += TOP_SPACE;
		m_nCondHeight += rect2.Height();

		for (int i = 0; i < m_nCondCnt; i++)
		{
			m_nCondHeight += LINE_SPACING;
			if (m_pOneCond[i] && m_pOneCond[i]->m_hWnd)
			{
				m_pOneCond[i]->m_ctrlDel.GetWindowRect(rect1);
				int h = rect1.Height();
				m_pOneCond[i]->m_ctrlMin.GetWindowRect(rect1);
				int h1 = rect1.Height();
				if (i == 0)
				{
					rect.top = rect2.bottom + LINE_SPACING;
					rect.left = rect.left + LEFT_MARGIN;
					rect.right = rect.right - RIGHT_MARGIN;
					rect.bottom = rect.top + h + h1 + 25 + m_pOneCond[i]->m_pGrid->GetGridHeight(VISIBLE_GRID_LINES);// rect1.Height();
				}
				else
				{
					rect.top = rect.bottom + LINE_SPACING;
					rect.bottom = rect.top + h + h1 + 25 + m_pOneCond[i]->m_pGrid->GetGridHeight(VISIBLE_GRID_LINES);
				}
				m_nCondHeight += rect.Height();
				m_pOneCond[i]->MoveWindow(rect);
			}
			m_nOneCondHeight = rect.Height();
		}
		if (m_nCondHeight > rectHeight)
		{
			EnableScrollBar(TRUE);
			m_nVScrollMax = m_nCondHeight - 1;
			ResetScrollBar();
		}
		else
		{
			EnableScrollBar(FALSE);
			m_nVScrollMax = 0;
		}
	}
}


BOOL CConditionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	for (int i=0; i<m_nCondCnt; i++)
	{
		m_pOneCond[i] = new COneConditionDlg;
		m_pOneCond[i]->Create(IDD_DIALOG_ONE_CONDITION, this);
		m_pOneCond[i]->m_nId = i;
		m_pOneCond[i]->ShowWindow(SW_SHOW);
	}

	SendMessage(WM_SIZE, 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CConditionsDlg::removeAllGrid()
{
	for (int i=0; i < MAX_COND_CNT;i++)
	{
		if (m_pOneCond[i])
		{
			delete m_pOneCond[i];
			m_pOneCond[i] = 0;
		}
	}
	m_nCondCnt = 0;
}


void CConditionsDlg::OnBnClickedButtonAddCondition()
{
	if (m_nCondCnt >= MAX_COND_CNT)
	{
		TCHAR str[50];
		_stprintf(str, _T("Can't add condition. Max condition count is %d."), MAX_COND_CNT);
		AfxMessageBox(str);
		return;
	}
	m_pOneCond[m_nCondCnt] = new COneConditionDlg;
	m_pOneCond[m_nCondCnt]->Create(IDD_DIALOG_ONE_CONDITION, this);
	m_pOneCond[m_nCondCnt]->m_nId = m_nCondCnt;
	m_pOneCond[m_nCondCnt]->CreateTable(((CTicketCreatorDlg *)m_pParentWnd)->m_pHomeDlg->m_nMatchCount, ((CTicketCreatorDlg*)m_pParentWnd)->m_pHomeDlg->m_nOutcomeCount);
	m_pOneCond[m_nCondCnt]->ShowWindow(SW_SHOW);
	m_nCondCnt++;
	SendMessage(WM_SIZE, 0, 0);
}


LRESULT CConditionsDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == WM_DELETE_COND)
	{
		delete m_pOneCond[wParam];
		for (int i=wParam+1; i < m_nCondCnt; i++)
		{
			m_pOneCond[i - 1] = m_pOneCond[i];
			m_pOneCond[i - 1]->m_nId = i-1;
		}
		m_pOneCond[m_nCondCnt - 1] = 0;
		m_nCondCnt--;
		SendMessage(WM_SIZE, 0, 0);
	}
	return CDialog::WindowProc(message, wParam, lParam);
}


void CConditionsDlg::ReloadTable(int nRow, int nCol)
{
	removeAllGrid();
	m_pOneCond[m_nCondCnt] = new COneConditionDlg;
	m_pOneCond[m_nCondCnt]->Create(IDD_DIALOG_ONE_CONDITION, this);
	m_pOneCond[m_nCondCnt]->m_nId = m_nCondCnt;
	m_pOneCond[m_nCondCnt]->CreateTable(nRow, nCol);
	m_pOneCond[m_nCondCnt]->ShowWindow(SW_SHOW);
	m_nCondCnt++;
	SendMessage(WM_SIZE, 0, 0);
}

void CConditionsDlg::AddTable(int nRow, int nCol)
{
	m_pOneCond[m_nCondCnt] = new COneConditionDlg;
	m_pOneCond[m_nCondCnt]->Create(IDD_DIALOG_ONE_CONDITION, this);
	m_pOneCond[m_nCondCnt]->m_nId = m_nCondCnt;
	m_pOneCond[m_nCondCnt]->CreateTable(nRow, nCol);
	m_pOneCond[m_nCondCnt]->ShowWindow(SW_SHOW);
	m_nCondCnt++;
	SendMessage(WM_SIZE, 0, 0);
}

int CConditionsDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pParentWnd = FromHandle(lpCreateStruct->hwndParent);

	return 0;
}


void CConditionsDlg::SetCondScrollPos(int nPos)
{
// 	SCROLLINFO si;
// 	si.cbSize = sizeof(SCROLLINFO);
// 	si.fMask = SIF_POS;
// 	si.nPos = nPos;
// 	si.nMin = 0;
// 	si.nMax = 100;
// 	SetScrollInfo(SB_VERT, &si, true);
//  	SetScrollRange(SB_VERT, 0, 100, TRUE);
	SetScrollPos(SB_VERT, nPos);
}


void CConditionsDlg::EnableScrollBar(BOOL bShow)
{
	EnableScrollBarCtrl(SB_VERT, bShow);
	ShowScrollBar(SB_VERT, bShow);
	m_bScrollBar = bShow;
}


int CConditionsDlg::GetCondScrollPos(bool bGetTrackPos)
{
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);

	if (bGetTrackPos)
	{
		if (GetScrollInfo(SB_VERT, &si, SIF_TRACKPOS))
			return si.nTrackPos;
	}
	else
	{
// 		if (GetScrollInfo(SB_VERT, &si, SIF_POS))
// 			return si.nPos;
		return GetScrollPos(SB_VERT);
	}

	return 0;
}


void CConditionsDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	int scrollPos = GetCondScrollPos();
	CRect rect;
	GetClientRect(rect);
	switch (nSBCode)
	{
	case SB_LINEDOWN:
		if (scrollPos < m_nVScrollMax)
		{
			scrollPos += m_nOneCondHeight;// m_nOneCondHeight;
			SetCondScrollPos(min(scrollPos, m_nVScrollMax));
			InvalidateRect(rect);
		}
		break;
	case SB_LINEUP:
		if (scrollPos > 0)
		{
			scrollPos -= m_nOneCondHeight;
			SetCondScrollPos(max(0, scrollPos));
			InvalidateRect(rect);
		}
		break;
	case SB_PAGEDOWN:
		if (scrollPos < m_nVScrollMax)
		{
			scrollPos += m_nOneCondHeight;
			SetCondScrollPos(min(scrollPos, m_nVScrollMax));
			InvalidateRect(rect);
		}
		break;
	case SB_PAGEUP:
		if (scrollPos > 0)
		{
			scrollPos -= m_nOneCondHeight;
			SetCondScrollPos(max(0, scrollPos));
			InvalidateRect(rect);
		}
		break;
	case SB_THUMBTRACK:
		int scrollPos1 = GetCondScrollPos(true);
		SetCondScrollPos(scrollPos1);
		SendMessage(WM_PAINT);
		break;
	}
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CConditionsDlg::ResetScrollBar()
{
	CRect rect; GetWindowRect(rect);
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_RANGE;
	si.nPage = (m_nVScrollMax > 0) ? rect.Height() : 0;
	si.nMin = 0;
	si.nMax = m_nVScrollMax;// m_nVScrollMax;
	si.nPos = 0;
	SetScrollInfo(SB_VERT, &si, TRUE);
}


void CConditionsDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	OnDraw(&dc);
	HBITMAP hbm = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BACKGROUND));
	BITMAP bm;
	HDC hdcMem = CreateCompatibleDC(dc);
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbm);
	GetObject(hbm, sizeof(bm), &bm);
	BitBlt(dc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hbmOld);
	DeleteDC(hdcMem);
}


void CConditionsDlg::OnDraw(CDC* pDC)
{
	CRect rect;
	GetClientRect(rect);
	int nPos = GetCondScrollPos();
	rect.top -= nPos;

// 	if(limit>0)
// 		nPos *= (m_nVScrollMax / limit);
	int rectHeight = rect.Height();
	int nCondHeight = 0;
	CRect rect1, rect2;
	m_ctrlAdd.GetClientRect(rect1);
	rect2.top = rect.top + TOP_SPACE;
	rect2.bottom = rect2.top + rect1.Height();
	rect2.left = rect.right/2 + rect1.Width();
	rect2.right = rect2.left + rect1.Width();
	m_ctrlAdd.MoveWindow(rect2);
	nCondHeight += TOP_SPACE;
	nCondHeight += rect2.Height();

	for (int i = 0; i < m_nCondCnt; i++)
	{
		nCondHeight += LINE_SPACING;
		if (m_pOneCond[i] && m_pOneCond[i]->m_hWnd)
		{
			m_pOneCond[i]->m_ctrlDel.GetWindowRect(rect1);
			int h = rect1.Height();
			m_pOneCond[i]->m_ctrlMin.GetWindowRect(rect1);
			int h1 = rect1.Height();
			if (i == 0)
			{
				rect.top = rect2.bottom + LINE_SPACING;
				rect.left = rect.left + LEFT_MARGIN;
				rect.right = rect.right - RIGHT_MARGIN;
				rect.bottom = rect.top + h + h1 + 25 + m_pOneCond[i]->m_pGrid->GetGridHeight(VISIBLE_GRID_LINES);// rect1.Height();
			}
			else
			{
				rect.top = rect.bottom + LINE_SPACING;
				rect.bottom = rect.top + h + h1 + 25 + m_pOneCond[i]->m_pGrid->GetGridHeight(VISIBLE_GRID_LINES);
			}
// 			if (nCondHeight < nPos)
// 				m_pOneCond[i]->ShowWindow(SW_HIDE);
// 			else
				m_pOneCond[i]->ShowWindow(SW_SHOW);
			nCondHeight += rect.Height();
			m_pOneCond[i]->MoveWindow(rect);
		}
	}
}


void CConditionsDlg::AddColumn(int nRow, int nCol)
{
	for (int i = 0; i < m_nCondCnt; i++)
	{
		m_pOneCond[i]->AddColumn(nRow, nCol);
	}
}


void CConditionsDlg::AddRow(int nRow, int nCol)
{
	for (int i = 0; i < m_nCondCnt; i++)
	{
		m_pOneCond[i]->AddRow(nRow, nCol);
	}
}
