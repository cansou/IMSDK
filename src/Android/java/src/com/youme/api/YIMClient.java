package com.youme.api;

import android.content.Context;
import android.os.Handler;
import android.text.TextUtils;
import android.util.Log;

import java.io.UnsupportedEncodingException;
import java.util.HashMap;
import java.util.Map;

import com.google.gson.Gson;
import com.youme.api.callback.YIMEventCallback;
import com.youme.api.internal.*;
import com.youme.im.IMEngine;

/**
 * Created by winnie on 2018/7/30.
 */

public class YIMClient {
    private static boolean DEBUG = false;
    private final static String TAG = YIMClient.class.getSimpleName();
    private static YIMClient mInstance;

    private Thread mThread = null;
    private Handler mHandler = null;

    private YIMEventCallback.ResultCallback<String> m_loginCallback = null;
    private YIMEventCallback.OperationCallback m_logoutCallback = null;
    private Map<String, YIMEventCallback.ResultCallback<ChatRoom>> joinChannelCallbackQueue = new HashMap<String, YIMEventCallback.ResultCallback<ChatRoom>>();
    private Map<String, YIMEventCallback.ResultCallback<ChatRoom>> leaveChannelCallbackQueue = new HashMap<String, YIMEventCallback.ResultCallback<ChatRoom>>();
    private Map<Long, MessageCallbackObject> messageCallbackQueue = new HashMap<Long, MessageCallbackObject>();

    private boolean mIsExit = true;

    public static YIMClient getInstance() {
        if (null == mInstance) {
            mInstance = new YIMClient();
        }
        return mInstance;
    }

    /**
     * 初始化
     *
     * @param context    应用上下文
     * @param appKey     申请的appkey
     * @param secrectKey 申请得到的secretKey
     * @param serverZone IM服务器区域
     */
    public int init(Context context, String appKey, String secrectKey, int serverZone) {
        if (null == context)
            return YIMConstInfo.Errorcode.InvalidContext;
        if (!checkString(appKey)) {
            return YIMConstInfo.Errorcode.InvalidAppKey;
        }
        if (!checkString(secrectKey)) {
            return YIMConstInfo.Errorcode.InvaliddSecretKey;
        }
        int ret = IMEngine.IM_Init(context, appKey, secrectKey, serverZone);
        if (ret == -1) {
            return YIMConstInfo.Errorcode.InitFailed;
        }

        return YIMConstInfo.Errorcode.Success;
    }

    /**
     * 登录IM
     *
     * @param userId   用户ID
     * @param password 用户密码
     * @param token    登录token
     * @param callback 登录回调
     */
    public void login(String userId, String password, String token, final YIMEventCallback.ResultCallback<String> callback) {
        if (callback == null) {
            Log.e(TAG, "login: please set callback interface");
            return;
        }
        m_loginCallback = callback;

        do {
            if (!checkString(userId)) {
                callback.onFailed(YIMConstInfo.Errorcode.InvalidUserId, userId);
                break;
            }

            if (!checkString(password)) {
                callback.onFailed(YIMConstInfo.Errorcode.InvalidPassword, userId);
                break;
            }

            int errcode = IMEngine.IM_Login(userId, password, token);
            if (YIMConstInfo.Errorcode.Success == errcode) {
                mIsExit = false;
                if (null == mHandler) {
                    mHandler = new Handler();
                }
                if (null == mThread) {
                    mThread = new Thread(paresMessage);
                    mThread.start();
                }
            } else {
                callback.onFailed(errcode, userId);
            }
            break;
        } while (true);
    }

    /**
     * 登出IM
     *
     * @param callback 登出回调
     */
    public void logout(final YIMEventCallback.OperationCallback callback) {
        if (callback == null) {
            Log.e(TAG, "logout: please set callback interface");
            return;
        }
        m_logoutCallback = callback;

        int errorcode = IMEngine.IM_Logout();
        if ((YIMConstInfo.Errorcode.Success != errorcode)) {
            callback.onFailed(errorcode);
        }
    }

    /**
     * 加入聊天室
     *
     * @param roomId   频道ID
     * @param callback 加入频道回调
     */
    public void joinChatRoom(String roomId, final YIMEventCallback.ResultCallback<ChatRoom> callback) {
        if (callback == null) {
            Log.e(TAG, "joinChatRoom: please set callback interface");
            return;
        }

        ChatRoom channelInfo = new ChatRoom();
        channelInfo.groupId = roomId;
        if (!checkString(roomId)) {
            callback.onFailed(YIMConstInfo.Errorcode.InvalidRoomId, channelInfo);
            return;
        }

        int errorcode = IMEngine.IM_JoinChatRoom(roomId);
        if (YIMConstInfo.Errorcode.Success == errorcode) {
            boolean ret = addJoinChannelCallbackObj(roomId, callback);
            if (!ret) {
                callback.onFailed(YIMConstInfo.Errorcode.IsWaitingJoin, channelInfo);
            }
        } else {
            callback.onFailed(errorcode, channelInfo);
        }
    }

