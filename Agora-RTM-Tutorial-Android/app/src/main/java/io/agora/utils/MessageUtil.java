package io.agora.utils;


import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import io.agora.model.MessageBean;
import io.agora.model.MessageListBean;
import io.agora.rtm.RtmMessage;
import io.agora.rtmtutorial.R;

public class MessageUtil {
    public static final int MAX_INPUT_NAME_LENGTH = 64;

    public static final int ACTIVITY_RESULT_CONN_ABORTED = 1;

    public static final String INTENT_EXTRA_IS_PEER_MODE = "chatMode";
    public static final String INTENT_EXTRA_USER_ID = "userId";
    public static final String INTENT_EXTRA_TARGET_NAME = "targetName";

    public static Random RANDOM = new Random();

    public static final int[] COLOR_ARRAY = new int[]{
            R.drawable.shape_circle_black,
            R.drawable.shape_circle_blue,
            R.drawable.shape_circle_pink,
            R.drawable.shape_circle_pink_dark,
            R.drawable.shape_circle_yellow,
            R.drawable.shape_circle_red
    };

    private static List<MessageListBean> messageListBeanList = new ArrayList<>();

    public static void addMessageListBeanList(MessageListBean messageListBean) {
        messageListBeanList.add(messageListBean);
    }

    // clean up list on logout
    public static void cleanMessageListBeanList() {
        messageListBeanList.clear();
    }

    public static MessageListBean getExistMessageListBean(String accountOther) {
        int ret = existMessageListBean(accountOther);
        if (ret > -1) {
            return messageListBeanList.remove(ret);
        }
        return null;
    }

    // return existing list position
    private static int existMessageListBean(String userId) {
        int size = messageListBeanList.size();
        for (int i = 0; i < size; i++) {
            if (messageListBeanList.get(i).getAccountOther().equals(userId)) {
                return i;
            }
        }
        return -1;
    }

    public static void addMessageBean(String account, RtmMessage msg) {
        MessageBean messageBean = new MessageBean(account, msg, false);
        int ret = existMessageListBean(account);
        if (ret == -1) {
            // account not exist new messagelistbean
            messageBean.setBackground(MessageUtil.COLOR_ARRAY[RANDOM.nextInt(MessageUtil.COLOR_ARRAY.length)]);
            List<MessageBean> messageBeanList = new ArrayList<>();
            messageBeanList.add(messageBean);
            messageListBeanList.add(new MessageListBean(account, messageBeanList));

        } else {
            // account exist get messagelistbean
            MessageListBean bean = messageListBeanList.remove(ret);
            List<MessageBean> messageBeanList = bean.getMessageBeanList();
            if (messageBeanList.size() > 0) {
                messageBean.setBackground(messageBeanList.get(0).getBackground());
            } else {
                messageBean.setBackground(MessageUtil.COLOR_ARRAY[RANDOM.nextInt(MessageUtil.COLOR_ARRAY.length)]);
            }
            messageBeanList.add(messageBean);
            bean.setMessageBeanList(messageBeanList);
            messageListBeanList.add(bean);
        }
    }
}
