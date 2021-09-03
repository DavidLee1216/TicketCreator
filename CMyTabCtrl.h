#pragma once
#include <afxcmn.h>
class CMyTabCtrl :
	public CTabCtrl
{
public:
	CMyTabCtrl();
	~CMyTabCtrl();
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
};

