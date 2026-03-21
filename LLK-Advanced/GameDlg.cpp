#include "pch.h"
#include "GameDlg.h"
#include "SettingDlg.h"
#include "LLKDlg.h"
#include "framework.h"
#include "LLK.h"
#include "HelpDlg.h"
#include "PngLoader.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGameDlg::CGameDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_GAME_DIALOG, pParent)
{
    m_bPlaying = false;
    m_bFirstSel = true;
    m_gameMode = MODE_CLASSIC;
    memset(&m_selVertex, 0, sizeof(m_selVertex));
}

void CGameDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGameDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_LBUTTONUP()
    ON_BN_CLICKED(IDC_BTN_START, &CGameDlg::OnBnClickedBtnStart)
    ON_BN_CLICKED(IDC_BTN_PAUSE, &CGameDlg::OnBnClickedBtnPause)
    ON_BN_CLICKED(IDC_BTN_TIP, &CGameDlg::OnBnClickedBtnTip)
    ON_BN_CLICKED(IDC_BTN_RESET, &CGameDlg::OnBnClickedBtnReset)
    ON_BN_CLICKED(IDC_BTN_SET, &CGameDlg::OnBnClickedBtnSetting)
    ON_BN_CLICKED(IDC_BTN_HELP, &CGameDlg::OnBnClickedBtnHelp)
    ON_WM_CLOSE()
END_MESSAGE_MAP()

void CGameDlg::SetGameMode(GameMode mode)
{
    m_gameCtr.SetGameMode(mode);
}

BOOL CGameDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);
    SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

    InitBackground();
    InitElement();

    m_bPlaying = false;
    m_bFirstSel = true;

    // 删除固定 MoveWindow(800x600)，避免覆盖系统DPI缩放
    // 保留居中即可
    CenterWindow();

    m_rtGameRect.SetRect(GAME_TOP_X, GAME_TOP_Y,
        GAME_TOP_X + GAME_COLS * ELEM_SIZE,
        GAME_TOP_Y + GAME_ROWS * ELEM_SIZE);

    GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);

    UpdateMap();
    return TRUE;
}

void CGameDlg::InitBackground()
{
    m_pngBg = CPngLoader::LoadFromResource(AfxGetInstanceHandle(), IDR_PNG_GAME_BG, L"PNG");
}

void CGameDlg::InitElement()
{
    m_pngElement = CPngLoader::LoadFromResource(AfxGetInstanceHandle(), IDR_PNG_ELEMENT, L"PNG");
}

void CGameDlg::OnPaint()
{
    CPaintDC dc(this);
    Gdiplus::Graphics g(dc.GetSafeHdc());

    CRect rc;
    GetClientRect(&rc);

    if (m_pngBg)
        g.DrawImage(m_pngBg.get(), 0, 0, rc.Width(), rc.Height());

    if (!m_bPlaying || !m_pngElement) return;

    for (int i = 0; i < GAME_ROWS; i++)
    {
        for (int j = 0; j < GAME_COLS; j++)
        {
            int nElemVal = m_gameCtr.GetElement(i, j);
            if (nElemVal == BLANK) continue;

            int nX = m_rtGameRect.left + j * ELEM_SIZE;
            int nY = m_rtGameRect.top + i * ELEM_SIZE;

            g.DrawImage(m_pngElement.get(),
                Gdiplus::Rect(nX, nY, ELEM_SIZE, ELEM_SIZE),
                0, nElemVal * ELEM_SIZE, ELEM_SIZE, ELEM_SIZE, Gdiplus::UnitPixel);
        }
    }
}

void CGameDlg::UpdateMap()
{
    Invalidate(FALSE);
    UpdateWindow();
}

void CGameDlg::DrawTipFrame(int nRow, int nCol)
{
    CClientDC dc(this);
    CRect rtFrame;
    rtFrame.left = GAME_TOP_X + nCol * ELEM_SIZE;
    rtFrame.top = GAME_TOP_Y + nRow * ELEM_SIZE;
    rtFrame.right = rtFrame.left + ELEM_SIZE;
    rtFrame.bottom = rtFrame.top + ELEM_SIZE;

    CPen pen(PS_SOLID, 2, RGB(255, 0, 0));
    CPen* pOldPen = dc.SelectObject(&pen);
    dc.SelectStockObject(NULL_BRUSH);
    dc.Rectangle(rtFrame);
    dc.SelectObject(pOldPen);
}

