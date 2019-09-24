// DlgChatMsg.cpp : Implement File
//

#include "stdafx.h"
#include "AgoraRTMTutorial.h"
#include "DlgChatMsg.h"
#include "afxdialogex.h"
#include "ChatDlg.h"
#include "commonFun.h"
#include "AgoraRTMInstance.h"
#include "DlgInput.h"
#include "RTMWrap.h"

#define TIMER_IDEVENT_QUERYISONLINE 1
#define TIMER_IDEVENT_QUERYISONLINE_INTERVAL  1000

// CDlgChatMsg Dialog

IMPLEMENT_DYNAMIC(CDlgChatMsg, CDialogEx)

CDlgChatMsg::CDlgChatMsg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgChatMsg::IDD, pParent),
	m_pCurChat(NULL),
	m_pSignalInstance(NULL),
	m_pSignalCallBack(NULL),
	m_pDlgInput(nullptr),
	m_curOptionType(eType_Instance)
{
	
}

CDlgChatMsg::~CDlgChatMsg()
{
	uninitCtrl();
	uninitResource();
}

void CDlgChatMsg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgChatMsg, CDialogEx)
	
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CDlgChatMsg::OnBnClickedButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CDlgChatMsg::OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_P2PMsg, &CDlgChatMsg::OnBnClickedButtonP2pmsg)
	ON_BN_CLICKED(IDC_BUTTON_CHANNELMsg, &CDlgChatMsg::OnBnClickedButtonChannelmsg)
	ON_MESSAGE(WM_UpdateInputParam, onUpdateInputParam)
	ON_MESSAGE(WM_QueryUserStatusResult, onQueryUserStatusResult)
	ON_MESSAGE(WM_MessageSendError, onMessageSendError)
	ON_MESSAGE(WM_MessageSendSuccess, onMessageSendSuccess)
	ON_MESSAGE(WM_MessageInstantReceive, onMessageInstantReceive)
	ON_MESSAGE(WM_MessageChannelReceive, onMessageChannelReceive)
	ON_MESSAGE(WM_ChannelJoined, onChannelJoined)
	ON_MESSAGE(WM_ChannelJoinedFailed, onChannelJoinFailed)
	ON_MESSAGE(WM_ChannelLeaved, onChannelLeaved)
	ON_MESSAGE(WM_ChannelUserList,onChannelUserList)
	ON_MESSAGE(WM_ChannelQueryUserNumResult, onChannelQueryUserNumResult)
END_MESSAGE_MAP()

BOOL CDlgChatMsg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	initResource();
	initCtrl();
	SetTimer(TIMER_IDEVENT_QUERYISONLINE, TIMER_IDEVENT_QUERYISONLINE_INTERVAL, NULL);

	return TRUE;
}

void CDlgChatMsg::setCallBack(CRTMCallBack* pCallBack)
{
	m_pSignalCallBack = pCallBack;
	m_pSignalCallBack->setCallBackWnd(m_hWnd);
}

// CDlgChatMsg Message Handler
void CDlgChatMsg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	CWnd* pTempWnd = GetDlgItem(IDC_BUTTON_P2PMsg);
	RECT rt;
	pTempWnd->GetWindowRect(&rt);
	ScreenToClient(&rt);
}

void CDlgChatMsg::OnClose()
{
	OnBnClickedButtonExit();
	KillTimer(TIMER_IDEVENT_QUERYISONLINE_INTERVAL);
	uninitCtrl();
	uninitResource();
	CDialogEx::OnCancel();
}

void CDlgChatMsg::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMER_IDEVENT_QUERYISONLINE == nIDEvent){

		if (m_pCurChat && eType_Instance == m_pCurChat->getCurType()){

			std::string curAccount = m_pCurChat->getCurParam();
			m_pSignalInstance->QueryIsOnline(curAccount);
		}
		else if (m_pCurChat && eType_Channel == m_pCurChat->getCurType()){

			std::string curChannel = m_pCurChat->getCurParam();
			m_pSignalInstance->ChannelQueryNumResult(curChannel);
		}
		
		if (m_pDlgInput && m_curOptionType == eType_Instance && !m_pDlgInput->IsWindowVisible()){

		}
		else if (m_pDlgInput && eType_Channel == m_curOptionType && !m_pDlgInput->IsWindowVisible()){

		}
	}
}

