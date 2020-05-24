package com.youme.imsdk;

import android.content.Context;
import android.os.Handler;
import android.speech.tts.TextToSpeech;
import android.text.TextUtils;
import android.util.Base64;
import android.util.Log;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.youme.im.IMEngine;
import com.youme.imsdk.callback.YIMEventCallback;
import com.youme.imsdk.internal.AccusationResult;
import com.youme.imsdk.internal.BlockUserInfo;
import com.youme.imsdk.internal.BlockUserList;
import com.youme.imsdk.internal.ChatRoom;
import com.youme.imsdk.internal.Contacts;
import com.youme.imsdk.internal.Download;
import com.youme.imsdk.internal.DownloadUrl;
import com.youme.imsdk.internal.GeographyLocation;
import com.youme.imsdk.internal.Login;
import com.youme.imsdk.internal.MicrophoneStatus;
import com.youme.imsdk.internal.MsgReadStatus;
import com.youme.imsdk.internal.NewMessageNotifyObj;
import com.youme.imsdk.internal.NoticeCancelInfo;
import com.youme.imsdk.internal.NoticeInfo;
import com.youme.imsdk.internal.OnPlayCompleteNotify;
import com.youme.imsdk.internal.PhotoUrlInfo;
import com.youme.imsdk.internal.RecognizeSpeechText;
import com.youme.imsdk.internal.ReconnectResult;
import com.youme.imsdk.internal.RecvMessage;
import com.youme.imsdk.internal.RelativeLocationInfo;
import com.youme.imsdk.internal.RoomHistoryMessage;
import com.youme.imsdk.internal.RoomHistoryMsgInfo;
import com.youme.imsdk.internal.RoomInfo;
import com.youme.imsdk.internal.SendMessage;
import com.youme.imsdk.internal.SendVoiceMsgInfo;
import com.youme.imsdk.internal.SpeechMessageInfo;
import com.youme.imsdk.internal.TranlateTextInfo;
import com.youme.imsdk.internal.UploadProgress;
import com.youme.imsdk.internal.UserChatRoom;
import com.youme.imsdk.internal.UserDistanceInfo;
import com.youme.imsdk.internal.UserInfoString;
import com.youme.imsdk.internal.UserProfileInfo;
import com.youme.imsdk.internal.UserStatusInfo;
import com.youme.imsdk.internal.VoiceInfo;
import com.youme.imsdk.internal.VolumeInfo;
import com.youme.imsdk.internal.YIMBlackFriendInfo;
import com.youme.imsdk.internal.YIMFriendCommon;
import com.youme.imsdk.internal.YIMFriendDealResult;
import com.youme.imsdk.internal.YIMFriendListInfo;
import com.youme.imsdk.internal.YIMFriendRequestInfoList;
import com.youme.imsdk.internal.YIMFriendUserID;
import com.youme.imsdk.internal.YIMUserBriefInfo;
import com.youme.imsdk.internal.YIMUserBriefInfoList;
import com.youme.imsdk.internal.YouMeIMJsonResponse;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;


/**
 * Created by winnie on 2018/7/30.
 */

public class YIMClient {

    private final static int CMD_UNKNOW = 0;
    private final static int CMD_LOGIN = 1;
    private final static int CMD_HEARTBEAT = 2;
    private final static int CMD_LOGOUT = 3;
    private final static int CMD_ENTER_ROOM = 4;
    private final static int CMD_LEAVE_ROOM = 5;
    private final static int CMD_SND_TEXT_MSG = 6;
    private final static int CMD_SND_VOICE_MSG = 7;
    private final static int CMD_SND_FILE_MSG = 8;
    private final static int CMD_GET_MSG = 9;
    private final static int CMD_GET_UPLOAD_TOKEN = 10;
    private final static int CMD_KICK_OFF = 11;
    private final static int CMD_SND_BIN_MSG = 12;
    private final static int CMD_RELOGIN = 13;
    private final static int CMD_CHECK_ONLINE = 14;
    private final static int CMD_SND_GIFT_MSG = 15;
    private final static int CMD_GET_ROOM_HISTORY_MSG = 16;
    private final static int CMD_GET_USR_INFO = 17;
    private final static int CMD_UPDATE_USER_INFO = 18;
    private final static int CMD_SND_TIPOFF_MSG = 19;
    private final static int CMD_GET_TIPOFF_MSG = 20;
    private final static int CMD_GET_DISTRICT = 21;
    private final static int CMD_GET_PEOPLE_NEARBY = 22;
    private final static int CMD_QUERY_NOTICE = 23;
    private final static int CMD_SET_MASK_USER_MSG = 24;		// 屏蔽用户
    private final static int CMD_GET_MASK_USER_MSG = 25;		// 获取屏蔽用户
    private final static int CMD_CLEAN_MASK_USER_MSG = 26;		// 解除所有屏蔽用户
    private final static int CMD_GET_ROOM_INFO = 27;			// 获取房间信息(人数)
    private final static int CMD_LEAVE_ALL_ROOM = 28;           // 绂诲紑鎵�鏈夋埧闂�

    private final static int CMD_GET_SESSION_HISTORY_MSG = 30;
    private final static int CMD_GET_FORBID_RECORD = 31;

    private final static int CMD_QUERY_USER_PROFILE = 37;           // 查询用户资料
    private final static int CMD_UPDATE_USER_PROFILE = 38;          // 更新用户资料
    private final static int CMD_UPDATE_ONLINE_STATUS = 39;         // 更新在线状态
    private final static int CMD_FIND_FRIEND_BY_ID = 40;		    // 按ID查找好友
    private final static int CMD_FIND_FRIEND_BY_NICKNAME = 41; 	    // 按昵称查找好友
    private final static int CMD_REQUEST_ADD_FRIEND = 42; 		    // 请求添加好友
    private final static int CMD_FRIND_NOTIFY = 44;				    // 好友请求通知
    private final static int CMD_DELETE_FRIEND = 45;			    // 删除好友
    private final static int CMD_BLACK_FRIEND = 46;				    // 拉黑好友
    private final static int CMD_UNBLACK_FRIEND = 47;			    // 解除黑名单
    private final static int CMD_DEAL_ADD_FRIEND = 48;			    // 好友验证
    private final static int CMD_QUERY_FRIEND_LIST = 49;		    // 获取好友列表
    private final static int CMD_QUERY_BLACK_FRIEND_LIST = 50;	    // 获取黑名单列表
    private final static int CMD_QUERY_FRIEND_REQUEST_LIST = 51;    // 获取好友验证消息列表
    private final static int CMD_UPDATE_FRIEND_RQUEST_STATUS = 52;	// 更新好友请求状态
    private final static int CMD_RELATION_CHAIN_HEARTBEAT = 53;		// 关系链心跳

    private final static int CMD_HXR_USER_INFO_CHANGE_NOTIFY = 74;	// 用户信息变更通知
    private final static int CMD_UPLOAD_PROGRESS = 75;	// 文件上传显示进度
    private final static int CMD_SET_READ_STATUS = 76; //对端已查看消息

    //服务器通知
    private final static int NOTIFY_LOGIN = 10001;
    private final static int NOTIFY_PRIVATE_MSG = 10002;
    private final static int NOTIFY_ROOM_MSG = 10003;

    //客户端(C接口使用)
    private final static int CMD_DOWNLOAD = 20001;
    private final static int CMD_SEND_MESSAGE_STATUS = 20002;
    private final static int CMD_RECV_MESSAGE = 20003;
    private final static int CMD_STOP_AUDIOSPEECH = 20004;
    private final static int CMD_QUERY_HISTORY_MESSAGE = 20005;
    private final static int CMD_GET_RENCENT_CONTACTS = 20006;
    private final static int CMD_RECEIVE_MESSAGE_NITIFY = 20007;
    private final static int CMD_QUERY_USER_STATUS = 20008;
    private final static int CMD_AUDIO_PLAY_COMPLETE= 20009;
    private final static int CMD_STOP_SEND_AUDIO = 20010;

    private final static int CMD_TRANSLATE_COMPLETE = 20011;
    private final static int CMD_DOWNLOAD_URL = 20012;
    private final static int CMD_GET_MICROPHONE_STATUS = 20013;
    private final static int CMD_USER_ENTER_ROOM = 20014;
    private final static int CMD_USER_LEAVE_ROOM = 20015;
    private final static int CMD_RECV_NOTICE = 20016;
    private final static int CMD_CANCEL_NOTICE = 20017;

    private final static int CMD_GET_SPEECH_TEXT = 20018;  		// 仅需要语音的文字识别内容
    private final static int CMD_GET_RECONNECT_RESULT = 20019; 	// 重连结果
    private final static int CMD_START_RECONNECT = 20020;  		// 开始重连
    private final static int CMD_RECORD_VOLUME = 20021;   		// 音量
    private final static int CMD_GET_DISTANCE = 20022;			// 获取指定用户距离
    private final static int CMD_REQUEST_ADD_FRIEND_NOTIFY = 20023;	// 请求添加好友通知
    private final static int CMD_ADD_FRIENT_RESULT_NOTIFY = 20024;	// 添加好友结果通知
    private final static int CMD_BE_ADD_FRIENT = 20025;				// 被好友添加通知
    private final static int CMD_BE_DELETE_FRIEND_NOTIFY = 20026;	// 被删除好友通知
    private final static int CMD_BE_BLACK_FRIEND_NOTIFY = 20027;	// 被拉黑好友通知
    private final static int CMD_GET_USER_PROFILE = 20028;          //关系链-查询用户信息
    private final static int CMD_SET_USER_PROFILE = 20029;          //关系链-设置用户信息
    private final static int CMD_SET_USER_PHOTO = 20030;            //关系链-设置头像
    private final static int CMD_SWITCH_USER_STATE = 20031;         //关系链-切换用户在线状态

    private static boolean DEBUG = false;
    private final static String TAG = YIMService.class.getSimpleName();
    private boolean mIsExit = true;
    private static YIMClient mInstance;

    private Thread mThread = null;
    private Handler mHandler = null;

    private YIMEventCallback.ResultCallback<String> m_loginCallback = null;
    private YIMEventCallback.OperationCallback m_logoutCallback = null;
    private YIMEventCallback.ResultCallback<RoomInfo> m_roomInfoCallback = null;

    private YIMEventCallback.OperationCallback m_leaveAllCallback = null;
    private YIMEventCallback.OperationCallback m_setprofileCallback = null;
    private YIMEventCallback.OperationCallback m_switchStatusCallback = null;
    private YIMEventCallback.OperationCallback m_setPermissionCallback = null;

    private YIMEventCallback.UserJoinLeaveChannelCallback m_userJoinLeaveCallback = null;
    private YIMEventCallback.AutoDownloadVoiceCallback m_autoDownloadCallback = null;
    private YIMEventCallback.KickOffCallback m_kickOffCallback = null;
    private YIMEventCallback.MessageEventCallback m_msgEventCallback = null;
    private YIMEventCallback.UpdateReadStatusCallback m_updateReadStatusCallback = null;
    private YIMEventCallback.ResultCallback<RoomHistoryMsgInfo> m_roomMsgCallback = null;
    private YIMEventCallback.ResultCallback<ArrayList<ContactsSessionInfo>> m_historyContactCallback = null;
    private YIMEventCallback.ResultCallback<YIMHistoryMessage> m_localHistoryMsgCallback = null;
    private YIMEventCallback.ResultCallback<YIMExtraUserInfo> m_userInfoCallback = null;
    private YIMEventCallback.ResultCallback<String> m_playCompleteCallback = null;
    private YIMEventCallback.ResultCallback<UserStatusInfo> m_userStatusCallback = null;
    private YIMEventCallback.ResultCallback<TranlateTextInfo> m_translateTextCallback = null;
    private YIMEventCallback.ResultCallback<BlockUserInfo> m_blockUserCallback = null;
    private YIMEventCallback.OperationCallback m_unBlockUserCallback = null;
    private YIMEventCallback.ResultCallback<ArrayList<String>> m_getBlockUsersCallback = null;
    private YIMEventCallback.GetLocationCallback m_getLocationCallback = null;
    private YIMEventCallback.ResultCallback<RelativeLocationInfo> m_getNearbyObjCallback = null;
    private YIMEventCallback.ResultCallback<UserDistanceInfo> m_getDistanceCallback = null;
    private YIMEventCallback.GetMicStatusCallback m_getMicStatusCallback = null;
    private YIMEventCallback.AccusationResultCallback m_accusationCallback = null;

    private YIMEventCallback.NoticeCallback m_noticeCallback = null;
    private YIMEventCallback.ReconnectCallback m_reconnectCallback = null;

    private YIMEventCallback.ResultCallback<ArrayList<YIMForbiddenSpeakInfo>> m_forbidSpeakInfoCallback = null;

    private YIMEventCallback.UserProfileChangeCallback m_userProfileChangeCallback = null;
    private YIMEventCallback.FriendNotifyCallback m_friendNotifyCallback = null;

    private YIMEventCallback.ResultCallback<UserProfileInfo> m_userProfileCallback = null;
    private YIMEventCallback.ResultCallback<String> m_photoUrlCallback = null;
    private YIMEventCallback.ResultCallback<ArrayList<YIMUserBriefInfo>> m_findUserCallback = null;
    private YIMEventCallback.ResultCallback<String> m_requestAddCallback = null;
    private YIMEventCallback.ResultCallback<YIMFriendDealResult> m_dealAddCallback = null;
    private YIMEventCallback.ResultCallback<String> m_deleteFriendCallback = null;
    private YIMEventCallback.ResultCallback<YIMBlackFriendInfo> m_blackFriendCallback = null;
    private YIMEventCallback.ResultCallback<YIMFriendListInfo> m_queryFriendsCallback = null;
    private YIMEventCallback.ResultCallback<YIMFriendRequestInfoList> m_requestListCallback = null;

