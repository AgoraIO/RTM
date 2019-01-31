package io.agora.activity;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.OrientationHelper;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import io.agora.adapter.MessageAdapter;
import io.agora.model.MessageBean;
import io.agora.model.MessageListBean;
import io.agora.rtm.ErrorInfo;
import io.agora.rtm.IResultCallback;
import io.agora.rtm.IStateListener;
import io.agora.rtm.RtmChannel;
import io.agora.rtm.RtmChannelListener;
import io.agora.rtm.RtmChannelMember;
import io.agora.rtm.RtmClient;
import io.agora.rtm.RtmClientListener;
import io.agora.rtm.RtmMessage;
import io.agora.rtm.RtmStatusCode;
import io.agora.rtmtutorial.AGApplication;
import io.agora.rtmtutorial.R;
import io.agora.rtmtutorial.ChatManager;
import io.agora.utils.Constant;


public class MessageActivity extends Activity {
    private final String TAG = MessageActivity.class.getSimpleName();

    private TextView textViewTitle;
    private EditText editText;
    private RecyclerView recyclerView;
    private List<MessageBean> messageBeanList = new ArrayList<>();
    private MessageAdapter adapter;

    private String selfName = "";
    private String peerName = "";
    private boolean peerToPeerMode; // peer to peer mode, or channel mode
    private String channelName = "";
    private int channelMemberCount = 1;

