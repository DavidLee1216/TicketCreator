#include "CouponView.h"
#include "framework.h"
#include "GridCtrl_src/MemDC.h"

#define ID_COUPON_VIEW 1234

IMPLEMENT_DYNAMIC(CouponView, CView)

CouponView::CouponView(CWnd *pParent)
{
	m_pTickets = 0;
	m_nVScrollMax = 0;
	m_nScrollPos = 0;
	m_bScrollBar = false;
	m_strTime[0] = 0;
	m_pPrintDC = NULL;
	m_pPD = NULL;
}
CouponView::CouponView(st_byte_array*** coupon, int nCouponCnt, int lineCnt)
{
// 	makeTickets(coupon, nCouponCnt, lineCnt);
}
CouponView::~CouponView()
{
	if (m_pTickets)
		delete[] m_pTickets;
}
BOOL CouponView::Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwStyle)
{
	if (!CWnd::Create(NULL, NULL, dwStyle, rect, pParentWnd, nID))
		return FALSE;
	return TRUE;
}

BEGIN_MESSAGE_MAP(CouponView, CWnd)
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_PRINT, &CouponView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CouponView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CouponView::OnFilePrintSetup)
END_MESSAGE_MAP()


void CouponView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int scrollPos = GetViewScrollPos();
	CRect rect;
	GetClientRect(rect);
	switch (nSBCode)
	{
	case SB_LINEDOWN:
		if (scrollPos < m_nVScrollMax)
		{
			scrollPos += 10;// m_nOneCondHeight;
			SetViewScrollPos(min(scrollPos, m_nVScrollMax));
			InvalidateRect(rect);
		}
		break;
	case SB_LINEUP:
		if (scrollPos > 0)
		{
			scrollPos -= 10;
			SetViewScrollPos(max(0, scrollPos));
			InvalidateRect(rect);
		}
		break;
	case SB_PAGEDOWN:
		if (scrollPos < m_nVScrollMax)
		{
			scrollPos += 50;
			SetViewScrollPos(min(scrollPos, m_nVScrollMax));
			InvalidateRect(rect);
		}
		break;
	case SB_PAGEUP:
		if (scrollPos > 0)
		{
			scrollPos -= 50;
			SetViewScrollPos(max(0, scrollPos));
			InvalidateRect(rect);
		}
		break;
	case SB_THUMBTRACK:
		int scrollPos1 = GetViewScrollPos(true);
		SetViewScrollPos(scrollPos1);
		InvalidateRect(rect);
// 		SendMessage(WM_PAINT);
		break;
	}

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CouponView::SetViewScrollPos(int nPos)
{
	SetScrollPos(SB_VERT, nPos);
}

int CouponView::GetViewScrollPos(bool bGetTrackPos)
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
		return GetScrollPos(SB_VERT);
	}

	return 0;
}

void CouponView::ResetScrollBar()
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

void CouponView::EnableScrollBar(BOOL bShow)
{
	EnableScrollBarCtrl(SB_VERT, bShow);
	ShowScrollBar(SB_VERT, bShow);
	m_bScrollBar = bShow;
}

