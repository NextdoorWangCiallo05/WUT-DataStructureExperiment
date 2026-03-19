#pragma once
#include "resource.h"
#include "global.h"
#include "GameControl.h"

#define RELAX_WND_WIDTH  800
#define RELAX_WND_HEIGHT 600

class CRelaxDlg : public CDialogEx
{
public:
    CRelaxDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_RELAX_DIALOG };
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

    // helpers
    void InitBackground();
    void InitElement();
    void UpdateMap();
    void DrawTipFrame(int nRow, int nCol);
    void DrawTipLine(Vertex avPath[4], int nVexNum);
};