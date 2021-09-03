#pragma once
#include "CouponView.h"
#include "CouponTicket.h"
#include <vector>
using namespace std;
class Coupon
{
public:
	Coupon();
	Coupon(int val, int depth);
	~Coupon();
	BYTE *values;//for merged numbers
	int valueCnt;
	int depth;
	CArray<Coupon *, Coupon*> childs;
	void setValue(BYTE* val, int cnt, int dep);
	void combineValue(BYTE* val1, int cnt1, BYTE* val2, int cnt2, int dep);
	void add(int val, int dep);
	void add(BYTE* vals, int cnt, int dep);
	void remove(int idx);
	int find(int* val, int cnt);
	bool compareValues(Coupon *coupon1, Coupon *coupon2);
	bool merge(int idx1, int idx2, Coupon* parent);
	bool merge();
	Coupon* getCommonTree(Coupon* coupon1, Coupon* coupon2, bool& succ, bool bmain);
	Coupon* getDiffTree(Coupon* srcCoupon, Coupon* subCoupon);
public:
};

#include "TicketCreatorDlg.h"

class CTicketCreatorDlg;
class CouponView;

class OneCoupon
{
public:
	OneCoupon(){
		byteRowValues = 0;
	}
	~OneCoupon(){
	}
	st_byte_array** byteRowValues;
	int nRow;
	int nSingleLineCnt;
};

struct st_byte_array
{
	int common_value;
	BYTE* arr;
	int cnt;
	st_byte_array() {
		common_value = 0;
		arr = 0;
		cnt = 0;
	}
};
class CouponAgenda
{
public:
	CouponAgenda() {
		m_coupons = new Coupon * [1000];
		m_nCouponCnt = 0;
	}
	~CouponAgenda() {
		for (int i = 0; i < m_nCouponCnt; i++)
		{
			delete m_coupons[i];
		}
		delete m_coupons;
	}
	Coupon** m_coupons;
	int m_nCouponCnt;

private:

public:
	void addCoupon(Coupon* coupon) {
		if ((m_nCouponCnt+1)%1000 == 0) {
			Coupon** coupons = new Coupon * [m_nCouponCnt + 1000];
			for (int i = 0; i < m_nCouponCnt; i++)
			{
				coupons[i] = m_coupons[i];
			}
			delete[] m_coupons;
			m_coupons = coupons;
		}
		m_coupons[m_nCouponCnt] = coupon;
		m_nCouponCnt++;
	}
	void reset() {
		for (int i = 0; i < m_nCouponCnt; i++)
		{
			delete m_coupons[i];
		}
		delete m_coupons;
		m_coupons = new Coupon * [1000];
		m_nCouponCnt = 0;
	}
};

class CCouponDlg : public CDialog
{
	DECLARE_DYNAMIC(CCouponDlg)

public:
	CCouponDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CCouponDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_COUPONS };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCoupon();
	CouponView *m_view;
	int m_nRow;
	int m_nCol;
	int* m_pSelected;
	st_byte_array*** m_ppbyteCoupons;
	OneCoupon* m_coupons;
	int m_nCouponCnt;
	BYTE** m_ppbyteSingleLines;
	int m_nSingleLines;
	CTicketCreatorDlg* m_pParentDlg;
	Coupon* baseCoupon;
	void setMatchInfo(int nRow, int nCol);
	void DuplicatePermutation(int N, int R, int depth);
	bool CheckConditions(int *arr, int cnt);
	bool CheckPointLimits(int* arr, int cnt);
	void addToSingleLines(int* arr, int cnt);
	void resetSingleLines();
	void prepareSingleLines(int nRow, int nCol);
	bool CheckOneCondition(int* arr, int cnt, COneConditionDlg* oneCond);
	void makeCoupons();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void prepareBaseCoupon();
	bool mergeCoupons(int start);
	int makeTickets(Coupon* coupon, int depth, st_byte_array** base, int order);
	void removeCouponsArray();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void sortCoupons();
	int countSingleLinesOfCoupon();
	int testCount(Coupon* coupon);
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonOpen();
	void writeToFile(CString pathName);
	void readFromFile(CString pathName, TCHAR* szTime);
	afx_msg void OnBnClickedButtonCouponPrint();
	int m_nMaxCouponCount;
	void increaseCouponStorage();
	afx_msg void OnBnClickedButtonChange();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	int m_nFrom;
	int m_nTo;
	int m_nCondRow;
	int m_nCondCol;
};


