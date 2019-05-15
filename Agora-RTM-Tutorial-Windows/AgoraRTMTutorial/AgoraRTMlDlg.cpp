
// AgoraRTMTutorialDlg.cpp : Implement File
//

#include "stdafx.h"
#include "AgoraRTMTutorial.h"
#include "AgoraRTMlDlg.h"
#include "afxdialogex.h"
#include "DlgChatMsg.h"
#include "commonFun.h"
#include "AgoraRTMInstance.h"
#include "RTMWrap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDlgChatMsg* m_pDlgChatMsg;

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    //DDX/DDV Support

// Implement
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAgoraRTMDlg Dialog



CAgoraRTMDlg::CAgoraRTMDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAgoraRTMDlg::IDD, pParent),
	m_isLogin(FALSE),
	m_pRTMInstance(NULL),
	m_pRTMCallBack(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAgoraRTMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAgoraRTMDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LoginPt, &CAgoraRTMDlg::OnBnClickedButtonLogin)
	ON_MESSAGE(WM_LoginSuccess,onLoginSuccess)
	ON_MESSAGE(WM_LogOut,onLogout)
	ON_MESSAGE(WM_LoginFailed,onLogFailed)
	ON_MESSAGE(WM_Error,onError)
	ON_MESSAGE(WM_Log,onLog)
END_MESSAGE_MAP()


// CAgoraRTMDlg Message Handler

BOOL CAgoraRTMDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);

	initCtrl();
	initResource();

	return TRUE; 
}

void CAgoraRTMDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CAgoraRTMDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

void CAgoraRTMDlg::OnClose()
{
	uninitResource();
	uninitCtrl();

	CDialogEx::OnCancel();
}


HCURSOR CAgoraRTMDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CAgoraRTMDlg::OnBnClickedButtonLogin()
{
	CString strAccount;
	GetDlgItem(IDC_EDIT_Login_Account)->GetWindowTextW(strAccount);
	if (_T("") == strAccount){
		AfxMessageBox(_T("Account can not be empty,please re_enter.."));
		return;
	}
	else if (isContailSpace(strAccount)){
		AfxMessageBox(_T("Account contain blank characters.please re_enter.."));
		return;
	}

	m_account = cs2s(strAccount);
	std::string appId = m_pRTMInstance->getAppId();
	std::string appCertificateId = m_pRTMInstance->getCertificateId();

	time_t ltime;
	time(&ltime);
	int expiredSecond = ltime + 3600;
	m_pRTMInstance->Login(m_account, appId);
}

void CAgoraRTMDlg::initCtrl()
{

}

void CAgoraRTMDlg::uninitCtrl(){}

void CAgoraRTMDlg::initResource()
{
	std::string AppId = gConfigSignal.getAppID();
	if ("" == AppId)
	{
		gConfigSignal.setAppID("");
		std::string ConfigPath = gConfigSignal.getFilePath();
		ShellExecute(NULL, _T("open"), s2cs(ConfigPath), NULL, NULL, SW_SHOW);
		AfxMessageBox(_T("APPID can not be empty ,Please modify the AppID and AppCertificatedId in the AgoraSignal.ini configuration item"));
		PostQuitMessage(0);
		return;
	}

  m_pRTMCallBack = new CRTMCallBack(m_hWnd);
  m_pRTMInstance = CAgoraRTMInstance::getSignalInstance(AppId, m_pRTMCallBack);
	std::string appCertificateID = gConfigSignal.getAppCertificatedId();
	m_pRTMInstance->setAppCertificateId(appCertificateID);
	
	m_pRTMInstance->setLoginWnd(m_hWnd);
	m_pRTMInstance->getAgoraAPI()->addEventHandler(m_pRTMCallBack);

	std::string sdkVersion = m_pRTMInstance->getSDKVersion();
	GetDlgItem(IDC_STATIC_SDKVERSION)->SetWindowTextW(s2cs(sdkVersion));
}

void CAgoraRTMDlg::uninitResource()
{

	if (m_pRTMInstance){
		m_pRTMInstance->ReleaseInstance();
		m_pRTMInstance = nullptr;
	}
}

LRESULT CAgoraRTMDlg::onLoginSuccess(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_LOGINSUCCESS lpData = (PAG_SIGNAL_LOGINSUCCESS)wParam;
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "onLoginSuccess(%u,%u)", lpData->uid, lpData->fd);
	LOG_MSG(logDesc, LogType_CallBack);

	delete lpData; lpData = nullptr;

	m_pRTMInstance->setptStatus(true);

	if (NULL == m_pDlgChatMsg)
	{
		m_pDlgChatMsg = new CDlgChatMsg;
		m_pDlgChatMsg->Create(CDlgChatMsg::IDD);
		m_pDlgChatMsg->setCallBack(m_pRTMCallBack);
		m_pDlgChatMsg->CenterWindow();
		m_pDlgChatMsg->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pDlgChatMsg->CenterWindow();
		m_pDlgChatMsg->ShowWindow(SW_SHOW);
	}

	return true;
}

LRESULT CAgoraRTMDlg::onLogout(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_LOGOUT lpData = (PAG_SIGNAL_LOGOUT)wParam;
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "onLogout(%d)", lpData->ecode);
	LOG_MSG(logDesc, LogType_CallBack);
	
	delete lpData; lpData = NULL;
	if (m_pDlgChatMsg){
		delete m_pDlgChatMsg;
		m_pDlgChatMsg = nullptr;
	}

	return TRUE;
}


LRESULT CAgoraRTMDlg::onLogFailed(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_LOGINFAILED lpData = (PAG_SIGNAL_LOGINFAILED)wParam;
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "onLogFailed(%d)", lpData->ecode);
	LOG_MSG(logDesc, LogType_CallBack);

	delete lpData; lpData = nullptr;
	return TRUE;
}


LRESULT CAgoraRTMDlg::onError(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_ERROR lpData = (PAG_SIGNAL_ERROR)wParam;
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "onError(%s,%u,%d,%s,%u)", lpData->name.data(), lpData->name.size(),lpData->ecode,lpData->desc.data(),lpData->desc.size());
	LOG_MSG(logDesc, LogType_SDK_Error);
	
	delete lpData; lpData = nullptr;
	return TRUE;
}

LRESULT CAgoraRTMDlg::onLog(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_LOG lpData = (PAG_SIGNAL_LOG)wParam;
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "onLog(%s,%u)", lpData->txt.data(),lpData->txt.size());
	LOG_MSG(logDesc, LogType_SDK_LOG);

	delete lpData; lpData = nullptr;
	return TRUE;
}

BOOL CAgoraRTMDlg::PreTranslateMessage(MSG* pMsg)
{
	if (VK_RETURN == pMsg->wParam && WM_KEYDOWN == pMsg->message){
		
		if (GetFocus() == GetDlgItem(IDC_EDIT_Login_Account)){
			OnBnClickedButtonLogin();
		}
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}