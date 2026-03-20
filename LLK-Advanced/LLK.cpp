// LLK.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "LLK.h"
#include "LLKDlg.h"
#include "GdiPlusManager.h"
#include "AudioManager.h"
#include <ShellScalingApi.h>
#pragma comment(lib, "Shcore.lib")

int GAME_ROWS = GAME_ROWS_CLASSIC;
int GAME_COLS = GAME_COLS_CLASSIC;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CLLKApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CLLKApp::CLLKApp()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

CLLKApp theApp;

BOOL CLLKApp::InitInstance()
{
	// 启动 GDI+
	CGdiPlusManager::Startup();

	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	CShellManager* pShellManager = new CShellManager;

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	// 启动BGM（建议放在exe同目录，比如 .\res\bgm.mp3）
	TCHAR exePath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, exePath, MAX_PATH);
	CString path(exePath);
	int pos = path.ReverseFind(_T('\\'));
	if (pos != -1) path = path.Left(pos); // exe目录

	// 回退到项目根（x64\Debug -> 项目根）
	for (int i = 0; i < 2; ++i)
	{
		int p = path.ReverseFind(_T('\\'));
		if (p != -1) path = path.Left(p);
	}

	CString bgmFile = path + _T("\\res\\bgm.mp3");

	CAudioManager::Instance().Init();
	CAudioManager::Instance().PlayBGMFromResource();

	CLLKDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	

	// 退出前释放音频
	CAudioManager::Instance().UnInit();


	// 关闭 GDI+
	CGdiPlusManager::Shutdown();

	return FALSE;
}