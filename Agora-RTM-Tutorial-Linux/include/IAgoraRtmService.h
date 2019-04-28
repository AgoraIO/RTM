//
//  Agora's RTM SDK
//
//
//  Copyright (c) 2019 Agora.io. All rights reserved.
//
#pragma once

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define AGORA_CALL __cdecl
#if defined(AGORARTC_EXPORT)
#define AGORA_API extern "C" __declspec(dllexport)
#else
#define AGORA_API extern "C" __declspec(dllimport)
#endif
#define _AGORA_CPP_API

#elif defined(__APPLE__)
#define AGORA_API __attribute__((visibility("default"))) extern "C"
#define AGORA_CALL
#define _AGORA_CPP_API

#elif defined(__ANDROID__) || defined(__linux__)
#if defined(__ANDROID__) && defined(FEATURE_RTM_STANDALONE_SDK)
#define AGORA_API extern "C"
#define _AGORA_CPP_API
#else
#define AGORA_API extern "C" __attribute__((visibility("default")))
#define _AGORA_CPP_API __attribute__((visibility("default")))
#endif
#define AGORA_CALL

#else
#define AGORA_API extern "C"
#define AGORA_CALL
#define _AGORA_CPP_API
#endif

namespace agora {
  namespace rtm {

    /** Login states.
    */
    enum LOGIN_ERR_CODE {
      /** Login succeeds. No error occurs. */
      LOGIN_ERR_OK = 0,
      /** Login fails. The reason is unknown.*/
      LOGIN_ERR_UNKNOWN = 1,
      /** Login is rejected. the SDK is not initialized or is rejected by the server. */
      LOGIN_ERR_REJECTED = 2, // Occurs when not initialized or server reject
      /** Invalid login argument.*/
      LOGIN_ERR_INVALID_ARGUMENT = 3,
      /** Invalid App ID. */
      LOGIN_ERR_INVALID_APP_ID = 4,
      /** Invalid token. */
      LOGIN_ERR_INVALID_TOKEN = 5,
      /** The token has expired and login is rejected.*/
      LOGIN_ERR_TOKEN_EXPIRED = 6,
      /** Unauthorized login. */
      LOGIN_ERR_NOT_AUTHORIZED = 7,
      /** The user is already logged-in or logging-in. */
      LOGIN_ERR_ALREADY_LOGIN = 8,
      /** A login timeout. */
      LOGIN_ERR_TIMEOUT = 9,
      LOGIN_ERR_TOO_OFTEN = 10,
    };
    /** Logout states.
    */
    enum LOGOUT_ERR_CODE {
      /** Logout succeeds. No error occurs. */
      LOGOUT_ERR_OK = 0,
      /** Logout fails. Maybe the SDK is not initialized or the user is not logged in. */
      LOGOUT_ERR_REJECTED = 1,
    };

