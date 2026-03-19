#include "pch.h"
#include "RelaxDlg.h"
#include "framework.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CRelaxDlg::CRelaxDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_RELAX_DIALOG, pParent)
{
    m_bPlaying = false;
    m_bFirstSel = true;
    memset(&m_selVertex, 0, sizeof(m_selVertex));
}

void CRelaxDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRelaxDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_LBUTTONUP()
    ON_BN_CLICKED(IDC_BTN_START, &CRelaxDlg::OnBnClickedBtnStart)
    ON_BN_CLICKED(IDC_BTN_TIP, &CRelaxDlg::OnBnClickedBtnTip)
    ON_BN_CLICKED(IDC_BTN_RESET, &CRelaxDlg::OnBnClickedBtnReset)
    ON_BN_CLICKED(IDC_BTN_HELP, &CRelaxDlg::OnBnClickedBtnHelp)
    ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CRelaxDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 设定休闲模式尺寸
    m_gameCtr.SetGameMode(MODE_RELAX);

    // 图标
    SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);
    SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

    InitBackground();
    InitElement();

    // 启动游戏
    m_gameCtr.StartGame();
    m_bPlaying = true;

    // 固定窗口大小并居中
    CRect rtClient, rtWin;
    GetWindowRect(&rtWin); GetClientRect(&rtClient);
    int nSpanW = rtWin.Width() - rtClient.Width();
    int nSpanH = rtWin.Height() - rtClient.Height();
    MoveWindow(0, 0, RELAX_WND_WIDTH + nSpanW, RELAX_WND_HEIGHT + nSpanH);
    CenterWindow();

    // 棋盘区域（根据休闲模式行列）
    m_rtGameRect.SetRect(GAME_TOP_X, GAME_TOP_Y,
        GAME_TOP_X + GAME_COLS * ELEM_SIZE,
        GAME_TOP_Y + GAME_ROWS * ELEM_SIZE);

    return TRUE;
}

void CRelaxDlg::InitBackground()
{
    CClientDC dc(this);
    m_bmpBG.LoadBitmap(IDB_GAME_BG);
    m_dcMem.CreateCompatibleDC(&dc);
    m_dcMem.SelectObject(&m_bmpBG);
}

void CRelaxDlg::InitElement()
{
    CClientDC dc(this);
    m_bmpElement.LoadBitmap(IDB_ELEMENT);
    m_bmpMask.LoadBitmap(IDB_MASK);
    m_dcElement.CreateCompatibleDC(&dc);
    m_dcMask.CreateCompatibleDC(&dc);
    m_dcElement.SelectObject(&m_bmpElement);
    m_dcMask.SelectObject(&m_bmpMask);
}

void CRelaxDlg::OnPaint()
{
    CPaintDC dc(this);
    dc.BitBlt(0, 0, RELAX_WND_WIDTH, RELAX_WND_HEIGHT, &m_dcMem, 0, 0, SRCCOPY);
}

void CRelaxDlg::UpdateMap()
{
    CClientDC dc(this);
    dc.BitBlt(m_rtGameRect.left, m_rtGameRect.top,
        m_rtGameRect.Width(), m_rtGameRect.Height(),
        &m_dcMem, m_rtGameRect.left, m_rtGameRect.top, SRCCOPY);

    for (int i = 0; i < GAME_ROWS; i++)
        for (int j = 0; j < GAME_COLS; j++)
        {
            int v = m_gameCtr.GetElement(i, j);
            if (v == BLANK) continue;
            int x = m_rtGameRect.left + j * ELEM_SIZE;
            int y = m_rtGameRect.top + i * ELEM_SIZE;
            dc.BitBlt(x, y, ELEM_SIZE, ELEM_SIZE, &m_dcMask, 0, v * ELEM_SIZE, SRCPAINT);
            dc.BitBlt(x, y, ELEM_SIZE, ELEM_SIZE, &m_dcElement, 0, v * ELEM_SIZE, SRCAND);
        }
}

void CRelaxDlg::DrawTipFrame(int r, int c)
{
    CClientDC dc(this);
    CRect rt(GAME_TOP_X + c * ELEM_SIZE, GAME_TOP_Y + r * ELEM_SIZE,
        GAME_TOP_X + (c + 1) * ELEM_SIZE, GAME_TOP_Y + (r + 1) * ELEM_SIZE);
    CPen pen(PS_SOLID, 2, RGB(255, 0, 0));
    CPen* pOld = dc.SelectObject(&pen);
    dc.SelectStockObject(NULL_BRUSH);
    dc.Rectangle(rt);
    dc.SelectObject(pOld);
}

