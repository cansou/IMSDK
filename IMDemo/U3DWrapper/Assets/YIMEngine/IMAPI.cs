using UnityEngine;
using System.Runtime.InteropServices;
using System.Collections.Generic;

#if UNITY_IOS && !UNITY_EDITOR
using AOT;
#endif
#if UNITY_IOS || UNITY_ANDROID || UNITY_STANDALONE_WIN || UNITY_EDITOR_WIN || UNITY_EDITOR_OSX || UNITY_STANDALONE_OSX

namespace YIMEngine{
	
	public class IMAPI
	{
		//所有的C接口的导出		
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern int IM_Init([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr 
        #else
        UnmanagedType.LPTStr
        #endif
        )] string strAppKey,[MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr 
        #else
        UnmanagedType.LPTStr
        #endif
        )] string strAppSecrect);

		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern int IM_Login([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr 
        #else
        UnmanagedType.LPTStr
        #endif
        )] string strYouMeID,[MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr 
        #else
        UnmanagedType.LPTStr
        #endif
		)] string strPasswd,
		[MarshalAs(
		#if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
		UnmanagedType.LPWStr 
		#else
		UnmanagedType.LPTStr
		#endif
		)] string strToken);

		//logout
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern int IM_Logout();


	
		//send text message
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern int IM_SendTextMessage([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr 
        #else
        UnmanagedType.LPTStr
        #endif
        )] string  strRecvID,int iChatType,[MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr 
        #else
        UnmanagedType.LPTStr
        #endif
		)] string strContent,[MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr 
        #else
        UnmanagedType.LPTStr
        #endif
		)] string strAttachParam, ref ulong iRequestID);

		//send message read status
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern int IM_SendMessageReadStatus([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr 
        #else
        UnmanagedType.LPTStr
        #endif
        )] string sendId,int iChatType, ulong iRequestID);

		//send custom message
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		
		private static extern int IM_SendCustomMessage([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr 
        #else
        UnmanagedType.LPTStr
        #endif
		)] string  strRecvID,int iChatType,[MarshalAs(UnmanagedType.LPArray)] byte[] buffer ,int bufferLen, ref ulong iRequestID);

		//send audio message
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern int IM_SendAudioMessage([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr 
        #else
        UnmanagedType.LPTStr
        #endif
        )] string  strRecvID,int iChatType,ref ulong iRequestID);

		//send audio message
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern int IM_SendOnlyAudioMessage([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr 
        #else
        UnmanagedType.LPTStr
        #endif
        )] string  strRecvID,int iChatType,ref ulong iRequestID);



		//send audio message
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern int IM_SendFile([MarshalAs(
		#if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
		UnmanagedType.LPWStr 
		#else
			UnmanagedType.LPTStr
		#endif
		)] string  strRecvID,int iChatType,[MarshalAs(
		#if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
		UnmanagedType.LPWStr 
		#else
			UnmanagedType.LPTStr
		#endif
		)] string  strFilePath,[MarshalAs(
		#if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
		UnmanagedType.LPWStr 
		#else
			UnmanagedType.LPTStr
		#endif
		)] string  strExtParam,int iFileType, ref ulong iRequestID);


		//stop audio message
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern int IM_StopAudioMessage([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr 
        #else
        UnmanagedType.LPTStr
        #endif
        )] string strParam);

		//cancle audio message
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern int IM_CancleAudioMessage();


		//download audio message
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern int IM_DownloadFile(ulong iSerial,[MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr 
        #else
        UnmanagedType.LPTStr
        #endif
        )] string strSavePath);

		//download audio message
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern int IM_DownloadFileByURL(
			[MarshalAs(
		#if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
			UnmanagedType.LPWStr 
		#else
			UnmanagedType.LPTStr
		#endif
		)] string strFromUrl,
		[MarshalAs(
		#if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
		UnmanagedType.LPWStr 
		#else
			UnmanagedType.LPTStr
		#endif
		)] string strSavePath,  FileType fileType);


		//join chatroom
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern int IM_JoinChatRoom([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr 
        #else
        UnmanagedType.LPTStr
        #endif
        )] string  strChatRoomID);

		//leave chatroom
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern int IM_LeaveChatRoom([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr 
        #else
        UnmanagedType.LPTStr
        #endif
        )] string strChatRoomID);

		//leave all chatrooms
        #if UNITY_IPHONE && !UNITY_EDITOR
            [DllImport("__Internal")]
        #else
		    [DllImport("yim")]
        #endif
		private static extern int IM_LeaveAllChatRooms();


        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
		#else
            [DllImport("yim")]
        #endif
        private static extern int IM_GetRoomMemberCount([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
            UnmanagedType.LPWStr
        #else
            UnmanagedType.LPTStr
        #endif
        )] string strChatRoomID);



		//new  start audiospeech
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
				[DllImport("yim")]
		#endif
		private static extern int IM_StartAudioSpeech(ref ulong iRequestID,bool translate);

		//start stopspeech
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
				[DllImport("yim")]
		#endif
		private static extern int IM_StopAudioSpeech();


		//queryhistory 
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
				[DllImport("yim")]
		#endif
		private static extern int IM_QueryHistoryMessage([MarshalAs(
		#if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
		UnmanagedType.LPWStr 
		#else
					UnmanagedType.LPTStr
		#endif
		)] string targetID,int chatType,ulong MessageID,int count,int directioin);

		//start deletehistory
		#if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
		#else
			[DllImport("yim")]
		#endif
        private static extern int IM_DeleteHistoryMessage(ChatType chatType, ulong time);

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
        #else
            [DllImport("yim")]
        #endif
        private static extern int IM_DeleteHistoryMessageByID(ulong messageID);

        #if UNITY_IPHONE && !UNITY_EDITOR
             [DllImport("__Internal")]
        #else
		     [DllImport("yim")]
        #endif
		private static extern int IM_DeleteSpecifiedHistoryMessage([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
            UnmanagedType.LPWStr
        #else
			UnmanagedType.LPTStr
        #endif
		)] string targetID, ChatType chatType, ulong[] excludeMesList, int num);


        #if UNITY_IPHONE && !UNITY_EDITOR
             [DllImport("__Internal")]
        #else
		     [DllImport("yim")]
        #endif
		private static extern int IM_DeleteHistoryMessageByTarget([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
            UnmanagedType.LPWStr
        #else
			UnmanagedType.LPTStr
        #endif
		)] string targetID, ChatType chatType, ulong startMessageID, uint count);

		//queryroom message 
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
				[DllImport("yim")]
		#endif
		private static extern int IM_QueryRoomHistoryMessageFromServer([MarshalAs(
		#if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
		UnmanagedType.LPWStr 
		#else
					UnmanagedType.LPTStr
		#endif
        )] string roomID, int count, int direction);


		//convert amr to wav 
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
				[DllImport("yim")]
		#endif
		private static extern int IM_ConvertAMRToWav([MarshalAs(
		#if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
		UnmanagedType.LPWStr 
		#else
					UnmanagedType.LPTStr
		#endif
		)] string strSrcPath,[MarshalAs(
		#if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
		UnmanagedType.LPWStr 
		#else
			UnmanagedType.LPTStr
		#endif
		)] string strSrcDestPath);



		//发送礼物
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
				[DllImport("yim")]
		#endif
		private static extern int IM_SendGift([MarshalAs(
		#if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
		UnmanagedType.LPWStr 
		#else
					UnmanagedType.LPTStr
		#endif
				)] string strRecvID,[MarshalAs(
		#if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
		UnmanagedType.LPWStr 
		#else
					UnmanagedType.LPTStr
		#endif
		)] string strChannel,int iGiftID,int iGiftCount,
			[MarshalAs(
				UnmanagedType.LPStr
			)] string strExtParam,ref ulong iRequestID);




		//sdkver
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern int IM_GetSDKVer();


		//getmessget 
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern System.IntPtr IM_GetMessage();


		//getmessget 
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
				[DllImport("yim")]
		#endif
		private static extern System.IntPtr IM_GetMessage2();


		//popmessage
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern void IM_PopMessage(System.IntPtr pBuffer);



		//getfiltertext
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern System.IntPtr IM_GetFilterText([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr 
        #else
        UnmanagedType.LPTStr
        #endif
        )] string strSource, ref int level);

		//set cache path for audio file
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern void IM_SetAudioCacheDir([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr 
        #else
        UnmanagedType.LPTStr
        #endif
        )] string cachePath);
		
		//destroyfiltertext
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern void IM_DestroyFilterText(System.IntPtr pBuffer);

		//set mode
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern void IM_SetMode(int iMode);//设置模式0 正式环境 1开发环境 2 测试环境 3 商务环境。 默认正式环境。所以客户不需要调用这个接口

        #if UNITY_IPHONE && !UNITY_EDITOR
        [DllImport("__Internal")]
        #else
        [DllImport("yim")]
        #endif
        private static extern void IM_SetServerZone(int zone);

		//On Pause
		#if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
		#else
		    [DllImport("yim")]
		#endif
		private static extern void IM_OnPause(bool pauseReceiveMessage);

		//On Resume
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
				[DllImport("yim")]
		#endif
		private static extern void IM_OnResume();


		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
				[DllImport("yim")]
		#endif
		private static extern int IM_MultiSendTextMessage(
			[MarshalAs(
				UnmanagedType.LPStr
			)] string strReceives,

			[MarshalAs(
		#if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
		UnmanagedType.LPWStr 
		#else
			UnmanagedType.LPTStr
		#endif
		)] string strText);

		//set download message switch
        #if UNITY_IPHONE && !UNITY_EDITOR
            [DllImport("__Internal")]
        #else
		    [DllImport("yim")]
        #endif
		private static extern int IM_SetDownloadAudioMessageSwitch(bool download);

		//set receive message switch
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern int IM_SetReceiveMessageSwitch([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
            UnmanagedType.LPWStr
        #else
            UnmanagedType.LPTStr
        #endif
        )] string targets, bool bAutoRecv);


		//getcontact
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
        private static extern int IM_GetNewMessage([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
            UnmanagedType.LPWStr
        #else
            UnmanagedType.LPTStr
        #endif
        )] string targets);

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
        #else
        [DllImport("yim")]
        #endif
        private static extern int IM_TranslateText(ref uint requestID, [MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
            UnmanagedType.LPWStr
        #else
            UnmanagedType.LPTStr
        #endif
        )] string text, LanguageCode destLangCode, LanguageCode srcLangCode);


		//getcontact
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern int IM_GetRecentContacts();

		//getcontact
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern int IM_SetUserInfo([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr 
        #else
        UnmanagedType.LPTStr
        #endif
        )] string userInfo);

		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern int IM_GetUserInfo([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr 
        #else
        UnmanagedType.LPTStr
        #endif
        )] string userID);

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
		#else
		    [DllImport("yim")]
		#endif
        private static extern int IM_QueryUserStatus([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
            UnmanagedType.LPWStr
        #else
                UnmanagedType.LPTStr
        #endif
        )] string userID);

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
        #else
            [DllImport("yim")]
        #endif
        private static extern void IM_SetVolume(float volume);

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
        #else
            [DllImport("yim")]
        #endif
        private static extern int IM_StartPlayAudio([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
            UnmanagedType.LPWStr
        #else
            UnmanagedType.LPTStr
        #endif
        )] string path);

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
        #else
            [DllImport("yim")]
        #endif
        private static extern int IM_StopPlayAudio();

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
        #else
            [DllImport("yim")]
        #endif
        private static extern bool IM_IsPlaying();

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
        #else
            [DllImport("yim")]
        #endif
        private static extern int IM_SetRoomHistoryMessageSwitch([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
            UnmanagedType.LPWStr
        #else
                UnmanagedType.LPTStr
        #endif
        )] string roomID, bool save);

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
        #else
            [DllImport("yim")]
        #endif
            private static extern System.IntPtr IM_GetAudioCachePath();

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
        #else
            [DllImport("yim")]
        #endif
        private static extern void IM_DestroyAudioCachePath(System.IntPtr pBuffer);

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
        #else
            [DllImport("yim")]
        #endif
        private static extern bool IM_ClearAudioCachePath();

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
        #else
            [DllImport("yim")]
        #endif
        private static extern int IM_GetCurrentLocation();

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
        #else
            [DllImport("yim")]
        #endif
        private static extern int IM_GetNearbyObjects(int count, [MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
            UnmanagedType.LPWStr
        #else
            UnmanagedType.LPTStr
        #endif
        )] string serverAreaID, YIMEngine.DistrictLevel districtlevel, bool resetStartDistance);

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
        #else
            [DllImport("yim")]
        #endif
            private static extern int IM_GetDistance([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
            UnmanagedType.LPWStr
        #else
            UnmanagedType.LPTStr
        #endif
        )] string userID);

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
        #else
            [DllImport("yim")]
        #endif
        private static extern void IM_SetUpdateInterval(uint interval);

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
        #else
            [DllImport("yim")]
        #endif
        private static extern void IM_SetKeepRecordModel(bool keep);

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
        #else
            [DllImport("yim")]
        #endif
        private static extern int IM_SetSpeechRecognizeLanguage(SpeechLanguage accent);

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
        #else
            [DllImport("yim")]
        #endif
		private static extern int IM_SetOnlyRecognizeSpeechText(bool recognition);

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
        #else
            [DllImport("yim")]
        #endif
        private static extern int IM_GetMicrophoneStatus();

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
        #else
            [DllImport("yim")]
        #endif
	    private static extern int IM_Accusation([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
            UnmanagedType.LPWStr
        #else
            UnmanagedType.LPTStr
        #endif
        )] string userID, ChatType source, int reason, [MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
            UnmanagedType.LPWStr
        #else
            UnmanagedType.LPTStr
        #endif
        )] string description, [MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
            UnmanagedType.LPWStr
        #else
            UnmanagedType.LPTStr
        #endif
        )] string extraParam);
        
        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
        #else
            [DllImport("yim")]
        #endif
	    private static extern int IM_QueryNotice();

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
        #else
            [DllImport("yim")]
        #endif
        private static extern int IM_SetMessageRead(ulong messageID, bool read);

		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport("__Internal")]
		#else
		[DllImport("yim")]
		#endif
		private static extern int IM_GetForbiddenSpeakInfo();

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
		#else
		    [DllImport("yim")]
		#endif
        private static extern int IM_BlockUser([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
            UnmanagedType.LPWStr
        #else
            UnmanagedType.LPTStr
        #endif
        )] string userID, bool block);

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
        #else
            [DllImport("yim")]
        #endif
	    private static extern int IM_UnBlockAllUser();

        #if UNITY_IPHONE && !UNITY_EDITOR
		    [DllImport("__Internal")]
        #else
            [DllImport("yim")]
        #endif
	    private static extern int IM_GetBlockUsers(); 


        #if UNITY_IPHONE && !UNITY_EDITOR
        [DllImport("__Internal")]
        #else
		[DllImport("yim")]
        #endif
		private static extern int IM_SetDownloadDir([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
            UnmanagedType.LPWStr 
        #else
			UnmanagedType.LPTStr
        #endif
		)] string strPath);


        //-------------------------------------好友接口-------------------------------------

