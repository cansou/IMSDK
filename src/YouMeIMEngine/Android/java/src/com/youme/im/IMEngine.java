package com.youme.im;

import com.youme.lbs.GeographyLocationManager;
import com.youme.voice.VoiceManager;
import com.youme.voice.AudioPlayer;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.IntentFilter;
import android.util.Log;

public class IMEngine {

	public static boolean m_init = false;
	private static BroadcastReceiver s_netStatusReceiver=null;
	private static boolean m_loadedLibrary = false;
	private static String m_appKey;
	private static String m_appSecret;
	private static int m_serverZone = 0;
	private static int ENGINE_NOT_INIT = 1;
	
//	public static void loadLibrary(){
	private static boolean loadLibrary(){
		if( !m_loadedLibrary ) {
			try {
				System.loadLibrary("yim");
			}catch (UnsatisfiedLinkError e) {
				Log.d("YIM_NATIVE", "UnsatisfiedLinkError: " + e.toString());
			}
			catch (Exception e) {
				Log.d("YIM_NATIVE", "loadLibrary error: " + e.toString());
			}
			catch (Throwable t) {
				Log.d("YIM_NATIVE", "loadLibrary error: " + t.toString());
			}
			m_loadedLibrary = true;
		}
		return true;
	}
	
	private static void initIMEngine(Context context) {
		if (!m_init){
			try {
				AppPara.initPara(context);
				VoiceManager.Instance().Init(context);			
				GeographyLocationManager.Instance().Init(context);
				
				if (s_netStatusReceiver == null)
				{
					s_netStatusReceiver = new NetworkStatusReceiver2();
					IntentFilter filter = new IntentFilter();
					filter.addAction("android.net.conn.CONNECTIVITY_CHANGE");
					context.registerReceiver(s_netStatusReceiver,filter);
				}
				m_init = true;
			}catch(Throwable err){
				Log.d("YIM_NATIVE", "initIMEngine error: " + err.toString());
			}			
		}
	}
	
	public static int init(Context context)
	{
		boolean ret = loadLibrary();
		if (!ret) {
			return -1;
		}
		initIMEngine(context);
		return 0;
	}

//	public static void init(Context context)
//	{
//		IMEngine.loadLibrary();
//		AppPara.initPara(context);
//		
//		//初始化一个语音合成对象
//
//		VoiceManager.Instance().Init(context);
//
//		GeographyLocationManager.Instance().Init(context);
//		if (s_netStatusReceiver == null)
//		{
//			s_netStatusReceiver = new NetworkStatusReceiver2();
//			IntentFilter filter = new IntentFilter();
//			filter.addAction("android.net.conn.CONNECTIVITY_CHANGE");
//			context.registerReceiver(s_netStatusReceiver,filter);
//		}
//
//		m_init = true;
//	}
	
	
	public static void WriteLog(CommonConst.LogLevel level, String text){
		NativeEngine.WriteLog(level.ordinal(), text);
	}
	
	//下面是IM 的封装
	//初始化和反初始化
//	public static int IM_Init(String strAppKey, String strSecrect){
//		return NativeEngine.Init(strAppKey, strSecrect);
//	}
	
	public static int IM_Init(Context context, String appKey, String secrectKey, int serverZone){
		m_appKey = appKey;
		m_appSecret = secrectKey;
		m_serverZone = serverZone;
		boolean ret = loadLibrary();
		if (!ret) {
			return -1;
		}
		
		initIMEngine(context);
		
		NativeEngine.SetServerZone(serverZone);
		return NativeEngine.Init(appKey, secrectKey);		
	}
	
	public static int IM_LoadLibray(Context context, String path){
		Log.i("YIM_NATIVE", "load library path:" + path);
		if(!m_loadedLibrary) {
			try {
				System.load(path);
			} catch (UnsatisfiedLinkError e) {
				Log.d("YIM_NATIVE", "UnsatisfiedLinkError: " + e.toString());
				return -1;
			} catch (Exception e) {
				Log.d("YIM_NATIVE", "loadLibrary error: " + e.toString());
				return -1;
			} catch (Throwable t) {
				Log.d("YIM_NATIVE", "loadLibrary error: " + t.toString());
				return -1;
			}
			m_loadedLibrary = true;
		}
		
		initIMEngine(context);
		try{				
			NativeEngine.SetServerZone(m_serverZone);
			
			if(!(m_appKey == null || m_appKey.length() <= 0) && !(m_appSecret == null || m_appSecret.length() <= 0)){
				return NativeEngine.Init(m_appKey, m_appSecret);
			}
			
		}catch(Throwable err){
			err.printStackTrace();
		}		
		
		return -1;
	}
	
	public static boolean IsInit(){
		return m_init;
	}
	
