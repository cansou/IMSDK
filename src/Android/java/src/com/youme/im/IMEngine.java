package com.youme.im;

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
				
//				if (s_netStatusReceiver == null)
//				{
//					s_netStatusReceiver = new NetworkStatusReceiver2();
//					IntentFilter filter = new IntentFilter();
//					filter.addAction("android.net.conn.CONNECTIVITY_CHANGE");
//					context.registerReceiver(s_netStatusReceiver,filter);
//				}
				m_init = true;
			}catch(Throwable err){
				Log.d("YIM_NATIVE", "initIMEngine error: " + err.toString());
			}			
		}
	}
	
	public static void WriteLog(CommonConst.LogLevel level, String text){
		NativeEngine.WriteLog(level.ordinal(), text);
	}
	
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

	public static void IM_SetServerZone(int zone){
		if (!m_init){
			return;
		}
		NativeEngine.SetServerZone(zone);
	}

	public static int IM_GetNewMessage(String targets){
		if (!m_init){
			return ENGINE_NOT_INIT;
		}
		return NativeEngine.GetNewMessage(targets);
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