void CDlgChatMsg::OnBnClickedButtonSend()
{
	CString sendMsg;
	GetDlgItem(IDC_EDIT_SendMsg)->GetWindowTextW(sendMsg);
	int bEnableOfflineMessage = ((CButton*)(GetDlgItem(IDC_CHECK_EnableOffLineMsg)))->GetCheck();
	if (_T("") != sendMsg && m_pCurChat)
	{
		m_pCurChat->AddChatMessage(m_curAccount,sendMsg);
		if (m_curOptionType == eType_Instance){

			m_pSignalInstance->sendInstantMsg(m_pCurChat->getCurParam(), cs2s(sendMsg),bEnableOfflineMessage);
		}
		else if (eType_Channel == m_curOptionType){

			m_pSignalInstance->sendChannelMsg(m_pCurChat->getCurParam(), cs2s(sendMsg),bEnableOfflineMessage);
		}
	}

	GetDlgItem(IDC_EDIT_SendMsg)->SetWindowTextW(_T(""));
}


void CDlgChatMsg::OnBnClickedButtonExit()
{
	if (m_pSignalInstance){
		m_pSignalCallBack->setCallBackWnd(m_pSignalInstance->getLoginWnd());
		m_pSignalInstance->LogOut();
	}
}


void CDlgChatMsg::OnBnClickedButtonP2pmsg()
{
	m_curOptionType = eType_Instance;
	m_pDlgInput->showWindow(eType_Instance);
}

void CDlgChatMsg::OnBnClickedButtonChannelmsg()
{
	m_curOptionType = eType_Channel;
	m_pDlgInput->showWindow(eType_Channel);
}

LRESULT CDlgChatMsg::onUpdateInputParam(WPARAM wParam, LPARAM lParam)
{
	PAG_INPUTPARAM lpData = (PAG_INPUTPARAM)wParam;
	switch (lpData->type)
	{
	case eType_Channel:
		chooseChannel(lpData->strParam);
		break;
	case eType_Instance:
		chooseInstance(lpData->strParam);
		break;
	default:break;
	}

	delete lpData; lpData = NULL;

	return TRUE;
}


LRESULT CDlgChatMsg::onQueryUserStatusResult(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_QUERYUSERSTATUSRESULT lpData = (PAG_SIGNAL_QUERYUSERSTATUSRESULT)wParam;
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "onQueryUserStatusResult(%s,%u,%s,%u)", utf82gbk(lpData->name).data(), utf82gbk(lpData->name).size(), utf82gbk(lpData->status).data(), utf82gbk(lpData->status).size());
	LOG_MSG(logDesc, LogType_CallBack);

	if (m_pCurChat->getCurParam() == lpData->name && eType_Instance == m_pCurChat->getCurType()){

		if ("1" == lpData->status){

			GetDlgItem(IDC_STATIC_CURCHATID)->SetWindowTextW(s2cs(lpData->name + std::string(" (Online)")));
		}
		else{

			GetDlgItem(IDC_STATIC_CURCHATID)->SetWindowTextW(s2cs(lpData->name + std::string(" (Offline)")));
		}
	}

	delete lpData; lpData = nullptr;
	return TRUE;
}


LRESULT CDlgChatMsg::onMessageSendSuccess(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_MESSAGESENDSUCCESS lpData = (PAG_SIGNAL_MESSAGESENDSUCCESS)wParam;
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "onMessageSendSuccess(%s,%u)", lpData->messageID.data(), lpData->messageID.size());
	LOG_MSG(logDesc, LogType_CallBack);

	delete lpData; lpData = nullptr;

	return TRUE;
}

LRESULT CDlgChatMsg::onMessageSendError(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_MESSAGESENDERROR lpData = (PAG_SIGNAL_MESSAGESENDERROR)wParam;
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "%s,%u,%d", lpData->message.data(), lpData->message.size(), lpData->ecode);
	LOG_MSG(logDesc, LogType_CallBack);

	delete lpData; lpData = nullptr;

	return TRUE;
}

LRESULT CDlgChatMsg::onMessageInstantReceive(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_MESSAGEINSTANCERECEIVE lpData = (PAG_SIGNAL_MESSAGEINSTANCERECEIVE)wParam;
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "onMessageInstantReceive(%s,%u,%u,%s,%u)", lpData->account.data(), lpData->account.size(), lpData->uid, lpData->msg.data(), lpData->msg.size());
	LOG_MSG(logDesc, LogType_CallBack);

	std::map<std::string, CChatDlg*>::iterator it = m_mapChatP2P.find(lpData->account);
	if (m_mapChatP2P.end() != it){

		it->second->AddChatMessage(lpData->account, s2cs(lpData->msg));
	}
	else{

		CWnd* pWnd = GetDlgItem(IDC_STATIC_MsgListBK);
		CChatDlg *pTemp = new CChatDlg(eType_Instance, lpData->account, pWnd);
		pTemp->Create(CChatDlg::IDD, pWnd);
		pTemp->AddChatMessage(lpData->account, s2cs(lpData->msg));
		m_mapChatP2P[lpData->account] = pTemp;
	}

	delete lpData; lpData = nullptr;

	return TRUE;
}

