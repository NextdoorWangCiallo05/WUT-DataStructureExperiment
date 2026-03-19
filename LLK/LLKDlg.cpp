#include "pch.h"
#include "framework.h"
#include "LLK.h"
#include "LLKDlg.h"
#include "GameDlg.h" // 游戏界面头文件
#include "RelaxDlg.h"
#include "TimedDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLLKDlg 对话框

CLLKDlg::CLLKDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_LLK_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLLKDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLLKDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BTN_BASIC, &CLLKDlg::OnBnClickedBtnBasic)
    ON_BN_CLICKED(IDC_BTN_RELAX, &CLLKDlg::OnBnClickedBtnRelax) // 休闲模式
    ON_BN_CLICKED(IDC_BTN_HELP, &CLLKDlg::OnBnClickedBtnHelp)
    ON_BN_CLICKED(IDC_BTN_TIMED, &CLLKDlg::OnBnClickedBtnTimed)
    ON_WM_CLOSE()
END_MESSAGE_MAP()

// 对话框初始化
BOOL CLLKDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 设置窗口图标
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    // 初始化背景
    InitBackground();

    // 调整窗口大小，适配800x600客户区
    CRect rtWin, rtClient;
    GetWindowRect(&rtWin);
    GetClientRect(&rtClient);
    int nSpanW = rtWin.Width() - rtClient.Width();
    int nSpanH = rtWin.Height() - rtClient.Height();
    MoveWindow(0, 0, WND_WIDTH + nSpanW, WND_HEIGHT + nSpanH);
    CenterWindow(); // 窗口居中

    return TRUE;
}

// 初始化背景位图
void CLLKDlg::InitBackground()
{
    CClientDC dc(this);
    // 从资源加载背景位图
    m_bmpBG.LoadBitmap(IDB_MAIN_BG);
    // 创建兼容内存DC
    m_dcMem.CreateCompatibleDC(&dc);
    m_dcMem.SelectObject(&m_bmpBG);
}

// 窗口绘制消息
void CLLKDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CPaintDC dc(this);
        // 绘制背景位图，拉伸填充整个客户区
        dc.BitBlt(0, 0, WND_WIDTH, WND_HEIGHT, &m_dcMem, 0, 0, SRCCOPY);
        CDialogEx::OnPaint();
    }
}

void CLLKDlg::OnBnClickedBtnBasic()
{
    // 隐藏主界面，打开游戏界面，设置为经典模式
    ShowWindow(SW_HIDE);
    CGameDlg gameDlg;
    gameDlg.SetGameMode(MODE_CLASSIC);  // 默认经典模式
    gameDlg.DoModal();
    ShowWindow(SW_SHOW); // 游戏结束后返回主界面
}

void CLLKDlg::OnBnClickedBtnRelax()
{
    ShowWindow(SW_HIDE);
    CRelaxDlg dlg;
    dlg.DoModal();
    ShowWindow(SW_SHOW);
}

// 窗口关闭消息
void CLLKDlg::OnClose()
{
    // 释放资源
    if (m_dcMem.GetSafeHdc())
        m_dcMem.DeleteDC();
    if (m_bmpBG.GetSafeHandle())
        m_bmpBG.DeleteObject();
    CDialogEx::OnClose();
}

// 系统生成的拖放图标处理，无需修改
HCURSOR CLLKDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CLLKDlg::OnBnClickedBtnHelp()
{
    // 打开帮助对话框（模态）
    CHelpDlg helpDlg;
    helpDlg.DoModal();
}

void CLLKDlg::OnBnClickedBtnTimed()
{
    ShowWindow(SW_HIDE);
    CTimedDlg dlg;
    dlg.DoModal();
    ShowWindow(SW_SHOW);
}