void CouponView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	CRect rect, rect1;
	GetClientRect(rect);
	int nClientWidth = rect.Width();
	int nClientHeight = rect.Height();
	int pos_y = 0, pos_x = 0;
	int width = 0, height = 0;
	for (int i = 0; i < m_nCouponCnt; i++)
	{
		width = max(width, m_pTickets[i].m_nWindowWidth);
		height = max(height, m_pTickets[i].m_nWindowHeight);
	}
	if (nClientWidth < width)
		return;
	rect.top -= m_nScrollPos;
	rect1 = rect; rect1.top = rect.top + 100; rect1.bottom = rect1.top + height;
	m_nVScrollMax = 100 + height;
	for (int i = 0; i < m_nCouponCnt; i++)
	{
		if (pos_x + width > nClientWidth)
		{
			rect1.top += height; rect1.left = 2;
			rect1.bottom = rect1.top + height; rect1.right = rect1.left + width;
			pos_x = 2 + width; pos_y = rect1.top;
			m_nVScrollMax += height;
		}
		else
		{
			rect1.top = rect1.top; rect1.bottom = rect1.bottom;
			if (i == 0)
			{
				rect1.left = 2; rect1.right = rect1.left + width;
			}
			else
			{
				rect1.left = rect1.left + width + 1; rect1.right = rect1.left + width;
			}
			pos_x += width;
		}
		m_pTickets[i].m_nWindowWidth = width;
		m_pTickets[i].m_nWindowHeight = height;
		m_pTickets[i].MoveWindow(rect1);
		m_pTickets[i].ShowWindow(SW_SHOW);
	}
	if (m_nVScrollMax > nClientHeight)
	{
		EnableScrollBar(TRUE);
		ResetScrollBar();
	}
	else
	{
		EnableScrollBar(FALSE);
		m_nVScrollMax = 0;
		SetViewScrollPos(0);
	}
}


void CouponView::OnPaint()
{
	CPaintDC dc(this); 
// 	CMyMemDC MemDC(&dc);
// 	OnDraw(&MemDC);
	OnDraw(&dc);
}


int CouponView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	

	return 0;
}


LRESULT CouponView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	return CWnd::WindowProc(message, wParam, lParam);
}


