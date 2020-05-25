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
import io.agora.rtm.RtmImageMessage;
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

    public static void uploadImage(final Context context, RtmClient rtmClient, final String file, @NonNull final ResultCallback<RtmImageMessage> resultCallback) {
        rtmClient.createImageMessageByUploading(file, new RtmRequestId(), new ResultCallback<RtmImageMessage>() {
            @Override
            public void onSuccess(final RtmImageMessage rtmImageMessage) {
                int width = rtmImageMessage.getWidth() / 5;
                int height = rtmImageMessage.getWidth() / 5;
                rtmImageMessage.setThumbnail(ImageUtil.preloadImage(context, file, width, height));
                rtmImageMessage.setThumbnailWidth(width);
                rtmImageMessage.setThumbnailHeight(height);

                resultCallback.onSuccess(rtmImageMessage);
            }

            @Override
            public void onFailure(ErrorInfo errorInfo) {
                resultCallback.onFailure(errorInfo);
            }
        });
    }

    public static void cacheImage(Context context, RtmClient rtmClient, RtmImageMessage rtmImageMessage, @NonNull ResultCallback<String> resultCallback) {
        final String cacheFile = getCacheFile(context, rtmImageMessage.getMediaId());
        if (new File(cacheFile).exists()) {
            resultCallback.onSuccess(cacheFile);
        } else {
            rtmClient.downloadMediaToFile(
                    rtmImageMessage.getMediaId(),
                    cacheFile,
                    new RtmRequestId(),
                    new ResultCallback<Void>() {
                        @Override
                        public void onSuccess(Void aVoid) {
                            resultCallback.onSuccess(cacheFile);
                        }

                        @Override
                        public void onFailure(ErrorInfo errorInfo) {
                            resultCallback.onFailure(errorInfo);
                        }
                    }
            );
        }
    }
}