    /**
     * 退出聊天室
     *
     * @param roomId   频道ID
     * @param callback 退出频道回调
     */
    public void leaveChatRoom(String roomId, YIMEventCallback.ResultCallback<ChatRoom> callback) {
        if (callback == null) {
            Log.e(TAG, "leaveChatRoom: please set callback interface");
            return;
        }

        ChatRoom channelInfo = new ChatRoom();
        channelInfo.groupId = roomId;
        if (!checkString(roomId)) {
            callback.onFailed(YIMConstInfo.Errorcode.InvalidRoomId, channelInfo);
            return;
        }

        int errorcode = IMEngine.IM_LeaveChatRoom(roomId);
        if (YIMConstInfo.Errorcode.Success == errorcode) {
            if (callback != null) {
                boolean ret = addLeaveChannelCallbackObj(roomId, callback);
                if (!ret) {
                    callback.onFailed(YIMConstInfo.Errorcode.IsWaitingLeave, channelInfo);
                }
            }
        } else {
            callback.onFailed(errorcode, channelInfo);
        }
    }

    /**
     * 发送文本消息
     * <p>
     * * @param recvId     消息接收者ID
     * * @param chatType   聊天类型，详见ChatType
     * * @param msgContent 消息内容
     * * @param callback   发送文本消息回调
     * * @return
     */
    public void sendTextMessage(String recvId, int chatType, String msgContent, String attachParam, YIMEventCallback.ResultCallback<SendMessage> callback) {
        if (callback == null) {
            Log.e(TAG, "sendTextMessage: please set callback interface");
            return;
        }

        SendMessage sendMessageInfo = new SendMessage();
        if (!checkString(recvId)) {
            sendMessageInfo.setRequestId(0L);
            callback.onFailed(YIMConstInfo.Errorcode.InvalidReceiver, sendMessageInfo);
            return;
        }

        IMEngine.MessageRequestId messageRequestId = new IMEngine().new MessageRequestId();
        int errcode = IMEngine.IM_SendTextMessage(recvId, chatType, msgContent, attachParam, messageRequestId);
        if (YIMConstInfo.Errorcode.Success == errcode) {
            YIMMessage yimMessage = new YIMMessage();
            yimMessage.setMeesageID(messageRequestId.getId());

            if (callback != null) {
                MessageCallbackObject callbackObject = new MessageCallbackObject(YIMConstInfo.MessageBodyType.TXT, yimMessage, callback);

                boolean ret = addMessageCallbackObj(messageRequestId.getId(), callbackObject);
                if (!ret) {
                    callback.onFailed(YIMConstInfo.Errorcode.IsWaitingSend, sendMessageInfo);
                }
            }
        } else {
            sendMessageInfo.setRequestId(messageRequestId.getId());
            callback.onFailed(errcode, sendMessageInfo);
        }
    }

    private boolean addJoinChannelCallbackObj(String channelID, YIMEventCallback.ResultCallback<ChatRoom> callbackObject) {
        if (!joinChannelCallbackQueue.containsKey(channelID)) {
            joinChannelCallbackQueue.put(channelID, callbackObject);
        } else {
            Log.e("YouMeIM", "channel id is already in joining queue.");
            return false;
        }
        return true;
    }

    private boolean addLeaveChannelCallbackObj(String channelID, YIMEventCallback.ResultCallback<ChatRoom> callbackObject) {
        if (!leaveChannelCallbackQueue.containsKey(channelID)) {
            leaveChannelCallbackQueue.put(channelID, callbackObject);
        } else {
            Log.e("YouMeIM", "channel id is already in leaving queue.");
            return false;
        }
        return true;
    }

    public final static class MessageCallbackObject<T> {
        public T callback;
        public YIMMessage message;
        public int msgType;

        public MessageCallbackObject(int msgType, YIMMessage message, T callback) {
            this.msgType = msgType;
            this.message = message;
            this.callback = callback;
        }
    }

    private boolean addMessageCallbackObj(long requestID, MessageCallbackObject callbackObject) {
        if (!messageCallbackQueue.containsKey(requestID)) {
            messageCallbackQueue.put(requestID, callbackObject);
        } else {
            Log.e("YouMeIM", "message id is already in sending queue.");
            return false;
        }
        return true;
    }

    private boolean checkString(String text) {
        boolean res = true;
        if (TextUtils.isEmpty(text)) {
            res = false;
        }
        return res;
    }