void CGameDlg::DrawTipLine(Vertex avPath[4], int nVexNum)
{
    if (nVexNum < 2) return;
    CClientDC dc(this);

    CPen pen(PS_SOLID, 2, RGB(0, 255, 0));
    CPen* pOldPen = dc.SelectObject(&pen);

    CPoint ptStart;
    ptStart.x = GAME_TOP_X + avPath[0].col * ELEM_SIZE + ELEM_SIZE / 2;
    ptStart.y = GAME_TOP_Y + avPath[0].row * ELEM_SIZE + ELEM_SIZE / 2;
    dc.MoveTo(ptStart);

    for (int i = 1; i < nVexNum; i++)
    {
        CPoint ptEnd;
        ptEnd.x = GAME_TOP_X + avPath[i].col * ELEM_SIZE + ELEM_SIZE / 2;
        ptEnd.y = GAME_TOP_Y + avPath[i].row * ELEM_SIZE + ELEM_SIZE / 2;
        dc.LineTo(ptEnd);
    }

    dc.SelectObject(pOldPen);
    Sleep(180);
}

void CGameDlg::OnBnClickedBtnStart()
{
    m_gameCtr.StartGame();
    m_bPlaying = true;
    m_bFirstSel = true;
    UpdateMap();
    GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);
}

void CGameDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (!m_bPlaying)
    {
        CDialogEx::OnLButtonUp(nFlags, point);
        return;
    }

    if (!m_rtGameRect.PtInRect(point))
    {
        CDialogEx::OnLButtonUp(nFlags, point);
        return;
    }

    int nRow = (point.y - m_rtGameRect.top) / ELEM_SIZE;
    int nCol = (point.x - m_rtGameRect.left) / ELEM_SIZE;

    if (m_gameCtr.GetElement(nRow, nCol) == BLANK)
    {
        CDialogEx::OnLButtonUp(nFlags, point);
        return;
    }

    if (m_bFirstSel)
    {
        m_gameCtr.SetFirstPoint(nRow, nCol);
        DrawTipFrame(nRow, nCol);
        m_bFirstSel = false;
    }
    else
    {
        m_gameCtr.SetSecPoint(nRow, nCol);
        Vertex avPath[4] = { 0 };
        int nVexNum = 0;

        bool bSuc = m_gameCtr.Link(avPath, nVexNum);
        if (bSuc)
        {
            DrawTipLine(avPath, nVexNum);
            UpdateMap();

            if (m_gameCtr.IsWin())
            {
                MessageBox(_T("恭喜你，游戏胜利！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
                m_bPlaying = false;
                GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
            }
        }
        else
        {
            UpdateMap();
        }

        m_bFirstSel = true;
    }

    CDialogEx::OnLButtonUp(nFlags, point);
}

void CGameDlg::OnBnClickedBtnPause()
{
    MessageBox(
        _T("非计时模式无需暂停哦！"),
        _T("提示"),
        MB_OK | MB_ICONINFORMATION
    );
}


void CGameDlg::OnBnClickedBtnTip()
{
    if (!m_bPlaying || m_gameCtr.IsWin()) return;

    Vertex v1, v2, avPath[4] = { 0 };
    int nVexNum = 0;
    bool bFind = m_gameCtr.FindTip(v1, v2, avPath, nVexNum);
    if (bFind)
    {
        DrawTipFrame(v1.row, v1.col);
        DrawTipFrame(v2.row, v2.col);
        DrawTipLine(avPath, nVexNum);
    }
    else
    {
        MessageBox(_T("当前无可用消除路径，请点击重排！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
    }
}

void CGameDlg::OnBnClickedBtnReset()
{
    if (!m_bPlaying || m_gameCtr.IsWin()) return;
    m_gameCtr.ShuffleMap();
    UpdateMap();
}

void CGameDlg::OnClose()
{
    m_gameCtr.EndGame();
    m_pngBg.reset();
    m_pngElement.reset();
    CDialogEx::OnClose();
}

void CGameDlg::OnBnClickedBtnSetting()
{
    CSettingDlg dlg;
    dlg.DoModal();
}

void CGameDlg::OnBnClickedBtnHelp()
{
    CHelpDlg helpDlg;
    helpDlg.DoModal();
}