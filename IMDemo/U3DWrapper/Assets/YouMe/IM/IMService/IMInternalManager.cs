using System;
using System.Collections.Generic;
using UnityEngine;
using YIMEngine;
using YouMe;

public class IMInternalManager:
    YIMEngine.LoginListen,
    YIMEngine.MessageListen,
    YIMEngine.ChatRoomListen,
    YIMEngine.DownloadListen,
    YIMEngine.ContactListen,
    YIMEngine.AudioPlayListen,
    YIMEngine.LocationListen,
    YIMEngine.NoticeListen,
    YIMEngine.ReconnectListen,
	YIMEngine.UpdateReadStatusListen {

    private static IMInternalManager _instance;
    public static IMInternalManager Instance
    {
        get
        {
            if (_instance == null)
            {
                _instance = new IMInternalManager();
            }
            return _instance;
        }
    }

    private IMUser _lastLoginUser;
    public IMUser LastLoginUser{
        get{
            return _lastLoginUser;
        }
    }

    private Dictionary<ulong, MessageCallbackObject> messageCallbackQueue = new Dictionary<ulong, MessageCallbackObject>(10);
    private Dictionary<ulong, Action<YouMe.StatusCode, IMMessage, string> > downloadCallbackQueue = new Dictionary<ulong, Action<YouMe.StatusCode, IMMessage, string>>(10);

    private Dictionary<string, Action<YouMe.StatusCode, string> > urlDownloadCallbackQueue = new Dictionary<string, Action<YouMe.StatusCode, string> >(10);
    private Dictionary<ulong, Action<YouMe.StatusCode, SpeechInfo> > uploadCallbackQueue = new Dictionary<ulong, Action<StatusCode, SpeechInfo>>(10);

    public bool AddMessageCallback(ulong reqID, MessageCallbackObject callback)
    {
        if (!messageCallbackQueue.ContainsKey(reqID))
        {
            messageCallbackQueue.Add(reqID, callback);
        }
        else
        {
            Log.e("message id is already in sending queue.");
            return false;
        }
        return true;
    }

	public bool AddDownloadCallback(ulong reqID, Action<YouMe.StatusCode, IMMessage, string> callback)
	{
        if(!downloadCallbackQueue.ContainsKey(reqID))
        {
            downloadCallbackQueue.Add(reqID, callback);
        }
        else
        {
            Log.e("file already in download queue.");
            return false;
        }
        return true;
    }

    public bool AddUrlDownloadCallback (string downloadUrl, Action<YouMe.StatusCode, string> callback)
	{
		if (!urlDownloadCallbackQueue.ContainsKey (downloadUrl)) {
			urlDownloadCallbackQueue.Add (downloadUrl, callback);
		} else
		{
			Log.e("file already in url download queue.");
			return false;
		}
		return true;
	}

	public bool AddUploadCallback (ulong reqID, Action<YouMe.StatusCode, SpeechInfo> callback)
	{
		if (!uploadCallbackQueue.ContainsKey (reqID)) {
			uploadCallbackQueue.Add (reqID, callback);
		}
		else
		{
			Log.e("file already in upload queue.");
			return false;
		}
		return true;
	}


    private IMInternalManager(){
        var youmeObj = new GameObject("__YIMGameObjectV2__");
        GameObject.DontDestroyOnLoad(youmeObj);
        youmeObj.hideFlags = HideFlags.DontSave;
        youmeObj.AddComponent<YIMBehaviour>();

        YIMEngine.IMAPI.Instance().SetLoginListen(this);
        YIMEngine.IMAPI.Instance().SetMessageListen(this);
        YIMEngine.IMAPI.Instance().SetDownloadListen(this);
        YIMEngine.IMAPI.Instance().SetChatRoomListen(this);
        YIMEngine.IMAPI.Instance().SetContactListen(this);
        YIMEngine.IMAPI.Instance().SetAudioPlayListen(this);
        YIMEngine.IMAPI.Instance().SetLocationListen(this);
		YIMEngine.IMAPI.Instance().SetNoticeListen(this);
		YIMEngine.IMAPI.Instance().SetReconnectListen(this);
		YIMEngine.IMAPI.Instance().SetUpdateReadStatusListen(this);
    }

    private class YIMBehaviour:MonoBehaviour{

        void OnApplicationQuit()
        {           
            IMAPI.Instance().Logout();           
        }

        void OnApplicationPause(bool isPause)
        {
            if (isPause)
            {
                IMAPI.Instance().OnPause(false);
            }
            else
            {
                IMAPI.Instance().OnResume();
            }
        }

    }
    
    
    #region YouMeLoginListen implementation

    public void OnLogin(YIMEngine.ErrorCode errorcode, string iYouMeID)
    {
        if(errorcode == YIMEngine.ErrorCode.Success){
            _lastLoginUser = new IMUser(iYouMeID);
        }
        if( IMClient.Instance.ConnectListener!=null ){
            IMConnectEvent e = new IMConnectEvent(Conv.ErrorCodeConvert(errorcode),errorcode ==0 ? 
            ConnectEventType.CONNECTED:ConnectEventType.CONNECT_FAIL,iYouMeID);			
            IMClient.Instance.ConnectListener(e);
        }
    }

    public void OnLogout()
    {
       if( IMClient.Instance.ConnectListener!=null ){
            IMConnectEvent e = new IMConnectEvent(YouMe.StatusCode.Success,ConnectEventType.DISCONNECTED,_lastLoginUser!=null ? _lastLoginUser.UserID:"");
            IMClient.Instance.ConnectListener(e);
        }
    }

	public void OnKickOff ()
	{
		if (IMClient.Instance.ConnectListener != null) {
		   IMConnectEvent e = new IMConnectEvent(YouMe.StatusCode.Success, ConnectEventType.KICKED,_lastLoginUser.UserID);
		   IMClient.Instance.ConnectListener(e);
		}
	}

    #endregion


    #region YouMeIMMessageListen implementation

	public void OnSendMessageStatus(ulong iRequestID, YIMEngine.ErrorCode errorcode, uint sendTime, bool isForbidRoom, int reasonType, ulong forbidEndTime)
    {
        MessageCallbackObject callbackObj=null;
		bool finded = messageCallbackQueue.TryGetValue(iRequestID, out callbackObj);
        if( finded ){
            if( callbackObj != null && callbackObj.callback != null ){
                try{
                    switch(callbackObj.msgType){
                        case MessageBodyType.TXT:
                            Action<YouMe.StatusCode, TextMessage> txt_call = (Action<YouMe.StatusCode, TextMessage>)callbackObj.callback;
                            var txt_msg = (TextMessage)callbackObj.message;
                            txt_msg.SendTime = TimeUtil.ConvertToTimestamp(System.DateTime.Now);
                            if (errorcode == YIMEngine.ErrorCode.Success) {								 
							  txt_msg.SendStatus = SendStatus.Sended;
                            } else {
							  txt_msg.SendStatus = SendStatus.Fail;
                            }
						    txt_msg.IsReceiveFromServer = false;
						    txt_call(Conv.ErrorCodeConvert(errorcode),txt_msg);
                            break;
					   case MessageBodyType.Gift:
                            Action<YouMe.StatusCode, GiftMessage> gift_call = (Action<YouMe.StatusCode, GiftMessage>)callbackObj.callback;
                            var gift_msg = (GiftMessage)callbackObj.message;
                            gift_msg.SendTime = TimeUtil.ConvertToTimestamp(System.DateTime.Now);
                            if (errorcode == YIMEngine.ErrorCode.Success) {								 
							  gift_msg.SendStatus = SendStatus.Sended;
                            } else {
							  gift_msg.SendStatus = SendStatus.Fail;
                            }
						    gift_msg.IsReceiveFromServer = false;
						    gift_call(Conv.ErrorCodeConvert(errorcode),gift_msg);
                            break;
					   case MessageBodyType.CustomMesssage:
                            Action<YouMe.StatusCode, CustomMessage> cus_call = (Action<YouMe.StatusCode, CustomMessage>)callbackObj.callback;
                            var cus_msg = (CustomMessage)callbackObj.message;
                            cus_msg.SendTime = TimeUtil.ConvertToTimestamp(System.DateTime.Now);
                            if (errorcode == YIMEngine.ErrorCode.Success) {								 
							  cus_msg.SendStatus = SendStatus.Sended;
                            } else {
							  cus_msg.SendStatus = SendStatus.Fail;
                            }
						    cus_msg.IsReceiveFromServer = false;
						    cus_call(Conv.ErrorCodeConvert(errorcode),cus_msg);
                            break;
					    case MessageBodyType.File:						    
						    Action<YouMe.StatusCode, FileMessage> file_call = (Action<YouMe.StatusCode, FileMessage>)callbackObj.callback;
						    var file_msg = (FileMessage)callbackObj.message;						   
						    file_msg.SendTime = TimeUtil.ConvertToTimestamp(System.DateTime.Now);
						    if (errorcode == YIMEngine.ErrorCode.Success) {
							   file_msg.SendStatus = SendStatus.Sended;
						    } else {
							   file_msg.SendStatus = SendStatus.Fail;
						    }
						    file_msg.IsReceiveFromServer = false;
						    file_call(Conv.ErrorCodeConvert(errorcode), file_msg);
						    break;
                        default:
                            break;
                    }
                }catch(Exception e){
                    Log.e(e.ToString());
                }
            }
            messageCallbackQueue.Remove(iRequestID);
        }
    }

	public bool ResolveMessage (YIMEngine.MessageInfoBase message, out IMMessage messageObj)
	{		
		messageObj = null;
		switch (message.MessageType) {
		case MessageBodyType.TXT:
			{
				var txtMsg = (YIMEngine.TextMessage)message;
				var msg = new TextMessage (message.SenderID, message.RecvID, (ChatType)message.ChatType, txtMsg.Content,txtMsg.AttachParam, true);
				msg.SendTime = (uint)message.CreateTime;
				msg.SendStatus = SendStatus.Sended;
				msg.Distance = message.Distance;
//				msg.IsRead = message.IsRead;
				messageObj = msg;
			}
			break;
		case MessageBodyType.Gift:
			{
				var giftMsg = (YIMEngine.GiftMessage)message;
				var msg = new GiftMessage (message.SenderID, message.RecvID, giftMsg.GiftID, giftMsg.GiftCount, giftMsg.ExtParam, true);
				msg.SendTime = (uint)message.CreateTime;
				msg.SendStatus = SendStatus.Sended;
				msg.Distance = message.Distance;
//				msg.IsRead = message.IsRead;
				messageObj = msg;
			}
			break;
		case MessageBodyType.Voice:
			{
				var voiceMsg = (YIMEngine.VoiceMessage)message;

				var msg = new AudioMessage (message.SenderID, message.RecvID, (ChatType)message.ChatType, voiceMsg.Param, true);
				msg.RecongnizeText = voiceMsg.Text;
				msg.AudioDuration = voiceMsg.Duration;
				msg.SendTime = (uint)message.CreateTime;
				msg.SendStatus = SendStatus.Sended;
				msg.Distance = message.Distance;
//				msg.IsRead = message.IsRead;
				messageObj = msg;
			}
			break;
		case MessageBodyType.File:
			{
				var fileMsg = (YIMEngine.FileMessage)message;

				var msg = new FileMessage (message.SenderID, message.RecvID, (ChatType)message.ChatType, fileMsg.ExtParam, (FileType)fileMsg.FileType, true);
				msg.SendTime = (uint)message.CreateTime;
				msg.FileName = fileMsg.FileName;
				msg.FileSize = fileMsg.FileSize;
				msg.Extension = fileMsg.FileExtension;
				msg.SendStatus = SendStatus.Sended;
				msg.Distance = message.Distance;
//				msg.IsRead = message.IsRead;
				messageObj = msg;				   
			}
			break;
		case MessageBodyType.CustomMesssage:
			{
				var cusMsg = (YIMEngine.CustomMessage)message;
				var msg = new CustomMessage (message.SenderID, message.RecvID, (ChatType)message.ChatType, cusMsg.Content, true);
				msg.SendTime = (uint)message.CreateTime;
				msg.SendStatus = SendStatus.Sended;
				msg.Distance = message.Distance;
//				msg.IsRead = message.IsRead;
				messageObj = msg;				   
			}
			break;
		default:
			Log.e ("unknown message type:" + message.MessageType.ToString ());
			break;
		}
		if (messageObj != null) {	
			messageObj.RequestID = message.RequestID;
		    return true;			
		} else {
			return false;
		}
	}

	public void OnRecvMessage (YIMEngine.MessageInfoBase message)
	{		
		if (IMClient.Instance.ReceiveMessageListener != null) {
			IMMessage messageObj = null;
			if (ResolveMessage (message, out messageObj)) {
				IMClient.Instance.ReceiveMessageListener( messageObj );
			}
        }
    }

    public void OnRecvNewMessage(YIMEngine.ChatType chatType,string targetID)
    {
        IMClient.Instance.RecvNewMessageListener(chatType, targetID);
    }

    /*录音结束 */
    public void OnStartSendAudioMessage(ulong iRequestID,  YIMEngine.ErrorCode errorcode,string strText,string strAudioPath,int iDuration){
		
		OnSendAudioMessageStatusChange(iRequestID, errorcode,strText,strAudioPath,iDuration,false);
    }

    /*发送结束 */
	public void OnSendAudioMessageStatus(ulong iRequestID, YIMEngine.ErrorCode errorcode, string strText, string strAudioPath, int iDuration, uint sendTime, bool isForbidRoom, int reasonType, ulong forbidEndTime)
    {		
        OnSendAudioMessageStatusChange(iRequestID, errorcode,strText,strAudioPath,iDuration,true);
    }

    private void OnSendAudioMessageStatusChange (ulong iRequestID, YIMEngine.ErrorCode errorcode, string strText, string strAudioPath, int iDuration, bool isFinish)
	{		
		MessageCallbackObject callbackObj = null;
		bool finded = messageCallbackQueue.TryGetValue (iRequestID, out callbackObj);
		if (finded) {
			if (callbackObj != null && callbackObj.callback != null) {
                
				Action<YouMe.StatusCode, AudioMessage> call = (Action<YouMe.StatusCode, AudioMessage>)callbackObj.callback;
				var msg = (AudioMessage)callbackObj.message;
				msg.RecongnizeText = strText;
				msg.AudioFilePath = strAudioPath;
				msg.AudioDuration = iDuration;
				if (!isFinish) {					
					msg.SendTime = TimeUtil.ConvertToTimestamp (System.DateTime.Now);
				}
				if (errorcode == YIMEngine.ErrorCode.Success) {
					msg.SendStatus = isFinish ? SendStatus.Sended : SendStatus.Sending;

					msg.MessageDownloadStatus = MessageDownloadStatus.DOWNLOADING;

                }else{
                    msg.SendStatus = SendStatus.Fail;
                }
                msg.IsReceiveFromServer = false;				
                call(Conv.ErrorCodeConvert(errorcode),msg);    
            }
			if (isFinish) {
				messageCallbackQueue.Remove(iRequestID);
			}            
        }
    }

    //获取消息历史纪录回调
    public void OnQueryHistoryMessage (YIMEngine.ErrorCode errorcode, string targetID, int remain, List<YIMEngine.HistoryMsg> messageList)
	{		
		for (int i = 0; i < messageList.Count; i++) {
		   YIMEngine.HistoryMsg msg = messageList[i];		
		}
		if (IMClient.Instance.QueryHistoryMesListener != null) {
		    var historyMesInfo = new IMHistoryMessageInfo(targetID,remain,messageList);
			IMClient.Instance.QueryHistoryMesListener(Conv.ErrorCodeConvert(errorcode), historyMesInfo);
		}        
    }

    //查询房间历史记录回调
	public void OnQueryRoomHistoryMessageFromServer (YIMEngine.ErrorCode errorcode, string roomID, int remain, List<YIMEngine.MessageInfoBase> messageList)
	{
		if (IMClient.Instance.QueryRoomHistoryMsgFromServerListener != null) {
			List<IMMessage> imMsgList = new List<IMMessage> ();
			imMsgList.Clear ();
			for (int i = 0; i < messageList.Count; i++) {
				YIMEngine.MessageInfoBase message = messageList [i];
				IMMessage messageObj = null;
				if (ResolveMessage (message, out messageObj)) {
					imMsgList.Add(messageObj);
				}

			}

		    IMClient.Instance.QueryRoomHistoryMsgFromServerListener(Conv.ErrorCodeConvert(errorcode),roomID,(uint)remain,imMsgList);
		}
	}
      
    //语音上传后回调
    public void OnStopAudioSpeechStatus(YIMEngine.ErrorCode errorcode, ulong iRequestID, string strDownloadURL, int iDuraton, int iFileSize, string strLocalPath, string strText)
    {       
		Action<YouMe.StatusCode , SpeechInfo > callbackObj=null;
		bool finded = uploadCallbackQueue.TryGetValue(iRequestID, out callbackObj);
        if (finded)
        {
            if( callbackObj!=null )
            {
                try{					
					var speechInfo = new SpeechInfo(iRequestID);
					speechInfo.HasUpload = true;
					speechInfo.Duration = iDuraton;
					speechInfo.FileSize = iFileSize;
					speechInfo.DownloadURL = strDownloadURL;
					speechInfo.LocalPath = strLocalPath;
					speechInfo.Text = strText;
                    callbackObj(Conv.ErrorCodeConvert(errorcode),speechInfo);
                }catch(Exception e){
					Log.e("OnStopAudioSpeechStatus error:"+e.ToString());
                }
            }
			uploadCallbackQueue.Remove(iRequestID);
        } 
    }

	public void OnAccusationResultNotify (AccusationDealResult result, string userID, uint accusationTime)
	{
		if (IMClient.Instance.AccusationListener != null) {
			var accusationInfo = new IMAccusationInfo((AccusationDealResult)result, userID, accusationTime);
			IMClient.Instance.AccusationListener (Conv.ErrorCodeConvert (YIMEngine.ErrorCode.Success), accusationInfo);
		}
	}

	public void OnGetForbiddenSpeakInfo (YIMEngine.ErrorCode errorcode, List<ForbiddenSpeakInfo> forbiddenSpeakList)
	{
		if (IMClient.Instance.GetForbiddenSpeakInfoListener != null) {
			IMClient.Instance.GetForbiddenSpeakInfoListener (Conv.ErrorCodeConvert(errorcode), forbiddenSpeakList);
		}
	}

	public void OnGetRecognizeSpeechText (ulong iRequestID, YIMEngine.ErrorCode errorcode, string text)
	{
		if (IMClient.Instance.GetRecognizeSpeechTextListener != null) {
			IMClient.Instance.GetRecognizeSpeechTextListener (Conv.ErrorCodeConvert(errorcode), iRequestID, text);
		}
	}

	public void OnBlockUser (YIMEngine.ErrorCode errorcode, string userID, bool block)
	{
		if (IMClient.Instance.BlockUserListener != null) {
			IMClient.Instance.BlockUserListener (Conv.ErrorCodeConvert(errorcode), block);
		}
	}

	public void OnUnBlockAllUser (YIMEngine.ErrorCode errorcode)
	{
		if (IMClient.Instance.UnblockAllUserListener != null) {
			IMClient.Instance.UnblockAllUserListener (Conv.ErrorCodeConvert (errorcode));
		}
	}

	public void OnGetBlockUsers (YIMEngine.ErrorCode errorcode, List<string> userList)
	{
		if (IMClient.Instance.GetBlockUsersListener != null) {
			IMClient.Instance.GetBlockUsersListener (Conv.ErrorCodeConvert(errorcode), userList);
		}
	}

	public void OnRecordVolumeChange (float volume)
	{
		if (IMClient.Instance.RecordVolumeListener != null) {
			IMClient.Instance.RecordVolumeListener (volume);
		}
	}

    #endregion

    #region OnJoinGroupRequest implementation

	public void OnJoinRoom(YIMEngine.ErrorCode errorcode, string strChatRoomID)
    {
        if( IMClient.Instance.ChannelEventListener!=null ){
            ChannelEventType et = errorcode == YIMEngine.ErrorCode.Success ? ChannelEventType.JOIN_SUCCESS : ChannelEventType.JOIN_FAIL;
			IMClient.Instance.ChannelEventListener(new ChannelEvent( Conv.ErrorCodeConvert(errorcode),et,strChatRoomID ));
        }
    }
	public void OnLeaveRoom(YIMEngine.ErrorCode errorcode, string strChatRoomID)
    {
        if( IMClient.Instance.ChannelEventListener!=null ){
            ChannelEventType et = errorcode == YIMEngine.ErrorCode.Success ? ChannelEventType.LEAVE_SUCCESS : ChannelEventType.LEAVE_FAIL;
			IMClient.Instance.ChannelEventListener(new ChannelEvent( Conv.ErrorCodeConvert(errorcode),et,strChatRoomID ));
        }
    }

	public void OnLeaveAllRooms (YIMEngine.ErrorCode errorcode)
	{
		if( IMClient.Instance.ChannelEventListener!=null ){
            ChannelEventType et = errorcode == YIMEngine.ErrorCode.Success ? ChannelEventType.LEAVE_ALL_SUCCESS : ChannelEventType.LEAVE_ALL_FAIL;
			IMClient.Instance.ChannelEventListener(new ChannelEvent( Conv.ErrorCodeConvert(errorcode),et,""));
        }
	}

	public void OnUserJoinChatRoom (string strRoomID, string strUserID)
	{
		if (IMClient.Instance.OtherUserChannelEventListener != null) {			
			IMClient.Instance.OtherUserChannelEventListener (new OtherUserChannelEvent (OtherUserChannelEventType.JOIN_CHANNEL, strRoomID, strUserID));
		}
	}

	public void OnUserLeaveChatRoom (string strRoomID, string strUserID)
	{
		if (IMClient.Instance.OtherUserChannelEventListener != null) {			
			IMClient.Instance.OtherUserChannelEventListener (new OtherUserChannelEvent(OtherUserChannelEventType.LEAVE_CHANNEL,strRoomID, strUserID));
		}
	}

	public void OnGetRoomMemberCount (YIMEngine.ErrorCode errorcode, string strRoomID, uint count)
	{		
		if (IMClient.Instance.GetRoomMemberCountListener != null) {
			IMClient.Instance.GetRoomMemberCountListener(Conv.ErrorCodeConvert(errorcode), strRoomID, count);
		}	   
	}

    #endregion

    #region DownloadListen implementation	

	public void OnDownload (YIMEngine.ErrorCode errorcode, YIMEngine.MessageInfoBase message, string strSavePath)
	{		
		Action<YouMe.StatusCode, IMMessage, string> callbackObj = null;
		bool finded = downloadCallbackQueue.TryGetValue (message.RequestID, out callbackObj);
		if (finded) {
			if (callbackObj != null) {
				try {					
					IMMessage messageObj = null;
					if(ResolveMessage(message, out messageObj)){
							callbackObj (Conv.ErrorCodeConvert (errorcode), messageObj, strSavePath);
					}

				} catch (Exception e) {
					Log.e ("OnDownload error:" + e.ToString ());
				}
			}
			downloadCallbackQueue.Remove (message.RequestID);
		} 

		if (IMClient.Instance.DownloadListener != null) {			
			AudioMessage messageObj = null;				               
			switch (message.MessageType) {
			case MessageBodyType.Voice:
				{
					var voiceMsg = (YIMEngine.VoiceMessage)message;
					var msg = new AudioMessage (message.SenderID, message.RecvID, (ChatType)message.ChatType, voiceMsg.Param, true);
					msg.RecongnizeText = voiceMsg.Text;
					msg.AudioDuration = voiceMsg.Duration;
					msg.SendTime = (uint)message.CreateTime;
					msg.SendStatus = SendStatus.Sended;
					msg.AudioFilePath = strSavePath;
					if (errorcode == ErrorCode.Success) {
						msg.MessageDownloadStatus = MessageDownloadStatus.DOWNLOADED;
					} else {
						msg.MessageDownloadStatus = MessageDownloadStatus.DOWNLOAD_FAIL;
					} 
					messageObj = msg;
				}
				break;			
			default:
				break;
			}
			if (messageObj != null){
				try {	                
					IMClient.Instance.DownloadListener(Conv.ErrorCodeConvert(errorcode), messageObj, strSavePath);	                
			    }catch(Exception e){
			        Log.e("OnDownload error:"+e.ToString());
			    } 
		    }
		}       
   	}


	public void OnDownloadByUrl( YIMEngine.ErrorCode errorcode, string strFromUrl, string strSavePath )
	{
		Action<YouMe.StatusCode , string > callbackObj=null;
		bool finded = urlDownloadCallbackQueue.TryGetValue(strFromUrl, out callbackObj);
        if (finded)
        {
            if( callbackObj!=null )
            {
                try{					
                    callbackObj(Conv.ErrorCodeConvert(errorcode),strSavePath);
                }catch(Exception e){
					Log.e("OnDownloadByUrl error:"+e.ToString());
                }
            }
			urlDownloadCallbackQueue.Remove(strFromUrl);
        } 
	}

    #endregion

    #region ContactListen implementation
	public void OnGetContact (List<ContactsSessionInfo> contactLists)
	{
		if (IMClient.Instance.GetContactListener != null) {			
			IMClient.Instance.GetContactListener(Conv.ErrorCodeConvert(YIMEngine.ErrorCode.Success), contactLists);
		}	          
    }

	public void OnGetUserInfo (YIMEngine.ErrorCode code, string userID, IMUserInfo userInfo)
	{
		if (IMClient.Instance.GetUserInfoListener != null) {
			IMClient.Instance.GetUserInfoListener (Conv.ErrorCodeConvert(code),userInfo);
		}        
    }

    public void OnQueryUserStatus (YIMEngine.ErrorCode code, string userID, UserStatus status)
	{
		if (IMClient.Instance.QueryUserStatusListener != null) {
			IMClient.Instance.QueryUserStatusListener (Conv.ErrorCodeConvert(code),(YIMUserStatus)status);
		}
    }

    #endregion

    #region YIMEngine.LocationListen implementation
    public void OnUpdateLocation (YIMEngine.ErrorCode errorcode, YIMEngine.GeographyLocation location)
	{
		if (IMClient.Instance.GetCurrentLocationListener != null) {
			IMClient.Instance.GetCurrentLocationListener (Conv.ErrorCodeConvert(errorcode), location);
		}
    }

    public void OnGetNearbyObjects (YIMEngine.ErrorCode errorcode, List<YIMEngine.RelativeLocation> neighbourList, uint startDistance, uint endDistance)
	{
		if (IMClient.Instance.GetNearbyObjectsListener != null) {
			var nearbyObjectInfo = new IMNearbyObjectInfo(neighbourList, startDistance, endDistance);
			IMClient.Instance.GetNearbyObjectsListener (Conv.ErrorCodeConvert(errorcode), nearbyObjectInfo);
		}
    }

	// 获取与指定用户距离回调
    public void OnGetDistance (YIMEngine.ErrorCode errorcode, string userID, uint distance)
	{

	}

    #endregion

    #region YIMEngine.AudioPlayListen implementation
    public void OnPlayCompletion (YIMEngine.ErrorCode errorcode, string path)
	{		
		if (IMClient.Instance.PlayListener != null) {
			IMClient.Instance.PlayListener (Conv.ErrorCodeConvert (errorcode), path);  
		}    
    }

	public void OnGetMicrophoneStatus (YIMEngine.AudioDeviceStatus status)
	{
		if (IMClient.Instance.GetMicrophoneStatusListener != null) {
			IMClient.Instance.GetMicrophoneStatusListener (Conv.ErrorCodeConvert (YIMEngine.ErrorCode.Success), (IMAudioDeviceStatus)status);
		}
	}

    #endregion

	#region YIMEngine.NoticeListen implementation
	public void OnRecvNotice (YIMEngine.Notice notice)
	{
		if (IMClient.Instance.RecvNoticeListener != null) {
			IMClient.Instance.RecvNoticeListener (notice);
		}
	}

	public void OnCancelNotice (ulong noticeID, string channelID)
	{
		if (IMClient.Instance.CancelNoticeListener != null) {
			IMClient.Instance.CancelNoticeListener (noticeID, channelID);
		}
	}

	#endregion

	#region YIMEngine.ReconnectListen implementation
	public void OnStartReconnect ()
	{
		if (IMClient.Instance.ReconnectListener != null) {
			IMClient.Instance.ReconnectListener (new IMReconnectEvent (ReconnectEventType.START_RECONNECT, ReconnectEventResult.RECONNECTRESULT_STARTING_RECONNECT));
		}

		if (IMClient.Instance.ConnectListener != null) {			
		    IMClient.Instance.ConnectListener(new IMConnectEvent(StatusCode.Disconnect, ConnectEventType.OFF_LINE, _lastLoginUser.UserID));
		}
	}
	// 收到重连结果
	public void OnRecvReconnectResult (ReconnectResult result)
	{
		if (IMClient.Instance.ReconnectListener != null) {
			IMClient.Instance.ReconnectListener (new IMReconnectEvent(ReconnectEventType.END_RECONNECT, (ReconnectEventResult)result) );
		}
	}

	#endregion

	#region YIMEngine.UpdateReadStatusListen implementation
	public void onRead(string recvId, int chatType, ulong msgId) {
		if (IMClient.Instance.UpdateReadStatusListener != null) {
			IMClient.Instance.UpdateReadStatusListener( recvId, chatType, msgId );
        }
	}
	#endregion
}

public class MessageCallbackObject{
    public object callback;
    public IMMessage message;
    public MessageBodyType msgType;
    public MessageCallbackObject(IMMessage msg,MessageBodyType msgType,object call){
        this.callback = call;
        this.message = msg;
        this.msgType = msgType;
    }
}