	public static void IM_Uninit(){
		if (!m_init){
			return;
		}
		NativeEngine.Uninit();
	}

	public static void setUpdateReadStatusCallbackFlag(boolean flag) {
		if (!m_init){
			return;
		}
		NativeEngine.setUpdateReadStatusCallbackFlag(flag);
	}
    
	//登陆，登出
	public static int IM_Login(String iYouMeID, String strPasswd,String strToken){		
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.Login(iYouMeID, strPasswd,strToken);
	}
	public static int IM_Logout(){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.Logout();
	}
    
    //消息接口
    public static int IM_SendTextMessage(String iRecvID, int chatType, String strContent, String strAttachParam, MessageRequestId requestID){
    	if (!m_init){
			return ENGINE_NOT_INIT;
		}
    	return NativeEngine.SendTextMessage(iRecvID, chatType, strContent, strAttachParam, requestID);
    }

    public static int IM_SendCustomMessage(String iRecvID, int chatType, byte[] strCustomMessage,int iBufferLen, MessageRequestId requestID){
    	if (!m_init){
			return ENGINE_NOT_INIT;
		}
    	return NativeEngine.SendCustomMessage(iRecvID, chatType, strCustomMessage, iBufferLen, requestID);
    }

	public static int IM_SendFile(String iRecvID, int chatType, String strFilePath,String strExtParam,int iFileType, IMEngine.MessageRequestId requestID){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return  NativeEngine.SendFile( iRecvID,  chatType,  strFilePath, strExtParam, iFileType, requestID);
	}

	public static int IM_SendAudioMessage(String iRecvID, int chatType,  MessageRequestId requestID){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.SendAudioMessage(iRecvID, chatType, requestID);
	}
    //不会转文字，直接发送的音频
	public static int IM_SendOnlyAudioMessage(String iRecvID, int chatType, MessageRequestId requestID)
	{
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.SendOnlyAudioMessage(iRecvID, chatType, requestID);
	}
	public static int IM_StopAudioMessage(String strParam){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.StopAudioMessage(strParam);
	}
    public static int IM_CancleAudioMessage(){
    	if (!m_init){
			return ENGINE_NOT_INIT;
		}
    	return NativeEngine.CancleAudioMessage();
    }
    public static int IM_DownloadAudioFile(long iSerial,String strSavePath){
    	if (!m_init){
			return ENGINE_NOT_INIT;
		}
    	return NativeEngine.DownloadAudioFile(iSerial, strSavePath);
    }
    //聊天室接口
    public static int IM_JoinChatRoom(String iID){
    	if (!m_init){
			return ENGINE_NOT_INIT;
		}
    	return NativeEngine.JoinChatRoom(iID);
    }
    public static int IM_LeaveChatRoom(String iID){
    	if (!m_init){
			return ENGINE_NOT_INIT;
		}
    	return NativeEngine.LeaveChatRoom(iID);
    }
    public static int IM_LeaveAllChatRooms(){
    	if (!m_init){
			return ENGINE_NOT_INIT;
		}
    	return NativeEngine.LeaveAllChatRooms();
    }
    
    public static int IM_GetRoomMemberCount(String roomID){
    	if (!m_init){
			return ENGINE_NOT_INIT;
		}
    	return NativeEngine.GetRoomMemberCount(roomID);
    }
    
    
	//提供一个get 接口，用来读取数据,如果没有数据会阻塞，调用登出的话，函数会返回
    public static byte[] IM_GetMessage(){
    	if (!m_init){
			return null;
		}
    	return NativeEngine.GetMessage();
    }
    public static void IM_PopMessage(){
    	if (!m_init){
			return;
		}
    	NativeEngine.PopMessage();
    }
    
    public static int IM_GetSDKVer(){
    	if (!m_init){
			return ENGINE_NOT_INIT;
		}
    	return NativeEngine.GetSDKVer();
    }
	public static void IM_SetServerZone(int zone){
		if (!m_init){
			return;
		}
		NativeEngine.SetServerZone(zone);
	}

	//设置模式0 正式环境 1开发环境 2 测试环境 3 商务环境。 默认正式环境。所以客户不需要调用这个接口
	public static void IM_SetMode(int iMode){
		if (!m_init){
			return;
		}
		NativeEngine.SetMode(iMode);
	}

	public static String IM_GetFilterText(String strSource, IntegerVal level){
		if (!m_init){
			return "";
		}
		if(strSource == null){
			return strSource;
		}
		
		return NativeEngine.GetFilterText(strSource, level);
	}
//	public static void IM_DestroyFilterText(String strSource){
//		NativeEngine.DestroyFilterText(strSource);
//	}
		
	public static void IM_OnPause(boolean pauseReceiveMessage){	
		if (!m_init){
			return;
		}
		NativeEngine.OnPause(pauseReceiveMessage);
	}

