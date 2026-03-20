#pragma once
#include "resource.h"
#include "global.h"

// 帮助对话框尺寸（与资源定义一致）
#define HELP_WND_WIDTH  800
#define HELP_WND_HEIGHT 500

class CHelpDlg : public CDialogEx
{
public:
    CHelpDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_HELP_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

private:
    CFont m_fontUi;                 // 统一字体
    void ApplyDialogFont(CWnd* pRoot); // 递归应用到所有子控件
};