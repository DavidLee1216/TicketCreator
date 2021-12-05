// CCouponDlg.cpp : implementation file
//

#include "TicketCreator.h"
#include "CCouponDlg.h"
#include "CConditionsDlg.h"
#include "CPointLimitsDlg.h"
#include "CouponTicket.h"
#include "CouponView.h"
#include "./NewCellTypes/GridCellSpinEdit.h"
#include "./NewCellTypes/GridCellCheck.h"
#include <algorithm>
#include "afxdialogex.h"
#include <regex>

// CCouponDlg dialog
#define MAX_SINGLE_LINE 0xffffffff
#define DEFAULT_ROW_CNT 5
#define DEFAULT_COL_CNT 3
#define MAX_COUPON_CNT 1000
#define ID_COUPON_VIEW 1234

IMPLEMENT_DYNAMIC(CCouponDlg, CDialog)

CouponAgenda g_couponAgenda;
extern double g_dblScoreBase;
extern CTicketCreatorDlg* g_pTicketCreatorDlg;

double OneCoupon::calculateSingleLineScore(int depth) {
	if (depth == nRow) {
		double v = 0;
		for (int i = 0; i < nRow; i++)
		{
			v += g_pTicketCreatorDlg->m_pPointLimitsDlg->m_nppPointMatrix[i + 1][m_pSelected[i]];
		}
		score += pow(g_dblScoreBase, v);
		return score;
	}
	for (int i = 0; i < byteRowValues[depth]->cnt; i++)
	{
		m_pSelected[depth] = byteRowValues[depth]->arr[i];
		calculateSingleLineScore(depth + 1);
	}
	return score;
}

void OneCoupon::calculateScore() {
	m_pSelected = (int*)malloc(sizeof(int) * (nRow + 1));
	score = 0;
	score = calculateSingleLineScore(0);
	free(m_pSelected);
}

Coupon::Coupon()
{
	values = 0;
	valueCnt = 0;
	depth = 0;
}
Coupon::~Coupon()
{
	if (values)
	{
		free(values);
		values = 0;
	}
	childs.RemoveAll();
}
Coupon::Coupon(int val, int dep)
{
	values = (BYTE*)malloc(2);
	values[0] = val;
	valueCnt = 1;
	depth = dep;
}
int Coupon::find(int *val, int cnt)
{
	int childcnt = childs.GetSize();
	for (int i = 0; i < childcnt; i++)
	{
		Coupon *c = childs.GetAt(i);
		if(cnt != c->valueCnt)
			continue;
		bool bres = true;
		for (int j = 0; j < c->valueCnt; j++)
		{
			if (val[j] == c->values[j])
				bres &= true;
			else
			{
				bres = false; break;
			}
		}
		if (bres)
		{
			return i;
		}
	}
	return -1;
}
void Coupon::add(BYTE* vals, int cnt, int depth)
{
	if (cnt == depth)
		return;
	int idx = -1;
	int v[2]; v[0] = vals[depth];
	Coupon* coupon = 0;
	if ((idx = find(v, 1)) != -1)
	{
		coupon = childs.GetAt(idx);
	}
	else
	{
		coupon = new Coupon(vals[depth], depth + 1);
		g_couponAgenda.addCoupon(coupon);
		int insertIdx = -1;
		for (int i = 0; i < childs.GetCount(); i++)
		{
			Coupon* c = childs.GetAt(i);
			if (c->values[0] > vals[depth])
			{
				insertIdx = i; break;
			}
		}
		if (insertIdx == -1)
			childs.Add(coupon);
		else
			childs.InsertAt(insertIdx, coupon);
	}
	coupon->add(vals, cnt, depth + 1);
}
void Coupon::setValue(BYTE* val, int cnt, int dep)
{
	if (values)
		free(values);
	values = (BYTE*)malloc(cnt);
	for (int i = 0; i < cnt; i++)
	{
		values[i] = val[i];
	}
	valueCnt = cnt;
	depth = dep;
}