	public static void IM_OnResume(){
		if (!m_init){
			return;
		}
		NativeEngine.OnResume();
	}

	public static int IM_SendGift(String strAnchorId,String strChannel,int iGiftID,int iGiftCount,String extParam,IMEngine.MessageRequestId requestID){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.SendGift(strAnchorId,strChannel,iGiftID,iGiftCount,extParam,requestID);
	}
	public static int IM_MultiSendTextMessage(String recvLists,String strText){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		if(recvLists==null || "".equals(recvLists)){
			return 3;
		}
		return NativeEngine.MultiSendTextMessage(recvLists,strText);
	}
	public static int IM_GetHistoryContact(){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.GetHistoryContact();
	}
	public static int IM_StartAudioSpeech(IMEngine.MessageRequestId requestID, boolean translate ){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.StartAudioSpeech(requestID,translate);
	}
	public static int IM_StopAudioSpeech(){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.StopAudioSpeech();
	}
	public static int IM_ConvertAMRToWav(String amrFilePath, String wavFielPath ){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.ConvertAMRToWav (amrFilePath,wavFielPath);
	}
	public static int IM_QueryHistoryMessage(String targetID, int chatType, long startMessageID , int count , int direction ){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.QueryHistoryMessage(targetID,chatType,startMessageID,count,direction);
	}
	public static int IM_DeleteHistoryMessage(int chatType, long time ){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.DeleteHistoryMessage(chatType,time);
	}
	public static int IM_DeleteHistoryMessageByID(long messageID){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.DeleteHistoryMessageByID(messageID);
	}
    public static int IM_DeleteSpecifiedHistoryMessage(String targetID, int chatType, long[] excludeMesList) {
    	if (!m_init){
			return ENGINE_NOT_INIT;
		}
    	return NativeEngine.DeleteSpecifiedHistoryMessage(targetID, chatType, excludeMesList);
    }
    public static int IM_DeleteHistoryMessageByTarget(String targetID, int chatType, long startMessageID, int count) {
    	if (!m_init){
			return ENGINE_NOT_INIT;
		}
    	return NativeEngine.DeleteHistoryMessageByTarget(targetID, chatType, startMessageID, count);
    }    
    
    public static int IM_QueryRoomHistoryMessageFromServer(String roomID, int count, int direction){
    	if (!m_init){
			return ENGINE_NOT_INIT;
		}
    	if(roomID==null || "".equals(roomID)){
			return 3;
		}
		return NativeEngine.QueryRoomHistoryMessageFromServer(roomID, count, direction);
	}
	
	public static int IM_SetDownloadAudioMessageSwitch(boolean download){		
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.SetDownloadAudioMessageSwitch(download);
	}
		
	public static int IM_SetReceiveMessageSwitch(String targets, boolean autoRecv){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		if(targets==null || "".equals(targets)){
			return 3;
		}
		return NativeEngine.SetReceiveMessageSwitch(targets, autoRecv);
	}

	public static int IM_GetNewMessage(String targets){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.GetNewMessage(targets);
	}

	public static void IM_SendMessageReadStatus(String sendId, int chatType, long msgId) {
		if (!m_init){
			return;
		}
		NativeEngine.SendMessageReadStatus(sendId, chatType, msgId);
	}

	//2.0.3 add
	public  static int IM_SetUserInfo(String userInfo){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		if(userInfo==null || "".equals(userInfo)){
			return 3;
		}
		return NativeEngine.SetUserInfo(userInfo);
	}
	public  static int IM_GetUserInfo(String userID){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		if(userID==null || "".equals(userID)){
			return 3;
		}
		return NativeEngine.GetUserInfo(userID);
	}
	// 2.0.4 add
	public  static int IM_SetRoomHistoryMessageSwitch(String roomID,boolean isSave){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		if(roomID==null || "".equals(roomID)){
			return 3;
		}
		return NativeEngine.SetRoomHistoryMessageSwitch(roomID,isSave);
	}
	public static int IM_StartPlayAudio(String audioPath){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		if(audioPath==null || "".equals(audioPath)){
			return 3;
		}
		return NativeEngine.StartPlayAudio(audioPath);
	}
	public static int IM_StopPlayAudio(){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.StopPlayAudio();
	}
	public static boolean IM_IsPlaying(){
		if (!m_init){
			return false;
		}
		return NativeEngine.IsPlaying();
	}
	
	//range 0.0 -1.0
	public static void IM_SetVolume(float volume){
		if (!m_init){
			return;
		}
		NativeEngine.SetVolume(volume);
	}
	public static String IM_GetAudioCachePath(){
		return NativeEngine.GetAudioCachePath();
	}
	public static boolean IM_ClearAudioCachePath(){
		return NativeEngine.ClearAudioCachePath();
	}
	public static int IM_QueryUserStatus(String userID){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		if(userID==null || "".equals(userID)){
			return 3;
		}
		return NativeEngine.QueryUserStatus(userID);
	}
	
