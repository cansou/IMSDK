using UnityEngine;
using System.Collections;

namespace YIMEngine
{
	public abstract class MessageInfoBase
	{
		private YIMEngine.ChatType chatType;
		private string strSenderID;
		private string strRecvID;
		private ulong iRequestID;
		private MessageBodyType messageType;
		private int iCreateTime;
        private uint iDistance;
        private bool bIsRead;
		public YIMEngine.ChatType ChatType {
			get {
				return chatType;
			}
			internal set {
				chatType = value;
			}
		}

		public int CreateTime {
			get {
				return iCreateTime;
			}
			set {
				iCreateTime = value;
			}
		}
		
		public string SenderID {
			get {
				return strSenderID;
			}
			set {
				strSenderID = value;
			}
		}
		
		public string RecvID {
			get {
				return strRecvID;
			}
			set {
				strRecvID = value;
			}
		}
		
		public MessageBodyType MessageType {
			get {
				return messageType;
			}
			set {
				messageType = value;
			}
		}

		public ulong RequestID {
			get {
				return iRequestID;
			}
			set {
				iRequestID = value;
			}
		}

        public uint Distance {
            get {
                return iDistance;
            }
            set {
                iDistance = value;
            }
        }

        public bool IsRead
        {
            get {
                return bIsRead;
            }
            set {
                bIsRead = value;
            }
        }
	}

	public class TextMessage:MessageInfoBase
	{
		private string strContent;
		private string strAttachParam;
		
		public string Content {
			get {
				return strContent;
			}
			set {
				strContent = value;
			}
		}
		public string AttachParam {
			get {
				return strAttachParam;
			}
			set {
				strAttachParam = value;
			}
		}
	}

	public class CustomMessage:MessageInfoBase
	{
		private byte[] strContent;
		
		public byte[] Content {
			get {
				return strContent;
			}
			set {
				strContent = value;
			}
		}
	}

	public class FileMessage:MessageInfoBase
	{
		private string strFileName;
		private int iFileSize;
		private FileType fFileType;
		private string strExtension;
		private string strExtParam;

		public string ExtParam {
			get {
				return strExtParam;
			}
			set {
				strExtParam = value;
			}
		}

		public string FileExtension {
			get {
				return strExtension;
			}
			set {
				strExtension = value;
			}
		}

		public string FileName {
			get {
				return strFileName;
			}
			set {
				strFileName = value;
			}
		}

		public int FileSize {
			get {
				return iFileSize;
			}
			set {
				iFileSize = value;
			}
		}

		public FileType FileType {
			get {
				return fFileType;
			}
			set {
				fFileType = value;
			}
		}
	}


	public class GiftMessage:MessageInfoBase
	{
		private int iGiftCount;
		private int iGiftID;
		private ExtraGifParam strParam;
		private string strAnchor;

		public string Anchor {
			get {
				return strAnchor;
			}
			set {
				strAnchor = value;
			}
		}

		public int GiftID {
			get {
				return iGiftID;
			}
			set {
				iGiftID = value;
			}
		}

		public ExtraGifParam ExtParam {
			get {
				return strParam;
			}
			set {
				strParam = value;
			}
		}

		public int GiftCount {
			get {
				return iGiftCount;
			}
			set {
				iGiftCount = value;
			}
		}
	}

	public class VoiceMessage:MessageInfoBase
	{
		private string strText;
		private string strParam;
		private int iDuration;

		public int Duration {
			get {
				return iDuration;
			}
			set {
				iDuration = value;
			}
		}

		public string Text {
			get {
				return strText;
			}
			set {
				strText = value;
			}
		}

		public string Param {
			get {
				return strParam;
			}
			set {
				strParam = value;
			}
		}
	}
	public enum ErrorCode
	{
		Success = 0,
		EngineNotInit = 1,
		NotLogin = 2,	
		ParamInvalid = 3,
		TimeOut = 4,
		StatusError = 5,
		SDKInvalid = 6,
		AlreadyLogin = 7,
		ServerError = 8,
		NetError = 9,
		LoginSessionError = 10,
		NotStartUp = 11,
		FileNotExist = 12,
		SendFileError = 13,
		UploadFailed = 14,
		UsernamePasswordError = 15,
		UserStatusError = 16,	
		MessageTooLong = 17,	//消息太长
		ReceiverTooLong = 18,	//接收方ID过长(检查房间名)
		InvalidChatType = 19,	//无效聊天类型(私聊、聊天室)
		InvalidReceiver = 20,	//无效用户ID（私聊接受者为数字格式ID）
        UnknowError = 21,		//常见于发送房间消息，房间号并不存在。修正的方法是往自己joinRoom的房间id发消息，就可以发送成功。
        InvalidAppkey = 22,			    //无效APPKEY
        ForbiddenSpeak = 23,			//被禁言
        CreateFileFailed = 24,          //无法创建文件
        UnsupportFormat = 25,			//不支持的文件格式
        ReceiverEmpty = 26,			    //接收方为空
        RoomIDTooLong = 27,			    //房间名太长
        ContentInvalid = 28,			//聊天内容严重非法
        NoLocationAuthrize = 29,		//未打开定位权限
        UnknowLocation = 30,			//未知位置
        Unsupport = 31,				    //不支持该接口
        NoAudioDevice = 32,			    //无音频设备
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
        AlreadyBlock = 43,	            //用户已经被屏蔽
        NotBlock = 44,		            //用户未被屏蔽
        MessageBlocked = 45,			//消息被屏蔽
		LocationTimeout = 46,			//定位超时
		NotJoinRoom = 47,				//未加入该房间
		LoginTokenInvalid = 48,		    //登录token错误
		CreateDirectoryFailed = 49,	    //创建目录失败
        InitFailed = 50,				//初始化失败
        Disconnect = 51,				//与服务器断开

