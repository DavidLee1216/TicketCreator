
// TicketCreatorDlg.h : header file
//

#pragma once
#include "CHomeDlg.h"
#include "CConditionsDlg.h"
#include "CPointLimitsDlg.h"
#include "CCouponDlg.h"
#include "CMyTabCtrl.h"

class CCouponDlg;
class CTicketCreatorDlg : public CDialogEx
{
// Construction
public:
	CTicketCreatorDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TICKETCREATOR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CMyTabCtrl m_tabControl;
	CHomeDlg* m_pHomeDlg;
	CConditionsDlg* m_pConditionsDlg;
	CPointLimitsDlg* m_pPointLimitsDlg;
	CCouponDlg* m_pCouponDlg;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL DestroyWindow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
