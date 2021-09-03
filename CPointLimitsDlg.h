#pragma once


#include "GridCtrl_src/GridCtrl.h"
#include "NumSpinCtrl.h"
#include "NumEdit.h"

class CPointLimitsDlg : public CDialog
{
	DECLARE_DYNAMIC(CPointLimitsDlg)

public:
	CPointLimitsDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPointLimitsDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_POINT_LIMITS };

	CWnd* m_pParentWnd;
	CGridCtrl* m_pGrid;

	CStatic m_ctrlStaticMin;
	CStatic m_ctrlStaticMax;
	int m_nMin;
	int m_nMax;
	CNumEdit m_ctrlMin;
	CNumEdit m_ctrlMax;
	CNumSpinCtrl m_ctrlMinSpin;
	CNumSpinCtrl m_ctrlMaxSpin;
	int m_nppPointMatrix[20+1][30+1];
	bool bOnceSelectionChanged;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEditMinCond();
	afx_msg void OnEnChangeEditMaxCond();
	void setDefaultGrid();
	void ReloadTable(int nRow, int nCol);
	afx_msg void OnIdok();
	afx_msg void OnBnClickedMfcbuttonCopy();
	afx_msg void OnBnClickedMfcbuttonPaste();
	CMFCButton m_ctrlCopy;
	CMFCButton m_ctrlPaste;
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnEditCopy();
	void OnEditPaste();
	COleDataSource* CopyTextFromGrid();
	BOOL PasteTextToGrid(CCellID cell, COleDataObject* pDataObject, BOOL bSelectPasteCells = TRUE);
	void ValidateAndModifyCellContents(int nRow, int nCol, LPCTSTR strText);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDeltaposSpinMinCond(NMHDR* pNMHDR, LRESULT* pResult);
	void AddColumn(int nRow, int nCol);
	void AddRow(int nRow, int nCol);
};