        TheSameParam = 52,             //设置参数相同
        QueryUserInfoFail = 53,        //查询用户信息失败
        SetUserInfoFail = 54,          //设置用户信息失败
        UpdateUserOnlineStateFail = 55,//更新用户在线状态失败
        NickNameTooLong = 56,          //昵称太长(> 64 bytes)
        SignatureTooLong = 57,         //个性签名太长(> 120 bytes)
        NeedFriendVerify = 58,         //需要好友验证信息
        BeRefuse = 59,                 //添加好友被拒绝
        HasNotRegisterUserInfo = 60,   //未注册用户信息
        AlreadyFriend = 61,            //已经是好友
        NotFriend = 62,                //非好友
        NotBlack = 63,                 //不在黑名单中
        PhotoUrlTooLong = 64,          //头像url过长(>500 bytes)
        PhotoSizeTooLarge = 65,        //头像太大（>100 kb）
        ChannelMemberOverflow = 66,    //达到频道人数上限

		//服务器的错误码
		ALREADYFRIENDS = 1000,
		LoginInvalid = 1001,

		//语音部分错误码
		PTT_Start = 2000,
		PTT_Fail = 2001,
		PTT_DownloadFail = 2002,
		PTT_GetUploadTokenFail = 2003,
		PTT_UploadFail = 2004,
		PTT_NotSpeech = 2005,
        PTT_DeviceStatusError = 2006,		//音频设备状态错误
        PTT_IsSpeeching = 2007,			    //已开启语音
        PTT_FileNotExist = 2008, 
        PTT_ReachMaxDuration = 2009,		//达到语音最大时长限制
        PTT_SpeechTooShort = 2010,			//语音时长太短
        PTT_StartAudioRecordFailed = 2011,	//启动语音失败
        PTT_SpeechTimeout = 2012,			//音频输入超时
        PTT_IsPlaying = 2013,				//正在播放
        PTT_NotStartPlay = 2014,			//未开始播放
        PTT_CancelPlay = 2015,				//主动取消播放
        PTT_NotStartRecord = 2016,			//未开始语音
		PTT_NotInit = 2017,				    // 未初始化
		PTT_InitFailed = 2018,				// 初始化失败
		PTT_Authorize = 2019,				// 录音权限
		PTT_StartRecordFailed = 2020,		// 启动录音失败
		PTT_StopRecordFailed = 2021,		// 停止录音失败
		PTT_UnsupprtFormat = 2022,			// 不支持的格式
		PTT_ResolveFileError = 2023,		// 解析文件错误
		PTT_ReadWriteFileError = 2024,		// 读写文件错误
		PTT_ConvertFileFailed = 2025,		// 文件转换失败
		PTT_NoAudioDevice = 2026,			// 无音频设备
		PTT_NoDriver = 2027,				// 驱动问题
		PTT_StartPlayFailed = 2028,		    // 启动播放失败
		PTT_StopPlayFailed = 2029,			// 停止播放失败
        PTT_RecognizeFailed = 2030,         // 识别失败
		Fail = 10000
	} 

	//群类型
	public enum GroupType
	{
		//普通群
		Group = 0,
		//聊天室
		ChatRoom = 1
	};

	public enum GroupEvent
	{
		Agree = 0,
		Refuse = 1,
	};

	public enum ChatType
	{
		Unknow = 0,
		PrivateChat = 1,
		RoomChat = 2,
	};

