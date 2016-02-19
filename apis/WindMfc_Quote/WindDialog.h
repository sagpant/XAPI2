#pragma once

#include "afxwin.h"
#include "Resource.h"

class CMdUserApi;

#define WM_WIND_START              WM_USER + 1
#define WM_WIND_STOP               WM_USER + 2
#define WM_WIND_QUERY_INSTRUMENT   WM_USER + 3
#define WM_WIND_SUBSCRIBE          WM_USER + 4
#define WM_WIND_UNSUBSCRIBE        WM_USER + 5

// CWindDialog dialog

class CWindDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CWindDialog)
public:
	CWindDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWindDialog();

// Dialog Data
	enum { IDD = IDD_WIND_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    CMdUserApi* MdUserApi;
protected:
    afx_msg LRESULT OnWindStart(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnWindStop(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnWindQueryInstrument(WPARAM wParam, LPARAM lParam);
};
