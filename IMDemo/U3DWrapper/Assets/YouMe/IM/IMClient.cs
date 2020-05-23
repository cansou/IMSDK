	using System;
using YIMEngine;
using System.Collections.Generic;

namespace YouMe
{

    public class IMClient : IClient
    {
        static IMClient _ins;
        public static IMClient Instance{
            get{
                if(_ins==null){
                    _ins = new IMClient();
                }
                return _ins;
            }
        }

        private IMClient(){
            IMManager = IMInternalManager.Instance;
            ConnectListener = OnConnect;
            ChannelEventListener = OnChannelEvent;
        }
        ~IMClient(){
            _downloadDirPath = "";
        }

        public static string FAKE_PAPSSWORD = "123456";
        public string _downloadDirPath = "";   

        public IMInternalManager IMManager;

        // event 
        public Action<IMConnectEvent> ConnectListener{set;get;}
        public Action<ChannelEvent> ChannelEventListener{set;get;}

		public Action<IMReconnectEvent> ReconnectListener{set;get;}
        public Action<OtherUserChannelEvent> OtherUserChannelEventListener{set;get;}
		public Action<IMMessage> ReceiveMessageListener{set; get;}

		public Action<string, int, ulong> UpdateReadStatusListener{set; get;}

		public Action<YouMe.StatusCode, string> PlayListener{set; get;}
		public Action<YouMe.StatusCode, string, uint> GetRoomMemberCountListener{set; get;}
		public Action<YouMe.StatusCode, IMHistoryMessageInfo> QueryHistoryMesListener{set; get;}
		public Action<YouMe.StatusCode, string, uint, List<IMMessage>> QueryRoomHistoryMsgFromServerListener{set;get;}
		public Action<YouMe.StatusCode, List<ContactsSessionInfo> > GetContactListener{set;get;}
		public Action<YouMe.StatusCode, IMUserInfo> GetUserInfoListener{set;get;}
		public Action<YouMe.StatusCode, YIMUserStatus> QueryUserStatusListener{set;get;}
		public Action<YouMe.StatusCode, IMNearbyObjectInfo> GetNearbyObjectsListener{set;get;}
		public Action<YouMe.StatusCode, IMAudioDeviceStatus> GetMicrophoneStatusListener{set;get;}
		public Action<YouMe.StatusCode, List<ForbiddenSpeakInfo> > GetForbiddenSpeakInfoListener{set;get;}
		public Action<YouMe.StatusCode, bool> BlockUserListener{set;get;}
		public Action<YouMe.StatusCode> UnblockAllUserListener{set;get;}
		public Action<YouMe.StatusCode, List<string> > GetBlockUsersListener{set;get;}

		public Action<YouMe.StatusCode, GeographyLocation> GetCurrentLocationListener{set;get;}
		public Action<YouMe.StatusCode, ulong, string> GetRecognizeSpeechTextListener{set;get;}
		public Action<YIMEngine.Notice> RecvNoticeListener{set;get;} 
		public Action<ulong, string> CancelNoticeListener{set;get;}
		public Action<float> RecordVolumeListener{set;get;}
		public Action<ChatType, string> RecvNewMessageListener{set;get;}
		public Action<YouMe.StatusCode, AudioMessage, string> DownloadListener{set;get;}
		public Action<YouMe.StatusCode, IMAccusationInfo> AccusationListener{set;get;}


        private Action<LoginEvent> loginCallback;
        private Action<LogoutEvent> logoutCallback;
        private Action<KickOffEvent> kickOffCallback;
        private Action<DisconnectEvent> disconnectCallback;

        private Action<ChannelEvent> joinChannelCallback = null;
        private Action<ChannelEvent> leaveChannelCallback = null;

		private Action<ErrorCode,string, LanguageCode, LanguageCode> TranslateListener;
		private Action<StatusCode,string, LanguageCode, LanguageCode> translateCallback;
                	

        private AudioMessage lastRecordAudioMessage;
        private SpeechInfo lastRecordSpeechMessage;

        /// <summary>
        /// 设置配置信息
        /// </summary>
        /// <param name="appKey">游密官方分配的APP唯一标识，在www.youme.im注册后可以自助获取</param>
        /// <param name="secretKey">游密官方分配的接入密钥，和appkey配对使用，在www.youme.im注册后可以自助获取</param>
        /// <param name="config">SDK配置对象，可以通过该对象的参数设置SDK的可选参数，比如服务器区域、日志级别、方言识别</param>
		/// <returns>返回 YouMe.StatusCode</returns> 
        public StatusCode Initialize (string appKey, string secretKey, Config config)
		{
            YIMEngine.ErrorCode code = IMAPI.Instance().Init(appKey, secretKey,config!=null ? (YIMEngine.ServerZone)config.ServerZone : YIMEngine.ServerZone.China);
            return (YouMe.StatusCode)code;
        }

		/*************************************************************************
		 *                                登录登出                                *		 
		 *************************************************************************/

        /// <summary>
        /// 登录IM系统。成功登录IM系统后就可以进行私聊消息的收发，以及进出聊天频道，进行频道消息的收发。
        /// </summary>
        /// <param name="userID">用户ID或者游戏角色ID，唯一标识一个用户在应用里的身份</param>
        /// <param name="token">使用服务器token验证模式时使用该参数，否则使用空字符串""即可</param>
        /// <param name="callback">登录结果的回调通知，在此回调里判读登录是否成功</param>
        public void Login(string userID, string token, Action<LoginEvent> callback)
        {             
            loginCallback = callback;
            YIMEngine.ErrorCode code = IMAPI.Instance().Login(userID, FAKE_PAPSSWORD, token);
            if( code!=YIMEngine.ErrorCode.Success && ConnectListener != null ){
                IMConnectEvent e = new IMConnectEvent(Conv.ErrorCodeConvert(code),ConnectEventType.CONNECT_FAIL,userID);
                ConnectListener( e );
            }
            
        }        

		/// <summary>
        /// 登出IM系统。
        /// </summary>
        /// <param name="callback">登出结果的回调通知，在此回调里判读登出是否成功</param>
        public void Logout(Action<LogoutEvent> callback)
        {           
            logoutCallback = callback;
            YIMEngine.ErrorCode code = IMAPI.Instance().Logout();
            if( code!=YIMEngine.ErrorCode.Success && ConnectListener != null )
            {
                IMConnectEvent e = new IMConnectEvent(Conv.ErrorCodeConvert(code),ConnectEventType.DISCONNECTED,GetCurrentUserID().UserID);
                ConnectListener( e );
            }
        }

