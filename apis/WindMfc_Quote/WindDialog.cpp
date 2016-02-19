// WindDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WindMfc_Quote.h"
#include "WindDialog.h"
#include "afxdialogex.h"

#include "MdUserApi.h"

// CWindDialog dialog

IMPLEMENT_DYNAMIC(CWindDialog, CDialogEx)

CWindDialog::CWindDialog(CWnd* pParent /*=NULL*/)
: CDialogEx(CWindDialog::IDD, pParent)
{

}

CWindDialog::~CWindDialog()
{
}

void CWindDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWindDialog, CDialogEx)
    ON_MESSAGE(WM_WIND_START, &CWindDialog::OnWindStart)
    ON_MESSAGE(WM_WIND_STOP, &CWindDialog::OnWindStop)
    ON_MESSAGE(WM_WIND_QUERY_INSTRUMENT, &CWindDialog::OnWindQueryInstrument)
END_MESSAGE_MAP()


// CWindDialog message handlers


BOOL CWindDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    MoveWindow(0, 0, 0, 0);
    ShowWindow(SW_HIDE);
    ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
    PostMessage(WM_WIND_START);
    return TRUE;
}

afx_msg LRESULT CWindDialog::OnWindStart(WPARAM wParam, LPARAM lParam)
{
    if (MdUserApi != nullptr)
        MdUserApi->WindStart();
    return 0;
}


afx_msg LRESULT CWindDialog::OnWindStop(WPARAM wParam, LPARAM lParam)
{
    if (MdUserApi != nullptr)
        MdUserApi->WindStop();
    return 0;
}


afx_msg LRESULT CWindDialog::OnWindQueryInstrument(WPARAM wParam, LPARAM lParam)
{
    return 0;
}