void Coupon::combineValue(BYTE* val1, int cnt1, BYTE* val2, int cnt2, int dep)
{
	if (values)
		free(values);
	values = (BYTE*)malloc(cnt1 + cnt2);
	for (int i = 0; i < cnt1; i++)
	{
		values[i] = val1[i];
	}
	int k = cnt1;
	for (int i = 0; i < cnt2; i++)
	{
		bool b = false;
		for (int j = 0; j < cnt1; j++)
		{
			if (val2[i] == val1[j])
			{
				b = true;
				break;
			}
		}
		if (!b)
			values[k++] = val2[i];
	}
	sort(values, values + k);
	valueCnt = k;
	depth = dep;
}
bool Coupon::compareValues(Coupon* coupon1, Coupon* coupon2)
{
	if (coupon1->valueCnt != coupon2->valueCnt)
		return false;
	for (int i = 0; i < coupon1->valueCnt; i++)
	{
		if (coupon1->values[i] != coupon2->values[i])
			return false;
	}
	return true;
}
Coupon* Coupon::getCommonTree(Coupon* coupon1, Coupon* coupon2, bool &succ, bool bmain)
{
	if (coupon1->depth == 4)
		int kk = 0;
	Coupon* coupon = new Coupon();
	int n1 = coupon1->childs.GetSize();
	int n2 = coupon2->childs.GetSize();
	succ = false;
	if (n1 == 0 && n2 == 0)
	{
		if (bmain)
			succ = true;
		else if(compareValues(coupon1, coupon2))
			succ = true;
	}
	for (int i = 0; i < n1; i++)
	{
		for (int j = 0; j < n2; j++)
		{
			Coupon* subCoup1 = coupon1->childs.GetAt(i);
			Coupon* subCoup2 = coupon2->childs.GetAt(j);
			if (compareValues(subCoup1, subCoup2))
			{
				bool bsucc = false;
				Coupon* newCoup = getCommonTree(subCoup1, subCoup2, bsucc, false);
				if(newCoup && bsucc)
					coupon->childs.Add(newCoup);
				succ |= bsucc;
			}
		}
	}
	if (succ)
	{
		if (compareValues(coupon1, coupon2) && !bmain)
		{
			succ = true;
			coupon->setValue(coupon1->values, coupon1->valueCnt, coupon1->depth);
			g_couponAgenda.addCoupon(coupon);
			return coupon;
		}
		else if(bmain)
		{
			coupon->combineValue(coupon1->values, coupon1->valueCnt, coupon2->values, coupon2->valueCnt, coupon1->depth);
			g_couponAgenda.addCoupon(coupon);
			return coupon;
		}
	}
	delete coupon;
	return NULL;
}
bool Coupon::merge(int idx1, int idx2, Coupon* parent)
{
	int num = parent->childs.GetCount();
	if (idx1 >= num || idx2 >= num)
		return false;
	Coupon* coupon1 = parent->childs.GetAt(idx1);
	Coupon* coupon2 = parent->childs.GetAt(idx2);
	bool succ = false;
	Coupon* common = getCommonTree(coupon1, coupon2, succ, true);
	if (!succ)
		return false;
	if (common == NULL)
		int kk = 0;
	Coupon* subCoupon1 = getDiffTree(coupon1, common);
	Coupon* subCoupon2 = getDiffTree(coupon2, common);
	int maxIdx = max(idx1, idx2);
	int minIdx = min(idx1, idx2);
	parent->childs.RemoveAt(maxIdx);
	parent->childs.RemoveAt(minIdx);
	parent->childs.Add(common);
	if (subCoupon1)
		parent->childs.Add(subCoupon1);
	if (subCoupon2)
		parent->childs.Add(subCoupon2);
	return true;
}
Coupon* Coupon::getDiffTree(Coupon* srcCoupon, Coupon* subCoupon)
{
	int srcCnt = srcCoupon->childs.GetSize();
	int subCnt = subCoupon->childs.GetSize();
	if (srcCnt == 0 && subCnt == 0)
		return NULL;
	Coupon* coupon = new Coupon();
	coupon->setValue(srcCoupon->values, srcCoupon->valueCnt, srcCoupon->depth);
	for (int i = 0; i < srcCnt; i++)
	{
		Coupon* c1 = srcCoupon->childs.GetAt(i);
		bool bMatch = false;
		Coupon* diff = 0;
		for (int j = 0; j < subCnt; j++)
		{
			Coupon* c2 = subCoupon->childs.GetAt(j);
			if (compareValues(c1, c2))
			{
				diff = getDiffTree(c1, c2);
				if (diff)
				{
					bMatch = false; break;
				}
				bMatch = true;
			}
		}
		if (!bMatch && diff)
			coupon->childs.Add(diff);
		else if (!bMatch && diff == NULL)
			coupon->childs.Add(c1);
	}
	if (coupon->childs.GetCount() == 0)
	{
		delete coupon;
		return NULL;
	}
	g_couponAgenda.addCoupon(coupon);
	return coupon;
}
bool Coupon::merge()
{
// 	if (childCnt == 0)
// 		return;
	for (int i = 0; i < childs.GetSize(); i++)
	{
		childs.GetAt(i)->merge();
	}
	bool bMerged = true;
	while (bMerged)
	{
		bMerged = false;
		for (int i = 0; i < childs.GetSize(); i++)
		{
			for (int j = 0; j < childs.GetSize(); j++)
			{
				if(i == j)
					continue;
				bMerged |= merge(i, j, this);
			}
		}
	}
	return true;
}
CCouponDlg::CCouponDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_COUPONS, pParent)
	, m_nFrom(0), m_nTo(0)
	, m_nCondRow(0)
	, m_nCondCol(0)
{
	m_nRow = DEFAULT_ROW_CNT;
	m_nCol = DEFAULT_COL_CNT;
	m_pSelected = 0;
	m_nSingleLines = 0;
	m_ppbyteSingleLines = 0;
	m_ppbyteCoupons = 0;
	m_nCouponCnt = 0;
	m_pParentDlg = NULL;
	baseCoupon = 0;// 
	m_coupons = 0;
	m_nMaxCouponCount = MAX_COUPON_CNT;

	m_view = new CouponView(this);
}