	public enum FileType
	{
		FileType_Other = 0,
		FileType_Audio = 1,
		FileType_Image = 2,
		FileType_Video = 3
	};
	public enum MessageBodyType
	{
		Unknow = 0,
		TXT = 1,
		CustomMesssage = 2,
		Emoji = 3,
		Image = 4,
		Voice = 5,
		Video = 6,
		File = 7,
		Gift = 8
	};
	public enum Command
	{
		CMD_UNKNOW = 0,
		CMD_LOGIN = 1,
		CMD_HEARTBEAT = 2,
		CMD_LOGOUT = 3,
		CMD_ENTER_ROOM = 4,
		CMD_LEAVE_ROOM = 5,
		CMD_SND_TEXT_MSG = 6,
		CMD_SND_VOICE_MSG = 7,
		CMD_SND_FILE_MSG = 8,
		CMD_GET_MSG = 9,
		CMD_GET_UPLOAD_TOKEN = 10,
		CMD_KICK_OFF = 11,
		CMD_SND_BIN_MSG = 12,
		CMD_RELOGIN = 13,
		CMD_CHECK_ONLINE = 14,
		CMD_SND_GIFT_MSG = 15,
		CMD_GET_ROOM_HISTORY_MSG = 16,
		CMD_GET_USR_INFO = 17,
		CMD_UPDATE_USER_INFO = 18,
        CMD_SND_TIPOFF_MSG = 19,
        CMD_GET_TIPOFF_MSG = 20,
        CMD_GET_DISTRICT = 21,
        CMD_GET_PEOPLE_NEARBY = 22,
        CMD_QUERY_NOTICE = 23,
        CMD_SET_MASK_USER_MSG = 24,		// 屏蔽用户
        CMD_GET_MASK_USER_MSG = 25,		// 获取屏蔽用户
        CMD_CLEAN_MASK_USER_MSG = 26,	// 解除所有屏蔽用户
        CMD_GET_ROOM_INFO = 27,			// 获取房间信息(人数)
		CMD_LEAVE_ALL_ROOM = 28,        // 离开所有房间
		CMD_GET_FORBID_RECORD = 31,

        CMD_REGISTER_USER_PROFILE = 36,
        CMD_QUERY_USER_PROFILE = 37,
        CMD_UPDATE_USER_PROFILE = 38,
        CMD_UPDATE_ONLINE_STATUS = 39,
        CMD_FIND_FRIEND_BY_ID = 40,				// 按ID查找好友
        CMD_FIND_FRIEND_BY_NICKNAME = 41,		// 按昵称查找好友
        CMD_REQUEST_ADD_FRIEND = 42,			// 请求添加好友
        CMD_FRIND_NOTIFY = 44,					// 好友请求通知
        CMD_DELETE_FRIEND = 45,					// 删除好友
        CMD_BLACK_FRIEND = 46,					// 拉黑好友
        CMD_UNBLACK_FRIEND = 47,				// 解除黑名单
        CMD_DEAL_ADD_FRIEND = 48,				// 好友验证
        CMD_QUERY_FRIEND_LIST = 49,				// 获取好友列表
        CMD_QUERY_BLACK_FRIEND_LIST = 50,		// 获取黑名单列表
        CMD_QUERY_FRIEND_REQUEST_LIST = 51,		// 获取好友验证消息列表
        CMD_UPDATE_FRIEND_RQUEST_STATUS = 52,	// 更新好友请求状态
        CMD_RELATION_CHAIN_HEARTBEAT = 53,		// 关系链心跳

		CMD_HXR_USER_INFO_CHANGE_NOTIFY = 74,	// 用户信息变更通知

        CMD_SET_READ_STATUS = 76,			    // 发送端设置消息已读

		//服务器通知
		NOTIFY_LOGIN = 10001,
		NOTIFY_PRIVATE_MSG,
		NOTIFY_ROOM_MSG,

		NOTIFY_ROOM_GENERAL,

		NOTIFY_UPDATE_CONFIG = 10008,

        NOTIFY_PRIVATE_MSG_V2 = 10012,
        NOTIFY_ROOM_MSG_V2 = 10013,

		//客户端(C接口使用)
		CMD_DOWNLOAD = 20001,
		CMD_SEND_MESSAGE_STATUS = 20002,
		CMD_RECV_MESSAGE = 20003,
		CMD_STOP_AUDIOSPEECH = 20004,
		CMD_QUERY_HISTORY_MESSAGE = 20005,
		CMD_GET_RENCENT_CONTACTS = 20006,
		CMD_RECEIVE_MESSAGE_NITIFY = 20007,
		CMD_QUERY_USER_STATUS = 20008,
		CMD_AUDIO_PLAY_COMPLETE = 20009,
		CMD_STOP_SEND_AUDIO = 20010,
		CMD_TRANSLATE_COMPLETE = 20011,
		CMD_DOWNLOAD_URL = 20012,
		CMD_GET_MICROPHONE_STATUS = 20013,
		CMD_USER_ENTER_ROOM = 20014,
		CMD_USER_LEAVE_ROOM = 20015,
		CMD_RECV_NOTICE = 20016,
		CMD_CANCEL_NOTICE = 20017,
        
