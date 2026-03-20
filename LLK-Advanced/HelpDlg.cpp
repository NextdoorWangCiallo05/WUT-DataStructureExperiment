#include "pch.h"
#include "HelpDlg.h"
#include "resource.h"
#include "framework.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CHelpDlg::CHelpDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_HELP_DIALOG, pParent)
{
}

void CHelpDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CHelpDlg, CDialogEx)
END_MESSAGE_MAP()

BOOL CHelpDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);
    SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

    // 调整窗口大小（适配客户区，消除边框偏移）
    CRect rtWin, rtClient;
    GetWindowRect(&rtWin);
    GetClientRect(&rtClient);
    int nSpanW = rtWin.Width() - rtClient.Width();
    int nSpanH = rtWin.Height() - rtClient.Height();
    MoveWindow(0, 0, HELP_WND_WIDTH + nSpanW, HELP_WND_HEIGHT + nSpanH);
    CenterWindow();

    // ===== 字体美化（类似 Word 观感）=====
    m_fontUi.CreatePointFont(100, _T("SimSun"));

    // 若你更想贴近英文版 Word，可改成：
    // m_fontUi.CreatePointFont(110, _T("Calibri"));

    SetFont(&m_fontUi);
    ApplyDialogFont(this);

    return TRUE;
}

void CHelpDlg::ApplyDialogFont(CWnd* pRoot)
{
    if (!pRoot || !::IsWindow(pRoot->GetSafeHwnd())) return;

    CWnd* pChild = pRoot->GetWindow(GW_CHILD);
    while (pChild)
    {
        pChild->SetFont(&m_fontUi);
        ApplyDialogFont(pChild); // 递归设置孙控件
        pChild = pChild->GetNextWindow();
    }
}