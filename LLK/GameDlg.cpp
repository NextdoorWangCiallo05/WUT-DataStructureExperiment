#include "pch.h"
#include "GameDlg.h"
#include "framework.h"
#include "LLK.h"
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
    ON_BN_CLICKED(IDC_BTN_TIP, &CGameDlg::OnBnClickedBtnTip)    // 提示按钮
    ON_BN_CLICKED(IDC_BTN_RESET, &CGameDlg::OnBnClickedBtnReset)// 重排按钮
    ON_BN_CLICKED(IDC_BTN_HELP, &CGameDlg::OnBnClickedBtnHelp)
    ON_WM_CLOSE()
END_MESSAGE_MAP()

void CGameDlg::SetGameMode(GameMode mode)
{
    // 设置并更新游戏模式
    m_gameCtr.SetGameMode(mode);

}

// 对话框初始化
BOOL CGameDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 设置窗口图标
    SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);
    SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

    // 初始化背景和元素
    InitBackground();
    InitElement();

    // 启动游戏
    m_gameCtr.StartGame();
    m_bPlaying = true;

    // 设置固定窗口尺寸（此处不变）
    CRect rtClient, rtWin;
    GetWindowRect(&rtWin);
    GetClientRect(&rtClient);
    int nSpanW = rtWin.Width() - rtClient.Width(); // 边框水平差值
    int nSpanH = rtWin.Height() - rtClient.Height(); // 边框垂直差值
    MoveWindow(0, 0, GAME_WND_WIDTH + nSpanW, GAME_WND_HEIGHT + nSpanH); // 使用固定窗口大小
    CenterWindow();

    // 设置棋盘逻辑区域（居中显示）
    m_rtGameRect.SetRect(GAME_TOP_X, GAME_TOP_Y,
        GAME_TOP_X + GAME_COLS * ELEM_SIZE,
        GAME_TOP_Y + GAME_ROWS * ELEM_SIZE);

    return TRUE;
}

// 初始化背景
void CGameDlg::InitBackground()
{
    CClientDC dc(this);
    // 从资源加载游戏背景
    m_bmpBG.LoadBitmap(IDB_GAME_BG);
    m_dcMem.CreateCompatibleDC(&dc);
    m_dcMem.SelectObject(&m_bmpBG);
}

// 初始化元素位图
void CGameDlg::InitElement()
{
    CClientDC dc(this);
    // 从资源加载元素和掩码位图
    m_bmpElement.LoadBitmap(IDB_ELEMENT);
    m_bmpMask.LoadBitmap(IDB_MASK);

    // 创建兼容DC
    m_dcElement.CreateCompatibleDC(&dc);
    m_dcMask.CreateCompatibleDC(&dc);

    // 选入位图
    m_dcElement.SelectObject(&m_bmpElement);
    m_dcMask.SelectObject(&m_bmpMask);
}

// 窗口绘制
void CGameDlg::OnPaint()
{
    CPaintDC dc(this);
    // 绘制背景
    dc.BitBlt(0, 0, GAME_WND_WIDTH, GAME_WND_HEIGHT, &m_dcMem, 0, 0, SRCCOPY);
}

// 更新地图绘制
void CGameDlg::UpdateMap()
{
    CClientDC dc(this);
    // 绘制背景，覆盖整个棋盘区域
    dc.BitBlt(
        m_rtGameRect.left, m_rtGameRect.top,
        m_rtGameRect.Width(), m_rtGameRect.Height(),
        &m_dcMem, m_rtGameRect.left, m_rtGameRect.top, SRCCOPY
    );

    // 绘制棋盘元素
    for (int i = 0; i < GAME_ROWS; i++)
    {
        for (int j = 0; j < GAME_COLS; j++)
        {
            int nElemVal = m_gameCtr.GetElement(i, j);
            if (nElemVal == BLANK) continue;  // 空白位置不绘制

            // 元素绘制位置：在逻辑棋盘区域内
            int nX = m_rtGameRect.left + j * ELEM_SIZE;
            int nY = m_rtGameRect.top + i * ELEM_SIZE;

            // 绘制元素，支持透明效果
            dc.BitBlt(nX, nY, ELEM_SIZE, ELEM_SIZE,
                &m_dcMask, 0, nElemVal * ELEM_SIZE, SRCPAINT);
            dc.BitBlt(nX, nY, ELEM_SIZE, ELEM_SIZE,
                &m_dcElement, 0, nElemVal * ELEM_SIZE, SRCAND);
        }
    }
}

// 绘制选中提示框
void CGameDlg::DrawTipFrame(int nRow, int nCol)
{
    CClientDC dc(this);
    // Calculate the coordinates of the frame
    CRect rtFrame;
    rtFrame.left = GAME_TOP_X + nCol * ELEM_SIZE;
    rtFrame.top = GAME_TOP_Y + nRow * ELEM_SIZE;
    rtFrame.right = rtFrame.left + ELEM_SIZE;
    rtFrame.bottom = rtFrame.top + ELEM_SIZE;

	// 创建红色画笔和透明画刷
    CPen pen(PS_SOLID, 2, RGB(255, 0, 0));
    CPen* pOldPen = dc.SelectObject(&pen);
    dc.SelectStockObject(NULL_BRUSH); // Transparent brush

    // Draw the rectangle for the tip
    dc.Rectangle(rtFrame);
    dc.SelectObject(pOldPen);
}