		CMD_GET_SPEECH_TEXT = 20018,			// 仅需要语音的文字识别内容
		CMD_GET_RECONNECT_RESULT = 20019,		// 重连结果
		CMD_START_RECONNECT = 20020,			// 开始重连
		CMD_RECORD_VOLUME = 20021,				// 音量
		CMD_GET_DISTANCE = 20022,				// 获取双方地理位置距离
		CMD_REQUEST_ADD_FRIEND_NOTIFY = 20023,	// 请求添加好友通知
		CMD_ADD_FRIENT_RESULT_NOTIFY = 20024,	// 添加好友结果通知
		CMD_BE_ADD_FRIENT = 20025,				// 被好友添加通知
		CMD_BE_DELETE_FRIEND_NOTIFY = 20026,	// 被好友删除通知
		CMD_BE_BLACK_FRIEND_NOTIFY = 20027,		// 被好友拉黑通知
        CMD_GET_USER_PROFILE = 20028,           //关系链-查询用户信息
        CMD_SET_USER_PROFILE = 20029,           //关系链-设置用户信息
        CMD_SET_USER_PHOTO = 20030,             //关系链-设置头像
        CMD_SWITCH_USER_STATE = 20031,          //关系链-切换用户在线状态
	}

	public enum ServerZone
	{
		China = 0,		// 中国
		Singapore = 1,	// 新加坡
		America = 2,		// 美国
		HongKong = 3,	// 香港
		Korea = 4,		// 韩国
		Australia = 5,	// 澳洲
		Deutschland = 6,	// 德国
		Brazil = 7,		// 巴西
		India = 8,		// 印度
		Japan = 9,		// 日本
		Ireland = 10,	// 爱尔兰
		ServerZone_Unknow = 9999
	};
	public class HistoryMsg
	{
		
		private ChatType iChatType;
		MessageBodyType iMessageType;
		string strParam;
		string strReceiveID;
		string strSenderID;
		ulong uMessageID;
		string strText;
		string strLocalPath;
		int iCreateTime;
		int iDuration;
        bool bIsRead;
        byte[] customMsg;
        string strFileName;
        string strFileExtension;
        int iFileSize;

        // 消息内容类型
        public MessageBodyType MessageType {
			get {
				return iMessageType;
			}
			set {
				iMessageType = value;
			}
		}
		// 聊天类型，私聊 or 频道聊天，目前历史记录都是私聊
		public ChatType ChatType {
			get {
				return iChatType;
			}
			set {
				iChatType = value;
			}
		}
		// 消息收发时间
		public int CreateTime {
			get {
				return iCreateTime;
			}
			set {
				iCreateTime = value;
			}
		}
		// 语音消息的wav文件本地路径
		public string LocalPath {
			get {
				return strLocalPath;
			}
			set {
				strLocalPath = value;
			}
		}
		// 文本消息内容 或者 语音消息的文本识别内容
		public string Text {
			get {
				return strText;
			}
			set {
				strText = value;
			}
		}
		// 历史记录消息ID
		public ulong MessageID {
			get {
				return uMessageID;
			}
			set {
				uMessageID = value;
			}
		}

		// 消息发送者id
		public string SenderID {
			get {
				return strSenderID;
			}
			set {
				strSenderID = value;
			}
		}

		// 消息接收者id
		public string ReceiveID {
			get {
				return strReceiveID;
			}
			set {
				strReceiveID = value;
			}
		}

		// 如果是语音消息，该值表示语音消息的自定义附加参数
		public string Param {
			get {
				return strParam;
			}
			set {
				strParam = value;
			}
		}

		// 如果是语音消息，该值表示语音消息时长
		public int Duration {
			get {
				return iDuration;
			}
			set {
				iDuration = value;
			}
		}

        public bool IsRead {
            get {
                return bIsRead;
            }
            set {
                bIsRead = value;
            }
        }

		public byte[] CustomMsg{
			get{
                return System.Convert.FromBase64String(Text);
            }
		}

		// 文件大小
		public int FileSize {
			get {
				return iFileSize;
			}
			set {
				iFileSize = value;
			}
		}
		// 文件名
		public string FileName {
			get {
				return strFileName;
			}
			set {
				strFileName = value;
			}
		}
		// 文件扩展信息
		public string FileExtension {
			get {
				return strFileExtension;
			}
			set {
				strFileExtension = value;
			}
		}
	};

    public class ContactsSessionInfo
    {
        private string strContactID;
        private MessageBodyType iMessageType;
        private string strMessageContent;
        private int iCreateTime;
        private uint iNotReadMsgNum;

        //联系人ID
        public string ContactID{
            get{
                return strContactID;
            }
            set{
                strContactID = value;
            }
        }

        //消息类型
        public MessageBodyType MessageType{
            get{
                return iMessageType;
            }
            set{
                iMessageType = value;
            }
        }

