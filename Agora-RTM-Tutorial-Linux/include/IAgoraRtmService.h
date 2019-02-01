//
//  Agora Rtm SDK
//
//  Created by junhao Wang in 2018-05.
//  Copyright (c) 2018 Agora IO. All rights reserved.
//
#pragma once

namespace agora {
    namespace rtm {
        /** Login status.
        */
        enum LOGIN_ERR_CODE {
            /** Login succeeds. No error occurs. */
            LOGIN_ERR_OK = 0,
            /** Login fails. The reason is unknown.*/
            LOGIN_ERR_UNKNOWN = 1,
            /** Login is rejected. the SDK is not initialized or is rejected by the server. */
            LOGIN_ERR_REJECTED = 2, // Occurs when not initialized or server reject
            /** Invalid login arguments.*/
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
        /** Logout status.
        */
        enum LOGOUT_ERR_CODE {
            /** Logout succeeds. No error occurs. */
            LOGOUT_ERR_OK = 0,
            /** Logout fails. Maybe the SDK is not initialized or the user is not logged in. */
            LOGOUT_ERR_REJECTED = 1,
        };
        /** Connection states.
        */
        enum CONNECTION_STATE {
            CONNECTION_STATE_DISCONNECTED = 1,
            CONNECTION_STATE_CONNECTING = 2,
            /** The SDK is connected to the server.
            */
            CONNECTION_STATE_CONNECTED = 3,
            /** The SDK is disconnected from the server and is trying to reconnect.
            */
            CONNECTION_STATE_RECONNECTING = 4,
            /** The SDK has given up connecting to the server, mainly because a user is banned by the server or because a user has logged in with the same user ID from a different instance or device. In order to reconnect, call the logout method before calling the login method again.
            */
            CONNECTION_STATE_ABORTED = 5,
        };
        
        /** Reasons for the connection state change.
        */
        enum CONNECTION_CHANGE_REASON {
            CONNECTION_CHANGE_REASON_LOGIN = 1,
            CONNECTION_CHANGE_REASON_LOGIN_SUCCESS = 2,
            CONNECTION_CHANGE_REASON_LOGIN_FAILURE = 3,
            CONNECTION_CHANGE_REASON_LOGIN_TIMEOUT = 4,
            CONNECTION_CHANGE_REASON_INTERRUPTED = 5,
            CONNECTION_CHANGE_REASON_LOGOUT = 6,
            CONNECTION_CHANGE_REASON_BANNED_BY_SERVER = 7,
        };
        
        /** The state of sending a point-to-point (P2P) message.
        */ 
        enum PEER_MESSAGE_STATE {
            /** The initial state. The P2P message is not sent. 
            */  
            PEER_MESSAGE_INIT = 0,
            /** Fails to send the P2P message. 
            */
            PEER_MESSAGE_FAILURE = 1,
            /** The P2P message is not received because the specified user (peer) is offline. 
            */
            PEER_MESSAGE_PEER_UNREACHABLE = 2,
            /** The P2P message is received by the specified user (peer).
            */
            PEER_MESSAGE_RECEIVED_BY_PEER = 3,
            /** A timeout in sending the P2P message. The current timeout is set as 5 seconds. 
            */
            PEER_MESSAGE_SENT_TIMEOUT = 4,
        };

#pragma mark channel
        /** The state of a user joining a channel.
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

        /** The state of a user leaving a channel.
        */
        enum LEAVE_CHANNEL_ERR {
            /** The user leaves the channel successfully.
            */
            LEAVE_CHANNEL_ERR_OK = 0,
            /** The user fails to leaves the channel.
            */
            LEAVE_CHANNEL_ERR_FAILURE = 1,
            /** The user cannot leave the channel. The user may not be in the channel.
            */
            LEAVE_CHANNEL_ERR_REJECTED = 2, // Usually occurs when the user is not in the channel
        };

        /** The reason of a user leaving a channel.
        */
        enum LEAVE_CHANNEL_REASON {
            /** The user has quit the call.
            */
            LEAVE_CHANNEL_REASON_QUIT = 1,
            /** The user is banned by the server.
            */
            LEAVE_CHANNEL_REASON_KICKED = 2,
        };

        /** The state of sending a channel message.
        */
        enum CHANNEL_MESSAGE_STATE {
            /** The initial state. The channel message is not sent. 
            */
            CHANNEL_MESSAGE_INIT = 0,
            /** Fails to send the channel message. 
            */
            CHANNEL_MESSAGE_FAILURE = 1,
            /** The channel message is received by the server. 
            */
            CHANNEL_MESSAGE_RECEIVED_BY_SERVER = 2,
            // 3, absent
            /** A timeout in sending the channel message. 
            */
            CHANNEL_MESSAGE_SENT_TIMEOUT = 4,
        };

        /** The state of retrieving the user list of a channel.
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

        enum MESSAGE_TYPE {
            MESSAGE_TYPE_UNDEFINED = 0,
            MESSAGE_TYPE_TEXT = 1,
        };

        class IMessage
        {
        public:
            /** Creates an RtmMessage instance.
            *
            * @note The RtmMessage instance can be either for a P2P or channel message.
            * 
            * @return The RtmMessage instance.
            */
            static IMessage *createMessage();
            virtual ~IMessage() {}
            /** Retrieves the message ID.
            * 
            * @return The message ID, which is created within the message.
            */
            virtual long long getMessageId() const = 0;
            virtual int getMessageType() const = 0;
            /** Sets the text message.
            * 
            * @param str Sent text message. 
            */
            virtual void setText(const char *str) = 0;
            /** Retrieves the text message.
            * 
            * @return Received text message.
            */
            virtual const char *getText() const = 0;

