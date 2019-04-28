//  Agora Engine SDK
//
//  Copyright (c) 2018 Agora.io. All rights reserved.
//

#ifndef AGORA_BASE_H
#define AGORA_BASE_H

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define AGORA_CALL __cdecl
#if defined(AGORARTC_EXPORT)
#define AGORA_API extern "C" __declspec(dllexport)
#else
#define AGORA_API extern "C" __declspec(dllimport)
#endif
#elif defined(__APPLE__)
#define AGORA_API __attribute__((visibility("default"))) extern "C"
#define AGORA_CALL
#elif defined(__ANDROID__) || defined(__linux__)
#if defined(__ANDROID__) && defined(FEATURE_RTM_STANDALONE_SDK)
#define AGORA_API extern "C"
#else
#define AGORA_API extern "C" __attribute__((visibility("default")))
#endif
#define AGORA_CALL
#else
#define AGORA_API extern "C"
#define AGORA_CALL
#endif

namespace agora {
namespace util {

template<class T>
class AutoPtr {
    typedef T value_type;
    typedef T* pointer_type;
public:
    AutoPtr(pointer_type p=0)
        :ptr_(p)
    {}
    ~AutoPtr() {
        if (ptr_)
            ptr_->release();
    }
    operator bool() const { return ptr_ != (pointer_type)0; }
    value_type& operator*() const {
        return *get();
    }

    pointer_type operator->() const {
        return get();
    }

    pointer_type get() const {
        return ptr_;
    }

    pointer_type release() {
        pointer_type tmp = ptr_;
        ptr_ = 0;
        return tmp;
    }

    void reset(pointer_type ptr = 0) {
        if (ptr != ptr_ && ptr_)
            ptr_->release();
        ptr_ = ptr;
    }
    template<class C1, class C2>
    bool queryInterface(C1* c, C2 iid) {
        pointer_type p = NULL;
        if (c && !c->queryInterface(iid, (void**)&p))
        {
            reset(p);
        }
        return p != NULL;
	}
private:
    AutoPtr(const AutoPtr&);
    AutoPtr& operator=(const AutoPtr&);
private:
    pointer_type ptr_;
};
class IString {
public:
    virtual bool empty() const = 0;
    virtual const char* c_str() = 0;
    virtual const char* data() = 0;
    virtual size_t length() = 0;
    virtual void release() = 0;
};
typedef AutoPtr<IString> AString;

}//namespace util

enum INTERFACE_ID_TYPE
{
    AGORA_IID_AUDIO_DEVICE_MANAGER = 1,
    AGORA_IID_VIDEO_DEVICE_MANAGER = 2,
    AGORA_IID_RTC_ENGINE_PARAMETER = 3,
    AGORA_IID_MEDIA_ENGINE = 4,
    AGORA_IID_SIGNALING_ENGINE = 8,
};

    /** Warning code.
     */
enum WARN_CODE_TYPE
{
  /** 8: The specified view is invalid. It is required to specify a view when using the video call function.
  */
    WARN_INVALID_VIEW = 8,
    /** 16: Failed to initialize the video function, possibly caused by lack of resources. The users cannot see the video while the voice communication is not affected.
    */
    WARN_INIT_VIDEO = 16,
    /** 20: The request is pending, usually due to some module not being ready, and the SDK postponed processing the request.
    */
    WARN_PENDING = 20,
    /** 103: No channel resources are available. Maybe because the server cannot allocate any channel resource.
    */
    WARN_NO_AVAILABLE_CHANNEL = 103,
    /** 104: A timeout when looking up the channel. When joining a channel, the SDK looks up the specified channel. The warning usually occurs when the network condition is too poor to connect to the server.
    */
    WARN_LOOKUP_CHANNEL_TIMEOUT = 104,
    /** 105: The server rejected the request to look up the channel. The server cannot process this request or the request is illegal.
    */
    WARN_LOOKUP_CHANNEL_REJECTED = 105,
    /** 106: A timeout when opening the channel. Once the specific channel is found, the SDK opens the channel. The warning usually occurs when the network condition is too poor to connect to the server.
    */
    WARN_OPEN_CHANNEL_TIMEOUT = 106,
    /** 107: The server rejected the request to open the channel. The server cannot process this request or the request is illegal.
    */
    WARN_OPEN_CHANNEL_REJECTED = 107,