        //消息内容
        public string MessageContent{
            get{
                return strMessageContent;
            }
            set{
                strMessageContent = value;
            }
        }

        // 消息时间
        public int CreateTime{
            get{
                return iCreateTime;
            }
            set{
                iCreateTime = value;
            }
        }

        // 未读消息数量
        public uint NotReadMsgNum {
			get {
				return iNotReadMsgNum;
			}
			set {
				iNotReadMsgNum = value;
			}
        }
    }


    //用户状态
    public enum UserStatus
    {
        STATUS_ONLINE,	//在线
        STATUS_OFFLINE,	//离线
        STATUS_INVISIBLE = 2 //隐身

    };

	public enum ForbidSpeakReason{
		Unkown = 0,         //未知
		AD      = 1,        //发广告
		Insult  = 2,        //侮辱
		Politics  = 3,      //政治敏感
		Terrorism  = 4,     //恐怖主义
		Reaction  = 5,      //反动
		Sexy  = 6,          //色情
		Other  = 7,         //其他
	};
	
	public enum LanguageCode
    {
        LANG_AUTO,
    LANG_AF,            // 南非荷兰语
    LANG_AM,            // 阿姆哈拉语
    LANG_AR,            // 阿拉伯语
    LANG_AR_AE,            // 阿拉伯语+阿拉伯联合酋长国
    LANG_AR_BH,            // 阿拉伯语+巴林
    LANG_AR_DZ,            // 阿拉伯语+阿尔及利亚
    LANG_AR_KW,            // 阿拉伯语+科威特
    LANG_AR_LB,            // 阿拉伯语+黎巴嫩
    LANG_AR_OM,            // 阿拉伯语+阿曼
    LANG_AR_SA,            // 阿拉伯语+沙特阿拉伯
    LANG_AR_SD,            // 阿拉伯语+苏丹
    LANG_AR_TN,            // 阿拉伯语+突尼斯
    LANG_AZ,            // 阿塞拜疆
    LANG_BE,            // 白俄罗斯语
    LANG_BG,            // 保加利亚语
    LANG_BN,            // 孟加拉
    LANG_BS,            // 波斯尼亚语
    LANG_CA,            // 加泰罗尼亚语
    LANG_CA_ES,            // 加泰罗尼亚语+西班牙
    LANG_CO,            // 科西嘉
    LANG_CS,            // 捷克语
    LANG_CY,            // 威尔士语
    LANG_DA,            // 丹麦语
    LANG_DE,            // 德语
    LANG_DE_CH,            // 德语+瑞士
    LANG_DE_LU,            // 德语+卢森堡
    LANG_EL,            // 希腊语
    LANG_EN,            // 英语
    LANG_EN_CA,            // 英语+加拿大
    LANG_EN_IE,            // 英语+爱尔兰
    LANG_EN_ZA,            // 英语+南非
    LANG_EO,            // 世界语
    LANG_ES,            // 西班牙语
    LANG_ES_BO,            // 西班牙语+玻利维亚
    LANG_ES_AR,            // 西班牙语+阿根廷
    LANG_ES_CO,            // 西班牙语+哥伦比亚
    LANG_ES_CR,            // 西班牙语+哥斯达黎加
    LANG_ES_ES,            // 西班牙语+西班牙
    LANG_ET,            // 爱沙尼亚语
    LANG_ES_PA,            // 西班牙语+巴拿马
    LANG_ES_SV,            // 西班牙语+萨尔瓦多
    LANG_ES_VE,            // 西班牙语+委内瑞拉
    LANG_ET_EE,            // 爱沙尼亚语+爱沙尼亚
    LANG_EU,            // 巴斯克
    LANG_FA,            // 波斯语
    LANG_FI,            // 芬兰语
    LANG_FR,            // 法语
    LANG_FR_BE,            // 法语+比利时
    LANG_FR_CA,            // 法语+加拿大
    LANG_FR_CH,            // 法语+瑞士
    LANG_FR_LU,            // 法语+卢森堡
    LANG_FY,            // 弗里斯兰
    LANG_GA,            // 爱尔兰语
    LANG_GD,            // 苏格兰盖尔语
    LANG_GL,            // 加利西亚
    LANG_GU,            // 古吉拉特文
    LANG_HA,            // 豪撒语
    LANG_HI,            // 印地语
    LANG_HR,            // 克罗地亚语
    LANG_HT,            // 海地克里奥尔
    LANG_HU,            // 匈牙利语
    LANG_HY,            // 亚美尼亚
    LANG_ID,            // 印度尼西亚
    LANG_IG,            // 伊博
    LANG_IS,            // 冰岛语
    LANG_IT,            // 意大利语
    LANG_IT_CH,            // 意大利语+瑞士
    LANG_JA,            // 日语
    LANG_KA,            // 格鲁吉亚语
    LANG_KK,            // 哈萨克语
    LANG_KN,            // 卡纳达
    LANG_KM,            // 高棉语
    LANG_KO,            // 朝鲜语
    LANG_KO_KR,            // 朝鲜语+南朝鲜
    LANG_KU,            // 库尔德
    LANG_KY,            // 吉尔吉斯斯坦
    LANG_LA,            // 拉丁语
    LANG_LB,            // 卢森堡语
    LANG_LO,            // 老挝
    LANG_LT,            // 立陶宛语
    LANG_LV,            // 拉托维亚语+列托
    LANG_MG,            // 马尔加什
    LANG_MI,            // 毛利
    LANG_MK,            // 马其顿语
    LANG_ML,            // 马拉雅拉姆
    LANG_MN,            // 蒙古
    LANG_MR,            // 马拉地语
    LANG_MS,            // 马来语
    LANG_MT,            // 马耳他
    LANG_MY,            // 缅甸
    LANG_NL,            // 荷兰语
    LANG_NL_BE,            // 荷兰语+比利时
    LANG_NE,            // 尼泊尔
    LANG_NO,            // 挪威语
    LANG_NY,            // 齐切瓦语
    LANG_PL,            // 波兰语
    LANG_PS,            // 普什图语
    LANG_PT,            // 葡萄牙语
    LANG_PT_BR,            // 葡萄牙语+巴西
    LANG_RO,            // 罗马尼亚语
    LANG_RU,            // 俄语
    LANG_SD,            // 信德
    LANG_SI,            // 僧伽罗语
    LANG_SK,            // 斯洛伐克语
    LANG_SL,            // 斯洛语尼亚语
    LANG_SM,            // 萨摩亚
    LANG_SN,            // 修纳
    LANG_SO,            // 索马里
    LANG_SQ,            // 阿尔巴尼亚语
    LANG_SR,            // 塞尔维亚语
    LANG_ST,            // 塞索托语
    LANG_SU,            // 巽他语
    LANG_SV,            // 瑞典语
    LANG_SV_SE,            // 瑞典语+瑞典
    LANG_SW,            // 斯瓦希里语
    LANG_TA,            // 泰米尔
    LANG_TE,            // 泰卢固语
    LANG_TG,            // 塔吉克斯坦
    LANG_TH,             //泰语
    LANG_TL,            // 菲律宾
    LANG_TR,            // 土耳其语
    LANG_UK,            // 乌克兰语
    LANG_UR,            // 乌尔都语
    LANG_UZ,            // 乌兹别克斯坦
    LANG_VI,            // 越南
    LANG_XH,            // 科萨
    LANG_YID,            // 意第绪语
    LANG_YO,            // 约鲁巴语
    LANG_ZH,            // 汉语
    LANG_ZH_TW,         // 繁体
    LANG_ZU                // 祖鲁语
    };

