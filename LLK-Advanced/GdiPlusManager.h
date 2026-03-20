#pragma once
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

class CGdiPlusManager
{
public:
    static void Startup();
    static void Shutdown();

private:
    static ULONG_PTR m_token;
    static bool m_inited;
};