#pragma once
#include <afxwin.h>
#include "CouponTicket.h"
#include "CCouponDlg.h"
#include "pch.h"

struct st_byte_array;
class OneCoupon;
class CouponTicket;


class CouponView :
	public CView
{
	DECLARE_DYNAMIC(CouponView)

public:
	CouponView(CWnd* pParent = nullptr);
	CouponView(st_byte_array*** coupon, int nCouponCnt, int lineCnt);
	~CouponView();
	BOOL Create(const RECT& rect, CWnd* parent, UINT nID,
		DWORD dwStyle = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	int m_nCouponCnt;
	int m_nLineCnt;
	CouponTicket* m_pTickets;
	void EnableScrollBar(BOOL bShow);
	void SetViewScrollPos(int pos);
	int GetViewScrollPos(bool bGetTrackPos = 0);
	void ResetScrollBar();
	void OnDraw(CDC* pDC);
	void OnPrintDraw(CDC* pDC, CPrintInfo* pInfo);
	int OnPrintCalcPage(CDC* pDC, CPrintInfo* pInfo);
	void reloadCoupons(OneCoupon* coupon, int nCouponCnt, int lineCnt, int singleCnt, TCHAR* time);
	void makeTickets(OneCoupon* coupon, int nCouponCnt, int lineCnt, int singleCnt, TCHAR* time);
	TCHAR m_strTime[100];
	int m_nSingleLineCnt;
	int m_nVScrollMax;
	int m_bScrollBar;
	int m_nScrollPos;
	CDC* m_pPrintDC;
	LPPRINTDLG m_pPD;
	afx_msg void OnFilePrint();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnFilePrintSetup();
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	CDC* CreatePrintDC();
};

