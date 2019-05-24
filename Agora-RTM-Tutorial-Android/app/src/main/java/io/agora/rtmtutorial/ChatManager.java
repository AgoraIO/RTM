package io.agora.rtmtutorial;

import android.content.Context;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;

import io.agora.rtm.RtmClient;
import io.agora.rtm.RtmClientListener;
import io.agora.rtm.RtmMessage;

public class ChatManager {
    private static final String TAG = ChatManager.class.getSimpleName();

    private Context mContext;
    private RtmClient mRtmClient;
    private List<RtmClientListener> mListenerList = new ArrayList<>();

    public ChatManager(Context context) {
        mContext = context;
    }

    public void init() {
        String appID = mContext.getString(R.string.agora_app_id);

        try {
            mRtmClient = RtmClient.createInstance(mContext, appID, new RtmClientListener() {
                @Override
                public void onConnectionStateChanged(int state, int reason) {
                    for (RtmClientListener listener : mListenerList) {
                        listener.onConnectionStateChanged(state, reason);
                    }
                }

                @Override
                public void onMessageReceived(RtmMessage rtmMessage, String peerId) {
                    for (RtmClientListener listener : mListenerList) {
                        listener.onMessageReceived(rtmMessage, peerId);
                    }
                }

                @Override
                public void onTokenExpired() {

                }
            });

            if (BuildConfig.DEBUG) {
                mRtmClient.setParameters("{\"rtm.log_filter\": 65535}");
            }
        } catch (Exception e) {
            Log.e(TAG, Log.getStackTraceString(e));
            throw new RuntimeException("NEED TO check rtm sdk init fatal error\n" + Log.getStackTraceString(e));
        }
    }

    public RtmClient getRtmClient() {
        return mRtmClient;
    }

    public void registerListener(RtmClientListener listener) {
        mListenerList.add(listener);
    }

    public void unregisterListener(RtmClientListener listener) {
        mListenerList.remove(listener);
    }
}
