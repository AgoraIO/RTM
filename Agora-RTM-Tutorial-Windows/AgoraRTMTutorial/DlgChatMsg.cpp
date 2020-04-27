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
    DDX_Control(pDX, IDC_STATIC_SENDIMAGE, m_staSendImage);
    DDX_Control(pDX, IDC_STATIC_IMAGE_INFO, m_staRecvImageinfo);
    DDX_Control(pDX, IDC_BUTTON1, m_btnSendImage);
    DDX_Control(pDX, IDC_BUTTON_CANCEL, m_btnCancel);
    DDX_Control(pDX, IDC_BUTTON_MEDIA_ID, m_sendImageByMediaId);
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

    lastUploadMediaId = gConfigSignal.getLastUploadMediaId();
    lastDownloadMediaId = gConfigSignal.getLastDownloadMediaId();

    if (lastUploadMediaId.empty() && lastDownloadMediaId.empty()) {
        m_sendImageByMediaId.EnableWindow(FALSE);
    }
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

void CDlgChatMsg::OnBnClickedSendImage()
{
   /* CDlgImageSettings dlgSettings;
    if (IDOK != dlgSettings.DoModal()) {
        return;
    }
    P2PImageMsg  = dlgSettings.bP2P;
    imageUID     = dlgSettings.strUID;
    imageChannel = dlgSettings.strChannel;*/
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
    if (P2PImageMsg) {
        imageUID = str;
    }
    else {
        imageChannel = str;
    }
    TCHAR szFilters[] = _T("MyType Files (*.*)|*.*|All Files (*.*)|*.*||");

    // Create an Open dialog; the default file name extension is ".my".
    CFileDialog fileDlg(TRUE, _T("All Files"), _T("*.*"),
        OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

    if (fileDlg.DoModal() == IDOK)
    {
        CString pathName = fileDlg.GetPathName();
        m_sta.SetWindowText(_T(""));
       
        //Change the window's title to the opened file's title.
        CString fileName = fileDlg.GetPathName();
        Image image(fileName);
        std::string filePath = cs2Utf8(fileName);
        FILE* fp = NULL;
        fopen_s(&fp, filePath.c_str(), "rb");
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        if (size > 32 * 1024 * 1024) {
            AfxMessageBox(_T("Image size larger than 32MB"));
            return;
        }

       
        Gdiplus::Graphics graphics(::GetDC(m_sta.GetSafeHwnd()));
        Rect rc;
        RECT rect;
        m_sta.GetWindowRect(&rect);
        rc.X = 0;
        rc.Y = 0;
        rc.Width  = rect.right - rect.left;
        rc.Height = rect.bottom - rect.top;
        graphics.DrawImage(&image, rc);

        int thumbWidth  = rc.Width;
        int thumbHeight = rc.Height;
        if (size > 32 * 1024) {
            int ratio_square   = size / (32 * 1024) + 1;
            int ratio = sqrt(ratio_square) + 1;
            thumbWidth  = rc.Width / (float)ratio;
            thumbHeight = rc.Height / (float)ratio;
        }

       // image.GetThumbnailImage()
        std::string file = cs2Utf8(fileDlg.GetFileName());
       
        long long requestId = 0;
        
        if (m_pSignalInstance->uploadImage(filePath, requestId)) {
            ImageMsgInfo info;
            info.fileName    = cs2Utf8(fileDlg.GetFileName());
            info.userAccount = cs2Utf8(imageUID);
            info.imageMsg    = nullptr;
            info.fullPath    = fileDlg.GetPathName();
            info.expectedThumbWidth  = thumbWidth;
            info.expectedThumbHeight = thumbHeight;
            m_mapImageMsg.insert(std::make_pair(requestId, info));
            m_btnSendImage.EnableWindow(FALSE);
            currentImageMsgRequestId = requestId;
        }
    }
}

LRESULT CDlgChatMsg::onRecvImageMsgfromPeer(WPARAM wParam, LPARAM lParam)
{
    PAG_IMAGE_MESSAGE lpData = (PAG_IMAGE_MESSAGE)wParam;

    if (!lpData)
        return 0;
    TCHAR szFile[MAX_PATH] = { 0 };
    MultiByteToWideChar(CP_UTF8, 0, lpData->thumbFile.c_str(), lpData->thumbFile.length(), szFile, MAX_PATH);

    if (PathFileExists(szFile)) {
        Image image(szFile);

        Graphics graphics(::GetDC(m_staImage.GetSafeHwnd()));
        Rect rc;
        RECT rect;
        m_staImage.GetWindowRect(&rect);

        rc.X = 0;
        rc.Y = 0;
        rc.Width = rect.right - rect.left;
        rc.Height = rect.bottom - rect.top;

        graphics.DrawImage(&image, rc);

    }
    CString strInfo;
    strInfo.Format(_T("Recv Image from:%s"), utf82cs(lpData->peerId));
    m_staRecvImageinfo.SetWindowText(strInfo);
  
    CString fileName = utf82cs(lpData->filePath);
    {
        long long requestId = -1;
        if (m_pSignalInstance->downloadImage(lpData->filePath.c_str(), lpData->mediaId.c_str(), requestId)) {
            ImageMsgInfo info;
            info.fileName    = lpData->filePath;
            info.userAccount = lpData->peerId;
            info.imageMsg    = nullptr;
            currentDownloadImageMsgRequestId = requestId;
            m_mapRecvImageMsg.insert(std::make_pair(requestId, info));
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
    TCHAR szFile[MAX_PATH] = { 0 };
    MultiByteToWideChar(CP_UTF8, 0, lpData->thumbFile.c_str(), lpData->thumbFile.length(), szFile, MAX_PATH);

    if (PathFileExists(szFile)) {
        Image image(szFile);

        Graphics graphics(::GetDC(m_staImage.GetSafeHwnd()));
        Rect rc;
        RECT rect;
        m_staImage.GetWindowRect(&rect);

        rc.X = 0;
        rc.Y = 0;
        rc.Width = rect.right - rect.left;
        rc.Height = rect.bottom - rect.top;

        graphics.DrawImage(&image, rc);

    }
    CString strInfo;
    strInfo.Format(_T("Recv Image from:%s"), utf82cs(lpData->peerId));
    m_staRecvImageinfo.SetWindowText(strInfo);

    CString fileName = utf82cs(lpData->filePath);
    {
        long long requestId = -1;
        if (m_pSignalInstance->downloadImage(lpData->filePath.c_str(), lpData->mediaId.c_str(), requestId)) {
            ImageMsgInfo info;
            info.fileName    = lpData->filePath;
            info.userAccount = lpData->peerId;
            info.imageMsg    = nullptr;
            currentDownloadImageMsgRequestId = requestId;
            m_mapRecvImageMsg.insert(std::make_pair(requestId, info));
            lastDownloadMediaId = lpData->mediaId;
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

    CString strInfo;
  
    strInfo.Format(_T("Upload %.2f"), 100 * ((float)uploadProgress->currentSize / (float)uploadProgress->totalSize));
    strInfo += "%";
    m_staSendImage.SetWindowText(strInfo);

    if (uploadProgress) {
        delete uploadProgress;
        uploadProgress = nullptr;
    }
    return 0;
}
LRESULT CDlgChatMsg::onMediaDownloadloadProgress(WPARAM wParam, LPARAM lParam)
{
    PMediaProgress downloadProgress = (PMediaProgress)wParam;
    ImageMsgInfo info = m_mapRecvImageMsg[currentDownloadImageMsgRequestId];
    CString strInfo;
    strInfo.Format(_T("Recv Image from %s:download %.2f"), utf82cs( info.userAccount.c_str()), 100 * ((float)downloadProgress->currentSize / (float)downloadProgress->totalSize));
    strInfo += "%";
    m_staRecvImageinfo.SetWindowText(strInfo);

    if (downloadProgress) {
        delete downloadProgress;
        downloadProgress = nullptr;
    }
    return 0;
}

LRESULT CDlgChatMsg::onImageMediaDownloadResult(WPARAM wParam, LPARAM lParam)
{
    long long requestId = (long long)wParam;
    DOWNLOAD_MEDIA_ERR_CODE code = (DOWNLOAD_MEDIA_ERR_CODE)lParam;
    ImageMsgInfo info = m_mapRecvImageMsg[requestId];

    if (code == DOWNLOAD_MEDIA_ERR_OK) {
        TCHAR szFile[MAX_PATH] = { 0 };
        MultiByteToWideChar(CP_UTF8, 0, info.fileName.c_str(), info.fileName.length(), szFile, MAX_PATH);

        if (PathFileExists(szFile)) {
            Image image(szFile);

            Graphics graphics(::GetDC(m_staImage.GetSafeHwnd()));
            Rect rc;
            RECT rect;
            m_staImage.GetWindowRect(&rect);

            rc.X = 0;
            rc.Y = 0;
            rc.Width = rect.right - rect.left;
            rc.Height = rect.bottom - rect.top;

            graphics.DrawImage(&image, rc);

            CString strInfo;
            strInfo.Format(_T("Recv Image from:%s"), utf82cs(info.userAccount.c_str()));
           
            m_sendImageByMediaId.EnableWindow(TRUE);
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
            strInfo = _T(" The size of image to upload exceeds 30 MB.");
            break;
        case DOWNLOAD_MEDIA_ERR_CONCURRENCY_LIMIT_EXCEEDED:
            strInfo = _T(" exceeded the upper limit for file upload.");
            break;
        case DOWNLOAD_MEDIA_ERR_INTERRUPTED:
            strInfo = _T("cancelled the upload task");
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
   
    return 0;
}

int GetEncoderClsid(WCHAR *format, CLSID *pClsid)
{
    unsigned int num = 0, size = 0;
    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) return -1;
    Gdiplus::ImageCodecInfo *pImageCodecInfo = (Gdiplus::ImageCodecInfo *)(malloc(size));
    if (pImageCodecInfo == NULL) return -1;
    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
    for (unsigned int j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }
    free(pImageCodecInfo);
    return -1;
}


bool SetThumbnail(ImageMsgInfo& info)
{
    Image image(info.fullPath);
    Image* pThumbnail = image.GetThumbnailImage(info.expectedThumbWidth, info.expectedThumbHeight);
    
    LONG uQuality = 100L;
    CLSID imageCLSID;
    Gdiplus::EncoderParameters encoderParams;
    encoderParams.Count = 1;
    encoderParams.Parameter[0].NumberOfValues = 1;
    encoderParams.Parameter[0].Guid = Gdiplus::EncoderQuality;
    encoderParams.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
    encoderParams.Parameter[0].Value = &uQuality;
    GetEncoderClsid(L"image/png", &imageCLSID);

  
    IStream* pIStream = nullptr;
    if (CreateStreamOnHGlobal(NULL, FALSE, (LPSTREAM*)&pIStream) != S_OK){
        AfxMessageBox(_T("create stream failed"));
        return false;
    }
    pThumbnail->Save(pIStream, &imageCLSID, NULL);

    STATSTG sts;
    pIStream->Stat(&sts, STATFLAG_DEFAULT);
    ULARGE_INTEGER uli = sts.cbSize;
    LARGE_INTEGER zero;
    zero.QuadPart = 0;
    int size = (int)uli.QuadPart;
    BYTE* imageData = new BYTE[size];
    ULONG written;
    pIStream->Seek(zero, STREAM_SEEK_SET, NULL);
    pIStream->Read(imageData, size, &written);

    info.imageMsg->setThumbnail(imageData, size);
    info.imageMsg->setThumbnailWidth(info.expectedThumbWidth);
    info.imageMsg->setThumbnailHeight(info.expectedThumbHeight);

    delete[] imageData;
    pIStream->Release();
}

LRESULT CDlgChatMsg::onImageMediaUploadResult(WPARAM wParam, LPARAM lParam)
{
    PImageMediaUploadResult imageMsg = (PImageMediaUploadResult)wParam;

    if (imageMsg) {
        ImageMsgInfo info = m_mapImageMsg[imageMsg->requestId];
        if (imageMsg->err == UPLOAD_MEDIA_ERR_OK) {
            imageMsg->imageMessage->setFileName(info.fileName.c_str()); 
            info.imageMsg = imageMsg->imageMessage;
            SetThumbnail(info);
            
            m_pSignalInstance->SendImageMsg(info.userAccount, imageMsg->imageMessage, P2PImageMsg);
            lastUploadMediaId = imageMsg->imageMessage->getMediaId();
            m_sendImageByMediaId.EnableWindow(TRUE);
        }
        else {
            CString strInfo;
            switch (imageMsg->err)
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
            m_staSendImage.SetWindowTextW(strInfo);
        }

        delete imageMsg;
        imageMsg = nullptr;
    }
    currentImageMsgRequestId = -1;
    m_btnSendImage.EnableWindow(TRUE);
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


    if (!lastUploadMediaId.empty()) {
        
        if (m_pSignalInstance->SendImageMsg(cs2Utf8(str), lastUploadMediaId.c_str(), P2PImageMsg))
            return;
    }


    if (!lastDownloadMediaId.empty()) {
       
        if (m_pSignalInstance->SendImageMsg(cs2Utf8(str), lastDownloadMediaId.c_str(), P2PImageMsg))
            return;
    }
}