#if UNITY_IPHONE && !UNITY_EDITOR
        [DllImport("__Internal")]
#else
        [DllImport("yim")]
#endif
        private static extern int IM_FindUser(int findType, [MarshalAs(
#if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr
#else
		UnmanagedType.LPTStr
#endif
        )] string target);

#if UNITY_IPHONE && !UNITY_EDITOR
        [DllImport("__Internal")]
#else
        [DllImport("yim")]
#endif
        private static extern int IM_RequestAddFriend([MarshalAs(
#if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr
#else
		UnmanagedType.LPTStr
#endif
        )] string users, [MarshalAs(
#if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr
#else
		UnmanagedType.LPTStr
#endif
        )] string comments);

#if UNITY_IPHONE && !UNITY_EDITOR
        [DllImport("__Internal")]
#else
        [DllImport("yim")]
#endif
        private static extern int IM_DealAddFriend([MarshalAs(
#if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr
#else
		UnmanagedType.LPTStr
#endif
        )] string userID, int dealResult,ulong reqID);

#if UNITY_IPHONE && !UNITY_EDITOR
        [DllImport("__Internal")]
#else
        [DllImport("yim")]
#endif
        private static extern int IM_DeleteFriend([MarshalAs(
#if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr
#else
		UnmanagedType.LPTStr
#endif
        )] string users, int deleteType);

#if UNITY_IPHONE && !UNITY_EDITOR
        [DllImport("__Internal")]
#else
        [DllImport("yim")]
#endif
        private static extern int IM_BlackFriend(int type, [MarshalAs(
#if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
        UnmanagedType.LPWStr
#else
		UnmanagedType.LPTStr
#endif
        )] string users);

#if UNITY_IPHONE && !UNITY_EDITOR
        [DllImport("__Internal")]
#else
        [DllImport("yim")]
#endif
	    private static extern int IM_QueryFriends(int type, int startIndex, int count);

#if UNITY_IPHONE && !UNITY_EDITOR
        [DllImport("__Internal")]
#else
        [DllImport("yim")]
#endif
	    private static extern int IM_QueryFriendRequestList(int startIndex, int count);

//  用户信息管理
        #if UNITY_IPHONE && !UNITY_EDITOR
        [DllImport("__Internal")]
        #else
		[DllImport("yim")]
        #endif
		private static extern int IM_SetUserProfileInfo([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
            UnmanagedType.LPWStr 
        #else
			UnmanagedType.LPTStr
        #endif
		)] string profileInfo);


        #if UNITY_IPHONE && !UNITY_EDITOR
        [DllImport("__Internal")]
        #else
		[DllImport("yim")]
        #endif
		private static extern int IM_SetUserProfilePhoto([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
            UnmanagedType.LPWStr 
        #else
			UnmanagedType.LPTStr
        #endif
		)] string photoPath);


        #if UNITY_IPHONE && !UNITY_EDITOR
        [DllImport("__Internal")]
        #else
		[DllImport("yim")]
        #endif
		private static extern int IM_GetUserProfileInfo([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
            UnmanagedType.LPWStr 
        #else
			UnmanagedType.LPTStr
        #endif
		)] string userID);


        #if UNITY_IPHONE && !UNITY_EDITOR
        [DllImport("__Internal")]
        #else
		[DllImport("yim")]
        #endif
		private static extern int IM_SwitchUserStatus([MarshalAs(
        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
            UnmanagedType.LPWStr 
        #else
			UnmanagedType.LPTStr
        #endif
		)] string userID, UserStatus userStatus);


        #if UNITY_IPHONE && !UNITY_EDITOR
        [DllImport("__Internal")]
        #else
		[DllImport("yim")]
        #endif
		private static extern int IM_SetAddPermission(bool beFound, IMUserBeAddPermission beAddPermission);


