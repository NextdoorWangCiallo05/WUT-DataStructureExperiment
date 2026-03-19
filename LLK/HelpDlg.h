#pragma once
#include "resource.h"
#include "global.h"

// 帮助对话框尺寸（与资源定义一致）
#define HELP_WND_WIDTH  800
#define HELP_WND_HEIGHT 500

class CHelpDlg : public CDialogEx
{
public:
    // 构造函数，指定对话框ID
    CHelpDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_HELP_DIALOG };
#endif

protected:
    // DDX/DDV 数据交换
    virtual void DoDataExchange(CDataExchange* pDX);
    // 对话框初始化
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()
};