package io.agora.activity;


import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import io.agora.rtm.ErrorInfo;
import io.agora.rtm.IResultCallback;
import io.agora.rtm.RtmClient;
import io.agora.rtmtutorial.AGApplication;
import io.agora.rtmtutorial.R;
import io.agora.rtmtutorial.ChatManager;
import io.agora.utils.Constant;


public class LoginActivity extends Activity {
    private final String TAG = LoginActivity.class.getSimpleName();

    private EditText textAccountName;
    private String account;
    private boolean enableLoginBtnClick = true;

    private ChatManager mChatManager;
    private RtmClient mRtmClient;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        mChatManager = AGApplication.the().getChatManager();
        mRtmClient = mChatManager.getRtmClient();
        textAccountName = (EditText) findViewById(R.id.account_name);
    }

    public void onClickLogin(View v) {
        if (!enableLoginBtnClick) {
            return;
        }

        account = textAccountName.getText().toString();
        if (account.equals("")) {
            showToast(getString(R.string.account_empty));

        } else if (account.length() > Constant.MAX_INPUT_NAME_LENGTH) {
            showToast(getString(R.string.account_too_long));

        } else if (account.startsWith(" ")) {
            showToast(getString(R.string.account_starts_with_space));

        } else if (account.equals("null")) {
            showToast(getString(R.string.account_literal_null));

        } else {
            enableLoginBtnClick = false;
            doLogin();
        }
    }

    // api call: login RTM server
    private void doLogin() {
        mRtmClient.login(null, account, new IResultCallback<Void>() {
            @Override
            public void onSuccess(Void responseInfo) {
                Log.i(TAG, "login success");
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Intent intent = new Intent(LoginActivity.this, SelectionActivity.class);
                        intent.putExtra("account", account);
                        startActivity(intent);
                    }
                });
            }

            @Override
            public void onFailure(ErrorInfo errorInfo) {
                Log.i(TAG, "login failed: " + errorInfo.getErrorCode());
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        enableLoginBtnClick = true;
                        showToast(getString(R.string.login_failed));
                    }
                });
            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
        enableLoginBtnClick = true;
    }

    private void showToast(String text) {
        Toast.makeText(this, text, Toast.LENGTH_SHORT).show();
    }
}
