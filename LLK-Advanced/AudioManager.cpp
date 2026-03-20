#include "pch.h"
#include "AudioManager.h"
#include "resource.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

static const TCHAR* kProfileSection = _T("Audio");
static const TCHAR* kKeyVolume = _T("Volume");
static const TCHAR* kKeyMuted = _T("Muted");
static const TCHAR* kKeyMusicEnabled = _T("MusicEnabled");

CAudioManager& CAudioManager::Instance()
{
    static CAudioManager s_mgr;
    return s_mgr;
}

CAudioManager::CAudioManager()
    : m_inited(false)
    , m_playing(false)
    , m_muted(false)
    , m_musicEnabled(true)
    , m_loop(true)
    , m_volume(60)
    , m_alias(_T("bgm_alias"))
{
}

CAudioManager::~CAudioManager()
{
    UnInit();
}

void CAudioManager::Init()
{
    if (m_inited) return;
    LoadSettings();
    m_inited = true;
}

void CAudioManager::UnInit()
{
    if (!m_inited) return;

    SaveSettings();
    StopBGM();

    // 删除资源导出的临时文件
    if (!m_tempResPath.IsEmpty())
    {
        ::DeleteFile(m_tempResPath);
        m_tempResPath.Empty();
    }

    m_inited = false;
}

bool CAudioManager::PlayBGM(const CString& filePath, bool loop)
{
    Init();

    m_loop = loop;
    m_bgmPath = filePath;

    // 音乐总开关关闭时，不实际播放，但返回true避免打断流程
    if (!m_musicEnabled)
        return true;

    // 文件存在性检查
    if (::GetFileAttributes(filePath) == INVALID_FILE_ATTRIBUTES)
    {
        CString msg;
        msg.Format(_T("背景音乐文件不存在：\n%s"), filePath.GetString());
        AfxMessageBox(msg);
        return false;
    }

    StopBGM();

    CString cmd;
    MCIERROR err = 0;
    TCHAR errText[256] = { 0 };

    // 不强制 type，兼容更好
    cmd.Format(_T("open \"%s\" alias %s"), filePath.GetString(), m_alias.GetString());
    err = mciSendString(cmd, nullptr, 0, nullptr);
    if (err != 0)
    {
        mciGetErrorString(err, errText, 256);
        CString msg;
        msg.Format(_T("打开背景音乐失败\n错误码: %u\n错误: %s"), err, errText);
        AfxMessageBox(msg);
        return false;
    }

    ApplyVolumeToBGM();

    cmd.Format(loop ? _T("play %s repeat") : _T("play %s"), m_alias.GetString());
    err = mciSendString(cmd, nullptr, 0, nullptr);
    if (err != 0)
    {
        mciGetErrorString(err, errText, 256);
        CString msg;
        msg.Format(_T("播放背景音乐失败\n错误码: %u\n错误: %s"), err, errText);
        AfxMessageBox(msg);

        cmd.Format(_T("close %s"), m_alias.GetString());
        mciSendString(cmd, nullptr, 0, nullptr);
        return false;
    }

    m_playing = true;
    return true;
}

bool CAudioManager::PlayBGMFromResource(bool loop)
{
    Init();

    // 资源ID/类型：请确保 resource.h 和 .rc 中一致
    CString tempPath = ExportResToTempFile(IDR_BGM_MP3, _T("RCDATA"), _T("bgm.mp3"));
    if (tempPath.IsEmpty())
    {
        AfxMessageBox(_T("导出背景音乐资源失败。请检查 IDR_BGM_MP3 是否存在。"));
        return false;
    }

    m_tempResPath = tempPath;
    return PlayBGM(tempPath, loop);
}

void CAudioManager::StopBGM()
{
    if (!m_playing) return;

    CString cmd;
    cmd.Format(_T("stop %s"), m_alias.GetString());
    mciSendString(cmd, nullptr, 0, nullptr);

    cmd.Format(_T("close %s"), m_alias.GetString());
    mciSendString(cmd, nullptr, 0, nullptr);

    m_playing = false;
}

