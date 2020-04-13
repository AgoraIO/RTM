
// AgoraRTMTutorial.cpp :

#include "stdafx.h"
#include "AgoraRTMTutorial.h"
#include "AgoraRTMlDlg.h"
#include "commonFun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAgoraRTMTutorialApp
Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;
ULONG_PTR m_gdiplusToken;

BEGIN_MESSAGE_MAP(CAgoraRTMTutorialApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAgoraRTMTutorialApp

CAgoraRTMTutorialApp::CAgoraRTMTutorialApp()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}


// CAgoraRTMTutorialApp Object

CAgoraRTMTutorialApp theApp;
CConfigSignal gConfigSignal;
CFileIO gLogSignal;

// CAgoraRTMTutorialApp Initialize

BOOL CAgoraRTMTutorialApp::InitInstance()
{
    TCHAR path[MAXPATHLEN] = { 0 };
    GetModuleFileName(nullptr, path, MAXPATHLEN);
    CString filePath = path;
    filePath = filePath.Mid(0, filePath.ReverseFind(_T('\\')) + 1);
    filePath += _T("agoraImageMessage");
    if (!PathFileExists(filePath)) {
        CreateDirectory(filePath, NULL);
    }
   
    recvfilePath = cs2Utf8(filePath);
    GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	CShellManager *pShellManager = new CShellManager;
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	initlog();
  CAgoraRTMDlg dlg;
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
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	uninitLog();
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}
    GdiplusShutdown(m_gdiplusToken);
	return FALSE;
}