            virtual void release() = 0;
        };

#pragma mark channel
        class IChannelMember
        {
        public:
            virtual ~IChannelMember() {}
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

            virtual void release() = 0;
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
        class IChannelEventHandler
        {
        public:

            virtual ~IChannelEventHandler()
            {
            }

            virtual void onJoinSuccess()
            {
            }

            virtual void onJoinFailure(JOIN_CHANNEL_ERR errorCode)
            {
                (JOIN_CHANNEL_ERR) errorCode;
            }

            virtual void onLeave(LEAVE_CHANNEL_ERR errorCode)
            {
                (LEAVE_CHANNEL_ERR) errorCode;
            }
            
            /** Occurs when a message is received.
             * 
             * @param userId User ID of the user sending the message.
             * @param message The received message.
             * 
             */
            virtual void onMessageReceived(const char *userId, const IMessage *message)
            {
                (const char *) userId;
                (IMessage *) message;
            }

            virtual void onSendMessageState(long long messageId, CHANNEL_MESSAGE_STATE state)
            {
                (long long) messageId;
                (CHANNEL_MESSAGE_STATE) state;
            }
            /** Occurs when a user joins a channel.
            * 
            * @param member User ID of the user joining the channel.
            */
            virtual void onMemberJoined(IChannelMember *member)
            {
                (IChannelMember *) member;
            }
            /** Occurs when a user leaves a channel.
            * 
            * @param member User ID of the user leaving the channel.
            */
            virtual void onMemberLeft(IChannelMember *member)
            {
                (IChannelMember *) member;
            }

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

        class IChannel
        {
        public:
            virtual ~IChannel() {}
//            virtual void setEventHandler(IChannelEventHandler *eventHandler) = 0;
            virtual int join() = 0;
            virtual int leave() = 0;

            virtual int sendMessage(const IMessage *message) = 0;
//            virtual int updateAttributes(IChannelAttributes *attributes, long long &requestId) = 0;  // move request id to attribute
//            virtual int deleteAttributes(IChannelAttributes *attributes, long long &requestId) = 0;
            virtual const char *getId() const = 0;
            virtual int getMembers() = 0;

            // sync_call
            virtual void release() = 0;
        };

        class IRtmServiceEventHandler
        {
        public:
            virtual ~IRtmServiceEventHandler()
            {
            }

#pragma mark local
            virtual void onLoginSuccess() {}
            virtual void onLoginFailure(LOGIN_ERR_CODE errorCode)
            {
                (LOGIN_ERR_CODE) errorCode;
            }

            virtual void onLogout(LOGOUT_ERR_CODE errorCode)
            {
                (LOGOUT_ERR_CODE) errorCode;
            }
            /** Occurs when a connection state changes.
             
             @param state New connection state. See CONNECTION_STATE.
             @param reason Reason of the connection state change. See CONNECTION_CHANGE_REASON.
             */
            virtual void onConnectionStateChanged(CONNECTION_STATE state, CONNECTION_CHANGE_REASON reason)
            {
            }

            virtual void onSendMessageState(long long messageId, PEER_MESSAGE_STATE state)
            {
                (long long) messageId;
                (PEER_MESSAGE_STATE) state;
            }

            virtual void onMessageReceivedFromPeer(const char *peerId, const IMessage *message)
            {
                (char *) peerId;
                (IMessage *) message;
            }
        };

        class IRtmService
        {
        public:
            virtual ~IRtmService() {}

            virtual int initialize(const char *appId, IRtmServiceEventHandler *eventHandler) = 0;
            virtual void release(bool sync = false) = 0;

            /** Allows a user to log in Agora's real-time messaging system.
            *
            * @param token Token used to log in Agora's real-time messaging system and used when dynamic authentication is enabled. Set @token as @p nil in the integration and test stages. 
            * @param userId User ID of the user logging in Agora's real-time messaging system. The string must not exceed 64 bytes in length or start with a space.
            * Supported characters:
            * 
            * - The 26 lowercase English letters: a to z
            * - The 26 uppercase English letters: A to Z
            * - The 10 numbers: 0 to 9
            * - Space
            * - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ","
            * 
            * @note Do not set @p userId as @p null.
            */
            virtual int login(const char *token, const char *userId) = 0;
            
            /** Allows a user to log out of Agora's real-time messaging system. 
            *
            */
            virtual int logout() = 0;
            
            /** Allows a user to send a point-to-point (P2P) message to a specified user (peer).
             *
             * @param peerId User ID of the specified user (peer).  
             * @param message The message to be sent. For information about creating a message, see IMessage.
             */
            virtual int sendMessageToPeer(const char *peerId, const IMessage *message) = 0;
            
            /** Creates an Agora real-time messaging channel.
            *
            * @note You can create multiple channels in an IRtmService instance.
            *
            * @param channelId The unique channel name for the Agora RTM session in the string format smaller than 64 bytes and cannot be empty or set as @p null. 
            * Supported characters:
            * - The 26 lowercase English letters: a to z
            * - The 26 uppercase English letters: A to Z
            * - The 10 numbers: 0 to 9
            * - Space
            * - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ","
            * 
            * @param eventHandler See IChannelEventHandler.
            * @return Returns:
            * <ul>
            *     <li>An IChannel instance if the method call succeeds.</li>
            *     <li>@p null if this method call fails, mostly because of an invalid @p channelId.</li>
            * </ul>
            */
            virtual IChannel * createChannel(const char *channelId, IChannelEventHandler *eventHandler) = 0;

            // todo: move to private
            virtual int setParameters(const char* parameters) = 0;
//            virtual int getParameters(const char* key, any_document_t& result) = 0;
        };

    }
}
