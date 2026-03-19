#include "pch.h"
#include "TimedDlg.h"
#include "framework.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTimedDlg::CTimedDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_TIMED_DIALOG, pParent)
{
    m_bPlaying = false;
    m_bFirstSel = true;
    m_nSecondsLeft = TIMED_LIMIT_SECONDS;
    memset(&m_selVertex, 0, sizeof(m_selVertex));
}

void CTimedDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTimedDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_LBUTTONUP()
    ON_BN_CLICKED(IDC_BTN_START, &CTimedDlg::OnBnClickedBtnStart)
    ON_BN_CLICKED(IDC_BTN_TIP, &CTimedDlg::OnBnClickedBtnTip)
    ON_BN_CLICKED(IDC_BTN_RESET, &CTimedDlg::OnBnClickedBtnReset)
    ON_BN_CLICKED(IDC_BTN_HELP, &CTimedDlg::OnBnClickedBtnHelp)
    ON_WM_TIMER()
    ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CTimedDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_gameCtr.SetGameMode(MODE_TIMED);

    SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);
    SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

    InitBackground();
    InitElement();

    m_gameCtr.StartGame();
    m_bPlaying = true;
    StartCountdown();

    CRect rtClient, rtWin;
    GetWindowRect(&rtWin); GetClientRect(&rtClient);
    int nSpanW = rtWin.Width() - rtClient.Width();
    int nSpanH = rtWin.Height() - rtClient.Height();
    MoveWindow(0, 0, TIMED_WND_WIDTH + nSpanW, TIMED_WND_HEIGHT + nSpanH);
    CenterWindow();

    m_rtGameRect.SetRect(GAME_TOP_X, GAME_TOP_Y,
        GAME_TOP_X + GAME_COLS * ELEM_SIZE,
        GAME_TOP_Y + GAME_ROWS * ELEM_SIZE);

    UpdateTimerText();
    return TRUE;
}

void CTimedDlg::InitBackground()
{
    CClientDC dc(this);
    m_bmpBG.LoadBitmap(IDB_GAME_BG);
    m_dcMem.CreateCompatibleDC(&dc);
    m_dcMem.SelectObject(&m_bmpBG);
}

void CTimedDlg::InitElement()
{
    CClientDC dc(this);
    m_bmpElement.LoadBitmap(IDB_ELEMENT);
    m_bmpMask.LoadBitmap(IDB_MASK);
    m_dcElement.CreateCompatibleDC(&dc);
    m_dcMask.CreateCompatibleDC(&dc);
    m_dcElement.SelectObject(&m_bmpElement);
    m_dcMask.SelectObject(&m_bmpMask);
}

void CTimedDlg::OnPaint()
{
    CPaintDC dc(this);
    dc.BitBlt(0, 0, TIMED_WND_WIDTH, TIMED_WND_HEIGHT, &m_dcMem, 0, 0, SRCCOPY);
}

void CTimedDlg::UpdateMap()
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

void CTimedDlg::DrawTipFrame(int r, int c)
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

void CTimedDlg::DrawTipLine(Vertex avPath[4], int nVexNum)
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

void CTimedDlg::StartCountdown()
{
    m_nSecondsLeft = TIMED_LIMIT_SECONDS;
    SetTimer(TIMER_ID_COUNTDOWN, TIMER_INTERVAL_MS, NULL);
    UpdateTimerText();
}

void CTimedDlg::StopCountdown()
{
    KillTimer(TIMER_ID_COUNTDOWN);
}

void CTimedDlg::UpdateTimerText()
{
    int m = m_nSecondsLeft / 60;
    int s = m_nSecondsLeft % 60;
    CString text;
    text.Format(_T("剩余时间：%02d:%02d"), m, s);
    CWnd* pStatic = GetDlgItem(IDC_STATIC_TIMER);
    if (pStatic) pStatic->SetWindowText(text);
}

void CTimedDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == TIMER_ID_COUNTDOWN)
    {
        if (!m_bPlaying) { StopCountdown(); return; }

        if (--m_nSecondsLeft <= 0)
        {
            StopCountdown();
            m_bPlaying = false;
            UpdateTimerText();
            MessageBox(_T("时间耗尽，游戏失败！"), _T("计时模式"), MB_OK | MB_ICONSTOP);
            GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
        }
        else
        {
            UpdateTimerText();
        }
    }
    CDialogEx::OnTimer(nIDEvent);
}

void CTimedDlg::OnBnClickedBtnStart()
{
    m_gameCtr.StartGame();
    m_bPlaying = true;
    m_bFirstSel = true;
    StartCountdown();
    UpdateMap();
    GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);
}

void CTimedDlg::OnLButtonUp(UINT nFlags, CPoint pt)
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
        if (ok)
        {
            DrawTipLine(path, n);
            UpdateMap();
            if (m_gameCtr.IsWin())
            {
                StopCountdown();
                MessageBox(_T("恭喜完成计时模式！"), _T("成功"), MB_OK | MB_ICONINFORMATION);
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
    CDialogEx::OnLButtonUp(nFlags, pt);
}

void CTimedDlg::OnBnClickedBtnTip()
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

void CTimedDlg::OnBnClickedBtnReset()
{
    if (!m_bPlaying || m_gameCtr.IsWin()) return;
    m_gameCtr.ShuffleMap();
    UpdateMap();
}

void CTimedDlg::OnBnClickedBtnHelp()
{
    CHelpDlg dlg; dlg.DoModal();
}

void CTimedDlg::OnClose()
{
    StopCountdown();
    m_gameCtr.EndGame();
    if (m_dcMem.GetSafeHdc()) m_dcMem.DeleteDC();
    if (m_dcElement.GetSafeHdc()) m_dcElement.DeleteDC();
    if (m_dcMask.GetSafeHdc()) m_dcMask.DeleteDC();
    if (m_bmpBG.GetSafeHandle()) m_bmpBG.DeleteObject();
    if (m_bmpElement.GetSafeHandle()) m_bmpElement.DeleteObject();
    if (m_bmpMask.GetSafeHandle()) m_bmpMask.DeleteObject();
    CDialogEx::OnClose();
}