    // sdk: 100~1000
    /** 111: A timeout when switching the live video.
    */
    WARN_SWITCH_LIVE_VIDEO_TIMEOUT = 111,
    /** 118: A timeout when setting the client role in the broadcast mode.
    */
    WARN_SET_CLIENT_ROLE_TIMEOUT = 118,
    /** 121: The ticket to open the channel is invalid.
    */
    WARN_OPEN_CHANNEL_INVALID_TICKET = 121,
    /** 122: Try another server.
    */
    WARN_OPEN_CHANNEL_TRY_NEXT_VOS = 122,
    /** 701: Error in opening the audio mixing.
    */
    WARN_AUDIO_MIXING_OPEN_ERROR = 701,
    /** 1014: Audio Device Module: A warning in the runtime playback device.
    */
    WARN_ADM_RUNTIME_PLAYOUT_WARNING = 1014,
    /** 1016: Audio Device Module: A warning in the runtime recording device.
    */
    WARN_ADM_RUNTIME_RECORDING_WARNING = 1016,
    /** 1019: Audio Device Module: No valid audio data is collected. This warning does not affect the ongoing call.
    */
    WARN_ADM_RECORD_AUDIO_SILENCE = 1019,
    /** 1020: Audio Device Module: A playback device failure.
    */
    WARN_ADM_PLAYOUT_MALFUNCTION = 1020,
    /** 1021: Audio Device Module: A recording device failure.
    */
    WARN_ADM_RECORD_MALFUNCTION = 1021,
    /**
    */
    WARN_ADM_IOS_CATEGORY_NOT_PLAYANDRECORD = 1029,
    /**
    */
    WARN_ADM_IOS_SAMPLERATE_CHANGE = 1030,
    /** 1031: Audio Device Module: The recorded audio voice is too low.
    */
    WARN_ADM_RECORD_AUDIO_LOWLEVEL = 1031,
    /** 1032: Audio Device Module: The playback audio voice is too low.
    */
    WARN_ADM_PLAYOUT_AUDIO_LOWLEVEL = 1032,
    /**
    */
    WARN_ADM_WINDOWS_NO_DATA_READY_EVENT = 1040,
    /** 1051: Audio Device Module: Howling is detected.
    */
    WARN_APM_HOWLING = 1051,
    /** 1052: Audio Device Module: The device is in the glitch state.
    */
    WARN_ADM_GLITCH_STATE = 1052,
    /** 1053: Audio Device Module: The underlying audio settings have changed.
    */
    WARN_ADM_IMPROPER_SETTINGS = 1053,
    /**
        */
    WARN_ADM_WIN_CORE_NO_RECORDING_DEVICE = 1322,
    /**
    */
    WARN_ADM_WIN_CORE_NO_PLAYOUT_DEVICE = 1323,
    /**
    */
    WARN_ADM_WIN_CORE_IMPROPER_CAPTURE_RELEASE = 1324,