void CRelaxDlg::DrawTipLine(Vertex avPath[4], int nVexNum)
{
    if (nVexNum < 2) return;
    CClientDC dc(this);
    CPen pen(PS_SOLID, 2, RGB(0, 255, 0));
    CPen* pOld = dc.SelectObject(&pen);
    CPoint ptStart(GAME_TOP_X + avPath[0].col * ELEM_SIZE + ELEM_SIZE / 2,
        GAME_TOP_Y + avPath[0].row * ELEM_SIZE + ELEM_SIZE / 2);
    dc.MoveTo(ptStart);
    for (int i = 1; i < nVexNum; i++)
    {
        CPoint ptEnd(GAME_TOP_X + avPath[i].col * ELEM_SIZE + ELEM_SIZE / 2,
            GAME_TOP_Y + avPath[i].row * ELEM_SIZE + ELEM_SIZE / 2);
        dc.LineTo(ptEnd);
    }
    dc.SelectObject(pOld);
    Sleep(180);
}

void CRelaxDlg::OnBnClickedBtnStart()
{
    m_gameCtr.StartGame();
    m_bPlaying = true;
    m_bFirstSel = true;
    UpdateMap();
    GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);
}

void CRelaxDlg::OnLButtonUp(UINT nFlags, CPoint pt)
{
    if (!m_bPlaying || !m_rtGameRect.PtInRect(pt))
    {
        CDialogEx::OnLButtonUp(nFlags, pt); return;
    }

    int r = (pt.y - m_rtGameRect.top) / ELEM_SIZE;
    int c = (pt.x - m_rtGameRect.left) / ELEM_SIZE;
    if (m_gameCtr.GetElement(r, c) == BLANK) { CDialogEx::OnLButtonUp(nFlags, pt); return; }

    if (m_bFirstSel)
    {
        m_gameCtr.SetFirstPoint(r, c);
        DrawTipFrame(r, c);
        m_bFirstSel = false;
    }
    else
    {
        m_gameCtr.SetSecPoint(r, c);
        Vertex path[4] = { 0 }; int n = 0;
        bool ok = m_gameCtr.Link(path, n);
        if (ok) {
            DrawTipLine(path, n); UpdateMap(); if (m_gameCtr.IsWin()) {
                MessageBox(_T("休闲模式胜利！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
                m_bPlaying = false; GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
            }
        }
        else { UpdateMap(); }
        m_bFirstSel = true;
    }
    CDialogEx::OnLButtonUp(nFlags, pt);
}

void CRelaxDlg::OnBnClickedBtnTip()
{
    if (!m_bPlaying || m_gameCtr.IsWin()) return;
    Vertex v1, v2, path[4] = { 0 }; int n = 0;
    if (m_gameCtr.FindTip(v1, v2, path, n))
    {
        DrawTipFrame(v1.row, v1.col);
        DrawTipFrame(v2.row, v2.col);
        DrawTipLine(path, n);
    }
    else
        MessageBox(_T("无可消除路径，试试重排~"), _T("提示"), MB_OK | MB_ICONINFORMATION);
}

void CRelaxDlg::OnBnClickedBtnReset()
{
    if (!m_bPlaying || m_gameCtr.IsWin()) return;
    m_gameCtr.ShuffleMap();
    UpdateMap();
}

void CRelaxDlg::OnBnClickedBtnHelp()
{
    CHelpDlg dlg; dlg.DoModal();
}

void CRelaxDlg::OnClose()
{
    m_gameCtr.EndGame();
    if (m_dcMem.GetSafeHdc()) m_dcMem.DeleteDC();
    if (m_dcElement.GetSafeHdc()) m_dcElement.DeleteDC();
    if (m_dcMask.GetSafeHdc()) m_dcMask.DeleteDC();
    if (m_bmpBG.GetSafeHandle()) m_bmpBG.DeleteObject();
    if (m_bmpElement.GetSafeHandle()) m_bmpElement.DeleteObject();
    if (m_bmpMask.GetSafeHandle()) m_bmpMask.DeleteObject();
    CDialogEx::OnClose();
}