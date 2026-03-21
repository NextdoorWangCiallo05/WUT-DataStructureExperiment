#include "pch.h"
#include "framework.h"
#include "AboutDlg.h"
#include "AudioManager.h"
#include "LLK.h"
#include "LLKDlg.h"
#include "GameDlg.h"
#include "RelaxDlg.h"
#include "TimedDlg.h"
#include "SettingDlg.h"
#include "HelpDlg.h"
#include "PngLoader.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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
    ON_BN_CLICKED(IDC_BTN_RELAX, &CLLKDlg::OnBnClickedBtnRelax)
    ON_BN_CLICKED(IDC_BTN_HELP, &CLLKDlg::OnBnClickedBtnHelp)
    ON_BN_CLICKED(IDC_BTN_TIMED, &CLLKDlg::OnBnClickedBtnTimed)
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_BTN_RANK, &CLLKDlg::OnBnClickedBtnRank)
    ON_BN_CLICKED(IDC_BTN_SETTING, &CLLKDlg::OnBnClickedBtnSetting)
    ON_BN_CLICKED(IDC_BTN_ABOUT, &CLLKDlg::OnBnClickedBtnAbout)
    ON_BN_CLICKED(IDC_BTN_MUSIC_TOGGLE, &CLLKDlg::OnBnClickedBtnMusicToggle)
    ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CLLKDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    InitBackground();

    // 记录初始布局比例（以资源编辑器布局为基准）
    CaptureInitialLayoutRatios();

    // 首次应用一次（确保初始状态一致）
    ApplyResponsiveLayout();

    UpdateMusicToggleText();

    return TRUE;
}

void CLLKDlg::CaptureInitialLayoutRatios()
{
    m_btnRatios.RemoveAll();
    GetClientRect(&m_initClientRect);
    if (m_initClientRect.Width() <= 0 || m_initClientRect.Height() <= 0) return;

    const UINT kBtnIds[] = {
        IDC_BTN_BASIC,      // 你的“经典模式”按钮ID
        IDC_BTN_RELAX,        // 休闲模式
        IDC_BTN_TIMED,        // 计时模式
        IDC_BTN_SETTING,      // 设置
        IDC_BTN_RANK,         // 排行榜
        IDC_BTN_HELP,         // 帮助
        IDC_BTN_ABOUT,
        IDC_BTN_MUSIC_TOGGLE  // 音乐开关
    };

    for (UINT id : kBtnIds)
    {
        CWnd* p = GetDlgItem(id);
        if (!p) continue;

        CRect rc;
        p->GetWindowRect(&rc);
        ScreenToClient(&rc);

        BtnRectRatio br;
        br.id = id;
        br.l = (double)rc.left / m_initClientRect.Width();
        br.t = (double)rc.top / m_initClientRect.Height();
        br.r = (double)rc.right / m_initClientRect.Width();
        br.b = (double)rc.bottom / m_initClientRect.Height();
        m_btnRatios.Add(br);
    }
}

