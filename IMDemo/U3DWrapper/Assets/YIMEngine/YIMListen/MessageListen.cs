using System.Collections.Generic;


namespace YIMEngine
{
	public interface MessageListen
	{
		void OnSendMessageStatus(ulong iRequestID,  YIMEngine.ErrorCode errorcode, uint sendTime, bool isForbidRoom, int reasonType, ulong forbidEndTime);
		//接收到用户发来的消息
		void OnRecvMessage(YIMEngine.MessageInfoBase message);
        //开始发送语音回调通知（调用StopAudioMessage停止语音之后，发送语音消息之前）
        void OnStartSendAudioMessage(ulong iRequestID, YIMEngine.ErrorCode errorcode, string strText, string strAudioPath, int iDuration);
		//语音发送回调
        void OnSendAudioMessageStatus(ulong iRequestID, YIMEngine.ErrorCode errorcode, string strText, string strAudioPath, int iDuration, uint sendTime, bool isForbidRoom, int reasonType, ulong forbidEndTime);

		/// <summary>
        /// 获取本地私聊历史记录的结果通知
        /// </summary>
        /// <param name="errorcode">错误码</param>
        /// <param name="targetID">用户id</param>
        /// <param name="remain">剩余历史消息条数</param>
        /// <param name="messageList">消息列表</param>
		void OnQueryHistoryMessage(YIMEngine.ErrorCode errorcode, string targetID, int remain, List <YIMEngine.HistoryMsg> messageList);

        void OnQueryRoomHistoryMessageFromServer(YIMEngine.ErrorCode errorcode, string roomID, int remain, List<YIMEngine.MessageInfoBase> messageList);

		/// <summary>
        /// StartAudioSpeech 接口对应的结果回调通知
        /// </summary>
        /// <param name="errorcode">错误码，ErrorCode.Success为成功</param>
        /// <param name="iRequestID">消息id</param>
        /// <param name="strDownloadURL">amr文件下载地址，接收方下载后可以调用ConvertAMRToWav转成wav播放</param>
        /// <param name="iDuration">录音时长，单位秒</param>
        /// <param name="iFileSize">文件大小，字节</param>
        /// <param name="strLocalPath">本地语音文件路径</param>
        /// <param name="strText">语音识别结果，可能为空null or ""</param>
		void OnStopAudioSpeechStatus(YIMEngine.ErrorCode errorcode, ulong iRequestID,string strDownloadURL,int iDuration,int iFileSize,string strLocalPath,string strText);
		
		//新消息通知
        void OnRecvNewMessage(YIMEngine.ChatType chatType,string targetID);

        //void OnTranslateTextComplete(YIMEngine.ErrorCode errorcode, uint requestID, string text, YIMEngine.LanguageCode destLangCode);

        void OnAccusationResultNotify(AccusationDealResult result, string userID, uint accusationTime);

		void OnGetForbiddenSpeakInfo( YIMEngine.ErrorCode errorcode, List<ForbiddenSpeakInfo> forbiddenSpeakList );

        void OnGetRecognizeSpeechText(ulong iRequestID, YIMEngine.ErrorCode errorcode, string text);

        void OnBlockUser(YIMEngine.ErrorCode errorcode, string userID, bool block);

	    void OnUnBlockAllUser(YIMEngine.ErrorCode errorcode);

	    void OnGetBlockUsers(YIMEngine.ErrorCode errorcode, List<string> userList);

		void OnRecordVolumeChange(float volume);
	}

}