    enum RENEW_TOKEN_ERR_CODE {
      RENEW_TOKEN_ERR_OK = 0,
      RENEW_TOKEN_ERR_FAILURE = 1,
      RENEW_TOKEN_ERR_INVALID_ARGUMENT = 2,
      RENEW_TOKEN_ERR_REJECTED = 3,
      RENEW_TOKEN_ERR_TOO_OFTEN = 4,
      RENEW_TOKEN_ERR_TOKEN_EXPIRED = 5,
      RENEW_TOKEN_ERR_INVALID_TOKEN = 6,
    };
    /** Connection states.
    */
    enum CONNECTION_STATE {
      /** The SDK is logged out of Agora's RTM service and is trying to log in.

      If the connection to Agora's RTM service is lost because, for example, if the network is down or switched, the SDK automatically tries to log in, triggers the \ref agora::rtm::IRtmServiceEventHandler::onConnectionStateChanged "onConnectionStateChanged" callback, and switches to the \ref agora::rtm::CONNECTION_STATE_RECONNECTING "CONNECTION_STATE_RECONNECTING" state.
*/
      CONNECTION_STATE_DISCONNECTED = 1,
       /** The SDK is logging in Agora's RTM service.

       When the application calls the \ref agora::rtm::IRtmService::login "login" method, the SDK starts to log in Agora's RTM service, triggers the \ref agora::rtm::IRtmServiceEventHandler::onConnectionStateChanged "onConnectionStateChanged" callback, and switches to the \ref agora::rtm::CONNECTION_STATE_CONNECTING "CONNECTION_STATE_CONNECTING" state.
       When the SDK successfully logs in Agora's RTM service, it triggers the \ref agora::rtm::IRtmServiceEventHandler::onConnectionStateChanged "onConnectionStateChanged" callback and switches to the \ref agora::rtm::CONNECTION_STATE_CONNECTED "CONNECTION_STATE_CONNECTED" state.

       After the SDK logs in Agora's RTM service and when it finishes initializing the media engine, the SDK triggers the \ref agora::rtm::IRtmServiceEventHandler::onLoginSuccess "onLoginSuccess" callback.
       */
      CONNECTION_STATE_CONNECTING = 2,
      /** The SDK is connected to Agora's RTM system.

       - When the app calls the \ref agora::rtm::IRtmService::login "login" method, the SDK starts to log in Agora's RTM service, triggers the \ref agora::rtm::IRtmServiceEventHandler::onConnectionStateChanged "onConnectionStateChanged" callback, and switches to the \ref agora::rtm::CONNECTION_STATE_CONNECTING "CONNECTION_STATE_CONNECTING" state.
       - When the SDK successfully logs in Agora's RTM service, it triggers the \ref agora::rtm::IRtmServiceEventHandler::onConnectionStateChanged "onConnectionStateChanged" callback and switches to the \ref agora::rtm::CONNECTION_STATE_CONNECTED "CONNECTION_STATE_CONNECTED" state.
       - After the SDK logs in Agora's RTM service and when it finishes initializing the media engine, the SDK triggers the \ref agora::rtm::IChannelEventHandler::onMemberJoined "onMemberJoined" callback.
       */
      CONNECTION_STATE_CONNECTED = 3,
      /** The SDK keeps logging in Agora's RTM service after being logged out because of network issues.

       - If the SDK cannot log in Agora's RTM service within 10 seconds after being logged out, the SDK stays in the \ref agora::rtm::CONNECTION_STATE_RECONNECTING "CONNECTION_STATE_RECONNECTING" state and keeps logging in.
       - If the SDK fails to log in Agora's RTM service 20 minutes after being logged out, the SDK triggers the \ref agora::rtm::IRtmServiceEventHandler::onConnectionStateChanged "onConnectionStateChanged" callback, switches to the \ref agora::rtm::CONNECTION_STATE_FAILED "CONNECTION_STATE_FAILED" state, and stops logging in.
       */
      CONNECTION_STATE_RECONNECTING = 4,
      /** The SDK has given up logging in Agora's RTM service, mainly because the SDK is banned by the service or because the SDK has logged in with the same user ID from a different instance or device. To log in again, call the \ref agora::rtm::IRtmService::logout "logout" method before calling the \ref agora::rtm::IRtmService::login "login" method again.
      */
      CONNECTION_STATE_ABORTED = 5,
    };

    /** Reasons for a connection state change.
    */
    enum CONNECTION_CHANGE_REASON {
      /** The SDK is logging in Agora's RTM service.
      */
      CONNECTION_CHANGE_REASON_LOGIN = 1,
      /** The SDK has logged in Agora's RTM service.
      */
      CONNECTION_CHANGE_REASON_LOGIN_SUCCESS = 2,
      /** The SDK fails to log in Agora's RTM service.
      */
      CONNECTION_CHANGE_REASON_LOGIN_FAILURE = 3,
      /** The SDK fails to log in Agora's RTM service for more than 20 minutes and stops logging in.
      */
      CONNECTION_CHANGE_REASON_LOGIN_TIMEOUT = 4,
      /** The SDK login to Agora's RTM service is interrupted.
      */
      CONNECTION_CHANGE_REASON_INTERRUPTED = 5,
      /** The SDK has logged out of Agora's RTM service.
      */
      CONNECTION_CHANGE_REASON_LOGOUT = 6,
      /**  The SDK login to Agora's RTM service is banned by Agora.
      */
      CONNECTION_CHANGE_REASON_BANNED_BY_SERVER = 7,

      CONNECTION_CHANGE_REASON_REMOTE_LOGIN = 8,
    };