LRESULT CDlgChatMsg::onMessageChannelReceive(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_MESSAGECHANNELRECEIVE lpData = (PAG_SIGNAL_MESSAGECHANNELRECEIVE)wParam;
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "onMessageChannelReceive(%s,%u,%s,%u,%u,%s,%u)", lpData->channelID.data(), lpData->channelID.size(),lpData->account.data(),lpData->account.size(),lpData->uid,lpData->msg.data(),lpData->msg.size());
	LOG_MSG(logDesc, LogType_CallBack);

	std::map<std::string, CChatDlg*>::iterator it = m_mapChatChannel.find(lpData->channelID);
	if (m_mapChatChannel.end() != it){
		if (m_curAccount != lpData->account)
		it->second->AddChatMessage(lpData->account, s2cs(lpData->msg));
	}
	else {

		CWnd* pWnd = GetDlgItem(IDC_STATIC_MsgListBK);
		CChatDlg* pTemp = new CChatDlg(eType_Channel, lpData->channelID, pWnd);
		pTemp->Create(CChatDlg::IDD, pWnd);
		m_mapChatChannel[lpData->channelID] = pTemp;
		if (m_curAccount == lpData->account)
		pTemp->AddChatMessage(lpData->account, s2cs(lpData->msg));
	}

	delete lpData; lpData = nullptr;
	return TRUE;
}

LRESULT CDlgChatMsg::onChannelJoined(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_CHANNELJOIN lpData = (PAG_SIGNAL_CHANNELJOIN)wParam;
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "onChannelJoined(%s,%u)", lpData->channelID.data(),lpData->channelID.size());
	LOG_MSG(logDesc, LogType_CallBack);

	showChannelInfo(lpData->channelID);

	delete lpData; lpData = nullptr;
	return TRUE;
}

LRESULT CDlgChatMsg::onChannelJoinFailed(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_CHANNELJOINFAILED lpData = (PAG_SIGNAL_CHANNELJOINFAILED)wParam;
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "onChannelJoinFailed(%s,%u,%d)", lpData->channelID.data(), lpData->channelID.size(),lpData->ecode);
	LOG_MSG(logDesc, LogType_CallBack);
	CString strChannel = s2cs(lpData->channelID);
	AfxMessageBox(_T("join channel failed.."));
	strChannel.ReleaseBuffer();

	delete lpData; lpData = nullptr;
	return TRUE;
}

LRESULT CDlgChatMsg::onChannelLeaved(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_CHANNELLEAVE lpData = (PAG_SIGNAL_CHANNELLEAVE)wParam;
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "onChannelLeaved(%s,%u,%d)", lpData->channelID.data(), lpData->channelID.size(), lpData->ecode);
	LOG_MSG(logDesc, LogType_CallBack);

	delete lpData; lpData = nullptr;
	return TRUE;
}

LRESULT CDlgChatMsg::onChannelUserList(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_CHANNELUSERLIST lpData = (PAG_SIGNAL_CHANNELUSERLIST)wParam;
	delete lpData; lpData = nullptr;

	return TRUE;
}

LRESULT CDlgChatMsg::onChannelQueryUserNumResult(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_CHANNELQUERYUSERNUMRESULT lpData = (PAG_SIGNAL_CHANNELQUERYUSERNUMRESULT)wParam;
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "onChannelQueryUserNumResult(%s,%u,%d,%d)", lpData->channelID.data(), lpData->channelID.size(), lpData->ecode,lpData->num);
	LOG_MSG(logDesc, LogType_CallBack);

	std::map<std::string, CChatDlg*>::iterator it = m_mapChatChannel.find(lpData->channelID);
	if (m_mapChatChannel.end() != it){
		CString strParam;
		strParam.Format(_T("%s (numbers of channel: %d)"), s2cs(lpData->channelID), lpData->num);
		GetDlgItem(IDC_STATIC_CURCHATID)->SetWindowTextW(strParam);
	}

	delete lpData; lpData = nullptr;
	return TRUE;
}