void CouponView::OnDraw(CDC* pDC)
{
	CRect rect;
	GetClientRect(rect);
	pDC->SetBkMode(TRANSPARENT);
	COLORREF Clr = RGB(255, 255, 255);
	CBrush brush(Clr);
	pDC->FillRect(rect, &brush);
	if (m_nCouponCnt > 0)
	{
		if (m_bScrollBar)
			m_nScrollPos = GetViewScrollPos(false);
		else
			m_nScrollPos = 0;
		COLORREF TextClr = RGB(0, 0, 0);// ::GetSysColor(COLOR_HIGHLIGHTTEXT);
		COLORREF oldColor = pDC->SetTextColor(TextClr);
		LOGFONT lf; CFont font, font1;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = 25; lf.lfWeight = FW_BOLD; lf.lfWidth = 0;
		_tcscpy_s(lf.lfFaceName, _T("Arial"));
		font.CreateFontIndirect(&lf);
		CFont* oldfont = pDC->SelectObject(&font);
		TCHAR str[1000];
		_stprintf(str, _T(" Coupon creation time %s "), m_strTime);
		rect.top -= m_nScrollPos;
		rect.top = rect.top + 20; rect.bottom = 50; rect.left = 50; rect.right = rect.right;
		DrawText(pDC->m_hDC, str, -1, rect, DT_LEFT | DT_NOPREFIX);
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = 18; lf.lfWidth = 0;
		_tcscpy_s(lf.lfFaceName, _T("Arial"));
		font1.CreateFontIndirect(&lf);
		pDC->SelectObject(&font1);
		_stprintf(str, _T(" Coupon count:%d         Single Line Count:%d, Total Score:%.2f"), m_nCouponCnt, m_nSingleLineCnt, m_dblTotalScore);
		rect.top = rect.top + 50; rect.bottom = 100; rect.left = 50; rect.right = rect.right;
		DrawText(pDC->m_hDC, str, -1, rect, DT_LEFT | DT_NOPREFIX);
		pDC->SelectObject(&oldfont);
		font.DeleteObject();
		pDC->SetTextColor(oldColor);
		SendMessage(WM_SIZE, 0, 0);

	}
}
#define TICKET_START_HORZ_POS 100
#define LINE_HEIGHT 20
#define PRINT_TICKET_HORZ_SPACE 10
#define PRINT_TICKET_VERT_SPACE 10
void CouponView::OnPrintDraw(CDC* pDC, CPrintInfo *pInfo)
{
	CRect rect;
	GetClientRect(rect);
	int horz = pDC->GetDeviceCaps(HORZRES);
	int vert = pDC->GetDeviceCaps(VERTRES);
	int horzsize = pDC->GetDeviceCaps(HORZSIZE);
	int vertsize = pDC->GetDeviceCaps(VERTSIZE);
// 	int h = rect.Height();
// 	int w = rect.Width();
// 	float rateX = horz / (float)w;
// 	float rateY = vert / (float)h;

	LPtoDP(pDC->m_hDC, (LPPOINT)&rect, 2);
	pDC->SetBkMode(TRANSPARENT);
	COLORREF Clr = RGB(255, 255, 255);
	CBrush brush(Clr);
	pDC->FillRect(rect, &brush);
	DPtoLP(pDC->m_hDC, (LPPOINT)&rect, 2);
	int min_ticket_width = 50*10;//(70mm)
	int max_ticket_width = 100*10;
	CRect rect_temp; rect_temp.left = min_ticket_width; rect_temp.top = 0;
	rect_temp.right = max_ticket_width; rect_temp.bottom = 0;
	DPtoLP(pDC->m_hDC, (LPPOINT)&rect_temp, 2);
	min_ticket_width = rect_temp.left; max_ticket_width = rect_temp.right;
	rect_temp.left = 0; rect_temp.top = vertsize*10; rect_temp.right = horzsize*10;//pixel
	DPtoLP(pDC->m_hDC, (LPPOINT)&rect_temp, 2);
	int w = rect_temp.Width();
	int h = rect_temp.Height();
	int nHorzLen = TICKET_START_HORZ_POS;
	int nVertLen = 0;
	if (m_nCouponCnt > 0)
	{
		int nTicketFontSize1 = 160;
		int nTicketFontSize2 = 150;
		if (pInfo->m_nCurPage == 1)
		{
			int nTitleFontSize1 = 250;
			int nTitleFontSize2 = 180;
			COLORREF TextClr = RGB(0, 0, 0);// ::GetSysColor(COLOR_HIGHLIGHTTEXT);
			COLORREF oldColor = pDC->SetTextColor(TextClr);
			LOGFONT lf; CFont font, font1;
			memset(&lf, 0, sizeof(LOGFONT));
			lf.lfHeight = nTitleFontSize1; lf.lfWeight = FW_BOLD; lf.lfWidth = 0;
// 			lf.lfHeight = nTitleFontSize1 *rateY; lf.lfWeight = FW_BOLD; lf.lfWidth = 0;
			_tcscpy_s(lf.lfFaceName, _T("Arial"));
			font.CreatePointFontIndirect(&lf);
			CFont* oldfont = pDC->SelectObject(&font);
			TCHAR str[1000];
			_stprintf(str, _T(" Coupon creation time %s "), m_strTime);
			rect.top = (rect.top + 100); rect.bottom = (rect.top + nTitleFontSize1/10); rect.left = 100; rect.right = w;
// 			rect.top = (rect.top + 100)* rateY; rect.bottom = (100 + nTitleFontSize1)*rateY; rect.left = nTitleFontSize1 *rateX; rect.right = rect.left+w*rateX;
			LPtoDP(pDC->m_hDC, (LPPOINT)&rect, 2);
			DrawText(pDC->m_hDC, str, -1, rect, DT_LEFT | DT_NOPREFIX);
			DPtoLP(pDC->m_hDC, (LPPOINT)&rect, 2);
			memset(&lf, 0, sizeof(LOGFONT));
			lf.lfHeight = nTitleFontSize2; lf.lfWidth = 0;
// 			lf.lfHeight = nTitleFontSize2 *rateY; lf.lfWidth = 0;
			_tcscpy_s(lf.lfFaceName, _T("Arial"));
			font1.CreatePointFontIndirect(&lf);
			pDC->SelectObject(&font1);
			_stprintf(str, _T(" Coupon count:%d         Single Line Count:%d, Total Score:%.2f"), m_nCouponCnt, m_nSingleLineCnt, m_dblTotalScore);
			rect.top = rect.top + nTitleFontSize1/10; rect.bottom = rect.top+ nTitleFontSize2/10; rect.left = 100; rect.right = w;
// 			rect.top = rect.top + nTitleFontSize1 *rateY; rect.bottom = rect.top+ nTitleFontSize2 *rateY; rect.left = nTitleFontSize1 *rateX; rect.right = rect.left + w*rateX;
			LPtoDP(pDC->m_hDC, (LPPOINT)&rect, 2);
			DrawText(pDC->m_hDC, str, -1, rect, DT_LEFT | DT_NOPREFIX);
			DPtoLP(pDC->m_hDC, (LPPOINT)&rect, 2);
			nVertLen = rect.bottom+30;
			pDC->SelectObject(&oldfont);
			font.DeleteObject();
			pDC->SetTextColor(oldColor);

			int ticket_width = 0;
			for (int i = 0; i < m_nCouponCnt; i++)
			{
				ticket_width = max(m_pTickets[i].m_nWindowWidth, ticket_width);
			}
			ticket_width = max(ticket_width, min_ticket_width);
			ticket_width = min(ticket_width, max_ticket_width);
			for (int i = 0; i < m_nCouponCnt; i++)
			{
				CRect rect1;
				if (nHorzLen + ticket_width > w)
				{
					nHorzLen = TICKET_START_HORZ_POS;
					nVertLen += m_pTickets[i - 1].m_nWindowHeight+ PRINT_TICKET_VERT_SPACE;
					if(nVertLen + m_pTickets[i].m_nWindowHeight > h)
						break;
				}
				m_pTickets[i].OnPrintDraw(pDC, pInfo, nHorzLen, nVertLen, ticket_width);
				nHorzLen += (ticket_width+ PRINT_TICKET_HORZ_SPACE);
			}
		}
		else
		{
			int nPage = 1;
			int ticket_width = 0;
			for (int i = 0; i < m_nCouponCnt; i++)
			{
				ticket_width = max(m_pTickets[i].m_nWindowWidth, ticket_width);
			}
			ticket_width = max(ticket_width, min_ticket_width);
			ticket_width = min(ticket_width, max_ticket_width);
			for (int i = 0; i < m_nCouponCnt; i++)
			{
				CRect rect1;
				if (nHorzLen + ticket_width > w)
				{
					nHorzLen = TICKET_START_HORZ_POS;
					nVertLen += m_pTickets[i - 1].m_nWindowHeight+ PRINT_TICKET_VERT_SPACE;
					if (nVertLen + m_pTickets[i].m_nWindowHeight > h)
					{
						nPage++;
						nVertLen = 100;
					}
				}
				if(pInfo->m_nCurPage==nPage)
					m_pTickets[i].OnPrintDraw(pDC, pInfo, nHorzLen, nVertLen, ticket_width);
				else if(pInfo->m_nCurPage < nPage)
					break;
				nHorzLen += (ticket_width+ PRINT_TICKET_HORZ_SPACE);
			}

		}
	}
}

