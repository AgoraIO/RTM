package io.agora.activity;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.IdRes;
import android.view.View;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

import io.agora.rtm.RtmClient;
import io.agora.rtmtutorial.AGApplication;
import io.agora.rtmtutorial.R;
import io.agora.rtmtutorial.ChatManager;
import io.agora.utils.Constant;


public class SelectionActivity extends Activity {
    private final String TAG = SelectionActivity.class.getSimpleName();

    private static final int CHAT_REQUEST_CODE = 1;
    private TextView textViewTitle;
    private TextView textViewButton;
    private EditText edittextName;
    private String otherName;
    private String selfAccount;

    private boolean peerToPeerMode = true; // peer to peer mode, or channel mode
    private boolean enableChatBtnClick = true;

    private ChatManager mChatManager;
    private RtmClient mRtmClient;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_selectchannel);

        mChatManager = AGApplication.the().getChatManager();
        mRtmClient = mChatManager.getRtmClient();

        initUI();
    }

    private void initUI() {
        Intent intent = getIntent();
        selfAccount = intent.getStringExtra("account");

        textViewTitle = (TextView) findViewById(R.id.selection_title);
        edittextName = (EditText) findViewById(R.id.selection_name);
        textViewButton = (TextView) findViewById(R.id.selection_start_btn);
        RadioGroup tabGroup = (RadioGroup) findViewById(R.id.selection_tab_group);
        tabGroup.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup group, @IdRes int checkedId) {
                switch (checkedId) {
                    case R.id.selection_tab_peer:
                        peerToPeerMode = true;
                        break;
                    case R.id.selection_tab_channel:
                        peerToPeerMode = false;
                        break;
                }
                onClickSelectMode();

            }
        });
        RadioButton peerTab = (RadioButton) findViewById(R.id.selection_tab_peer);
        peerTab.setChecked(true);
    }


    public void onClickChat(View v) {
        if (!enableChatBtnClick) {
            return;
        }

        otherName = edittextName.getText().toString();
        if (otherName.equals("")) {
            showToast(getString(peerToPeerMode ? R.string.account_empty : R.string.channel_name_empty));

        } else if (otherName.length() >= Constant.MAX_INPUT_NAME_LENGTH) {
            showToast(getString(peerToPeerMode ? R.string.account_too_long : R.string.channel_name_too_long));

        } else if (otherName.startsWith(" ")) {
            showToast(getString(peerToPeerMode ? R.string.account_starts_with_space : R.string.channel_name_starts_with_space));

        } else if (peerToPeerMode && otherName.equals(selfAccount)) {
            showToast(getString(R.string.account_cannot_be_yourself));

        } else {
            enableChatBtnClick = false;
            jumpToMessageActivity();
        }
    }

    private void jumpToMessageActivity() {
        Intent intent = new Intent(this, MessageActivity.class);
        intent.putExtra("mode", peerToPeerMode);
        intent.putExtra("name", otherName);
        intent.putExtra("selfname", selfAccount);
        startActivityForResult(intent, CHAT_REQUEST_CODE);
    }

    public void onClickSelectMode() {
        textViewTitle.setText(peerToPeerMode ? getString(R.string.title_peer_msg) : getString(R.string.title_channel_message));
        textViewButton.setText(peerToPeerMode ? getString(R.string.btn_chat) : getString(R.string.btn_join));
        edittextName.setHint(peerToPeerMode ? getString(R.string.hint_friend) : getString(R.string.hint_channel));
    }

    @Override
    protected void onResume() {
        super.onResume();
        enableChatBtnClick = true;
    }

    @Override
    public void onBackPressed() {
        mRtmClient.logout(null);
        Constant.cleanMessageListBeanList();
        super.onBackPressed();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == CHAT_REQUEST_CODE) {
            if (resultCode == RESULT_CANCELED) {
                mRtmClient.logout(null);
                Constant.cleanMessageListBeanList();
                finish();
            }
        }
    }

    public void onClickFinish(View v) {
        mRtmClient.logout(null);
        Constant.cleanMessageListBeanList();
        finish();
    }

    private void showToast(String text) {
        Toast.makeText(this, text, Toast.LENGTH_SHORT).show();
    }
}
