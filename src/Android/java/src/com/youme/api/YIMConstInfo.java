package com.youme.api;

/**
 * Created by winnie on 2018/7/30.
 */

public class YIMConstInfo {
    public final static class Errorcode{
        public final static int Success = 0;
        public final static int EngineNotInit = 1;
        public final static int NotLogin = 2;
        public final static int ParamInvalid = 3;
        public final static int TimeOut = 4;
        public final static int StatusError = 5;
        public final static int SDKInvalid = 6;
        public final static int AlreadyLogin = 7;
        public final static int ServerError = 8;
        public final static int NetError = 9;
        public final static int LoginSessionError = 10;
        public final static int NotStartUp = 11;
        public final static int FileNotExist = 12;
        public final static int SendFileError = 13;
        public final static int UploadFailed = 14;
        public final static int UsernamePasswordError = 15;
        public final static int UserStatError = 16;
        public final static int MessageTooLong = 17;   //消息太长
        public final static int ReceiverTooLong = 18;  //接收方ID过长(检查房间名)
        public final static int InvalidChatType = 19;  //无效聊天类型(私聊、聊天室)
        public final static int InvalidReceiver = 20;  //无效用户ID（私聊接受者为）
        public final static int UnknowError = 21;
        public final static int InvalidAppkey = 22;
        public final static int ForbiddenSpeak = 23;
        public final static int CreateFileFailed = 24;

        public final static int UnsupportFormat = 25;   //不支持的文件格式
        public final static int ReceiverEmpty   = 26;   //接收方为空
        public final static int RoomIDTooLong = 27;     //房间名太长
        public final static int ContentInvalid = 28;    //聊天内容严重非法
        public final static int NoLocationAuthrize = 29;//未打开定位权限
        public final static int UnknowLocation = 30;    //未知位置
        public final static int Unsupport  = 31;        //不支持该接口
        public final static int NoAudioDevice = 32;     //无音频设备
        public final static int AudioDriver = 33;       //音频驱动问题
        public final static int DeviceStatusInvalid = 34;//设备状态错误
        public final static int ResolveFileError = 35;  //文件解析错误
        public final static int ReadWriteFileError = 36;//文件读写错误
        public final static int NoLangCode = 37;        //语言编码错误
        public final static int TranslateUnable = 38;   //翻译接口不可用
        public final static int SpeechAccentInvalid = 39;//语音识别方言无效
        public final static int SpeechLanguageInvalid = 40;//语音识别语言无效
        public final static int HasIllegalText = 41;			//消息含非法字符
        public final static int	AdvertisementMessage = 42;		//消息涉嫌广告
        public final static int AlreadyBlock = 43;				//用户已经被屏蔽
        public final static int NotBlock = 44;					//用户未被屏蔽
        public final static int MessageBlocked = 45;			//消息被屏蔽
        public final static int LocationTimeout = 46;			//定位超时
        public final static int NotJoinRoom = 47;				//未加入该房间
        public final static int LoginTokenInvalid = 48;			//登录token错误
        public final static int CreateDirectoryFailed = 49;		//创建目录失败
        public final static int InitFailed = 50;				//初始化失败
        public final static int Disconnect = 51;				//与服务器断开


        //服务器的错误码
        public final static int ALREADYFRIENDS = 1000;
        public final static int LoginInvalid = 1001;

        //语音部分错误码
        public final static int PTT_Start = 2000;
        public final static int PTT_Fail = 2001;
        public final static int PTT_DownloadFail = 2002;
        public final static int PTT_GetUploadTokenFail = 2003;
        public final static int PTT_UploadFail = 2004;
        public final static int PTT_NotSpeech = 2005;
        public final static int PTT_DeviceStatusError = 2006;
        public final static int PTT_IsSpeeching = 2007;
        public final static int PTT_FileNotExist = 2008;
        public final static int PTT_ReachMaxDuration = 2009;
        public final static int PTT_SpeechTooShort = 2010;
        public final static int PTT_StartAudioRecordFailed = 2011;
        public final static int PTT_SpeechTimeout = 2012;			//音频输入超时
        public final static int PTT_IsPlaying = 2013;				//正在播放
        public final static int PTT_NotStartPlay = 2014;			//未开始播放
        public final static int PTT_CancelPlay = 2015;				//主动取消播放
        public final static int PTT_NotStartRecord = 2016;			//未开始语音

