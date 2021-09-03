
// TicketCreatorDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "TicketCreator.h"
#include "TicketCreatorDlg.h"
#include "afxdialogex.h"
#include "CHomeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TAB_HEIGHT 30
#define WM_TABLE_UPDATED WM_USER+2
#define WM_TABLE_ADD_COLUMN WM_USER+3
#define WM_TABLE_ADD_ROW WM_USER+4

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTicketCreatorDlg dialog



CTicketCreatorDlg::CTicketCreatorDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TICKETCREATOR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pHomeDlg = NULL;
	m_pConditionsDlg = NULL;
	m_pPointLimitsDlg = NULL;
	m_pCouponDlg = NULL;
}

void CTicketCreatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tabControl);
}

BEGIN_MESSAGE_MAP(CTicketCreatorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CTicketCreatorDlg::OnBnClickedOk)
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CTicketCreatorDlg::OnTcnSelchangeTab1)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CTicketCreatorDlg message handlers

BOOL CTicketCreatorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	
	CRect rect;
	GetClientRect(rect);
	m_tabControl.InsertItem(0, _T("Initial Setting"));
	m_tabControl.InsertItem(1, _T("Conditions"));
	m_tabControl.InsertItem(2, _T("Point Limits"));
	m_tabControl.InsertItem(3, _T("Coupons"));
	m_tabControl.SetCurSel(0);

	rect.top = rect.top + 5;
	rect.left = rect.left + 5;
	rect.right = rect.right - 5;
	rect.bottom = rect.bottom - 5;
	m_tabControl.AdjustRect(TRUE, rect);
	m_tabControl.SetItemSize(CSize(50, TAB_HEIGHT));

	if (m_pHomeDlg == NULL) {
		m_pHomeDlg = new CHomeDlg;
		m_pHomeDlg->Create(IDD_DIALOG_HOME, this);
	}
	if (m_pConditionsDlg == NULL) {
		m_pConditionsDlg = new CConditionsDlg;
		m_pConditionsDlg->Create(IDD_DIALOG_CONDITIONS, this);
		m_pConditionsDlg->ShowWindow(SW_HIDE);
	}
	if (m_pPointLimitsDlg == NULL) {
		m_pPointLimitsDlg = new CPointLimitsDlg;
		m_pPointLimitsDlg->Create(IDD_DIALOG_POINT_LIMITS, this);
		m_pPointLimitsDlg->ShowWindow(SW_HIDE);
	}
	if (m_pCouponDlg == NULL) {
		m_pCouponDlg = new CCouponDlg;
		m_pCouponDlg->Create(IDD_DIALOG_COUPONS, this);
		m_pCouponDlg->ShowWindow(SW_HIDE);
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	ShowWindow(SW_MAXIMIZE);

	GetClientRect(rect);
	MoveWindow(rect);
	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTicketCreatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTicketCreatorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);

		HBITMAP hbm = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BACKGROUND));
		BITMAP bm;
		HDC hdcMem = CreateCompatibleDC(dc);
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbm);
		GetObject(hbm, sizeof(bm), &bm);
		BitBlt(dc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);
	}
	else
	{
		CPaintDC dc(this);
		HBITMAP hbm = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BACKGROUND));
		BITMAP bm;
		HDC hdcMem = CreateCompatibleDC(dc);
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbm);
		GetObject(hbm, sizeof(bm), &bm);
		BitBlt(dc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTicketCreatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTicketCreatorDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
}


void CTicketCreatorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	CRect rect;
	GetClientRect(rect);
	if (m_tabControl.m_hWnd)
	{
		rect.top = rect.top + 5;
		rect.left = rect.left + 5;
		rect.right = rect.right - 5;
		rect.bottom = rect.bottom - 5;
		m_tabControl.MoveWindow(rect);
	}
	CRect rect1 = rect;
	if (m_pHomeDlg && m_pHomeDlg->m_hWnd) {
		rect.top = rect1.top + TAB_HEIGHT + 5;
		rect.bottom = rect1.bottom - 5;
		rect.left = rect1.left + 5;
		rect.right = rect1.right - 5;
		m_pHomeDlg->MoveWindow(rect);
	}
	if (m_pConditionsDlg && m_pConditionsDlg->m_hWnd) {
		rect.top = rect1.top + TAB_HEIGHT + 5;
		rect.bottom = rect1.bottom - 5;
		rect.left = rect1.left + 5;
		rect.right = rect1.right - 5;
		m_pConditionsDlg->MoveWindow(rect);
	}
	if (m_pPointLimitsDlg && m_pPointLimitsDlg->m_hWnd) {
		rect.top = rect1.top + TAB_HEIGHT + 5;
		rect.bottom = rect1.bottom - 5;
		rect.left = rect1.left + 5;
		rect.right = rect1.right - 5;
		m_pPointLimitsDlg->MoveWindow(rect);
	}
	if (m_pCouponDlg && m_pCouponDlg->m_hWnd) {
		rect.top = rect1.top + TAB_HEIGHT + 5;
		rect.bottom = rect1.bottom - 5;
		rect.left = rect1.left + 5;
		rect.right = rect1.right - 5;
		m_pCouponDlg->MoveWindow(rect);
	}
	// TODO: Add your message handler code here
}


void CTicketCreatorDlg::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	int sel = m_tabControl.GetCurSel();
	if (sel == 0) {
		m_pHomeDlg->ShowWindow(SW_SHOW);
		m_pConditionsDlg->ShowWindow(SW_HIDE);
		m_pPointLimitsDlg->ShowWindow(SW_HIDE);
		m_pCouponDlg->ShowWindow(SW_HIDE);
	}
	if (sel == 1) {
		m_pHomeDlg->ShowWindow(SW_HIDE);
		m_pConditionsDlg->ShowWindow(SW_SHOW);
		m_pPointLimitsDlg->ShowWindow(SW_HIDE);
		m_pCouponDlg->ShowWindow(SW_HIDE);
	}
	if (sel == 2) {
		m_pHomeDlg->ShowWindow(SW_HIDE);
		m_pConditionsDlg->ShowWindow(SW_HIDE);
		m_pPointLimitsDlg->ShowWindow(SW_SHOW);
		m_pCouponDlg->ShowWindow(SW_HIDE);
	}
	if (sel == 3) {
		m_pHomeDlg->ShowWindow(SW_HIDE);
		m_pConditionsDlg->ShowWindow(SW_HIDE);
		m_pPointLimitsDlg->ShowWindow(SW_HIDE);
		m_pCouponDlg->ShowWindow(SW_SHOW);
	}
}


BOOL CTicketCreatorDlg::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_pHomeDlg)
		delete m_pHomeDlg;
	if (m_pConditionsDlg)
		delete m_pConditionsDlg;
	if (m_pPointLimitsDlg)
		delete m_pPointLimitsDlg;
	if (m_pCouponDlg)
		delete m_pCouponDlg;
	return CDialogEx::DestroyWindow();
}


LRESULT CTicketCreatorDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_TABLE_UPDATED)
	{
		m_pConditionsDlg->ReloadTable(wParam, lParam);
		m_pPointLimitsDlg->ReloadTable(wParam, lParam);
		m_pCouponDlg->setMatchInfo(wParam, lParam);
		SendMessage(WM_SIZE, 0, 0);
	}
	if (message == WM_TABLE_ADD_COLUMN)
	{
		m_pConditionsDlg->AddColumn(wParam, lParam);
		m_pPointLimitsDlg->AddColumn(wParam, lParam);
		m_pCouponDlg->setMatchInfo(wParam, lParam);
		SendMessage(WM_SIZE, 0, 0);
	}
	if (message == WM_TABLE_ADD_ROW)
	{
		m_pConditionsDlg->AddRow(wParam, lParam);
		m_pPointLimitsDlg->AddRow(wParam, lParam);
		m_pCouponDlg->setMatchInfo(wParam, lParam);
		SendMessage(WM_SIZE, 0, 0);
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}


HBRUSH CTicketCreatorDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkColor(RGB(190, 219, 255));
	}
	return hbr;
}
