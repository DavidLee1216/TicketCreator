#include "CouponTicket.h"
#include "CCouponDlg.h"
#include "framework.h"

#define LINE_HEIGHT 20
#define DEFAULT_TICKET_WIDTH 120
#define DEFAULT_TICKET_HEIGHT 90

IMPLEMENT_DYNAMIC(CouponTicket, CWnd)

BEGIN_MESSAGE_MAP(CouponTicket, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()

CouponTicket::CouponTicket()
{
	m_pCoupon = 0;
	m_nlineCount = 0;
	m_num = 0;
	m_nWindowHeight = DEFAULT_TICKET_HEIGHT;
	m_nWindowWidth = DEFAULT_TICKET_WIDTH;
}
CouponTicket::~CouponTicket()
{
	if(m_pCoupon)
		resetCoupon();
}
CouponTicket::CouponTicket(int num, st_byte_array** coupon, int line)
{
	m_num = num;
	m_pCoupon = coupon;
	m_nlineCount = line;
	m_nSingleLineCnt = 1;
	for (int i = 0; i < line; i++)
	{
		m_nSingleLineCnt *= coupon[i]->cnt;
	}
	m_nWindowWidth = DEFAULT_TICKET_WIDTH;
	m_nWindowHeight = max(DEFAULT_TICKET_HEIGHT, LINE_HEIGHT * (line + 1));
}

BOOL CouponTicket::Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwStyle)
{
	m_pParent = pParentWnd;
	if (!CWnd::Create(NULL, NULL, dwStyle, rect, pParentWnd, nID))
		return FALSE;

}

int CouponTicket::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;


	return 0;
}


void CouponTicket::OnPaint()
{
	CPaintDC dc(this);
	OnDraw(&dc);
}


void CouponTicket::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	MoveWindow(0, 0, m_nWindowWidth, m_nWindowHeight);
}



LRESULT CouponTicket::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	return CWnd::WindowProc(message, wParam, lParam);
}


void CouponTicket::OnDraw(CDC* pDC)
{
	CRect rect;
	GetClientRect(rect);
	pDC->SetBkMode(TRANSPARENT);
	CRect rect1;
	rect1.left = 0; rect1.right = rect.right; rect1.top = 0; rect1.bottom = LINE_HEIGHT;
// 	COLORREF Clr = RGB(0, 0, 0);
	COLORREF Clr = RGB(213, 212, 210);
	COLORREF TextClr = RGB(0, 0, 0);// ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	CBrush brush(Clr);
	pDC->FillRect(rect1, &brush);
	
	COLORREF oldColor = pDC->SetTextColor(TextClr);
	LOGFONT lf; CFont font, font1;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 16; lf.lfWeight = FW_BOLD; lf.lfWidth = 0; lf.lfItalic = TRUE;
	_tcscpy_s(lf.lfFaceName, _T("Arial"));
	font.CreateFontIndirect(&lf);
 	CFont* oldfont = pDC->SelectObject(&font);
	TCHAR str[1000];
	_stprintf(str, _T("  Coupon No. %d - SingleLine %d  "), m_num, m_nSingleLineCnt);
	DrawText(pDC->m_hDC, str, -1, rect1, DT_CALCRECT | DT_CENTER | DT_NOPREFIX);
	m_nWindowWidth = max(m_nWindowWidth, rect1.Width());
	DrawText(pDC->m_hDC, str, -1, rect1, DT_CENTER|DT_NOPREFIX);
	font.DeleteObject();
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 15; lf.lfWidth = 0;
	_tcscpy_s(lf.lfFaceName, _T("Arial"));
	font1.CreateFontIndirect(&lf);
	pDC->SelectObject(&font1);
	for (int i = 0; i < m_nlineCount; i++)
	{
		str[0] = 0;
		if (m_pCoupon[i]->common_value > 0)
			_stprintf(str, _T(" %d/"), m_pCoupon[i]->common_value);
		for (int j = 0; j < m_pCoupon[i]->cnt; j++)
		{
			_stprintf(str, _T("%s %d"), str, m_pCoupon[i]->arr[j]);
		}
		rect1.top += LINE_HEIGHT; rect1.bottom += LINE_HEIGHT;
		DrawText(pDC->m_hDC, str, -1, rect1, DT_CALCRECT | DT_CENTER | DT_NOPREFIX);
		m_nWindowWidth = max(m_nWindowWidth, rect1.Width());
		DrawText(pDC->m_hDC, str, -1, rect1, DT_CENTER | DT_NOPREFIX);
	}
	font1.DeleteObject();
	pDC->SetTextColor(oldColor);
 	pDC->SelectObject(oldfont);
	CPen penShadow(PS_SOLID, 0, ::GetSysColor(COLOR_3DSHADOW));
	CPen penLight(PS_SOLID, 0, ::GetSysColor(COLOR_3DHILIGHT));
	CPen* pOldPen = (CPen*)pDC->SelectObject(&penLight);
	pDC->MoveTo(2, 2);
	pDC->LineTo(m_nWindowWidth-1, 2);
	pDC->LineTo(m_nWindowWidth-1, m_nWindowHeight-1);
	pDC->LineTo(2, m_nWindowHeight - 1);
	pDC->LineTo(2, 2);

	pDC->SelectObject(&penShadow);
	pDC->MoveTo(1, 1);
	pDC->LineTo(m_nWindowWidth - 2, 1);
	pDC->LineTo(m_nWindowWidth - 2, m_nWindowHeight - 2);
	pDC->LineTo(1, m_nWindowHeight - 2);
	pDC->LineTo(1, 1);
	pDC->SelectObject(pOldPen);

// 	m_pParent->SendMessage(WM_SIZE, 0, 0);
}

