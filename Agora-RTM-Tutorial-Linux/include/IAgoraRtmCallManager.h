//
//  Agora's RTM SDK
//
//
//  Copyright (c) 2019 Agora.io. All rights reserved.
//
#pragma once

#include "IAgoraRtmService.h"

namespace agora {
  namespace rtm {

    enum LOCAL_INVITATION_STATE {
      LOCAL_INVITATION_STATE_IDLE = 0,
      LOCAL_INVITATION_STATE_SENT_TO_REMOTE = 1,
      LOCAL_INVITATION_STATE_RECEIVED_BY_REMOTE = 2,
      LOCAL_INVITATION_STATE_ACCEPTED_BY_REMOTE = 3,
      LOCAL_INVITATION_STATE_REFUSED_BY_REMOTE = 4,
      LOCAL_INVITATION_STATE_CANCELED = 5,
      LOCAL_INVITATION_STATE_FAILURE = 6,
    };

    enum REMOTE_INVITATION_STATE {
      REMOTE_INVITATION_STATE_IDLE = 0,
      REMOTE_INVITATION_STATE_INVITATION_RECEIVED = 1,
      REMOTE_INVITATION_STATE_ACCEPT_SENT_TO_LOCAL = 2,
      REMOTE_INVITATION_STATE_REFUSED = 3,
      REMOTE_INVITATION_STATE_ACCEPTED = 4,
      REMOTE_INVITATION_STATE_CANCELED = 5,
      REMOTE_INVITATION_STATE_FAILURE = 6,
    };

    enum LOCAL_INVITATION_ERR_CODE {
      LOCAL_INVITATION_ERR_OK = 0,
      LOCAL_INVITATION_ERR_PEER_OFFLINE = 1,
      LOCAL_INVITATION_ERR_PEER_NO_RESPONSE = 2,
      LOCAL_INVITATION_ERR_INVITATION_EXPIRE = 3,
      LOCAL_INVITATION_ERR_NOT_LOGGEDIN = 4,
    };

    enum REMOTE_INVITATION_ERR_CODE {
      REMOTE_INVITATION_ERR_OK = 0,
      REMOTE_INVITATION_ERR_PEER_OFFLINE = 1,
      REMOTE_INVITATION_ERR_ACCEPT_FAILURE = 2,
      REMOTE_INVITATION_ERR_INVITATION_EXPIRE = 3,
    };

    enum INVITATION_API_CALL_ERR_CODE {
      INVITATION_API_CALL_ERR_OK = 0,
      INVITATION_API_CALL_ERR_INVALID_ARGUMENT = 1,
      INVITATION_API_CALL_ERR_NOT_STARTED = 2,
      INVITATION_API_CALL_ERR_ALREADY_END = 3, // accepted, failure, canceled, refused
      INVITATION_API_CALL_ERR_ALREADY_ACCEPT = 4,   // more details
      INVITATION_API_CALL_ERR_ALREADY_SENT = 5,
    };

    /**  RTM call request methods.
    */
    class ILocalCallInvitation
    {
    protected:
      virtual ~ILocalCallInvitation() {}
    public:
      virtual const char *getCalleeId() const = 0;

      virtual void setContent(const char *content) = 0;
      virtual const char *getContent() const = 0;

      virtual void setChannelId(const char *channelId) = 0;
      virtual const char *getChannelId() const = 0;

      virtual const char *getResponse() const = 0;

      virtual LOCAL_INVITATION_STATE getState() const = 0;

      virtual void release() = 0;
    };

    class IRemoteCallInvitation
    {
    protected:
      virtual ~IRemoteCallInvitation() {}
    public:
      virtual const char *getCallerId() const = 0;

      virtual const char *getContent() const = 0;

      virtual void setResponse(const char *response) = 0;
      virtual const char *getResponse() const = 0;

      virtual const char *getChannelId() const = 0;

      virtual REMOTE_INVITATION_STATE getState() const = 0;
      virtual void release() = 0;
    };

    class IRtmCallEventHandler
    {
    public:
      virtual ~IRtmCallEventHandler()
      {
      }

      virtual void onLocalInvitationReceivedByPeer(ILocalCallInvitation *localInvitation)
      {
        (ILocalCallInvitation *) localInvitation;
      }

      virtual void onLocalInvitationCanceled(ILocalCallInvitation *localInvitation)
      {
        (ILocalCallInvitation *) localInvitation;
      }

      virtual void onLocalInvitationFailure(ILocalCallInvitation *localInvitation, LOCAL_INVITATION_ERR_CODE errorCode)
      {
        (ILocalCallInvitation *) localInvitation;
        (LOCAL_INVITATION_ERR_CODE) errorCode;
      }

      virtual void onLocalInvitationAccepted(ILocalCallInvitation *localInvitation, const char *response)
      {
        (ILocalCallInvitation *) localInvitation;
        (const char *) response;
      }

      virtual void onLocalInvitationRefused(ILocalCallInvitation *localInvitation, const char *response)
      {
        (ILocalCallInvitation *) localInvitation;
        (const char *) response;
      }

      virtual void onRemoteInvitationRefused(IRemoteCallInvitation *remoteInvitation)
      {
        (IRemoteCallInvitation *) remoteInvitation;
      }

      virtual void onRemoteInvitationAccepted(IRemoteCallInvitation *remoteInvitation)
      {
        (IRemoteCallInvitation *) remoteInvitation;
      }

      virtual void onRemoteInvitationReceived(IRemoteCallInvitation *remoteInvitation)
      {
        (IRemoteCallInvitation *) remoteInvitation;
      }

      virtual void onRemoteInvitationFailure(IRemoteCallInvitation *remoteInvitation, REMOTE_INVITATION_ERR_CODE errorCode)
      {
        (IRemoteCallInvitation *) remoteInvitation;
      }

      virtual void onRemoteInvitationCanceled(IRemoteCallInvitation *remoteInvitation)
      {
        (IRemoteCallInvitation *) remoteInvitation;
      }
    };

    class IRtmCallManager
    {
    protected:
      virtual ~IRtmCallManager() {}
    public:
      /** Send Rtm call invite request.

      */
      virtual int sendLocalInvitation(ILocalCallInvitation *invitation)= 0;
      /** Send Rtm call invite request.

      */
      virtual int acceptRemoteInvitation(IRemoteCallInvitation *invitation) = 0;
      /** Send Rtm call refuse request

      */
      virtual int refuseRemoteInvitation(IRemoteCallInvitation *invitation) = 0;
      /** Send Rtm call end request
      */
      virtual int cancelLocalInvitation(ILocalCallInvitation *invitation) = 0;

      virtual ILocalCallInvitation *createLocalCallInvitation(const char *calleeId) = 0;
      // sync_call
      /** Releases all resources used by the IRtmCall instance.
      */
      virtual void release() = 0;
    };
  }
}