        public final static int PTT_NotInit = 2017;			        // 未初始化
        public final static int PTT_InitFailed = 2018;				// 初始化失败
        public final static int PTT_Authorize = 2019;				// 录音权限
        public final static int PTT_StartRecordFailed = 2020;		// 启动录音失败
        public final static int PTT_StopRecordFailed = 2021;		// 停止录音失败
        public final static int PTT_UnsupprtFormat = 2022;			// 不支持的格式
        public final static int PTT_ResolveFileError = 2023;		// 解析文件错误
        public final static int PTT_ReadWriteFileError = 2024;		// 读写文件错误
        public final static int PTT_ConvertFileFailed = 2025;		// 文件转换失败
        public final static int PTT_NoAudioDevice = 2026;			// 无音频设备
        public final static int PTT_NoDriver = 2027;				// 驱动问题
        public final static int PTT_StartPlayFailed = 2028;		    // 启动播放失败
        public final static int PTT_StopPlayFailed = 2029;			// 停止播放失败
        /**
         * Java安全验证错误码
         */
        public final static int InvalidUserId = 3000;
        public final static int InvalidRoomId = 3001;
        public final static int InvalidPassword = 3002;
        public final static int InvalidStoragaePath = 3003;
        public final static int InvalidAppKey = 3004;
        public final static int InvalidContext = 3005;
        public final static int InvaliddSecretKey = 3006;

        /**
         * Java 独立错误码
         */
        public final static int IsWaitingSend = 4000;
        public final static int IsWaitingDownload = 4001;
        public final static int IsWaitingUpload = 4002;
        public final static int StopPlayFailBeforeStart = 4003;
        public final static int IsWaitingJoin = 4004;
        public final static int IsWaitingLeave = 4005;


        public final static int Fail = 10000;
    }

    public final static class CMD {
        public final static int CMD_UNKNOW = 0;
        public final static int CMD_LOGIN = 1;
        public final static int CMD_HEARTBEAT = 2;
        public final static int CMD_LOGOUT = 3;
        public final static int CMD_ENTER_ROOM = 4;
        public final static int CMD_LEAVE_ROOM = 5;
        public final static int CMD_SND_TEXT_MSG = 6;
        public final static int CMD_SND_VOICE_MSG = 7;
        public final static int CMD_SND_FILE_MSG = 8;
        public final static int CMD_GET_MSG = 9;
        public final static int CMD_GET_UPLOAD_TOKEN = 10;
        public final static int CMD_KICK_OFF = 11;
        public final static int CMD_SND_BIN_MSG = 12;
        public final static int CMD_RELOGIN = 13;
        public final static int CMD_CHECK_ONLINE = 14;
        public final static int CMD_SND_GIFT_MSG = 15;
        public final static int CMD_GET_ROOM_HISTORY_MSG = 16;
        public final static int CMD_GET_USR_INFO = 17;
        public final static int CMD_UPDATE_USER_INFO = 18;
        public final static int CMD_SND_TIPOFF_MSG = 19;
        public final static int CMD_GET_TIPOFF_MSG = 20;
        public final static int CMD_GET_DISTRICT = 21;
        public final static int CMD_GET_PEOPLE_NEARBY = 22;
        public final static int CMD_QUERY_NOTICE = 23;
        public final static int CMD_SET_MASK_USER_MSG = 24;        // 屏蔽用户
        public final static int CMD_GET_MASK_USER_MSG = 25;        // 获取屏蔽用户
        public final static int CMD_CLEAN_MASK_USER_MSG = 26;        // 解除所有屏蔽用户
        public final static int CMD_GET_ROOM_INFO = 27;            // 获取房间信息(人数)
        public final static int CMD_LEAVE_ALL_ROOM = 28;           // 绂诲紑鎵�鏈夋埧闂�

        public final static int CMD_GET_SESSION_HISTORY_MSG = 30;
        public final static int CMD_GET_FORBID_RECORD = 31;

        public final static int CMD_QUERY_USER_PROFILE = 37;           // 查询用户资料
        public final static int CMD_UPDATE_USER_PROFILE = 38;          // 更新用户资料
        public final static int CMD_UPDATE_ONLINE_STATUS = 39;         // 更新在线状态
        public final static int CMD_FIND_FRIEND_BY_ID = 40;            // 按ID查找好友
        public final static int CMD_FIND_FRIEND_BY_NICKNAME = 41;        // 按昵称查找好友
        public final static int CMD_REQUEST_ADD_FRIEND = 42;            // 请求添加好友
        public final static int CMD_FRIND_NOTIFY = 44;                    // 好友请求通知
        public final static int CMD_DELETE_FRIEND = 45;                // 删除好友
        public final static int CMD_BLACK_FRIEND = 46;                    // 拉黑好友
        public final static int CMD_UNBLACK_FRIEND = 47;                // 解除黑名单
        public final static int CMD_DEAL_ADD_FRIEND = 48;                // 好友验证
        public final static int CMD_QUERY_FRIEND_LIST = 49;            // 获取好友列表
        public final static int CMD_QUERY_BLACK_FRIEND_LIST = 50;        // 获取黑名单列表
        public final static int CMD_QUERY_FRIEND_REQUEST_LIST = 51;    // 获取好友验证消息列表
        public final static int CMD_UPDATE_FRIEND_RQUEST_STATUS = 52;    // 更新好友请求状态
        public final static int CMD_RELATION_CHAIN_HEARTBEAT = 53;        // 关系链心跳