    WARN_RTM_LOGIN_TIMEOUT = 2005,
    WARN_RTM_KEEP_ALIVE_TIMEOUT = 2009
};

/** Error code.
*/
enum ERROR_CODE_TYPE
{
  /** 0: No error.
  */
    ERR_OK = 0,
    //1~1000
    /** 1: General error (no specified reason).
    */
    ERR_FAILED = 1,
    /** 2: Invalid parameter called. For example, the specific channel name includes illegal characters.
    */
    ERR_INVALID_ARGUMENT = 2,
    /** 3: The SDK module is not ready. Possible solutions:

     - Check the state of the audio device.
     - Check the completeness of the application.
     - Re-initiate the RTC engine.
     */
    ERR_NOT_READY = 3,
    /** 4: The SDK does not support this function.
     */
    ERR_NOT_SUPPORTED = 4,
    /** 5: The request is rejected. This is for internal SDK internal use only, and it will not return to the application through an API or callback event.
     */
    ERR_REFUSED = 5,
    /** 6: The buffer size is not big enough to store the returned data.
     */
    ERR_BUFFER_TOO_SMALL = 6,
    /** 7: The SDK is not initialized before calling this API.
     */
    ERR_NOT_INITIALIZED = 7,
    /** 9: No permission. This is for internal SDK internal use only, and it will not return to the application through an API or callback event.
     */
    ERR_NO_PERMISSION = 9,
    /** 10: An API timeout. Some APIs require the SDK to return the execution result, and this error occurs if the request takes too long (for more than 10 seconds) for the SDK to process.
     */
    ERR_TIMEDOUT = 10,
    /** 11: The request is canceled. This is for internal SDK internal use only, and it will not return to the application through an API or callback event.
     */
    ERR_CANCELED = 11,
    /** 12: The call frequency is too high. This is for internal SDK internal use only, and it will not return to the application through an API or callback event.
     */
    ERR_TOO_OFTEN = 12,
    /** 13: The SDK failed to bind to the network socket. This is for internal SDK internal use only, and it will not return to the application through an API or callback event.
     */
    ERR_BIND_SOCKET = 13,
    /** 14: The network is unavailable. This is for internal SDK internal use only, and it will not return to the application through an API or callback event.
     */
    ERR_NET_DOWN = 14,
    /** 15: No network buffers available. This is for internal SDK internal use only, and it will not return to the application through an API or callback event.
     */
    ERR_NET_NOBUFS = 15,
    /** 17: The request to join the channel is rejected. This error usually occurs when the user is already in the channel, and still calls the API to join the channel, for example, \ref agora::rtc::IRtcEngine::joinChannel "joinChannel".
     */
    ERR_JOIN_CHANNEL_REJECTED = 17,
    /** 18: The request to leave the channel is rejected.

     This error usually occurs:

     - When the user has left the channel and still calls \ref agora::rtc::IRtcEngine::leaveChannel "leaveChannel" to leave the channel. In this case, please stop calling \ref agora::rtc::IRtcEngine::leaveChannel "leaveChannel".
     - When the user has not joined the channel and and still calls \ref agora::rtc::IRtcEngine::leaveChannel "leaveChannel" to leave the channel. In this case, no extra operation is needed.
     */
    ERR_LEAVE_CHANNEL_REJECTED = 18,
    /** 19: Resources have been occupied and cannot be reused.
     */
    ERR_ALREADY_IN_USE = 19,
    /** 20: The SDK gave up the request due to too many requests. This is for internal SDK internal use only, and it will not return to the application through an API or callback event.
     */
    ERR_ABORTED = 20,
    /** 21: In Windows, specific firewall settings can cause the SDK to fail to initialize and crash.
     */
    ERR_INIT_NET_ENGINE = 21,
    /** 22: The application has taken up too much of the system resources and the SDK has failed to allocate the resources.
     */
    ERR_RESOURCE_LIMITED = 22,
    /** 101: The specified App ID is invalid. Please try to rejoin the channel with a valid App ID.
     */
    ERR_INVALID_APP_ID = 101,
    /** 102: The specified channel name is invalid. Please try to rejoin the channel with a valid channel name.
     */
    ERR_INVALID_CHANNEL_NAME = 102,
    /** 109: The Token expired due to one of the following reasons:

     - Authorized Timestamp expired: The timestamp is represented by the number of seconds elapsed since 1/1/1970. The user can use the Token to access the Agora service within five minutes after the Token is generated. If the user does not access the Agora service after five minutes, this Token will no longer be valid.
     - Call Expiration Timestamp expired: The timestamp is the exact time when a user can no longer use the Agora service (for example, when a user is forced to leave an ongoing call). When the value is set for the Call Expiration Timestamp, it does not mean that the Token will be expired, but that the user will be kicked out of the channel.
     */
    ERR_TOKEN_EXPIRED = 109,
    /** 110: The Token is invalid due to one of the following reasons:

     - The App Certificate for the project is enabled in Dashboard, but the user is still using the App ID. Once the App Certificate is enabled, the user must use a Token.
     - The uid field is mandatory, and users must set the same uid when setting the uid parameter when calling \ref agora::rtc::IRtcEngine::joinChannel "joinChannel".
     */
    ERR_INVALID_TOKEN = 110,
    /** 111: The internet connection is interrupted. This applies to the Agora Web SDK only.
     */
    ERR_CONNECTION_INTERRUPTED = 111, // only used in web sdk
    /** 112: The internet connection is lost. This applies to the Agora Web SDK only.
     */
    ERR_CONNECTION_LOST = 112, // only used in web sdk
    /** 113: The user is not in the channel when calling the \ref agora::rtc::IRtcEngine::sendStreamMessage "sendStreamMessage" method.
     */
    ERR_NOT_IN_CHANNEL = 113,
    /** 114: The size of the sent data is over 1024 bytes when the user calls the \ref agora::rtc::IRtcEngine::sendStreamMessage "sendStreamMessage" method.
     */
    ERR_SIZE_TOO_LARGE = 114,
    /** 115: The bitrate of the sent data exceeds the limit of 6 Kbps when the user calls the \ref agora::rtc::IRtcEngine::sendStreamMessage "sendStreamMessage" method.
     */
    ERR_BITRATE_LIMIT = 115,
    /** 116: Too many data streams (over 5 streams) are created when the user calls the \ref agora::rtc::IRtcEngine::createDataStream "createDataStream" method.
     */
    ERR_TOO_MANY_DATA_STREAMS = 116,
    /** 117: The data stream transmission timed out.
     */
    ERR_STREAM_MESSAGE_TIMEOUT = 117,
    /** 119: Switching roles failed. Please try to rejoin the channel.
     */
    ERR_SET_CLIENT_ROLE_NOT_AUTHORIZED = 119,
    /** 120: Decryption failed. The user may have used a different encryption password to join the channel. Please check your settings or try rejoining the channel.
     */
    ERR_DECRYPTION_FAILED = 120,
    /** 123: The client is banned by the server.
     */
    ERR_CLIENT_IS_BANNED_BY_SERVER = 123,
    /** 124: Error in the watermark file parameter.
     */
    ERR_WATERMARK_PARAM = 124,
    /** 125: Error in the watermark file path.
     */
    ERR_WATERMARK_PATH = 125,
    /** 126: Error in the watermark file format.
     */
    ERR_WATERMARK_PNG = 126,
    /** 127: Error in the watermark file information.
     */
    ERR_WATERMARKR_INFO = 127,
    /** 128: Error in the watermark file data format.
     */
    ERR_WATERMARK_ARGB = 128,
    /** 129: Error in reading the watermark file.
     */
    ERR_WATERMARK_READ = 129,
    /** 130: Encryption is enabled when the user calls the \ref agora::rtc::IRtcEngine::addPublishStreamUrl "addPublishStreamUrl" method (CDN live streaming does not support encrypted streams).
     */
    ERR_ENCRYPTED_STREAM_NOT_ALLOWED_PUBLISH = 130,

