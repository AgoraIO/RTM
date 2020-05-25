package io.agora.rtmtutorial;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import io.agora.rtm.RtmMessage;

/**
 * Receives and manages messages from RTM engine.
 */
public class RtmMessagePool {
    private Map<String, List<RtmMessage>> mOfflineMessageMap = new HashMap<>();

    void insertOfflineMessage(RtmMessage rtmMessage, String peerId) {
        boolean contains = mOfflineMessageMap.containsKey(peerId);
        List<RtmMessage> list = contains ? mOfflineMessageMap.get(peerId) : new ArrayList<>();

        if (list != null) {
            list.add(rtmMessage);
        }

        if (!contains) {
            mOfflineMessageMap.put(peerId, list);
        }
    }

    List<RtmMessage> getAllOfflineMessages(String peerId) {
        return mOfflineMessageMap.containsKey(peerId) ?
                mOfflineMessageMap.get(peerId) : new ArrayList<>();
    }

    void removeAllOfflineMessages(String peerId) {
        mOfflineMessageMap.remove(peerId);
    }
}