    /** States of sending a peer-to-peer message.
    */
    enum PEER_MESSAGE_ERR_CODE {
      /** The receiver receives the peer-to-peer message.
       */
      PEER_MESSAGE_ERR_OK = 0,
      /** The sender fails to send the peer-to-peer message.
      */
      PEER_MESSAGE_ERR_FAILURE = 1,
      /** A timeout in sending the peer-to-peer message. The current timeout is set as 5 seconds.
      */
      PEER_MESSAGE_ERR_SENT_TIMEOUT = 2,
      /** The receiver is offline and does not receive the peer-to-peer message.
       */
      PEER_MESSAGE_ERR_PEER_UNREACHABLE = 3,
    };

    /** States of a user joining a channel.
    */
    enum JOIN_CHANNEL_ERR {
      /** The user joins the channel successfully.
      */
      JOIN_CHANNEL_ERR_OK = 0,
      /** The user fails to join the channel.
      */
      JOIN_CHANNEL_ERR_FAILURE = 1,
      /** The user cannot join the channel. The user may already be in the channel.
      */
      JOIN_CHANNEL_ERR_REJECTED = 2, // Usually occurs when the user is already in the channel
      /** The user fails to join the channel. Maybe the argument is invalid.
      */
      JOIN_CHANNEL_ERR_INVALID_ARGUMENT = 3,
      /** A timeout in joining the channel.
      */
      JOIN_CHANNEL_TIMEOUT = 4,
    };

    /** States of a user leaving a channel.
    */
    enum LEAVE_CHANNEL_ERR {
      /** The user leaves the channel successfully.
      */
      LEAVE_CHANNEL_ERR_OK = 0,
      /** The user fails to leave the channel.
      */
      LEAVE_CHANNEL_ERR_FAILURE = 1,
      /** The user cannot leave the channel. The user may not be in the channel.
      */
      LEAVE_CHANNEL_ERR_REJECTED = 2, // Usually occurs when the user is not in the channel
    };

    /** Reasons for a user leaving a channel.
    */
    enum LEAVE_CHANNEL_REASON {
      /** The user has quit the call.
      */
      LEAVE_CHANNEL_REASON_QUIT = 1,
      /** The user is banned by the server.
      */
      LEAVE_CHANNEL_REASON_KICKED = 2,
    };

    /** States of sending a channel message.
    */
    enum CHANNEL_MESSAGE_ERR_CODE {
      /** The sender sends the channel message successfully.
      */
      CHANNEL_MESSAGE_ERR_OK = 0,
      /** The sender fails to send the channel message.
      */
      CHANNEL_MESSAGE_ERR_FAILURE = 1,
      /** A timeout in sending the channel message.
      */
      CHANNEL_MESSAGE_ERR_SENT_TIMEOUT = 2,
    };

    /** States of retrieving the user list of a channel.
    */
    enum GET_MEMBERS_ERR {
      /** Retrieves the user list of a channel.
      */
      GET_MEMBERS_ERR_OK = 0,
      /** Fails to retrieve the user list of a channel.
      */
      GET_MEMBERS_ERR_FAILURE = 1,
      /** Cannot retrieve the user list of a channel.
      */
      GET_MEMBERS_ERR_REJECTED = 2,
      /** A timeout in retreiving the user list of a channel.
      */
      GET_MEMBERS_ERR_TIMEOUT = 3,
    };

    enum QUERY_PEERS_ONLINE_STATUS_ERR {
      QUERY_PEERS_ONLINE_STATUS_ERR_OK = 0,
      QUERY_PEERS_ONLINE_STATUS_ERR_FAILURE = 1,
      QUERY_PEERS_ONLINE_STATUS_ERR_INVALID_ARGUMENT = 2,
      QUERY_PEERS_ONLINE_STATUS_ERR_REJECTED = 3,
      QUERY_PEERS_ONLINE_STATUS_ERR_TIMEOUT = 4,
      QUERY_PEERS_ONLINE_STATUS_ERR_TOO_OFTEN = 5,
    };

    /** Message types.
    */
    enum MESSAGE_TYPE {
      /** The message type is undefined.
      */
      MESSAGE_TYPE_UNDEFINED = 0,
      /** A text message.
      */
      MESSAGE_TYPE_TEXT = 1,
    };

