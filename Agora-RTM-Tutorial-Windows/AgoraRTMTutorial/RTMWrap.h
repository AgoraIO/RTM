#ifndef __SINGLEWRAP_H__
#define __SINGLEWRAP_H__

#include "IAgoraRtmService.h"
using namespace agora::rtm;
class CRTMCallBack :public agora::rtm::IRtmServiceEventHandler,
  public agora::rtm::IChannelEventHandler {
public:
  CRTMCallBack(HWND wnd);
  ~CRTMCallBack();

  void setCallBackWnd(HWND wnd);
  void setJoinChannel(const std::string & channelId) {
    m_channeId = channelId;
  }

protected:
  // Inherited from |agora::rtm::IRtmServiceEventHandler|
  void onLoginSuccess() override;

  void onLoginFailure(LOGIN_ERR_CODE errorCode) override;

  void onLogout(LOGOUT_ERR_CODE errorCode) override;

  void onConnectionStateChanged(CONNECTION_STATE state, CONNECTION_CHANGE_REASON reason) override;

  void onSendMessageResult(long long messageId, PEER_MESSAGE_ERR_CODE errorCode) override;

  void onMessageReceivedFromPeer(const char *peerId, const IMessage *message) override;

  // Inherited from |agora::rtm::IChannelEventHandler|
  void onJoinSuccess() override;

  void onJoinFailure(JOIN_CHANNEL_ERR errorCode) override;

  void onLeave(LEAVE_CHANNEL_ERR errorCode) override;

  void onMessageReceived(const char *userId, const IMessage *message) override;

  void onSendMessageResult(long long messageId, CHANNEL_MESSAGE_ERR_CODE state) override;

  void onMemberJoined(IChannelMember *member) override;

  void onMemberLeft(IChannelMember *member) override;

  void onGetMembers(IChannelMember **members, int userCount, GET_MEMBERS_ERR errorCode) override;

private:
  inline void postMsg(UINT msg, WPARAM wParam = NULL, LPARAM lParam = NULL);

private:
  HWND m_MsgWnd;
  std::string m_channeId;
};


#endif