    public class GeographyLocation
    {
        uint iDistrictCode;
        string strCountry;
        string strProvince;
        string strCity;
        string strDistrictCounty;
        string strStreet;
        double fLongitude;
        double fLatitude;

        public uint DistrictCode {
            get {
                return iDistrictCode;
            }
            set {
                iDistrictCode = value;
            }
        }

        public string Country {
			get {
                return strCountry;
			}
			set {
                strCountry = value;
			}
		}

        public string Province {
            get {
                return strProvince;
            }
            set {
                strProvince = value;
            }
        }

        public string City {
            get {
                return strCity;
            }
            set {
                strCity = value;
            }
        }

        public string DistrictCounty {
            get {
                return strDistrictCounty;
            }
            set {
                strDistrictCounty = value;
            }
        }

        public string Street {
            get {
                return strStreet;
            }
            set {
                strStreet = value;
            }
        }

        public double Longitude
        {
            get {
                return fLongitude;
            }
            set {
                fLongitude = value;
            }
        }
        public double Latitude
        {
            get {
                return fLatitude;
            }
            set {
                fLatitude = value;
            }
        }
    };

	public class ForbiddenSpeakInfo
	{
		string channelID;
		bool isForbidRoom;
		int  reasonType;
		ulong  endTime;

		public string ChannelID{
			get{
				return channelID;
			}
			set{
				channelID = value;
			}
		}

		public bool IsForbidRoom{
			get{
				return isForbidRoom;
			}
			set{
				isForbidRoom = value;
			}
		}

		public int ReasonType{
			get{
				return reasonType;
			}
			set{
				reasonType = value;
			}
		}


		public ulong EndTime{
			get{
				return endTime;
			}
			set{
				endTime = value;
			}
		}
	};

    public class RelativeLocation
    {
        uint iDistance;
        double fLongitude;
        double fLatitude;
        string strUserID;
        string strCountry;
        string strProvince;
        string strCity;
        string strDistrictCounty;
        string strStreet;