    /** Channel and peer-to-peer message methods.
    */
    class IMessage
    {
    protected:
      virtual ~IMessage() {}
    public:
      /** Retrieves the channel or peer-to-peer message ID.
      *
      * @return The message ID, which is created within the message.
      */
      virtual long long getMessageId() const = 0;
      /** Retrieves the channel or peer-to-peer message type.

      @return The message type.
      */
      virtual MESSAGE_TYPE getMessageType() const = 0;
      /** Sets the channel or peer-to-peer text message.
      *
      * @param str The text message to be set.
      */
      virtual void setText(const char *str) = 0;
      /** Retrieves the channel or peer-to-peer text message.
      *
      * @return The received text message.
      */
      virtual const char *getText() const = 0;
      /** Releases all resources used by the IMessage instance.
      */
      virtual void release() = 0;
    };

    /** Agora RTM channel member methods.
    */
    class IChannelMember
    {
    protected:
      virtual ~IChannelMember() {}
    public:
      /** Gets the user ID of a user in the channel.
      *
      * @return User ID of a user in the channel.
      */
      virtual const char * getUserId() const = 0;
      /** Gets the channel ID of the user.
      *
      * @return Channel ID of the user.
      */
      virtual const char * getChannelId() const = 0;
      /** Releases all resources used by the IChannelMember instance.
      */
      virtual void release() = 0;
    };

    struct PeerOnlineStatus
    {
      const char* peerId;
      bool isOnline;
    };

//        class IChannelAttributes
//        {
//        public:
//            static IChannelAttributes *createChannelAttributes();
//
//            virtual int addAttribute(const char *key, const char *value) = 0;
//            virtual int removeAttribute(const char *key) = 0;
//            virtual int getAttributesSize() const = 0;
//            virtual void getAttributes(int size, char **key, char **value) const = 0;   // todo: discussion, how to traveral
//            virtual const char * getAttributeValue(const char *key) const = 0;
//
//            virtual void release() = 0;
//        };
//
    /** Callbacks for the Agora RTM channel methods.
    */
    class IChannelEventHandler
    {
    public:

      virtual ~IChannelEventHandler()
      {
      }
      /** Occurs when a user joins a channel.

      The local user receives this callback while all remote users receive the \ref agora::rtm::IChannelEventHandler::onMemberJoined "onMemberJoined" callback when the \ref agora::rtm::IChannel::join "join" method call succeeds.
      */
      virtual void onJoinSuccess()
      {
      }
      /** Occurs when a user fails to join a channel.

      The local user receives this callback when the \ref agora::rtm::IChannel::join "join" method call fails. See \ref agora::rtm::JOIN_CHANNEL_ERR "JOIN_CHANNEL_ERR" for the error codes.
      */
      virtual void onJoinFailure(JOIN_CHANNEL_ERR errorCode)
      {
          (JOIN_CHANNEL_ERR) errorCode;
      }
      /** Occurs when a user leaves a channel.

      The SDK triggers this callback when the \ref agora::rtm::IChannel::leave "leave" method call succeeds. See \ref agora::rtm::LEAVE_CHANNEL_ERR "LEAVE_CHANNEL_ERR" for the states.
      */
      virtual void onLeave(LEAVE_CHANNEL_ERR errorCode)
      {
          (LEAVE_CHANNEL_ERR) errorCode;
      }

      /** Occurs when all remote users receive a channel message from a sender.

      All remote users receive this callback while the sender receives the \ref agora::rtm::IChannelEventHandler::onSendMessageResult "onSendMessageResult" callback when the \ref agora::rtm::IChannel::sendMessage "sendMessage" method call succeeds.

      @param userId User ID of the sender.
      @param message The received channel message. See IMessage.

      */
      virtual void onMessageReceived(const char *userId, const IMessage *message)
      {
          (const char *) userId;
          (IMessage *) message;
      }

