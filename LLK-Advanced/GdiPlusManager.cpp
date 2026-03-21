#include "pch.h"
#include "GdiPlusManager.h"

ULONG_PTR CGdiPlusManager::m_token = 0;
bool CGdiPlusManager::m_inited = false;

// 全局初始化，建议在应用程序入口处调用一次
void CGdiPlusManager::Startup()
{
    if (!m_inited)
    {
        Gdiplus::GdiplusStartupInput input;
        Gdiplus::GdiplusStartup(&m_token, &input, nullptr);
        m_inited = true;
    }
}

// 全局清理，建议在应用程序退出前调用一次
void CGdiPlusManager::Shutdown()
{
    if (m_inited)
    {
        Gdiplus::GdiplusShutdown(m_token);
        m_token = 0;
        m_inited = false;
    }
}