        public IUser GetCurrentUserID()
        {
            return IMManager.LastLoginUser;
        }

		/*************************************************************************
		 *                                频道接口                                *		 
		 *************************************************************************/
		/// <summary>
        /// 加入频道
        /// </summary>
		/// <param name="channel">频道id</param>        
        /// <param name="callback">加入频道结果的回调通知</param>
		public void JoinChannel(IMChannel channel,Action<ChannelEvent> callback)
        {
            joinChannelCallback = callback;
            var code = IMAPI.Instance().JoinChatRoom( channel.ChannelID );
            if( code!=YIMEngine.ErrorCode.Success && ChannelEventListener!=null )
            {
                ChannelEventListener(new ChannelEvent( Conv.ErrorCodeConvert(code),ChannelEventType.JOIN_FAIL,channel.ChannelID ));
            }
        }

		/// <summary>
        /// 加入多个频道
        /// </summary>
		/// <param name="channels">频道id数组</param>        
        /// <param name="callback">加入频道结果的回调通知</param>
        public void JoinMultiChannel(IMChannel[] channels,Action<ChannelEvent> callback)
        {
            joinChannelCallback = callback;
            for (int i = 0; i < channels.Length;i++){
                var code = IMAPI.Instance().JoinChatRoom(channels[i].ChannelID);
                if( code!=YIMEngine.ErrorCode.Success && ChannelEventListener!=null )
                {
                    ChannelEventListener(new ChannelEvent( Conv.ErrorCodeConvert(code),ChannelEventType.JOIN_FAIL,channels[i].ChannelID ));
                }
            }
        }

		/// <summary>
        /// 离开频道
        /// </summary>	
		/// <param name="channel">频道id</param> 	       
        /// <param name="callback">离开频道结果的回调通知</param>
        public void LeaveChannel(IMChannel channel,Action<ChannelEvent> callback)
        {
            leaveChannelCallback = callback;
            var code = IMAPI.Instance().LeaveChatRoom( channel.ChannelID );
            if( code!=YIMEngine.ErrorCode.Success && ChannelEventListener!=null )
            {
                ChannelEventListener(new ChannelEvent( Conv.ErrorCodeConvert(code),ChannelEventType.LEAVE_FAIL,channel.ChannelID ));
            }
        }

		/// <summary>
        /// 离开所有频道
        /// </summary>				       
        /// <param name="callback">离开所有频道结果的回调通知</param>
        public void LeaveAllChannels (Action<ChannelEvent> callback)
		{
			leaveChannelCallback = callback;
			var code = IMAPI.Instance ().LeaveAllChatRooms ();
			if (code != YIMEngine.ErrorCode.Success && ChannelEventListener != null) 
			{
			    ChannelEventListener(new ChannelEvent(Conv.ErrorCodeConvert(code), ChannelEventType.LEAVE_ALL_FAIL, ""));
			}
        }

		/// <summary>
        /// 离开所有频道后重新进入频道
        /// </summary>	
		/// <param name="channel">频道id数组</param> 	       
		/// <param name="leaveCallback">离开所有频道结果的回调通知</param>
		/// <param name="joinCallback">加入频道结果的回调通知</param>
        public void SwitchToChannels(IMChannel[] channel,Action<ChannelEvent> leaveCallback,Action<ChannelEvent> joinCallback)
        {
            LeaveAllChannels(leaveCallback);
            JoinMultiChannel(channel,joinCallback);
        }

		/// <summary>
        /// 获取频道成员数量
        /// </summary>	
		/// <param name="channel">频道id</param> 	       
		/// <param name="callback">获取频道成员数量结果的回调通知</param>
		public void GetRoomMemberCount (IMChannel channel, Action<YouMe.StatusCode, string, uint> callback)
		{
			GetRoomMemberCountListener = callback;
			YIMEngine.ErrorCode code = 0;
			code = IMAPI.Instance ().GetRoomMemberCount (channel.ChannelID);
			if (code != YIMEngine.ErrorCode.Success && callback != null){
			    callback(Conv.ErrorCodeConvert(code), channel.ChannelID, 0);
			}		    
		}

		/*************************************************************************
		 *                               发送消息接口                              *		 
		 *************************************************************************/

        /// <summary>
        /// 发送文本消息
        /// </summary>
        /// <param name="reciverID">接收者id，私聊就用用户id，频道聊天就用频道id</param>
        /// <param name="chatType">私聊消息还是频道消息</param>
        /// <param name="msgContent">文本消息内容</param>
        /// <param name="OnSendCallBack">消息发送结果的回调通知</param>
        /// <returns>返回 TextMessage 实例</returns>
		public TextMessage SendTextMessage (string reciverID, ChatType chatType, string msgContent, string extraParam, Action<YouMe.StatusCode,TextMessage> OnSendCallBack)
		{
			ulong reqID = 0;
			YIMEngine.ErrorCode code = IMAPI.Instance ().SendTextMessage (reciverID, (YIMEngine.ChatType)chatType, msgContent, extraParam, ref reqID);
			var msg = new TextMessage (GetCurrentUserID ().UserID, reciverID, chatType, msgContent,extraParam, false);

			if (code == YIMEngine.ErrorCode.Success) {
				msg.SendStatus = SendStatus.Sending;
				msg.RequestID = reqID;
				MessageCallbackObject callbackObj = new MessageCallbackObject (msg, MessageBodyType.TXT, OnSendCallBack);
				bool ret = IMInternalManager.Instance.AddMessageCallback (reqID, callbackObj);
				if (!ret && OnSendCallBack != null) {
					OnSendCallBack (YouMe.StatusCode.Is_Waiting_Send, msg);
				}
            }else{
                msg.SendStatus = SendStatus.Fail;
                if(OnSendCallBack!=null){
                    OnSendCallBack(Conv.ErrorCodeConvert(code),msg);
                }
            }
            return msg;
        }

		/// <summary>
		/// 当用户点击聊天框，通知对端消息已读
        /// </summary>
		/// <param name="sendId">对端发送消息的userId</param>
		/// <param name="chatType">聊天类型，详见ChatType</param>
		/// <param name="msgId">最新消息的msgId</param>
    	public void sendMessageReadStatus(string sendId, ChatType chatType, ulong msgId) {
			IMAPI.Instance ().sendMessageReadStatus (sendId, (YIMEngine.ChatType)chatType, msgId);
		}