    private Map<String, YIMEventCallback.ResultCallback<ChatRoom>> joinChannelCallbackQueue = new HashMap<String, YIMEventCallback.ResultCallback<ChatRoom>>();
    private Map<String, YIMEventCallback.ResultCallback<ChatRoom>> leaveChannelCallbackQueue = new HashMap<String, YIMEventCallback.ResultCallback<ChatRoom>>();

    private Map<Long,MessageCallbackObject> messageCallbackQueue = new HashMap<Long, MessageCallbackObject>();
    private Map<Long,YIMEventCallback.ShowUploadFileProgressCallback> uploadProgressCallbackQueue = new HashMap<Long, YIMEventCallback.ShowUploadFileProgressCallback>();
    private YIMMessage lastRecordMsg = null;

    private Map<Long,YIMEventCallback.DownloadFileCallback> downloadMsgCallbackQueue = new HashMap<Long,YIMEventCallback.DownloadFileCallback>();

    private Map<Long, YIMEventCallback.SpeechEventCallback> uploadSpeechCallbackQueue = new HashMap<Long, YIMEventCallback.SpeechEventCallback>();
    private SpeechMessageInfo lastSpeechMsg = null;

    private Map<String, YIMEventCallback.DownloadByUrlCallback> downloadByUrlCallbackQueue = new HashMap<String, YIMEventCallback.DownloadByUrlCallback>();

    private String m_currentUserID = "";
    private String m_downloadDir = "";

    public static YIMClient getInstance() {
        if (null == mInstance) {
            mInstance = new YIMClient();
        }
        return mInstance;
    }

    /**
     * 设置用户进出房间通知监听
     * @param 用户进出房间通知接口
     */
    public void registerUserJoinLeaveCallback(YIMEventCallback.UserJoinLeaveChannelCallback callback){
        m_userJoinLeaveCallback = callback;
    }
    public void unRegisterUserJoinLeaveCallback(){
        m_userJoinLeaveCallback = null;
    }

    public void registerUpdateReadStatusCallback(YIMEventCallback.UpdateReadStatusCallback callback){
        m_updateReadStatusCallback = callback;
        IMEngine.setUpdateReadStatusCallbackFlag(true);
    }
    public void unRegisterUpdateReadStatusCallback(){
        m_updateReadStatusCallback = null;
    }

    /**
     * 设置用户被踢通知监听
     * @param 用户被踢通知接口
     */
    public void registerKickOffCallback(YIMEventCallback.KickOffCallback callback){
        m_kickOffCallback = callback;
    }
    public void unRegisterKickOffCallback(){
        m_kickOffCallback = null;
    }

    /**
     * 设置消息相关回调
     * @param 消息相关回调接口
     */
    public void registerMsgEventCallback(YIMEventCallback.MessageEventCallback callback){
        m_msgEventCallback = callback;
    }
    public void unRegisterMsgEventCallback(){
        m_msgEventCallback = null;
    }

    /**
     * 设置自动下载回调
     * @param 自动下载回调接口
     */
    public void registerAutoDownloadCallback(YIMEventCallback.AutoDownloadVoiceCallback callback){
        m_autoDownloadCallback = callback;
    }
    public void unRegisterAutoDownloadCallback(){
        m_autoDownloadCallback = null;
    }

    /**
     * 设置lbs信息回调
     * @param lbs回调接口
     */
    public void registerGetLocationCallback(YIMEventCallback.GetLocationCallback callback) {
        m_getLocationCallback = callback;
    }
    public void unregisterGetLocationCallback(){
        m_getLocationCallback = null;
    }

    /**
     * 设置举报结果回调
     * @param 举报结果回调接口
     */
    public void registerAccusationCallback(YIMEventCallback.AccusationResultCallback callback) {
        m_accusationCallback = callback;
    }
    public void unRegisterAccusationCallback(){
        m_accusationCallback = null;
    }

    /**
     * 设置公告回调
     * @param 公告回调接口
     */
    public void registerNoticeCallback(YIMEventCallback.NoticeCallback callback){
        m_noticeCallback = callback;
    }
    public void unRegisterNoticeCallback(){
        m_noticeCallback = null;
    }

    /**
     * 设置重连回调
     * @param 重连回调接口
     */
    public void registerReconnectCallback(YIMEventCallback.ReconnectCallback callback){
        m_reconnectCallback = callback;
    }
    public void unRegisterReconnectCallback(){
        m_reconnectCallback = null;
    }

    /**
     * 设置用户信息变更监听
     * @param 用户信息变更接口
     */
    public void registerUserProfileChangeCallback(YIMEventCallback.UserProfileChangeCallback callback){
        m_userProfileChangeCallback = callback;
    }
    public void unRegisterUserProfileChangeCallback(){
        m_userProfileChangeCallback = null;
    }

    /**
     * 设置好友相关通知监听
     * @param 好友相关通知接口
     */
    public void registerFriendNotifyCallback(YIMEventCallback.FriendNotifyCallback callback){
        m_friendNotifyCallback = callback;
    }
    public void unRegisterFriendNotifyCallback(){
        m_friendNotifyCallback = null;
    }

    /**
     * 初始化
     * @param context 应用上下文
     * @param appKey 申请的appkey
     * @param secrectKey 申请得到的secretKey
     * @param serverZone IM服务器区域
     */
    public int init(Context context, String appKey, String secrectKey, int serverZone) {
        if (null == context)
            return YIMConstInfo.Errorcode.InvalidContext;
        if (!checkString("AppKey", appKey)){
            return YIMConstInfo.Errorcode.InvalidAppKey;
        }
        if (!checkString("SecrectKey", secrectKey)) {
            return YIMConstInfo.Errorcode.InvaliddSecretKey;
        }
        int ret = IMEngine.IM_Init(context, appKey, secrectKey, serverZone);
        if (ret == -1){
            return YIMConstInfo.Errorcode.InitFailed;
        }

        return YIMConstInfo.Errorcode.Success;
    }

    private static void initEngine(Context context) {
        IMEngine.init(context);
    }

//    public void setMode(int mode ){
//        IMEngine.IM_SetMode(mode);
//    }

    /**
     * 登录IM
     * @param userId 用户ID
     * @param password 用户密码
     * @param token 登录token
     * @param callback 登录回调
     */
    public void login(String userId, String password,String token, final YIMEventCallback.ResultCallback<String> callback) {
        m_loginCallback = callback;

        do {
            if (!checkString("UserId", userId)){
                callback.onFailed(YIMConstInfo.Errorcode.InvalidUserId,userId);
                break;
            }

            if (!checkString("Password", password)){
                callback.onFailed(YIMConstInfo.Errorcode.InvalidPassword,userId);
                break;
            }

            int errcode = IMEngine.IM_Login(userId, password,token);
            if (DEBUG)
                Log.d(TAG, "errcode:" + errcode + ", mThread is null" + (null == mThread));
            if (YIMConstInfo.Errorcode.Success == errcode) {
                mIsExit = false;

                if (null == mHandler) {
                    mHandler = new Handler();
                }
                if (null == mThread) {
                    mThread = new Thread(paresMessage);
                    mThread.start();
                }
            }else {
                callback.onFailed(errcode,userId);
            }
            break;
        }while(true);
    }

