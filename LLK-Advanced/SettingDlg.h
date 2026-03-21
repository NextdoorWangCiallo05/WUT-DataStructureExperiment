#pragma once
#include "resource.h"
#include <afxcmn.h>
#include <afxbutton.h>

// 设置对话框尺寸（与资源定义一致）
class CSettingDlg : public CDialogEx
{
public:
    CSettingDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SETTING_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnBnClickedCheckMute();
    DECLARE_MESSAGE_MAP()

private:
    CSliderCtrl m_sliderVolume;
    CButton     m_chkMute;

    void UpdateVolumeText(int vol);
};