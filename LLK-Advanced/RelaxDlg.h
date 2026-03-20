#pragma once
#include "resource.h"
#include "global.h"
#include "GameControl.h"
#include <memory>
#include <gdiplus.h>

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
    afx_msg void OnBnClickedBtnSetting();
    afx_msg void OnBnClickedBtnHelp();
    afx_msg void OnClose();
    DECLARE_MESSAGE_MAP()

private:
    std::unique_ptr<Gdiplus::Bitmap> m_pngBg;
    std::unique_ptr<Gdiplus::Bitmap> m_pngElement;

    CGameControl m_gameCtr;
    bool  m_bPlaying;
    bool  m_bFirstSel;
    Vertex m_selVertex;
    CRect  m_rtGameRect;

    void InitBackground();
    void InitElement();
    void UpdateMap();
    void DrawTipFrame(int nRow, int nCol);
    void DrawTipLine(Vertex avPath[4], int nVexNum);
};