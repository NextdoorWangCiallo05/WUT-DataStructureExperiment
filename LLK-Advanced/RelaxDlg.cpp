#include "pch.h"
#include "RelaxDlg.h"
#include "HelpDlg.h"
#include "LLKDlg.h"
#include "SettingDlg.h"
#include "framework.h"
#include "PngLoader.h"
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
    ON_BN_CLICKED(IDC_BTN_SET, &CRelaxDlg::OnBnClickedBtnSetting)
    ON_BN_CLICKED(IDC_BTN_HELP, &CRelaxDlg::OnBnClickedBtnHelp)
    ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CRelaxDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_gameCtr.SetGameMode(MODE_RELAX);

    SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);
    SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

    InitBackground();
    InitElement();

    m_bPlaying = false;
    m_bFirstSel = true;

    // 删除固定 MoveWindow(800x600)
    CenterWindow();

    m_rtGameRect.SetRect(GAME_TOP_X, GAME_TOP_Y,
        GAME_TOP_X + GAME_COLS * ELEM_SIZE,
        GAME_TOP_Y + GAME_ROWS * ELEM_SIZE);

    GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);

    UpdateMap();
    return TRUE;
}


void CRelaxDlg::InitBackground()
{
    m_pngBg = CPngLoader::LoadFromResource(AfxGetInstanceHandle(), IDR_PNG_GAME_BG, L"PNG");
}

void CRelaxDlg::InitElement()
{
    m_pngElement = CPngLoader::LoadFromResource(AfxGetInstanceHandle(), IDR_PNG_ELEMENT, L"PNG");
}

void CRelaxDlg::OnPaint()
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
            int v = m_gameCtr.GetElement(i, j);
            if (v == BLANK) continue;

            int x = m_rtGameRect.left + j * ELEM_SIZE;
            int y = m_rtGameRect.top + i * ELEM_SIZE;

            g.DrawImage(m_pngElement.get(),
                Gdiplus::Rect(x, y, ELEM_SIZE, ELEM_SIZE),
                0, v * ELEM_SIZE, ELEM_SIZE, ELEM_SIZE, Gdiplus::UnitPixel);
        }
    }
}

void CRelaxDlg::UpdateMap()
{
    Invalidate(FALSE);
    UpdateWindow();
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

void CRelaxDlg::OnBnClickedBtnSetting()
{
    CSettingDlg dlg;
    dlg.DoModal();
}

void CRelaxDlg::OnBnClickedBtnHelp()
{
    CHelpDlg dlg;
    dlg.DoModal();
}

void CRelaxDlg::OnClose()
{
    m_gameCtr.EndGame();
    m_pngBg.reset();
    m_pngElement.reset();
    CDialogEx::OnClose();
}