        public final static int CMD_HXR_USER_INFO_CHANGE_NOTIFY = 74;    // 用户信息变更通知
        public final static int CMD_UPLOAD_PROGRESS = 75;    // 文件上传显示进度
        public final static int CMD_SET_READ_STATUS = 76; //对端已查看消息

        //服务器通知
        public final static int NOTIFY_LOGIN = 10001;
        public final static int NOTIFY_public_MSG = 10002;
        public final static int NOTIFY_ROOM_MSG = 10003;

        //客户端(C接口使用)
        public final static int CMD_DOWNLOAD = 20001;
        public final static int CMD_SEND_MESSAGE_STATUS = 20002;
        public final static int CMD_RECV_MESSAGE = 20003;
        public final static int CMD_STOP_AUDIOSPEECH = 20004;
        public final static int CMD_QUERY_HISTORY_MESSAGE = 20005;
        public final static int CMD_GET_RENCENT_CONTACTS = 20006;
        public final static int CMD_RECEIVE_MESSAGE_NITIFY = 20007;
        public final static int CMD_QUERY_USER_STATUS = 20008;
        public final static int CMD_AUDIO_PLAY_COMPLETE = 20009;
        public final static int CMD_STOP_SEND_AUDIO = 20010;

        public final static int CMD_TRANSLATE_COMPLETE = 20011;
        public final static int CMD_DOWNLOAD_URL = 20012;
        public final static int CMD_GET_MICROPHONE_STATUS = 20013;
        public final static int CMD_USER_ENTER_ROOM = 20014;
        public final static int CMD_USER_LEAVE_ROOM = 20015;
        public final static int CMD_RECV_NOTICE = 20016;
        public final static int CMD_CANCEL_NOTICE = 20017;

        public final static int CMD_GET_SPEECH_TEXT = 20018;        // 仅需要语音的文字识别内容
        public final static int CMD_GET_RECONNECT_RESULT = 20019;    // 重连结果
        public final static int CMD_START_RECONNECT = 20020;        // 开始重连
        public final static int CMD_RECORD_VOLUME = 20021;        // 音量
        public final static int CMD_GET_DISTANCE = 20022;            // 获取指定用户距离
        public final static int CMD_REQUEST_ADD_FRIEND_NOTIFY = 20023;    // 请求添加好友通知
        public final static int CMD_ADD_FRIENT_RESULT_NOTIFY = 20024;    // 添加好友结果通知
        public final static int CMD_BE_ADD_FRIENT = 20025;                // 被好友添加通知
        public final static int CMD_BE_DELETE_FRIEND_NOTIFY = 20026;    // 被删除好友通知
        public final static int CMD_BE_BLACK_FRIEND_NOTIFY = 20027;    // 被拉黑好友通知
        public final static int CMD_GET_USER_PROFILE = 20028;          //关系链-查询用户信息
        public final static int CMD_SET_USER_PROFILE = 20029;          //关系链-设置用户信息
        public final static int CMD_SET_USER_PHOTO = 20030;            //关系链-设置头像
        public final static int CMD_SWITCH_USER_STATE = 20031;         //关系链-切换用户在线状态
    }

    /**
     * YouMeIMChatType
     */
    public final static class ChatType{
        public final static int Unknow = 0;
        public final static int publicChat = 1;
        public final static int RoomChat = 2;
    }

    /**
     * YouMeIMFileType
     */
    public final static class FileType{
        public final static int FileType_Other = 0;
        public final static int FileType_Audio = 1;
        public final static int FileType_Image = 2;
        public final static int FileType_Video = 3;
    }

    /**
     * YouMeIMSampleRateType
     */
    public final static class SampleRateType{
        public final static int SAMPLERATE_8 = 0;      // 8000
        public final static int SAMPLERATE_16 = 1;	    // 16000
        public final static int SAMPLERATE_32 = 2;      // 32000
        public final static int SAMPLERATE_44_1 = 3;    // 44100
        public final static int SAMPLERATE_48 = 4;       // 48000
    };

    public enum UserStatus {
        ON_LINE(0),OFF_LINE(1);
        public int index;

        UserStatus(int idx) {
            this.index = idx;
        }