		/// <summary>
		/// 群发文本消息
        /// </summary>
		/// <param name="recvLists">接收者id数组</param>
		/// <param name="strText">文本消息内容</param>
		/// <returns>返回 YouMe.StatusCode </returns>        
		public StatusCode MultiSendTextMessage(List<string> recvLists,string strText)
		{				
			YIMEngine.ErrorCode code = IMAPI.Instance().MultiSendTextMessage(recvLists, strText);
			return (YouMe.StatusCode)code;
		}

		/// <summary>
		/// 发送礼物
        /// </summary>
		/// <param name="anchorID">主播ID</param>
		/// <param name="channel">频道ID</param>
		/// <param name="giftID">礼物ID</param>
		/// <param name="giftCount">礼物数量</param>
		/// <param name="extParam">附加参数 格式为json {"nickname":"","server_area":"","location":"","score":"","level":"","vip_level":"","extra":""}</param>
        /// <param name="OnSendCallBack">礼物消息发送结果的回调通知</param> 
		public void SendGift (string anchorID, IMChannel channel, int giftID, int giftCount, ExtraGifParam extParam, Action<YouMe.StatusCode,GiftMessage> OnSendCallBack)
		{			
			ulong serial = 0;
			YIMEngine.ErrorCode code = IMAPI.Instance ().SendGift (anchorID, channel.ChannelID, giftID, giftCount, extParam, ref serial);

			var msg = new GiftMessage (GetCurrentUserID ().UserID, anchorID, giftID, giftCount, extParam, false);
			if (code == YIMEngine.ErrorCode.Success) {
				msg.SendStatus = SendStatus.Sending;
				msg.RequestID = serial;

				MessageCallbackObject callbackObj = new MessageCallbackObject (msg, MessageBodyType.Gift, OnSendCallBack);
				bool ret = IMInternalManager.Instance.AddMessageCallback (serial, callbackObj);
				if (!ret && OnSendCallBack != null) {
					OnSendCallBack (YouMe.StatusCode.Is_Waiting_Send, msg);
				}
			} else {
				msg.SendStatus = SendStatus.Fail;
				if (OnSendCallBack != null) {
					OnSendCallBack (Conv.ErrorCodeConvert (code), msg);
				}
			}
		}

		/// <summary>
        /// 发送自定义消息
        /// </summary>
        /// <param name="reciverID">接收者id，私聊就用用户id，频道聊天就用频道id</param>
        /// <param name="chatType">私聊消息还是频道消息</param>
		/// <param name="customMsg">自定义消息内容</param>
        /// <param name="OnSendCallBack">消息发送结果的回调通知</param>       
		public void SendCustomMessage (string reciverID, ChatType chatType, byte[] customMsg, Action<YouMe.StatusCode,CustomMessage> OnSendCallBack)
		{
			ulong reqID = 0;
			YIMEngine.ErrorCode code = IMAPI.Instance ().SendCustomMessage (reciverID, (YIMEngine.ChatType)chatType, customMsg, ref reqID);
			var msg = new CustomMessage (GetCurrentUserID ().UserID, reciverID, chatType, customMsg, false);
			if (code == YIMEngine.ErrorCode.Success) {
				msg.SendStatus = SendStatus.Sending;
				msg.RequestID = reqID;
				MessageCallbackObject callbackObj = new MessageCallbackObject (msg, MessageBodyType.CustomMesssage, OnSendCallBack);
				bool ret = IMInternalManager.Instance.AddMessageCallback (reqID, callbackObj);
				if (!ret && OnSendCallBack != null) {
					OnSendCallBack (YouMe.StatusCode.Is_Waiting_Send, msg);
				}
			}else{
				msg.SendStatus = SendStatus.Fail;
				if (OnSendCallBack != null) {
					OnSendCallBack (Conv.ErrorCodeConvert (code), msg);
				}
			}
		}

		/// <summary>
        /// 发送文件
        /// </summary>
        /// <param name="reciverID">接收者id，私聊就用用户id，频道聊天就用频道id</param>
        /// <param name="chatType">私聊消息还是频道消息</param>
		/// <param name="filePath">文件绝对路径</param>
		/// <param name="extParam">附加参数</param>
		/// <param name="fileType">文件类型</param>
        /// <param name="OnSendCallBack">消息发送结果的回调通知</param>       
		public void SendFile (string reciverID, ChatType chatType, string filePath, string extParam, FileType fileType, Action<YouMe.StatusCode,FileMessage> OnSendCallBack)
		{
			ulong reqID = 0;
			YIMEngine.ErrorCode code = IMAPI.Instance ().SendFile (reciverID, (YIMEngine.ChatType)chatType, filePath, extParam, (YIMEngine.FileType)fileType, ref reqID);
			var msg = new FileMessage (GetCurrentUserID ().UserID, reciverID, chatType, extParam, fileType, false);
			if (code == YIMEngine.ErrorCode.Success) {
				msg.SendStatus = SendStatus.Sending;
				msg.RequestID = reqID;

				MessageCallbackObject callbackObj = new MessageCallbackObject (msg, MessageBodyType.File, OnSendCallBack);

				bool ret = IMInternalManager.Instance.AddMessageCallback (reqID, callbackObj);
				if (!ret && OnSendCallBack != null) {
					OnSendCallBack (YouMe.StatusCode.Is_Waiting_Send, msg);
				}
			}else{
				msg.SendStatus = SendStatus.Fail;
				if (OnSendCallBack != null){
					OnSendCallBack (Conv.ErrorCodeConvert(code),msg);
				}
			}
		}

		/// <summary>
		/// 是否自动下载语音消息,下载的路径是默认路径，下载回调中可以得到
        /// </summary>
		/// <param name="download">true: 自动下载语音消息， false:不自动下载语音消息(默认)</param>
		/// <returns> YouMe.StatusCode </returns>
		public StatusCode SetAutoDownloadAudioMessage (bool download)
		{
			YIMEngine.ErrorCode code = IMAPI.Instance ().SetDownloadAudioMessageSwitch (download);
		    return (YouMe.StatusCode)code;
		}