    private ChatManager mChatManager;
    private RtmClient mRtmClient;
    private RtmClientListener mClientListener;
    private RtmChannel mRtmChannel;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_message);

        initUIAndData();
        initChat();
    }

    private void initUIAndData() {
        textViewTitle = (TextView) findViewById(R.id.message_title);
        editText = (EditText) findViewById(R.id.message_edittiext);
        recyclerView = (RecyclerView) findViewById(R.id.message_list);

        Intent intent = getIntent();
        peerToPeerMode = intent.getBooleanExtra("mode", true);
        selfName = intent.getStringExtra("selfname");
        String name = intent.getStringExtra("name");
        if (peerToPeerMode) {
            peerName = name;
            textViewTitle.setText(peerName);
            MessageListBean messageListBean = Constant.getExistMessageListBean(peerName);
            if (messageListBean != null) {
                messageBeanList.addAll(messageListBean.getMessageBeanList());
            }
        } else {
            channelName = name;
            channelMemberCount = 1;
            textViewTitle.setText(channelName + "(" + channelMemberCount + ")");
        }

        LinearLayoutManager layoutManager = new LinearLayoutManager(this);
        recyclerView.setLayoutManager(layoutManager);
        layoutManager.setOrientation(OrientationHelper.VERTICAL);
        adapter = new MessageAdapter(this, messageBeanList);
        recyclerView.setAdapter(adapter);
    }

    private void initChat() {
        mChatManager = AGApplication.the().getChatManager();
        mRtmClient = mChatManager.getRtmClient();
        mClientListener = new MyRtmClientListener();
        mChatManager.registerListener(mClientListener);

        if (!peerToPeerMode) {
            createAndJoinChannel();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (peerToPeerMode) {
            Constant.addMessageListBeanList(new MessageListBean(peerName, messageBeanList));
        } else {
            leaveAndReleaseChannel();
        }
        mChatManager.unregisterListener(mClientListener);
    }

    public void onClickSend(View v) {
        String msg = editText.getText().toString();
        if (!msg.equals("")) {
            MessageBean messageBean = new MessageBean(selfName, msg, true);
            messageBeanList.add(messageBean);
            adapter.notifyItemRangeChanged(messageBeanList.size(), 1);
            recyclerView.scrollToPosition(messageBeanList.size() - 1);
            if (peerToPeerMode) {
                sendPeerMessage(msg);
            } else {
                sendChannelMessage(msg);
            }
        }
        editText.setText("");
    }

    public void onClickFinish(View v) {
        finish();
    }

    // api call: create and join channel
    private void createAndJoinChannel() {

        // 1: create channel
        mRtmChannel = mRtmClient.createChannel(channelName, new MyChannelListener());
        if (mRtmChannel == null) {
            showToast(getString(R.string.join_channel_failed));
            finish();
            return;
        }

        // 2: join channel
        mRtmChannel.join(new IResultCallback<Void>() {
            @Override
            public void onSuccess(Void responseInfo) {

            }

            @Override
            public void onFailure(ErrorInfo errorInfo) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        showToast(getString(R.string.join_channel_failed));
                        finish();
                    }
                });
            }
        });
    }

    // api call: leave and release channel
    private void leaveAndReleaseChannel() {
        if (mRtmChannel != null) {
            mRtmChannel.leave(null);
            mRtmChannel.release();
            mRtmChannel = null;
        }
    }

    // api call: send message to peer
    private void sendPeerMessage(String content) {
        // 1: create a message
        RtmMessage message = RtmMessage.createMessage();
        message.setText(content);
    
        // 2: send message to peer
        mRtmClient.sendMessageToPeer(peerName, message, new IStateListener() {
            @Override
            public void onStateChanged(final int newState) {
                // refer to RtmStatusCode.PeerMessageState for the message state
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        switch (newState) {
                            case RtmStatusCode.PeerMessageState.PEER_MESSAGE_SENT_TIMEOUT:
                            case RtmStatusCode.PeerMessageState.PEER_MESSAGE_FAILURE:
                                showToast(getString(R.string.send_msg_failed));
                                break;
                            case RtmStatusCode.PeerMessageState.PEER_MESSAGE_PEER_UNREACHABLE:
                                showToast(getString(R.string.peer_offline));
                                break;
                        }
                    }
                });
            }
        });
    }

    // api call: send message to channel
    private void sendChannelMessage(String content) {
        // 1: create a message
        RtmMessage message = RtmMessage.createMessage();
        message.setText(content);

        // 2: send message to channel
        mRtmChannel.sendMessage(message, new IStateListener() {
            @Override
            public void onStateChanged(final int newState) {
                // refer to RtmStatusCode.ChannelMessageState for the message state
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        switch (newState) {
                            case RtmStatusCode.ChannelMessageState.CHANNEL_MESSAGE_SENT_TIMEOUT:
                            case RtmStatusCode.ChannelMessageState.CHANNEL_MESSAGE_FAILURE:
                                showToast(getString(R.string.send_msg_failed));
                                break;
                        }
                    }
                });
            }
        });
    }

    // api callback: rtm event listener
    class MyRtmClientListener implements RtmClientListener {

        @Override
        public void onConnectionStateChanged(final int state, int reason) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    switch (state) {
                        case RtmStatusCode.ConnectionState.CONNECTION_STATE_RECONNECTING:
                            showToast(getString(R.string.reconnecting));
                            break;
                        case RtmStatusCode.ConnectionState.CONNECTION_STATE_ABORTED:
                            showToast(getString(R.string.account_offline));
                            setResult(RESULT_CANCELED);
                            finish();
                            break;
                    }
                }
            });
        }

        @Override
        public void onMessageReceived(final RtmMessage message, final String peerId) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    String content = message.getText();
                    if (peerId.equals(peerName)) {
                        MessageBean messageBean = new MessageBean(peerId, content,false);
                        messageBean.setBackground(getMessageColor(peerId));
                        messageBeanList.add(messageBean);
                        adapter.notifyItemRangeChanged(messageBeanList.size(), 1);
                        recyclerView.scrollToPosition(messageBeanList.size() - 1);
                    } else {
                        Constant.addMessageBean(peerId, content);
                    }
                }
            });
        }
    }

    // api callback: rtm channel event listener
    class MyChannelListener implements RtmChannelListener {

        @Override
        public void onMessageReceived(final RtmMessage message, final RtmChannelMember fromMember) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    String account = fromMember.getUserId();
                    String msg = message.getText();
                    Log.i(TAG, "onMessageReceived account = " + account + " msg = " + msg);
                    MessageBean messageBean = new MessageBean(account, msg, false);
                    messageBean.setBackground(getMessageColor(account));
                    messageBeanList.add(messageBean);
                    adapter.notifyItemRangeChanged(messageBeanList.size(), 1);
                    recyclerView.scrollToPosition(messageBeanList.size() - 1);
                }
            });
        }

        @Override
        public void onMemberJoined(RtmChannelMember member) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    channelMemberCount++;
                    textViewTitle.setText(channelName + "(" + channelMemberCount + ")");
                }
            });
        }

        @Override
        public void onMemberLeft(RtmChannelMember member) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    channelMemberCount--;
                    textViewTitle.setText(channelName + "(" + channelMemberCount + ")");
                }
            });
        }

        @Override
        public void onAttributesUpdated(Map<String, String> attributes) {
            // not supported yet
        }

        @Override
        public void onAttributesDeleted(Map<String, String> attributes) {
            // not supported yet
        }
    }

    private int getMessageColor(String account) {
        for (int i = 0; i < messageBeanList.size(); i++) {
            if (account.equals(messageBeanList.get(i).getAccount())) {
                return messageBeanList.get(i).getBackground();
            }
        }
        return Constant.COLOR_ARRAY[Constant.RANDOM.nextInt(Constant.COLOR_ARRAY.length)];
    }

    private void showToast(final String text) {
        Toast.makeText(MessageActivity.this, text, Toast.LENGTH_SHORT).show();
    }
}
