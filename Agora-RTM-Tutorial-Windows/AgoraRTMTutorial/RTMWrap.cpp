#include "stdafx.h"
#include "commonFun.h"
#include "RTMWrap.h"
#include "AgoraRTMInstance.h"

CRTMCallBack::CRTMCallBack(HWND wnd) :m_MsgWnd(wnd)
{
}

CRTMCallBack::~CRTMCallBack()
{
  m_MsgWnd = nullptr;
}

void CRTMCallBack::setCallBackWnd(HWND wnd)
{
  m_MsgWnd = wnd;
}

// Inherited from |agora::rtm::IRtmServiceEventHandler|
void CRTMCallBack::onLoginSuccess() {
  PAG_SIGNAL_LOGINSUCCESS lpData = new AG_SIGNAL_LOGINSUCCESS;
  lpData->fd = 0;
  lpData->uid = 0;

  postMsg(WM_LoginSuccess, WPARAM(lpData));
}

void CRTMCallBack::onLoginFailure(LOGIN_ERR_CODE errorCode) {
  PAG_SIGNAL_LOGINFAILED lpData = new AG_SIGNAL_LOGINFAILED;
  lpData->ecode = errorCode;

  postMsg(WM_LoginFailed, WPARAM(lpData), LPARAM(errorCode));
}

void CRTMCallBack::onLogout(LOGOUT_ERR_CODE errorCode) {
  PAG_SIGNAL_LOGOUT lpData = new AG_SIGNAL_LOGOUT;
  lpData->ecode = errorCode;

  postMsg(WM_LogOut, WPARAM(lpData), LPARAM(errorCode));
}

void CRTMCallBack::onConnectionStateChanged(CONNECTION_STATE state, CONNECTION_CHANGE_REASON reason) {
}

void CRTMCallBack::onSendMessageResult(long long messageId, PEER_MESSAGE_ERR_CODE errorCode) {
}

void CRTMCallBack::onMessageReceivedFromPeer(const char *peerId, const IMessage *message) {
  PAG_SIGNAL_MESSAGEINSTANCERECEIVE lpData = new AG_SIGNAL_MESSAGEINSTANCERECEIVE;
  lpData->account = utf82gbk(peerId);
  lpData->uid = 0;
  lpData->msg = utf82gbk(message->getText());

  postMsg(WM_MessageInstantReceive, WPARAM(lpData));
}

// Inherited from |agora::rtm::IChannelEventHandler|
void CRTMCallBack::onJoinSuccess() {
  PAG_SIGNAL_CHANNELJOIN lpData = new AG_SIGNAL_CHANNELJOIN;
  lpData->channelID = utf82gbk(m_channeId.c_str());

  postMsg(WM_ChannelJoined, WPARAM(lpData));
}

void CRTMCallBack::onJoinFailure(JOIN_CHANNEL_ERR errorCode) {
  PAG_SIGNAL_CHANNELJOINFAILED lpData = new AG_SIGNAL_CHANNELJOINFAILED;
  lpData->channelID = utf82gbk(m_channeId);
  lpData->ecode = errorCode;

  postMsg(WM_ChannelJoinedFailed, WPARAM(lpData));
}

void CRTMCallBack::onLeave(LEAVE_CHANNEL_ERR errorCode) {
  PAG_SIGNAL_CHANNELLEAVE lpData = new AG_SIGNAL_CHANNELLEAVE;
  lpData->channelID = utf82gbk(m_channeId);
  lpData->ecode = errorCode;

  postMsg(WM_ChannelLeaved, WPARAM(lpData));
}

void CRTMCallBack::onMessageReceived(const char *userId, const IMessage *message) {
  PAG_SIGNAL_MESSAGECHANNELRECEIVE lpData = new AG_SIGNAL_MESSAGECHANNELRECEIVE;
  lpData->account = utf82gbk(userId);
  lpData->channelID = utf82gbk(m_channeId);
  lpData->uid = 0;
  lpData->msg = utf82gbk(message->getText());

  postMsg(WM_MessageChannelReceive, WPARAM(lpData));
}

void CRTMCallBack::onSendMessageResult(long long messageId, CHANNEL_MESSAGE_ERR_CODE state) {
  PAG_SIGNAL_MESSAGESENDSUCCESS  lpData = new AG_SIGNAL_MESSAGESENDSUCCESS;
  lpData->messageID = utf82gbk(std::to_string(messageId));

  postMsg(WM_MessageSendSuccess, WPARAM(lpData));;
}

void CRTMCallBack::onMemberJoined(IChannelMember *member) {
}

void CRTMCallBack::onMemberLeft(IChannelMember *member) {
}

void CRTMCallBack::onGetMembers(IChannelMember **members, int userCount, GET_MEMBERS_ERR errorCode) {
}

void CRTMCallBack::postMsg(UINT msg, WPARAM wParam /*= NULL*/, LPARAM lParam /*= NULL*/)
{
  ::PostMessage(m_MsgWnd, msg, wParam, lParam);
}
