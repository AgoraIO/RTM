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
#include <io.h>
#include "DlgImageSettings.h"
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
    DDX_Control(pDX, IDC_LISTIMAGE, m_lstImage);
    //DDX_Control(pDX, IDC_EDIT_UID, m_edtUID);

    DDX_Control(pDX, IDC_STATIC_SHOW, m_staImage);
    DDX_Control(pDX, IDC_STATIC_SHOW_SEND, m_sta);
    DDX_Control(pDX, IDC_STATIC_SENDIMAGE, m_staSendImageInfo);
    DDX_Control(pDX, IDC_STATIC_IMAGE_INFO, m_staRecvImageinfo);
    DDX_Control(pDX, IDC_BUTTON1, m_btnSendImage);
    DDX_Control(pDX, IDC_BUTTON_CANCEL, m_btnCancel);
    DDX_Control(pDX, IDC_BUTTON_MEDIA_ID, m_sendImageByMediaId);
    DDX_Control(pDX, IDC_BUTTON_CANCEL2, m_btnCancelDownload);
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
    ON_MESSAGE(WM_ImageMessageUploadResult, onImageMediaUploadResult)
    ON_MESSAGE(WM_ImageMessageDownloadResult, onImageMediaDownloadResult)

    
    ON_MESSAGE(WM_ImageMessageRecvFromPeer, onRecvImageMsgfromPeer)
    ON_MESSAGE(WM_ImageMessageRecvChannel, onRecvImageMsgfromChannel)

    
    ON_MESSAGE(WM_MediaUploadingProgress, onMediaUploadProgress)
    ON_MESSAGE(WM_MediaDownloadingProgress, onMediaDownloadloadProgress)

	ON_MESSAGE(WM_MessageInstantReceive, onMessageInstantReceive)
	ON_MESSAGE(WM_MessageChannelReceive, onMessageChannelReceive)
	ON_MESSAGE(WM_ChannelJoined, onChannelJoined)
	ON_MESSAGE(WM_ChannelJoinedFailed, onChannelJoinFailed)
	ON_MESSAGE(WM_ChannelLeaved, onChannelLeaved)
	ON_MESSAGE(WM_ChannelUserList,onChannelUserList)
	ON_MESSAGE(WM_ChannelQueryUserNumResult, onChannelQueryUserNumResult)
    ON_BN_CLICKED(IDC_BUTTON1, &CDlgChatMsg::OnBnClickedSendImage)
    ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CDlgChatMsg::OnBnClickedButtonCancel)
    ON_STN_CLICKED(IDC_STATIC_SHOW, &CDlgChatMsg::OnStnClickedStaticShow)
    ON_BN_CLICKED(IDC_BUTTON_CANCEL2, &CDlgChatMsg::OnBnClickedButtonCancel2)
    ON_BN_CLICKED(IDC_BUTTON_IMG_SET, &CDlgChatMsg::OnBnClickedButtonImgSet)
    ON_BN_CLICKED(IDC_BUTTON_MEDIA_ID, &CDlgChatMsg::OnBnClickedButtonMediaId)
END_MESSAGE_MAP()