      /** Occurs when the sent channel message state changes.

      The sender receives this callback while all remote users receive the \ref agora::rtm::IChannelEventHandler::onMessageReceived "onMessageReceived" callback when the \ref agora::rtm::IChannel::sendMessage "sendMessage" method call succeeds.

      @param messageID The ID of the sent channel message.
      @param state The new channel message state. See \ref agora::rtm::CHANNEL_MESSAGE_ERR_CODE "CHANNEL_MESSAGE_ERR_CODE".

      */
      virtual void onSendMessageResult(long long messageId, CHANNEL_MESSAGE_ERR_CODE state)
      {
          (long long) messageId;
          (CHANNEL_MESSAGE_ERR_CODE) state;
      }
      /** Occurs when a user joins a channel.

      All remote users receive this callback while the local user receives the \ref agora::rtm::IChannelEventHandler::onJoinSuccess "onJoinSuccess" callback when the \ref agora::rtm::IChannel::join "join" method call succeeds.

      @param member The user joining the channel. See IChannelMember.
      */
      virtual void onMemberJoined(IChannelMember *member)
      {
          (IChannelMember *) member;
      }
      /** Occurs when a user leaves a channel.

      All remote users receive this callback while the local user receives the the \ref agora::rtm::IChannelEventHandler::onLeave "onLeave" callback when the \ref agora::rtm::IChannel::leave "leave" method call succeeds.

      @param member The user leaving the channel. See IChannelMember.
      */
      virtual void onMemberLeft(IChannelMember *member)
      {
          (IChannelMember *) member;
      }
      /** Occurs when the user list of a channel is retrieved.

      The SDK triggers this callback when the \ref agora::rtm::IChannel::getMembers "getMembers" method call succeeds.

      @param members The user list. See IChannelMember.
      @param userCount The number of users.
      @param errorCode Error code. See \ref agora::rtm::GET_MEMBERS_ERR "GET_MEMBERS_ERR".

      */
      virtual void onGetMembers(IChannelMember **members, int userCount, GET_MEMBERS_ERR errorCode)
      {
          (IChannelMember **) members;
          (int) userCount;
      }

//            virtual void onAttributesUpdated(const IChannelAttributes *attributes)
//            {
//                (IChannelAttributes *) attributes;
//            }
//
//            virtual void onUpdateAttributesResponse(long long requestId, RESPONSE_CODE resCode)
//            {
//                (long long) requestId;
//                (RESPONSE_CODE) resCode;
//            }
//
//
//            virtual void onAttributesDeleted(const IChannelAttributes *attributes)
//            {
//                (IChannelAttributes *) attributes;
//            }
//
//            virtual void onDeleteAttributesResponse(long long requestId, RESPONSE_CODE resCode)
//            {
//                (long long) requestId;
//                (RESPONSE_CODE) resCode;
//            }
    };
    /** Agora RTM channel methods.
    */
    class IChannel
    {
    protected:
      virtual ~IChannel() {}
    public:
//            virtual void setEventHandler(IChannelEventHandler *eventHandler) = 0;
      /** Allows a user to join a channel.

      - If this method call succeeds:
          - The local user receives the \ref agora::rtm::IChannelEventHandler::onJoinSuccess "onJoinSuccess" callback.
          - All remote users receive the \ref agora::rtm::IChannelEventHandler::onMemberJoined "onMemberJoined" callback.
      - If this method call fails, the local user receives the \ref agora::rtm::IChannelEventHandler::onJoinFailure "onJoinFailure" callback. See \ref agora::rtm::JOIN_CHANNEL_ERR "JOIN_CHANNEL_ERR"
      for the error codes.
      */
      virtual int join() = 0;
      /** Allows a user to leave a channel.

      - If this method call succeeds:
          - The local user receives the \ref agora::rtm::IChannelEventHandler::onLeave "onLeave" callback with the LEAVE_CHANNEL_ERR_OK state.
          - All remote users receive the \ref agora::rtm::IChannelEventHandler::onMemberLeft "onMemberLeft" callback.
      - If this method call fails, the local user receives the \ref agora::rtm::IChannelEventHandler::onLeave "onLeave" callback with an error code. See \ref agora::rtm::LEAVE_CHANNEL_ERR "LEAVE_CHANNEL_ERR" for the error codes.

      */
      virtual int leave() = 0;
      /** Allows a local user to send a message in a channel.

      If this method call succeeds:
      - The local user receives the \ref agora::rtm::IChannelEventHandler::onSendMessageResult "onSendMessageResult" callback.
      - All remote users receive the \ref agora::rtm::IChannelEventHandler::onMessageReceived	"onMessageReceived" callback.

      @param message The message to be sent. See IMessage.
      */
      virtual int sendMessage(const IMessage *message) = 0;
//            virtual int updateAttributes(IChannelAttributes *attributes, long long &requestId) = 0;  // move request id to attribute
//            virtual int deleteAttributes(IChannelAttributes *attributes, long long &requestId) = 0;
      /** Gets the channel ID.
      */
      virtual const char *getId() const = 0;
      /** Retrieves the user list of a channel.

      If this method call succeeds, the SDK triggers the \ref agora::rtm::IChannelEventHandler::onGetMembers "onGetMembers" callback.
      */
      virtual int getMembers() = 0;