void CouponTicket::OnPrintDraw(CDC* pDC, CPrintInfo* pInfo, int left, int top, int width)
{
	CRect rect;
	CRect rect1;
	rect1.left = left; rect1.right = rect1.left + width; rect1.top = top; rect1.bottom = rect1.top + LINE_HEIGHT;
	LPtoDP(pDC->m_hDC, (LPPOINT)&rect1, 2);
	// 	COLORREF Clr = RGB(0, 0, 0);
	COLORREF Clr = RGB(213, 212, 210);
	COLORREF TextClr = RGB(0, 0, 0);// ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	CBrush brush(Clr);
	pDC->FillRect(rect1, &brush);

	COLORREF oldColor = pDC->SetTextColor(TextClr);
	LOGFONT lf; CFont font, font1;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 160; lf.lfWeight = FW_BOLD; lf.lfWidth = 0; lf.lfItalic = TRUE;
// 	lf.lfHeight = 160*rateY; lf.lfWeight = FW_BOLD; lf.lfWidth = 0; lf.lfItalic = TRUE;
	_tcscpy_s(lf.lfFaceName, _T("Arial"));
	font.CreatePointFontIndirect(&lf);
	CFont* oldfont = pDC->SelectObject(&font);
	TCHAR str[1000];
	_stprintf(str, _T("  Coupon No. %d - SingleLine %d  "), m_num, m_nSingleLineCnt);
	DrawText(pDC->m_hDC, str, -1, rect1, DT_BOTTOM | DT_SINGLELINE| DT_CENTER | DT_NOPREFIX);
	font.DeleteObject();
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 150; lf.lfWidth = 0;
// 	lf.lfHeight = 15*rateY; lf.lfWidth = 0;
	_tcscpy_s(lf.lfFaceName, _T("Arial"));
	font1.CreatePointFontIndirect(&lf);
	pDC->SelectObject(&font1);
	for (int i = 0; i < m_nlineCount; i++)
	{
		DPtoLP(pDC->m_hDC, (LPPOINT)&rect1, 2);
		str[0] = 0;
		if (m_pCoupon[i]->common_value > 0)
			_stprintf(str, _T(" %d/"), m_pCoupon[i]->common_value);
		for (int j = 0; j < m_pCoupon[i]->cnt; j++)
		{
			_stprintf(str, _T("%s %d"), str, m_pCoupon[i]->arr[j]);
		}
		rect1.top += LINE_HEIGHT; rect1.bottom += LINE_HEIGHT;
// 		rect1.top += LINE_HEIGHT * rateY; rect1.bottom += LINE_HEIGHT * rateY;
		LPtoDP(pDC->m_hDC, (LPPOINT)&rect1, 2);
		DrawText(pDC->m_hDC, str, -1, rect1, DT_CENTER | DT_NOPREFIX);
	}
	DPtoLP(pDC->m_hDC, (LPPOINT)&rect1, 2);
	font1.DeleteObject();
	pDC->SetTextColor(oldColor);
	pDC->SelectObject(oldfont);
	CPen penShadow(PS_SOLID, 0, ::GetSysColor(COLOR_3DSHADOW));
	CPen penLight(PS_SOLID, 0, ::GetSysColor(COLOR_3DHILIGHT));
	CPen* pOldPen = (CPen*)pDC->SelectObject(&penLight);
	rect.left = left + 2; rect.top = top + 2;
	rect.right = rect1.right - 1; rect.bottom = top + m_nWindowHeight - 1;
// 	rect.left = left + 2 * rateX; rect1.top = top + 2 * rateY;
// 	rect.right = rect1.right - 1 * rateX; rect.bottom = top + m_nWindowHeight * rateY - 1 * rateY;
	LPtoDP(pDC->m_hDC, (LPPOINT)&rect, 2);
	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.right, rect.top);
	pDC->LineTo(rect.right, rect.bottom);
	pDC->LineTo(rect.left, rect.bottom);
	pDC->LineTo(rect.left, rect.top);

	pDC->SelectObject(&penShadow);
	rect.left = left + 1; rect.top = top + 1;
	rect.right = rect1.right - 2; rect.bottom = top + m_nWindowHeight - 2;
	LPtoDP(pDC->m_hDC, (LPPOINT)&rect, 2);
	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.right, rect.top);
	pDC->LineTo(rect.right, rect.bottom);
	pDC->LineTo(rect.left, rect.bottom);
	pDC->LineTo(rect.left, rect.top);
	pDC->SelectObject(pOldPen);
}
void CouponTicket::addCoupon(int num, OneCoupon* coupon, int line)
{
	if (m_pCoupon)
	{
		resetCoupon();
	}
	m_pCoupon = new st_byte_array * [line];
	for (int i = 0; i < line; i++)
	{
		m_pCoupon[i] = new st_byte_array;
		m_pCoupon[i]->common_value = coupon->byteRowValues[i]->common_value;
		m_pCoupon[i]->arr = new BYTE[coupon->byteRowValues[i]->cnt];
		m_pCoupon[i]->cnt = coupon->byteRowValues[i]->cnt;
		for (int j = 0; j < m_pCoupon[i]->cnt; j++)
		{
			m_pCoupon[i]->arr[j] = coupon->byteRowValues[i]->arr[j];
		}
	}
	m_num = num;
	m_nlineCount = line;
	m_nSingleLineCnt = coupon->nSingleLineCnt;
	m_nWindowWidth = DEFAULT_TICKET_WIDTH;
	m_nWindowHeight = max(DEFAULT_TICKET_HEIGHT, LINE_HEIGHT * (line + 1));

	SendMessage(WM_PAINT, 0, 0);
// 	m_pParent->SendMessage(WM_SIZE, 0, 0);
}


void CouponTicket::resetCoupon()
{
	for (int i = 0; i < m_nlineCount; i++)
	{
		delete m_pCoupon[i]->arr;
		delete m_pCoupon[i];
	}
	delete[] m_pCoupon;
}