        public int getIndex() {
            return index;
        }
    }
    /**
     * ServerZone
     */
    public final static class ServerZone{
        public final static int China = 0; // 中国
        public final static int Singapore = 1; // 新加坡
        public final static int America = 2; // 美国
        public final static int HongKong = 3; // 香港
        public final static int Korea = 4; // 韩国
        public final static int Australia = 5; // 澳洲
        public final static int Deutschland = 6; // 德国
        public final static int Brazil = 7; // 巴西
        public final static int India = 8; // 印度
        public final static int Japan = 9; // 日本
        public final static int Ireland = 10; // 爱尔兰
    }

    public final static class MessageBodyType {
        public final static int Unknow = 0;
        public final static int TXT = 1;
        public final static int CustomMesssage = 2;
        public final static int Emoji = 3;
        public final static int Image = 4;
        public final static int Voice = 5;
        public final static int Video = 6;
        public final static int File = 7;
        public final static int Gift = 8;
    }

    public final static class ForbidSpeakReason{
        public final static int Unkown = 0;     //未知
        public final static int AD = 1;         //发广告
        public final static int Insult = 2;     //侮辱
        public final static int Politics = 3;   //政治敏感
        public final static int Terrorism = 4;  //恐怖主义
        public final static int Reaction = 5;   //反动
        public final static int Sexy = 6;       //色情
        public final static int Other = 7;    //其他
    }


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

    public final static class DistrictLevel{
        public final static int DISTRICT_UNKNOW = 0;
        public final static int DISTRICT_COUNTRY = 1;	// 国家
        public final static int DISTRICT_PROVINCE=2;	// 省份
        public final static int DISTRICT_CITY=3;		// 市
        public final static int DISTRICT_COUNTY=4;	// 区县
        public final static int DISTRICT_STREET	=5;	// 街道
    }

    // 举报处理结果
    public final static class AccusationDealResult{
        public final static int ACCUSATIONRESULT_IGNORE = 0;			// 忽略
        public final static int ACCUSATIONRESULT_WARNING = 1;			// 警告
        public final static int ACCUSATIONRESULT_FROBIDDEN_SPEAK = 2;	// 禁言
    }

    // 语音识别语言
    public final static class SpeechRecognizeLanguage{
        public final static int SPEECHLANG_MANDARIN = 0;	// 普通话(Android IOS Windows)
        public final static int SPEECHLANG_YUEYU = 1;		// 粤语(Android IOS Windows)
        public final static int SPEECHLANG_SICHUAN = 2;		// 四川话(Android IOS)
        public final static int SPEECHLANG_HENAN = 3;		// 河南话(IOS)
        public final static int SPEECHLANG_ENGLISH = 4;		// 英语(Android IOS Windows)
        public final static int SPEECHLANG_TRADITIONAL = 5;	// 繁体中文(Android IOS Windows)
    }

    public final static class  AudioDeviceStatus{
        public final static int STATUS_AVAILABLE = 0;		// 可用
        public final static int STATUS_NO_AUTHORITY = 1;	// 无权限
        public final static int STATUS_MUTE = 2;			// 静音
        public final static int STATUS_UNAVAILABLE = 3;		// 不可用
    }

    //重连结果
    public final static class ReconnectDealResult
    {
        public final static int RECONNECTRESULT_SUCCESS = 0;        // 重连成功
        public final static int RECONNECTRESULT_FAIL_AGAIN = 1;     // 重连失败，再次重连
        public final static int RECONNECTRESULT_FAIL = 2;           // 重连失败
    }

    //用户状态
    public final static class UserOnlineState
    {
        public final static int USER_STATUS_ONLINE = 0;     //在线，默认值（已登录）
        public final static int USER_STATUS_OFFLINE = 1;    //离线
        public final static int USER_STATUS_INVISIBLE = 2;  //隐身
    }

    //用户性别
    public final static class UserSex
    {
        public final static int SEX_UNKNOWN = 0; //未知性别
        public final static int SEX_MALE = 1;    //男性
        public final static int SEX_FEMALE = 2;  //女性
    }

    //被添加权限
    public final static class UserBeAddPermission
    {
        public final static int NOT_ALLOW_ADD = 0;      //不允许被添加
        public final static int NEED_VALIDATE = 1;      //需要验证
        public final static int NO_ADD_PERMISSION = 2;  //允许被添加，不需要验证, 默认值
    }

    //权限显示
    public final static class UserFoundPermission
    {
        public final static int CAN_BE_FOUND = 0;      //能被其它用户查找到，默认值
        public final static int CAN_NOT_BE_FOUND = 1;  //不能被其它用户查找到
    }
}
