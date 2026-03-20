#pragma once
#include "resource.h"
#include "global.h"
#include <memory>
#include <gdiplus.h>

#define WND_WIDTH  800
#define WND_HEIGHT 600

// 按钮布局比例结构体
struct BtnRectRatio
{
    UINT id;
    double l, t, r, b;
};

class CLLKDlg : public CDialogEx
{
public:
	CLLKDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LLK_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedBtnBasic();
	afx_msg void OnBnClickedBtnRelax();
	afx_msg void OnBnClickedBtnHelp();
	afx_msg void OnBnClickedBtnTimed();
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnMusicToggle();
	void UpdateMusicToggleText();
	void CaptureInitialLayoutRatios();
	void ApplyResponsiveLayout();
	DECLARE_MESSAGE_MAP()

protected:
	HICON m_hIcon;

	// 改为 PNG 背景
	std::unique_ptr<Gdiplus::Bitmap> m_pngBG;
	void InitBackground();

	//afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg HCURSOR OnQueryDragIcon();
public:
	afx_msg void OnBnClickedBtnRank();
	afx_msg void OnBnClickedBtnSetting();
	

private:
	CMFCButton m_btnBasic;
	CMFCButton m_btnRelax;
	CMFCButton m_btnTimed;
	CMFCButton m_btnHelp;
	CMFCButton m_btnRank;
	CMFCButton m_btnSetting;

	void SetupButton(CMFCButton& btn, UINT nID, UINT nImageID, LPCTSTR lpszText); // 设置按钮属性

	// 添加按钮布局比例集合及初始窗口大小
	CArray<BtnRectRatio, BtnRectRatio&> m_btnRatios;
	CRect m_initClientRect;
};
