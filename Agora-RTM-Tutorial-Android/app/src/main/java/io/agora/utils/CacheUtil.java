package io.agora.utils;

import android.content.Context;

import java.io.File;

public class CacheUtil {
    static String CACHE_DIR = "rtm_image_disk_cache";

    public static String getCacheFile(Context context, String id) {
        File parent = new File(context.getCacheDir(), CACHE_DIR);
        if (!parent.exists()) {
            parent.mkdirs();
        }
        return new File(parent, id).getAbsolutePath();
    }
}
