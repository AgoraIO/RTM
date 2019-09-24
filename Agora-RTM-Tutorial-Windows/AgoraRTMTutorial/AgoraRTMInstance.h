#pragma once

#include <memory>
#include <unordered_map>

#include "IAgoraRtmService.h"
#include "IAgoraService.h"
#include "RTmWrap.h"

using namespace agora::rtm;

class CAgoraRTMInstance
{
public:
  static CAgoraRTMInstance* getSignalInstance(const std::string &appId = "", CRTMCallBack* callBack = nullptr);
  void ReleaseInstance();
  ~CAgoraRTMInstance();

  void setLoginWnd(HWND wnd);
  HWND getLoginWnd();
  void setChatMsgWnd(HWND wnd);
  HWND getChatMsgWnd();

  bool getptStatus();
  void setptStatus(bool status);
  bool getChannelStatus();
  void setChannleStatus(bool status);

  void setAppId(const std::string &appId);
  std::string getAppId();
  void setAppCertificateId(const std::string &certificateId);
  std::string getCertificateId();
  std::string getLoginAccount();

  IRtmService* getAgoraAPI();

  bool Login(const std::string &account, const std::string & token);
  bool LogOut();

  bool QueryIsOnline(const std::string &account);
  bool sendInstantMsg(const std::string &account, const std::string &instanmsg,bool bEnableOfflineMessage);
  bool sendChannelMsg(const std::string &channel, const std::string &ChannelMsg,bool bEnableOfflineMessage);
  bool ChannelQueryNumResult(const std::string &channelID);

  bool JoinChannel(const std::string &channelID);
  bool LeaveChannel();

  std::string getSDKVersion();

protected:
  CAgoraRTMInstance(const std::string appId, CRTMCallBack* callBack);
  void InitRTMService();

private:

  static CAgoraRTMInstance* m_pAgoraRTMInstance;

  bool m_isLoginPt;
  bool m_isJoinChannel;

  std::string m_Account;
  std::string m_ChannelName;

  std::string m_AppId;
  std::string m_AppCertificateId;

  std::unique_ptr<agora::base::IAgoraService> m_agoraService;
  agora::base::AgoraServiceContext context_;
  CRTMCallBack *m_RtmCallback;
  IRtmService* m_rtmService;
  IChannel* m_Channel;

  HWND m_loginWnd;
  HWND m_ChatMsg;
};