    /**
     * 登出IM
     * @param callback 登出回调
     */
    public void logout(final YIMEventCallback.OperationCallback callback) {
        m_logoutCallback = callback;
        int errorcode = IMEngine.IM_Logout();
        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            callback.onFailed(errorcode);
        }
    }

    /**
     * 加入聊天室
     * @param roomId 频道ID
     * @param callback 加入频道回调
     */
    public void joinChatRoom(String roomId, final YIMEventCallback.ResultCallback<ChatRoom> callback) {

        ChatRoom channelInfo = new ChatRoom();
        channelInfo.groupId = roomId;

        if (!checkString("RoomID", roomId)){
            callback.onFailed(YIMConstInfo.Errorcode.InvalidRoomId,channelInfo);
            return;
        }
        int errorcode = IMEngine.IM_JoinChatRoom(roomId);
        if (YIMConstInfo.Errorcode.Success == errorcode){
            if (callback != null) {
                boolean ret = addJoinChannelCallbackObj(roomId, callback);

                if (!ret){
                    callback.onFailed(YIMConstInfo.Errorcode.IsWaitingJoin,channelInfo);
                }
            }
        }else {
            if (callback != null) {
                callback.onFailed(errorcode,channelInfo);
            }
        }
    }

    /**
     * 退出聊天室
     * @param roomId 频道ID
     * @param callback 退出频道回调
     */
    public void leaveChatRoom(String roomId,YIMEventCallback.ResultCallback<ChatRoom> callback) {

        ChatRoom channelInfo = new ChatRoom();
        channelInfo.groupId = roomId;

        if (!checkString("RoomID", roomId)){
            callback.onFailed(YIMConstInfo.Errorcode.InvalidRoomId,channelInfo);
            return;
        }
        int errorcode = IMEngine.IM_LeaveChatRoom(roomId);
        if (YIMConstInfo.Errorcode.Success == errorcode){
            if (callback != null) {
                boolean ret = addLeaveChannelCallbackObj(roomId, callback);

                if (!ret){
                    callback.onFailed(YIMConstInfo.Errorcode.IsWaitingLeave,channelInfo);
                }
            }
        }else {
            if (callback != null) {
                callback.onFailed(errorcode,channelInfo);
            }
        }
    }

    /**
     * 离开所有频道
     * @param callback 离开所有频道回调
     */
    public void leaveAllChatRooms(YIMEventCallback.OperationCallback callback) {
        m_leaveAllCallback = callback;
        int errorcode = IMEngine.IM_LeaveAllChatRooms();

        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            callback.onFailed(errorcode);
        }
    }

   /**
	* 功能：获取频道成员数量
	* @param chatRoomID：频道ID(已成功加入该频道)
	* @param callback 获取频道成员数量回调
	*/
    public void getRoomMemberCount(String roomId,YIMEventCallback.ResultCallback<RoomInfo> callback) {
        m_roomInfoCallback = callback;
        RoomInfo roomInfo = new RoomInfo();
        roomInfo.roomID = roomId;
        roomInfo.memberCount = 0;
        int errorcode = IMEngine.IM_GetRoomMemberCount(roomId);

        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            callback.onFailed(errorcode,roomInfo);
        }
    }


    /**
     * 发送文本消息
     * @param recvId 消息接收者ID
     * @param chatType 聊天类型，详见ChatType
     * @param msgContent 消息内容
     * @param callback 发送文本消息回调
     * @return
     */
    public void sendTextMessage(String recvId, int chatType, String msgContent, String attachParam, YIMEventCallback.ResultCallback<SendMessage> callback) {

        SendMessage sendMessageInfo = new SendMessage();
        sendMessageInfo.setSendTime(0);
        sendMessageInfo.setIsForbidRoom(false);
        sendMessageInfo.setEndTime(0L);

        if (!checkString("ReceiverID", recvId)){
            sendMessageInfo.setRequestId(0L);
            callback.onFailed(YIMConstInfo.Errorcode.InvalidReceiver, sendMessageInfo);
            return;
        }else{
            IMEngine.MessageRequestId messageRequestId = new IMEngine().new MessageRequestId();
            int errcode = IMEngine.IM_SendTextMessage(recvId, chatType, msgContent, attachParam, messageRequestId);

            if (YIMConstInfo.Errorcode.Success == errcode){
                YIMMessage yimMessage = new YIMMessage();
                yimMessage.setChatType(chatType);
                yimMessage.setMeesageID(messageRequestId.getId());
                yimMessage.setSenderID(m_currentUserID);
                yimMessage.setReceiveID(recvId);
                yimMessage.setMessageType(YIMConstInfo.MessageBodyType.TXT);

                YIMMessageBodyText txtMsg = new YIMMessageBodyText();
                txtMsg.setMessageContent(msgContent);
                txtMsg.setAttachParam(attachParam);
                yimMessage.setMessageBody(txtMsg);

                if (callback != null){
                    MessageCallbackObject callbackObject = new MessageCallbackObject(YIMConstInfo.MessageBodyType.TXT,yimMessage,callback);

                    boolean ret = addMessageCallbackObj(messageRequestId.getId(),callbackObject);
                    if (!ret){
                        callback.onFailed(YIMConstInfo.Errorcode.IsWaitingSend,sendMessageInfo);
                    }
                }
            }else {
                if (callback != null){
                    sendMessageInfo.setRequestId(messageRequestId.getId());
                    callback.onFailed(errcode,sendMessageInfo);
                }
            }
        }
    }

    /**
     * 当用户点击聊天框，通知对端消息已读
     * @param sendId 对端发送消息的userId
     * @param chatType 聊天类型，详见ChatType
     * @param msgId 最新消息的msgId
     */
    public void sendMessageReadStatus(String sendId, int chatType, long msgId) {
        if (!checkString("SendId", sendId)){
            Log.d(TAG, "sendMessageReadStatus: recvId is empty");
            return;
        }else {
            Log.d(TAG, "sendMessageReadStatus: bruce >>> 确认消息已读");
            IMEngine.IM_SendMessageReadStatus(sendId, chatType, msgId);
        }
    }

    /**
     * 开始录音,带语音转文字识别
     * @param recvId 消息接收者ID
     * @param chatType 聊天类型，详见ChatType
     * @param extraText 语音消息附带信息
     * @param recognizeText 是否带文字识别
     * @param IsOpenOnlyRecognizeText 是否开启仅识别语音文本，不发送语音消息
     * @return YIMClient.MessageSendStatus
     */
    public MessageSendStatus startRecordAudioMessage(String recvId, int chatType, String extraText, boolean recognizeText, boolean IsOpenOnlyRecognizeText) {
        MessageSendStatus status = new MessageSendStatus();
        if (!checkString("ReceiverID", recvId))
        {
            status.setMessageId(0);
            status.setErrorCode(YIMConstInfo.Errorcode.InvalidReceiver);
        }
        else
        {
            int errcode = YIMConstInfo.Errorcode.Fail;
            IMEngine.MessageRequestId messageRequestId = new IMEngine().new MessageRequestId();

            if (recognizeText) {

                IMEngine.IM_SetOnlyRecognizeSpeechText(IsOpenOnlyRecognizeText);

                errcode = IMEngine.IM_SendAudioMessage(recvId, chatType, messageRequestId);
            }else {
                errcode = IMEngine.IM_SendOnlyAudioMessage(recvId, chatType, messageRequestId);
            }

            status.setMessageId(messageRequestId.getId());
            status.setErrorCode(errcode);

            if(YIMConstInfo.Errorcode.Success == errcode) {
                lastRecordMsg = new YIMMessage();

                lastRecordMsg.setSenderID(m_currentUserID);
                lastRecordMsg.setReceiveID(recvId);
                lastRecordMsg.setChatType(chatType);

                YIMMessageBodyAudio audioMsg = new YIMMessageBodyAudio();
                audioMsg.setParam(extraText);
                lastRecordMsg.setMessageBody(audioMsg);
                lastRecordMsg.setMeesageID(messageRequestId.getId());
            }
        }
        return status;
    }

    /**
     * 停止录音并发送
     * @param callback 发送语音消息回调
     */
    public void stopAndSendAudioMessage(YIMEventCallback.AudioMsgEventCallback callback) {

        SendVoiceMsgInfo voiceMsgInfo = new SendVoiceMsgInfo();
        voiceMsgInfo.setRequestId(0L);
        voiceMsgInfo.setText("");
        voiceMsgInfo.setLocalPath("");
        voiceMsgInfo.setDuration(0);
        voiceMsgInfo.setSendTime(0);
        voiceMsgInfo.setIsForbidRoom(false);
        voiceMsgInfo.setReasonType(-1);
        voiceMsgInfo.setEndTime(0L);

        if (lastRecordMsg != null){
            YIMMessageBodyAudio audioMsg = (YIMMessageBodyAudio) lastRecordMsg.getMessageBody();

            int errorcode = IMEngine.IM_StopAudioMessage(audioMsg.getParam());

            voiceMsgInfo.setRequestId(lastRecordMsg.getMessageID());

            if (YIMConstInfo.Errorcode.Success == errorcode){
                YIMMessage yimMessage = new YIMMessage();
                yimMessage.setChatType(lastRecordMsg.getChatType());
                yimMessage.setMeesageID(lastRecordMsg.getMessageID());
                yimMessage.setSenderID(m_currentUserID);
                yimMessage.setReceiveID(lastRecordMsg.getReceiveID());
                yimMessage.setMessageType(YIMConstInfo.MessageBodyType.Voice);

                YIMMessageBodyAudio audioMsg2 = new YIMMessageBodyAudio();
                audioMsg2.setParam(audioMsg.getParam());
                yimMessage.setMessageBody(audioMsg2);

                if (callback != null){
                    MessageCallbackObject callbackObject = new MessageCallbackObject(YIMConstInfo.MessageBodyType.Voice,yimMessage,callback);

                    boolean ret = addMessageCallbackObj(lastRecordMsg.getMessageID(),callbackObject);
                    if (!ret){
                        callback.onSendAudioMessageStatus (YIMConstInfo.Errorcode.IsWaitingSend, voiceMsgInfo);
                    }
                }

            }else {
                if (callback != null){
                    callback.onSendAudioMessageStatus(errorcode, voiceMsgInfo);
                }
            }

            lastRecordMsg = null;
        }else {
            Log.e ("YouMeIM","Has no start record!");
            if (callback != null) {
                callback.onSendAudioMessageStatus(YIMConstInfo.Errorcode.PTT_NotStartRecord, voiceMsgInfo);
            }
        }
    }

    /**
     * 取消录音
     * @return
     */
    public int cancleAudioMessage() {
        return IMEngine.IM_CancleAudioMessage();
    }

    /**
     * 下载语音文件
     * @param serial 语音消息ID
     * @param savePath 本地缓存路径，必须保证该路径有可写权限
     * @param callback 下载语音文件回调
     */
    public void downloadAudioMessage(long serial, String savePath, YIMEventCallback.DownloadFileCallback callback) {

        int errorcode = YIMConstInfo.Errorcode.ParamInvalid;
        if ((m_downloadDir != "") && (savePath == "" || savePath == null)){
            errorcode = IMEngine.IM_DownloadAudioFile(serial, m_downloadDir);
        }
        else if ((savePath != "") && (savePath != null)){
            errorcode = IMEngine.IM_DownloadAudioFile(serial, savePath);
        }

        YIMMessage message = new YIMMessage();
        message.setMeesageID(serial);
        if (YIMConstInfo.Errorcode.Success == errorcode){
            if (callback != null){
                boolean ret = addDownloadMsgCallbackObj(serial, callback);

                if (!ret){
                    callback.onDownload(YIMConstInfo.Errorcode.IsWaitingDownload,message,savePath);
                }
            }
        }else {
            if (callback != null){
                callback.onDownload(errorcode,message,savePath);
            }
        }
    }

    /**
     * 下载文件
     * @param serial 消息ID
     * @param savePath 本地缓存路径，必须保证该路径有可写权限
     * @param callback 下载文件回调
     */
    public void downloadFile(long serial, String savePath, YIMEventCallback.DownloadFileCallback callback) {

        int errorcode = IMEngine.IM_DownloadAudioFile(serial, savePath);

        YIMMessage message = new YIMMessage();
        message.setMeesageID(serial);

        if (YIMConstInfo.Errorcode.Success == errorcode){
            if (callback != null){
                boolean ret = addDownloadMsgCallbackObj(serial, callback);

                if (!ret){
                    callback.onDownload(YIMConstInfo.Errorcode.IsWaitingDownload,message,savePath);
                }
            }
        }else {
            if (callback != null){
                callback.onDownload(errorcode,message,savePath);
            }
        }
    }

    /**
     * 根据Url下载文件
     * @param fromUrl 下载地址
     * @param savePath 本地缓存路径，必须保证该路径有可写权限
     * @param callback 下载文件回调
     */
    public void downloadFileByUrl( String fromUrl, String savePath, int fileType, YIMEventCallback.DownloadByUrlCallback callback){

        int errorcode = IMEngine.IM_DownloadFileByURL( fromUrl, savePath, fileType);

        if (YIMConstInfo.Errorcode.Success == errorcode){
            if (callback != null){
                boolean ret = addUrlDownloadCallbackObj(fromUrl, callback);

                if (!ret){
                    callback.onDownloadByUrl(YIMConstInfo.Errorcode.IsWaitingDownload,fromUrl,savePath,0);
                }
            }
        }else {
            if (callback != null){
                callback.onDownloadByUrl(errorcode,fromUrl,savePath,0);
            }
        }
    }

    /**
     * 设置下载保存目录
     * @param path 保存目录的路径
     * @return
     */
    public int setDownloadDir(String path){
        m_downloadDir = path;
        return IMEngine.IM_SetDownloadDir(path);
    }

    /**
     * 发送自定义消息
     * @param recvId 消息接收者ID
     * @param chatType 聊天类型，详见ChatType
     * @param customMessage 二进制消息内容
     * @param bufferLen 二进制消息长度
     * @param callback 发送二进制消息回调
     */
    public void sendCustomMessage(String recvId, int chatType, byte[] customMessage, int bufferLen, YIMEventCallback.ResultCallback<SendMessage> callback) {

        SendMessage sendMessageInfo = new SendMessage();
        sendMessageInfo.setSendTime(0);
        sendMessageInfo.setIsForbidRoom(false);
        sendMessageInfo.setEndTime(0L);

        if (!checkString("ReceiverID", recvId)){
            sendMessageInfo.setRequestId(0L);
            callback.onFailed(YIMConstInfo.Errorcode.InvalidReceiver, sendMessageInfo);
            return;
        }else{
            IMEngine.MessageRequestId messageRequestId = new IMEngine().new MessageRequestId();
            int errcode = IMEngine.IM_SendCustomMessage(recvId, chatType, customMessage ,bufferLen, messageRequestId);

            if (YIMConstInfo.Errorcode.Success == errcode) {
                YIMMessage yimMessage = new YIMMessage();
                yimMessage.setChatType(chatType);
                yimMessage.setMeesageID(messageRequestId.getId());
                yimMessage.setSenderID(m_currentUserID);
                yimMessage.setReceiveID(recvId);
                yimMessage.setMessageType(YIMConstInfo.MessageBodyType.TXT);

                YIMMessageBodyCustom customMsg = new YIMMessageBodyCustom();
                customMsg.setMessageContent(Base64.encodeToString(customMessage,Base64.DEFAULT));

                yimMessage.setMessageBody(customMsg);

                MessageCallbackObject callbackObject = new MessageCallbackObject(YIMConstInfo.MessageBodyType.CustomMesssage, yimMessage, callback);

                boolean ret = addMessageCallbackObj(messageRequestId.getId(), callbackObject);
                if (!ret && (callback != null)) {
                    callback.onFailed(YIMConstInfo.Errorcode.IsWaitingSend, sendMessageInfo);
                }
            } else {
                if (callback != null){
                    sendMessageInfo.setRequestId(messageRequestId.getId());
                    callback.onFailed(errcode, sendMessageInfo);
                }
            }
        }

    }

    /**
     * 发送文件消息
     * @param recvId 消息接收者ID
     * @param chatType 聊天类型，详见ChatType
     * @param filePath 文件路径
     * @param extParam 附加参数
     * @param fileType 文件类型
     * @param callback 发送文件回调
     */
    public void sendFile(String recvId, int chatType, String filePath, String extParam, int fileType, YIMEventCallback.ResultCallback<SendMessage> callback) {

        SendMessage sendMessageInfo = new SendMessage();
        sendMessageInfo.setSendTime(0);
        sendMessageInfo.setIsForbidRoom(false);
        sendMessageInfo.setEndTime(0L);

        if (!checkString("ReceiverID", recvId)) {
            sendMessageInfo.setRequestId(0L);
            callback.onFailed(YIMConstInfo.Errorcode.InvalidReceiver, sendMessageInfo);
            return;
        } else {
            IMEngine.MessageRequestId messageRequestId = new IMEngine().new MessageRequestId();
            int errcode = IMEngine.IM_SendFile(recvId, chatType, filePath, extParam, fileType, messageRequestId);

            if (YIMConstInfo.Errorcode.Success == errcode) {
                YIMMessage yimMessage = new YIMMessage();
                yimMessage.setChatType(chatType);
                yimMessage.setMeesageID(messageRequestId.getId());
                yimMessage.setSenderID(m_currentUserID);
                yimMessage.setReceiveID(recvId);
                yimMessage.setMessageType(YIMConstInfo.MessageBodyType.TXT);

                YIMMessageBodyFile fileMsg = new YIMMessageBodyFile();
                fileMsg.setFileType(fileType);
                fileMsg.setExtParam(extParam);

                yimMessage.setMessageBody(fileMsg);

                if (callback != null){
                    MessageCallbackObject callbackObject = new MessageCallbackObject(YIMConstInfo.MessageBodyType.File, yimMessage, callback);

                    boolean ret = addMessageCallbackObj(messageRequestId.getId(), callbackObject);
                    if (!ret) {
                        callback.onFailed(YIMConstInfo.Errorcode.IsWaitingSend, sendMessageInfo);
                    }
                }
            } else {
                if (callback != null){
                    sendMessageInfo.setRequestId(messageRequestId.getId());
                    callback.onFailed(errcode, sendMessageInfo);
                }
            }
        }
    }

    /**
     * 发送文件消息显示进度
     * @param recvId 消息接收者ID
     * @param chatType 聊天类型，详见ChatType
     * @param filePath 文件路径
     * @param extParam 附加参数
     * @param fileType 文件类型
     * @param callback 发送文件回调
     * @param progressCallback 文件上传进度回调
     */
    public void sendFileWithProgress(String recvId, int chatType, String filePath, String extParam, int fileType, YIMEventCallback.ResultCallback<SendMessage> callback, final YIMEventCallback.ShowUploadFileProgressCallback progressCallback) {
        final String uploadFilePath = filePath;

        if (!checkString("ReceiverID", recvId)) {
            callback.onFailed(YIMConstInfo.Errorcode.InvalidReceiver, null);
            return;
        } else {
            IMEngine.MessageRequestId messageRequestId = new IMEngine().new MessageRequestId();

            int errcode = IMEngine.IM_SendFile(recvId, chatType, filePath, extParam, fileType, messageRequestId);
            setProgressCallback(messageRequestId.getId(), progressCallback);
            if (YIMConstInfo.Errorcode.Success == errcode) {
                YIMMessage yimMessage = new YIMMessage();
                yimMessage.setChatType(chatType);
                yimMessage.setMeesageID(messageRequestId.getId());
                yimMessage.setSenderID(m_currentUserID);
                yimMessage.setReceiveID(recvId);
                yimMessage.setMessageType(YIMConstInfo.MessageBodyType.TXT);

                YIMMessageBodyFile fileMsg = new YIMMessageBodyFile();
                fileMsg.setFileType(fileType);
                fileMsg.setExtParam(extParam);

                yimMessage.setMessageBody(fileMsg);

                if (callback != null){
                    MessageCallbackObject callbackObject = new MessageCallbackObject(YIMConstInfo.MessageBodyType.File, yimMessage, callback);
                    setProgressCallback(messageRequestId.getId(), progressCallback);
                    boolean ret = addMessageCallbackObj(messageRequestId.getId(), callbackObject);
                    if (!ret) {
                        callback.onFailed(YIMConstInfo.Errorcode.IsWaitingSend, null);
                    }
                }
            } else {
                if (callback != null){
                    callback.onFailed(errcode, null);
                }
            }
        }
    }

    public void setProgressCallback(long msgSerial, YIMEventCallback.ShowUploadFileProgressCallback progressCallback) {
        if (!uploadProgressCallbackQueue.containsKey(msgSerial)){
            uploadProgressCallbackQueue.put(msgSerial,progressCallback);
        }
    }

    /**
     * 消息关键词过滤
     * @param strSource 消息内容
     * @param level 过滤等级
     * @return
     */
    public String getFilterText(String strSource, IMEngine.IntegerVal level) {
        return IMEngine.IM_GetFilterText(strSource, level);
    }

    public void onPause(boolean pauseReceiveMessage) {
        IMEngine.IM_OnPause(pauseReceiveMessage);
    }

    public void onResume() {
        IMEngine.IM_OnResume();
    }

    /**
     * 获取新消息（只有setReceiveMessageSwitch设置为不自动接收消息，才需要在收到OnReceiveMessageNotify回调时调用该函数）
     * @param targets 房间ID列表
     * @return
     */
    public int getNewMessage(List<String> targets){
        JSONArray jsonArray = new JSONArray(targets);
        return IMEngine.IM_GetNewMessage(jsonArray.toString());
    }

    /**
     * 是否自动下载语音消息
     * @param download：自动下载语音消息  false：不自动下载语音消息(默认)
     * @return
     */
    public int setDownloadAudioMessageSwitch(boolean download){
        return IMEngine.IM_SetDownloadAudioMessageSwitch(download);
    }

    /**
     * 设置是否自动接收消息（房间消息 ）
     * @param targets 房间ID列表
     * @param autoReceive true:自动接收(默认)	false:不自动接收消息,有新消息达到时，SDK会发出onReceiveMessageNotify回调，调用方需要调用GetMessage获取新消息
     * @return
     */
    public int setReceiveMessageSwitch(List<String> targets, boolean autoReceive){
        JSONArray jsonArray = new JSONArray(targets);
        return IMEngine.IM_SetReceiveMessageSwitch(jsonArray.toString(), autoReceive);
    }

    /**
     * 从服务器查询房间最近历史消息
     * @param roomID 房间id
     * @param count 消息数量(最大200条)
     * @param direction 历史消息排序方向 0：按时间戳升序	1：按时间戳逆序
     * @param callback 查询历史记录回调
     */
    public void queryRoomHistoryMessageFromServer(String roomID, int count, int direction, YIMEventCallback.ResultCallback<RoomHistoryMsgInfo> callback){

        m_roomMsgCallback = callback;

        int errorcode = IMEngine.IM_QueryRoomHistoryMessageFromServer(roomID, count, direction);
        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            RoomHistoryMsgInfo roomHistoryMsg = new RoomHistoryMsgInfo(roomID,0,null);

            callback.onFailed(errorcode,roomHistoryMsg);
        }
    }

    /**
     * 发送礼物消息
     * @param anchorId 主播ID
     * @param channel 频道ID
     * @param giftID 礼物ID
     * @param giftCount 礼物数量
     * @param extParam 附加参数（格式为json {"nickname":"昵称","server_area":"区服","location":"位置","score":"积分","level":"等级","vip_level":"VIP等级","extra":"附加参数"}）
     * @param callback 发送礼物消息回调
     * @return
     */
    public void sendGift(String anchorId, String channel, int giftID, int giftCount, YIMExtraGifParam extParam, YIMEventCallback.ResultCallback<SendMessage> callback){

        SendMessage sendMessageInfo = new SendMessage();
        sendMessageInfo.setSendTime(0);
        sendMessageInfo.setIsForbidRoom(false);
        sendMessageInfo.setEndTime(0L);

        if (!checkString("Receive strChannel ", channel)){
            sendMessageInfo.setRequestId(0L);
            callback.onFailed(YIMConstInfo.Errorcode.InvalidReceiver, sendMessageInfo);
            return;
        }else {
            IMEngine.MessageRequestId messageRequestId = new IMEngine().new MessageRequestId();
            GsonBuilder builder = new GsonBuilder();
            Gson gson = builder.create();
            if (DEBUG)
                Log.d(TAG, "gift extparam:" + gson.toJson(extParam));
            int errcode = IMEngine.IM_SendGift(anchorId, channel, giftID, giftCount, gson.toJson(extParam), messageRequestId);

            if (YIMConstInfo.Errorcode.Success == errcode) {
                YIMMessage yimMessage = new YIMMessage();
                yimMessage.setChatType(YIMConstInfo.ChatType.RoomChat);
                yimMessage.setMeesageID(messageRequestId.getId());
                yimMessage.setSenderID(m_currentUserID);
                yimMessage.setReceiveID(anchorId);
                yimMessage.setMessageType(YIMConstInfo.MessageBodyType.Gift);

                YIMMessageBodyGift giftMsg = new YIMMessageBodyGift();
                giftMsg.setAnchor(anchorId);
                giftMsg.setGiftID(giftID);
                giftMsg.setGiftCount(giftCount);
                giftMsg.setExtParam(extParam);

                yimMessage.setMessageBody(giftMsg);

                if (callback != null){
                    MessageCallbackObject callbackObject = new MessageCallbackObject(YIMConstInfo.MessageBodyType.Gift, yimMessage, callback);

                    boolean ret = addMessageCallbackObj(messageRequestId.getId(), callbackObject);
                    if (!ret) {
                        callback.onFailed(YIMConstInfo.Errorcode.IsWaitingSend, sendMessageInfo);
                    }
                }
            } else {
                if (callback != null){
                    sendMessageInfo.setRequestId(messageRequestId.getId());
                    callback.onFailed(errcode, sendMessageInfo);
                }
            }
        }
    }

    /**
     * 群发文本消息
     * @param recvLists 消息接收者ID列表
     * @param strText 文本内容
     * @return
     */
    public int multiSendTextMessage(List<String> recvLists,String strText){
        JSONArray jsonArray = new JSONArray(recvLists);
        return IMEngine.IM_MultiSendTextMessage(jsonArray.toString(),strText);
    }

    /**
     * 获取最近联系人
     * @param callback 获取最近联系人回调
     */
    public void getHistoryContact(YIMEventCallback.ResultCallback<ArrayList<ContactsSessionInfo>> callback){
        m_historyContactCallback = callback;
        int errorcode = IMEngine.IM_GetHistoryContact();

        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            callback.onFailed(errorcode, null);
        }
    }

    /**
     * 启动录音
     * @param translateToText 是否带文字识别
     * @return YIMClient.MessageSendStatus
     */
    public MessageSendStatus startAudioSpeech(boolean translateToText){
        MessageSendStatus status = new MessageSendStatus();
        IMEngine.MessageRequestId messageRequestId = new IMEngine().new MessageRequestId();
        int errcode = IMEngine.IM_StartAudioSpeech(messageRequestId,translateToText);

        if(YIMConstInfo.Errorcode.Success == errcode) {
            lastSpeechMsg = new SpeechMessageInfo();
            lastSpeechMsg.setRequestID(messageRequestId.getId());
        }

        status.setMessageId(messageRequestId.getId());
        status.setErrorCode(errcode);
        return status;
    }

    /**
     * 停止录音
     * @param callback 上传录音回调
     */
    public void stopAudioSpeech(YIMEventCallback.SpeechEventCallback callback){

        SpeechMessageInfo speechMessageInfo = new SpeechMessageInfo();
        speechMessageInfo.setRequestID(0L);
        speechMessageInfo.setDownloadURL("");
        speechMessageInfo.setDuration(0);
        speechMessageInfo.setFileSize(0);
        speechMessageInfo.setLocalPath("");
        speechMessageInfo.setText("");

        if (lastSpeechMsg != null){

            int errorcode = IMEngine.IM_StopAudioSpeech();

            speechMessageInfo.setRequestID(lastSpeechMsg.getRequestID());
            if (YIMConstInfo.Errorcode.Success == errorcode){
                if (callback != null){
                    boolean ret = addUploadSpeechCallbackObj(lastSpeechMsg.getRequestID(), callback);
                    if (!ret){
                        callback.onStopAudioSpeechStatus (YIMConstInfo.Errorcode.IsWaitingUpload, speechMessageInfo);
                    }
                }
            }else {
                if (callback != null){
                    callback.onStopAudioSpeechStatus (errorcode, speechMessageInfo);
                }
            }

            lastSpeechMsg = null;
        }else {
            Log.e ("YouMeIM","Has no start record!");
            if (callback != null) {
                callback.onStopAudioSpeechStatus (YIMConstInfo.Errorcode.PTT_NotStartRecord, speechMessageInfo);
            }
        }
    }

    public int convertAMRToWav(String amrFilePath,String toWavFilePath){
        return IMEngine.IM_ConvertAMRToWav(amrFilePath,toWavFilePath);
    }

    /**
     * 查询本地历史消息记录
     * @param targetID 目标ID
     * @param chatType 聊天类型，详见ChatType
     * @param startMessageID 起始消息ID（默认为0，从最新一条消息ID开始查询）
     * @param count 消息数量（一次最大100条）
     * @param direction 查询方向 0：向前查找（比startMessageID时间更早）	1：向后查找（比startMessageID时间更晚）
     * @param callback 查询本地历史记录回调
     */
    public void queryHistoryMessage(String targetID, int chatType, long startMessageID, int count, int direction, YIMEventCallback.ResultCallback<YIMHistoryMessage> callback) {
        m_localHistoryMsgCallback = callback;
        int errorcode = IMEngine.IM_QueryHistoryMessage(targetID,chatType,startMessageID,count,direction);

        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            callback.onFailed(errorcode,null);
        }
    }

    /**
     * 根据消息ID删除本地记录
     * @param messageID 消息ID
     * @return
     */
    public int deleteHistoryMessageByID(long messageID){
        return IMEngine.IM_DeleteHistoryMessageByID(messageID);
    }

    /**
     * 根据时间删除本地记录
     * @param chatType 聊天类型，详见ChatType
     * @param time unix时间戳
     * @return
     */
    public int deleteHistoryMessageBeforeTime(int chatType, long time){
        return IMEngine.IM_DeleteHistoryMessage(chatType, time);
    }

    /**
     * 设置消息转发方式
     * @param int 聊天类型，0 正常传输，1 只抄送，默认为0
     * @return
     */
    public int switchMsgTransType(int transType){
        return IMEngine.IM_SwitchMsgTransType(transType);
    }

    /**
     * 删除指定用户的本地消息历史记录，保留指定的消息ID列表记录
     * @param targetID：指定的用户
     * @param chatType：聊天类型（指定私聊）
     * @param messageList：消息ID白名单
     * @return
     */
    public int deleteSpecifiedHistoryMessage(String targetID, int chatType, long[] excludeMesList) {
        return IMEngine.IM_DeleteSpecifiedHistoryMessage(targetID, chatType, excludeMesList);
    }

    /**
     * 删除本地历史消息
     * @param targetID userID或roomID
     * @param chatType 聊天类型，详见ChatType
     * @param startMessageID 起始消息ID（默认0 最近一条消息）
     * @param count 消息数量（默认0 删除所有消息）
     * @return
     */
    public int deleteHistoryMessageByTarget(String targetID, int chatType, long startMessageID, int count) {
        return IMEngine.IM_DeleteHistoryMessageByTarget(targetID, chatType, startMessageID, count);
    }

    /**
     * 设置用户信息
     * @param userInfo 用户信息 JSON格式：{"nickname":"","server_area_id":"","server_area":"","location_id":"","location":"","level":"","vip_level":"","platform_id":"","platform":""} (前七个必填，可以添加其他字段)
     * @return
     */
    public int setUserInfo(YIMExtraUserInfo userInfo){
        if(userInfo == null){
            return YIMConstInfo.Errorcode.ParamInvalid;
        }
        GsonBuilder builder = new GsonBuilder();
        Gson gson = builder.create();
        return IMEngine.IM_SetUserInfo(gson.toJson(userInfo));
    }

    /**
     * 查询用户信息
     * @param userID 用户ID
     * @param callback 查询用户信息回调
     */
    public void getUserInfo(String userID, YIMEventCallback.ResultCallback<YIMExtraUserInfo> callback){
        m_userInfoCallback = callback;

        int errorcode = IMEngine.IM_GetUserInfo(userID);

        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            callback.onFailed(errorcode,null);
        }
    }

    /**
     * 是否保存房间消息到本地历史记录
     * @param roomIDs 房间ID列表
     * @param isSave 是否保存（默认不保存）
     * @return
     */
    public int setRoomHistoryMessageSwitch(List<String> roomIDs,boolean isSave){
        JSONArray jsonArray = new JSONArray(roomIDs);
        return IMEngine.IM_SetRoomHistoryMessageSwitch(jsonArray.toString(),isSave);
    }

    /**
     * 播放语音
     * @param audioPath 语音文件路径
     * @param callback 播放语音回调
     */
    public void startPlayAudio(String audioPath, YIMEventCallback.ResultCallback<String> callback){

        m_playCompleteCallback = callback;

        boolean ret = isPlaying();
        if (ret){
            int stop_code = stopPlayAudio();
            if ((stop_code != YIMConstInfo.Errorcode.Success) && (callback != null)){
                m_playCompleteCallback = null;
                callback.onFailed(YIMConstInfo.Errorcode.StopPlayFailBeforeStart, audioPath);
            }
        }
        int errorcode = IMEngine.IM_StartPlayAudio(audioPath);
        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)) {
            m_playCompleteCallback = null;
            callback.onFailed(errorcode, audioPath);
        }
    }

    /**
     * 停止语音播放
     * @return
     */
    public int stopPlayAudio(){
        return IMEngine.IM_StopPlayAudio();
    }

    private boolean isPlaying(){
        return IMEngine.IM_IsPlaying();
    }

    /**
     * 设置语音播放音量
     * @param volume 音量值，取值范围0.0到1.0
     */
    public void setVolume(float volume){
        if(volume<0.0f) volume=0;
        if(volume>1.0f) volume = 1.0f;
        IMEngine.IM_SetVolume(volume);
    }

    /**
     * 获取语音缓存目录
     * @return
     */
    public String getAudioCachePath(){
        return IMEngine.IM_GetAudioCachePath();
    }

    /**
     * 清理语音缓存目录（注意清空语音缓存目录后历史记录中会无法读取到音频文件，调用清理历史记录接口也会自动删除对应的音频缓存文件）
     * @return
     */
    public boolean clearAudioCachePath(){
        return IMEngine.IM_ClearAudioCachePath();
    }

    /**
     * 查询用户状态
     * @param userID 用户ID
     * @param callback 查询结果回调
     */
    public void queryUserStatus(String userID, YIMEventCallback.ResultCallback<UserStatusInfo> callback){

        m_userStatusCallback = callback;
        int errorcode = IMEngine.IM_QueryUserStatus(userID);
        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            UserStatusInfo statusInfo = new UserStatusInfo();
            statusInfo.status = 2; //未知
            statusInfo.userID = userID;
            callback.onFailed(errorcode, statusInfo);
            m_userStatusCallback = null;
        }
    }


    /**
     *  文本翻译
     *  @pararm text 原语言字符串
     *  @param destLangCode 目标语言
     *  @param srcLangCode 原语言
     *  @param callback 文本翻译回调
     */
    public void translateText(String text, int destLangCode, int srcLangCode, YIMEventCallback.ResultCallback<TranlateTextInfo> callback){

        m_translateTextCallback = callback;
        IMEngine.IntegerVal val = new IMEngine().new IntegerVal();

        int errorCode = IMEngine.IM_TranslateText(val,text,destLangCode,srcLangCode) ;

        if ((YIMConstInfo.Errorcode.Success != errorCode) && (callback != null)) {
            TranlateTextInfo tranlateInfo = new TranlateTextInfo();
            tranlateInfo.requestID = val.getValue();
            tranlateInfo.text = text;
            tranlateInfo.srcLangCode = srcLangCode;
            tranlateInfo.destLangCode = destLangCode;

            callback.onFailed(errorCode, tranlateInfo);
        }
    }

   /**
	* 功能：屏蔽/解除屏蔽用户消息
	* @param userID：用户ID
	* @param block：true-屏蔽 false-解除屏蔽
	* @param callback 屏蔽用户回调
	*/
    public void blockUser(String userID, boolean block, YIMEventCallback.ResultCallback<BlockUserInfo> callback){

        m_blockUserCallback = callback;
        int errorcode = IMEngine.IM_BlockUser(userID, block);
        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            BlockUserInfo blockInfo = new BlockUserInfo();
            blockInfo.setUserID(userID);
            blockInfo.setBlock(false);

            callback.onFailed(errorcode,blockInfo);
        }
    }

   /**
	* 功能：解除所有已屏蔽用户
	* @param callback 解除用户屏蔽回调
	*/
    public void unBlockAllUser(YIMEventCallback.OperationCallback callback){
        m_unBlockUserCallback = callback;
        int errorcode = IMEngine.IM_UnBlockAllUser();
        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            callback.onFailed(errorcode);
        }
    }

   /**
	* 功能：获取被屏蔽消息用户
	* @param callback 获取屏蔽用户回调
	*/
    public void getBlockUsers(YIMEventCallback.ResultCallback<ArrayList<String>> callback){

        m_getBlockUsersCallback = callback;
        int errorcode = IMEngine.IM_GetBlockUsers();
        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            callback.onFailed(errorcode,null);
        }
    }


    /**
     * 获取地理定位
     * @return
     */
    public int getCurrentLocation(){
        return IMEngine.IM_GetCurrentLocation();
    }

    /**
     * 获取附近的目标	（玩家的区服ID必须一致）
     * @param count:获取数量（一次最大200）
     * @param serverAreaID：区服ID（SetUserInfo接口设置）
     * @param districtlevel：行政区划等级
     * @param resetStartDistance：是否重置查找起始距离（true：重新从距离自己0米查找	false：从上次结果中距离最远的位置查找）
     * @param callback  获取附近人回调
     */
    public void getNearbyObjects(int count, String serverAreaID, int districtlevel, boolean resetStartDistance, YIMEventCallback.ResultCallback<RelativeLocationInfo> callback){

        m_getNearbyObjCallback = callback;
        int errorcode = IMEngine.IM_GetNearbyObjects(count,serverAreaID,districtlevel,resetStartDistance) ;
        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            RelativeLocationInfo relativeLocations = new RelativeLocationInfo();
            relativeLocations.startDistance = 0;
            relativeLocations.endDistance = 0;
            relativeLocations.relativeLocations = null;

            callback.onFailed(errorcode,relativeLocations);
        }
    }

    /**
     * 功能：获取与指定用户距离
     * @param userID 用户ID
     * @param callback  获取距离回调
     */
    public void getDistance(String userID, YIMEventCallback.ResultCallback<UserDistanceInfo> callback){

        m_getDistanceCallback = callback;
        int errorcode = IMEngine.IM_GetDistance(userID);
        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            UserDistanceInfo distanceInfo = new UserDistanceInfo();
            distanceInfo.userID = userID;
            distanceInfo.distance = -1;

            callback.onFailed(errorcode,distanceInfo);
        }
    }

    /**
     * 设置地理定位更新时间（分钟），默认不更新
     * @param interval
     */
    public void setUpdateInterval(int interval) {
        IMEngine.IM_SetUpdateInterval(interval);
    }


    /**
     * 获取麦克风状态
     * @param callback 获取麦克风状态回调
     */
    public void getMicrophoneStatus(YIMEventCallback.GetMicStatusCallback callback){
        m_getMicStatusCallback = callback;
        IMEngine.IM_GetMicrophoneStatus();
    }

    /**
     * 设置语音识别语言
     * @param accent：语言
     */
    public int setSpeechRecognizeLanguage(int language){
        return IMEngine.IM_SetSpeechRecognizeLanguage(language);
    }

    /**
     * 设置仅识别语音文字，不发送语音消息;
     * @param recognition：是否开启仅识别文字，false:识别语音文字并发送语音消息，true:仅识别语音文字
     */
    private int setOnlyRecognizeSpeechText(boolean recognition){
        return IMEngine.IM_SetOnlyRecognizeSpeechText(recognition);
    }

    /**
     * 举报
     * @param userID：被举报人ID
     * @param source：举报来源（私聊/房间）
     * @param reason：举报原因
     * @param description：举报原因描述（最长180字）
     * @param extraParam：附加参数（JSON格式 {"nickname":"","server_area":"","level":"","vip_level":""}）
     * @return
     */

    public int accusation(String userID, int source, int reason, String description, String extraParam){
        return IMEngine.IM_Accusation(userID, source, reason, description, extraParam);
    }

    /**
     * 查询公告信息
     * @return
     */
    public int queryNotice(){
        return IMEngine.IM_QueryNotice();
    }

    /**
     * 获取禁言信息
     * @param callback 禁言信息回调
     * @return
     */
    public void getForbiddenSpeakInfo(YIMEventCallback.ResultCallback<ArrayList<YIMForbiddenSpeakInfo>> callback)
    {
        m_forbidSpeakInfoCallback = callback;
        int errorcode = IMEngine.IM_GetForbiddenSpeakInfo();
        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            callback.onFailed(errorcode,null);
        }
    }

    /**
     * 设置消息已读
     * @param messageID 消息ID
     * @param read 是否已读，true-已读，false-未读
     * @return
     */
    public int setMessageRead(long messageID, boolean read)
    {
        return IMEngine.IM_SetMessageRead(messageID, read);
    }

    /**
     * 设置所有消息为已读
     * @param userID 用户ID
     * @param read 是否已读，true-已读，false-未读
     * @return
     */
    public int setAllMessageRead(String userID, boolean read)
    {
        return IMEngine.IM_SetAllMessageRead(userID,read);
    }

    /**
     * 设置语音消息为已播放
     * @param messageID 消息ID
     * @param played 是否播放，true-播放，false-未播放
     * @return
     */
    public int setVoiceMsgPlayed(long messageID, boolean played)
    {
        return IMEngine.IM_SetVoiceMsgPlayed(messageID,played);
    }

    /**
     * 设置用户基本资料
     * @param userInfo 用户基本信息
     * @param callback 设置用户资料回调
     */
    public void setUserProfileInfo(YIMUserSettingInfo userInfo, YIMEventCallback.OperationCallback callback){
        if(userInfo == null){
            if (callback != null){
                callback.onFailed(YIMConstInfo.Errorcode.ParamInvalid);
            }

            return;
        }

        m_setprofileCallback = callback;
        JSONObject profileInfo = new JSONObject();
        try {
            profileInfo.put("NickName",userInfo.getNickName());

            profileInfo.put("Sex",String.valueOf(userInfo.getSex()));
            profileInfo.put("Signature",userInfo.getSignature());
            profileInfo.put("Country",userInfo.getCountry());
            profileInfo.put("Province",userInfo.getProvince());
            profileInfo.put("City",userInfo.getCity());
            profileInfo.put("ExtraInfo",userInfo.getExtraInfo());
        } catch (JSONException e) {
            e.printStackTrace();
        }
        int errorcode = IMEngine.IM_SetUserProfileInfo(profileInfo.toString());
        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            callback.onFailed(errorcode);
        }
    }

    /**
     * 设置用户头像
     * @param photoPath 本地图片绝对路径
     * @param callback 设置头像回调
     */
    public void setUserProfilePhoto(String photoPath, YIMEventCallback.ResultCallback<String> callback)
    {
        m_photoUrlCallback = callback;
        int errorcode = IMEngine.IM_SetUserProfilePhoto(photoPath);
        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            callback.onFailed(errorcode, "");
        }
    }

    /**
     * 查询用户基本资料
     * @param userID 指定用户ID
     * @param callback 查询用户基本资料回调
     */
    public void getUserProfileInfo(String userID, YIMEventCallback.ResultCallback<UserProfileInfo> callback)
    {
        m_userProfileCallback = callback;
        int errorcode = IMEngine.IM_GetUserProfileInfo(userID);
        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            callback.onFailed(errorcode, null);
        }
    }

    /**
     * 切换用户状态
     * @param userID 用户ID
     * @param userStatus 用户状态，0：在线 1：隐身 2：离线
     * @param callback 切换在线状态回调
     */
    public void switchUserStatus(String userID, int userStatus, YIMEventCallback.OperationCallback callback)
    {
        m_switchStatusCallback = callback;
        int errorcode = IMEngine.IM_SwitchUserStatus(userID, userStatus);
        if ((YIMConstInfo.Errorcode.Success != errorcode) && (m_switchStatusCallback != null)){
            m_switchStatusCallback.onFailed(errorcode);
        }
    }

    /**
     * 设置好友添加权限
     * @param beFound 是否被别人查找到，true-能被查找，false-不能被查找
     * @param beAddPermission 被其它用户添加的权限
     * @param callback 设置添加权限回调
     */
    public void setAddPermission(boolean beFound, int beAddPermission, YIMEventCallback.OperationCallback callback)
    {
        m_setPermissionCallback = callback;
        int errorcode = IMEngine.IM_SetAddPermission(beFound, beAddPermission);
        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            callback.onFailed(errorcode);
        }
    }

    /**
     * 查找添加好友（获取用户简要信息）
     * @param findType 查找类型 0：按ID查找	1：按昵称查找
     * @param target 对应查找类型用户ID或昵称
     * @param callback 查找好友回调
     */
    public void findUser(int findType, String target, YIMEventCallback.ResultCallback<ArrayList<YIMUserBriefInfo>> callback)
    {
        m_findUserCallback = callback;
        int errorcode = IMEngine.IM_FindUser(findType, target);
        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            callback.onFailed(errorcode, null);
        }
    }

    /**
     * 请求添加好友
     * @param users 用户ID列表
     * @param comments 备注或验证信息(长度最大128)
     * @param callback 请求添加好友回调
     */
    public void requestAddFriend(List<String> users, String comments, YIMEventCallback.ResultCallback<String> callback)
    {
        m_requestAddCallback = callback;
        JSONArray jsonArray = new JSONArray(users);
        int errorcode = IMEngine.IM_RequestAddFriend(jsonArray.toString(), comments);
        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            callback.onFailed(errorcode,"");
        }
    }

    /**
     * 处理被请求添加好友
     * @param userID 用户ID
     * @param dealResult 处理结果	0：同意	1：拒绝
     * @param callback 处理请求回调
     */
    public void dealBeRequestAddFriend(String userID, int dealResult, YIMEventCallback.ResultCallback<YIMFriendDealResult> callback)
    {
        m_dealAddCallback = callback;
        int errorcode = IMEngine.IM_DealBeRequestAddFriend(userID, dealResult);
        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            YIMFriendDealResult dealResultInfo = new YIMFriendDealResult();
            dealResultInfo.userID = userID;
            dealResultInfo.comments = "";
            dealResultInfo.dealResult = -1;

            callback.onFailed(errorcode, dealResultInfo);
        }
    }

    /**
     * 删除好友
     * @param users 用户ID列表
     * @param deleteType 删除类型	0：双向删除	1：单向删除(删除方在被删除方好友列表依然存在)
     * @param callback 删除好友回调
     */
    public void deleteFriend(List<String> users, int deleteType, YIMEventCallback.ResultCallback<String> callback)
    {
        m_deleteFriendCallback = callback;
        JSONArray jsonArray = new JSONArray(users);
        int errorcode = IMEngine.IM_DeleteFriend(jsonArray.toString(), deleteType);
        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            callback.onFailed(errorcode,"");
        }
    }

    /**
     * 拉黑好友
     * @param type 0：拉黑	1：解除拉黑
     * @param users 用户ID列表
     * @param callback 拉黑好友的回调
     */
    public void blackFriend(int type, List<String> users, YIMEventCallback.ResultCallback<YIMBlackFriendInfo> callback)
    {
        m_blackFriendCallback = callback;
        JSONArray jsonArray = new JSONArray(users);
        int errorcode = IMEngine.IM_BlackFriend(type, jsonArray.toString());
        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            YIMBlackFriendInfo blackFriendInfo = new YIMBlackFriendInfo();
            blackFriendInfo.userID = "";
            blackFriendInfo.type = type;

            callback.onFailed(errorcode, blackFriendInfo);
        }
    }

    /**
     * 查询我的好友
     * @param type 0：正常好友	1：被拉黑好友
     * @param startIndex 起始序号
     * @param count 数量（一次最大100）
     * @param callback 查询我的好友回调
     */
    public void queryFriends(int type, int startIndex, int count, YIMEventCallback.ResultCallback<YIMFriendListInfo> callback)
    {
        m_queryFriendsCallback = callback;
        int errorcode = IMEngine.IM_QueryFriends(type, startIndex, count);
        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            YIMFriendListInfo friendList = new YIMFriendListInfo();
            friendList.startIndex = startIndex;
            friendList.type = type;
            friendList.userList = null;

            callback.onFailed(errorcode,friendList);
        }
    }

    /**
     * 查询好友请求列表
     * @param startIndex 起始序号
     * @param count 数量（一次最大20）
     * @param callback 查询好友请求列表回调
     */
    public void queryFriendRequestList(int startIndex, int count, YIMEventCallback.ResultCallback<YIMFriendRequestInfoList> callback)
    {
        m_requestListCallback = callback;
        int errorcode = IMEngine.IM_QueryFriendRequestList(startIndex, count);
        if ((YIMConstInfo.Errorcode.Success != errorcode) && (callback != null)){
            YIMFriendRequestInfoList requestInfoList = new YIMFriendRequestInfoList();
            requestInfoList.startIndex = startIndex;
            requestInfoList.userList = null;

            callback.onFailed(errorcode, requestInfoList);
        }
    }

    private boolean addJoinChannelCallbackObj(String channelID, YIMEventCallback.ResultCallback<ChatRoom> callbackObject){
        if (!joinChannelCallbackQueue.containsKey(channelID)){
            joinChannelCallbackQueue.put(channelID,callbackObject);
        }else {
            Log.e("YouMeIM","channel id is already in joining queue.");
            return false;
        }
        return true;
    }

    private boolean addLeaveChannelCallbackObj(String channelID, YIMEventCallback.ResultCallback<ChatRoom> callbackObject){
        if (!leaveChannelCallbackQueue.containsKey(channelID)){
            leaveChannelCallbackQueue.put(channelID,callbackObject);
        }else {
            Log.e("YouMeIM","channel id is already in leaving queue.");
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

    private boolean addMessageCallbackObj(long requestID, MessageCallbackObject callbackObject){
        if (!messageCallbackQueue.containsKey(requestID)){
            messageCallbackQueue.put(requestID,callbackObject);
        }else {
            Log.e("YouMeIM","message id is already in sending queue.");
            return false;
        }
        return true;
    }

    private boolean addDownloadMsgCallbackObj(long requestID, YIMEventCallback.DownloadFileCallback callbackObject){
        if (!downloadMsgCallbackQueue.containsKey(requestID)){
            downloadMsgCallbackQueue.put(requestID,callbackObject);
        }else {
            Log.e("YouMeIM","message id is already in downloading queue.");
            return false;
        }
        return true;
    }

    private boolean addUrlDownloadCallbackObj(String fromUrl, YIMEventCallback.DownloadByUrlCallback callbackObj){
        if (!downloadByUrlCallbackQueue.containsKey(fromUrl)){
            downloadByUrlCallbackQueue.put(fromUrl,callbackObj);
        }else {
            Log.e("YouMeIM","message id is already in downloading queue.");
            return false;
        }
        return true;
    }

    private boolean addUploadSpeechCallbackObj(long requestID, YIMEventCallback.SpeechEventCallback callbackObj){
        if (!uploadSpeechCallbackQueue.containsKey(requestID)){
            uploadSpeechCallbackQueue.put(requestID,callbackObj);
        }else {
            Log.e("YouMeIM","message id is already in uploading queue.");
            return false;
        }
        return true;
    }


    public final static class RequestId {
        public long id;
    }

    public final static class MessageSendStatus {
        private int errCode;
        private long msgId;


        public MessageSendStatus() {

        }

        public MessageSendStatus(int errCode, long msgId) {
            this.errCode = errCode;
            this.msgId = msgId;
        }

        public void setErrorCode(int errCode) {
            this.errCode = errCode;
        }

        public void setMessageId(long msgId) {
            this.msgId = msgId;
        }

        public int getErrorCode() {
            return errCode;
        }

        public long getMessageId() {
            return msgId;
        }
    }

    public final static class TranslateMsgStatus{
        private int errCode;
        private long msgId;

        public void setErrorCode(int errCode) {
            this.errCode = errCode;
        }

        public void setMessageId(long msgId) {
            this.msgId = msgId;
        }

        public int getErrorCode() {
            return errCode;
        }

        public long getMessageId() {
            return msgId;
        }
    }

    public enum ChannelEventType {
        UserJoinRoom,
        UserLeaveRoom
    }

    private boolean checkString(String type, String text){
        boolean res = true;
        if (TextUtils.isEmpty(type)){
            res = false;
        }else {
            if (TextUtils.isEmpty(text)){
                res = false;
                debugInfo(type + " is null");
            }
        }
        return res;
    }

    private void debugInfo(String info){
        if (DEBUG)
            Log.e(TAG, info);
    }

    private Runnable paresMessage = new Runnable() {
        @Override
        public void run() {
            String msg = null;
            while (true) {
                try {
                    byte[] bytes = IMEngine.IM_GetMessage();
                    msg = new String(bytes, "UTF-8"); // please debate what the safest charset should be?
                } catch (UnsupportedEncodingException e) {
                    Log.e(TAG, "Couldn't convert the jbyteArray to jstring");
                }
                if(null == msg){
                    continue;
                }
                if (mIsExit)
                    break;
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
        if (null == msg ||  msg.equals("") )
            return;
        try {
            Gson gson = new Gson();
            YouMeIMJsonResponse jsonResponse = null;
            if (null != msg) {
                jsonResponse = gson.fromJson(msg, YouMeIMJsonResponse.class);
            }else{
                return;
            }
            switch (jsonResponse.command) {
                case CMD_RECEIVE_MESSAGE_NITIFY:
                {
                    try {
                        NewMessageNotifyObj newNotifier = gson.fromJson(msg, NewMessageNotifyObj.class);
                        mHandler.post(new YIMCallBackProtocolV2(m_msgEventCallback, "onRecvNewMessage", newNotifier.chatType, newNotifier.targetID));

                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_GET_RENCENT_CONTACTS:
                {
                    try {
                        Contacts contacts = gson.fromJson(msg,Contacts.class);
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_historyContactCallback, "onSuccess", contacts.contacts));
                        } else {
                            mHandler.post(new YIMCallBackProtocolV2(m_historyContactCallback, "onFailed", jsonResponse.errcode,contacts.contacts));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_QUERY_HISTORY_MESSAGE:
                {
                    try {
                        YIMHistoryMessage historyMessage = gson.fromJson(msg,YIMHistoryMessage.class);
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_localHistoryMsgCallback, "onSuccess", historyMessage));
                        } else {
                            mHandler.post(new YIMCallBackProtocolV2(m_localHistoryMsgCallback, "onFailed", jsonResponse.errcode, historyMessage));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_GET_ROOM_HISTORY_MSG:
                {
                    RoomHistoryMessage roomHistoryMessages = gson.fromJson(msg, RoomHistoryMessage.class);
                    ArrayList<YIMMessage> messages = new ArrayList<YIMMessage>();
                    if (roomHistoryMessages.messageList != null && roomHistoryMessages.messageList.size() > 0){
                        for(Iterator<RecvMessage> it = roomHistoryMessages.messageList.iterator(); it.hasNext();) {
                            YIMMessage message = new YIMMessage();
                            IYIMMessageBodyBase msgBody = null;
                            RecvMessage item = (RecvMessage)it.next();
                            message.setChatType(item.chatType);
                            message.setReceiveID(item.receiveId);
                            message.setSenderID(item.senderId);
                            message.setMeesageID(item.serial);
                            message.setMessageType(item.msgType);
                            message.setCreateTime(item.createTime);
                            message.setDistance(item.distance);
                            message.setRead(item.getIsRead());
                            if (item.msgType == YIMService.MessageBodyType.TXT) {
                                msgBody = new YIMMessageBodyText();
                                ((YIMMessageBodyText) msgBody).setMessageContent(item.content);
                                ((YIMMessageBodyText) msgBody).setAttachParam(item.attachParam);
                            } else if (item.msgType == YIMService.MessageBodyType.CustomMesssage) {
                                msgBody = new YIMMessageBodyCustom();
                                ((YIMMessageBodyCustom) msgBody).setMessageContent(item.content);
                            } else if (item.msgType == YIMService.MessageBodyType.Voice) {
                                msgBody = new YIMMessageBodyAudio();
                                ((YIMMessageBodyAudio) msgBody).setAudioTime(item.duration);
                                ((YIMMessageBodyAudio) msgBody).setParam(item.param);
                                ((YIMMessageBodyAudio) msgBody).setText(item.extraText);
                            }else if (item.msgType == YIMService.MessageBodyType.File){
                                msgBody = new YIMMessageBodyFile();
                                ((YIMMessageBodyFile) msgBody).setFileType(item.fileType);
                                ((YIMMessageBodyFile) msgBody).setFileSize(item.fileSize);
                                ((YIMMessageBodyFile) msgBody).setFileExtension(item.fileExtension);
                                ((YIMMessageBodyFile) msgBody).setExtParam(item.extraParam);
                                ((YIMMessageBodyFile) msgBody).setFileName(item.fileName);
                            }else if (item.msgType == YIMService.MessageBodyType.Gift){
                                msgBody = new YIMMessageBodyGift();
                                ((YIMMessageBodyGift) msgBody).setAnchor(item.anchor);
                                ((YIMMessageBodyGift) msgBody).setGiftCount(item.giftCount);
                                ((YIMMessageBodyGift) msgBody).setGiftID(item.giftID);
                                try {
                                    YIMExtraGifParam giftExtParam = gson.fromJson(item.param, YIMExtraGifParam.class);
                                    ((YIMMessageBodyGift) msgBody).setExtParam(giftExtParam);
                                }catch (Exception e){
                                    e.printStackTrace();
                                    return;
                                }
                            }
                            message.setMessageBody(msgBody);
                            messages.add(message);
                        }
                    }
                    RoomHistoryMsgInfo roomHistoryMsgInfo = new RoomHistoryMsgInfo(roomHistoryMessages.roomID,roomHistoryMessages.remain,messages);
                    try {
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_roomMsgCallback, "onSuccess", roomHistoryMsgInfo));
                        } else {
                            mHandler.post(new YIMCallBackProtocolV2(m_roomMsgCallback, "onFailed", jsonResponse.errcode, roomHistoryMsgInfo));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_STOP_AUDIOSPEECH:
                {
                    SpeechMessageInfo speechMessage = gson.fromJson(msg,SpeechMessageInfo.class);

                    YIMEventCallback.SpeechEventCallback callbackObject = null;

                    boolean finded = uploadSpeechCallbackQueue.containsKey(speechMessage.getRequestID());
                    if (finded){
                        callbackObject = uploadSpeechCallbackQueue.get(speechMessage.getRequestID());
                        try {
                             mHandler.post(new YIMCallBackProtocolV2(callbackObject, "onStopAudioSpeechStatus", jsonResponse.errcode, speechMessage));
                        } catch (NullPointerException e) {
                            e.printStackTrace();
                        }

                        messageCallbackQueue.remove(speechMessage.getRequestID());
                    }
                }
                break;
                case CMD_LOGIN: {

                    try {
                        Login login = gson.fromJson(msg, Login.class);
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_loginCallback, "onSuccess", login.getYoumeId()));
                            m_currentUserID = login.getYoumeId();
                        }else {
                            mHandler.post(new YIMCallBackProtocolV2(m_loginCallback, "onFailed", jsonResponse.errcode,login.getYoumeId()));
                        }

                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_LOGOUT:
                    try {
                        mHandler.post(new YIMCallBackProtocolV2(m_logoutCallback, "onSuccess"));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }

                    break;
                case CMD_KICK_OFF:
                    try {
                        mHandler.post(new YIMCallBackProtocolV2(m_kickOffCallback, "onKickOff"));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }

                    break;
                case CMD_ENTER_ROOM: {
                    ChatRoom joinGroupResponse = gson.fromJson(msg, ChatRoom.class);

                    YIMEventCallback.ResultCallback<ChatRoom> callbackObject = null;
                    boolean finded = joinChannelCallbackQueue.containsKey(joinGroupResponse.groupId);
                    if (finded){
                        callbackObject = joinChannelCallbackQueue.get(joinGroupResponse.groupId);

                        try {
                            if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                                mHandler.post(new YIMCallBackProtocolV2(callbackObject, "onSuccess", joinGroupResponse));

                            }else {
                                mHandler.post(new YIMCallBackProtocolV2(callbackObject, "onFailed", jsonResponse.errcode,joinGroupResponse));
                            }

                        } catch (NullPointerException e) {
                            e.printStackTrace();
                        }

                        joinChannelCallbackQueue.remove(joinGroupResponse.groupId);
                    }
                }
                break;
                case CMD_LEAVE_ROOM: {
                    ChatRoom joinGroupResponse = gson.fromJson(msg, ChatRoom.class);

                    YIMEventCallback.ResultCallback<ChatRoom> callbackObject = null;
                    boolean finded = leaveChannelCallbackQueue.containsKey(joinGroupResponse.groupId);
                    if (finded){
                        callbackObject = leaveChannelCallbackQueue.get(joinGroupResponse.groupId);

                        try {
                            if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                                mHandler.post(new YIMCallBackProtocolV2(callbackObject, "onSuccess", joinGroupResponse));

                            }else {
                                mHandler.post(new YIMCallBackProtocolV2(callbackObject, "onFailed", jsonResponse.errcode,joinGroupResponse));
                            }
                        } catch (NullPointerException e) {
                            e.printStackTrace();
                        }

                        leaveChannelCallbackQueue.remove(joinGroupResponse.groupId);
                    }
                }
                break;
                case CMD_USER_ENTER_ROOM: {
                    try {
                        UserChatRoom userChatRoom = gson.fromJson(msg, UserChatRoom.class);
                        mHandler.post(new YIMCallBackProtocolV2(m_userJoinLeaveCallback, "joinLeaveNotify", ChannelEventType.UserJoinRoom, userChatRoom));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_USER_LEAVE_ROOM: {
                    try {
                        UserChatRoom userChatRoom = gson.fromJson(msg, UserChatRoom.class);
                        mHandler.post(new YIMCallBackProtocolV2(m_userJoinLeaveCallback, "joinLeaveNotify", ChannelEventType.UserLeaveRoom, userChatRoom));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_RECV_MESSAGE: {

                        YIMMessage message = new YIMMessage();
                        IYIMMessageBodyBase msgBody = null;
//                    Log.e("CMD_RECV_MESSAGE","msg = " + msg ) ;
                        RecvMessage recvMessage = gson.fromJson(msg, RecvMessage.class);
                        message.setChatType(recvMessage.chatType);
                    /*message.setGroupID(recvMessage.);*/
                        message.setReceiveID(recvMessage.receiveId);
                        message.setSenderID(recvMessage.senderId);
                        message.setMeesageID(recvMessage.serial);
                        message.setMessageType(recvMessage.msgType);
                        message.setCreateTime(recvMessage.createTime);
                        message.setDistance(recvMessage.distance);
                        message.setRead(recvMessage.getIsRead());
                        if (recvMessage.msgType == YIMService.MessageBodyType.TXT) {
                            msgBody = new YIMMessageBodyText();
                            ((YIMMessageBodyText) msgBody).setMessageContent(recvMessage.content);
                            ((YIMMessageBodyText) msgBody).setAttachParam(recvMessage.attachParam);
                        } else if (recvMessage.msgType == YIMService.MessageBodyType.CustomMesssage) {
                            msgBody = new YIMMessageBodyCustom();
                            ((YIMMessageBodyCustom) msgBody).setMessageContent(recvMessage.content);
                        } else if (recvMessage.msgType == YIMService.MessageBodyType.Voice) {
                            msgBody = new YIMMessageBodyAudio();
                            ((YIMMessageBodyAudio) msgBody).setAudioTime(recvMessage.duration);
                            ((YIMMessageBodyAudio) msgBody).setParam(recvMessage.param);
                            ((YIMMessageBodyAudio) msgBody).setText(recvMessage.extraText);
                        }else if (recvMessage.msgType == YIMService.MessageBodyType.File)
                        {
                            msgBody = new YIMMessageBodyFile();
                            ((YIMMessageBodyFile) msgBody).setFileType(recvMessage.fileType);
                            ((YIMMessageBodyFile) msgBody).setFileSize(recvMessage.fileSize);
                            ((YIMMessageBodyFile) msgBody).setFileExtension(recvMessage.fileExtension);
                            ((YIMMessageBodyFile) msgBody).setExtParam(recvMessage.extraParam);
                            ((YIMMessageBodyFile) msgBody).setFileName(recvMessage.fileName);
                        }else if (recvMessage.msgType == YIMService.MessageBodyType.Gift)
                        {

                            msgBody = new YIMMessageBodyGift();
                            ((YIMMessageBodyGift) msgBody).setAnchor(recvMessage.anchor);
                            ((YIMMessageBodyGift) msgBody).setGiftCount(recvMessage.giftCount);
                            ((YIMMessageBodyGift) msgBody).setGiftID(recvMessage.giftID);
                            try {
                                YIMExtraGifParam giftExtParam = gson.fromJson(recvMessage.param, YIMExtraGifParam.class);
                                if (DEBUG)
                                    Log.d(TAG, "recv giftExtParam:" + giftExtParam.getNickName());
                                ((YIMMessageBodyGift) msgBody).setExtParam(giftExtParam);
                            }catch (Exception e){
                                e.printStackTrace();
                                return;
                            }
                        }

                        message.setMessageBody(msgBody);

                    try {
                        mHandler.post(new YIMCallBackProtocolV2(m_msgEventCallback, "onRecvMessage", message));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }

                }
                break;
                case CMD_SND_VOICE_MSG: {
                    SendVoiceMsgInfo voiceMsgInfo = gson.fromJson(msg, SendVoiceMsgInfo.class);
                    MessageCallbackObject callbackObject = null;
                    boolean finded = messageCallbackQueue.containsKey(voiceMsgInfo.getRequestId());
                    if (finded){
                        callbackObject = messageCallbackQueue.get(voiceMsgInfo.getRequestId());
                        if ((callbackObject != null) && (callbackObject.callback != null))
                        {
                            try {
                                 mHandler.post(new YIMCallBackProtocolV2((YIMEventCallback.AudioMsgEventCallback)callbackObject.callback, "onSendAudioMessageStatus", jsonResponse.errcode, voiceMsgInfo));

                            } catch (NullPointerException e) {
                                e.printStackTrace();
                            }
                        }
                        messageCallbackQueue.remove(voiceMsgInfo.getRequestId());
                    }
                }

                break;
                case CMD_SEND_MESSAGE_STATUS: {
                    SendMessage sendMessage = gson.fromJson(msg, SendMessage.class);
                    
                    MessageCallbackObject callbackObject = null;
                    boolean finded = messageCallbackQueue.containsKey(sendMessage.getRequestId());
                    if (finded){
                        callbackObject = messageCallbackQueue.get(sendMessage.getRequestId());
                        if ((callbackObject != null) && (callbackObject.callback != null))
                        {
                            try {
                                if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                                    mHandler.post(new YIMCallBackProtocolV2((YIMEventCallback.ResultCallback<SendMessage>)callbackObject.callback, "onSuccess", sendMessage));

                                }else {
                                    mHandler.post(new YIMCallBackProtocolV2((YIMEventCallback.ResultCallback<SendMessage>)callbackObject.callback, "onFailed", jsonResponse.errcode,sendMessage));
                                }
                            } catch (NullPointerException e) {
                                e.printStackTrace();
                            }
                        }
                        messageCallbackQueue.remove(sendMessage.getRequestId());
                    }
                }
                break;

                case CMD_UPLOAD_PROGRESS: {
                    UploadProgress uploadProgress = gson.fromJson(msg, UploadProgress.class);
                    YIMEventCallback.ShowUploadFileProgressCallback uploadFileProgressCallback = null;
                    boolean finded = uploadProgressCallbackQueue.containsKey(uploadProgress.getRequestId());
                    if (finded){
                        uploadFileProgressCallback = uploadProgressCallbackQueue.get(uploadProgress.getRequestId());
                        if (uploadFileProgressCallback != null)
                        {
                            if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                                uploadFileProgressCallback.onShowUploadFileProgress(uploadProgress.getPercent());
                            }
                        }
                        if (uploadProgress.getPercent() >= 100.00) {
                            uploadProgressCallbackQueue.remove(uploadProgress.getRequestId());

                        }
                    }
                }
                break;

                case CMD_DOWNLOAD: {
                    Download download = gson.fromJson( msg, Download.class );

                    YIMMessage message = new YIMMessage();
                    IYIMMessageBodyBase msgBody = null;
                    RecvMessage recvMessage = gson.fromJson(msg, RecvMessage.class);
                    message.setChatType(recvMessage.chatType);

                    message.setReceiveID(recvMessage.receiveId);
                    message.setSenderID(recvMessage.senderId);
                    message.setMeesageID(recvMessage.serial);
                    message.setMessageType(recvMessage.msgType);
                    message.setCreateTime(recvMessage.createTime);
                    message.setDistance(recvMessage.distance);
                    message.setRead(recvMessage.getIsRead());

                    if (recvMessage.msgType == YIMService.MessageBodyType.Voice) {
                        msgBody = new YIMMessageBodyAudio();
                        ((YIMMessageBodyAudio) msgBody).setAudioTime(recvMessage.duration);
                        ((YIMMessageBodyAudio) msgBody).setParam(recvMessage.param);
                        ((YIMMessageBodyAudio) msgBody).setText(recvMessage.extraText);
                    }else if (recvMessage.msgType == YIMService.MessageBodyType.File)
                    {
                        msgBody = new YIMMessageBodyFile();
                        ((YIMMessageBodyFile) msgBody).setFileType(recvMessage.fileType);
                        ((YIMMessageBodyFile) msgBody).setFileSize(recvMessage.fileSize);
                        ((YIMMessageBodyFile) msgBody).setFileExtension(recvMessage.fileExtension);
                        ((YIMMessageBodyFile) msgBody).setExtParam(recvMessage.extraParam);
                        ((YIMMessageBodyFile) msgBody).setFileName(recvMessage.fileName);
                    }

                    message.setMessageBody(msgBody);

                    YIMEventCallback.DownloadFileCallback callbackObject = null;
                    boolean finded = downloadMsgCallbackQueue.containsKey(message.getMessageID());
                    if (finded){
                        callbackObject = downloadMsgCallbackQueue.get(message.getMessageID());

                        try {
                             mHandler.post(new YIMCallBackProtocolV2(callbackObject, "onDownload", jsonResponse.errcode,message,download.savePath));

                        } catch (NullPointerException e) {
                            e.printStackTrace();
                        }

                        downloadMsgCallbackQueue.remove(message.getMessageID());
                    }

                    if (m_autoDownloadCallback != null){
                        m_autoDownloadCallback.onAutoDownload(jsonResponse.errcode, message, download.savePath);
                    }
                }
                break;
                case CMD_DOWNLOAD_URL:{
                    DownloadUrl downloadurl = gson.fromJson( msg, DownloadUrl.class );

                    YIMEventCallback.DownloadByUrlCallback callbackObject = null;
                    boolean finded = downloadByUrlCallbackQueue.containsKey(downloadurl.fromUrl);
                    if (finded){
                        callbackObject = downloadByUrlCallbackQueue.get(downloadurl.fromUrl);

                        try {
                            mHandler.post(new YIMCallBackProtocolV2(callbackObject, "onDownloadByUrl", jsonResponse.errcode,downloadurl.fromUrl,downloadurl.savePath, downloadurl.audioTime));

                        } catch (NullPointerException e) {
                            e.printStackTrace();
                        }

                        downloadByUrlCallbackQueue.remove(downloadurl.fromUrl);
                    }
                }
                break;
                case CMD_GET_USR_INFO: {
                    UserInfoString userInfoString = gson.fromJson(msg, UserInfoString.class);
                    YIMExtraUserInfo userInfo = gson.fromJson(userInfoString.userInfoString, YIMExtraUserInfo.class);
                    userInfo.setUserID(userInfoString.userID);

                    try {
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_userInfoCallback, "onSuccess", userInfo));
                        }else {
                            mHandler.post(new YIMCallBackProtocolV2(m_userInfoCallback, "onFailed", jsonResponse.errcode,userInfo));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_QUERY_USER_STATUS: {
                    UserStatusInfo userStatusInfo = gson.fromJson(msg, UserStatusInfo.class);
                    try {
                        if(m_userStatusCallback != null)
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_userStatusCallback, "onSuccess", userStatusInfo));
                        }else {
                            mHandler.post(new YIMCallBackProtocolV2(m_userStatusCallback, "onFailed", jsonResponse.errcode, userStatusInfo));
                        }
                        m_userStatusCallback = null;
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_AUDIO_PLAY_COMPLETE: {
                    OnPlayCompleteNotify onPlayCompleteNotify = gson.fromJson(msg, OnPlayCompleteNotify.class);
                    try {
                        if(m_playCompleteCallback != null) {
                            if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success) {
                                mHandler.post(new YIMCallBackProtocolV2(m_playCompleteCallback, "onSuccess", onPlayCompleteNotify.audioPath));
                            } else {
                                mHandler.post(new YIMCallBackProtocolV2(m_playCompleteCallback, "onFailed", jsonResponse.errcode, onPlayCompleteNotify.audioPath));
                            }
                            m_playCompleteCallback = null;
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_STOP_SEND_AUDIO: {
                    VoiceInfo voiceInfo = gson.fromJson(msg, VoiceInfo.class);
                    MessageCallbackObject callbackObject = null;
                    boolean finded = messageCallbackQueue.containsKey(voiceInfo.requestId);
                    if (finded){
                        callbackObject = messageCallbackQueue.get(voiceInfo.requestId);
                        if ((callbackObject != null) && (callbackObject.callback != null))
                        {
                            try {
                                mHandler.post(new YIMCallBackProtocolV2((YIMEventCallback.AudioMsgEventCallback)callbackObject.callback, "onStartSendAudioMessage", voiceInfo.requestId, jsonResponse.errcode, voiceInfo.extraText, voiceInfo.localPath,
                                    voiceInfo.duration));

                            } catch (NullPointerException e) {
                                e.printStackTrace();
                            }
                        }
                    }
                }
                break;
                case CMD_GET_DISTRICT:{
                    Log.d("CMD_GET_DISTRICT","msg = " + msg);
                    GeographyLocation geographyLocation = gson.fromJson(msg,GeographyLocation.class) ;
                    try {
                        mHandler.post(new YIMCallBackProtocolV2(m_getLocationCallback, "onUpdateLocation", jsonResponse.errcode, geographyLocation));

                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_GET_PEOPLE_NEARBY:{
                    Log.d("CMD_GET_PEOPLE_NEARBY","msg = " + msg);
                    RelativeLocationInfo relativeLocations = gson.fromJson(msg,RelativeLocationInfo.class) ;

                    try {
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_getNearbyObjCallback, "onSuccess", relativeLocations));
                        }else {
                            mHandler.post(new YIMCallBackProtocolV2(m_getNearbyObjCallback, "onFailed", jsonResponse.errcode, relativeLocations));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_GET_DISTANCE:{
                    UserDistanceInfo userDistance = gson.fromJson(msg, UserDistanceInfo.class) ;

                    try {
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_getDistanceCallback, "onSuccess", userDistance));
                        }else {
                            mHandler.post(new YIMCallBackProtocolV2(m_getDistanceCallback, "onFailed", jsonResponse.errcode, userDistance));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_TRANSLATE_COMPLETE:{
                    Log.d("CMD_TRANSLATE_COMPLETE","msg = " + msg);
                    TranlateTextInfo translateInfo = gson.fromJson(msg,TranlateTextInfo.class) ;

                    try {
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_translateTextCallback, "onSuccess", translateInfo));
                        }else {
                            mHandler.post(new YIMCallBackProtocolV2(m_translateTextCallback, "onFailed", jsonResponse.errcode, translateInfo));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_GET_MICROPHONE_STATUS: {
                    MicrophoneStatus MicrophoneStatus = gson.fromJson(msg,MicrophoneStatus.class) ;

                    try {
                        mHandler.post(new YIMCallBackProtocolV2(m_getMicStatusCallback, "onGetMicrophoneStatus", MicrophoneStatus.status));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_GET_TIPOFF_MSG: {
                    AccusationResult accusationResult = gson.fromJson(msg,AccusationResult.class);

                    try {
                        mHandler.post(new YIMCallBackProtocolV2(m_accusationCallback, "onAccusationResultNotify", accusationResult.iResult,accusationResult.strUserID,accusationResult.iAccusationTime));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_RECV_NOTICE: {
                    NoticeInfo noticeInfo = gson.fromJson(msg,NoticeInfo.class) ;

                    try {
                        mHandler.post(new YIMCallBackProtocolV2(m_noticeCallback, "onRecvNotice",noticeInfo));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_CANCEL_NOTICE: {
                    NoticeCancelInfo noticeInfo = gson.fromJson(msg,NoticeCancelInfo.class) ;

                    try {
                        mHandler.post(new YIMCallBackProtocolV2(m_noticeCallback, "onCancelNotice",noticeInfo.iNoticeID, noticeInfo.strChannelID));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_GET_FORBID_RECORD:{
                    Log.d("CMD_GET_PEOPLE_NEARBY","msg = " + msg);
                    YIMForbiddenSpeakList forbiddenList = gson.fromJson(msg,YIMForbiddenSpeakList.class) ;

                    try {
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_forbidSpeakInfoCallback, "onSuccess", forbiddenList.forbiddenList));
                        }else {
                            mHandler.post(new YIMCallBackProtocolV2(m_forbidSpeakInfoCallback, "onFailed", jsonResponse.errcode, forbiddenList.forbiddenList));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_SET_MASK_USER_MSG:{
                    BlockUserInfo block = gson.fromJson(msg, BlockUserInfo.class);

                    try {
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_blockUserCallback, "onSuccess", block));
                        }else {
                            mHandler.post(new YIMCallBackProtocolV2(m_blockUserCallback, "onFailed", jsonResponse.errcode, block));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_GET_MASK_USER_MSG:{
                    BlockUserList userList = gson.fromJson(msg, BlockUserList.class);

                    try {
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_getBlockUsersCallback, "onSuccess", userList.userList));
                        }else {
                            mHandler.post(new YIMCallBackProtocolV2(m_getBlockUsersCallback, "onFailed", jsonResponse.errcode, userList.userList));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_CLEAN_MASK_USER_MSG:{
                    try {
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_unBlockUserCallback, "onSuccess"));
                        }else {
                            mHandler.post(new YIMCallBackProtocolV2(m_unBlockUserCallback, "onFailed", jsonResponse.errcode));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_GET_ROOM_INFO:{
                    try {
                        RoomInfo roomInfo = gson.fromJson(msg, RoomInfo.class);
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_roomInfoCallback, "onSuccess", roomInfo));
                        }else {
                            mHandler.post(new YIMCallBackProtocolV2(m_roomInfoCallback, "onFailed", jsonResponse.errcode, roomInfo));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_GET_SPEECH_TEXT:{

                    RecognizeSpeechText speechTextInfo = gson.fromJson(msg, RecognizeSpeechText.class);

                    try {
                        mHandler.post(new YIMCallBackProtocolV2(m_msgEventCallback, "onGetRecognizeSpeechText",jsonResponse.errcode, speechTextInfo.requestId, speechTextInfo.text));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_LEAVE_ALL_ROOM: {
                    try {
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_leaveAllCallback, "onSuccess"));
                        }else {
                            mHandler.post(new YIMCallBackProtocolV2(m_leaveAllCallback, "onFailed", jsonResponse.errcode));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_GET_RECONNECT_RESULT: {
                    ReconnectResult reconnectResult = gson.fromJson(msg, ReconnectResult.class);

                    try {
                        mHandler.post(new YIMCallBackProtocolV2(m_reconnectCallback, "onRecvReconnectResult", reconnectResult.iResult));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_START_RECONNECT: {
                    try {
                        mHandler.post(new YIMCallBackProtocolV2(m_reconnectCallback, "onStartReconnect"));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_RECORD_VOLUME: {
                    VolumeInfo volumeInfo = gson.fromJson(msg, VolumeInfo.class);

                    try {
                        mHandler.post(new YIMCallBackProtocolV2(m_msgEventCallback, "onRecordVolume", volumeInfo.iVolume));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_GET_USER_PROFILE: {

                    UserProfileInfo profileInfo = gson.fromJson(msg,UserProfileInfo.class);

                    try {
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_userProfileCallback, "onSuccess", profileInfo));
                        }else {
                            mHandler.post(new YIMCallBackProtocolV2(m_userProfileCallback, "onFailed", jsonResponse.errcode, profileInfo));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_SET_USER_PROFILE: {
                    try {
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_setprofileCallback, "onSuccess"));
                        }else {
                            mHandler.post(new YIMCallBackProtocolV2(m_setprofileCallback, "onFailed", jsonResponse.errcode));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_SWITCH_USER_STATE: {
                    try {
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_switchStatusCallback, "onSuccess"));
                        }else {
                            mHandler.post(new YIMCallBackProtocolV2(m_switchStatusCallback, "onFailed", jsonResponse.errcode));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_SET_USER_PHOTO: {
                    PhotoUrlInfo photoUrlInfo = gson.fromJson(msg, PhotoUrlInfo.class);

                    try {
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_photoUrlCallback, "onSuccess", photoUrlInfo.photoUrl));
                        }else {
                            mHandler.post(new YIMCallBackProtocolV2(m_photoUrlCallback, "onFailed", jsonResponse.errcode, photoUrlInfo.photoUrl));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_FIND_FRIEND_BY_ID: {
                    YIMUserBriefInfoList briefInfoList = gson.fromJson(msg, YIMUserBriefInfoList.class);

                    try {
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_findUserCallback, "onSuccess", briefInfoList.userList));
                        }else {
                            mHandler.post(new YIMCallBackProtocolV2(m_findUserCallback, "onFailed", jsonResponse.errcode, briefInfoList.userList));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_REQUEST_ADD_FRIEND: {
                    YIMFriendUserID userID = gson.fromJson(msg, YIMFriendUserID.class);

                    try {
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_requestAddCallback, "onSuccess", userID.userID));
                        }else {
                            mHandler.post(new YIMCallBackProtocolV2(m_requestAddCallback, "onFailed", jsonResponse.errcode, userID.userID));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_REQUEST_ADD_FRIEND_NOTIFY: {
                    try {
                        YIMFriendCommon commonInfo = gson.fromJson(msg, YIMFriendCommon.class);
                        mHandler.post(new YIMCallBackProtocolV2(m_friendNotifyCallback, "onBeRequestAddFriendNotify", commonInfo.userID, commonInfo.comments));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_BE_ADD_FRIENT: {
                    try {
                        YIMFriendCommon commonInfo = gson.fromJson(msg, YIMFriendCommon.class);
                        mHandler.post(new YIMCallBackProtocolV2(m_friendNotifyCallback, "onBeAddFriendNotify", commonInfo.userID, commonInfo.comments));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_DEAL_ADD_FRIEND: {
                    YIMFriendDealResult dealResult = gson.fromJson(msg, YIMFriendDealResult.class);

                    try {
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_dealAddCallback, "onSuccess", dealResult));
                        }else {
                            mHandler.post(new YIMCallBackProtocolV2(m_dealAddCallback, "onFailed", jsonResponse.errcode, dealResult));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_ADD_FRIENT_RESULT_NOTIFY: {
                    YIMFriendDealResult dealResult = gson.fromJson(msg, YIMFriendDealResult.class);

                    try {
                        mHandler.post(new YIMCallBackProtocolV2(m_friendNotifyCallback, "onRequestAddFriendResultNotify", dealResult.userID, dealResult.comments, dealResult.dealResult));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_DELETE_FRIEND: {
                    YIMFriendUserID userID = gson.fromJson(msg, YIMFriendUserID.class);

                    try {
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_deleteFriendCallback, "onSuccess", userID.userID));
                        }else {
                            mHandler.post(new YIMCallBackProtocolV2(m_deleteFriendCallback, "onFailed", jsonResponse.errcode, userID.userID));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_BE_DELETE_FRIEND_NOTIFY: {
                    try {
                        YIMFriendUserID userID = gson.fromJson(msg, YIMFriendUserID.class);
                        mHandler.post(new YIMCallBackProtocolV2(m_friendNotifyCallback, "onBeDeleteFriendNotify", userID.userID));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_BLACK_FRIEND: {
                    YIMBlackFriendInfo blackInfo = gson.fromJson(msg, YIMBlackFriendInfo.class);

                    try {
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_blackFriendCallback, "onSuccess", blackInfo));
                        }else {
                            mHandler.post(new YIMCallBackProtocolV2(m_blackFriendCallback, "onFailed", jsonResponse.errcode, blackInfo));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_BE_BLACK_FRIEND_NOTIFY: {
                    try {
                        YIMFriendUserID userID = gson.fromJson(msg, YIMFriendUserID.class);
                        mHandler.post(new YIMCallBackProtocolV2(m_friendNotifyCallback, "onBeBlackNotify", userID.userID));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_QUERY_FRIEND_LIST: {
                    YIMFriendListInfo friendList = gson.fromJson(msg, YIMFriendListInfo.class);

                    try {
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_queryFriendsCallback, "onSuccess", friendList));
                        }else {
                            mHandler.post(new YIMCallBackProtocolV2(m_queryFriendsCallback, "onFailed", jsonResponse.errcode, friendList));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_QUERY_FRIEND_REQUEST_LIST: {
                    YIMFriendRequestInfoList requestList = gson.fromJson(msg, YIMFriendRequestInfoList.class);

                    try {
                        if (jsonResponse.errcode == YIMConstInfo.Errorcode.Success){
                            mHandler.post(new YIMCallBackProtocolV2(m_requestListCallback, "onSuccess", requestList));
                        }else {
                            mHandler.post(new YIMCallBackProtocolV2(m_requestListCallback, "onFailed", jsonResponse.errcode, requestList));
                        }
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_HXR_USER_INFO_CHANGE_NOTIFY: {
                    try {
                        YIMFriendUserID userID = gson.fromJson(msg, YIMFriendUserID.class);
                        mHandler.post(new YIMCallBackProtocolV2(m_userProfileChangeCallback, "onUserInfoChangeNotify", userID.userID));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_SET_READ_STATUS: {
                    MsgReadStatus readStatus = gson.fromJson(msg, MsgReadStatus.class);
                    try {
                        mHandler.post(new YIMCallBackProtocolV2(m_updateReadStatusCallback, "onRead",readStatus.getRecvId(), readStatus.getChatType(), readStatus.getMsgId()));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
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