		/// <summary>
        /// 启动录音
        /// </summary>
        /// <param name="reciverID">接收者id，私聊就用用户id，频道聊天就用频道id</param>
        /// <param name="chatType">私聊消息还是频道消息</param>
        /// <param name="extraMsg">附带自定义文本消息内容</param>
		/// <param name="recognizeText">是否开启语音转文字识别功能</param> 		
		/// <param name="IsOpenOnlyRecognizeText">可选参数，仅需要语音识别的文本内容，不发送语音；默认不开启，未开启语音转文字功能忽略此参数, 若此值为真，调用StopAudioMessage成功后会收到OnGetRecognizeSpeechText回调</param> 		   
        /// <returns> YouMe.StatusCode </returns>
		public StatusCode StartRecordAudio (string reciverID, ChatType chatType, string extraMsg, bool recognizeText, bool IsOpenOnlyRecognizeText = false)
		{
			ulong reqID = 0;
			YIMEngine.ErrorCode code = 0;

			if (recognizeText) {				
				if (IsOpenOnlyRecognizeText) {
					IMAPI.Instance ().SetOnlyRecognizeSpeechText (true);
				} else {
					IMAPI.Instance ().SetOnlyRecognizeSpeechText (false);
				}
				code = IMAPI.Instance().SendAudioMessage(reciverID, (YIMEngine.ChatType)chatType, ref reqID);
			}else{
				code = IMAPI.Instance().SendOnlyAudioMessage(reciverID, (YIMEngine.ChatType)chatType, ref reqID);
			}
			var msg = new AudioMessage(GetCurrentUserID().UserID,reciverID,chatType,extraMsg,false);
			if(code == YIMEngine.ErrorCode.Success){
				msg.RequestID = reqID;
				msg.SendStatus = SendStatus.NotStartSend;
				lastRecordAudioMessage = msg;
				return YouMe.StatusCode.Success;
			}else{
				msg.SendStatus = SendStatus.Fail;
				Log.e("Start Record Fail! code:"+code.ToString());
				return (YouMe.StatusCode)code;
			}
		}

        /// <summary>
        /// 结束录音并发送语音消息
		/// </summary>
		/// <param name="callback">语音消息发送回调通知，会通知多次，通过AudioMessage的sendStatus属性可以判断是哪个状态的回调</param>       
		public void StopRecordAndSendAudio (Action<YouMe.StatusCode,AudioMessage> callback)
		{
	        YouMe.StatusCode errorcode = YouMe.StatusCode.PTT_NotStartRecord;
			if (lastRecordAudioMessage == null) {
				Log.e ("Has no start record!");
				callback(errorcode, null);
				return;
			}
			var audioMsg = lastRecordAudioMessage;
		
			YIMEngine.ErrorCode code = IMAPI.Instance ().StopAudioMessage (audioMsg.ExtraParam);
			lastRecordAudioMessage = null;
						
			if (code == YIMEngine.ErrorCode.Success) {
				MessageCallbackObject callbackObj = new MessageCallbackObject (audioMsg, MessageBodyType.Voice, callback);

				bool ret = IMInternalManager.Instance.AddMessageCallback (audioMsg.RequestID, callbackObj);
				if (!ret && callback!=null){					
					callback(YouMe.StatusCode.Is_Waiting_Send,audioMsg);				  
				}
			}else{
				audioMsg.SendStatus = SendStatus.Fail;
				errorcode = Conv.ErrorCodeConvert(code);

				if( callback!=null ){
					callback(errorcode,audioMsg);
				}
			}
		}

		/// <summary>
        /// 取消录音
        /// </summary>
        /// <returns> YouMe.StatusCode </returns>
		public StatusCode CancleRecordAudio() 
		{
			YIMEngine.ErrorCode code = IMAPI.Instance().CancleAudioMessage();
			return (YouMe.StatusCode)code;
		}

		/// <summary>
        /// 下载语音消息   
		/// </summary>
		/// <param name="requestID">消息ID</param>
		/// <param name="targetFilePath">语音消息的保存路径</param>
		/// <param name="downloadCallback">语音消息下载回调</param> 

		public void DownloadFile (ulong requestID, string targetFilePath, Action<YouMe.StatusCode,IMMessage,string> downloadCallback)
		{
			YIMEngine.ErrorCode code = IMAPI.Instance ().DownloadAudioFile (requestID, targetFilePath);
			if (code == YIMEngine.ErrorCode.Success) {
				bool ret = IMInternalManager.Instance.AddDownloadCallback (requestID, downloadCallback);
				if (!ret && downloadCallback != null) {
					downloadCallback (YouMe.StatusCode.Is_Waiting_Download,null,"");
				}
			}else{
				if (downloadCallback != null) {
					downloadCallback (YouMe.StatusCode.Start_Download_Fail, null,"");
				}
			}            
        }
               
		/// <summary>
		/// 设置播放语音音量  不对外
		/// </summary>
		/// <param name="volume">音量值，范围：0.0-1.0</param>
        private void SetVolume (float volume)
		{
		   IMAPI.Instance().SetVolume(volume);
		}

		/// <summary>
        /// 播放语音消息  不对外
		/// </summary>
		/// <param name="audioPath">语音消息路径</param>
		/// <param name="playCallback">语音播放回调</param>
		/// <param name="volume">可选参数，语音播放的音量值，范围：0.0-1.0 默认是1.0f</param>       
		public void StartPlayAudio (string audioPath, Action<YouMe.StatusCode, string> playCallback, float volume = 1.0f)
		{
			PlayListener = playCallback;
			bool ret = IMAPI.Instance ().IsPlaying ();
			if (ret) {
				YouMe.StatusCode stop_code = StopPlayAudio ();
				if (stop_code != YouMe.StatusCode.Success && playCallback != null) {
					playCallback (YouMe.StatusCode.StopPlay_Fail_Before_Start, "");
					return;
				}
			}
			if (volume != 1.0f) {
				IMAPI.Instance ().SetVolume (volume);
			}
		    YIMEngine.ErrorCode code = IMAPI.Instance ().StartPlayAudio (audioPath);
			if (code != YIMEngine.ErrorCode.Success && playCallback != null) {
				playCallback(YouMe.StatusCode.Start_Play_Fail, "");
			}
        }

		/// <summary>
		/// 停止语音播放  不对外
		/// </summary>
		/// <returns> YouMe.StatusCode </returns>
	    public StatusCode StopPlayAudio ()
		{
			bool ret = IMAPI.Instance ().IsPlaying ();
			if (ret) {
				YIMEngine.ErrorCode code = IMAPI.Instance().StopPlayAudio();
				return (YouMe.StatusCode)code;
			}	    
			return YouMe.StatusCode.Success;
		}