// 绘制连通路径
void CGameDlg::DrawTipLine(Vertex avPath[4], int nVexNum)
{
    if (nVexNum < 2) return;
    CClientDC dc(this);

    // 创建绿色画笔
    CPen pen(PS_SOLID, 2, RGB(0, 255, 0));
    CPen* pOldPen = dc.SelectObject(&pen);

    // 计算起点坐标（元素中心）
    CPoint ptStart;
    ptStart.x = GAME_TOP_X + avPath[0].col * ELEM_SIZE + ELEM_SIZE / 2;
    ptStart.y = GAME_TOP_Y + avPath[0].row * ELEM_SIZE + ELEM_SIZE / 2;
    dc.MoveTo(ptStart);

    // 绘制路径
    for (int i = 1; i < nVexNum; i++)
    {
        CPoint ptEnd;
        ptEnd.x = GAME_TOP_X + avPath[i].col * ELEM_SIZE + ELEM_SIZE / 2;
        ptEnd.y = GAME_TOP_Y + avPath[i].row * ELEM_SIZE + ELEM_SIZE / 2;
        dc.LineTo(ptEnd);
    }

    dc.SelectObject(pOldPen);
    Sleep(180); // 路径显示180ms，提升视觉效果
}

// 开始游戏按钮点击
void CGameDlg::OnBnClickedBtnStart()
{
    // 初始化游戏
    m_gameCtr.StartGame();
    m_bPlaying = true;
    m_bFirstSel = true;
    // 绘制地图
    UpdateMap();
    // 禁用开始按钮
    GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);
}

// 鼠标左键抬起事件
void CGameDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    // 游戏未启动时不响应点击
    if (!m_bPlaying)
    {
        CDialogEx::OnLButtonUp(nFlags, point);
        return;
    }

    // 点击位置不在棋盘区域内时不处理
    if (!m_rtGameRect.PtInRect(point))
    {
        CDialogEx::OnLButtonUp(nFlags, point);
        return;
    }

    // 将点击的点转换为棋盘的行列坐标
    int nRow = (point.y - m_rtGameRect.top) / ELEM_SIZE;
    int nCol = (point.x - m_rtGameRect.left) / ELEM_SIZE;

    // 如果点击的是空白位置，忽略
    if (m_gameCtr.GetElement(nRow, nCol) == BLANK)
    {
        CDialogEx::OnLButtonUp(nFlags, point);
        return;
    }

    // 处理点击的棋盘逻辑
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

        // 检查两点是否可以连通
        bool bSuc = m_gameCtr.Link(avPath, nVexNum);
        if (bSuc)
        {
            // 绘制连通路径并刷新界面
            DrawTipLine(avPath, nVexNum);
            UpdateMap();

            // 如果游戏胜利，显示提示，恢复按钮状态
            if (m_gameCtr.IsWin())
            {
                MessageBox(_T("恭喜你，游戏胜利！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
                m_bPlaying = false;
                GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
            }
        }
        else
        {
            // 否则清空选中状态
            UpdateMap();
        }

        m_bFirstSel = true;
    }

    CDialogEx::OnLButtonUp(nFlags, point);
}


// 提示按钮点击事件
void CGameDlg::OnBnClickedBtnTip()
{
    if (!m_bPlaying || m_gameCtr.IsWin()) return;

    Vertex v1, v2, avPath[4] = { 0 };
    int nVexNum = 0;
    // 调用控制层的提示接口，查找可消除的一对图案
    bool bFind = m_gameCtr.FindTip(v1, v2, avPath, nVexNum);
    if (bFind)
    {
        // 绘制选中框和连通路径
        DrawTipFrame(v1.row, v1.col);
        DrawTipFrame(v2.row, v2.col);
        DrawTipLine(avPath, nVexNum);
    }
    else
    {
        MessageBox(_T("当前无可用消除路径，请点击重排！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
    }
}

// 重排按钮点击事件
void CGameDlg::OnBnClickedBtnReset()
{
    if (!m_bPlaying || m_gameCtr.IsWin()) return;

    // 调用控制层的重排接口
    m_gameCtr.ShuffleMap();
    // 刷新地图绘制
    UpdateMap();
}

// 窗口关闭
void CGameDlg::OnClose()
{
    // 释放资源
    m_gameCtr.EndGame();
    if (m_dcMem.GetSafeHdc()) m_dcMem.DeleteDC();
    if (m_dcElement.GetSafeHdc()) m_dcElement.DeleteDC();
    if (m_dcMask.GetSafeHdc()) m_dcMask.DeleteDC();
    if (m_bmpBG.GetSafeHandle()) m_bmpBG.DeleteObject();
    if (m_bmpElement.GetSafeHandle()) m_bmpElement.DeleteObject();
    if (m_bmpMask.GetSafeHandle()) m_bmpMask.DeleteObject();

    CDialogEx::OnClose();
}

void CGameDlg::OnBnClickedBtnHelp()
{
    // 打开帮助对话框（模态），游戏进行中也可打开
    CHelpDlg helpDlg;
    helpDlg.DoModal();
}