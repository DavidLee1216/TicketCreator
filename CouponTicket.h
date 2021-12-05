#pragma once
#include <afxwin.h>
// #include "CCouponDlg.h"
#include "pch.h"

struct st_byte_array;
class OneCoupon;

class CouponTicket :
	public CWnd
{
	DECLARE_DYNAMIC(CouponTicket)

public:
	CouponTicket();
	CouponTicket(int num, st_byte_array** coupon, int line);
	~CouponTicket();
	BOOL Create(const RECT& rect, CWnd* parent, UINT nID,
		DWORD dwStyle = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE);
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	st_byte_array** m_pCoupon;
	int m_num;
	int m_nlineCount;
	int m_nWindowWidth;
	int m_nWindowHeight;
	void OnDraw(CDC* pDC);
	void OnPrintDraw(CDC* pDC, CPrintInfo* pInfo, int left, int top, int width);
	int m_nSingleLineCnt;
	void addCoupon(int num, OneCoupon* coupon, int line);
	void resetCoupon();
	CWnd* m_pParent;
	double m_dblScore;
};