		/// <summary>
        /// 下载文件
		/// </summary>
		/// <param name="downloadUrl">下载链接</param>
		/// <param name="savePath">文件的保存路径</param>
		/// <param name="downloadCallback">文件下载回调</param>       
		public void DownloadFileByUrl (string downloadUrl, string savePath, Action<YouMe.StatusCode, string> downloadCallback)
		{		    
			YIMEngine.ErrorCode code = IMAPI.Instance ().DownloadFileByUrl (downloadUrl, savePath);			 
			if (code == YIMEngine.ErrorCode.Success) {
				bool ret = IMInternalManager.Instance.AddUrlDownloadCallback (downloadUrl, downloadCallback);
				if (!ret && downloadCallback != null) {
					downloadCallback (YouMe.StatusCode.Is_Waiting_Download, "");
				}
			}else{
				if (downloadCallback != null) {
					downloadCallback (YouMe.StatusCode.Start_Download_Fail, "");
				}
			}
		}

		/// <summary>
		/// 获取语音缓存目录
		/// </summary>
		/// <returns> 语音缓存目录路径 </returns>
        public string GetAudioCachePath ()
		{
		   return IMAPI.Instance().GetAudioCachePath();
		}

		/// <summary>
		/// 清理语音缓存目录
		/// </summary>
		/// <returns> true表示清理成功，false表示清理失败 </returns>
        public bool ClearAudioCachePath ()
		{
		   return IMAPI.Instance().ClearAudioCachePath();
		}

		/// <summary>
		/// 设置下载目录
		/// </summary>
		/// <returns> YouMe.StatusCode </returns>
		public StatusCode SetDownloadDir (string path)
		{
			YIMEngine.ErrorCode code = IMAPI.Instance ().SetDownloadDir (path);
			if (code == YIMEngine.ErrorCode.Success) {
				_downloadDirPath = path;
			}
		    return (YouMe.StatusCode)code;
		}

		/// <summary>
		/// 开始语音（不通过游密发送该语音消息，由调用方发送，调用StopAudioSpeech完成语音及上传后会回调OnStopAudioSpeechStatus）
		/// </summary>
		/// <param name="translate">是否识别语音文字</param>
		/// <returns> YouMe.StatusCode </returns>
		public StatusCode StartAudioSpeech (bool translate)
		{
			ulong requestID = 0;

			YIMEngine.ErrorCode code = IMAPI.Instance ().StartAudioSpeech (ref requestID, translate);
			if (code == YIMEngine.ErrorCode.Success){
				var msg = new SpeechInfo(requestID);
			    msg.HasUpload = false;
			    lastRecordSpeechMessage = msg;				
			}
			return (YouMe.StatusCode)code;
		}

		/// <summary>
		//停止语音（不通过游密发送该语音消息，由调用方发送，完成语音及上传后会回调OnStopAudioSpeechStatus）
		/// </summary>
		/// <param name="callback">语音上传回调</param>
		public void StopAudioSpeech (Action<YouMe.StatusCode, SpeechInfo> callback)
		{
		    YouMe.StatusCode errorcode = YouMe.StatusCode.PTT_NotStartRecord;
			if (lastRecordSpeechMessage==null && callback!=null) {
				Log.e ("Has no start record!");
				callback(errorcode, null);
				return;
			}
			var speechMsg = lastRecordSpeechMessage;
			YIMEngine.ErrorCode code = IMAPI.Instance ().StopAudioSpeech ();
			lastRecordSpeechMessage = null;
			if (code == YIMEngine.ErrorCode.Success) {
				bool ret = IMInternalManager.Instance.AddUploadCallback (speechMsg.RequestID, callback);
				if (!ret && callback != null) {
					callback (YouMe.StatusCode.Is_Waiting_Upload, speechMsg);
				}
			} else {
				if (callback != null) {
					callback (YouMe.StatusCode.PTT_UploadFail, speechMsg);
				}
			}							
		}

		/// <summary>
		//获取麦克风状态
		/// </summary>
		/// <param name="callback">获取麦克风状态的回调通知</param>
		public void GetMicrophoneStatus (Action<YouMe.StatusCode,IMAudioDeviceStatus> callback)
		{
		    GetMicrophoneStatusListener = callback;
			YIMEngine.ErrorCode code = IMAPI.Instance ().GetMicrophoneStatus ();
			if (code != YIMEngine.ErrorCode.Success && callback != null) {
				callback (YouMe.StatusCode.Get_Microphone_Status_Fail, IMAudioDeviceStatus.UNKNOWN);
			}
		}

		/// <summary>
		// 文本翻译
		/// </summary>
		/// <param name="callback">文本翻译的回调通知</param>
        public void TranslateText (string text, LanguageCode destLangCode, LanguageCode srcLangCode, Action<StatusCode,string, LanguageCode, LanguageCode> callback)
		{	
			SetTranslateListener(OnTranslateCompelete);	    
			translateCallback = callback;		
			IMAPI.Instance().TranslateText(text, destLangCode, srcLangCode, TranslateListener);		    
		}

		/// <summary>
		// 设置语音识别的语言
		/// </summary>
		/// <param name="language">语言（普通话 粤语 四川话 河南话 英语）</param>
		/// <returns>YouMe.StatusCode</returns>
		public StatusCode SetSpeechRecognizeLanguage (SpeechLanguage language)
		{
			YIMEngine.ErrorCode code = IMAPI.Instance().SetSpeechRecognizeLanguage(language);
		    return (YouMe.StatusCode)code;
		}
		/*************************************************************************
		 *                             用户相关接口                             *		 
		 *************************************************************************/

		/// <summary>
		/// 获取最近联系人列表
		/// </summary>
		/// <param name="callback">获取联系人列表的回调通知</param>	
		public void GetHistoryContact (Action<YouMe.StatusCode, List<ContactsSessionInfo>> callback)
		{
		    GetContactListener = callback;
			YIMEngine.ErrorCode code = IMAPI.Instance ().GetHistoryContact ();
			if (code != YIMEngine.ErrorCode.Success && callback!=null) {				
				callback (YouMe.StatusCode.Get_Contacts_Fail, null);
			}
		}

		/// <summary>
		/// 获取用户信息
		/// </summary>
		/// <param name="userID">用户ID</param>
		/// <param name="callback">获取用户信息的回调通知</param>	
		public void GetUserInfo (string userID, Action<YouMe.StatusCode, IMUserInfo> callback)
		{
			GetUserInfoListener = callback;
			YIMEngine.ErrorCode code = IMAPI.Instance ().GetUserInfo (userID);
			if (code != YIMEngine.ErrorCode.Success && callback != null) {
				callback (YouMe.StatusCode.Get_User_Info_Fail, null);
			}
		}

