package com.youme.imsdk.callback;

import com.youme.imsdk.YIMClient;

import com.youme.imsdk.YIMMessage;

import com.youme.imsdk.internal.GeographyLocation;
import com.youme.imsdk.internal.NoticeInfo;
import com.youme.imsdk.internal.SendVoiceMsgInfo;
import com.youme.imsdk.internal.SpeechMessageInfo;
import com.youme.imsdk.internal.UserChatRoom;

import java.util.ArrayList;

/**
 * Created by winnie on 2018/7/30.
 */

public class YIMEventCallback {


    public interface ResultCallback<T> {

        public void onSuccess(T info);
        public void onFailed(int errorcode, T info);
    }

    public interface OperationCallback {
        public void onSuccess();
        public void onFailed(int errorcode);
    }


    public interface DownloadFileCallback {
        /*
	    * 功能：下载回调（根据messageID下载）
	    * @param errorcode：错误码
	    * @param message 消息
	    * @param savePath 保存路径
	    */
        public void onDownload(int errorcode, YIMMessage message , String savePath);
    }

    public interface DownloadByUrlCallback {
        /*
	    * 功能：下载回调
	    * @param errorcode：错误码
	    * @param fromUrl 下载URL
	    * @param savePath 保存路径
	    */
        public void onDownloadByUrl(int errorcode, String fromUrl,  String savePath,int audioTime);
    }


    public interface AudioMsgEventCallback {
        /*
	    * 功能：停止语音回调（发送端停止语音，发送语音消息之前，发送端可在此时显示消息）
	    * @param requestID：请求ID（与startRecordAudioMessage输出参数requestID一致）
	    * @param errorcode：错误码
	    * @param text：语音识别结果
	    * @param audioPath：语音文件路径
	    * @param audioTime：语音时长（单位：秒）
	    */
        public void onStartSendAudioMessage(long requestID, int errorcode, String strText, String strAudioPath, int audioTime);

        /*
	    * 功能：发送语音消息回调
	    * @param errorcode：错误码
	    * @param voiceMsgInfo：语音信息
	    */
        public void onSendAudioMessageStatus(int errorcode, SendVoiceMsgInfo voiceMsgInfo);
    }

    public interface SpeechEventCallback {
        /*
	    * 功能：只录音语音结束回调（只录音接口startAudioSpeech和stopAudioSpeech）
	    * @param errorcode：错误码
	    * @param speechMsgInfo：语音消息
	    */
        public void onStopAudioSpeechStatus(int errorcode, SpeechMessageInfo speechMsgInfo);
    }

    public interface GetMicStatusCallback {
        /*
	    * 功能：获取麦克风状态回调
	    * @param status：麦克风状态
	    */
        public void onGetMicrophoneStatus(int status);
    }


    public interface KickOffCallback {
        /*
	    * 功能：被踢下线通知
	    */
        public void onKickOff();
    }

    public interface UserJoinLeaveChannelCallback {
        /*
        * 功能：其他用户进出频道通知
        * @param eventType：频道事件类型
        * @param info：用户和频道信息
        */
        public void joinLeaveNotify(YIMClient.ChannelEventType eventType, UserChatRoom info);
    }

    public interface MessageEventCallback {
        /*
	    * 功能：新消息通知（默认自动接收消息，只有调用setReceiveMessageSwitch设置为不自动接收消息，才会收到该回调）
	    * @param chatType：聊天类型
	    * @param targetID：频道ID
	    */
        public void onRecvNewMessage(int chatType, String targetID);

        /*
	    * 功能：接收消息回调
	    * @param message：消息
	    */
        public void onRecvMessage(YIMMessage message);

        /*
	    * 功能：仅语音识别回调（调用startRecordAudioMessage时开启仅需要语音识别结果才会有该回调，不会发送语音消息，停止语音后产生该回调）
	    * @param errorcode：错误码
	    * @param requestID：请求ID（与startRecordAudioMessage输出参数requestID一致）
	    * @param text：语音识别结果
	    */
        public void onGetRecognizeSpeechText(int errorcode, long requestID, String text);

        /*
	    * 功能：录音音量变化回调
	    * @param volume：音量值(0到1)
	    */
        public void onRecordVolume(float volume);
    }

    public interface AutoDownloadVoiceCallback {
        /*
	    * 功能：自动下载回调
	    * @param errorcode：错误码
	    * @param message 消息
	    * @param savePath 保存路径
	    */
        public void onAutoDownload(int errorcode, YIMMessage message , String savePath);
    }

    public interface NoticeCallback {
        /*
	    * 功能：接收公告通知
	    * @param notice：公告信息
	    */
        public void onRecvNotice(NoticeInfo notice);

        /*
	    * 功能：撤销公告通知
	    * @param noticeID：公告ID
	    * @param channelID：频道ID
	    */
        public void onCancelNotice(long noticeID, String channelID);
    }

    public interface AccusationResultCallback {
        /*
    	* 功能：举报结果通知
    	* @param result：举报处理结果
	    * @param userID：用户ID
	    * @param accusationTime：举报时间戳
    	*/
        public void onAccusationResultNotify(int result, String userID, int accusationTime);
    }

    public interface GetLocationCallback {
        /*
    	* 功能：地理位置回调
    	* @param errorcode：错误码
    	* @param location：地理位置信息
    	*/
        public void onUpdateLocation(int errorcode, GeographyLocation location);
    }

    public interface ReconnectCallback {
        /*
    	* 功能：开始重连通知
    	*/
        public void onStartReconnect();

        /*
    	* 功能：重连结果通知
    	* @param result：重连结果
    	*/
        public void onRecvReconnectResult(int result);
    }

    public interface UserProfileChangeCallback {
        /*
    	* 功能：用户信息变更通知
    	* @param userID：用户ID
    	*/
        public void onUserInfoChangeNotify(String userID);
    }

    public interface FriendNotifyCallback {
        /*
    	* 功能：被邀请添加好友通知（需要验证）
    	* @param userID：用户ID
    	* @param comments：备注或验证信息
    	* commonts：显示用户信息可以根据userID查询
    	*/
        public void onBeRequestAddFriendNotify(String userID, String comments);

        /*
    	* 功能：被添加为好友通知（不需要验证）
    	* @param userID：用户ID
    	* @param comments：备注或验证信息
    	*/
        public void onBeAddFriendNotify(String userID, String comments);

        /*
    	* 功能：请求添加好友结果通知(需要好友验证，待被请求方处理后回调)
    	* @param userID：用户ID
    	* @param comments：备注或验证信息
    	* @param dealResult：处理结果	0：同意	1：拒绝
    	*/
        public void onRequestAddFriendResultNotify(String userID, String comments, int dealResult);

        /*
    	* 功能：被好友删除通知
    	* @param userID：用户ID
    	*/
        public void onBeDeleteFriendNotify(String userID);
    }

    public interface ShowUploadFileProgressCallback {
    	public void onShowUploadFileProgress(float percent);
	}

	public interface UpdateReadStatusCallback {
    	public void onRead(String recvId, int chatType, long msgId);
	}
}
