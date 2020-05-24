package com.youme.im;


public class NativeEngine
{
	public native static void setModel (String model);
	public native static void setSysVersion (String sysversion);
	public native static void setDeviceIMEI (String deviceIMEI);
	public native static void setCPUArch (String ABI);
	public native static void setPackageName (String packageName);
	public native static void setDocumentPath (String path);
	public native static void setCachePath (String path);
	public native static void onNetWorkChanged (int type);
	public native static void setBrand (String brand);
	public native static void setCPUChip(String strCPUChip);
	
	public native static void WriteLog(int level, String text);
	public native static void setUpdateReadStatusCallbackFlag(boolean flag);
	
	public native static void OnRecordFinish(int errorcode, String path, long serial, String text);
	public native static byte[] OnRecordData(byte buffer[], int size, boolean resample);
	public native static void OnPlayFinish(int errrocode, String path);
	public native static void OnPlayData(byte buffer[], int size);
	
	public native static int Init(String strAppKey, String strSecrect);
	public native static void Uninit();
	public native static int Login(String iYouMeID, String strPasswd,String strToken);
	public native static int Logout();
	public native static int SendTextMessage(String iRecvID, int chatType, String strContent, String strAttachParam, IMEngine.MessageRequestId requestID);
	public native static int SendCustomMessage(String iRecvID, int chatType, byte[] strCustomMessage,int iBufferLen, IMEngine.MessageRequestId requestID);
	public native static int SendFile(String iRecvID, int chatType, String strFilePath,String strExtParam,int iFileType, IMEngine.MessageRequestId requestID);
	public native static int SendAudioMessage(String iRecvID, int chatType,  IMEngine.MessageRequestId requestID);
	public native static int SendOnlyAudioMessage(String iRecvID, int chatType, IMEngine.MessageRequestId requestID);
	public native static int StopAudioMessage(String strParam);
	public native static int CancleAudioMessage();
	public native static int DownloadAudioFile(long iSerial,String strSavePath);
	public native static int JoinChatRoom(String iID);
	public native static int LeaveChatRoom(String iID);
	public native static int LeaveAllChatRooms();
	public native static byte[] GetMessage();
	public native static void PopMessage();
	public native static int GetSDKVer();
	public native static void SetServerZone(int zone);
	public native static void SetMode(int iMode);
	public native static String GetFilterText(String strSource, IMEngine.IntegerVal level);
//	public native static void DestroyFilterText(String strSource);
	//2.0.3 added
	public native static void OnPause(boolean pauseReceiveMessage);	
	public native static void OnResume();
	public native static int SendGift(String strAnchorId,String strChannel,int iGiftID,int iGiftCount,String extParam,IMEngine.MessageRequestId requestID);
	public native static int MultiSendTextMessage(String recvLists,String strText);
	//获取最近联系人(最大100条)
	public native static int GetHistoryContact();
	public native static int StartAudioSpeech(IMEngine.MessageRequestId requestID, boolean translate );
	public native static int StopAudioSpeech();
	public native static int ConvertAMRToWav(String amrFilePath, String wavFielPath );
	public native static int QueryHistoryMessage(String targetID, int chatType, long startMessageID , int count , int direction );
	public native static int DeleteHistoryMessage(int chatType, long time);
	public native static int DeleteHistoryMessageByID(long messageID);
	public native static int DeleteSpecifiedHistoryMessage(String targetID, int chatType, long[] excludeMesList);
	public native static int DeleteHistoryMessageByTarget(String targetID, int chatType, long startMessageID, int count);
	public native static int QueryRoomHistoryMessageFromServer(String roomID, int count, int direction);
	public native static int SetDownloadAudioMessageSwitch(boolean download);  
	public native static int SetReceiveMessageSwitch(String targets, boolean bAutoRecv);
	public native static int GetNewMessage(String targets);
	public native static int SendMessageReadStatus(String sendId, int chatType, long msgId);

	//2.0.3 add 获取最近联系人(最大100条)
	public native static int SetUserInfo(String userInfo);
	public native static int GetUserInfo(String userID);

	// 2.0.4 add
	public native static int SetRoomHistoryMessageSwitch(String roomID,boolean isSave);
	public native static int StartPlayAudio(String audioPath);
	public native static int StopPlayAudio();
	public native static boolean IsPlaying();
	
	//range 0.0 -1.0
	public native static void SetVolume(float volume);
	public native static String GetAudioCachePath();
	public native static boolean ClearAudioCachePath();
	public native static int QueryUserStatus(String userID);
	
	
	public native static void OnUpdateLocation(int errorcode, double longitude, double latitude);
	
	public native static int TranslateText(IMEngine.IntegerVal requestID, String text, int destLangCode, int srcLangCode);
	
	public native static int GetCurrentLocation();
	public native static int GetNearbyObjects(int count, String serverAreaID, int districtlevel, boolean resetStartDistance);
	public native static int GetDistance(String userID);
	public native static void SetUpdateInterval(int interval);

	public native static int DownloadFileByURL(String url,String savePath,int fileType);
	public native static int SetDownloadDir(String path);
	
	public native static void GetMicrophoneStatus();
	public native static int SetSpeechRecognizeLanguage(int language);
	
	public native static int SetOnlyRecognizeSpeechText(boolean recognition);
	public native static int Accusation(String userID, int source, int reason, String description, String extraParam);
	public native static int QueryNotice();
	
	public native static int GetForbiddenSpeakInfo();
	
	public native static int GetRoomMemberCount(String roomID);
	public native static int BlockUser(String userID, boolean block);
	public native static int UnBlockAllUser();
	public native static int GetBlockUsers();
    
    public native static void NotifyVolumeChange(int volume, int type);   
    public native static int SetMessageRead(long messageID, boolean read);
    public native static int SetAllMessageRead(String userID, boolean read);
	public native static int SetVoiceMsgPlayed(long messageID, boolean played);

    public native static int SetUserProfileInfo(String profileInfo);   
    
    public native static int SetUserProfilePhoto(String photoPath);
    public native static int GetUserProfileInfo(String userID);
    public native static int SwitchUserStatus(String userID, int userStatus);
    public native static int SetAddPermission(boolean beFound, int beAddPermission);
    
    public native static int FindUser(int findType, String target);
    public native static int RequestAddFriend(String users, String comments);
    public native static int DealBeRequestAddFriend(String userID, int dealResult);
    public native static int DeleteFriend(String users, int deleteType);
    public native static int BlackFriend(int type, String users);
    public native static int QueryFriends(int type, int startIndex, int count);
    public native static int QueryFriendRequestList(int startIndex, int count);
	public native static int SwitchMsgTransType(int transType);
	
	public native static int SetLoginAddress(String ip, int port);

}
