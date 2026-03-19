#pragma once
#include "resource.h"
#include "global.h"

#define WND_WIDTH  800
#define WND_HEIGHT 600

// CLLKDlg 对话框
class CLLKDlg : public CDialogEx
{
public:
	CLLKDlg(CWnd* pParent = nullptr);	// 标准构造函数

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LLK_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual BOOL OnInitDialog(); // 对话框初始化
	afx_msg void OnPaint(); // 窗口绘制消息
	afx_msg void OnBnClickedBtnBasic(); // 基本模式按钮点击
	afx_msg void OnBnClickedBtnRelax();
	afx_msg void OnBnClickedBtnHelp();
	afx_msg void OnBnClickedBtnTimed(); // 定时模式按钮点击
	afx_msg void OnClose(); // 窗口关闭消息
	DECLARE_MESSAGE_MAP()

// 实现
protected:
	HICON m_hIcon;
	CDC m_dcMem;        // 内存DC，缓存背景位图
	CBitmap m_bmpBG;    // 背景位图对象
	void InitBackground(); // 初始化背景
	// 生成的消息映射函数
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg HCURSOR OnQueryDragIcon();
};
