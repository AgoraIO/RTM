package io.agora.rtmtutorial;

import android.app.Application;

public class AGApplication extends Application {
    private static AGApplication sInstance;
    private ChatManager mChatManager;


    public static AGApplication the() {
        return sInstance;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        sInstance = this;

        mChatManager = new ChatManager(this);
        mChatManager.init();
    }

    public ChatManager getChatManager() {
        return mChatManager;
    }
}

