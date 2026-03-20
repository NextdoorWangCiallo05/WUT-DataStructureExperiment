#include "pch.h"
#include "SettingDlg.h"
#include "AudioManager.h"
#include "afxdialogex.h"

BEGIN_MESSAGE_MAP(CSettingDlg, CDialogEx)
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_CHECK_MUTE, &CSettingDlg::OnBnClickedCheckMute)
END_MESSAGE_MAP()

CSettingDlg::CSettingDlg(CWnd* pParent)
    : CDialogEx(IDD_SETTING_DIALOG, pParent)
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SLIDER_VOLUME, m_sliderVolume);
    DDX_Control(pDX, IDC_CHECK_MUTE, m_chkMute);
}

BOOL CSettingDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_sliderVolume.SetRange(0, 100);
    m_sliderVolume.SetTicFreq(10);

    auto& audio = CAudioManager::Instance();
    int vol = audio.GetVolume();
    m_sliderVolume.SetPos(vol);
    m_chkMute.SetCheck(audio.IsMuted() ? BST_CHECKED : BST_UNCHECKED);
    UpdateVolumeText(vol);

    return TRUE;
}

void CSettingDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);

    if (pScrollBar && pScrollBar->GetSafeHwnd() == m_sliderVolume.GetSafeHwnd())
    {
        int vol = m_sliderVolume.GetPos();
        CAudioManager::Instance().SetVolume(vol);
        UpdateVolumeText(vol);
    }
}

void CSettingDlg::OnBnClickedCheckMute()
{
    bool muted = (m_chkMute.GetCheck() == BST_CHECKED);
    CAudioManager::Instance().SetMuted(muted);
}

void CSettingDlg::UpdateVolumeText(int vol)
{
    CString s;
    s.Format(_T("音量：%d"), vol);
    SetDlgItemText(IDC_STATIC_VOLUME, s);
}