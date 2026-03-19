#pragma once
#include "resource.h"
#include "global.h"
#include "GameControl.h"

#define TIMED_WND_WIDTH  800
#define TIMED_WND_HEIGHT 600
#define TIMER_ID_COUNTDOWN 1
#define TIMER_INTERVAL_MS 1000
#define TIMED_LIMIT_SECONDS (10 * 60)   // 10 分钟

class CTimedDlg : public CDialogEx
{
public:
    CTimedDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_TIMED_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnBnClickedBtnStart();
    afx_msg void OnBnClickedBtnTip();
    afx_msg void OnBnClickedBtnReset();
    afx_msg void OnBnClickedBtnHelp();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnClose();
    DECLARE_MESSAGE_MAP()

private:
    // 绘图
    CDC     m_dcMem, m_dcElement, m_dcMask;
    CBitmap m_bmpBG, m_bmpElement, m_bmpMask;

    // 游戏
    CGameControl m_gameCtr;
    bool  m_bPlaying;
    bool  m_bFirstSel;
    Vertex m_selVertex;
    CRect  m_rtGameRect;

    // 计时
    int    m_nSecondsLeft;

    // helpers
    void InitBackground();
    void InitElement();
    void UpdateMap();
    void DrawTipFrame(int nRow, int nCol);
    void DrawTipLine(Vertex avPath[4], int nVexNum);
    void StartCountdown();
    void StopCountdown();
    void UpdateTimerText();
};