int CouponView::OnPrintCalcPage(CDC* pDC, CPrintInfo *pInfo)
{
	CRect rect;
	GetClientRect(rect);
	int horz = pDC->GetDeviceCaps(HORZRES);
	int vert = pDC->GetDeviceCaps(VERTRES);
	int horzsize = pDC->GetDeviceCaps(HORZSIZE);
	int vertsize = pDC->GetDeviceCaps(VERTSIZE);

	LPtoDP(pDC->m_hDC, (LPPOINT)&rect, 2);
	pDC->SetBkMode(TRANSPARENT);
	COLORREF Clr = RGB(255, 255, 255);
	DPtoLP(pDC->m_hDC, (LPPOINT)&rect, 2);
	int min_ticket_width = 50*10;//(70mm)
	int max_ticket_width = 100*10;
	CRect rect_temp; rect_temp.left = min_ticket_width; rect_temp.top = 0;
	rect_temp.right = max_ticket_width; rect_temp.bottom = 0;
	DPtoLP(pDC->m_hDC, (LPPOINT)&rect_temp, 2);
	min_ticket_width = rect_temp.left; max_ticket_width = rect_temp.right;
	rect_temp.left = 0; rect_temp.top = vertsize*10; rect_temp.right = horzsize*10;//pixel
	DPtoLP(pDC->m_hDC, (LPPOINT)&rect_temp, 2);
	int w = rect_temp.Width();
	int h = rect_temp.Height();
	int nHorzLen = TICKET_START_HORZ_POS;
	int nVertLen = 0;
	if (m_nCouponCnt > 0)
	{
		int nTicketFontSize1 = 160;
		int nTicketFontSize2 = 150;
		int nTitleFontSize1 = 250;
		int nTitleFontSize2 = 180;
		rect.top = (rect.top + 100); rect.bottom = (rect.top + nTitleFontSize1 / 10); rect.left = 100; rect.right = w;
		rect.top = rect.top + nTitleFontSize1 / 10; rect.bottom = rect.top + nTitleFontSize2 / 10; rect.left = 100; rect.right = w;
		nVertLen = rect.bottom+30;

		int ticket_width = 0;
		for (int i = 0; i < m_nCouponCnt; i++)
		{
			ticket_width = max(m_pTickets[i].m_nWindowWidth, ticket_width);
		}
		ticket_width = max(ticket_width, min_ticket_width);
		ticket_width = min(ticket_width, max_ticket_width);
		int nFirstPageTicketNum = 0;
		for (int i = 0; i < m_nCouponCnt; i++)
		{
			CRect rect1;
			if (nHorzLen + ticket_width > w)
			{
				nHorzLen = TICKET_START_HORZ_POS;
				nVertLen += m_pTickets[i - 1].m_nWindowHeight+ PRINT_TICKET_VERT_SPACE;
				nFirstPageTicketNum = i;
				if (nVertLen + m_pTickets[i].m_nWindowHeight > h)
					break;
			}
			nHorzLen += (ticket_width+ PRINT_TICKET_HORZ_SPACE);
		}

		int nPage = 1;
		for (int i = nFirstPageTicketNum; i < m_nCouponCnt; i++)
		{
			CRect rect1;
			if (nHorzLen + ticket_width > w)
			{
				nHorzLen = TICKET_START_HORZ_POS;
				nVertLen += m_pTickets[i - 1].m_nWindowHeight+ PRINT_TICKET_VERT_SPACE;
				if (nVertLen + m_pTickets[i].m_nWindowHeight > h)
				{
					nPage++;
					nVertLen = 100;
				}
			}
			nHorzLen += (ticket_width+ PRINT_TICKET_HORZ_SPACE);
		}
		return nPage;
	}
}