//        #if UNITY_IPHONE && !UNITY_EDITOR
//        [DllImport("__Internal")]
//        #else
//		[DllImport("yim")]
//        #endif
//		private static extern int IM_ResizeImage([MarshalAs(
//        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
//            UnmanagedType.LPWStr 
//        #else
//			UnmanagedType.LPTStr
//        #endif
//		)] string srcImagePath, [MarshalAs(
//        #if (UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN
//            UnmanagedType.LPWStr 
//        #else
//			UnmanagedType.LPTStr
//        #endif
//		)] string resizedSavePath);

        //--------------------------------------------------------------------------


		/****************************************************************************************/
		private static IMAPI s_Instance = null;
		private LoginListen m_loginListen;
		private MessageListen m_messageListen;
		private ChatRoomListen m_groupListen;
		private DownloadListen m_downloadListen;
		private ContactListen m_contactListen;
        private AudioPlayListen m_audioPlayListen;
        private LocationListen m_locationListen;
        private NoticeListen m_noticeListen;
		private ReconnectListen m_reconnectListen;
        private FriendListen m_friendListen;
		private UserProfileListen m_userProfileListen;
		private UpdateReadStatusListen m_updateReadStatusListen;

        //tranlate callback quen
        private Dictionary<uint, System.Action<ErrorCode,string, LanguageCode, LanguageCode>> tranlateCallbackQuen = new Dictionary <uint, System.Action<ErrorCode,string, LanguageCode, LanguageCode>>();

        public static IMAPI Instance()
		{
			if (s_Instance == null) 
			{
				s_Instance = new IMAPI();
			}
			return s_Instance;
		}

		public static int GetSDKVer()
		{
			return IM_GetSDKVer ();
		}
		public void SetLoginListen(LoginListen listen)
		{
			m_loginListen = listen;
		}

	
		public void SetMessageListen(MessageListen listen)
		{
			m_messageListen = listen;
		}
		public void SetChatRoomListen(ChatRoomListen listen)
		{
			m_groupListen = listen;
		}
		public void SetDownloadListen(DownloadListen listen)
		{
			m_downloadListen = listen;
		}
		public void SetContactListen(ContactListen listen)
		{
			m_contactListen = listen;
		}
        public void SetAudioPlayListen(AudioPlayListen listen)
        {
            m_audioPlayListen = listen;
        }
        public void SetLocationListen(LocationListen listen)
        {
            m_locationListen = listen;
        }
        public void SetNoticeListen(NoticeListen listen)
        {
            m_noticeListen = listen;
        }
		public void SetReconnectListen(ReconnectListen listen)
		{
			m_reconnectListen = listen;
		}
        public void SetFriendListen(FriendListen listen)
        {
            m_friendListen = listen;
        }

		public void SetUserProfileListen (UserProfileListen listen)
		{
		    m_userProfileListen = listen;
		}

		public void SetUpdateReadStatusListen(UpdateReadStatusListen listen) {
			m_updateReadStatusListen = listen;
		}

		#if ((UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN) && (UNITY_4_7 || UNITY_4_6 || UNITY_4_5 || UNITY_4_8)
		[DllImport("msc")]
		public static extern int MSPLogin(string usr, string pwd, string _params);
		#endif

		//api 
		public static bool inited = false;
        public static bool throwException = false;
        public ErrorCode Init(string strAppKey,string strSecrect, ServerZone serverZone)
		{
			#if !UNITY_EDITOR && UNITY_ANDROID
				if(!inited){
					inited =true;
					AndroidJavaClass unity = new AndroidJavaClass ("com.unity3d.player.UnityPlayer");
					AndroidJavaObject currentActivity = unity.GetStatic<AndroidJavaObject> ("currentActivity");
					AndroidJavaClass YouMeManager = new AndroidJavaClass ("com.youme.im.IMEngine");
					YouMeManager.CallStatic<int> ("init", currentActivity);
				}
			#elif ((UNITY_STANDALONE_WIN && ! UNITY_EDITOR_OSX) || UNITY_EDITOR_WIN) && (UNITY_4_7 || UNITY_4_6 || UNITY_4_5 || UNITY_4_8)
			if(!inited){
				inited =true;
				string dllPath =  Application.dataPath+"\\Plugins"+"\\msc.dll";
				if(System.IO.File.Exists(dllPath)){
					MSPLogin("","","");
				}
			}
			#endif
			Debug.Log ("start init");
			IM_SetServerZone((int)serverZone);

			ErrorCode code = (ErrorCode)IM_Init (strAppKey, strSecrect);
            if(code == ErrorCode.Success){
				GameObject yimUpdateObj = new GameObject ("youme_update_once");
				GameObject.DontDestroyOnLoad (yimUpdateObj);
				yimUpdateObj.hideFlags = HideFlags.HideInHierarchy;
				yimUpdateObj.AddComponent <YIMUpdateObject>();

            }
            return code;
		}		  

		//是否自动下载语音消息(true: 自动下载语音消息， false:不自动下载语音消息(默认))，下载的路径是默认路径，下载回调中可以得到
		public ErrorCode SetDownloadAudioMessageSwitch(bool download)
		{
			return (ErrorCode)IM_SetDownloadAudioMessageSwitch (download);
		}

		public int SetAutoRecvMsg(List<string> targets, bool bAutoRecv)
		{
			JsonData recvJsonArray = new JsonData ();
			for (int i = 0; i < targets.Count; i++) {
				recvJsonArray.Add (targets [i]);	
			}
            return IM_SetReceiveMessageSwitch(recvJsonArray.ToJson(), bAutoRecv);
		}

        // 是否保存房间消息记录
        public void SetRoomHistoryMessageSwitch(List<string> roomIDs,bool save)
        {
			JsonData recvJsonArray = new JsonData ();
            for (int i = 0; i < roomIDs.Count; i++) {
                recvJsonArray.Add (roomIDs [i]);    
            }
            IM_SetRoomHistoryMessageSwitch(recvJsonArray.ToJson(), save);
        }

        public ErrorCode GetNewMessage(List<string> targets)
		{
            JsonData recvJsonArray = new JsonData ();
            for (int i = 0; i < targets.Count; i++) {
                recvJsonArray.Add (targets [i]);    
            }
            return (ErrorCode)IM_GetNewMessage (recvJsonArray.ToJson());
		}

		private class YIMUpdateObject :MonoBehaviour{

			void Start() {
				InvokeRepeating("YIMUpdate", 0.5f, 0.05f);
			}

			void YIMUpdate ()
			{				
				System.IntPtr pBuffer = IM_GetMessage2();
				if( pBuffer == System.IntPtr.Zero ){
					return;
				}
				string strMessage = Marshal.PtrToStringAuto(pBuffer);
				// Debug.Log("recv message:" + strMessage);
				if(null != strMessage)
				{
					if(throwException){
						IMAPI.Instance().ParseJsonMessageCallback(strMessage);
					}else{
						try{
							IMAPI.Instance().ParseJsonMessageCallback(strMessage);
						}catch(System.Exception e){
							Debug.LogError(e.Message);
						}
					}
				}

				IM_PopMessage(pBuffer);
			}

		}

		//statci 
        public static string GetFilterText(string strSource, ref int level)
		{
            System.IntPtr pBuffer = IM_GetFilterText(strSource, ref level);
			if( pBuffer == System.IntPtr.Zero ){
				return strSource;
			}
			string strMessage = Marshal.PtrToStringAuto(pBuffer);
			IM_DestroyFilterText (pBuffer);
			return strMessage;
		}
		
		public static void AllowThrowCallbackException(bool allow){
            throwException = allow;
        }

		public static string GetFilterText(string strSource)
		{
            int level = 0;
            System.IntPtr pBuffer = IM_GetFilterText(strSource, ref level);
			if( pBuffer == System.IntPtr.Zero ){
				return strSource;
			}
			string strMessage = Marshal.PtrToStringAuto(pBuffer);
			IM_DestroyFilterText (pBuffer);
			return strMessage;
		}

		void ParseJsonMessageCallback(string strMessage)
		{
			JsonData jsonMessage =  JsonMapper.ToObject (strMessage);
			Command command = (Command)(int)jsonMessage ["Command"];
			ErrorCode errorcode = (ErrorCode)(int)jsonMessage ["Errorcode"];
			switch (command) {
			case Command.CMD_RECEIVE_MESSAGE_NITIFY:
				{
					if (null != m_messageListen) {
                        ChatType chatType = (ChatType)(int)jsonMessage ["ChatType"];
                        string targetID = (string)jsonMessage ["TargetID"];
                        m_messageListen.OnRecvNewMessage (chatType,targetID);
					}
				}
				break;
			case Command.CMD_GET_RENCENT_CONTACTS:
				{
                    List<ContactsSessionInfo> contactLists = new List<ContactsSessionInfo>();
					JsonData contactJson = jsonMessage ["contacts"];
					for (int i = 0; i < contactJson.Count; i++) {
                        JsonData contactSessionJson = contactJson[i];
                        ContactsSessionInfo info = new ContactsSessionInfo();
                        info.ContactID = (string)contactSessionJson["ContactID"];
                        info.MessageType = (MessageBodyType)(int)contactSessionJson["MessageType"];
                        info.MessageContent = (string)contactSessionJson["MessageContent"];
                        info.CreateTime = (int)contactSessionJson["CreateTime"];
                        info.NotReadMsgNum = (uint)(int)contactSessionJson["NotReadMsgNum"];
						
                        contactLists.Add(info);
					}
					if (null != m_contactListen) {
						m_contactListen.OnGetContact (contactLists);
					}
				}
				break;
			case Command.CMD_QUERY_HISTORY_MESSAGE:
				{
					int iRemainCount =(int) jsonMessage["Remain"];
					string strTargetID = (string)jsonMessage["TargetID"];
					JsonData msgListJson = jsonMessage["messageList"];
					List<YIMEngine.HistoryMsg> hisoryLists=new List<YIMEngine.HistoryMsg>();
					for (int i = 0; i < msgListJson.Count; i++)
					{
						YIMEngine.HistoryMsg pMsg=new HistoryMsg();
						JsonData msg = msgListJson[i];

						pMsg.ChatType =(ChatType)(int) msg["ChatType"];
						pMsg.MessageType = (MessageBodyType)(int)msg["MessageType"];
						pMsg.ReceiveID = (string)msg["ReceiveID"];
						pMsg.SenderID = (string)msg["SenderID"];
						pMsg.MessageID =ulong.Parse(msg["Serial"].ToString());
						pMsg.CreateTime = (int)msg ["CreateTime"];
                        if (msg.Keys.Contains("IsRead"))
                        {
                            pMsg.IsRead = (int)msg["IsRead"] == 1;
                        }
						if (pMsg.MessageType == MessageBodyType.TXT) {
							pMsg.Text = (string)msg ["Content"];
							pMsg.Param = (string)msg["Param"];
						} else if (pMsg.MessageType == MessageBodyType.CustomMesssage) {
							//base64
							pMsg.Text = (string)msg ["Content"];
						} else if (pMsg.MessageType == MessageBodyType.Voice) {
							pMsg.Text =(string) msg["Text"];
							pMsg.LocalPath = (string)msg ["LocalPath"];
							pMsg.Duration = (int)msg["Duration"];
							pMsg.Param = (string)msg["Param"];
						} else if (pMsg.MessageType == MessageBodyType.File) { 
							pMsg.LocalPath = (string)msg ["LocalPath"];
							pMsg.Param = (string)msg["Param"];
						}

                            hisoryLists.Add(pMsg);
					}
					if (null != m_messageListen) {
						m_messageListen.OnQueryHistoryMessage(errorcode,strTargetID,iRemainCount,hisoryLists);
					}
				}
				break;
            case Command.CMD_GET_ROOM_HISTORY_MSG:
                {
                    int iRemainCount = (int)jsonMessage["Remain"];
                    string strRoomID = (string)jsonMessage["RoomID"];
                    JsonData msgListJson = jsonMessage["MessageList"];
                    List<YIMEngine.MessageInfoBase> messageList = new List<YIMEngine.MessageInfoBase>();
                    for (int i = 0; i < msgListJson.Count; i++)
                    {
                        MessageInfoBase message = GetMessage(msgListJson[i]);
                        if (message != null)
                        {
                            messageList.Add(message);
                        }
                    }
                    if (null != m_messageListen)
                    {
                        m_messageListen.OnQueryRoomHistoryMessageFromServer(errorcode, strRoomID, iRemainCount, messageList);
                    }
                }
                break;
			case Command.CMD_STOP_AUDIOSPEECH:
				{
					string strDownloadURL = (string)jsonMessage ["DownloadURL"];
					int iDuration = (int)jsonMessage["Duration"];
					int iFileSize = (int)jsonMessage["FileSize"];
					string strLocalPath = (string)jsonMessage["LocalPath"];
					string strRequestID = (string)jsonMessage["RequestID"];
					string strText = (string)jsonMessage["Text"];
					if (null != m_messageListen) {
						m_messageListen.OnStopAudioSpeechStatus (errorcode, ulong.Parse (strRequestID), strDownloadURL, iDuration, iFileSize, strLocalPath, strText);
					}
				}
				break;
			case Command.CMD_DOWNLOAD:
				{
					if(null != m_downloadListen)
					{
						string strSavePath = (string)jsonMessage["SavePath"];

						MessageBodyType bodyType = (MessageBodyType)(int)jsonMessage["MessageType"];
						if (bodyType == MessageBodyType.Voice) {
							VoiceMessage message = new VoiceMessage ();
							message.ChatType = (ChatType)(int)jsonMessage ["ChatType"];
							message.RequestID = ulong.Parse (jsonMessage ["Serial"].ToString ());
							message.MessageType = bodyType;
							message.RecvID = (string)jsonMessage ["ReceiveID"];
							message.SenderID = (string)jsonMessage ["SenderID"];
							message.Text = (string)jsonMessage ["Text"];
							message.Param = (string)jsonMessage ["Param"];
							message.Duration = (int)jsonMessage ["Duration"];
							message.CreateTime = (int)jsonMessage ["CreateTime"];
							if (jsonMessage.Keys.Contains ("Distance")) {
								message.Distance = (uint)(int)jsonMessage ["Distance"];
							}
                            if (jsonMessage.Keys.Contains("IsRead")){
                                message.IsRead = (int)jsonMessage["IsRead"] == 1;
                            }
							m_downloadListen.OnDownload (errorcode, message, strSavePath);
						} else if (bodyType == MessageBodyType.File) {
							FileMessage message = new FileMessage ();
							message.ChatType = (ChatType)(int)jsonMessage ["ChatType"];
							message.RequestID = ulong.Parse (jsonMessage ["Serial"].ToString ());
							message.MessageType = bodyType;
							message.RecvID = (string)jsonMessage ["ReceiveID"];
							message.SenderID = (string)jsonMessage ["SenderID"];
							message.FileName = (string)jsonMessage ["FileName"];
							message.FileSize = (int)jsonMessage ["FileSize"];
							message.FileType = (FileType)(int)jsonMessage ["FileType"];
							message.FileExtension = (string)jsonMessage ["FileExtension"];
							message.ExtParam = (string)jsonMessage ["ExtraParam"];
							message.CreateTime = (int)jsonMessage ["CreateTime"];
							if (jsonMessage.Keys.Contains ("Distance")) {
								message.Distance = (uint)(int)jsonMessage ["Distance"];
							}
                            if (jsonMessage.Keys.Contains("IsRead"))
                            {
                                message.IsRead = (int)jsonMessage["IsRead"] == 1;
                            }

							m_downloadListen.OnDownload (errorcode, message, strSavePath);
						}
					}
				}
				break;
			case Command.CMD_DOWNLOAD_URL:
				{
					if (null != m_downloadListen) {
						string strFromUrl = (string)jsonMessage["FromUrl"];
						string strSavePath = (string)jsonMessage["SavePath"];
						m_downloadListen.OnDownloadByUrl( errorcode, strFromUrl, strSavePath );
					}
				}
				break;
			case Command.CMD_LOGIN:
				{
					if(m_loginListen != null)
					{
						m_loginListen.OnLogin(errorcode,(string)jsonMessage ["UserID"]);
					}
				}
				break;
			case Command.CMD_LOGOUT:
				{
					if(m_loginListen != null)
					{
						m_loginListen.OnLogout();
					}
				}
				break;
			case Command.CMD_KICK_OFF:
				{
					if(m_loginListen != null)
					{
						m_loginListen.OnKickOff();
					}
				}
				break;

			case Command.CMD_SEND_MESSAGE_STATUS:
				{
					if(null != m_messageListen)
					{
						m_messageListen.OnSendMessageStatus(ulong.Parse(jsonMessage["RequestID"].ToString()),errorcode,
                            (uint)(int)jsonMessage["SendTime"],
                            ((int)jsonMessage["IsForbidRoom"] == 0 ? false : true),
                            (int)jsonMessage["reasonType"],
                            ulong.Parse(jsonMessage["forbidEndTime"].ToString()));
					}
				}
				break;
			case Command.CMD_SND_VOICE_MSG:
			{
				if(null != m_messageListen)
				{
					string strText = (string)jsonMessage["Text"];
					string strLocalPath = (string)jsonMessage["LocalPath"];
					int iDuration = (int)jsonMessage["Duration"];
					m_messageListen.OnSendAudioMessageStatus(ulong.Parse(jsonMessage["RequestID"].ToString()),errorcode,strText,strLocalPath,iDuration,
                        (uint)(int)jsonMessage["SendTime"],
					    ((int)jsonMessage["IsForbidRoom"] == 0 ? false : true) , 
					    (int)jsonMessage["reasonType"],
					    ulong.Parse(jsonMessage["forbidEndTime"].ToString()));
				}
			}
				break;
            case Command.CMD_STOP_SEND_AUDIO:
                {
                    if (null != m_messageListen)
                    {
                        string strText = (string)jsonMessage["Text"];
                        string strLocalPath = (string)jsonMessage["LocalPath"];
                        int iDuration = (int)jsonMessage["Duration"];
                        m_messageListen.OnStartSendAudioMessage(ulong.Parse(jsonMessage["RequestID"].ToString()), errorcode, strText, strLocalPath, iDuration);
                    }
                }
                break;
			case Command.CMD_GET_USR_INFO:
				{
					if(null != m_contactListen)
					{
                        string strUserID = (string)jsonMessage["UserID"];
						string strUserInfo = (string)jsonMessage["UserInfo"];
                        m_contactListen.OnGetUserInfo(errorcode, strUserID, new IMUserInfo().ParseFromJsonString(strUserInfo));
                    }
				}
                break;
            case Command.CMD_RECV_MESSAGE:
			{
				if(null != m_messageListen)
				{
                    MessageInfoBase message = GetMessage(jsonMessage);
                    if (message != null)
                    {
                        m_messageListen.OnRecvMessage(message);
                    }
				}
			}
				break;
	
			case Command.CMD_ENTER_ROOM:
			{
				if(null != m_groupListen)
				{
					string iChatRoomID = (string)jsonMessage["GroupID"];
					// GroupEvent evtType  = (GroupEvent)(int)jsonMessage["GroupEvt"];
					m_groupListen.OnJoinRoom(errorcode,iChatRoomID);
				}
			}
			break;
			case Command.CMD_LEAVE_ROOM:
				{
					if(null != m_groupListen)
					{
						string iChatRoomID = (string)jsonMessage["GroupID"];
						m_groupListen.OnLeaveRoom(errorcode,iChatRoomID);
					}
				}
			break;
			case Command.CMD_LEAVE_ALL_ROOM:
				{
					if(null != m_groupListen)
					{						
						m_groupListen.OnLeaveAllRooms(errorcode);
					}
				}
			break;
            case Command.CMD_USER_ENTER_ROOM:
                {
                    string strChannelID = (string)jsonMessage["ChannelID"];
                    string strUserID = (string)jsonMessage["UserID"];
                    m_groupListen.OnUserJoinChatRoom(strChannelID, strUserID);
                }
                break;
            case Command.CMD_USER_LEAVE_ROOM:
                {
                    string strChannelID = (string)jsonMessage["ChannelID"];
                    string strUserID = (string)jsonMessage["UserID"];
                    m_groupListen.OnUserLeaveChatRoom(strChannelID, strUserID);
                }
                break;
            case Command.CMD_QUERY_USER_STATUS:
            {
                if (null != m_contactListen)
                {
                    string strUserID = (string)jsonMessage["UserID"];
                    UserStatus status = (UserStatus)(int)jsonMessage["Status"];
                    m_contactListen.OnQueryUserStatus(errorcode, strUserID, status);
                }
            }
                break;
            case Command.CMD_AUDIO_PLAY_COMPLETE:
            {
                if(null != m_audioPlayListen)
                {
                    string path = (string)jsonMessage["Path"];
                    m_audioPlayListen.OnPlayCompletion(errorcode, path);
                }
            }
                break;
            case Command.CMD_GET_DISTRICT:
            {
                if (null != m_locationListen)
                {
                    GeographyLocation location = new GeographyLocation();
                    location.DistrictCode = (uint)(int)jsonMessage["DistrictCode"];
                    location.Country = (string)jsonMessage["Country"];
                    location.Province = (string)jsonMessage["Province"];
                    location.City = (string)jsonMessage["City"];
                    location.DistrictCounty = (string)jsonMessage["DistrictCounty"];
                    location.Street = (string)jsonMessage["Street"];
                    location.Longitude = (double)jsonMessage["Longitude"];
                    location.Latitude = (double)jsonMessage["Latitude"];

                    m_locationListen.OnUpdateLocation(errorcode, location);
                }
            }
                break;
            case Command.CMD_GET_PEOPLE_NEARBY:
            {
                uint startDistance = (uint)(int)jsonMessage["StartDistance"];
                uint endDistance = (uint)(int)jsonMessage["EndDistance"];
                JsonData neighbourListJson = jsonMessage["NeighbourList"];
                List<YIMEngine.RelativeLocation> heighbourLists = new List<YIMEngine.RelativeLocation>();
                for (int i = 0; i < neighbourListJson.Count; i++)
                {
                    YIMEngine.RelativeLocation relativeLocation = new RelativeLocation();
                    JsonData locationJson = neighbourListJson[i];

                    relativeLocation.Distance = (uint)(int)locationJson["Distance"];
                    relativeLocation.UserID = (string)locationJson["UserID"];
                    relativeLocation.Longitude = (double)locationJson["Longitude"];
                    relativeLocation.Latitude = (double)locationJson["Latitude"];
                    relativeLocation.Country = (string)locationJson["Country"];
                    relativeLocation.Province = (string)locationJson["Province"];
                    relativeLocation.City = (string)locationJson["City"];
                    relativeLocation.DistrictCounty = (string)locationJson["DistrictCounty"];
                    relativeLocation.Street = (string)locationJson["Street"];

                    heighbourLists.Add(relativeLocation);
                }
                if (null != m_locationListen)
                {
                    m_locationListen.OnGetNearbyObjects(errorcode, heighbourLists, startDistance, endDistance);
                }
            }
                break;
            case Command.CMD_GET_DISTANCE:
            {
                string userID = (string)jsonMessage["UserID"];
                uint distance = (uint)(int)jsonMessage["Distance"];

                if (null != m_locationListen)
                {
                    m_locationListen.OnGetDistance(errorcode, userID, distance);
                }
            }
                break;
            case Command.CMD_TRANSLATE_COMPLETE:
            {
                if (null != m_messageListen)
                {
                    uint requestID = (uint)(int)jsonMessage["RequestID"];
                    string text = (string)jsonMessage["Text"];
                    LanguageCode srcLangCode = (LanguageCode)((int)jsonMessage["SrcLangCode"]);
                    LanguageCode destLangCode = (LanguageCode)((int)jsonMessage["DestLangCode"]);
                    //m_messageListen.OnTranslateTextComplete(errorcode, requestID, text, destLangCode);
                    
					System.Action<ErrorCode, string, LanguageCode, LanguageCode> callback = null;
					bool finded = tranlateCallbackQuen.TryGetValue(requestID, out callback);
					if (finded)
					{
						tranlateCallbackQuen.Remove(requestID);
						if(callback!=null) callback(errorcode, text, srcLangCode, destLangCode);
					}
                }
            }
                break;
            case Command.CMD_GET_MICROPHONE_STATUS:
            {
                if (null != m_audioPlayListen)
                {
                    AudioDeviceStatus status = (AudioDeviceStatus)(int)jsonMessage["Status"];
                    m_audioPlayListen.OnGetMicrophoneStatus(status);
                }
            }
                break;
            case Command.CMD_GET_TIPOFF_MSG:
            {
                if (null != m_messageListen)
                {
                    AccusationDealResult result = (AccusationDealResult)(int)jsonMessage["Result"];
                    string userID = (string)jsonMessage["UserID"];
                    uint accusationTime = (uint)(int)jsonMessage["AccusationTime"];

                    m_messageListen.OnAccusationResultNotify(result, userID, accusationTime);
                }
            }
                break;
            case Command.CMD_RECV_NOTICE:
		    {
                if (null != m_noticeListen)
                {
                    YIMEngine.Notice notice = new Notice();
                    notice.NoticeID = ulong.Parse(jsonMessage["NoticeID"].ToString());
                    notice.ChannelID = (string)jsonMessage["ChannelID"];
                    notice.NoticeType = (int)jsonMessage["NoticeType"];
                    notice.Content = (string)jsonMessage["NoticeContent"];
                    notice.LinkText = (string)jsonMessage["LinkText"];
                    notice.LinkAddr = (string)jsonMessage["LinkAddress"];
                    notice.BeginTime = (uint)(int)jsonMessage["BeginTime"];
                    notice.EndTime = (uint)(int)jsonMessage["EndTime"];

                    m_noticeListen.OnRecvNotice(notice);
                }
		    }
			    break;
		    case Command.CMD_CANCEL_NOTICE:
		    {
                if (null != m_noticeListen)
                {
                    ulong noticeID = ulong.Parse(jsonMessage["NoticeID"].ToString());
			        string channelID = (string)jsonMessage["ChannelID"];
                    m_noticeListen.OnCancelNotice(noticeID, channelID);
                }
		    }
			    break;
			case Command.CMD_GET_FORBID_RECORD:
			{
				if( null != m_messageListen )
				{
					JsonData forbidListJson = jsonMessage["ForbiddenSpeakList"];

					List<YIMEngine.ForbiddenSpeakInfo> forbidLists = new List<YIMEngine.ForbiddenSpeakInfo>();
					for (int i = 0; i < forbidListJson.Count; i++)
					{
						YIMEngine.ForbiddenSpeakInfo  info = new ForbiddenSpeakInfo();
						JsonData forbidJson = forbidListJson[i];
						
						info.ChannelID = (string)forbidJson["ChannelID"];
						info.IsForbidRoom = (int)forbidJson["IsForbidRoom"] == 0 ? false : true ;
						info.ReasonType = (int)forbidJson["reasonType"];
						info.EndTime = ulong.Parse(forbidJson["forbidEndTime"].ToString());
					
						forbidLists.Add(info);
					}

					m_messageListen.OnGetForbiddenSpeakInfo(errorcode, forbidLists );
				}
			}
				break;
            case Command.CMD_SET_MASK_USER_MSG:
            {
                if (null != m_messageListen)
                {
                    string userID = (string)jsonMessage["UserID"];
                    bool block = (int)jsonMessage["Block"] == 1;
                    m_messageListen.OnBlockUser(errorcode, userID, block);
                }
            }
                break;
            case Command.CMD_GET_MASK_USER_MSG:
            {
                if (null != m_messageListen)
                {
                    List<string> userList = new List<string>();
                    JsonData users = jsonMessage["UserList"];
                    for (int i = 0; i < users.Count; i++)
                    {
                        string userID = (string)users[i];
                        userList.Add(userID);
                    }
                    m_messageListen.OnGetBlockUsers(errorcode, userList);
                }
            }
                break;
            case Command.CMD_CLEAN_MASK_USER_MSG:
            {
                if (null != m_messageListen)
                {
                    m_messageListen.OnUnBlockAllUser(errorcode);
                }
            }
               break;
            case Command.CMD_GET_ROOM_INFO:
            {
                if (null != m_messageListen)
                {
                    string roomID = (string)jsonMessage["RoomID"];
                    uint count = (uint)(int)jsonMessage["Count"];
                    m_groupListen.OnGetRoomMemberCount(errorcode, roomID, count);
                }
            }
                break;
			case Command.CMD_GET_SPEECH_TEXT:
				{
					if (null != m_messageListen)
					{
                        ulong requestID = ulong.Parse(jsonMessage["RequestID"].ToString());
						string text = (string)jsonMessage["Text"];
                        m_messageListen.OnGetRecognizeSpeechText(requestID, errorcode, text);
					}
				}
				break;
			case Command.CMD_GET_RECONNECT_RESULT:
				{
					if (null != m_reconnectListen)
					{
						ReconnectResult result = (ReconnectResult)(int)jsonMessage["Result"];
						m_reconnectListen.OnRecvReconnectResult(result);
					}
				}
				break;
			case Command.CMD_START_RECONNECT:
				{
					if (null != m_reconnectListen)
					{						
						m_reconnectListen.OnStartReconnect();
					}
				}
				break;
			case Command.CMD_RECORD_VOLUME:
				{
				   if (null != m_messageListen)
					{	
						double volume = (double)jsonMessage["Volume"];
						m_messageListen.OnRecordVolumeChange((float)volume);
					}
				}
				break;
            case Command.CMD_FIND_FRIEND_BY_ID:
            {
                if (m_friendListen != null)
                {
                    JsonData userArray = jsonMessage["UserList"];
                    List<YIMEngine.UserBriefInfo> userList = new List<YIMEngine.UserBriefInfo>();
                    for (int i = 0; i < userArray.Count; ++i)
                    {
                        JsonData userInfo = userArray[i];
                        YIMEngine.UserBriefInfo info = new UserBriefInfo();
                        info.UserID = (string)userInfo["UserID"];
                        info.Nickname = (string)userInfo["Nickname"];
                        info.Status = (UserStatus)((int)userInfo["Status"]);
                        userList.Add(info);
                    }

                    m_friendListen.OnFindUser(errorcode, userList);
                }
            }
                break;
			case Command.CMD_REQUEST_ADD_FRIEND:
            {
                if (m_friendListen != null)
                {
                    string userID = (string)jsonMessage["UserID"];
                    m_friendListen.OnRequestAddFriend(errorcode, userID);
                }
            }
                break;
			case Command.CMD_REQUEST_ADD_FRIEND_NOTIFY:
            {
                if (m_friendListen != null)
                {
                    string userID = (string)jsonMessage["UserID"];
					string comments = (string)jsonMessage["Comments"];
					ulong reqID = ulong.Parse(jsonMessage["ReqID"].ToString());
                    m_friendListen.OnBeRequestAddFriendNotify(userID,comments,reqID);
                }
            }
                break;
			case Command.CMD_BE_ADD_FRIENT:
            {
                if (m_friendListen != null)
                {
                    string userID = (string)jsonMessage["UserID"];
					string comments = (string)jsonMessage["Comments"];
                    m_friendListen.OnBeAddFriendNotify(userID,comments);
                }
            }
                break;			
			case Command.CMD_DEAL_ADD_FRIEND:
            {
                if (m_friendListen != null)
                {
                    string userID = (string)jsonMessage["UserID"];
                    string comments = (string)jsonMessage["Comments"];
					int dealResult = (int)jsonMessage["DealResult"];
                    m_friendListen.OnDealBeRequestAddFriend(errorcode,userID,comments,dealResult);
                }
            }
                break;
            case Command.CMD_ADD_FRIENT_RESULT_NOTIFY:
            {
                if (m_friendListen != null)
                {
                    string userID = (string)jsonMessage["UserID"];
                    string comments = (string)jsonMessage["Comments"];
					int dealResult = (int)jsonMessage["DealResult"];
                    m_friendListen.OnRequestAddFriendResultNotify(userID,comments,dealResult);
                }
            }
                break;
            case Command.CMD_DELETE_FRIEND:
            {
                if (m_friendListen != null)
                {
                    string userID = (string)jsonMessage["UserID"];
                    m_friendListen.OnDeleteFriend(errorcode, userID);
                }
            }
                break;
            case Command.CMD_BE_DELETE_FRIEND_NOTIFY:
            {
                if (m_friendListen != null)
                {
                    string userID = (string)jsonMessage["UserID"];
                    m_friendListen.OnBeDeleteFriendNotify(userID);
                }
            }
                break;
            case Command.CMD_BLACK_FRIEND:
            {
                if (m_friendListen != null)
                {
                    int type = (int)jsonMessage["Type"];
                    string userID = (string)jsonMessage["UserID"];
                    m_friendListen.OnBlackFriend(errorcode, type, userID);
                }
            }
                break;            
            case Command.CMD_QUERY_FRIEND_LIST:
            {
                if (m_friendListen != null)
                {
                    int type = (int)jsonMessage["Type"];
                    int startIndex = (int)jsonMessage["StartIndex"];
                    JsonData userArray = jsonMessage["UserList"];
                    List<YIMEngine.UserBriefInfo> userList = new List<YIMEngine.UserBriefInfo>();
                    for (int i = 0; i < userArray.Count; ++i)
                    {
                        JsonData userInfo = userArray[i];
                        YIMEngine.UserBriefInfo info = new UserBriefInfo();
                        info.UserID = (string)userInfo["UserID"];
                        info.Nickname = (string)userInfo["Nickname"];
						info.Status = (UserStatus)((int)userInfo["Status"]);
                        userList.Add(info);
                    }
                    m_friendListen.OnQueryFriends(errorcode, type, startIndex, userList);
                }
            }
                break;
            case Command.CMD_QUERY_FRIEND_REQUEST_LIST:
            {
                if (m_friendListen != null)
                {
                    int startIndex = (int)jsonMessage["StartIndex"];
                    JsonData requestArray = jsonMessage["UserList"];
                    List<YIMEngine.FriendRequestInfo> requestList = new List<YIMEngine.FriendRequestInfo>();
                    for (int i = 0; i < requestArray.Count; ++i)
                    {
                        JsonData requestInfo = requestArray[i];
                        YIMEngine.FriendRequestInfo info = new FriendRequestInfo();
                        info.AskerID = (string)requestInfo["AskerID"];
                        info.AskerNickname = (string)requestInfo["AskerNickname"];
                        info.InviteeID = (string)requestInfo["InviteeID"];
                        info.InviteeNickname = (string)requestInfo["InviteeNickname"];
                        info.ValidateInfo = (string)requestInfo["ValidateInfo"];
                        info.Status = (AddFriendStatus)((int)requestInfo["Status"]);
                        info.CreateTime = (uint)((int)requestInfo["CreateTime"]);
						info.ReqID = ulong.Parse(requestInfo["ReqID"].ToString());
                        requestList.Add(info);
                    }
                    m_friendListen.OnQueryFriendRequestList(errorcode, startIndex, requestList);
                }
            }
                break;
			case Command.CMD_HXR_USER_INFO_CHANGE_NOTIFY:
			{
				if (null != m_userProfileListen)
				{							
					string strUserID = (string)jsonMessage["UserID"];
					m_userProfileListen.OnUserInfoChangeNotify(strUserID);
				}
			}
				break;
			case Command.CMD_SET_USER_PROFILE:
			{
				if (null != m_userProfileListen)
				{							
					m_userProfileListen.OnSetUserInfo(errorcode);
				}
			}
				break;
			case Command.CMD_GET_USER_PROFILE:
			{
				if (null != m_userProfileListen)
				{	
					IMUserProfileInfo userProfileInfo = new IMUserProfileInfo();
					userProfileInfo.UserID = (string)jsonMessage["UserID"];
					userProfileInfo.PhotoURL = (string)jsonMessage["PhotoUrl"];
					userProfileInfo.OnlineState = (UserStatus)((int)jsonMessage["OnlineState"]);
					userProfileInfo.BeAddPermission = (IMUserBeAddPermission)((int)jsonMessage["BeAddPermission"]);
					userProfileInfo.FoundPermission = (IMUserFoundPermission)((int)jsonMessage["FoundPermission"]);
					userProfileInfo.NickName = (string)jsonMessage["NickName"];
					userProfileInfo.Sex = (IMUserSex)((int)jsonMessage["Sex"]);
					userProfileInfo.Signature = (string)jsonMessage["Signature"];
					userProfileInfo.Country = (string)jsonMessage["Country"];
					userProfileInfo.Province = (string)jsonMessage["Province"];
					userProfileInfo.City = (string)jsonMessage["City"];
					userProfileInfo.ExtraInfo = (string)jsonMessage["ExtraInfo"];

					m_userProfileListen.OnQueryUserInfo(errorcode, userProfileInfo);
				}
			}
				break;
			case Command.CMD_SET_USER_PHOTO:
			{
				if (null != m_userProfileListen)
				{	
					string strPhotoUrl = (string)jsonMessage["PhotoUrl"];
					m_userProfileListen.OnSetPhotoUrl(errorcode,strPhotoUrl);
				}
			}
				break;
			case Command.CMD_SWITCH_USER_STATE:
			{
				if (null != m_userProfileListen)
				{	
					m_userProfileListen.OnSwitchUserOnlineState(errorcode);
				}
			}
				break;
			case Command.CMD_SET_READ_STATUS:
			{
				string recvId = (string)jsonMessage["RecvId"];
				int chatType = int.Parse(jsonMessage["ChatType"].ToString());
				ulong msgId = ulong.Parse(jsonMessage["MsgId"].ToString());
				if (null != m_updateReadStatusListen)
				{	
					m_updateReadStatusListen.onRead(recvId, chatType, msgId);
				}
			}
				break;
		    default:
					break;
			}
		}


        public MessageInfoBase GetMessage(JsonData jsonMessage)
        {
            //MessageInfoBase message;
            MessageBodyType bodyType = (MessageBodyType)(int)jsonMessage["MessageType"];
			if (bodyType == MessageBodyType.TXT) {
				TextMessage message = new TextMessage ();
				message.ChatType = (ChatType)(int)jsonMessage ["ChatType"];
				message.RequestID = ulong.Parse (jsonMessage ["Serial"].ToString ());
				message.MessageType = bodyType;
				message.RecvID = (string)jsonMessage ["ReceiveID"];
				message.SenderID = (string)jsonMessage ["SenderID"];
				message.Content = (string)jsonMessage ["Content"];
				message.AttachParam = (string)jsonMessage ["AttachParam"];
				message.CreateTime = (int)jsonMessage ["CreateTime"];
                if (jsonMessage.Keys.Contains("Distance"))
                {
                    message.Distance = (uint)(int)jsonMessage["Distance"];
                }
                if (jsonMessage.Keys.Contains("IsRead"))
                {
                    message.IsRead = (int)jsonMessage["IsRead"] == 1;
                }
				return message;
			} else if (bodyType == MessageBodyType.CustomMesssage) {
				CustomMessage message = new CustomMessage ();
				message.ChatType = (ChatType)(int)jsonMessage ["ChatType"];
				message.RequestID = ulong.Parse (jsonMessage ["Serial"].ToString ());
				message.MessageType = bodyType;
				message.RecvID = (string)jsonMessage ["ReceiveID"];
				message.SenderID = (string)jsonMessage ["SenderID"];
				string strBase64Content = (string)jsonMessage ["Content"];
				message.Content = System.Convert.FromBase64String (strBase64Content);
				message.CreateTime = (int)jsonMessage ["CreateTime"];
                if (jsonMessage.Keys.Contains("Distance"))
                {
                    message.Distance = (uint)(int)jsonMessage["Distance"];
                }
                if (jsonMessage.Keys.Contains("IsRead"))
                {
                    message.IsRead = (int)jsonMessage["IsRead"] == 1;
                }
                return message;
			} else if (bodyType == MessageBodyType.Voice) {
				VoiceMessage message = new VoiceMessage ();
				message.ChatType = (ChatType)(int)jsonMessage ["ChatType"];
				message.RequestID = ulong.Parse (jsonMessage ["Serial"].ToString ());
				message.MessageType = bodyType;
				message.RecvID = (string)jsonMessage ["ReceiveID"];
				message.SenderID = (string)jsonMessage ["SenderID"];
				message.Text = (string)jsonMessage ["Text"];
				message.Param = (string)jsonMessage ["Param"];
				message.Duration = (int)jsonMessage ["Duration"];
				message.CreateTime = (int)jsonMessage ["CreateTime"];
                if (jsonMessage.Keys.Contains("Distance"))
                {
                    message.Distance = (uint)(int)jsonMessage["Distance"];
                }
                if (jsonMessage.Keys.Contains("IsRead"))
                {
                    message.IsRead = (int)jsonMessage["IsRead"] == 1;
                }
                return message;
			} else if (bodyType == MessageBodyType.Gift) {
				GiftMessage message = new GiftMessage ();
				message.ChatType = (ChatType)(int)jsonMessage ["ChatType"];
				message.RequestID = ulong.Parse (jsonMessage ["Serial"].ToString ());
				message.MessageType = bodyType;
				message.RecvID = (string)jsonMessage ["ReceiveID"];
				message.SenderID = (string)jsonMessage ["SenderID"];
				message.CreateTime = (int)jsonMessage ["CreateTime"];
                if (jsonMessage.Keys.Contains("Distance"))
                {
                    message.Distance = (uint)(int)jsonMessage["Distance"];
                }
                if (jsonMessage.Keys.Contains("IsRead"))
                {
                    message.IsRead = (int)jsonMessage["IsRead"] == 1;
                }
				message.ExtParam = new ExtraGifParam ().ParseFromJsonString ((string)jsonMessage ["Param"]);
				message.GiftID = (int)jsonMessage ["GiftID"];
				message.GiftCount = (int)jsonMessage ["GiftCount"];
				message.Anchor = (string)jsonMessage ["Anchor"];

                return message;
			} else if (bodyType == MessageBodyType.File) 
			{
				FileMessage message = new FileMessage ();
				message.ChatType = (ChatType)(int)jsonMessage ["ChatType"];
				message.RequestID = ulong.Parse (jsonMessage ["Serial"].ToString ());
				message.MessageType = bodyType;
				message.RecvID = (string)jsonMessage ["ReceiveID"];
				message.SenderID = (string)jsonMessage ["SenderID"];
				message.FileName = (string)jsonMessage ["FileName"];
				message.FileSize = (int)jsonMessage ["FileSize"];
				message.FileType = (FileType)(int)jsonMessage ["FileType"];
				message.FileExtension = (string)jsonMessage ["FileExtension"];
				message.ExtParam = (string)jsonMessage ["ExtraParam"];
				message.CreateTime = (int)jsonMessage ["CreateTime"];
                if (jsonMessage.Keys.Contains("Distance"))
                {
                    message.Distance = (uint)(int)jsonMessage["Distance"];
                }
                if (jsonMessage.Keys.Contains("IsRead"))
                {
                    message.IsRead = (int)jsonMessage["IsRead"] == 1;
                }

                return message;
			}
            return null;
        }


		//login
		public ErrorCode Login(string strYouMeID,string strPasswd,string strToken="")
		{
            return (ErrorCode)IM_Login(strYouMeID,strPasswd,strToken);
		}


		public ErrorCode Logout()
		{
			return  (ErrorCode)IM_Logout ();
		}


		public ErrorCode SendTextMessage(string strRecvID,ChatType chatType,string strContent, string strAttachParam, ref ulong iRequestID)
		{
			return (ErrorCode)IM_SendTextMessage(strRecvID,(int)chatType,strContent,strAttachParam,ref iRequestID);
		}

		public ErrorCode sendMessageReadStatus(string sendId, ChatType chatType, ulong msgId) {
			return (ErrorCode)IM_SendMessageReadStatus(sendId, (int)chatType, msgId);
		}

		public ErrorCode SendFile(string strRecvID,ChatType chatType,string strFilePath,string strExtParam,FileType fileType, ref ulong iRequestID)
		{
			return (ErrorCode)IM_SendFile(strRecvID,(int)chatType,strFilePath,strExtParam,(int)fileType,ref iRequestID);
		}

		public ErrorCode SendAudioMessage(string strRecvID,ChatType chatType,ref ulong iRequestID)
		{
			return (ErrorCode)IM_SendAudioMessage(strRecvID,(int)chatType,ref iRequestID);
		}

		public ErrorCode SendOnlyAudioMessage(string strRecvID,ChatType chatType,ref ulong iRequestID)
		{
			return (ErrorCode)IM_SendOnlyAudioMessage(strRecvID,(int)chatType,ref iRequestID);
		}
		//strParam can be empty
		public ErrorCode StopAudioMessage(string strParam)
		{
			return (ErrorCode)IM_StopAudioMessage (strParam);
		}

		public ErrorCode CancleAudioMessage()
		{
			return (ErrorCode)IM_CancleAudioMessage ();
		}

		public ErrorCode DownloadAudioFile(ulong iRequestID,string strSavePath)
		{
			return (ErrorCode)IM_DownloadFile (iRequestID, strSavePath);
		}

		public ErrorCode DownloadFileByUrl( string strFromUrl, string strSavePath , FileType fileType = FileType.FileType_Audio)
		{
			return (ErrorCode)IM_DownloadFileByURL (strFromUrl, strSavePath, fileType);
		
		}
		public ErrorCode SendCustomMessage(string strRecvID,ChatType chatTpye,byte[] customMsg,ref ulong iRequestID)
		{
			return (ErrorCode)IM_SendCustomMessage(strRecvID,(int)chatTpye,customMsg,customMsg.Length, ref iRequestID);
		}

		//chatroom
		public ErrorCode JoinChatRoom (string strChatRoomID)
		{
			return (ErrorCode)IM_JoinChatRoom(strChatRoomID);
		}
        
		public ErrorCode LeaveChatRoom (string strChatRoomID)
		{
			return (ErrorCode)IM_LeaveChatRoom(strChatRoomID);
		}
        
		public ErrorCode LeaveAllChatRooms ()
		{
			return (ErrorCode)IM_LeaveAllChatRooms();
		}

        public ErrorCode GetRoomMemberCount(string strChatRoomID)
        {
            return (ErrorCode)IM_GetRoomMemberCount(strChatRoomID);
        }

		private IMAPI()
		{
		}

		public void SetAudioCachePath(string cachePath){
            IM_SetAudioCacheDir(cachePath);
        }

		//新加接口
		//查询消息记录
        public ErrorCode QueryHistoryMessage(string targetID, ChatType chatType, ulong startMessageID, int count, int direction)
		{
			return (ErrorCode)IM_QueryHistoryMessage (targetID, (int)chatType, startMessageID, count, direction);
		}

        /*清理消息记录
	    YIMChatType:私聊消息、房间消息
	    time：删除指定时间之前的消息*/
	    public ErrorCode DeleteHistoryMessage(ChatType chatType, ulong time)
        {
            return (ErrorCode)IM_DeleteHistoryMessage(chatType, time);
        }
        
        //删除指定messageID对应消息
	    public ErrorCode DeleteHistoryMessageByID(ulong messageID)
        {
            return (ErrorCode)IM_DeleteHistoryMessageByID(messageID);
        }

        //删除指定用户的本地消息记录，保留消息ID列表的记录
		public ErrorCode DeleteSpecifiedHistoryMessage(string targetID, ChatType chatType, ulong[] excludeMesList)
		{			
			int num = excludeMesList.Length;
			return (ErrorCode)IM_DeleteSpecifiedHistoryMessage(targetID, chatType, excludeMesList, num);
		}

		public ErrorCode DeleteHistoryMessageByTarget(string targetID, ChatType chatType, ulong startMessageID, uint count)
		{			
			return (ErrorCode)IM_DeleteHistoryMessageByTarget(targetID, chatType, startMessageID, count);
		}
		
        public ErrorCode SetMessageRead(ulong messageID, bool read)
        {
            return (ErrorCode)IM_SetMessageRead(messageID, read);
        }

		//查询房间历史消息(房间最近N条聊天记录)
        public ErrorCode QueryRoomHistoryMessageFromServer(string roomID, int count, int direction)
		{
			return (ErrorCode)IM_QueryRoomHistoryMessageFromServer(roomID, count, direction);
		}

		//开始语音（不通过游密发送该语音消息，由调用方发送，调用StopAudioSpeech完成语音及上传后会回调OnStopAudioSpeechStatus）
		public ErrorCode StartAudioSpeech(ref ulong requestID, bool translate ) 
		{
			return (ErrorCode)IM_StartAudioSpeech (ref requestID, translate);
		}
		//停止语音（不通过游密发送该语音消息，由调用方发送，完成语音及上传后会回调OnStopAudioSpeechStatus）
		public ErrorCode StopAudioSpeech()
		{
			return (ErrorCode)IM_StopAudioSpeech ();
		}
		//转换AMR格式到WAV格式
		public ErrorCode ConvertAMRToWav(string amrFilePath, string wavFielPath )
		{
			return (ErrorCode)IM_ConvertAMRToWav (amrFilePath, wavFielPath);
		}


		//应用前后台切换调用
		public void OnResume()
		{
			IM_OnResume ();
		}

		//程序切到后台运行	,pauseReceiveMessage true-暂停接收 false-不暂停接收	
		public void OnPause(bool pauseReceiveMessage)
		{
			IM_OnPause(pauseReceiveMessage);
		}
		//发送礼物
		//extraParam:附加参数 格式为json {"nickname":"","server_area":"","location":"","score":"","level":"","vip_level":"","extra":""}
		public ErrorCode SendGift(string strAnchorId,string strChannel,int iGiftID,int iGiftCount,ExtraGifParam extParam,ref ulong serial)
		{
			return (ErrorCode)IM_SendGift (strAnchorId, strChannel, iGiftID, iGiftCount, extParam.ToJsonString() , ref serial);
		}

		//群发消息
		public ErrorCode MultiSendTextMessage(List<string> recvLists,string strText)
		{				
			JsonData recvJsonArray = new JsonData ();
			for (int i = 0; i < recvLists.Count; i++) {
				recvJsonArray.Add (recvLists [i]);	
			}
			return(ErrorCode)IM_MultiSendTextMessage (recvJsonArray.ToJson(), strText);
		}

		//获取最近联系人
		public ErrorCode GetHistoryContact()
		{
			return (ErrorCode)IM_GetRecentContacts ();
		}

		public ErrorCode GetUserInfo(string userID){
            return (ErrorCode)IM_GetUserInfo(userID);
        }

		public ErrorCode SetUserInfo(IMUserInfo userInfo){
            return (ErrorCode)IM_SetUserInfo(userInfo.ToJsonString());
        }

        //查询用户在线状态
        public ErrorCode QueryUserStatus(string userID)
        {
            return (ErrorCode)IM_QueryUserStatus(userID);
        }

        //设置播放语音音量(volume:0.0-1.0)
        public void SetVolume(float volume)
        {
            IM_SetVolume(volume);
        }

        //播放语音
        public ErrorCode StartPlayAudio(string path)
        {
            return (ErrorCode)IM_StartPlayAudio(path);
        }

	    //停止语音播放
	    public ErrorCode StopPlayAudio()
        {
            return (ErrorCode) IM_StopPlayAudio();
        }

	    //查询播放状态
	    public bool IsPlaying()
        {
            return IM_IsPlaying();
        }        

        //获取语音缓存目录
        public string GetAudioCachePath()
        {
            string strPath = "";
            System.IntPtr pBuffer = IM_GetAudioCachePath();
            if (pBuffer == System.IntPtr.Zero)
            {
                return strPath;
            }
            strPath = Marshal.PtrToStringAuto(pBuffer);
            IM_DestroyAudioCachePath(pBuffer);
            return strPath;
        }
        
        //清理语音缓存目录
        public bool ClearAudioCachePath()
        {
            return IM_ClearAudioCachePath();
        }

        // 文本翻译
        public void TranslateText( string text, LanguageCode destLangCode, LanguageCode srcLangCode,System.Action<ErrorCode,string, LanguageCode, LanguageCode> callback)
		{
            uint requestID = 0;
            var code = (ErrorCode) IM_TranslateText(ref requestID, text, destLangCode, srcLangCode);
            if(code == ErrorCode.Success)
            {
                tranlateCallbackQuen.Add(requestID, callback);
            }
            else
            {
                callback(code,"", srcLangCode, destLangCode);
            }
           
		}

        public ErrorCode GetCurrentLocation()
        {
            return (ErrorCode) IM_GetCurrentLocation();
        }
	    
        // 获取附近的目标	count:获取数量（一次最大200） serverAreaID：区服	districtlevel：行政区划等级		resetStartDistance：是否重置查找起始距离
        public ErrorCode GetNearbyObjects(int count, string serverAreaID, DistrictLevel districtlevel = YIMEngine.DistrictLevel.DISTRICT_UNKNOW, bool resetStartDistance = false)
        {
            return (ErrorCode) IM_GetNearbyObjects(count, serverAreaID, districtlevel, resetStartDistance);
        }

        // 获取与指定用户距离
        public ErrorCode GetDistance(string userID)
        {
            return (ErrorCode) IM_GetDistance(userID);
        }

        // 设置位置更新间隔(单位：分钟)
	    public void SetUpdateInterval(uint interval)
        {
            IM_SetUpdateInterval(interval);
        }

        public void SetKeepRecordModel(bool keep)
        {
            IM_SetKeepRecordModel(keep);
        }

		public ErrorCode SetSpeechRecognizeLanguage(SpeechLanguage language)
        {
			return (ErrorCode)IM_SetSpeechRecognizeLanguage(language);
        }

		public ErrorCode SetOnlyRecognizeSpeechText(bool recognition)
		{
			return (ErrorCode)IM_SetOnlyRecognizeSpeechText(recognition);
		}

		public ErrorCode GetMicrophoneStatus()
        {
			return (ErrorCode)IM_GetMicrophoneStatus();
        }

		public ErrorCode Accusation(string userID, ChatType source, int reason, string description, string extraParam)
        {
			return (ErrorCode)IM_Accusation(userID, source, reason, description, extraParam);
        }

		public ErrorCode QueryNotice()
        {
			return (ErrorCode)IM_QueryNotice();
        }

		public ErrorCode GetForbiddenSpeakInfo()
		{
			return (ErrorCode)IM_GetForbiddenSpeakInfo ();
		}

        public ErrorCode BlockUser(string userID, bool block)
        {
            return (ErrorCode)IM_BlockUser(userID, block);
        }

	    public ErrorCode UnBlockAllUser()
        {
            return (ErrorCode)IM_UnBlockAllUser();
        }
    
	    public ErrorCode GetBlockUsers()
        {
            return (ErrorCode)IM_GetBlockUsers();
        }

		public ErrorCode SetDownloadDir(string path)
		{
			return (ErrorCode)IM_SetDownloadDir(path);
		}


        //-------------------------------------好友接口-------------------------------------

        /*
	    * 功能：查找添加好友（获取用户简要信息）
	    * @param findType：查找类型	0：按ID查找	1：按昵称查找
	    * @param target：对应查找类型用户ID或昵称
	    * @return 错误码
	    */
	    public ErrorCode FindUser(int findType, string target)
        {
            return (ErrorCode) IM_FindUser(findType, target);
        }

	    /*
	    * 功能：添加好友
	    * @param userID：用户ID
	    * @param comments：备注或验证信息(长度最大128)
	    * @return 错误码
	    */
	    public ErrorCode RequestAddFriend(List<string> users, string comments)
        {
            JsonData usersArray = new JsonData();
            for (int i = 0; i < users.Count; ++i)
            {
                usersArray.Add(users[i]);
            }
            return (ErrorCode)IM_RequestAddFriend(usersArray.ToJson(), comments);
        }

	    /*
	    * 功能：处理好友添加请求
	    * @param userID：用户ID
	    * @param dealResult：处理结果	0：同意	1：拒绝
		* @param reqID: 对应 OnBeRequestAddFriendNotify 或者 OnQueryFriendRequestList 回调的reqID
	    * @return 错误码
	    */
	    public ErrorCode DealAddFriend(string userID, int dealResult, ulong reqID)
        {
            return (ErrorCode) IM_DealAddFriend(userID, dealResult, reqID);
        }

	    /*
	    * 功能：删除好友
	    * @param users：用户ID
	    * @param deleteType：删除类型	0：双向删除	1：单向删除(删除方在被删除方好友列表依然存在)
	    * @return 错误码
	    */
	    public ErrorCode DeleteFriend(List<string> users, int deleteType = 1)
        {
            JsonData usersArray = new JsonData();
            for (int i = 0; i < users.Count; ++i)
            {
                usersArray.Add(users[i]);
            }
            return (ErrorCode) IM_DeleteFriend(usersArray.ToJson(), deleteType);
        }

	    /*
	    * 功能：拉黑好友
	    * @param type：0：拉黑	1：解除拉黑
	    * @param users：用户ID
	    * @return 错误码
	    */
	    public ErrorCode BlackFriend(int type, List<string> users)
        {
            JsonData usersArray = new JsonData();
            for (int i = 0; i < users.Count; ++i)
            {
                usersArray.Add(users[i]);
            }
            return (ErrorCode)IM_BlackFriend(type, usersArray.ToJson());
        }

	    /*
	    * 功能：查询我的好友
	    * @param type：0：正常好友	1：被拉黑好友
	    * @param startIndex：起始序号
	    * @param count：数量（一次最大100）
	    * @return 错误码
	    */
	    public ErrorCode QueryFriends(int type = 0, int startIndex = 0, int count = 50)
        {
            return (ErrorCode) IM_QueryFriends(type, startIndex, count);
        }

	    /*
	    * 功能：查询好友请求列表
	    * @param startIndex：起始序号
	    * @param count：数量（一次最大20）
	    * @return 错误码
	    */
	    public ErrorCode QueryFriendRequestList(int startIndex = 0, int count = 20)
        {
            return (ErrorCode)IM_QueryFriendRequestList(startIndex, count);
        }

        //--------------------------------------------------------------------------

        /*
	    * 功能：设置用户基本资料
	    * @param userSettingInfo：用户基本信息 (昵称，性别，个性签名，国家，省份，城市，扩展信息)	    
	    * @return 错误码
	    */
		public ErrorCode SetUserProfileInfo (IMUserSettingInfo settingInfo)
		{			
		    Dictionary<string,string> settingInfoDic = new Dictionary<string, string>();
			settingInfoDic.Add("NickName",settingInfo.NickName);
			int tmp = (int)settingInfo.Sex;		
			settingInfoDic.Add("Sex",tmp.ToString());
			settingInfoDic.Add("Signature",settingInfo.Signature);
			settingInfoDic.Add("Country",settingInfo.Country);
			settingInfoDic.Add("Province",settingInfo.Province);
			settingInfoDic.Add("City",settingInfo.City);
			settingInfoDic.Add("ExtraInfo",settingInfo.ExtraInfo);			  
			          
			return (ErrorCode)IM_SetUserProfileInfo(JsonMapper.ToJson(settingInfoDic));
		}
        
		/*
	    * 功能：查询用户基本资料
	    * @param userID：指定用户ID，可选参数(为空查询自己的信息)	    
	    * @return 错误码
	    */
		public ErrorCode GetUserProfileInfo (string userID = "")
		{
		    return (ErrorCode)IM_GetUserProfileInfo(userID);
		}

        /*
	    * 功能：设置用户头像
	    * @param photoUrl：本地图片绝对路径	    
	    * @return 错误码
	    */
		public ErrorCode SetUserProfilePhoto (string photoPath)
		{		    
		    return (ErrorCode)IM_SetUserProfilePhoto(photoPath);
		}

        /*
	    * 功能：切换用户状态
	    * @param userID：用户ID
	    * @param userStatus：用户状态，在线 | 隐身 | 离线
	    * @return 错误码
	    */
		public ErrorCode SwitchUserStatus (string userID, UserStatus userStatus)
		{
		    return (ErrorCode)IM_SwitchUserStatus(userID, userStatus);
		}

        /*
	    * 功能：设置好友添加权限
	    * @param beFound：查找时权限，true-能被其它用户查找到，false-不能被其它用户查找到
	    * @param beAddPermission：被其它用户添加的权限
	    * @return 错误码
	    */
		public ErrorCode SetAddPermission (bool beFound, IMUserBeAddPermission beAddPermission)
		{
		    return (ErrorCode)IM_SetAddPermission(beFound, beAddPermission);
		}
	}
}
#endif