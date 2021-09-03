#pragma once

#include "GridCtrl_src/GridCtrl.h"
#include "COneConditionDlg.h"
// CConditionsDlg dialog
#define MAX_COND_CNT 10

class CConditionsDlg : public CDialog
{
	DECLARE_DYNAMIC(CConditionsDlg)

public:
	CConditionsDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CConditionsDlg();
	COneConditionDlg* m_pOneCond[MAX_COND_CNT];
	CWnd* m_pParentWnd;
	int m_nCondCnt;

// Dialog Data
	enum { IDD = IDD_DIALOG_CONDITIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	void removeAllGrid();
	afx_msg void OnBnClickedButtonAddCondition();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void ReloadTable(int nRow, int nCol);
	void AddTable(int nRow, int nCol);
	CButton m_ctrlAdd;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void SetCondScrollPos(int nPos);
	int m_nCondHeight;
	void EnableScrollBar(BOOL bShow);
	int m_nOneCondHeight;
	int GetCondScrollPos(bool bGetTrackPos=0);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	int m_nVScrollMax;
	void ResetScrollBar();
	bool m_bScrollBar;
	afx_msg void OnPaint();
	void OnDraw(CDC* pDC);
	void AddColumn(int nRow, int nCol);
	void AddRow(int nRow, int nCol);
};
