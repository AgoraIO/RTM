package io.agora.utils;

import android.content.Context;
import android.graphics.Bitmap;

import androidx.annotation.NonNull;

import com.bumptech.glide.Glide;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.util.concurrent.ExecutionException;

import io.agora.rtm.ErrorInfo;
import io.agora.rtm.ResultCallback;
import io.agora.rtm.RtmClient;
import io.agora.rtm.RtmRequestId;

public class ImageUtil {
    private static final String CACHE_DIR = "rtm_image_disk_cache";

    public static String getCacheFile(Context context, String id) {
        File parent = new File(context.getCacheDir(), CACHE_DIR);
        if (!parent.exists()) {
            parent.mkdirs();
        }
        return new File(parent, id).getAbsolutePath();
    }

    public static byte[] bitmapToByteArray(Bitmap bitmap) {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        bitmap.compress(Bitmap.CompressFormat.JPEG, 10, baos);
        return baos.toByteArray();
    }

    public static byte[] preloadImage(Context context, String file, int width, int height) {
        try {
            Bitmap bitmap = Glide.with(context).asBitmap().encodeQuality(10).load(file).submit(width, height).get();
            return bitmapToByteArray(bitmap);
        } catch (ExecutionException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        return null;
    }
}
