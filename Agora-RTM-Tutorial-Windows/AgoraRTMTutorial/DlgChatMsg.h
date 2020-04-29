#pragma once


#include <map>
#include <set>
#include "AgoraRTMInstance.h"
#include "RTMWrap.h"
// CDlgChatMsg Dialog

class CChatDlg;
class CAgoraSignalInstance;
class CSingleCallBack;
class CDlgInput;
#define THUMBWIDTH   100
#define THUMBHEIGHT  90



class CDlgChatMsg : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgChatMsg)

public:
	CDlgChatMsg(CWnd* pParent = NULL);
	virtual ~CDlgChatMsg();
	void setCallBack(CRTMCallBack* pCallBack);

// Dialog Data
	enum { IDD = IDD_DIALOG_MsgList };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    //DDX/DDV Support
	virtual BOOL OnInitDialog(); 


	DECLARE_MESSAGE_MAP()

private:
	void initCtrl();
	void uninitCtrl();

	void initResource();
	void uninitResource();

	void chooseInstance(const std::string &userAccount);
	void chooseChannel(const std::string &channelName);
	void showChannelInfo(const std::string &channelName);
	
public:
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnBnClickedButtonP2pmsg();
	afx_msg void OnBnClickedButtonChannelmsg();

	LRESULT onUpdateInputParam(WPARAM wParam, LPARAM lParam);
	LRESULT onQueryUserStatusResult(WPARAM wParam, LPARAM lParam);
	LRESULT onMessageSendSuccess(WPARAM wParam, LPARAM lParam);
    LRESULT onImageMediaUploadResult(WPARAM wParam, LPARAM lParam);
    LRESULT onImageMediaDownloadResult(WPARAM wParam, LPARAM lParam);
    LRESULT onRecvImageMsgfromPeer(WPARAM wParam, LPARAM lParam);
    LRESULT onRecvImageMsgfromChannel(WPARAM wParam, LPARAM lParam);
    LRESULT onMediaUploadProgress(WPARAM wParam, LPARAM lParam);
    LRESULT onMediaDownloadloadProgress(WPARAM wParam, LPARAM lParam);

	LRESULT onMessageSendError(WPARAM wParam, LPARAM lParam);
	LRESULT onMessageInstantReceive(WPARAM wParam, LPARAM lParam);
	LRESULT onMessageChannelReceive(WPARAM wParam, LPARAM lParam);

	LRESULT onChannelJoined(WPARAM wParam, LPARAM lParam);
	LRESULT onChannelJoinFailed(WPARAM wParam, LPARAM lParam);
	LRESULT onChannelLeaved(WPARAM wParam, LPARAM lParam);
	LRESULT onChannelUserList(WPARAM wParam,LPARAM lParam);
	LRESULT onChannelQueryUserNumResult(WPARAM wParam, LPARAM lParam);


private:
	std::map<std::string, CChatDlg* > m_mapChatP2P;
	std::map<std::string, CChatDlg*> m_mapChatChannel;
	CChatDlg* m_pCurChat;
	std::string m_curAccount;
	std::string m_curChannel;
	CAgoraRTMInstance *m_pSignalInstance;
	CRTMCallBack *m_pSignalCallBack;
	CDlgInput* m_pDlgInput;
	eTagMsgtype m_curOptionType;

    std::string recvUid;
    std::map<long long, AG_IMAGE_MESSAGE> m_mapImageMsg;
    std::map<long long, AG_IMAGE_MESSAGE> m_mapRecvImageMsg;
    std::set<long long> m_imageMessagIdSet;

    long long currentImageMsgRequestId = -1;
    long long currentDownloadImageMsgRequestId = -1;
    bool P2PImageMsg = false;
    CString imageUID;
    CString imageChannel;

    LastImageInfo lastUploadImageInfo;
    LastImageInfo lastDownloadImageInfo;
  
    bool bSendImageByMedia;
public:
    CListCtrl m_lstImage;
    afx_msg void OnBnClickedSendImage();
   //CEdit m_edtUID;
    CImageList m_ImageListThumb;
    CStatic m_staImage;
    CStatic m_sta;
    CStatic m_staSendImageInfo;
    CStatic m_staRecvImageinfo;
    CButton m_btnSendImage;
    CButton m_btnCancel;
    afx_msg void OnBnClickedButtonCancel();
    afx_msg void OnStnClickedStaticShow();
    afx_msg void OnBnClickedButtonCancel2();
    afx_msg void OnBnClickedButtonImgSet();
    afx_msg void OnBnClickedButtonMediaId();
    CButton m_sendImageByMediaId;
    CButton m_btnCancelDownload;
};

