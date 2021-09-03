#include "CMyTabCtrl.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CMyTabCtrl, CTabCtrl)
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()

CMyTabCtrl::CMyTabCtrl()
{

}
CMyTabCtrl::~CMyTabCtrl()
{

}
void CMyTabCtrl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	char        szTabText[100];
	UINT        bkColor;
	CBrush cbr(RGB(190, 219, 255));
	TC_ITEM     tci;
	LOGBRUSH m_LogBrush;

	CTabCtrl* pTabCtrl = (CTabCtrl*)GetDlgItem(IDC_TAB1);

	if (pTabCtrl->m_hWnd == lpDrawItemStruct->hwndItem)
	{
		// To determine which tab to be drawn. I assume you only have two tabs
// 		switch (lpDrawItemStruct->itemID)
// 		{
// 		case 0:
// 			cbr = new CBrush(RGB(190, 219, 255));
// 			cbr->GetLogBrush(&m_LogBrush);
// 			bkColor = m_LogBrush.lbColor;
// 			break;
// 
// 		case 1:
// 			cbr = &m_brBlue;
// 			cbr->GetLogBrush(&m_LogBrush);
// 			bkColor = m_LogBrush.lbColor;
// 			break;
// 		}
		cbr.GetLogBrush(&m_LogBrush);
		bkColor = m_LogBrush.lbColor;
		tci.mask = TCIF_TEXT;

		CDC* dc = CDC::FromHandle(lpDrawItemStruct->hDC);
		dc->FillRect(&lpDrawItemStruct->rcItem, &cbr);
		dc->SetBkColor(bkColor);

	}
// 	CTabCtrl::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
