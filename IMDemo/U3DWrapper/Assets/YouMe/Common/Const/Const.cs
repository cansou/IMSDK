namespace YouMe
{
    public enum LogLevel
    {
        NONE = 0,
        FATAL  = 1,
        ERROR = 10,
        WARNING = 20,
        INFO = 40,
        ALL = 50
    };

    public enum ConnectStatus
    {
        DISCONNECTED = 0,
        CONNECTED  = 1,
        CONNECTING = 2,
        RECONNECTING = 3
    };

    public enum YIMUserStatus
    {
		ONLINE,	  //在线
        OFFLINE,  //离线
        UNKNOWN   //未知
    };

	public enum IMAudioDeviceStatus
    {        
        AVAILABLE,		// 可用
        NO_AUTHORITY,	// 无权限
        MUTE,			// 静音
        UNAVAILABLE,	// 不可用
        UNKNOWN         // 未知
    };

    public enum StatusCode{
        // =======================原始ERRORCODE保留========================
		Success = 0,                    //成功
		EngineNotInit = 1,              //未初始化
		NotLogin = 2,	                //未登录
		ParamInvalid = 3,               //参数错误
		TimeOut = 4,                    //超时
		StatusError = 5,                //状态错误
		SDKInvalid = 6,                 //SDK验证无效
		AlreadyLogin = 7,               //已经登录
		ServerError = 8,                //服务器错误
		NetError = 9,                   //网络错误
		LoginSessionError = 10,         //session错误
		NotStartUp = 11,                //未启动
		FileNotExist = 12,              //文件不存在
		SendFileError = 13,             //发送文件失败
		UploadFailed = 14,              //上传失败
		UsernamePasswordError = 15,     //用户名密码错误	
		UserStatusError = 16,	        //用户状态错误(无效用户)
		MessageTooLong = 17,	        //消息太长
		ReceiverTooLong = 18,	        //接收方ID过长(检查房间名)
		InvalidChatType = 19,	        //无效聊天类型(私聊、聊天室)
		InvalidReceiver = 20,	        //无效用户ID（私聊接受者为数字格式ID）
        UnknowError = 21,		        //常见于发送房间消息，房间号并不存在。修正的方法是往自己joinRoom的房间id发消息，就可以发送成功。
        InvalidAppkey = 22,			    //无效APPKEY
        ForbiddenSpeak = 23,			//被禁言
        CreateFileFailed = 24,          //无法创建文件
        UnsupportFormat = 25,			//不支持的文件格式
        ReceiverEmpty = 26,			    //接收方为空
        RoomIDTooLong = 27,			    //房间名太长
        ContentInvalid = 28,			//聊天内容严重非法
        NoLocationAuthrize = 29,		//未打开定位权限
		UnknowLocation = 30,            //未知位置
		Unsupport = 31,                 //不支持该接口
        NoAudioDevice = 32,	            //无音频设备
        AudioDriver = 33,				//音频驱动问题
        DeviceStatusInvalid = 34,		//设备状态错误
        ResolveFileError = 35,			//文件解析错误
        ReadWriteFileError = 36,		//文件读写错误
        NoLangCode = 37,				//语言编码错误
        TranslateUnable = 38,			//翻译接口不可用
		SpeechAccentInvalid = 39,		//语音识别方言无效
		SpeechLanguageInvalid = 40,	    //语音识别语言无效
		HasIllegalText = 41,			//消息含非法字符
		AdvertisementMessage = 42,		//消息涉嫌广告
		AlreadyBlock = 43,				//用户已经被屏蔽
		NotBlock = 44,					//用户未被屏蔽
		MessageBlocked = 45,			//消息被屏蔽
		LocationTimeout = 46,			//定位超时
		NotJoinRoom = 47,				//未加入该房间
		LoginTokenInvalid = 48,		    //登录token错误
		CreateDirectoryFailed = 49,	    //创建目录失败
		InitFailed = 50,				//初始化失败
		Disconnect = 51,				//与服务器断开

		//服务器的错误码
		ALREADYFRIENDS = 1000,
		LoginInvalid = 1001,

		//语音部分错误码
		PTT_Start = 2000,
		PTT_Fail = 2001,
		PTT_DownloadFail = 2002,              //下载语音失败
		PTT_GetUploadTokenFail = 2003,        //获取token失败
		PTT_UploadFail = 2004,                //上传失败
		PTT_NotSpeech = 2005,                 //未检测到语音或未开始语音
        PTT_DeviceStatusError = 2006,		  //音频设备状态错误
		PTT_IsSpeeching = 2007,			      //正在录音
		PTT_FileNotExist = 2008,              //文件不存在
        PTT_ReachMaxDuration = 2009,		  //达到语音最大时长限制
        PTT_SpeechTooShort = 2010,			  //语音时长太短
        PTT_StartAudioRecordFailed = 2011,	  //启动语音失败
        PTT_SpeechTimeout = 2012,			  //音频输入超时
        PTT_IsPlaying = 2013,				  //正在播放
        PTT_NotStartPlay = 2014,			  //未开始播放
        PTT_CancelPlay = 2015,				  //主动取消播放
        PTT_NotStartRecord = 2016,			  //未开始语音
		PTT_NotInit = 2017,				      // 未初始化
		PTT_InitFailed = 2018,				  // 初始化失败
		PTT_Authorize = 2019,				  // 录音权限
		PTT_StartRecordFailed = 2020,		  // 启动录音失败
		PTT_StopRecordFailed = 2021,		  // 停止录音失败
		PTT_UnsupprtFormat = 2022,			  // 不支持的格式
		PTT_ResolveFileError = 2023,		  // 解析文件错误
		PTT_ReadWriteFileError = 2024,		  // 读写文件错误
		PTT_ConvertFileFailed = 2025,		  // 文件转换失败
		PTT_NoAudioDevice = 2026,			  // 无音频设备
		PTT_NoDriver = 2027,				  // 驱动问题
		PTT_StartPlayFailed = 2028,		      // 启动播放失败
		PTT_StopPlayFailed = 2029,			  // 停止播放失败

		Fail = 10000,
        // =======================END 原始ERRORCODE保留========================

        // ==========================IM 状态码扩充================================
        Start_Download_Fail = 20001,          // 开始下载失败
        Is_Waiting_Download = 20002,          // 下载队列中待下载
		Is_Waiting_Upload = 20003,            // 上传队列中待上传
		Is_Waiting_Send = 20004,              // 消息队列中待发送
        Query_Records_Fail = 20005,           // 查询历史记录失败
        Get_Contacts_Fail = 20006,            // 获取联系人列表失败
		Get_User_Info_Fail = 20007,           // 获取用户信息失败
		Query_User_Status_Fail = 20008,       // 查询用户状态失败
		Start_Play_Fail = 20009,              // 开始播放语音失败
		StopPlay_Fail_Before_Start = 20010,   // 调用开始播放前停止播放失败
		Get_Current_Location_Fail = 20011,    // 获取当前地理位置失败
		Get_Nearby_Objects_Fail = 20012,      // 获取附近人失败
		Get_Microphone_Status_Fail = 20013,   // 获取麦克风状态失败
//		REPORT_FAIL = 20014,                  // 举报失败
		Get_Forbidden_SpeakInfo_Fail = 20015, // 获取禁言状态失败
		Block_User_Fail = 20016,              // 屏蔽用户失败
		Unblock_All_User_Fail = 20017,        // 解除所有的用户屏蔽失败
		Get_Block_Users_Fail = 20018,         // 获取屏蔽用户失败
		Has_No_Microphone_Authority = 20019,  // 没有麦克风权限
//		Query_RoomHistoryMsg_Fail = 20020     // 查询房间历史消息记录失败
        // =========================END状态码扩充==============================
    }

}