    private Runnable paresMessage = new Runnable() {
        @Override
        public void run() {
            String msg = null;
            while (true) {
                if (mIsExit)
                    break;

                try {
                    byte[] bytes = IMEngine.IM_GetMessage();
                    msg = new String(bytes, "UTF-8"); // please debate what the safest charset should be?
                } catch (UnsupportedEncodingException e) {
                    Log.e(TAG, "Couldn't convert the jbyteArray to jstring");
                }
                if (null == msg) {
                    continue;
                }

                if (DEBUG) {
                    Log.d(TAG, "IM_GetMessage :" + msg);
                }
                handlerMessage(msg);
                IMEngine.IM_PopMessage();
            }
            mThread = null;
        }
    };

    private void handlerMessage(String msg) {
        if (null == msg || msg.equals(""))
            return;
        try {
            Gson gson = new Gson();
            YouMeIMJsonResponse jsonResponse = null;
            if (null != msg) {
                jsonResponse = gson.fromJson(msg, YouMeIMJsonResponse.class);
            } else {
                return;
            }
            switch (jsonResponse.command) {
                case YIMConstInfo.CMD.CMD_LOGIN:
                    try {
                        Login login = gson.fromJson(msg, Login.class);
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success) {
                            mHandler.post(new YIMCallBackProtocolV2(m_loginCallback, "onSuccess", login.getYoumeId()));
                        } else {
                            mHandler.post(new YIMCallBackProtocolV2(m_loginCallback, "onFailed", jsonResponse.errcode, login.getYoumeId()));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                    break;
                case YIMConstInfo.CMD.CMD_LOGOUT:
                    try {
                        mHandler.post(new YIMCallBackProtocolV2(m_logoutCallback, "onSuccess"));
                        mIsExit = true;
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                    break;
                case YIMConstInfo.CMD.CMD_ENTER_ROOM: {
                    ChatRoom joinGroupResponse = gson.fromJson(msg, ChatRoom.class);
                    YIMEventCallback.ResultCallback<ChatRoom> callbackObject = null;
                    boolean finded = joinChannelCallbackQueue.containsKey(joinGroupResponse.groupId);
                    if (finded) {
                        callbackObject = joinChannelCallbackQueue.get(joinGroupResponse.groupId);
                        try {
                            if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success) {
                                mHandler.post(new YIMCallBackProtocolV2(callbackObject, "onSuccess", joinGroupResponse));
                            } else {
                                mHandler.post(new YIMCallBackProtocolV2(callbackObject, "onFailed", jsonResponse.errcode, joinGroupResponse));
                            }
                        } catch (NullPointerException e) {
                            e.printStackTrace();
                        }

                        joinChannelCallbackQueue.remove(joinGroupResponse.groupId);
                    }
                    break;
                }
                case YIMConstInfo.CMD.CMD_LEAVE_ROOM: {
                    ChatRoom joinGroupResponse = gson.fromJson(msg, ChatRoom.class);
                    YIMEventCallback.ResultCallback<ChatRoom> callbackObject = null;
                    boolean finded = leaveChannelCallbackQueue.containsKey(joinGroupResponse.groupId);
                    if (finded) {
                        callbackObject = leaveChannelCallbackQueue.get(joinGroupResponse.groupId);

                        try {
                            if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success) {
                                mHandler.post(new YIMCallBackProtocolV2(callbackObject, "onSuccess", joinGroupResponse));

                            } else {
                                mHandler.post(new YIMCallBackProtocolV2(callbackObject, "onFailed", jsonResponse.errcode, joinGroupResponse));
                            }
                        } catch (NullPointerException e) {
                            e.printStackTrace();
                        }

                        leaveChannelCallbackQueue.remove(joinGroupResponse.groupId);
                    }
                }
                break;
                case YIMConstInfo.CMD.CMD_SEND_MESSAGE_STATUS: {
                    SendMessage sendMessage = gson.fromJson(msg, SendMessage.class);

                    MessageCallbackObject callbackObject = null;
                    boolean finded = messageCallbackQueue.containsKey(sendMessage.getRequestId());
                    if (finded) {
                        callbackObject = messageCallbackQueue.get(sendMessage.getRequestId());
                        if ((callbackObject != null) && (callbackObject.callback != null)) {
                            try {
                                if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success) {
                                    mHandler.post(new YIMCallBackProtocolV2(callbackObject.callback, "onSuccess", sendMessage));

                                } else {
                                    mHandler.post(new YIMCallBackProtocolV2(callbackObject.callback, "onFailed", jsonResponse.errcode, sendMessage));
                                }
                            } catch (NullPointerException e) {
                                e.printStackTrace();
                            }
                        }
                        messageCallbackQueue.remove(sendMessage.getRequestId());
                    }
                }
                break;
                default:
                    break;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}