BOOL CDlgChatMsg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	initResource();
	initCtrl();
	SetTimer(TIMER_IDEVENT_QUERYISONLINE, TIMER_IDEVENT_QUERYISONLINE_INTERVAL, NULL);
    m_ImageListThumb.Create(THUMBWIDTH, THUMBHEIGHT, ILC_COLOR24, 0, 1);
    m_lstImage.SetImageList(&m_ImageListThumb, LVSIL_NORMAL);

    if (lastUploadImageInfo.mediaId.empty() && lastDownloadImageInfo.mediaId.empty()) {
        m_sendImageByMediaId.EnableWindow(FALSE);
    }
    m_hSendDC = ::GetDC(m_sta.GetSafeHwnd());
    m_hRecvDC = ::GetDC(m_staImage.GetSafeHwnd());
    RECT rect;
    m_sta.GetWindowRect(&rect);
    rcSendImage.X = 0;
    rcSendImage.Y = 0;
    rcSendImage.Width  = rect.right - rect.left;
    rcSendImage.Height = rect.bottom - rect.top;

    m_staImage.GetWindowRect(&rect);
    rcRecvImage.X = 0;
    rcRecvImage.Y = 0;
    rcRecvImage.Width  = rect.right - rect.left;
    rcRecvImage.Height = rect.bottom - rect.top;
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

    Graphics graphics(m_hSendDC);
    Image imageSend(sendImageFullPath);
    graphics.DrawImage(&imageSend, rcSendImage);

    Image image(recevImageFullPath);
    Graphics graphicsRecv(m_hRecvDC);
    graphicsRecv.DrawImage(&image, rcRecvImage);

    TCHAR szLog[MAX_PATH] = { 0 };
    _stprintf_s(szLog, MAX_PATH, L"OnPaint, sendImageFullPath:%s, recevImageFullPath:%s"
        , sendImageFullPath.GetBuffer(0), recevImageFullPath.GetBuffer(0));

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

	//delete lpData; lpData = NULL;

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

CString GetChannelSendInfo(CHANNEL_MESSAGE_ERR_CODE code)
{
    switch (code)
    {
    case CHANNEL_MESSAGE_ERR_FAILURE:
        return _T("Common failure, The user fails to send the channel message, code: 1");
    case CHANNEL_MESSAGE_ERR_SENT_TIMEOUT:
        return _T("The SDK does not receive a response from the server in 10 seconds.The current timeout is set as 10 seconds.Possible reasons : The user is in the \ref agora::rtm::CONNECTION_STATE_ABORTED or \ref agora::rtm::CONNECTION_STATE_RECONNECTING  state.");
    case CHANNEL_MESSAGE_ERR_TOO_OFTEN:
        return _T("The method call frequency exceeds the limit of 60 queries per second (channel and peer messages taken together).");
    case CHANNEL_MESSAGE_ERR_INVALID_MESSAGE:
        return _T("The message is null or exceeds 32 KB in length.");
    case CHANNEL_MESSAGE_ERR_NOT_INITIALIZED:
        return _T("ref agora::rtm::IRtmService is not initialized.");
    case CHANNEL_MESSAGE_ERR_USER_NOT_LOGGED_IN:
        return _T("The user does not call the ref agora::rtm::IRtmService::login method, or the method call of ref agora::rtm::IRtmService::login does not succeed before sending out a channel message.");
    default:
        return _T("Unkown Error");
    }
}

CString GetP2PSendInfo(PEER_MESSAGE_ERR_CODE code)
{
    switch (code)
    {
    case PEER_MESSAGE_ERR_FAILURE:
        return _T(" The sender fails to send the peer-to-peer message, code: 1.");
    case PEER_MESSAGE_ERR_SENT_TIMEOUT:
        return _T(" A timeout occurs when sending the peer-to-peer message. The current timeout is set as 10 seconds. Possible reasons: The user is in the ref agora::rtm::CONNECTION_STATE_ABORTED or ref agora::rtm::CONNECTION_STATE_RECONNECTING state.");
    case PEER_MESSAGE_ERR_PEER_UNREACHABLE:
        return _T("The specified user is offline and does not receive this peer-to-peer message.");
    case PEER_MESSAGE_ERR_CACHED_BY_SERVER:
        return _T("The receiver is offline and does not receive this offline peer-to-peer message, but the server has cached it and will re-send it once he/she is back online.");
    case PEER_MESSAGE_ERR_TOO_OFTEN:
        return _T("The method call frequency exceeds the limit of 60 queries per second (channel and peer messages taken together).");
    case PEER_MESSAGE_ERR_INVALID_USERID:
        return _T("The user ID is invalid.");
    case PEER_MESSAGE_ERR_INVALID_MESSAGE:
        return _T(" The message is null or exceeds 32 KB in length.");
    case PEER_MESSAGE_ERR_IMCOMPATIBLE_MESSAGE:
        return _T("The message receiver¡®s SDK is of an earlier version and hence cannot recognize this message.");
    case PEER_MESSAGE_ERR_NOT_INITIALIZED:
        return _T(" agora::rtm::IRtmService  is not initialized.");
    case PEER_MESSAGE_ERR_USER_NOT_LOGGED_IN:
        return _T("The user does not call the ref agora::rtm::IRtmService::login method, or the method call of ref agora::rtm::IRtmService::login does not succeed before sending out a channel message.");
    default:
        return _T("Unkown Error");
    }
}
  