CCouponDlg::~CCouponDlg()
{
	resetSingleLines();
	delete m_view;
	if (m_coupons)
		removeCouponsArray();
// 	if(baseCoupon)
// 		delete baseCoupon;
}

void CCouponDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_nFrom);
	DDX_Text(pDX, IDC_EDIT2, m_nTo);
	DDX_Text(pDX, IDC_EDIT_COUPON_COND_ROW, m_nCondRow);
	DDX_Text(pDX, IDC_EDIT_COUPON_COND_COL, m_nCondCol);
}


BEGIN_MESSAGE_MAP(CCouponDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_COUPON, &CCouponDlg::OnBnClickedButtonCoupon)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CCouponDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CCouponDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_COUPON_PRINT, &CCouponDlg::OnBnClickedButtonCouponPrint)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE, &CCouponDlg::OnBnClickedButtonChange)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


void CCouponDlg::DuplicatePermutation(int N, int R, int depth)
{
	int i;
	if (R == depth) {
		bool bres = CheckConditions(m_pSelected, R);
		if (!bres)
			return;
		bres = CheckPointLimits(m_pSelected, R);
		if (!bres)
			return;
		addToSingleLines(m_pSelected, R);
		return;
	}
	for (i = 1; i <= N; i++) {
		m_pSelected[depth] = i;
		DuplicatePermutation(N, R, depth + 1);
	}
}


void CCouponDlg::OnBnClickedButtonCoupon()
{
	resetSingleLines();
	prepareSingleLines(m_nRow, m_nCol);
	DuplicatePermutation(m_nCol, m_nRow, 0);
	makeCoupons();
}


void CCouponDlg::setMatchInfo(int nRow, int nCol)
{
	m_nRow = nRow;
	m_nCol = nCol;
}


bool CCouponDlg::CheckConditions(int *arr, int cnt)
{
	COneConditionDlg **pOneCond = m_pParentDlg->m_pConditionsDlg->m_pOneCond;
	int nOneCondCnt = m_pParentDlg->m_pConditionsDlg->m_nCondCnt;
	bool bres = true;
	for (int i = 0; i < nOneCondCnt; i++)
	{
		bres &= CheckOneCondition(arr, cnt, pOneCond[i]);
		if (!bres)
			return false;
	}
	return true;
}


bool CCouponDlg::CheckPointLimits(int* arr, int cnt)
{
	CPointLimitsDlg* pPointLimt = m_pParentDlg->m_pPointLimitsDlg;
	pPointLimt->m_nppPointMatrix;
	int sum = 0;
	for (int i = 0; i < cnt; i++)
	{
		sum += pPointLimt->m_nppPointMatrix[i + 1][arr[i]];
	}
	if (sum < 140)
		int kk = 0;
	if (sum > 140 && sum < 160)
		int kk = 0;
	int min = pPointLimt->m_nMin;
	int max = pPointLimt->m_nMax;
	if (min == 0 && max == 0)
		return true;
	else if(max != 0)
	{
		if (min == 0 && sum <= max)
			return true;
		if (sum >= min && sum <= max)
			return true;
	}
	else if (min != 0)
	{
		if (max == 0 && sum >= min)
			return true;
	}
	return false;
}


void CCouponDlg::addToSingleLines(int* arr, int cnt)
{
	if (cnt >= MAX_SINGLE_LINE)
		return;
	BYTE* line = (BYTE*)malloc(m_nRow + 1);
	if (arr[0] == 1 && arr[1] == 1 && arr[2] == 2 && arr[3] == 3 && arr[4] == 2)
		int kk = 0;
	for (int i = 0; i < cnt; i++)
	{
		line[i] = arr[i];
	}
	m_ppbyteSingleLines[m_nSingleLines++] = line;

}


void CCouponDlg::resetSingleLines()
{
	if (m_pSelected)
	{
		free(m_pSelected);
		m_pSelected = 0;
	}
	if (m_ppbyteSingleLines)
	{
		for (int i = 0; i < m_nSingleLines; i++)
		{
			if(m_ppbyteSingleLines[i])
				free(m_ppbyteSingleLines[i]);
		}
		delete[] m_ppbyteSingleLines;
		m_ppbyteSingleLines = 0;
		m_nSingleLines = 0;
	}
}


void CCouponDlg::prepareSingleLines(int nRow, int nCol)
{
	int cnt = pow(nCol, nRow);
	m_pSelected = (int*)malloc(sizeof(int) * (nRow + 1));
	int lines = min(MAX_SINGLE_LINE, cnt);
	m_ppbyteSingleLines = new BYTE*[lines];
	for (int i = 0; i < lines; i++)
	{
		m_ppbyteSingleLines[i] = 0;
	}
	m_nSingleLines = 0;
}