	public static int IM_TranslateText(IMEngine.IntegerVal requestID, String text, int destLangCode, int srcLangCode){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.TranslateText(requestID, text, destLangCode, srcLangCode);
	}
	
	public static int IM_GetCurrentLocation(){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.GetCurrentLocation();
	}
	
	public static int IM_GetNearbyObjects(int count, String serverAreaID, int districtlevel, boolean resetStartDistance){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.GetNearbyObjects(count, serverAreaID, districtlevel, resetStartDistance);
	}
	
	public static int IM_GetDistance(String userID){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.GetDistance(userID);
	}
	
	public static void IM_SetUpdateInterval(int interval){
		if (!m_init){
			return;
		}
		NativeEngine.SetUpdateInterval(interval);
	}

	public static int IM_DownloadFileByURL(String fromUrl, String savePath, int fileType){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.DownloadFileByURL(fromUrl,savePath, fileType);
	}
	
	public static void IM_GetMicrophoneStatus() {
		if (!m_init){
			return;
		}
		NativeEngine.GetMicrophoneStatus();
	}
	
	public static int IM_SetSpeechRecognizeLanguage(int language) {
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.SetSpeechRecognizeLanguage(language);
	}
	
	public static int IM_SetOnlyRecognizeSpeechText(boolean recognition) {
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.SetOnlyRecognizeSpeechText(recognition);
	}

	public static int IM_Accusation(String userID, int source, int reason, String description, String extraParam) {
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.Accusation(userID, source, reason, description, extraParam);
	}
	
	public static int IM_QueryNotice() {
		if (!m_init){
			return ENGINE_NOT_INIT;
		} 
		return NativeEngine.QueryNotice();
	}
	
	public static int IM_GetForbiddenSpeakInfo(){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.GetForbiddenSpeakInfo();
	}
	
	public static int IM_BlockUser(String userID, boolean block) {
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.BlockUser(userID, block);
	}
	public static int IM_UnBlockAllUser(){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.UnBlockAllUser();
	}
	
	public static int IM_GetBlockUsers() {
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.GetBlockUsers();
	}
	
	public static int IM_SetMessageRead(long messageID, boolean read){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.SetMessageRead(messageID, read);
	}
	
	public static int IM_SetAllMessageRead(String userID, boolean read){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.SetAllMessageRead(userID, read);
	}
	public static int IM_SetVoiceMsgPlayed(long messageID, boolean played){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.SetVoiceMsgPlayed(messageID, played);
	}
	
	public static int IM_SetDownloadDir(String path){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.SetDownloadDir(path);
	}
	
	public static int IM_SetUserProfileInfo(String profileInfo)
	{		
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.SetUserProfileInfo(profileInfo);
	}
	public static int IM_SetUserProfilePhoto(String photoPath){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.SetUserProfilePhoto(photoPath);
	}
	public static int IM_GetUserProfileInfo(String userID){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.GetUserProfileInfo(userID);
	}
	public static int IM_SwitchUserStatus(String userID, int userStatus){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.SwitchUserStatus(userID, userStatus);
	}
	public static int IM_SetAddPermission(boolean beFound, int beAddPermission){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.SetAddPermission(beFound, beAddPermission);
	}
	
	public static int IM_FindUser(int findType, String target){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.FindUser(findType, target);
	}
	public static int IM_RequestAddFriend(String users, String comments){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.RequestAddFriend(users, comments);
	}
	public static int IM_DealBeRequestAddFriend(String userID, int dealResult){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.DealBeRequestAddFriend(userID, dealResult);
	}
	public static int IM_DeleteFriend(String users, int deleteType){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.DeleteFriend(users, deleteType);
	}
	public static int IM_BlackFriend(int type, String users){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.BlackFriend(type, users);
	}
	public static int IM_QueryFriends(int type, int startIndex, int count){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.QueryFriends(type, startIndex, count);
	}
	public static int IM_QueryFriendRequestList(int startIndex, int count){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.QueryFriendRequestList(startIndex, count);
	}

	public static int IM_SwitchMsgTransType(int transType){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.SwitchMsgTransType(transType);
	}
	
	
	public static int IM_SetLoginAddress(String ip, int port){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.SetLoginAddress(ip, port);
	}
	
	public class MessageRequestId{
		private long requestId;
		public void setId(long id){
			requestId = id;
		}
		public long getId(){
			return requestId;
		}
	}
	
	public class IntegerVal{
		private int iValue = 0;
		public void setValue(int value){
			iValue = value;
		}
		public int getValue(){
			return iValue;
		}
	}
}