void CDlgChatMsg::initCtrl()
{
	GetDlgItem(IDC_BUTTON_P2PMsg)->SetFocus();
	m_pDlgInput = new CDlgInput;
	m_pDlgInput->Create(CDlgInput::IDD, this);
	m_pDlgInput->ShowWindow(SW_HIDE);

	m_pSignalInstance->setChatMsgWnd(m_hWnd);
	m_curAccount = m_pSignalInstance->getLoginAccount();
	GetDlgItem(IDC_STATIC_CUR_USERID)->SetWindowTextW(s2cs(m_curAccount));
}


void CDlgChatMsg::uninitCtrl()
{
	for (std::map<std::string, CChatDlg*>::iterator it = m_mapChatP2P.begin(); m_mapChatP2P.end() != it; it++){
		delete it->second;
		it->second = nullptr;
	}
	m_mapChatP2P.clear();

	for (std::map<std::string, CChatDlg*>::iterator it = m_mapChatChannel.begin(); m_mapChatChannel.end() != it; it++){
		delete it->second;
		it->second = NULL;
	}
	m_mapChatChannel.clear();

	if (m_pDlgInput){
		delete m_pDlgInput;
		m_pDlgInput = nullptr;
	}
}

void CDlgChatMsg::initResource()
{
	m_pSignalInstance = CAgoraRTMInstance::getSignalInstance();
}

void CDlgChatMsg::uninitResource()
{
	m_pSignalInstance = NULL;
}

void CDlgChatMsg::chooseInstance(const std::string &userAccount)
{
	GetDlgItem(IDC_STATIC_CURCHATID)->SetWindowTextW(s2cs(userAccount));

	std::map<std::string, CChatDlg*>::iterator it = m_mapChatP2P.find(userAccount);
	if (m_mapChatP2P.end() != it)
	{
		if (it->second != m_pCurChat){
			if (m_pCurChat){
				m_pCurChat->ShowWindow(SW_HIDE);
			}
			m_pCurChat = it->second;
		}
		it->second->CenterWindow();
		it->second->ShowWindow(SW_SHOW);
		it->second->Invalidate(TRUE);
		m_pCurChat = it->second;
	}
	else
	{
		if (m_pCurChat)
		{
			m_pCurChat->ShowWindow(SW_HIDE);
		}
		CWnd* pWnd = GetDlgItem(IDC_STATIC_MsgListBK);
		CChatDlg *pTemp = new CChatDlg(eType_Instance,userAccount,pWnd);
		m_pCurChat = pTemp;
		m_mapChatP2P[userAccount] = m_pCurChat;
		m_pCurChat->Create(CChatDlg::IDD, pWnd);
		m_pCurChat->ShowWindow(SW_SHOW);
		m_pCurChat->Invalidate(TRUE);
	}
}

void CDlgChatMsg::chooseChannel(const std::string &channelName)
{	
	if (m_pSignalInstance){

		m_pSignalInstance->JoinChannel(channelName);
	}
}

void CDlgChatMsg::showChannelInfo(const std::string &channelName)
{
	GetDlgItem(IDC_STATIC_CURCHATID)->SetWindowTextW(s2cs("channelName:" + channelName));

	std::map<std::string, CChatDlg*>::iterator it = m_mapChatChannel.find(channelName);
	if (m_mapChatChannel.end() != it)
	{
		if (it->second != m_pCurChat){
			if (m_pCurChat){
				m_pCurChat->ShowWindow(SW_HIDE);
			}
			m_pCurChat = it->second;
		}
		it->second->CenterWindow();
		it->second->ShowWindow(SW_SHOW);
	}
	else
	{
		if (m_pCurChat)
		{
			m_pCurChat->ShowWindow(SW_HIDE);
		}
		CWnd* pWnd = GetDlgItem(IDC_STATIC_MsgListBK);
		CChatDlg *pTemp = new CChatDlg(eType_Channel, channelName, pWnd);
		m_pCurChat = pTemp;
		m_mapChatChannel[channelName] = m_pCurChat;
		m_pCurChat->Create(CChatDlg::IDD, pWnd);
		m_pCurChat->ShowWindow(SW_SHOW);
	}
}

BOOL CDlgChatMsg::PreTranslateMessage(MSG* pMsg)
{
	if (VK_RETURN == pMsg->wParam && WM_KEYDOWN == pMsg->message){

		if (GetFocus() == GetDlgItem(IDC_EDIT_SendMsg)){
			OnBnClickedButtonSend();
		}
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}