        public uint Distance {
            get {
                return iDistance;
            }
            set {
                iDistance = value;
            }
        }

        public double Longitude {
            get {
                return fLongitude;
            }
            set {
                fLongitude = value;
            }
        }
        public double Latitude {
            get {
                return fLatitude;
            }
            set {
                fLatitude = value;
            }
        }

        public string UserID {
            get {
                return strUserID;
            }
            set {
                strUserID = value;
            }
        }

        public string Country {
            get {
                return strCountry;
            }
            set {
                strCountry = value;
            }
        }

        public string Province {
            get {
                return strProvince;
            }
            set {
                strProvince = value;
            }
        }

        public string City {
            get {
                return strCity;
            }
            set {
                strCity = value;
            }
        }

        public string DistrictCounty {
            get {
                return strDistrictCounty;
            }
            set {
                strDistrictCounty = value;
            }
        }

        public string Street {
            get {
                return strStreet;
            }
            set {
                strStreet = value;
            }
        }
    }

    public enum DistrictLevel
    {
        DISTRICT_UNKNOW,
        DISTRICT_COUNTRY,	// 国家
        DISTRICT_PROVINCE,	// 省份
        DISTRICT_CITY,		// 市
        DISTRICT_COUNTY,	// 区县
        DISTRICT_STREET		// 街道
    };

    // 举报处理结果
    public enum AccusationDealResult
    {
        ACCUSATIONRESULT_IGNORE,			// 忽略
        ACCUSATIONRESULT_WARNING,			// 警告
        ACCUSATIONRESULT_FROBIDDEN_SPEAK	// 禁言
    };


    public enum SpeechLanguage
    {
        SPEECHLANG_MANDARIN,	// 普通话(Android IOS Windows)
        SPEECHLANG_YUEYU,		// 粤语(Android IOS Windows)
        SPEECHLANG_SICHUAN,		// 四川话(Android IOS)
        SPEECHLANG_HENAN,		// 河南话(IOS)
        SPEECHLANG_ENGLISH,		// 英语(Android IOS Windows)
        SPEECHLANG_TRADITIONAL	// 繁体中文(Android IOS Windows)
    };

    public enum AudioDeviceStatus
    {
        STATUS_AVAILABLE,		// 可用
        STATUS_NO_AUTHORITY,	// 无权限
        STATUS_MUTE,			// 静音
        STATUS_UNAVAILABLE		// 不可用
    };

	public enum SampleRateType
	{
		SAMPLERATE_8 = 0,       // 8000
		SAMPLERATE_16 = 1,	    // 16000
		SAMPLERATE_32 = 2,      // 32000
		SAMPLERATE_44_1 = 3,    // 44100
		SAMPLERATE_48 = 4       // 48000
	};
	
	public enum ReconnectResult
	{
		RECONNECTRESULT_SUCCESS,        // 重连成功
		RECONNECTRESULT_FAIL_AGAIN,     // 重连失败，再次重连
		RECONNECTRESULT_FAIL           // 重连失败
	};

    //编码格式
    public enum CodingFormat
	{
		CODING_OPUS = 0,  //opus编解码
        CODING_AMR = 1    //amr编解码
	};

    public class Notice
    {
        ulong uNoticeID;
	    int iNoticeType;
	    string strChannelID;
	    string strContent;
	    string strLinkeText;
	    string strLinkAddr;
	    uint iBeginTime;
	    uint iEndTime;

        public ulong NoticeID
        {
            get{
                return uNoticeID;
            }
            set{
                uNoticeID = value;
            }
        }
        
        public int NoticeType
        {
            get{
                return iNoticeType;
            }
            set{
                iNoticeType = value;
            }
        }

        public string ChannelID
        {
            get{
                return strChannelID;
            }
            set{
                strChannelID = value;
            }
        }

        public string Content
        {
            get{
                return strContent;
            }
            set{
                strContent = value;
            }
        }

        public string LinkText
        {
            get{
                return strLinkeText;
            }
            set{
                strLinkeText = value;
            }
        }

        public string LinkAddr
        {
            get{
                return strLinkAddr;
            }
            set{
                strLinkAddr = value;
            }
        }

        public uint BeginTime
        {
            get{
                return iBeginTime;
            }
            set{
                iBeginTime = value;
            }
        }

        public uint EndTime
        {
            get{
                return iEndTime;
            }
            set{
                iEndTime = value;
            }
        }
    }


    public class UserBriefInfo
    {
        string userID;
        string nickname;
		UserStatus status;


        public string UserID
        {
            get
            {
                return userID;
            }
            set
            {
                userID = value;
            }
        }

        public string Nickname
        {
            get
            {
                return nickname;
            }
            set
            {
                nickname = value;
            }
        }