bool CCouponDlg::CheckOneCondition(int* arr, int cnt, COneConditionDlg *oneCond)
{
	int min = oneCond->m_nMin;
	int max = oneCond->m_nMax;
	CheckedItems* pCheckedItems = &oneCond->m_checkedItems;
	int nCheckItemCnt = pCheckedItems->m_arrCheckedItems.GetCount();
	int nMatchedCnt = 0;
	for (int i = 0; i < nCheckItemCnt; i++)
	{
		CheckedOneItem oneItem = pCheckedItems->m_arrCheckedItems.GetAt(i);
		if (arr[oneItem.row - 1] == oneItem.col)
			nMatchedCnt++;
	}
	if (min == 0 && max == 0 && nMatchedCnt==0)
		return true;
// 	else if (min == 0 && max != 0)
// 	{
// 		if (nMatchedCnt<=max)
// 			return true;
// 	}
	else if (min != 0 && max == 0)
	{
		if (nMatchedCnt >= min)
			return true;
	}
	else
	{
		if (nMatchedCnt >= min && nMatchedCnt <= max)
			return true;
	}
	return false;
}


void CCouponDlg::makeCoupons()
{
	prepareBaseCoupon();
	mergeCoupons(0);
	removeCouponsArray();
// 	int l = testCount(baseCoupon);
	m_coupons = new OneCoupon[1000];
	for (int i = 0; i < 1000; i++)
	{
		m_coupons[i].byteRowValues = 0;
		m_coupons[i].nRow = m_nRow;
	}
	m_nCouponCnt = 0;
	m_nMaxCouponCount = MAX_COUPON_CNT;

	makeTickets(baseCoupon, 0, 0, 0);
	int nSingleLines = countSingleLinesOfCoupon();
	sortCoupons();
	calculateCouponScores();
	SYSTEMTIME time;
	GetLocalTime(&time);
	TCHAR str_time[100];
	_stprintf(str_time, _T("%d.%d.%d %d:%d"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
// 	m_view->reloadCoupons(m_ppbyteCoupons, m_nCouponCnt, m_nRow, m_nSingleLines, str_time);
	m_view->reloadCoupons(m_coupons, m_nCouponCnt, m_nRow, nSingleLines, str_time);
	SendMessage(WM_SIZE, 0, 0);
/*
	FILE* fp = fopen("debug.txt", "w");
	for (int i = 0; i < m_nCouponCnt; i++)
	{
		for (int j = 0; j < m_nRow; j++)
		{
			for (int k = 0; k < m_ppbyteCoupons[i][j]->cnt; k++)
			{
				char str[30];
				sprintf(str, "%d,", m_ppbyteCoupons[i][j]->arr[k]);
				fwrite(str, strlen(str), 1, fp);
			}
			fwrite("\r\n", 2, 1, fp);
		}
		fwrite("\r\n", 2, 1, fp);
	}
	fclose(fp);
*/
	baseCoupon = 0;
	int kk = 0;
}


int CCouponDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pParentDlg = (CTicketCreatorDlg *)FromHandle(lpCreateStruct->hwndParent);
	m_view->Create(CRect(0, 0, 20, 20), this, ID_COUPON_VIEW);
	m_view->ShowWindow(SW_SHOW);
	return 0;
}


void CCouponDlg::prepareBaseCoupon()
{
	g_couponAgenda.reset();
	if (baseCoupon == 0)
	{
		baseCoupon = new Coupon(0, 0);
		g_couponAgenda.addCoupon(baseCoupon);
	}
	for (int i = 0; i < m_nSingleLines; i++)
	{
		baseCoupon->add(m_ppbyteSingleLines[i], m_nRow, 0);
	}
}


bool CCouponDlg::mergeCoupons(int start)
{
	baseCoupon->merge();
	return true;
}

int compareCouponByCount(void *c1, void *c2)
{
	OneCoupon* one = (OneCoupon*)c1;
	OneCoupon* two = (OneCoupon*)c2;
	return one->nSingleLineCnt - two->nSingleLineCnt;
}

int CCouponDlg::makeTickets(Coupon *coupon, int depth, st_byte_array**base, int order)
{
	int cnt = 0;
	int total = 0;
	if (coupon->childs.GetCount() == 0)
		return coupon->valueCnt;
	for (int i = 0; i < coupon->childs.GetCount(); i++)
	{
		if (m_coupons[m_nCouponCnt].byteRowValues == 0) {
			st_byte_array** row = new st_byte_array * [m_nRow];
			if (base)
				for (int j = 0; j < depth; j++)
				{
					row[j] = new st_byte_array;
					row[j]->arr = (BYTE*)malloc(base[j]->cnt);
					row[j]->cnt = base[j]->cnt;
					for (int k = 0; k < base[j]->cnt; k++)
					{
						row[j]->arr[k] = base[j]->arr[k];
					}
				}
			m_coupons[m_nCouponCnt].byteRowValues = row;
		}
		st_byte_array* st_arr = new st_byte_array;
		m_coupons[m_nCouponCnt].byteRowValues[depth] = st_arr;
		m_coupons[m_nCouponCnt].byteRowValues[depth]->arr = (BYTE*)malloc(coupon->childs.GetAt(i)->valueCnt);
		for (int k = 0; k < coupon->childs.GetAt(i)->valueCnt; k++)
		{
			m_coupons[m_nCouponCnt].byteRowValues[depth]->arr[k] = coupon->childs.GetAt(i)->values[k];
		}
		m_coupons[m_nCouponCnt].byteRowValues[depth]->cnt = coupon->childs.GetAt(i)->valueCnt;
		cnt += makeTickets(coupon->childs.GetAt(i), depth + 1, m_coupons[m_nCouponCnt].byteRowValues, order);
		if (depth == m_nRow - 1)
		{
			if (m_nCouponCnt+1 >= m_nMaxCouponCount)
			{
				increaseCouponStorage();
			}
			m_nCouponCnt++;
		}
		order++;
	}
	total = coupon->valueCnt*cnt;
	return total;
}


void CCouponDlg::removeCouponsArray()
{
	if (m_coupons) {
		for (int i = 0; i < m_nCouponCnt; i++)
		{
			for (int j = 0; j < m_coupons[i].nRow; j++)
			{
				free(m_coupons[i].byteRowValues[j]->arr);
				delete m_coupons[i].byteRowValues[j];
			}
			delete m_coupons[i].byteRowValues;
		}
		delete[] m_coupons;
		m_coupons = 0;
	}

}


void CCouponDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	HBITMAP hbm = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BACKGROUND));
	BITMAP bm;
	HDC hdcMem = CreateCompatibleDC(dc);
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbm);
	GetObject(hbm, sizeof(bm), &bm);
	BitBlt(dc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hbmOld);
	DeleteDC(hdcMem);
}


void CCouponDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_view->m_hWnd)
	{
		CRect rect;
		GetClientRect(rect);
		rect.top = rect.top + 80; rect.left = rect.left + 20; rect.right = rect.right - 20; rect.bottom = rect.bottom - 20;
		m_view->MoveWindow(rect);
		m_view->ShowWindow(SW_SHOW);
	}
}


void CCouponDlg::sortCoupons()
{
	for (int i = 0; i < m_nCouponCnt; i++)
	{
		int imax = i;
		for (int j = i + 1; j < m_nCouponCnt; j++)
		{
			if (m_coupons[imax].nSingleLineCnt < m_coupons[j].nSingleLineCnt)
				imax = j;
		}
		if (imax != i)
		{
			OneCoupon temp = m_coupons[i];
			m_coupons[i] = m_coupons[imax];
			m_coupons[imax] = temp;
		}
	}
	// 	qsort(m_coupons, m_nCouponCnt, sizeof(OneCoupon), compareCouponByCount);
}


int CCouponDlg::countSingleLinesOfCoupon()
{
	int nSingleLines = 0;
	for (int i = 0; i < m_nCouponCnt; i++)
	{
		int lines = 1;
		for (int j = 0; j < m_coupons[i].nRow; j++)
		{
			lines *= m_coupons[i].byteRowValues[j]->cnt;
		}
		m_coupons[i].nSingleLineCnt = lines;
		nSingleLines += lines;
	}
	return nSingleLines;
}


int CCouponDlg::testCount(Coupon *coupon)
{
	int cnt = 0;
	int total = 0;
	if (coupon->childs.GetCount() == 0)
		return coupon->valueCnt;
	for (int i = 0; i < coupon->childs.GetCount(); i++)
	{
		int c = testCount(coupon->childs.GetAt(i));
		cnt += c;
	}
	total = cnt * coupon->valueCnt;
	return total;
}