void CouponView::reloadCoupons(OneCoupon* coupon, int nCouponCnt, int lineCnt, int singleCnt, TCHAR* time)
{
	if(m_pTickets)
		delete[] m_pTickets;
	m_nCouponCnt = 0;
	m_dblTotalScore = 0;
	makeTickets(coupon, nCouponCnt, lineCnt, singleCnt, time);
}


void CouponView::makeTickets(OneCoupon* coupon, int nCouponCnt, int lineCnt, int singleCnt, TCHAR *time)
{
	m_nLineCnt = lineCnt;
	m_nScrollPos = 0;
	m_nSingleLineCnt = singleCnt;
	_tcscpy_s(m_strTime, time);
	m_pTickets = new CouponTicket[nCouponCnt];
	for (int i = 0; i < nCouponCnt; i++)
	{
		m_pTickets[i].Create(CRect(0, 0, 20, 20), this, 1250 + i);
		m_nCouponCnt++;
		m_pTickets[i].addCoupon(i+1, coupon+i, lineCnt);
		m_pTickets[i].ShowWindow(SW_HIDE);
		m_dblTotalScore += coupon[i].score;
	}
	Invalidate(TRUE);
}


void CouponView::OnFilePrint()
{
	CPrintDialog printDlg(FALSE);
	if (printDlg.DoModal() == IDCANCEL) return;

	m_pPD = &printDlg.m_pd;

	m_pPrintDC = CreatePrintDC();
	m_pPrintDC->m_bPrinting = TRUE;
	m_pPrintDC->SetMapMode(MM_LOMETRIC);
// 	m_pPrintDC->SetMapMode(MM_HIMETRIC);
// 	m_pPrintDC->SetMapMode(MM_TWIPS);

	CString strTitle;
	strTitle.LoadString(AFX_IDS_APP_TITLE);
	DOCINFO di;
	::ZeroMemory(&di, sizeof(DOCINFO));
	di.cbSize = sizeof(DOCINFO);
	di.lpszDocName = strTitle;

	BOOL bPrintingOK = m_pPrintDC->StartDoc(&di);

	CPrintInfo Info;
	Info.m_rectDraw.SetRect(0, 0, m_pPrintDC->GetDeviceCaps(HORZRES),
		m_pPrintDC->GetDeviceCaps(VERTRES));
// 	dc.DPtoLP(&Info.m_rectDraw);
	OnPreparePrinting(&Info);
	int nPage = OnPrintCalcPage(m_pPrintDC, &Info);
	Info.SetMaxPage(nPage);

	OnBeginPrinting(m_pPrintDC, &Info);

	for (UINT page = Info.GetMinPage();
		page <= Info.GetMaxPage() && bPrintingOK; page++)
	{
		m_pPrintDC->StartPage();
		Info.m_nCurPage = page;

		OnPrint(m_pPrintDC, &Info);

		bPrintingOK = (m_pPrintDC->EndPage() > 0);
	}

	OnEndPrinting(m_pPrintDC, &Info);

	if (bPrintingOK) m_pPrintDC->EndDoc();
	else m_pPrintDC->AbortDoc();

	m_pPrintDC->Detach();
}