LRESULT CDlgChatMsg::onMessageSendSuccess(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_MESSAGESENDSUCCESS lpData = (PAG_SIGNAL_MESSAGESENDSUCCESS)wParam;
	
    if (lParam == (LPARAM)PEER_MESSAGE_ERR_OK
        || lParam == (LPARAM)CHANNEL_MESSAGE_ERR_OK) {
        char logDesc[MAXPATHLEN] = { '\0' };
        sprintf_s(logDesc, "onMessageSendSuccess(%s,%u)", lpData->messageID.data(), lpData->messageID.size());
        LOG_MSG(logDesc, LogType_CallBack);

        long long id = strtoll(lpData->messageID.data(), NULL, 0);

        if (m_imageMessagIdSet.find(id) != m_imageMessagIdSet.end()) {
            m_staSendImageInfo.SetWindowText(_T("Send image message success."));
        }

        if (bSendImageByMedia) {
            m_sendImageByMediaId.EnableWindow(TRUE);
            bSendImageByMedia = FALSE;
        }
    }
	delete lpData; lpData = nullptr;
    
	return TRUE;
}

LRESULT CDlgChatMsg::onMessageSendError(WPARAM wParam, LPARAM lParam)
{
    PAG_SIGNAL_MESSAGESENDSUCCESS lpData = (PAG_SIGNAL_MESSAGESENDSUCCESS)wParam;
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "%s,%u,%d", lpData->messageID.data(), lpData->messageID.size(), lParam);
	LOG_MSG(logDesc, LogType_CallBack);

    long long msgId = strtoll(lpData->messageID.c_str(), NULL, 0);
    if (m_mapImageMsg.find(msgId) != m_mapImageMsg.end()) {
        CString strInfo;
        if (P2PImageMsg) {
            strInfo = GetP2PSendInfo((PEER_MESSAGE_ERR_CODE)lParam);
            m_staSendImageInfo.SetWindowText(strInfo);
        }
        else {
            strInfo = GetChannelSendInfo((CHANNEL_MESSAGE_ERR_CODE)lParam);
            m_staSendImageInfo.SetWindowText(strInfo);
        }
    }
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

    m_mapRecvImageMsg.clear();
    m_mapImageMsg.clear();  
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