void CAudioManager::SetVolume(int volume)
{
    if (volume < 0) volume = 0;
    if (volume > 100) volume = 100;

    m_volume = volume;
    ApplyVolumeToBGM();
    SaveSettings();
}

void CAudioManager::SetMuted(bool muted)
{
    m_muted = muted;
    ApplyVolumeToBGM();
    SaveSettings();
}

void CAudioManager::SetMusicEnabled(bool enabled)
{
    if (m_musicEnabled == enabled) return;

    m_musicEnabled = enabled;

    if (!m_musicEnabled)
    {
        StopBGM();
    }
    else
    {
        // 恢复播放优先顺序：外部路径 > 资源音乐
        if (!m_bgmPath.IsEmpty())
        {
            PlayBGM(m_bgmPath, m_loop);
        }
        else
        {
            PlayBGMFromResource(m_loop);
        }
    }

    SaveSettings();
}

int CAudioManager::EffectiveVolume() const
{
    return m_muted ? 0 : m_volume;
}

void CAudioManager::ApplyVolumeToBGM()
{
    // MCI volume 0~1000
    int mciVol = EffectiveVolume() * 10;

    CString cmd;
    cmd.Format(_T("setaudio %s volume to %d"), m_alias.GetString(), mciVol);
    mciSendString(cmd, nullptr, 0, nullptr);
}

CString CAudioManager::ExportResToTempFile(UINT resId, LPCTSTR /*resType*/, LPCTSTR fileName)
{
    // 关键：用资源句柄，不用实例句柄
    HINSTANCE hResInst = AfxGetResourceHandle();
    if (!hResInst)
    {
        AfxMessageBox(_T("AfxGetResourceHandle 为空"));
        return _T("");
    }

    HRSRC hRes = ::FindResource(hResInst, MAKEINTRESOURCE(resId), RT_RCDATA);
    if (!hRes)
    {
        DWORD err = ::GetLastError();
        CString msg;
        msg.Format(_T("FindResource 失败，resId=%u，err=%lu"), resId, err);
        AfxMessageBox(msg);
        return _T("");
    }

    DWORD size = ::SizeofResource(hResInst, hRes);
    HGLOBAL hData = ::LoadResource(hResInst, hRes);
    if (!hData || size == 0) return _T("");

    void* pData = ::LockResource(hData);
    if (!pData) return _T("");

    TCHAR tempDir[MAX_PATH] = { 0 };
    if (::GetTempPath(MAX_PATH, tempDir) == 0) return _T("");

    CString outPath = CString(tempDir) + fileName;

    HANDLE hFile = ::CreateFile(outPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return _T("");

    DWORD written = 0;
    BOOL ok = ::WriteFile(hFile, pData, size, &written, nullptr);
    ::CloseHandle(hFile);

    if (!ok || written != size)
    {
        ::DeleteFile(outPath);
        return _T("");
    }
    return outPath;
}

void CAudioManager::LoadSettings()
{
    CWinApp* app = AfxGetApp();
    if (!app) return;

    m_volume = app->GetProfileInt(kProfileSection, kKeyVolume, 60);
    if (m_volume < 0) m_volume = 0;
    if (m_volume > 100) m_volume = 100;

    m_muted = app->GetProfileInt(kProfileSection, kKeyMuted, 0) != 0;
    m_musicEnabled = app->GetProfileInt(kProfileSection, kKeyMusicEnabled, 1) != 0;
}

void CAudioManager::SaveSettings()
{
    CWinApp* app = AfxGetApp();
    if (!app) return;

    app->WriteProfileInt(kProfileSection, kKeyVolume, m_volume);
    app->WriteProfileInt(kProfileSection, kKeyMuted, m_muted ? 1 : 0);
    app->WriteProfileInt(kProfileSection, kKeyMusicEnabled, m_musicEnabled ? 1 : 0);
}