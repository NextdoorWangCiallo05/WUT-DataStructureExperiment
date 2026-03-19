#pragma once
#include "resource.h"
#include "global.h"
#include "GameControl.h"

// 游戏窗口尺寸
#define GAME_WND_WIDTH  800
#define GAME_WND_HEIGHT 600

class CGameDlg : public CDialogEx
{
public:
    CGameDlg(CWnd* pParent = nullptr);
    void SetGameMode(GameMode mode);

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_GAME_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point); // 鼠标左键抬起
    afx_msg void OnBnClickedBtnStart(); // 开始游戏按钮
    afx_msg void OnBnClickedBtnTip();    // 提示按钮响应
    afx_msg void OnBnClickedBtnReset();  // 重排按钮响应
	afx_msg void OnBnClickedBtnHelp(); //帮助按钮响应
    afx_msg void OnClose();
    DECLARE_MESSAGE_MAP()

private:
    // 绘图相关
    CDC m_dcMem;         // 游戏界面内存DC
    CDC m_dcElement;     // 元素位图DC
    CDC m_dcMask;        // 掩码位图DC
    CBitmap m_bmpBG;     // 游戏背景位图
    CBitmap m_bmpElement;// 元素位图
    CBitmap m_bmpMask;   // 掩码位图

    // 游戏相关
    GameMode m_gameMode;
    CGameControl m_gameCtr; // 游戏控制对象
    bool m_bPlaying;        // 游戏是否进行中
    bool m_bFirstSel;       // 是否是第一个选中点
    Vertex m_selVertex;     // 选中的顶点
    CRect m_rtGameRect;     // 游戏地图区域

    // 私有函数
    void InitBackground();  // 初始化背景
    void InitElement();     // 初始化元素位图
    void UpdateMap();       // 更新地图绘制
    void DrawTipFrame(int nRow, int nCol); // 绘制选中提示框
    void DrawTipLine(Vertex avPath[4], int nVexNum); // 绘制连通路径
};