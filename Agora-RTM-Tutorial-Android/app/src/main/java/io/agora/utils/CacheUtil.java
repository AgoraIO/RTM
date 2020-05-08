package io.agora.utils;

import android.content.Context;

import java.io.File;

import io.agora.rtm.ResultCallback;
import io.agora.rtm.RtmClient;
import io.agora.rtm.RtmImageMessage;
import io.agora.rtm.RtmRequestId;

public class CacheUtil {
    static String CACHE_DIR = "rtm_image_disk_cache";

    public static String getCacheFile(Context context, String id) {
        File parent = new File(context.getCacheDir(), CACHE_DIR);
        if (!parent.exists()) {
            parent.mkdirs();
        }
        return new File(parent, id).getAbsolutePath();
    }

    public static void cacheImage(Context context, RtmClient rtmClient, RtmImageMessage rtmImageMessage, ResultCallback<Void> resultCallback) {
        rtmImageMessage.setFileName(getCacheFile(context, rtmImageMessage.getMediaId()));
        rtmClient.downloadMediaToFile(
                rtmImageMessage.getMediaId(),
                rtmImageMessage.getFileName(),
                new RtmRequestId(),
                resultCallback
        );
    }
}
