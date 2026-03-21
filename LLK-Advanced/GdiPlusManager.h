#pragma once
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

// GDI+ 管理器，负责全局初始化和清理
class CGdiPlusManager
{
public:
    static void Startup();
    static void Shutdown();

private:
    static ULONG_PTR m_token;
    static bool m_inited;
};