      // sync_call
      /** Releases all resources used by the IChannel instance.
      */
      virtual void release() = 0;
    };

    class IRtmServiceEventHandler
    {
    public:
      virtual ~IRtmServiceEventHandler()
      {
      }

      /** Occurs when a user logs in Agora's RTM service.

      The local user receives this callback when the \ref agora::rtm::IRtmService::login "login" method call succeeds.
      */
      virtual void onLoginSuccess() {}
      /** Occurs when a user fails to log in Agora's RTM service.

      The local user receives this callback when the \ref agora::rtm::IRtmService::login "login" method call fails. See \ref agora::rtm::LOGIN_ERR_CODE "LOGIN_ERR_CODE" for the error codes.
      */
      virtual void onLoginFailure(LOGIN_ERR_CODE errorCode)
      {
          (LOGIN_ERR_CODE) errorCode;
      }

      virtual void onRenewTokenResult(const char* token, RENEW_TOKEN_ERR_CODE errorCode)
      {
          (const char*) token;
          (RENEW_TOKEN_ERR_CODE) errorCode;
      }

      virtual void onTokenExpired()
      {
      }

      /** Occurs when a user logs out of Agora's RTM service.

      The local user receives this callback when the SDK calls the \ref agora::rtm::IRtmService::logout "logout" method. See \ref agora::rtm::LOGOUT_ERR_CODE "LOGOUT_ERR_CODE" for the states.

      */
      virtual void onLogout(LOGOUT_ERR_CODE errorCode)
      {
          (LOGOUT_ERR_CODE) errorCode;
      }
      /** Occurs when a connection state changes between the SDK and Agora's RTM service.

       @param state The new connection state. See \ref agora::rtm::CONNECTION_STATE "CONNECTION_STATE".
       @param reason The reason for the connection state change. See \ref agora::rtm::CONNECTION_CHANGE_REASON "CONNECTION_CHANGE_REASON".
       */
      virtual void onConnectionStateChanged(CONNECTION_STATE state, CONNECTION_CHANGE_REASON reason)
      {
      }
      /** Occurs when the sent peer-to-peer message state changes.

      The sender receives this callback while the receiver receives the \ref agora::rtm::IRtmServiceEventHandler::onMessageReceivedFromPeer "onMessageReceivedFromPeer" callback when the \ref agora::rtm::IRtmService::sendMessageToPeer "sendMessageToPeer" method call succeeds.

      @param messageID The ID of the sent message.
      @param state The new peer-to-peer message state. See \ref agora::rtm::PEER_MESSAGE_ERR_CODE "PEER_MESSAGE_ERR_CODE".

      */
      virtual void onSendMessageResult(long long messageId, PEER_MESSAGE_ERR_CODE errorCode)
      {
          (long long) messageId;
          (PEER_MESSAGE_ERR_CODE) errorCode;
      }
      /** Occurs when the receiver receives the peer-to-peer message from the sender.

      The receiver receives this callback while the sender receives the \ref agora::rtm::IRtmServiceEventHandler::onSendMessageResult "onSendMessageResult" callback when the \ref agora::rtm::IRtmService::sendMessageToPeer "sendMessageToPeer" method call succeeds.

      @param peerID User ID of the sender.
      @param message The message sent by the sender. See IMessage.

      */
      virtual void onMessageReceivedFromPeer(const char *peerId, const IMessage *message)
      {
          (char *) peerId;
          (IMessage *) message;
      }

     virtual void onQueryPeersOnlineStatusResult(long long requestId, const PeerOnlineStatus* peersStatus , int peerCount, QUERY_PEERS_ONLINE_STATUS_ERR errorCode)
      {
          (long long) requestId; 
          (const PeerOnlineStatus*) peersStatus;
          (int) peerCount;
          (QUERY_PEERS_ONLINE_STATUS_ERR) errorCode;
      }
    };


    class IRtmCallManager;
    class IRtmCallEventHandler;