void CDlgChatMsg::OnBnClickedSendImage()
{
    CString str = m_pDlgInput->GetInputString();
    if (m_curOptionType == eType_Instance) {
        if (str.IsEmpty()) {
            AfxMessageBox(_T("Input UID by clicking Peer-to-Peer message"));
            return;
        }
    }
    else if (m_curOptionType == eType_Channel) {
        if (str.IsEmpty()) {
            AfxMessageBox(_T("Input UID by clicking In-Channel message"));
            return;
        }
    }

    P2PImageMsg = m_curOptionType == eType_Instance;
    //JPG, JPEG, BMP, or PNG
    TCHAR szFilters[] = _T("Image Files (*.jpg,.JPEG,*.BMP,*.PNG)||");

    // Create an Open dialog; the default file name extension is ".my".
    CFileDialog fileDlg(TRUE, _T("All Files"), _T("*.*"),
        OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

    if (fileDlg.DoModal() == IDOK)
    {
   
        m_staSendImageInfo.SetWindowText(_T(""));
        //Change the window's title to the opened file's title.
        sendImageFullPath = fileDlg.GetPathName();

        TCHAR szLog[MAX_PATH] = { 0 };
        _stprintf_s(szLog, MAX_PATH, L"OnBnClickedSendImage, sendImageFullPath:%s, recevImageFullPath:%s"
            , sendImageFullPath.GetBuffer(0), recevImageFullPath.GetBuffer(0));


        Image image(sendImageFullPath);
        FILE* fp = NULL;
        fopen_s(&fp, cs2s(sendImageFullPath).c_str(), "rb");
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        if (size > 30 * 1024 * 1024) {
            AfxMessageBox(_T("Image size larger than 30MB"));
            return;
        }
        Gdiplus::Graphics graphics(m_hSendDC);
        graphics.DrawImage(&image, rcSendImage);

        int imageWidth  = image.GetWidth();
        int imageHeight = image.GetHeight();
        int thumbWidth  = imageWidth;
        int thumbHeight = imageHeight;
        int r = 8;
        if (size + fileDlg.GetFileName().GetLength() > 32 * 1024) {
            int ratio_square = size / (32 * 1024) + 1;
            int ratio   = sqrt(ratio_square) + 1;
            thumbWidth  = imageWidth / (r*(float)ratio);
            thumbHeight = imageHeight / (r*(float)ratio);
        }
        m_pSignalCallBack->SetImageInfo(imageWidth, imageHeight, thumbWidth, thumbHeight);

        long long requestId = 0;
        if (m_pSignalInstance->uploadImage(cs2Utf8(sendImageFullPath), requestId)) {
            AG_IMAGE_MESSAGE info;
            info.fileName    = cs2Utf8(fileDlg.GetFileName());
            info.filePath    = cs2Utf8(sendImageFullPath);
            info.thumbnailWidth  = thumbWidth;
            info.thumbnailHeight = thumbHeight;
            m_mapImageMsg.insert(std::make_pair(requestId, info));
            m_btnSendImage.EnableWindow(FALSE);
            currentImageMsgRequestId = requestId;
            m_btnCancel.EnableWindow(TRUE);
        }
    }
}

LRESULT CDlgChatMsg::onRecvImageMsgfromPeer(WPARAM wParam, LPARAM lParam)
{
    PAG_IMAGE_MESSAGE lpData = (PAG_IMAGE_MESSAGE)wParam;
   
    if (!lpData)
        return 0;

    char szLog[MAX_PATH] = { 0 };
    sprintf_s(szLog, MAX_PATH, "onRecvImageMsgfromPeer, thumbFile:%s", lpData->thumbFile);
    LOG_MSG(szLog);
    recevImageFullPath = s2cs(lpData->thumbFile);
    if (PathFileExists(recevImageFullPath)) {
        Image image(recevImageFullPath);
        Graphics graphics(m_hRecvDC);
        graphics.DrawImage(&image, rcRecvImage);

        TCHAR szLog[MAX_PATH] = { 0 };
        _stprintf_s(szLog, MAX_PATH, L"onRecvImageMsgfromPeer, sendImageFullPath:%s, recevImageFullPath:%s"
            , sendImageFullPath.GetBuffer(0), recevImageFullPath.GetBuffer(0));
    }
    CString strInfo;
    strInfo.Format(_T("Recv Image from:%s"), utf82cs(lpData->peerId));
    m_staRecvImageinfo.SetWindowText(strInfo);
  
    CString fileName = utf82cs(lpData->filePath);
    {
        long long requestId = -1;
        sprintf_s(szLog, MAX_PATH, "onRecvImageMsgfromPeer, filePath:%s", cs2s(fileName));
        LOG_MSG(szLog);
        if (m_pSignalInstance->downloadImage(lpData->filePath.c_str(), lpData->mediaId.c_str(), requestId)) {
            lastDownloadImageInfo.mediaId      = lpData->mediaId;
            lastDownloadImageInfo.imageMessage = *lpData;
            currentDownloadImageMsgRequestId   = requestId;
            m_mapRecvImageMsg.insert(std::make_pair(requestId, *lpData));
        }

    }
   
    delete lpData;
    lpData = NULL;
    return 0;
}

LRESULT CDlgChatMsg::onRecvImageMsgfromChannel(WPARAM wParam, LPARAM lParam)
{
    PAG_IMAGE_MESSAGE lpData = (PAG_IMAGE_MESSAGE)wParam;

    if (!lpData)
        return 0;
  
    recevImageFullPath = s2cs(lpData->thumbFile);
    if (PathFileExists(recevImageFullPath)) {
        Image image(recevImageFullPath);
        Graphics graphics(m_hRecvDC);
        graphics.DrawImage(&image, rcRecvImage);
    }
    CString strInfo;
    strInfo.Format(_T("Recv Image from:%s"), utf82cs(lpData->peerId));
    m_staRecvImageinfo.SetWindowText(strInfo);

    recevImageFullPath = utf82cs(lpData->filePath);

    TCHAR szLog[MAX_PATH] = { 0 };
    _stprintf_s(szLog, MAX_PATH, L"onRecvImageMsgfromChannel, sendImageFullPath:%s, recevImageFullPath:%s"
        , sendImageFullPath.GetBuffer(0), recevImageFullPath.GetBuffer(0));
    {
        long long requestId = -1;
        if (m_pSignalInstance->downloadImage(lpData->filePath.c_str(), lpData->mediaId.c_str(), requestId)) {
            AG_IMAGE_MESSAGE info;
            lastDownloadImageInfo.imageMessage = *lpData;
            lastDownloadImageInfo.mediaId = lpData->mediaId;
            currentDownloadImageMsgRequestId = requestId;
            m_mapRecvImageMsg.insert(std::make_pair(requestId, *lpData));
            m_sendImageByMediaId.EnableWindow(TRUE);
        }
    }

    delete lpData;
    lpData = NULL;
    return 0;
}

LRESULT CDlgChatMsg::onMediaUploadProgress(WPARAM wParam, LPARAM lParam)
{
    PMediaProgress uploadProgress = (PMediaProgress)wParam;
    if (uploadProgress) {
        if (uploadProgress->totalSize != 0) {
            CString strInfo;
            strInfo.Format(_T("Upload %.2f"), 100 * ((float)uploadProgress->currentSize / (float)uploadProgress->totalSize));
            strInfo += "%";
            m_staSendImageInfo.SetWindowText(strInfo);
        }


        delete uploadProgress;
        uploadProgress = nullptr;
    }
    return 0;
}
LRESULT CDlgChatMsg::onMediaDownloadloadProgress(WPARAM wParam, LPARAM lParam)
{
    CString userAccount = m_pDlgInput->GetInputString();
    PMediaProgress downloadProgress = (PMediaProgress)wParam;
    if (downloadProgress) {
        if (currentDownloadImageMsgRequestId == -1)
            return 0;
        AG_IMAGE_MESSAGE info = m_mapRecvImageMsg[currentDownloadImageMsgRequestId];
        if (downloadProgress->totalSize != 0) {
            CString strInfo;
            strInfo.Format(_T("Recv Image from %s:download %.2f"), userAccount.GetBuffer(0), 100 * ((float)downloadProgress->currentSize / (float)downloadProgress->totalSize));
            strInfo += "%";
            m_staRecvImageinfo.SetWindowText(strInfo);
        }

        delete downloadProgress;
        downloadProgress = nullptr;
    }
    return 0;
}

LRESULT CDlgChatMsg::onImageMediaDownloadResult(WPARAM wParam, LPARAM lParam)
{
    long long requestId = (long long)wParam;
    DOWNLOAD_MEDIA_ERR_CODE code = (DOWNLOAD_MEDIA_ERR_CODE)lParam;
    AG_IMAGE_MESSAGE info = m_mapRecvImageMsg[requestId];
    CString userAccount = m_pDlgInput->GetInputString();
    if (code == DOWNLOAD_MEDIA_ERR_OK) {
       
        recevImageFullPath = utf82cs(info.filePath);
        if (PathFileExists(recevImageFullPath)) {
            Image image(recevImageFullPath);
            Graphics graphics(m_hRecvDC);
            graphics.DrawImage(&image, rcRecvImage);

            TCHAR szLog[MAX_PATH] = { 0 };
            _stprintf_s(szLog, MAX_PATH, L"onImageMediaDownloadResult, sendImageFullPath:%s, recevImageFullPath:%s"
                , sendImageFullPath.GetBuffer(0), recevImageFullPath.GetBuffer(0));

            CString strInfo;
            strInfo.Format(_T("Recv Image from:%s"), userAccount);
        }
    }
    else {
        CString strInfo;
        switch (code)
        {
        case DOWNLOAD_MEDIA_ERR_FAILURE:
            strInfo = _T("Unknown common failure");
            break;
        case DOWNLOAD_MEDIA_ERR_INVALID_ARGUMENT:
            strInfo = _T("invalid argument");
            break;
        case DOWNLOAD_MEDIA_ERR_TIMEOUT:
            strInfo = _T("download timeout");
            break;
        case DOWNLOAD_MEDIA_ERR_NOT_EXIST:
            strInfo = _T(" The size of image to download exceeds 30 MB.");
            break;
        case DOWNLOAD_MEDIA_ERR_CONCURRENCY_LIMIT_EXCEEDED:
            strInfo = _T(" exceeded the upper limit for file download.");
            break;
        case DOWNLOAD_MEDIA_ERR_INTERRUPTED:
            strInfo = _T("cancelled the download task");
            break;
        case DOWNLOAD_MEDIA_ERR_NOT_INITIALIZED:
            strInfo = _T("IRtmService is not initialized");
            break;
        case DOWNLOAD_MEDIA_ERR_NOT_LOGGED_IN:
            strInfo = _T("not call IRtmService::login");
            break;
        default:
            break;
        }
        m_staRecvImageinfo.SetWindowTextW(strInfo);
    }
    m_btnCancelDownload.EnableWindow(FALSE);
    m_sendImageByMediaId.EnableWindow(TRUE);
    return 0;
}


LRESULT CDlgChatMsg::onImageMediaUploadResult(WPARAM wParam, LPARAM lParam)
{
    UPLOAD_MEDIA_ERR_CODE err = (UPLOAD_MEDIA_ERR_CODE)lParam;
    if (err == UPLOAD_MEDIA_ERR_OK) {
        PImageMediaUploadResult imageUploadResult = (PImageMediaUploadResult)wParam;
        CString userAccount = m_pDlgInput->GetInputString();
        if (imageUploadResult) {
            AG_IMAGE_MESSAGE info = m_mapImageMsg[imageUploadResult->requestId];
            if (imageUploadResult->err == UPLOAD_MEDIA_ERR_OK) {
                imageUploadResult->imageMessage->setFileName(info.fileName.c_str());
                
                CAgoraRTMInstance::SetThumbnail(info, *imageUploadResult->imageMessage);

                m_staSendImageInfo.SetWindowText(_T("Upload Finished"));
                m_imageMessagIdSet.insert(imageUploadResult->imageMessage->getMessageId());
                int bEnableOfflineMessage = ((CButton*)(GetDlgItem(IDC_CHECK_EnableOffLineMsg)))->GetCheck();
                if (m_pSignalInstance->SendImageMsg(cs2Utf8(userAccount), imageUploadResult->imageMessage, P2PImageMsg, bEnableOfflineMessage)) {
                    lastUploadImageInfo.mediaId = imageUploadResult->imageMessage->getMediaId();
                    m_sendImageByMediaId.EnableWindow(TRUE);
                    lastUploadImageInfo.imageMessage = info;
                }
                else
                    m_staSendImageInfo.SetWindowText(_T("send image message failed"));

            }
            else {
                CString strInfo;
                switch (imageUploadResult->err)
                {
                case UPLOAD_MEDIA_ERR_FAILURE:
                    strInfo = _T("Unknown common failure");
                    break;
                case UPLOAD_MEDIA_ERR_INVALID_ARGUMENT:
                    strInfo = _T("invalid argument");
                    break;
                case UPLOAD_MEDIA_ERR_TIMEOUT:
                    strInfo = _T("upload timeout");
                    break;
                case UPLOAD_MEDIA_ERR_SIZE_OVERFLOW:
                    strInfo = _T(" The size of image to upload exceeds 30 MB.");
                    break;
                case UPLOAD_MEDIA_ERR_CONCURRENCY_LIMIT_EXCEEDED:
                    strInfo = _T(" exceeded the upper limit for file upload.");
                    break;
                case UPLOAD_MEDIA_ERR_INTERRUPTED:
                    strInfo = _T("cancelled the upload task");
                    break;
                case UPLOAD_MEDIA_ERR_NOT_INITIALIZED:
                    strInfo = _T("IRtmService is not initialized");
                    break;
                case UPLOAD_MEDIA_ERR_NOT_LOGGED_IN:
                    strInfo = _T("not call IRtmService::login");
                    break;
                default:
                    break;
                }
                m_staSendImageInfo.SetWindowTextW(strInfo);
            }

            delete imageUploadResult;
            imageUploadResult = nullptr;
        }
        currentImageMsgRequestId = -1;
    }
    else {
        CString strInfo;
        strInfo.Format(_T("Upload Failed ,err: %d"), lParam);
        m_staSendImageInfo.SetWindowText(strInfo);
    }
    
    m_btnSendImage.EnableWindow(TRUE);
    m_btnCancel.EnableWindow(FALSE);
    return 0;
}

void CDlgChatMsg::OnBnClickedButtonCancel()
{
    if (currentImageMsgRequestId != -1) {
        m_pSignalInstance->CancelMediaUpload(currentImageMsgRequestId);
    }
}


void CDlgChatMsg::OnStnClickedStaticShow()
{
    
}


void CDlgChatMsg::OnBnClickedButtonCancel2()
{
    if (currentDownloadImageMsgRequestId != -1) {
        m_pSignalInstance->CancelMediaDownload(currentDownloadImageMsgRequestId);
    }
}


void CDlgChatMsg::OnBnClickedButtonImgSet()
{
    CDlgImageSettings dlg;
    if (IDOK == dlg.DoModal()) {
        m_pSignalCallBack->SetImageInfo(dlg.width, dlg.height, dlg.thumb_width, dlg.thumb_height);
    }
}


void CDlgChatMsg::OnBnClickedButtonMediaId()
{
    P2PImageMsg = m_curOptionType == eType_Instance;
    CString str = m_pDlgInput->GetInputString();
    if (m_curOptionType == eType_Instance) {
        if (str.IsEmpty()) {
            AfxMessageBox(_T("Input UID by clicking Peer-to-Peer message"));
            return;
        }
    }
    else if (m_curOptionType == eType_Channel) {
        if (str.IsEmpty()) {
            AfxMessageBox(_T("Input UID by clicking In-Channel message"));
            return;
        }
    }

    int bEnableOfflineMessage = ((CButton*)(GetDlgItem(IDC_CHECK_EnableOffLineMsg)))->GetCheck();
    if (!lastUploadImageInfo.mediaId.empty()) {
        
        if (m_pSignalInstance->SendImageMsg(cs2Utf8(str), lastUploadImageInfo, P2PImageMsg, bEnableOfflineMessage)) {
            bSendImageByMedia = true;
            m_sendImageByMediaId.EnableWindow(FALSE);
            return;
        }
    }
    if (!lastDownloadImageInfo.mediaId.empty()) {
        if (m_pSignalInstance->SendImageMsg(cs2Utf8(str), lastDownloadImageInfo, P2PImageMsg, bEnableOfflineMessage)) {
            bSendImageByMedia = true;
            m_sendImageByMediaId.EnableWindow(FALSE);
            return;
        }
    }
}
