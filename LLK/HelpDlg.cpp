#include "pch.h"
#include "HelpDlg.h"
#include "resource.h"
#include "framework.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 构造函数
CHelpDlg::CHelpDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_HELP_DIALOG, pParent)
{
}

// 数据交换（无自定义控件，仅默认实现）
void CHelpDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

// 消息映射（无额外消息，仅默认）
BEGIN_MESSAGE_MAP(CHelpDlg, CDialogEx)
END_MESSAGE_MAP()

// 对话框初始化
BOOL CHelpDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 设置窗口图标（与游戏主界面一致）
    SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);
    SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

    // 调整窗口大小（适配客户区，消除边框偏移）
    CRect rtWin, rtClient;
    GetWindowRect(&rtWin);
    GetClientRect(&rtClient);
    int nSpanW = rtWin.Width() - rtClient.Width();
    int nSpanH = rtWin.Height() - rtClient.Height();
    MoveWindow(0, 0, HELP_WND_WIDTH + nSpanW, HELP_WND_HEIGHT + nSpanH);
    // 窗口居中显示
    CenterWindow();

    return TRUE;
}