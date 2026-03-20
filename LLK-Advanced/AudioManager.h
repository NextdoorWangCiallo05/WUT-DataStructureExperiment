#pragma once
#include <afxwin.h>

class CAudioManager
{
public:
    static CAudioManager& Instance();

    void Init();
    void UnInit();

    // 直接播放文件
    bool PlayBGM(const CString& filePath, bool loop = true);
    // 从资源播放（IDR_BGM_MP3, RCDATA）
    bool PlayBGMFromResource(bool loop = true);

    void StopBGM();

    // 音量 0~100
    void SetVolume(int volume);
    int  GetVolume() const { return m_volume; }

    // 静音
    void SetMuted(bool muted);
    bool IsMuted() const { return m_muted; }

    // 音乐总开关（关=停止，开=恢复播放）
    void SetMusicEnabled(bool enabled);
    bool IsMusicEnabled() const { return m_musicEnabled; }

    // 可选：指定默认音乐文件路径（用于开关恢复）
    void SetBgmPath(const CString& filePath) { m_bgmPath = filePath; }

    // 持久化
    void LoadSettings();
    void SaveSettings();

private:
    CAudioManager();
    ~CAudioManager();
    CAudioManager(const CAudioManager&) = delete;
    CAudioManager& operator=(const CAudioManager&) = delete;

    int  EffectiveVolume() const;
    void ApplyVolumeToBGM();
    CString ExportResToTempFile(UINT resId, LPCTSTR resType, LPCTSTR fileName);

private:
    bool    m_inited;
    bool    m_playing;
    bool    m_muted;
    bool    m_musicEnabled;
    bool    m_loop;
    int     m_volume;      // 0~100

    CString m_alias;       // MCI alias
    CString m_bgmPath;     // 当前/默认bgm��径
    CString m_tempResPath; // 资源导出临时文件路径
};