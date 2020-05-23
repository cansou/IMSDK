package com.touchtech.imdemo_android;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;

import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.youme.api.YIMClient;
import com.youme.api.YIMConstInfo;
import com.youme.api.callback.YIMEventCallback;
import com.youme.api.internal.ChatRoom;

public class MainActivity extends AppCompatActivity {
    private final String TAG = MainActivity.this.getClass().getName();

    private static String strAppKey =  "YOUME5BE427937AF216E88E0F84C0EF148BD29B691556";
    private static String strSecrect = "y1sepDnrmgatu/G8rx1nIKglCclvuA5tAvC0vXwlfZKOvPZfaUYOTkfAdUUtbziW8Z4HrsgpJtmV/RqhacllbXD3abvuXIBlrknqP+Bith9OHazsC1X96b3Inii6J7Und0/KaGf3xEzWx/t1E1SbdrbmBJ01D1mwn50O/9V0820BAAE=";

    private Button login;
    private Button logout;
    private Button sendText;
    private Button sendAudio;
    private EditText userId;
    private EditText channelId;
    private EditText recvId;
    private TextView tips;
    private String mDefaultSendPassword = "123456";

    private boolean isLogined;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initCtrl();
        initYouMeIMEngine();
    }

    void initCtrl() {
        login = findViewById(R.id.login);
        login.setOnClickListener(mOnClickListener);
        logout = findViewById(R.id.logout);
        logout.setOnClickListener(mOnClickListener);
        sendText = findViewById(R.id.sendText);
        sendText.setOnClickListener(mOnClickListener);
        sendAudio = findViewById(R.id.sendAudio);
        sendAudio.setOnClickListener(mOnClickListener);

        userId = findViewById(R.id.userId);
        recvId = findViewById(R.id.recvId);
        channelId = findViewById(R.id.channelId);
        tips = findViewById(R.id.tips);
    }

    private void initYouMeIMEngine() {
        YIMClient.getInstance().init(this, strAppKey, strSecrect, YIMConstInfo.ServerZone.China);
    }

    private void login() {
        String mSendId = userId.getText().toString();
        YIMClient.getInstance().login(mSendId, mDefaultSendPassword, "", new YIMEventCallback.ResultCallback<String>() {
            @Override
            public void onSuccess(String userId) {
                isLogined = true;
                showSend("用户: " + userId + " 登录成功");
                joinChatRoom();
            }

            @Override
            public void onFailed(int errorCode, String userId) {
                showSend("用户:" + userId + "登录失败:" + errorCode);
            }
        });
    }

    private void joinChatRoom() {
        String mRoomId = channelId.getText().toString();
        YIMClient.getInstance().joinChatRoom(mRoomId, new YIMEventCallback.ResultCallback<ChatRoom>() {
            @Override
            public void onSuccess(ChatRoom chatRoom) {
                showSend("进入频道: " + chatRoom.groupId);
            }

            @Override
            public void onFailed(int errorCode, ChatRoom chatRoom) {
                showSend("进入频道失败: " + chatRoom.groupId);
            }
        });
    }

    private void logout() {

    }

    private void sendText() {
        String mRecvId = recvId.getText().toString();
//        YIMClient.getInstance().
    }

    private void sendAudio() {
        String mRecvId = recvId.getText().toString();

    }

    private void showSend(String text) {
        tips.setText(text);
    }

    private OnClickListener mOnClickListener = new OnClickListener() {
        @Override
        public void onClick(View v) {
            switch (v.getId()) {
                case R.id.login:
                    login();
                    break;
                case R.id.logout:
                    logout();
                    break;
                case R.id.sendText:
                    sendText();
                    break;
                case R.id.sendAudio:
                    sendAudio();
                    break;
            }
        }
    };
}