		public UserStatus Status
        {
            get
            {
                return status;
            }
            set
            {
                status = value;
            }
        }
    }
	public enum AddFriendStatus
	{
		STATUS_ADD_SUCCESS = 0,			// 添加完成
		STATUS_ADD_FAILED = 1,			// 添加失败
		STATUS_WAIT_OTHER_VALIDATE = 2,	// 等待对方验证
		STATUS_WAIT_ME_VALIDATE = 3		// 等待我验证
	}

    public class FriendRequestInfo
    {
        string askerID;         // 请求方ID
	    string askerNickname;   // 请求方昵称
	    string inviteeID;       // 受邀方
	    string inviteeNickname; // 受邀方昵称
	    string validateInfo;    // 验证信息
		AddFriendStatus status;             // 状态
	    uint createTime;        // 时间
        ulong reqID;        // 请求唯一编号

        public string AskerID
        {
            get
            {
                return askerID;
            }
            set
            {
                askerID = value;
            }
        }

        public string AskerNickname
        {
            get
            {
                return askerNickname;
            }
            set
            {
                askerNickname = value;
            }
        }

        public string InviteeID
        {
            get
            {
                return inviteeID;
            }
            set
            {
                inviteeID = value;
            }
        }

        public string InviteeNickname
        {
            get
            {
                return inviteeNickname;
            }
            set
            {
                inviteeNickname = value;
            }
        }

        public string ValidateInfo
        {
            get
            {
                return validateInfo;
            }
            set
            {
                validateInfo = value;
            }
        }

		public AddFriendStatus Status
        {
            get
            {
                return status;
            }
            set
            {
                status = value;
            }
        }

        public uint CreateTime
        {
            get
            {
                return createTime;
            }
            set
            {
                createTime = value;
            }
        }

        public ulong ReqID
        {
            get
            {
                return reqID;
            }
            set
            {
                reqID = value;
            }
        }
    }

	//用户性别
	public enum IMUserSex
	{
		SEX_UNKNOWN, //未知性别
		SEX_MALE,    //男性
		SEX_FEMALE   //女性
	};

	public enum IMUserBeAddPermission
	{
	    NOT_ALLOW_ADD,     //不允许被添加
	    NEED_VALIDATE,     //需要验证
	    NO_ADD_PERMISSION  //允许被添加，不需要验证, 默认值
	};

	public enum IMUserFoundPermission
	{
	    CAN_BE_FOUND,        //能被其它用户查找到，默认值
	    CAN_NOT_BE_FOUND     //不能被其它用户查找到
	};

	public class IMUserSettingInfo
	{
	    private string strNickName;
	    private IMUserSex iSex;
	    private string strSignature;
		private string strCountry;
		private string strProvince;
		private string strCity;
		private string strExtraInfo;

		public IMUserSettingInfo ()
		{
		   this.strNickName = "";
		   this.iSex = IMUserSex.SEX_UNKNOWN;
		   this.strSignature = "";
		   this.strCountry = "";
		   this.strProvince = "";
		   this.strCity = "";
		   this.strExtraInfo = "";
		}

		public string NickName
        {
            get{
                return strNickName;
            }
            set{
                strNickName = value;
            }
        }
        public IMUserSex Sex
        {
			get{
                return iSex;
            }
            set{
                iSex = value;
            }
        }
		public string Signature
        {
            get{
                return strSignature;
            }
            set{
                strSignature = value;
            }
        }
		public string Country
        {
            get{
                return strCountry;
            }
            set{
                strCountry = value;
            }
        }
		public string Province
        {
            get{
                return strProvince;
            }
            set{
                strProvince = value;
            }
        }
		public string City
        {
            get{
                return strCity;
            }
            set{
                strCity = value;
            }
        }
		public string ExtraInfo
        {
            get{
                return strExtraInfo;
            }
            set{
                strExtraInfo = value;
            }
        }        
	}

	public class IMUserProfileInfo: IMUserSettingInfo
	{
	    private string strUserID;
	    private string strPhotoURL;
		private UserStatus iOnlineState;
		private IMUserBeAddPermission iBeAddPermission;
		private IMUserFoundPermission iFoundPermission;

		public string UserID {
			get {
				return strUserID;
			}
			set {
			    strUserID = value;
			}
		}
		public string PhotoURL {
			get {
				return strPhotoURL;
			}
			set {
			    strPhotoURL = value;
			}
		}
		public UserStatus OnlineState {
			get {
				return iOnlineState;
			}
			set {
			    iOnlineState = value;
			}
		}
		public IMUserBeAddPermission BeAddPermission {
			get {
				return iBeAddPermission;
			}
			set {
			    iBeAddPermission = value;
			}
		}
		public IMUserFoundPermission FoundPermission {
			get {
				return iFoundPermission;
			}
			set {
			    iFoundPermission = value;
			}
		}
	}
}