		/// <summary>
		/// 设置用户信息
		/// </summary>
		/// <param name="userInfo">用户信息</param>
		public StatusCode SetUserInfo (IMUserInfo userInfo)
		{
		    YIMEngine.ErrorCode code = IMAPI.Instance().SetUserInfo(userInfo);
		    return (YouMe.StatusCode)code;
		}

		/// <summary>
		/// 查询用户在线状态
		/// </summary>
		/// <param name="userID">用户ID</param>
		/// <param name="callback">查询用户状态的回调通知</param>
        public void QueryUserStatus (string userID, Action<YouMe.StatusCode, YIMUserStatus> callback)
		{
			QueryUserStatusListener = callback;
			YIMEngine.ErrorCode code = IMAPI.Instance ().QueryUserStatus (userID);
			if (code != YIMEngine.ErrorCode.Success && callback != null) {
				callback (YouMe.StatusCode.Query_User_Status_Fail, YIMUserStatus.UNKNOWN);
			}
		}

		/// <summary>
		/// 屏蔽用户
		/// </summary>
		/// <param name="userID">用户ID</param>
		/// <param name="block">是否屏蔽用户，true表示屏蔽，false表示解除屏蔽</param>
		/// <param name="callback">屏蔽用户的回调通知</param>
		public void BlockUser (string userID, bool block, Action<YouMe.StatusCode, bool> callback)
		{
		    BlockUserListener = callback;
			YIMEngine.ErrorCode code = IMAPI.Instance ().BlockUser (userID, block);
			if (code!=YIMEngine.ErrorCode.Success && callback!=null) {
				callback (YouMe.StatusCode.Block_User_Fail, false);
			}
		}

		/// <summary>
		/// 解除所有用户的屏蔽
		/// </summary>
		/// <param name="userID">用户ID</param>
		/// <param name="callback">解除所有屏蔽用户的回调通知</param>
		public void UnBlockAllUser (Action<YouMe.StatusCode> callback)
		{
			UnblockAllUserListener = callback;
			YIMEngine.ErrorCode code = IMAPI.Instance ().UnBlockAllUser ();
			if (code != YIMEngine.ErrorCode.Success && callback != null) {
				callback (YouMe.StatusCode.Unblock_All_User_Fail);
			}
		}

		/// <summary>
		/// 获取屏蔽的用户
		/// </summary>
		/// <param name="callback">获取屏蔽用户的回调通知</param>
		public void GetBlockUsers (Action<YouMe.StatusCode, List<string> > callback)
		{
		    GetBlockUsersListener = callback;
			YIMEngine.ErrorCode code = IMAPI.Instance ().GetBlockUsers ();
			if (code != YIMEngine.ErrorCode.Success && callback != null) {
				callback (YouMe.StatusCode.Get_Block_Users_Fail, null);
			}
		}

		/// <summary>
		/// 获取禁言状态
		/// </summary>
		/// <param name="callback">获取禁言状态的回调通知</param>
		public void GetForbiddenSpeakInfo (Action<YouMe.StatusCode, List<ForbiddenSpeakInfo> > callback)
		{
		    GetForbiddenSpeakInfoListener = callback;
			YIMEngine.ErrorCode code = IMAPI.Instance ().GetForbiddenSpeakInfo ();
			if (code != YIMEngine.ErrorCode.Success && callback != null) {
				callback (YouMe.StatusCode.Get_Forbidden_SpeakInfo_Fail, null);
			}
		}

		/*************************************************************************
		 *                             消息记录相关接口                             *		 
		 *************************************************************************/

		/// <summary>
		/// 设置消息为已读
		/// </summary>
		/// <param name="messageID">消息ID</param>
		/// <param name="read">是否已读，true为已读，false为未读</param>
		/// <returns>YouMe.StatusCode</returns>
		public StatusCode SetMessageRead (ulong messageID, bool read)
		{
			YIMEngine.ErrorCode code = IMAPI.Instance ().SetMessageRead(messageID,read);
			return (YouMe.StatusCode)code;
		}

		/// <summary>
		/// 切换接收消息模式
		/// </summary>
		/// <param name="targets">频道ID数组，可以指定多个</param>
		/// <param name="bAutoRecv">是否自动接收，默认是自动接收消息，true为自动接收消息，false为手动接收消息</param>
		/// <returns>YouMe.StatusCode</returns>
		public StatusCode SetAutoRecvMsg (List<string> targets, bool bAutoRecv)
		{
		    int err = IMAPI.Instance().SetAutoRecvMsg(targets, bAutoRecv);
		    return (YouMe.StatusCode)err;
		}

		/// <summary>
		/// 手动接收消息, 调用后消息通过ReceiveMessageListener回调通知，需手动接收消息的频道必须先切换接收消息模式，设置为手动接收
		/// </summary>
		/// <param name="targets">频道ID数组，可以指定多个</param>
		/// <returns>YouMe.StatusCode</returns>
		public StatusCode GetNewMessage (List<string> targets)
		{
			YIMEngine.ErrorCode code = IMAPI.Instance ().GetNewMessage (targets);
			return (YouMe.StatusCode)code;
		}

		/// <summary>
		/// 过滤关键字
		/// </summary>
		/// <param name="strSource">消息原文</param>
		/// <param name="level">匹配的策略词等级</param>
		/// <returns>过滤关键字后的消息，敏感字会替换为"*"</returns>
		public static string GetFilterText (string strSource, int level = 0)
		{
		    int fileterLevel = 0;
		    if (level != 0) {
		       fileterLevel = level;
		    }
		    return IMAPI.GetFilterText(strSource, ref fileterLevel);
		}

		/// <summary>
		/// 查询房间历史消息(房间最近N条聊天记录)，返回的房间历史消息记录通过ReciveMessageListener监听
		/// </summary>
		/// <param name="channelID">频道ID</param>
		/// <param name="count">消息数量（最多200条）</param>
		/// <param name="direction">历史消息排序方向 0：按时间戳升序	1：按时间戳逆序</param>
		/// <param name="callback">查询房间历史消息的回调通知</param>
		public void QueryRoomHistoryMessageFromServer (string channelID, int count, int direction,Action<YouMe.StatusCode, string, uint, List<IMMessage>> callback)
		{	
			QueryRoomHistoryMsgFromServerListener = callback;		       
			YIMEngine.ErrorCode code = IMAPI.Instance().QueryRoomHistoryMessageFromServer(channelID,count,direction);
			if (code != YIMEngine.ErrorCode.Success && callback != null) {
				callback (YouMe.StatusCode.Query_Records_Fail, channelID, 0, null);
			}		    
		}