void CouponView::OnFilePrintPreview()
{
	// TODO: Add your command handler code here
}


void CouponView::OnFilePrintSetup()
{
	// TODO: Add your command handler code here
}


void CouponView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	CView::OnEndPrinting(pDC, pInfo);
}


void CouponView::OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView)
{
	// TODO: Add your specialized code here and/or call the base class

	CView::OnEndPrintPreview(pDC, pInfo, point, pView);
}


void CouponView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	CView::OnPrepareDC(pDC, pInfo);
}


BOOL CouponView::OnPreparePrinting(CPrintInfo* pInfo)
{

	// TODO:  call DoPreparePrinting to invoke the Print dialog box
// 	CDC dc;
// 	dc.Attach(pInfo->m_pPD->GetPrinterDC());
// 	int horz = dc.GetDeviceCaps(HORZRES);
// 	int vert = dc.GetDeviceCaps(VERTRES);
// 
// 	CRect rect;
// 	GetClientRect(rect);
// 
// 	pInfo->m_rectDraw.left = pInfo->m_rectDraw.left * rect.Width() / horz;
	pInfo->m_rectDraw.left += 100;
	pInfo->m_rectDraw.right -= 100;
	pInfo->m_rectDraw.top += 100;
	pInfo->m_rectDraw.bottom -= 100;
	pInfo->m_rectDraw.NormalizeRect();
	m_pPrintDC->DPtoLP(&pInfo->m_rectDraw);
	return CView::OnPreparePrinting(pInfo);
}


void CouponView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	OnPrintDraw(pDC, pInfo);

	CView::OnPrint(pDC, pInfo);
}


void CouponView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	CView::OnBeginPrinting(pDC, pInfo);
}


CDC* CouponView::CreatePrintDC()
{
	if (m_pPD)
	{
		CDC* pDC = new CDC();
		pDC->Attach(m_pPD->hDC);
		return pDC;
	}

	return NULL;
}