    class IRtmService
    {
    protected:
      virtual ~IRtmService() {}
    public:
      /** Creates an IRtmService instance.

      The Agora RTM SDK supports multiple {@link RtmClient} instances.

      All methods in the {@link RtmClient} class are executed asynchronously.

      @param appId The App ID issued by Agora to you. Apply for a new App ID from Agora if it is missing from your kit.
      @param eventHandler An IRtmServiceEventHandler object that invokes callbacks to be passed to the application on Agora RTM SDK runtime events.
      */
      virtual int initialize(const char *appId, IRtmServiceEventHandler *eventHandler) = 0;

      virtual void addEventHandler(IRtmServiceEventHandler *eventHandler) = 0;
      virtual void removeEventHandler(IRtmServiceEventHandler *eventHandler) = 0;

      /** Releases all resources used by the IRtmService instance.
      */
      virtual void release(bool sync = false) = 0;

      /** Allows a user to log in Agora's RTM system.

      - If this method call succeeds, the local user receives the \ref agora::rtm::IRtmServiceEventHandler::onLoginSuccess "onLoginSuccess" callback.
      - If this method call fails, the local user receives the \ref agora::rtm::IRtmServiceEventHandler::onLoginFailure "onLoginFailure" callback. See \ref agora::rtm::LOGIN_ERR_CODE "LOGIN_ERR_CODE" for the error codes.

      @param token Token used to log in Agora's RTM system and used when dynamic authentication is enabled. Set @token as @p nil in the integration and test stages.
      @param userId User ID of the user logging in Agora's RTM system. The string must not exceed 64 bytes in length or start with a space.
      Supported characters:

      - The 26 lowercase English letters: a to z
      - The 26 uppercase English letters: A to Z
      - The 10 numbers: 0 to 9
      - Space
      - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ","

      @note Do not set @p userId as @p null.
      */
      virtual int login(const char *token, const char *userId) = 0;

      /** Allows a user to log out of Agora's RTM system.

      The local user receives the \ref agora::rtm::IRtmServiceEventHandler::onLogout "onLogout" callback. See \ref agora::rtm::LOGOUT_ERR_CODE "LOGIN_ERR_CODE" for the states.
      */
      virtual int logout() = 0;

      virtual int renewToken(const char* token) = 0;

      /** Allows a local user (sender) to send a peer-to-peer message to a specific user (receiver).

      If this method call succeeds:
      - The sender receives the \ref agora::rtm::IRtmServiceEventHandler::onSendMessageResult "onSendMessageResult" callback.
      - The receiver receives the \ref agora::rtm::IRtmServiceEventHandler::onMessageReceivedFromPeer "onMessageReceivedFromPeer" callback.

       @param peerId User ID of the receiver.
       @param message The message to be sent. For information about creating a message, see IMessage.
       */
      virtual int sendMessageToPeer(const char *peerId, const IMessage *message) = 0;

      /** Creates an Agora RTM channel.

      - If the method call succeeds, the SDK returns an IChannel instance.
      - If this method call fails, the SDK returns @p null. The @p channelId may be invalid, or an existing channel with the same @p channelId exists, or the number of channels is over the limit.

      @note You can create multiple channels in an IRtmService instance.

      @param channelId The unique channel name for the Agora RTM session in the string format smaller than 64 bytes and cannot be empty or set as @p null.
      Supported characters:
      - The 26 lowercase English letters: a to z
      - The 26 uppercase English letters: A to Z
      - The 10 numbers: 0 to 9
      - Space
      - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ","

      @param eventHandler See IChannelEventHandler.
      */
      virtual IChannel * createChannel(const char *channelId, IChannelEventHandler *eventHandler) = 0;

      virtual IRtmCallManager* getRtmCallManager(IRtmCallEventHandler*eventHandler) = 0;
      /** Creates an IMessage instance.
      *
      * @note The IMessage instance can be either for a channel or peer-to-peer message.
      *
      * @return An IMessage instance.
      */
      virtual IMessage *createMessage() = 0;
      // todo: move to private
      virtual int setParameters(const char* parameters) = 0;
//            virtual int getParameters(const char* key, any_document_t& result) = 0;
      virtual int queryPeersOnlineStatus(const char* peerIds[], int peerCount, long long& requestId) = 0;
    };

  }
}
