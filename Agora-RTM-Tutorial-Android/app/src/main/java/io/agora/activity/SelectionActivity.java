package io.agora.activity;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.widget.AppCompatCheckBox;

import io.agora.rtmtutorial.AGApplication;
import io.agora.rtmtutorial.ChatManager;
import io.agora.rtmtutorial.R;
import io.agora.utils.MessageUtil;

public class SelectionActivity extends Activity {
    private static final int CHAT_REQUEST_CODE = 1;

    private TextView mTitleTextView;
    private TextView mChatButton;
    private EditText mNameEditText;

    private boolean mIsPeerToPeerMode = true; // whether peer to peer mode or channel mode\
    private String mTargetName;
    private String mUserId;

    private ChatManager mChatManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_selection);

        mChatManager = AGApplication.the().getChatManager();

        initUIAndData();
    }

    private void initUIAndData() {
        Intent intent = getIntent();
        mUserId = intent.getStringExtra(MessageUtil.INTENT_EXTRA_USER_ID);
        mTitleTextView = findViewById(R.id.selection_title);
        mNameEditText = findViewById(R.id.selection_name);
        mChatButton = findViewById(R.id.selection_chat_btn);
        RadioGroup modeGroup = findViewById(R.id.mode_radio_group);
        modeGroup.setOnCheckedChangeListener((group, checkedId) -> {
            switch (checkedId) {
                case R.id.peer_radio_button:
                    mIsPeerToPeerMode = true;
                    mTitleTextView.setText(getString(R.string.title_peer_msg));
                    mChatButton.setText(getString(R.string.btn_chat));
                    mNameEditText.setHint(getString(R.string.hint_friend));
                    break;
                case R.id.selection_tab_channel:
                    mIsPeerToPeerMode = false;
                    mTitleTextView.setText(getString(R.string.title_channel_message));
                    mChatButton.setText(getString(R.string.btn_join));
                    mNameEditText.setHint(getString(R.string.hint_channel));
                    break;
            }
        });
        RadioButton peerMode = findViewById(R.id.peer_radio_button);
        peerMode.setChecked(true);

        AppCompatCheckBox mOfflineMsgCheck = findViewById(R.id.offline_msg_check);
        mOfflineMsgCheck.setChecked(mChatManager.isOfflineMessageEnabled());
        mOfflineMsgCheck.setOnCheckedChangeListener((buttonView, isChecked) -> mChatManager.enableOfflineMessage(isChecked));
    }

    public void onClickChat(View v) {
        mTargetName = mNameEditText.getText().toString();
        if (mTargetName.equals("")) {
            showToast(getString(mIsPeerToPeerMode ? R.string.account_empty : R.string.channel_name_empty));
        } else if (mTargetName.length() >= MessageUtil.MAX_INPUT_NAME_LENGTH) {
            showToast(getString(mIsPeerToPeerMode ? R.string.account_too_long : R.string.channel_name_too_long));
        } else if (mTargetName.startsWith(" ")) {
            showToast(getString(mIsPeerToPeerMode ? R.string.account_starts_with_space : R.string.channel_name_starts_with_space));
        } else if (mTargetName.equals("null")) {
            showToast(getString(mIsPeerToPeerMode ? R.string.account_literal_null : R.string.channel_name_literal_null));
        } else if (mIsPeerToPeerMode && mTargetName.equals(mUserId)) {
            showToast(getString(R.string.account_cannot_be_yourself));
        } else {
            mChatButton.setEnabled(false);
            jumpToMessageActivity();
        }
    }

    private void jumpToMessageActivity() {
        Intent intent = new Intent(this, MessageActivity.class);
        intent.putExtra(MessageUtil.INTENT_EXTRA_IS_PEER_MODE, mIsPeerToPeerMode);
        intent.putExtra(MessageUtil.INTENT_EXTRA_TARGET_NAME, mTargetName);
        intent.putExtra(MessageUtil.INTENT_EXTRA_USER_ID, mUserId);
        startActivityForResult(intent, CHAT_REQUEST_CODE);
    }

    @Override
    protected void onResume() {
        super.onResume();
        mChatButton.setEnabled(true);
    }

    public void onClickFinish(View v) {
        finish();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == CHAT_REQUEST_CODE) {
            if (resultCode == MessageUtil.ACTIVITY_RESULT_CONN_ABORTED) {
                finish();
            }
        }
    }

    private void showToast(String text) {
        Toast.makeText(this, text, Toast.LENGTH_SHORT).show();
    }
}