    //signaling: 400~600
    /**
    */
    ERR_LOGOUT_OTHER = 400,  //
    /** 401: The user has logged out.
     */
    ERR_LOGOUT_USER = 401,  // logout by user
    /** 402: Network failure.
     */
    ERR_LOGOUT_NET = 402,  // network failure
    /** 403: Logged in with another device.
     */
    ERR_LOGOUT_KICKED = 403,  // login in other device
    /**
     */
    ERR_LOGOUT_PACKET = 404,  //
    /** 405: The Token has expired.
     */
    ERR_LOGOUT_TOKEN_EXPIRED = 405,  // token expired
    /**
     */
    ERR_LOGOUT_OLDVERSION = 406,  //
    /**
     */
    ERR_LOGOUT_TOKEN_WRONG = 407,
    /**
    */
    ERR_LOGOUT_ALREADY_LOGOUT = 408,
    /**
     */
    ERR_LOGIN_OTHER = 420,
    /**
    */
    ERR_LOGIN_NET = 421,
    /**
     */
    ERR_LOGIN_FAILED = 422,
    /**
     */
    ERR_LOGIN_CANCELED = 423,
    /**
     */
    ERR_LOGIN_TOKEN_EXPIRED = 424,
    /**
     */
    ERR_LOGIN_OLD_VERSION = 425,
    /**
     */
    ERR_LOGIN_TOKEN_WRONG = 426,
    /**
     */
    ERR_LOGIN_TOKEN_KICKED = 427,
    /**
     */
    ERR_LOGIN_ALREADY_LOGIN = 428,
    /**
    */
    ERR_JOIN_CHANNEL_OTHER = 440,
    /**
     */
    ERR_SEND_MESSAGE_OTHER = 440,
    /**
     */
    ERR_SEND_MESSAGE_TIMEOUT = 441,
    /**
     */
    ERR_QUERY_USERNUM_OTHER = 450,
    /**
     */
    ERR_QUERY_USERNUM_TIMEOUT = 451,
    /**
     */
    ERR_QUERY_USERNUM_BYUSER = 452,
    /**
     */
    ERR_LEAVE_CHANNEL_OTHER = 460,
    /**
     */
    ERR_LEAVE_CHANNEL_KICKED = 461,
    /**
     */
    ERR_LEAVE_CHANNEL_BYUSER = 462,
    /**
     */
    ERR_LEAVE_CHANNEL_LOGOUT = 463,
    /**
     */
    ERR_LEAVE_CHANNEL_DISCONNECTED = 464,
    /**
     */
    ERR_INVITE_OTHER = 470,
    /**
     */
    ERR_INVITE_REINVITE = 471,
    /**
     */
    ERR_INVITE_NET = 472,
    /**
     */
    ERR_INVITE_PEER_OFFLINE = 473,
    ERR_INVITE_TIMEOUT = 474,
    ERR_INVITE_CANT_RECV = 475,