void CLLKDlg::ApplyResponsiveLayout()
{
    CRect rcClient;
    GetClientRect(&rcClient);
    if (rcClient.Width() <= 0 || rcClient.Height() <= 0) return;
    if (m_btnRatios.GetCount() == 0) return;

    for (INT_PTR i = 0; i < m_btnRatios.GetCount(); ++i)
    {
        const BtnRectRatio& br = m_btnRatios[i];
        CWnd* p = GetDlgItem(br.id);
        if (!p) continue;

        int l = (int)(br.l * rcClient.Width());
        int t = (int)(br.t * rcClient.Height());
        int r = (int)(br.r * rcClient.Width());
        int b = (int)(br.b * rcClient.Height());

        p->SetWindowPos(nullptr, l, t, r - l, b - t, SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

void CLLKDlg::SetupButton(CMFCButton& btn, UINT nID, UINT nImageID, LPCTSTR lpszText)
{
    // 子类化控件
    btn.SubclassDlgItem(nID, this);

    // 设置图标（普通状态）
    btn.SetImage(nImageID);  // 假设资源是 BMP

    // 设置文字
    btn.SetWindowText(lpszText);

    // 设置文字颜色
    btn.SetTextColor(RGB(255, 255, 255));      // 普通白色
    btn.SetTextHotColor(RGB(200, 200, 200));   // 悬浮/按下灰色

    // 设置背景色（透明以显示背景图）
    btn.SetFaceColor(RGB(0, 0, 0), FALSE);     // FALSE 表示不使用自定义背景色（透明）
    btn.m_bTransparent = TRUE; // 透明背景

    // 设置按钮样式为扁平（可选）
    btn.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;

}

// 初始化背景图
void CLLKDlg::InitBackground()
{
    m_pngBG = CPngLoader::LoadFromResource(AfxGetInstanceHandle(), IDR_PNG_MAIN_BG, L"PNG");
}

// 绘制背景图
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
        Gdiplus::Graphics g(dc.GetSafeHdc());
        if (m_pngBG)
        {
            CRect rc;
            GetClientRect(&rc);
            g.DrawImage(m_pngBG.get(), 0, 0, rc.Width(), rc.Height());
        }
        CDialogEx::OnPaint();
    }
}

// 经典模式按钮事件
void CLLKDlg::OnBnClickedBtnBasic()
{
    ShowWindow(SW_HIDE);
    CGameDlg gameDlg;
    gameDlg.SetGameMode(MODE_CLASSIC);
    gameDlg.DoModal();
    ShowWindow(SW_SHOW);
}

// 休闲模式按钮事件
void CLLKDlg::OnBnClickedBtnRelax()
{
    ShowWindow(SW_HIDE);
    CRelaxDlg dlg;
    dlg.DoModal();
    ShowWindow(SW_SHOW);
}

// 关闭窗口时释放背景图资源
void CLLKDlg::OnClose()
{
    m_pngBG.reset();
    CDialogEx::OnClose();
}

// 拖动图标时显示应用程序图标
HCURSOR CLLKDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

// 帮助按钮事件
void CLLKDlg::OnBnClickedBtnHelp()
{
    CHelpDlg helpDlg;
    helpDlg.DoModal();
}

// 计时模式按钮事件
void CLLKDlg::OnBnClickedBtnTimed()
{
    ShowWindow(SW_HIDE);
    CTimedDlg dlg;
    dlg.DoModal();
    ShowWindow(SW_SHOW);
}

// 排行榜按钮事件
void CLLKDlg::OnBnClickedBtnRank()
{
    MessageBox(
        _T("排行榜功能正在开发中，敬请期待！"),
        _T("提示"),
        MB_OK | MB_ICONINFORMATION
    );
}

// 设置按钮事件
void CLLKDlg::OnBnClickedBtnSetting()
{
    CSettingDlg dlg;
    dlg.DoModal();
}

// 音乐开关按钮事件
void CLLKDlg::OnBnClickedBtnMusicToggle()
{
    auto& audio = CAudioManager::Instance();
    audio.SetMusicEnabled(!audio.IsMusicEnabled());
    UpdateMusicToggleText();
}

// 更新音乐开关按钮文本
void CLLKDlg::UpdateMusicToggleText()
{
    CString txt = CAudioManager::Instance().IsMusicEnabled()
        ? _T("音乐：开")
        : _T("音乐：关");
    CWnd* btn = GetDlgItem(IDC_BTN_MUSIC_TOGGLE);
    if (btn) btn->SetWindowText(txt);
}

void CLLKDlg::OnBnClickedBtnAbout()
{
    CAboutDlg dlg;
    dlg.DoModal();
}

void CLLKDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    if (cx <= 0 || cy <= 0) return;
    if (!::IsWindow(GetSafeHwnd())) return;

    ApplyResponsiveLayout();
    Invalidate(FALSE);
}