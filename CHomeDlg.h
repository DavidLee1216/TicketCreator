#pragma once

#include "GridCtrl_src/GridCtrl.h"
#include "resource.h"

class CHomeDlg : public CDialog
{
	DECLARE_DYNAMIC(CHomeDlg)

public:
	CHomeDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CHomeDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_HOME };
	CGridCtrl* m_pGrid;
	CWnd* m_pParentWnd;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	void setDefaultGrid();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	int m_nMatchCount;
	int m_nOutcomeCount;
	afx_msg void OnBnClickedButtonCreatTable();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	void ReloadTable(int nRow, int nCol);
	afx_msg void OnBnClickedButtonAddColumn();
	afx_msg void OnBnClickedButtonAddRow();
};