    //1001~2000
    /** 1001: Failed to load the media engine.
     */
    ERR_LOAD_MEDIA_ENGINE = 1001,
    /** 1002: Failed to start the call after enabling the media engine.
     */
    ERR_START_CALL = 1002,
    /** 1003: Failed to start the camera.
     */
    ERR_START_CAMERA = 1003,
    /** 1004: Failed to start the video rendering module.
     */
    ERR_START_VIDEO_RENDER = 1004,
    /** 1005: General error on the Audio Device Module (no specified reason). Please check if the audio device is used by other applications, or try joining the channel.
     */
    ERR_ADM_GENERAL_ERROR = 1005,
    /** 1006: Audio Device Module: Error in using the Java resources.
     */
    ERR_ADM_JAVA_RESOURCE = 1006,
    /** 1007: Audio Device Module: Error in setting the sampling frequency.
     */
    ERR_ADM_SAMPLE_RATE = 1007,
    /** 1008: Audio Device Module: Error in initializing the playback device.
     */
    ERR_ADM_INIT_PLAYOUT = 1008,
    /** 1009: Audio Device Module: Error in starting the playback device.
     */
    ERR_ADM_START_PLAYOUT = 1009,
    /** 1010: Audio Device Module: Error in stopping the playback device.
     */
    ERR_ADM_STOP_PLAYOUT = 1010,
    /** 1011: Audio Device Module: Error in initializing the recording device.
     */
    ERR_ADM_INIT_RECORDING = 1011,
    /** 1012: Audio Device Module: Error in starting the recording device.
     */
    ERR_ADM_START_RECORDING = 1012,
    /** 1013: Audio Device Module: Error in stopping the recording device.
     */
    ERR_ADM_STOP_RECORDING = 1013,
    /** 1015: Audio Device Module: Runtime playback error. Please check the status of your playback device and try rejoining the channel.
     */
    ERR_ADM_RUNTIME_PLAYOUT_ERROR = 1015,
    /** 1017: Audio Device Module: Runtime recording error.
     */
    ERR_ADM_RUNTIME_RECORDING_ERROR = 1017,
    /** 1018: Audio Device Module: Failed to record.
     */
    ERR_ADM_RECORD_AUDIO_FAILED = 1018,
    /** 1022: Audio Device Module: Error in initializing the loopback device.
     */
    ERR_ADM_INIT_LOOPBACK = 1022,
    /** 1023: Audio Device Module: Error in starting the loopback device.
     */
    ERR_ADM_START_LOOPBACK = 1023,
    /** 1027: Audio Device Module: No recording permission. Please check if the recording permission is granted.
     */
    ERR_ADM_NO_PERMISSION = 1027,
    ERR_ADM_RECORD_AUDIO_IS_ACTIVE = 1033,
    ERR_ADM_ANDROID_JNI_JAVA_RESOURCE = 1101,
    ERR_ADM_ANDROID_JNI_NO_RECORD_FREQUENCY = 1108,
    ERR_ADM_ANDROID_JNI_NO_PLAYBACK_FREQUENCY = 1109,
    ERR_ADM_ANDROID_JNI_JAVA_START_RECORD = 1111,
    ERR_ADM_ANDROID_JNI_JAVA_START_PLAYBACK = 1112,
    ERR_ADM_ANDROID_JNI_JAVA_RECORD_ERROR = 1115,
    ERR_ADM_ANDROID_OPENSL_CREATE_ENGINE = 1151,
    ERR_ADM_ANDROID_OPENSL_CREATE_AUDIO_RECORDER = 1153,
    ERR_ADM_ANDROID_OPENSL_START_RECORDER_THREAD = 1156,
    ERR_ADM_ANDROID_OPENSL_CREATE_AUDIO_PLAYER = 1157,
    ERR_ADM_ANDROID_OPENSL_START_PLAYER_THREAD = 1160,
    ERR_ADM_IOS_INPUT_NOT_AVAILABLE = 1201,
    ERR_ADM_IOS_ACTIVATE_SESSION_FAIL = 1206,
    ERR_ADM_IOS_VPIO_INIT_FAIL = 1210,
    ERR_ADM_IOS_VPIO_REINIT_FAIL = 1213,
    ERR_ADM_IOS_VPIO_RESTART_FAIL = 1214,
    ERR_ADM_IOS_SET_RENDER_CALLBACK_FAIL = 1219,
    ERR_ADM_IOS_SESSION_SAMPLERATR_ZERO = 1221,
    ERR_ADM_WIN_CORE_INIT = 1301,
    ERR_ADM_WIN_CORE_INIT_RECORDING = 1303,
    ERR_ADM_WIN_CORE_INIT_PLAYOUT = 1306,
    ERR_ADM_WIN_CORE_INIT_PLAYOUT_NULL = 1307,
    ERR_ADM_WIN_CORE_START_RECORDING = 1309,
    ERR_ADM_WIN_CORE_CREATE_REC_THREAD = 1311,
    ERR_ADM_WIN_CORE_CAPTURE_NOT_STARTUP = 1314,
    ERR_ADM_WIN_CORE_CREATE_RENDER_THREAD = 1319,
    ERR_ADM_WIN_CORE_RENDER_NOT_STARTUP = 1320,
    ERR_ADM_WIN_CORE_NO_RECORDING_DEVICE = 1322,
    ERR_ADM_WIN_CORE_NO_PLAYOUT_DEVICE = 1323,
    ERR_ADM_WIN_WAVE_INIT = 1351,
    ERR_ADM_WIN_WAVE_INIT_RECORDING = 1353,
    ERR_ADM_WIN_WAVE_INIT_MICROPHONE = 1354,
    ERR_ADM_WIN_WAVE_INIT_PLAYOUT = 1355,
    ERR_ADM_WIN_WAVE_INIT_SPEAKER = 1356,
    ERR_ADM_WIN_WAVE_START_RECORDING = 1357,
    ERR_ADM_WIN_WAVE_START_PLAYOUT = 1358,
    /** 1359: Audio Device Module: No recording device.
     */
    ERR_ADM_NO_RECORDING_DEVICE = 1359,
    /** 1360: Audio Device Module: No playback device.
     */
    ERR_ADM_NO_PLAYOUT_DEVICE = 1360,