		/// <summary>
		/// 设置是否在本地保存频道聊天记录，默认不保存，私聊历史记录默认保存
		/// </summary>
		/// <param name="channelIDs">频道ID数组</param>
		/// <param name="save">是否保存，true表示保存聊天记录，false表示不保存，默认不保存</param>
		public void SetRoomHistoryMessageSwitch (List<string> channelIDs, bool save)
		{
		    IMAPI.Instance().SetRoomHistoryMessageSwitch(channelIDs, save);
		}

		/// <summary>
        /// 查询本地历史记录, 如需查询本地的频道的历史记录，需先保存该频道的历史记录
		/// </summary>
		/// <param name="targetID">用户ID或者频道ID</param>
		/// <param name="chatType">私聊或者频道聊天类型</param>
		/// <param name="startMessageID">起始消息ID</param>
		/// <param name="count">消息记录数量</param>
		/// <param name="direction">向前或向后查询</param>
		/// <param name="callback">查询历史消息的回调通知</param>
		public void QueryHistoryMessage (string targetID, ChatType chatType, ulong startMessageID, int count, int direction, Action<YouMe.StatusCode,IMHistoryMessageInfo > callback)
		{
		    QueryHistoryMesListener = callback;
			YIMEngine.ErrorCode code = IMAPI.Instance ().QueryHistoryMessage (targetID, (YIMEngine.ChatType)chatType, startMessageID, count, direction);
			if (code != YIMEngine.ErrorCode.Success && callback != null) {
				var hisMsg = new IMHistoryMessageInfo(targetID, 0, null);
				callback(YouMe.StatusCode.Query_Records_Fail, hisMsg);
			}
		}

		/// <summary>
        /// 删除本地历史记录
		/// </summary>
		/// <param name="chatType">私聊或者频道聊天类型</param>
		/// <param name="time">时间点</param>
		/// <returns>YouMe.StatusCode</returns>
		public StatusCode DeleteHistoryMessage (ChatType chatType, ulong time)
		{
			YIMEngine.ErrorCode code = IMAPI.Instance ().DeleteHistoryMessage ((YIMEngine.ChatType)chatType, time);
			return (YouMe.StatusCode)code;
		}

		/// <summary>
        /// 根据ID删除本地私聊历史记录
		/// </summary>
		/// <param name="messageID">消息ID</param>
		/// <returns>YouMe.StatusCode</returns>
		public StatusCode DeleteHistoryMessageByID (ulong messageID)
		{
		    YIMEngine.ErrorCode code = IMAPI.Instance().DeleteHistoryMessageByID(messageID);
		    return (YouMe.StatusCode)code;
		}

		/// <summary>
		/// 删除指定用户的本地消息记录，保留消息ID列表的记录
		/// </summary>
		/// <param name="targetID">用户ID</param>
		/// <param name="chatType">私聊或者频道聊天类型</param>
		/// <param name="excludeMesList">保留的消息ID列表</param>
		/// <returns>YouMe.StatusCode</returns>
		public StatusCode DeleteSpecifiedHistoryMessage (string targetID, ChatType chatType, ulong[] excludeMesList)
		{
		    YIMEngine.ErrorCode code = IMAPI.Instance().DeleteSpecifiedHistoryMessage(targetID, (YIMEngine.ChatType)chatType, excludeMesList);
		    return (YouMe.StatusCode)code;
		}

		/// <summary>
		/// 根据用户或房间ID清理本地私聊历史记录
		/// </summary>
		/// <param name="targetID">用户ID或者房间ID</param>
		/// <param name="chatType">私聊或者频道聊天类型</param>
		/// <param name="startMessageID">起始消息ID</param>
		/// <param name="count">消息记录数量(默认值0表示删除所有消息)</param>
		/// <returns>YouMe.StatusCode</returns>
		public StatusCode DeleteHistoryMessageByTarget (string targetID, ChatType chatType, ulong startMessageID, uint count)
		{
		   YIMEngine.ErrorCode code = IMAPI.Instance().DeleteHistoryMessageByTarget(targetID, (YIMEngine.ChatType)chatType, startMessageID, count);
		   return (YouMe.StatusCode)code;
		}

		/*************************************************************************
		 *                             暂停恢复接口                             *		 
		 *************************************************************************/
		
		/// <summary>
		/// 应用暂停后恢复
		/// </summary>
		public void OnResume()
		{
			IMAPI.Instance().OnResume();
		}

		/// <summary>
		/// 程序切到后台运行
		/// </summary>
		/// <param name="pauseReceiveMessage">是否暂停接收IM消息，true-暂停接收 false-不暂停接收</param>
		public void OnPause(bool pauseReceiveMessage)
		{
			IMAPI.Instance().OnPause(pauseReceiveMessage);
		}

		/*************************************************************************
		 *                             公告相关接口                             *		 
		 *************************************************************************/

		/// <summary>
		/// 查询公告， 公告信息通过RecvNoticeListener回调通知
		/// </summary>
		/// <returns>YouMe.StatusCode</returns>
		public StatusCode QueryNotice ()
		{
			YIMEngine.ErrorCode ret = IMAPI.Instance().QueryNotice();
		    return (YouMe.StatusCode)ret;
		}

		/// <summary>
		/// 举报
		/// </summary>
		/// <param name="userID">用户ID</param>
		/// <param name="chatType">聊天类型，私聊或频道聊天</param>
		/// <param name="reason">原因</param>
		/// <param name="description">原因描述</param>
		/// <param name="extraParam">附加信息， json格式</param>
		/// <returns>YouMe.StatusCode</returns>
		public StatusCode Accusation (string userID, ChatType chatType, int reason, string description, string extraParam)
		{		    
			YIMEngine.ErrorCode code = IMAPI.Instance ().Accusation (userID, (YIMEngine.ChatType)chatType, reason, description, extraParam);
			return (YouMe.StatusCode)code;
		}

		/*************************************************************************
		 *                             地理位置相关接口                             *		 
		 *************************************************************************/

		/// <summary>
		/// 获取当前的位置, 当前位置信息通过GetCurrentLocationListener回调通知
		/// </summary>
		/// <returns>YouMe.StatusCode</returns>
		public StatusCode GetCurrentLocation ()
		{
			YIMEngine.ErrorCode code = IMAPI.Instance ().GetCurrentLocation ();
			return (YouMe.StatusCode)code;
		}