void CCouponDlg::OnBnClickedButtonSave()
{
	TCHAR szFilters[] = _T("Coupon Files (*.coupon)|*.coupon");//|All Files (*.*)|*.*||

	// Create an Open dialog; the default file name extension is ".my".
	CFileDialog fileDlg(FALSE, _T("coupon"), _T("*.coupon"),
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	// Display the file dialog. When user clicks OK, fileDlg.DoModal() 
	// returns IDOK.
	if (fileDlg.DoModal() == IDOK)
	{
		CString pathName = fileDlg.GetPathName();

		// Implement opening and reading file in here.

		//Change the window's title to the opened file's title.
		CString fileName = fileDlg.GetFileTitle();
		writeToFile(pathName);
	}
}


void CCouponDlg::OnBnClickedButtonOpen()
{
	TCHAR szFilters[] = _T("Coupon Files (*.coupon)|*.coupon");//|All Files (*.*)|*.*||

	// Create an Open dialog; the default file name extension is ".my".
	CFileDialog fileDlg(TRUE, _T("coupon"), _T("*.coupon"),
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	// Display the file dialog. When user clicks OK, fileDlg.DoModal() 
	// returns IDOK.
	if (fileDlg.DoModal() == IDOK)
	{
		CString pathName = fileDlg.GetPathName();

		// Implement opening and reading file in here.

		//Change the window's title to the opened file's title.
		m_nSingleLines = 0;
		CString fileName = fileDlg.GetFileTitle();
		TCHAR szTime[30];
		readFromFile(pathName, szTime);
		int nSingleLines = countSingleLinesOfCoupon();
		m_view->reloadCoupons(m_coupons, m_nCouponCnt, m_nRow, nSingleLines, szTime);
		SendMessage(WM_SIZE, 0, 0);
	}
}

char* CStringToChar(CString str, char* charStr);
char* TCHARToChar(TCHAR* str, char* charStr);
regex compChangeRegEx("([1-9]+0?/)?([1-9]+0?,)*([0-9]+)");

void CCouponDlg::writeToFile(CString pathName)
{
	char filename[MAX_PATH];
	CStringToChar(pathName, filename);
	FILE* fp = fopen(filename, "wb");
	char time[100]; time[0] = 0;
	CStringToChar(m_view->m_strTime, time);
	fwrite(time, strlen(time), 1, fp);
	fwrite("\r\n", 2, 1, fp);
	char cnt[3]; cnt[0] = 0;
	int nRow = m_pParentDlg->m_pHomeDlg->m_nMatchCount;
	cnt[0] = nRow; cnt[1] = 0;
	fwrite(cnt, 1, 1, fp);
	int nCol = m_pParentDlg->m_pHomeDlg->m_nOutcomeCount;
	cnt[0] = nCol; cnt[1] = 0;
	fwrite(cnt, 1, 1, fp);
	fwrite("\r\n", 2, 1, fp);
	COneConditionDlg** pOneCond = m_pParentDlg->m_pConditionsDlg->m_pOneCond;
	int nOneCondCnt = m_pParentDlg->m_pConditionsDlg->m_nCondCnt;
	fwrite(&nOneCondCnt, 1, 1, fp);
	for (int i = 0; i < nOneCondCnt; i++)
	{
		fwrite("COND\r\n", 6, 1, fp);
		cnt[0] = pOneCond[i]->m_nMin; cnt[1] = 0;
		fwrite(cnt, 1, 1, fp);
		cnt[0] = pOneCond[i]->m_nMax; cnt[1] = 0;
		fwrite(cnt, 1, 1, fp);
		int nChecked = pOneCond[i]->m_checkedItems.m_arrCheckedItems.GetSize();
		fwrite(&nChecked, 2, 1, fp);
		fwrite("\r\n", 2, 1, fp);
		for (int j = 0; j < pOneCond[i]->m_checkedItems.m_arrCheckedItems.GetCount(); j++)
		{
// 			char str[20]; str[0] = 0;
// 			sprintf(str, "%d %d\r\n", pOneCond[i]->m_checkedItems.m_arrCheckedItems.GetAt(j).row, pOneCond[i]->m_checkedItems.m_arrCheckedItems.GetAt(j).col);
			fwrite(&pOneCond[i]->m_checkedItems.m_arrCheckedItems.GetAt(j).row, 1, 1, fp);
			fwrite(&pOneCond[i]->m_checkedItems.m_arrCheckedItems.GetAt(j).col, 1, 1, fp);
		}
		fwrite("\r\n", 2, 1, fp);
	}
	fwrite("POINT\r\n", 7, 1, fp);
	CPointLimitsDlg* pPointLimt = m_pParentDlg->m_pPointLimitsDlg;
	fwrite(&(pPointLimt->m_nMin), sizeof(WORD), 1, fp);
	fwrite(&(pPointLimt->m_nMax), sizeof(WORD), 1, fp);
	fwrite("\r\n", 2, 1, fp);
	for (int i = 1; i <= nRow; i++)
	{
		for (int j = 1; j <= nCol; j++)
		{
			fwrite(&pPointLimt->m_nppPointMatrix[i][j], sizeof(WORD), 1, fp);
		}
		fwrite("\r\n", 2, 1, fp);
	}
	
	fwrite(&m_nCouponCnt, 4, 1, fp);
	fwrite("\r\n", 2, 1, fp);
	for (int i = 0; i < m_nCouponCnt; i++)
	{
		fwrite("COUPON\r\n", 8, 1, fp);
		for (int j = 0; j < m_coupons[i].nRow; j++)
		{
			char str[200]; str[0] = 0;
			if (m_coupons[i].byteRowValues[j]->common_value > 0)
				sprintf(str, "%d/", m_coupons[i].byteRowValues[j]->common_value);
			for (int k = 0; k < m_coupons[i].byteRowValues[j]->cnt; k++)
			{
				sprintf(str, "%s %d", str, m_coupons[i].byteRowValues[j]->arr[k]);
			}
			fwrite(str, strlen(str), 1, fp);
			fwrite("\r\n", 2, 1, fp);
		}
	}

	fclose(fp);
}

TCHAR* charToTCHAR(char* charStr, TCHAR *str)
{
	int len = strlen(charStr);
	MultiByteToWideChar(CP_ACP, 0, charStr, -1, str, len);
	str[len] = 0;
	return str;
}

void CCouponDlg::readFromFile(CString pathName, TCHAR *szTime)
{
	removeCouponsArray();
	char filename[MAX_PATH];
	CStringToChar(pathName, filename);
	FILE* fp = fopen(filename, "rb");
	char time[200];
	fgets(time, 200, fp);
	int len = strlen(time);
	int i = len - 1;
	while (time[i]=='\r'|| time[i] == '\n')
	{
		time[i] = 0;
		i--;
	}
	charToTCHAR(time, szTime);
	char str[200];
	int nRow = 0;
	fread(&nRow, 1, 1, fp);
	int nCol = 0;
	fread(&nCol, 1, 1, fp);
	m_pParentDlg->m_pHomeDlg->ReloadTable(nRow, nCol);
	fseek(fp, 2, SEEK_CUR);
	int nOneCondCnt = 0;
	fread(&nOneCondCnt, 1, 1, fp);
	for (int i = 0; i < nOneCondCnt; i++)
	{
		fgets(str, 200, fp);
		if (strstr(str, "COND"))
		{
			TCHAR szVal[10];
			if (i == 0)
				m_pParentDlg->m_pConditionsDlg->ReloadTable(nRow, nCol);
			else
				m_pParentDlg->m_pConditionsDlg->AddTable(nRow, nCol);
			fread(&m_pParentDlg->m_pConditionsDlg->m_pOneCond[i]->m_nMin, 1, 1, fp);
			_stprintf(szVal, _T("%d"), m_pParentDlg->m_pConditionsDlg->m_pOneCond[i]->m_nMin);
			m_pParentDlg->m_pConditionsDlg->m_pOneCond[i]->m_ctrlMin.SetWindowText(szVal);
			fread(&m_pParentDlg->m_pConditionsDlg->m_pOneCond[i]->m_nMax, 1, 1, fp);
			_stprintf(szVal, _T("%d"), m_pParentDlg->m_pConditionsDlg->m_pOneCond[i]->m_nMax);
			m_pParentDlg->m_pConditionsDlg->m_pOneCond[i]->m_ctrlMax.SetWindowText(szVal);
			int nChecked = 0;
			fread(&nChecked, 2, 1, fp);
			fseek(fp, 2, SEEK_CUR);
			for (int j = 0; j < nChecked; j++)
			{
				int row = 0, col = 0; 
				fread(&row, 1, 1, fp);
				fread(&col, 1, 1, fp);
				CGridCellCheck* clickedCell = (CGridCellCheck*)m_pParentDlg->m_pConditionsDlg->m_pOneCond[i]->m_pGrid->GetCell(row, col);
				bool checkState = clickedCell->SetCheck();
				m_pParentDlg->m_pConditionsDlg->m_pOneCond[i]->m_checkedItems.addItem(row, col);
			}
			fseek(fp, 2, SEEK_CUR);
		}
	}
	fgets(str, 200, fp);
	if (strstr(str, "POINT"))
	{
		TCHAR szVal[10];
		m_pParentDlg->m_pPointLimitsDlg->ReloadTable(nRow, nCol);
		fread(&m_pParentDlg->m_pPointLimitsDlg->m_nMin, sizeof(WORD), 1, fp);
		_stprintf(szVal, _T("%d"), m_pParentDlg->m_pPointLimitsDlg->m_nMin);
		m_pParentDlg->m_pPointLimitsDlg->m_ctrlMin.SetWindowText(szVal);
		fread(&m_pParentDlg->m_pPointLimitsDlg->m_nMax, sizeof(WORD), 1, fp);
		_stprintf(szVal, _T("%d"), m_pParentDlg->m_pPointLimitsDlg->m_nMax);
		m_pParentDlg->m_pPointLimitsDlg->m_ctrlMax.SetWindowText(szVal);
		fseek(fp, 2, SEEK_CUR);
		for (int i = 1; i <= nRow; i++)
		{
			for (int j = 1; j <= nCol; j++)
			{
				fread(&m_pParentDlg->m_pPointLimitsDlg->m_nppPointMatrix[i][j], sizeof(WORD), 1, fp);
				CGridCellSpinEdit* cell = (CGridCellSpinEdit*)m_pParentDlg->m_pPointLimitsDlg->m_pGrid->GetCell(i, j);
				_stprintf(szVal, _T("%d"), m_pParentDlg->m_pPointLimitsDlg->m_nppPointMatrix[i][j]);
				cell->m_pEdit->SetWindowText(szVal);
			}
			fseek(fp, 2, SEEK_CUR);
		}
	}
	fread(&m_nCouponCnt, 4, 1, fp);
	fseek(fp, 2, SEEK_CUR);
	m_coupons = new OneCoupon[m_nCouponCnt];
	i = -1;
	int j = 0;
	while (fgets(str, 200, fp))
	{
		if (strcmp(str, "COUPON\r\n") == 0)
		{
			j = 0;
			i++;
			m_coupons[i].byteRowValues = new st_byte_array * [20];
			m_coupons[i].nRow = 0;
			continue;
		}
		m_coupons[i].nRow++;
		m_coupons[i].byteRowValues[j] = new st_byte_array;
		m_coupons[i].byteRowValues[j]->arr = new BYTE[100];
		m_coupons[i].byteRowValues[j]->cnt = 0;
		char* slash = strstr(str, "/");
		char* str1 = 0;// ;
		if (slash == 0)
		{
			m_coupons[i].byteRowValues[j]->common_value = 0;
			str1 = str;
		}
		else
		{
			char ss[5];
			strncpy(ss, str, slash - str);
			ss[slash - str] = 0;
			m_coupons[i].byteRowValues[j]->common_value = atoi(ss);
			str1 = slash + 1;
		}
		int k = 0;
		char* token = strtok(str1, " ");
		while (token)
		{
			BYTE val = atoi(token);
			m_coupons[i].byteRowValues[j]->arr[k] = val;
			m_coupons[i].byteRowValues[j]->cnt++;
			k++;
			token = strtok(NULL, " " );
		}
		j++;

	}
	m_nRow = m_coupons[0].nRow;
	m_nCol = nCol;
}


void CCouponDlg::OnBnClickedButtonCouponPrint()
{
	m_view->OnFilePrint();
}


void CCouponDlg::increaseCouponStorage()
{
	OneCoupon* coupon = new OneCoupon[m_nMaxCouponCount + 1000];
	for (int i = 0; i <= m_nCouponCnt; i++)
	{
		coupon[i].byteRowValues = m_coupons[i].byteRowValues;
		coupon[i].nRow = m_coupons[i].nRow;
		coupon[i].nSingleLineCnt = m_coupons[i].nSingleLineCnt;
	}
	delete[] m_coupons;
	m_coupons = coupon;
	m_nMaxCouponCount += 1000;
	for (int i = m_nCouponCnt+1; i < m_nMaxCouponCount; i++)
	{
		m_coupons[i].byteRowValues = 0;
		m_coupons[i].nRow = m_nRow;
	}
}


void CCouponDlg::OnBnClickedButtonChange()
{
	UpdateData(TRUE);
	if (m_nCouponCnt == 0)
	{
		AfxMessageBox(_T("You have to create coupons"));
		return;
	}
	if (m_nFrom < 0 || m_nTo < 0)
	{
		AfxMessageBox(_T("Enter valid number"));
		return;
	}
	TCHAR szChangeStr[100];
	GetDlgItem(IDC_EDIT2)->GetWindowText(szChangeStr, 100);
	bool bNoCondition = true;
	bool bComplexChange = false;
	if (m_nCondRow > 0)
		bNoCondition = false;
	if (bNoCondition == true && _tcslen(szChangeStr) > 1) {
		AfxMessageBox(_T("Please enter right format. In this case, only one number is accepted."));
		return;
	}
	if (_tcsstr(szChangeStr, _T("/"))|| _tcsstr(szChangeStr, _T(",")))
		bComplexChange = true;
	char szcharChangeStr[100];
	TCHARToChar(szChangeStr, szcharChangeStr);
	if (regex_match(szcharChangeStr, compChangeRegEx) == false)
	{
		AfxMessageBox(_T("Please enter right format."));
		return;
	}
	SYSTEMTIME time;
	GetLocalTime(&time);
	TCHAR str_time[100];
	_stprintf(str_time, _T("%d.%d.%d %d:%d"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
	for (int i = 0; i < m_nCouponCnt; i++)
	{
		for (int j = 0; j < m_coupons[i].nRow; j++)
		{
			char str[200]; str[0] = 0;
			if (!bComplexChange)
			{
				for (int k = 0; k < m_coupons[i].byteRowValues[j]->cnt; k++)
				{
					if (bNoCondition && m_coupons[i].byteRowValues[j]->arr[k] == m_nFrom)
						m_coupons[i].byteRowValues[j]->arr[k] = m_nTo;
					else if (!bNoCondition && (j + 1 == m_nCondRow) && m_coupons[i].byteRowValues[j]->arr[k] == m_nFrom)
						m_coupons[i].byteRowValues[j]->arr[k] = m_nTo;
				}
			}
			else
			{
				if (m_coupons[i].byteRowValues[j]->cnt == 1)
				{
					if (m_coupons[i].byteRowValues[j]->arr[0] == m_nFrom && (bNoCondition || (!bNoCondition && (j + 1 == m_nCondRow))))
					{
						char ss[5], szcharChangeStr[100];
						TCHARToChar(szChangeStr, szcharChangeStr);
						char* slash = strstr(szcharChangeStr, "/");
						if (slash)
						{
							strncpy(ss, szcharChangeStr, slash - szcharChangeStr);
							ss[slash - szcharChangeStr] = 0;
							m_coupons[i].byteRowValues[j]->common_value = atoi(ss);
							slash++;
						}
						else
						{
							slash = szcharChangeStr;
							m_coupons[i].byteRowValues[j]->common_value = 0;
						}
						free(m_coupons[i].byteRowValues[j]->arr);
						m_coupons[i].byteRowValues[j]->arr = new BYTE[100];
						char delim[] = ",";
						char* token = strtok(slash, delim);
						int tt = 0;
						while (token)
						{
							int val = atoi(token);
							m_coupons[i].byteRowValues[j]->arr[tt] = (BYTE)val;
							token = strtok(NULL, delim);
							tt++;
						}
						m_coupons[i].byteRowValues[j]->cnt = tt;
					}
				}
			}
		}
	}
	int nSingleLines = countSingleLinesOfCoupon();
	m_view->reloadCoupons(m_coupons, m_nCouponCnt, m_nRow, nSingleLines, str_time);
	SendMessage(WM_SIZE, 0, 0);
}


HBRUSH CCouponDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkColor(RGB(190, 219, 255));
	}
	return hbr;
}


void CCouponDlg::calculateCouponScores()
{
	for (int i = 0; i < m_nCouponCnt; i++)
	{
		m_coupons[i].calculateScore();
	}
}