    // VDM error code starts from 1500
    /** 1501: Video Device Module: The camera is not authorized.
     */
    ERR_VDM_CAMERA_NOT_AUTHORIZED = 1501,

    // VCM error code starts from 1600
    /** 1600: Video Device Module: Unknown error.
     */
    ERR_VCM_UNKNOWN_ERROR = 1600,
    /** 1601: Video Device Module: Error in initializing the video encoder.
    */
    ERR_VCM_ENCODER_INIT_ERROR = 1601,
    /** 1602: Video Device Module: Error in encoding.
     */
    ERR_VCM_ENCODER_ENCODE_ERROR = 1602,
    /** 1603: Video Device Module: Error in setting the video encoder.
     */
    ERR_VCM_ENCODER_SET_ERROR = 1603,
};

    /** Output log filter level. */
enum LOG_FILTER_TYPE
{
/** 0: Do not output any log information. */
    LOG_FILTER_OFF = 0,
     /** 0x080f: Output all API log information */
    LOG_FILTER_DEBUG = 0x080f,
     /** 0x000f: Output CRITICAL, ERROR, WARNING, and INFO level log information. */
    LOG_FILTER_INFO = 0x000f,
     /** 0x000e: Outputs CRITICAL, ERROR, and WARNING level log information. */
    LOG_FILTER_WARN = 0x000e,
     /** 0x000c: Outputs CRITICAL and ERROR level log information. */
    LOG_FILTER_ERROR = 0x000c,
     /** 0x0008: Outputs CRITICAL level log information. */
    LOG_FILTER_CRITICAL = 0x0008,
    LOG_FILTER_MASK = 0x80f,
};
} // namespace agora

#endif