		/// <summary>
		/// 设置位置更新间隔(单位：分钟), 每个时间间隔的当前位置信息通过GetCurrentLocationListener回调通知
		/// </summary>
		public void SetUpdateInterval (uint interval)
		{
		    IMAPI.Instance().SetUpdateInterval(interval);
		}

		/// <summary>
		/// 获取附近的用户
		/// </summary>
		/// <param name="count">目标数量（一次最大200）</param>
		/// <param name="serverAreaID">区服</param>
		/// <param name="callback">获取附近用户的回调通知</param>
		/// <param name="districtlevel">行政区划等级，可选参数，默认不划分等级</param>
		/// <param name="resetStartDistance">是否重置查找起始距离，可选参数，默认是不重置</param>
		public void GetNearbyObjects (int count, string serverAreaID, Action<YouMe.StatusCode, IMNearbyObjectInfo> callback, DistrictLevel districtlevel = YIMEngine.DistrictLevel.DISTRICT_UNKNOW, bool resetStartDistance = false)
        {
            GetNearbyObjectsListener = callback;
            YIMEngine.ErrorCode code = IMAPI.Instance().GetNearbyObjects(count, serverAreaID, districtlevel, resetStartDistance);
            if (code!=YIMEngine.ErrorCode.Success && callback!=null){
				var nearbyObjectInfo = new IMNearbyObjectInfo(null, 0, 0);
				callback(YouMe.StatusCode.Get_Nearby_Objects_Fail, nearbyObjectInfo);
            }
		}

		/*************************************************************************
		 *                             设置回调监听接口                             *		 
		 *************************************************************************/

        public void SetReconnectListener (Action<IMReconnectEvent> listener)
		{
			ReconnectListener = listener;
		}

		public void SetOtherUserChannelEventListener (Action<OtherUserChannelEvent> listener)
		{
			OtherUserChannelEventListener = listener;
		}

		public void SetReceiveMessageListener(Action<IMMessage> listener){
			ReceiveMessageListener = listener;
		}

		public void SetUpdateReadStatusListener(Action<string, int, ulong> listener){
			UpdateReadStatusListener = listener;
		}

		public void SetRecvNoticeListener (Action<YIMEngine.Notice> listener)
		{
			RecvNoticeListener = listener;
		}

		public void SetCancelNoticeListener (Action<ulong, string> listener)
		{
			CancelNoticeListener = listener;
		}

		public void SetRecvRecordVolumeListener (Action<float> listener)
		{
		    RecordVolumeListener = listener;
		}

		public void SetGetCurrentLocationListener (Action<YouMe.StatusCode, GeographyLocation> listener)
		{
			GetCurrentLocationListener = listener;
		}

		public void SetGetRecognizeSpeechTextListener (Action<YouMe.StatusCode, ulong, string> listener)
		{
		   GetRecognizeSpeechTextListener = listener;
		}

		public void SetRecvNewMessageListener (Action<ChatType, string> listener)
		{
		    RecvNewMessageListener = listener;
		}

		public void SetDownloadListener(Action<YouMe.StatusCode, AudioMessage, string> listener)
		{
		   DownloadListener = listener;
		}

		public void SetAccusationListener (Action<YouMe.StatusCode, IMAccusationInfo> listener)
		{
		   AccusationListener = listener;
		}

		private void SetTranslateListener (Action<ErrorCode,string, LanguageCode, LanguageCode> listener)
		{
		   TranslateListener = listener;
		}

		private void OnTranslateCompelete (ErrorCode code, string text, LanguageCode srcCode, LanguageCode destCode)
		{			
		   translateCallback(Conv.ErrorCodeConvert(code),text,srcCode,destCode);
		}	

		/// <summary>
		/// 被踢下线事件监听器，在同时登录多个相同userID的情况下，先登录的帐号会被后登录的帐号踢下线。
		/// </summary>
		/// <param name="callback">
		/// 回调绑定
		/// KickOffEvent：响应结果对象
		///     KickOffEvent.UserID  用户ID
		///     KickOffEvent.Code    事件结果，断线通知的情况下始终为SUCCESS
		/// </param>
		public void SetKickOffListener(Action<KickOffEvent> listener){
			kickOffCallback = listener;
		}

		/// <summary>
		/// 断线事件监听器，一般发生在网络异常的时候。  考虑用开始重连的回调作为此事件
		/// </summary>
		/// <param name="callback">
		/// 回调绑定
		/// DisconnectEvent：响应结果对象
		///     DisconnectEvent.UserID  用户ID
		///     DisconnectEvent.Code    事件结果，断线通知的情况下始终为SUCCESS
		/// </param>
		public void SetDisconnectListener(Action<DisconnectEvent> listener) {
			disconnectCallback = listener;
		}

        private void OnConnect(IMConnectEvent connectEvent)
        {			
            switch ( connectEvent.EventType ){
			    case ConnectEventType.CONNECTED: 				   
                    if( loginCallback != null ) loginCallback(new LoginEvent(connectEvent.Code,connectEvent.UserID));
                    break;
                case ConnectEventType.CONNECT_FAIL:
                     if( loginCallback != null ) loginCallback(new LoginEvent(connectEvent.Code,connectEvent.UserID));
                    break;
                case ConnectEventType.OFF_LINE:
                    if( disconnectCallback != null ) disconnectCallback(new DisconnectEvent(connectEvent.Code,connectEvent.UserID));
                    break;
                case ConnectEventType.DISCONNECTED:
                    if( logoutCallback != null ) logoutCallback(new LogoutEvent(connectEvent.Code,connectEvent.UserID));
                    break;
                case ConnectEventType.KICKED:
                    if( kickOffCallback != null ) kickOffCallback(new KickOffEvent(connectEvent.Code,connectEvent.UserID));
                    break;
                default:
                    break;
            }
        }

        void OnChannelEvent(ChannelEvent channelEvent){
            switch(channelEvent.EventType){
                case ChannelEventType.JOIN_SUCCESS:
				    if(joinChannelCallback != null) joinChannelCallback(channelEvent);
				    break;
                case ChannelEventType.JOIN_FAIL:
                    if(joinChannelCallback != null) joinChannelCallback(channelEvent);
                    break;
                case ChannelEventType.LEAVE_FAIL:
				case ChannelEventType.LEAVE_ALL_FAIL:
				case ChannelEventType.LEAVE_ALL_SUCCESS:
                case ChannelEventType.LEAVE_SUCCESS:
                    if(leaveChannelCallback != null) leaveChannelCallback(channelEvent);
                    break;
            }
	    }
    }
}
		
