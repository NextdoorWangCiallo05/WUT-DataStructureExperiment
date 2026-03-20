#include "pch.h"
#include "GdiPlusManager.h"

ULONG_PTR CGdiPlusManager::m_token = 0;
bool CGdiPlusManager::m_inited = false;

void CGdiPlusManager::Startup()
{
    if (!m_inited)
    {
        Gdiplus::GdiplusStartupInput input;
        Gdiplus::GdiplusStartup(&m_token, &input, nullptr);
        m_inited = true;
    }
}

void CGdiPlusManager::Shutdown()
{
    if (m_inited)
    {
        Gdiplus::GdiplusShutdown(m_token);
        m_token = 0;
        m_inited = false;
    }
}