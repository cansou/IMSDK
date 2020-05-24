package com.youme.imsdk;


import android.content.Context;
import android.os.Handler;
import android.text.TextUtils;
import android.util.Log;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.youme.im.IMEngine;
import com.youme.imsdk.YIMService.Errorcode;
import com.youme.imsdk.internal.AccusationResult;
import com.youme.imsdk.internal.BlockUserInfo;
import com.youme.imsdk.internal.BlockUserList;
import com.youme.imsdk.internal.ChatRoom;
import com.youme.imsdk.internal.Contacts;
import com.youme.imsdk.internal.Download;
import com.youme.imsdk.internal.DownloadUrl;
import com.youme.imsdk.internal.GeographyLocation;
import com.youme.imsdk.internal.Login;
import com.youme.imsdk.internal.MicrophoneStatus;
import com.youme.imsdk.internal.NewMessageNotifyObj;
import com.youme.imsdk.internal.NoticeCancelInfo;
import com.youme.imsdk.internal.NoticeInfo;
import com.youme.imsdk.internal.OnPlayCompleteNotify;
import com.youme.imsdk.internal.PhotoUrlInfo;
import com.youme.imsdk.internal.RecvMessage;
import com.youme.imsdk.internal.RelativeLocation;
import com.youme.imsdk.internal.RelativeLocationInfo;
import com.youme.imsdk.internal.RoomHistoryMessage;
import com.youme.imsdk.internal.RoomInfo;
import com.youme.imsdk.internal.SendMessage;
import com.youme.imsdk.internal.SendVoiceMsgInfo;
import com.youme.imsdk.internal.SpeechMessageInfo;
import com.youme.imsdk.internal.TranlateTextInfo;
import com.youme.imsdk.internal.UserChatRoom;
import com.youme.imsdk.internal.UserDistanceInfo;
import com.youme.imsdk.internal.UserInfoString;
import com.youme.imsdk.internal.UserProfileInfo;
import com.youme.imsdk.internal.UserStatusInfo;
import com.youme.imsdk.internal.VoiceInfo;
import com.youme.imsdk.internal.YIMBlackFriendInfo;
import com.youme.imsdk.internal.YIMFriendListInfo;
import com.youme.imsdk.internal.YouMeIMJsonResponse;
import com.youme.voice.VoiceManager;
import com.youme.imsdk.internal.VolumeInfo;
import com.youme.imsdk.internal.YIMFriendCommon;
import com.youme.imsdk.internal.YIMFriendDealResult;
import com.youme.imsdk.internal.YIMFriendRequestInfo;
import com.youme.imsdk.internal.YIMFriendRequestInfoList;
import com.youme.imsdk.internal.YIMFriendUserID;
import com.youme.imsdk.internal.YIMUserBriefInfo;
import com.youme.imsdk.internal.YIMUserBriefInfoList;
import com.youme.imsdk.internal.RecognizeSpeechText;
import com.youme.imsdk.internal.ReconnectResult;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class YIMService {
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
        public final static int MessageTooLong = 17;   //娑堟伅澶暱
        public final static int ReceiverTooLong = 18;  //鎺ユ敹鏂笽D杩囬暱(妫�鏌ユ埧闂村悕)
        public final static int InvalidChatType = 19;  //鏃犳晥鑱婂ぉ绫诲瀷(绉佽亰銆佽亰澶╁)
        public final static int InvalidReceiver = 20;  //鏃犳晥鐢ㄦ埛ID锛堢鑱婃帴鍙楄�呬负锛�
        public final static int UnknowError = 21;
        public final static int InvalidAppkey = 22;
        public final static int ForbiddenSpeak = 23;
        public final static int CreateFileFailed = 24;

        public final static int UnsupportFormat = 25;   //涓嶆敮鎸佺殑鏂囦欢鏍煎紡
        public final static int ReceiverEmpty   = 26;   //鎺ユ敹鏂逛负绌�
        public final static int RoomIDTooLong = 27;     //鎴块棿鍚嶅お闀�
        public final static int ContentInvalid = 28;    //鑱婂ぉ鍐呭涓ラ噸闈炴硶
        public final static int NoLocationAuthrize = 29;//鏈墦寮�瀹氫綅鏉冮檺
        public final static int UnknowLocation = 30;    //鏈煡浣嶇疆
        public final static int Unsupport  = 31;        //涓嶆敮鎸佽鎺ュ彛
        public final static int NoAudioDevice = 32;     //鏃犻煶棰戣澶�
        public final static int AudioDriver = 33;       //闊抽椹卞姩闂
        public final static int DeviceStatusInvalid = 34;//璁惧鐘舵�侀敊璇�
        public final static int ResolveFileError = 35;  //鏂囦欢瑙ｆ瀽閿欒
        public final static int ReadWriteFileError = 36;//鏂囦欢璇诲啓閿欒
        public final static int NoLangCode = 37;        //璇█缂栫爜閿欒
        public final static int TranslateUnable = 38;   //缈昏瘧鎺ュ彛涓嶅彲鐢�
        public final static int SpeechAccentInvalid = 39;//璇煶璇嗗埆鏂硅█鏃犳晥
        public final static int SpeechLanguageInvalid = 40;//璇煶璇嗗埆璇█鏃犳晥
        public final static int HasIllegalText = 41;			//娑堟伅鍚潪娉曞瓧绗�
        public final static int	AdvertisementMessage = 42;		//娑堟伅娑夊珜骞垮憡
        public final static int AlreadyBlock = 43;				//鐢ㄦ埛宸茬粡琚睆钄�
        public final static int NotBlock = 44;					//鐢ㄦ埛鏈灞忚斀
        public final static int MessageBlocked = 45;			//娑堟伅琚睆钄�
        public final static int LocationTimeout = 46;			//瀹氫綅瓒呮椂        	    
        public final static int NotJoinRoom = 47;				//未加入该房间
        public final static int LoginTokenInvalid = 48;		    //登录token错误
        public final static int CreateDirectoryFailed = 49;	    //创建目录失败
        
        public final static int InitFailed = 50;	//初始化失败
        public final static int	Disconnect = 51;   //与服务器断开
        	    
        public final static int	TheSameParam = 52;              //设置参数相同
        public final static int QueryUserInfoFail = 53;         //查询用户信息失败
        public final static int	SetUserInfoFail = 54;           //设置用户信息失败
        public final static int	UpdateUserOnlineStateFail = 55; //更新用户在线状态失败
        public final static int	NickNameTooLong = 56;           //昵称太长(> 64 bytes)
        public final static int	SignatureTooLong = 57;          //个性签名太长(> 120 bytes)   
        public final static int NeedFriendVerify = 58;			//需要好友验证信息
        public final static int	BeRefuse = 59;					//添加好友被拒绝
        public final static int	HasNotRegisterUserInfo = 60;    //未注册用户信息
        public final static int AlreadyFriend = 61;			    //已经是好友
        public final static int NotFriend = 62;				    //非好友
        public final static int NotBlack = 63;					//不在黑名单中
        public final static int PhotoUrlTooLong = 64;           //头像url过长(>500 bytes)
        public final static int PhotoSizeTooLarge = 65;         //头像太大（>100 kb）
                        		
        //鏈嶅姟鍣ㄧ殑閿欒鐮�
        public final static int ALREADYFRIENDS = 1000;
        public final static int LoginInvalid = 1001;

        //璇煶閮ㄥ垎閿欒鐮�
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
        public final static int PTT_SpeechTimeout = 2012;			//闊抽杈撳叆瓒呮椂
        public final static int PTT_IsPlaying = 2013;				//姝ｅ湪鎾斁
        public final static int PTT_NotStartPlay = 2014;			//鏈紑濮嬫挱鏀�
        public final static int PTT_CancelPlay = 2015;				//涓诲姩鍙栨秷鎾斁
        public final static int PTT_NotStartRecord = 2016;			//鏈紑濮嬭闊�
        
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
         * Java瀹夊叏楠岃瘉閿欒鐮�
         */
        public final static int InvalidUserId = 3000;
        public final static int InvalidRoomId = 3001;
        public final static int InvalidPassword = 3002;
        public final static int InvalidStoragaePath = 3003;
        public final static int InvalidAppKey = 3004;
        public final static int InvalidContext = 3005;
        public final static int InvaliddSecretKey = 3006;
        public final static int Fail = 10000;
    }


    /**
     * YouMeIMChatType
     */
    public final static class ChatType{
        public final static int Unknow = 0;
        public final static int PrivateChat = 1;
        public final static int RoomChat = 2;
    }

    /**
     * YouMeIMFileType
     */
    public final static class FileType{
        public final static int FileType_Other = 0;
        public final static int FileType_Audio = 1;
        public final static int FileType_Image = 2;
        public final static int FileType_Video = 3; //modify  src 2
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
        private int index;

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
        public final static int China = 0; // 涓浗
        public final static int Singapore = 1; // 鏂板姞鍧�
        public final static int America = 2; // 缇庡浗
        public final static int HongKong = 3; // 棣欐腐
        public final static int Korea = 4; // 闊╁浗
        public final static int Australia = 5; // 婢虫床
        public final static int Deutschland = 6; // 寰峰浗
        public final static int Brazil = 7; // 宸磋タ
        public final static int India = 8; // 鍗板害
        public final static int Japan = 9; // 鏃ユ湰
        public final static int Ireland = 10; // 鐖卞皵鍏�
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
        public final static int Unkown = 0;     //鏈煡
        public final static int AD = 1;         //鍙戝箍鍛�
        public final static int Insult = 2;     //渚颈
        public final static int Politics = 3;   //鏀挎不鏁忔劅
        public final static int Terrorism = 4;  //鎭愭�栦富涔�
        public final static int Reaction = 5;   //鍙嶅姩
        public final static int Sexy = 6;       //鑹叉儏
        public final static int Other = 7;    //鍏朵粬
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
        public final static int DISTRICT_COUNTRY = 1;	// 鍥藉
        public final static int DISTRICT_PROVINCE=2;	// 鐪佷唤
        public final static int DISTRICT_CITY=3;		// 甯�
        public final static int DISTRICT_COUNTY=4;	// 鍖哄幙
        public final static int DISTRICT_STREET	=5;	// 琛楅亾
    }
    
    // 涓炬姤澶勭悊缁撴灉
    public final static class AccusationDealResult{
    	public final static int ACCUSATIONRESULT_IGNORE = 0;			// 蹇界暐
    	public final static int ACCUSATIONRESULT_WARNING = 1;			// 璀﹀憡
    	public final static int ACCUSATIONRESULT_FROBIDDEN_SPEAK = 2;	// 绂佽█
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
    	public final static int STATUS_AVAILABLE = 0;		// 鍙敤
    	public final static int STATUS_NO_AUTHORITY = 1;	// 鏃犳潈闄�
    	public final static int STATUS_MUTE = 2;			// 闈欓煶
    	public final static int STATUS_UNAVAILABLE = 3;		// 涓嶅彲鐢�
    }

    //閲嶈繛缁撴灉
    public final static class ReconnectDealResult
    {
        public final static int RECONNECTRESULT_SUCCESS = 0;        // 閲嶈繛鎴愬姛
        public final static int RECONNECTRESULT_FAIL_AGAIN = 1;     // 閲嶈繛澶辫触锛屽啀娆￠噸杩�
        public final static int RECONNECTRESULT_FAIL = 2;           // 閲嶈繛澶辫触
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

    /* private final static int COMMAND_UNKNOW = 0; */
    private final static int CMD_UNKNOW = 0;
    private final static int CMD_LOGIN = 1;
    private final static int CMD_HEARTBEAT = 2;
    private final static int CMD_LOGOUT = 3;
    private final static int CMD_ENTER_ROOM = 4;
    private final static int CMD_LEAVE_ROOM = 5;
    private final static int CMD_SND_TEXT_MSG = 6;
    private final static int CMD_SND_VOICE_MSG = 7;
    private final static int CMD_SND_FILE_MSG = 8;
    private final static int CMD_GET_MSG = 9;
    private final static int CMD_GET_UPLOAD_TOKEN = 10;
    private final static int CMD_KICK_OFF = 11;
    private final static int CMD_SND_BIN_MSG = 12;
    private final static int CMD_RELOGIN = 13;
    private final static int CMD_CHECK_ONLINE = 14;
    private final static int CMD_SND_GIFT_MSG = 15;
    private final static int CMD_GET_ROOM_HISTORY_MSG = 16;
    private final static int CMD_GET_USR_INFO = 17;
    private final static int CMD_UPDATE_USER_INFO = 18;
    private final static int CMD_SND_TIPOFF_MSG = 19;
    private final static int CMD_GET_TIPOFF_MSG = 20;
    private final static int CMD_GET_DISTRICT = 21;
    private final static int CMD_GET_PEOPLE_NEARBY = 22;
    private final static int CMD_QUERY_NOTICE = 23;
    private final static int CMD_SET_MASK_USER_MSG = 24;		// 灞忚斀鐢ㄦ埛
	private final static int CMD_GET_MASK_USER_MSG = 25;		// 鑾峰彇灞忚斀鐢ㄦ埛
	private final static int CMD_CLEAN_MASK_USER_MSG = 26;		// 瑙ｉ櫎鎵�鏈夊睆钄界敤鎴�
	private final static int CMD_GET_ROOM_INFO = 27;			// 鑾峰彇鎴块棿淇℃伅(浜烘暟)
	private final static int CMD_LEAVE_ALL_ROOM = 28;           // 绂诲紑鎵�鏈夋埧闂�
    private final static int CMD_GET_FORBID_RECORD = 31;   
    
    private final static int CMD_QUERY_USER_PROFILE = 37;           // 查询用户资料
    private final static int CMD_UPDATE_USER_PROFILE = 38;          // 更新用户资料
    private final static int CMD_UPDATE_ONLINE_STATUS = 39;         // 更新在线状态
    private final static int CMD_FIND_FRIEND_BY_ID = 40;		    // 按ID查找好友
    private final static int CMD_FIND_FRIEND_BY_NICKNAME = 41; 	    // 按昵称查找好友
    private final static int CMD_REQUEST_ADD_FRIEND = 42; 		    // 请求添加好友
    private final static int CMD_FRIND_NOTIFY = 44;				    // 好友请求通知
    private final static int CMD_DELETE_FRIEND = 45;			    // 删除好友
    private final static int CMD_BLACK_FRIEND = 46;				    // 拉黑好友
    private final static int CMD_UNBLACK_FRIEND = 47;			    // 解除黑名单
    private final static int CMD_DEAL_ADD_FRIEND = 48;			    // 好友验证
    private final static int CMD_QUERY_FRIEND_LIST = 49;		    // 获取好友列表
    private final static int CMD_QUERY_BLACK_FRIEND_LIST = 50;	    // 获取黑名单列表
    private final static int CMD_QUERY_FRIEND_REQUEST_LIST = 51;    // 获取好友验证消息列表
    private final static int CMD_UPDATE_FRIEND_RQUEST_STATUS = 52;	// 更新好友请求状态
    private final static int CMD_RELATION_CHAIN_HEARTBEAT = 53;		// 关系链心跳

    private final static int CMD_HXR_USER_INFO_CHANGE_NOTIFY = 74;	// 用户信息变更通知
    
    //鏈嶅姟鍣ㄩ�氱煡
    private final static int NOTIFY_LOGIN = 10001;
    private final static int NOTIFY_PRIVATE_MSG = 10002;
    private final static int NOTIFY_ROOM_MSG = 10003;
    private final static int NOTIFY_ROOM_GENERAL = 10004;
    private final static int NOTIFY_UPDATE_CONFIG = 10008;

    private final static int NOTIFY_PRIVATE_MSG_V2 = 10012;
    private final static int NOTIFY_ROOM_MSG_V2 = 10013;

    private final static int NOTIFY_MAX_ID = 19999;	// 服务端通知最大ID
 
    //瀹㈡埛绔�(C鎺ュ彛浣跨敤)
    private final static int CMD_DOWNLOAD = 20001;
    private final static int CMD_SEND_MESSAGE_STATUS = 20002;
    private final static int CMD_RECV_MESSAGE = 20003;
    private final static int CMD_STOP_AUDIOSPEECH = 20004;
    private final static int CMD_QUERY_HISTORY_MESSAGE = 20005;
    private final static int CMD_GET_RENCENT_CONTACTS = 20006;
    private final static int CMD_RECEIVE_MESSAGE_NITIFY = 20007;
    private final static int CMD_QUERY_USER_STATUS = 20008;
    private final static int CMD_AUDIO_PLAY_COMPLETE= 20009;
    private final static int CMD_STOP_SEND_AUDIO = 20010;

    private final static int CMD_TRANSLATE_COMPLETE = 20011;
    private final static int CMD_DOWNLOAD_URL = 20012;
    private final static int CMD_GET_MICROPHONE_STATUS = 20013;
    private final static int CMD_USER_ENTER_ROOM = 20014;
    private final static int CMD_USER_LEAVE_ROOM = 20015;
    private final static int CMD_RECV_NOTICE = 20016;
    private final static int CMD_CANCEL_NOTICE = 20017;

    private final static int CMD_GET_SPEECH_TEXT = 20018;  // 浠呴渶瑕佽闊崇殑鏂囧瓧璇嗗埆鍐呭
    private final static int CMD_GET_RECONNECT_RESULT = 20019; //閲嶈繛缁撴灉
    private final static int CMD_START_RECONNECT = 20020;  // 寮�濮嬮噸杩�
    private final static int CMD_RECORD_VOLUME = 20021;   //闊抽噺
    private final static int CMD_GET_DISTANCE = 20022;			    // 获取双方地理位置距离
    private final static int CMD_REQUEST_ADD_FRIEND_NOTIFY = 20023;	// 请求添加好友通知
    private final static int CMD_ADD_FRIENT_RESULT_NOTIFY = 20024;	// 添加好友结果通知
    private final static int CMD_BE_ADD_FRIENT = 20025;				// 被好友添加通知
    private final static int CMD_BE_DELETE_FRIEND_NOTIFY = 20026;	// 被删除好友通知
    private final static int CMD_BE_BLACK_FRIEND_NOTIFY = 20027;	// 被拉黑好友通知
    private final static int CMD_GET_USER_PROFILE = 20028;          //关系链-查询用户信息
    private final static int CMD_SET_USER_PROFILE = 20029;          //关系链-设置用户信息
    private final static int CMD_SET_USER_PHOTO = 20030;            //关系链-设置头像
    private final static int CMD_SWITCH_USER_STATE = 20031;         //关系链-切换用户在线状态  

    public interface ChatRoomListener {
        /**
         * 鍔犲叆缇ょ粍
         *
         * @param errorcode 閿欒鐮�
         * @param roomID    棰戦亾id
         */
        public void onJoinChatRoom(Integer errorcode, String roomID);


        /**
         * 閫�鍑虹兢缁�
         *
         * @param errorcode 閿欒鐮�
         * @param roomId    棰戦亾id
         */
        public void onLeaveChatRoom(Integer errorcode, String roomId);
        
        /**
         * 閫�鍑烘墍鏈夌兢缁�
         *
         * @param errorcode 閿欒鐮�         
         */
        public void OnLeaveAllChatRooms(Integer errorcode);
        
        /**
         * 鐢ㄦ埛鍔犲叆棰戦亾
         * @param roomID	 棰戦亾id
         * @param userID	 鐢ㄦ埛id
         */
        public void onUserJoinChatRoom(String roomID, String userID);
        
        /**
         * 鐢ㄦ埛閫�鍑洪閬�
         * @param roomID	 棰戦亾id
         * @param userID	 鐢ㄦ埛id
         */
        public void onUserLeaveChatRoom(String roomID, String userID);
        
        /*
    	* 鍔熻兘锛氳幏鍙栨埧闂存垚鍛樻暟閲忓洖璋�
    	* @param errorcode锛氶敊璇爜
    	* @param chatRoomID锛氭埧闂碔D
    	* @param count锛氭垚鍛樻暟閲�
    	*/
    	public void OnGetRoomMemberCount(Integer errorcode, String chatRoomID, Integer count);
    }

    public interface ContactListener{
        /**
         * 鏍规嵁绉佽亰鍘嗗彶璁板綍鐢熸垚鐨勬渶杩戣仈绯讳汉鍒楄〃锛屾寜鏃堕棿鍊掑簭鎺掑垪
         * @param contactLists
         */
        public void onGetContact(ArrayList<ContactsSessionInfo> contactLists);

        /**
         * 鑾峰彇鐜╁鎵╁睍淇℃伅
         * @param extraUserInfo
         */
        public void onGetUserInfo(Integer errorcode, YIMExtraUserInfo extraUserInfo);

        /**
         * 鏌ヨ鐜╁鏄惁鍦ㄧ嚎鐨勭粨鏋滈�氱煡
         * @param errorcode
         * @param status
         */
        public void onQueryUserStatus(Integer errorcode, UserStatusInfo status);

    }

    public interface AudioPlayListener{
        public void onPlayCompletion(Integer errorcode, String audioPath);
        
        /**
         * 鑾峰彇楹﹀厠椋庣姸鎬佸洖璋�
         * @param status	鐘舵��(AudioDeviceStatus)
         */
        public void onGetMicrophoneStatus(Integer status);
    }

    public interface LoginListener {
        /**
         * 鐧诲綍鍥炶皟
         *
         * @param userId  鐢ㄦ埛Id
         * @param errcode 閿欒鐮�
         */
        public void onLogin(String userId, Integer errcode);

        /**
         * 鐧诲嚭鍥炶皟
         */
        public void onLogout();
        
        public void onKickOff();
    }

    public interface LocationListen
    {
        /**
         * 鑾峰彇鑷繁浣嶇疆鍥炶皟
         * @param errorcode
         */

         public void OnUpdateLocation(Integer errorcode, GeographyLocation location);
        /**
         * 鑾峰彇闄勮繎鐩爣鍥炶皟
          */

        public void OnGetNearbyObjects(Integer errorcode, ArrayList<RelativeLocation> neighbourList, Integer startDistance, Integer endDistance);
        
        /**
    	* 功能：获取与指定用户距离回调
    	* @param errorcode：错误码
    	* @param userID 用户ID
    	* @param distance 距离（米）
    	*/
    	public void OnGetDistance(Integer errorcode, String userID, Integer distance);
    }

    public interface MessageListener {
        /**
         * 鍙戦�佹秷鎭姸鎬�
         *
         * @param requestId 娑堟伅Id
         * @param errcode   閿欒鐮�
         * @param sendTime	鍙戦�佹椂闂存埑
         * @isForbidRoom  鏄惁鎴块棿琚瑷�
         * @reasonType  绂佽█鐨勫師鍥�
         * @forbidEndTime  绂佽█鎴鏃堕棿
         */
        public void onSendMessageStatus(Long requestId, Integer errcode, Integer sendTime, Boolean isForbidRoom, Integer reasonType, Long forbidEndTime, Long messageID);

        /**
         * 鎺ユ敹鍒扮敤鎴峰彂鏉ョ殑娑堟伅
         *
         * @param message 娑堟伅鍐呭缁撴瀯浣�
         */
        public void onRecvMessage(YIMMessage message);

        /**
         * 鍙戦�佽闊虫秷鎭紝涓嶄細杞枃鏈�
         *
         * @param requestID    娑堟伅Id
         * @param errorcode    閿欒鐮�
         * @param strText      鏂囨湰淇℃伅
         * @param strAudioPath 璇煶鏂囦欢瀛樻斁璺緞
         * @param audioTime 璇煶鏃堕暱
         * @param sendTime	鍙戦�佹椂闂存埑
         * @isForbidRoom  鏄惁鎴块棿琚瑷�
         * @reasonType  绂佽█鐨勫師鍥�
         * @forbidEndTime  绂佽█鎴鏃堕棿
         */
        public void onSendAudioMessageStatus(Long requestID, Integer errorcode, String strText, String strAudioPath, Integer audioTime, Integer sendTime,
                                             Boolean isForbidRoom, Integer reasonType, Long forbidEndTime, Long messageID);

        /**
         * 寮�濮嬪彂閫佽闊虫秷鎭殑閫氱煡锛岃繖涓椂鍊欏氨鍙互涓婂睆鎾斁浜�
         * @param requestID
         * @param errorcode
         * @param strText
         * @param strAudioPath
         * @param audioTime
         */
        public void onStartSendAudioMessage(Long requestID, Integer errorcode, String strText, String strAudioPath, Integer audioTime);
     
        /**
         * 涓嬭浇鏂囦欢鍥炶皟
         *
         * @param errorcode 閿欒鐮�
         * @param message   涓嬭浇璇煶鎴栨枃浠跺搴旂殑娑堟伅
         * @param savePath  鏂囦欢瀛樻斁璺緞
         */
        public void onDownload(Integer errorcode, YIMMessage message, String savePath);

        /**
         * 涓嬭浇鏂囦欢鍥炶皟
         *
         * @param errorcode 閿欒鐮�
         * @param fromUrl  鏂囦欢url
         * @param savePath  鏂囦欢瀛樻斁璺緞
         */
        public void onDownloadByUrl(Integer errorcode, String fromUrl, String savePath,Integer audioTime);

        /**
         * StartAudioSpeech 鎺ュ彛瀵瑰簲鐨勭粨鏋滃洖璋冮�氱煡
         * @param errorcode 閿欒鐮侊紝0 琛ㄧず姝ｅ父
         * @param iRequestID 娑堟伅id
         * @param strDownloadURL amr鏂囦欢涓嬭浇鍦板潃锛屾帴鏀舵柟涓嬭浇鍚庡彲浠ヨ皟鐢–onvertAMRToWav杞垚wav鎾斁
         * @param iDuration 褰曢煶鏃堕暱锛屽崟浣嶇
         * @param iFileSize 鏂囦欢澶у皬锛屽瓧鑺�
         * @param strLocalPath 鏈湴璇煶鏂囦欢璺緞
         * @param strText 璇煶璇嗗埆缁撴灉锛屽彲鑳戒负绌簄ull or ""
         */
        public void OnStopAudioSpeechStatus(Integer errorcode, Long iRequestID, String strDownloadURL, Integer iDuration, Integer iFileSize, String strLocalPath, String strText);

        /**
         *
         * @param errorcode
         * @param message
         */
        public void OnQueryHistoryMessage(Integer errorcode, YIMHistoryMessage message);
        
        /**
    	* 功能：从服务器查询房间历史消息回调（每次30条）
    	* @param errorcode：错误码
    	* @param roomID 房间ID
    	* @param remain 剩余消息数量
    	* @param messageList 消息列表
    	*/
        public void OnQueryRoomHistoryMessageFromServer(Integer errorcode, String roomID, Integer remain, ArrayList<YIMMessage> messageList);

        /**
         * SetReceiveMessageSwitch(false)鍚庯紝杩涘叆鎵嬪姩鎺ユ敹娑堟伅妯″紡锛屾湁鏂版秷鎭殑鏃跺�欎細閫氱煡璇ュ洖璋�
         */
        public void OnRecvNewMessage(Integer chatType, String targetID);

        /**
         * 缈昏瘧鍥炶皟
         * @param errorcode 閿欒鐮� 0琛ㄧず姝ｅ父
         * @param requestID 娑堟伅id
         * @param text  缈昏瘧缁撴灉
         * @param destLangCode 缈昏瘧鐩爣璇█
         *
         */
        public void OnTranslateTextComplete(Integer errorcode, Integer requestID, String text, Integer srcLangCode,Integer destLangCode);
        
        /**
         * 涓炬姤澶勭悊缁撴灉鍥炶皟
         * @param result 澶勭悊缁撴灉	(AccusationDealResult)
         * @param userID 琚妇鎶ョ敤鎴�
         * @param accusationTime 涓炬姤鏃堕棿
         */
        public void onAccusationResultNotify(Integer result, String userID, Integer accusationTime);

        /**
         * 绂佽█鐘舵�佹煡璇㈢粨鏋�
         * @param errorcode 閿欒鐮侊紝0琛ㄧず姝ｅ父
         * @param forbiddenInfoList 琚瑷�鐨勮缁嗕俊鎭�
         */
        public void OnGetForbiddenSpeakInfo(Integer errorcode, ArrayList<YIMForbiddenSpeakInfo> forbiddenInfoList);
        
        /**
         * 璇煶鏂囨湰鐨勮瘑鍒洖璋�        
         * @param errorcode
         * @param text         
         */        
        public void OnGetRecognizeSpeechText(Integer errorcode, Long requestID, String text);
        
        /*
    	* 鍔熻兘锛氬睆钄�/瑙ｉ櫎灞忚斀鐢ㄦ埛娑堟伅鍥炶皟
    	* @param errorcode锛氶敊璇爜
    	* @param userID
    	* @param block true-灞忚斀 false-瑙ｉ櫎灞忚斀
    	*/
        public void OnBlockUser(Integer errorcode, String userID, Boolean block);

    	/*
    	* 鍔熻兘锛氳В闄ゆ墍鏈夊凡灞忚斀鐢ㄦ埛鍥炶皟
    	* @param errorcode 閿欒鐮�
    	*/
        public void OnUnBlockAllUser(Integer errorcode);

    	/*
    	* 鍔熻兘锛氳幏鍙栬灞忚斀娑堟伅鐢ㄦ埛鍥炶皟
    	* @param errorcode锛氶敊璇爜
    	* @param userList userID闆�
    	*/
        public void OnGetBlockUsers(Integer errorcode, ArrayList<String> userList);
        
        /*
         * 鍔熻兘锛氬綍闊抽煶閲忓彉鍖栧洖璋�
         * @param volume锛氶煶閲忓��(0鍒�30)
         */
        public void OnRecordVolume(Float volume);
    }
    
    // 閫氱煡鍥炶皟
    public interface NoticeListener {
    	/**
         * 鎺ユ敹鍏憡鍥炶皟
         * @param notice 鍏憡淇℃伅
         */
    	public void onRecvNotice(NoticeInfo notice);
    	
    	/**
         * 鎾ら攢鍏憡锛堢疆椤跺叕鍛婏級
         * @param noticeID 鍏憡id
         * @param channelID 棰戦亾id
         */
    	public void onCancelNotice(Integer noticeID, String channelID);
    }

    
    // 閲嶈繛鍥炶皟
    public interface ReconnectListener {
        /**
         * 寮�濮嬮噸杩炲洖璋�
         */
        public void OnStartReconnect();
        /**
         * 鎺ユ敹閲嶈繛缁撴灉鍥炶皟
         * @param result 閲嶈繛缁撴灉(ReconnectDealResult)
         */
        public void OnRecvReconnectResult(Integer result);
    }
    
    //用户信息管理回调
    public interface UserProfileListener {
    	/**
         * 功能：查询用户信息回调
         * @param errorcode：错误码
         * @param userInfo：用户信息
         */
        public void OnQueryUserInfo(Integer errorcode, UserProfileInfo userInfo);
        
        /**
         * 功能：设置用户信息回调
         * @param errorcode：错误码
         */
        public void OnSetUserInfo(Integer errorcode);
        
        /**
         * 功能：切换用户在线状态回调
         * @param status: 在线状态
         * @param errorcode：错误码
         */
        public void OnSwitchUserOnlineState(Integer errorcode);
        
        /**
         * 功能：设置头像回调
         * @param errorcode：错误码
         * @param photoUrl：头像的下载URL
         */
        public void OnSetPhotoUrl(Integer errorcode, String photoUrl);
        
        /**
    	 * 功能：用户信息变更通知
    	 * @param users：用户ID
    	 */
    	public void OnUserInfoChangeNotify(String userID);
    }
    
    //好友接口回调
    public interface FriendListener {
       /**
    	* 功能：查找用户回调
    	* @param errorcode：错误码
    	* @param users：用户简要信息
    	*/
    	public void OnFindUser(Integer errorcode, ArrayList<YIMUserBriefInfo> userList);

    	/*
    	* 功能：请求添加好友回调
    	* @param errorcode：错误码
    	* @param userID：用户ID
    	*/
    	public void OnRequestAddFriend(Integer errorcode, String userID);

    	/*
    	* 功能：被邀请添加好友通知（需要验证）
    	* @param userID：用户ID
    	* @param comments：备注或验证信息
    	* commonts：显示用户信息可以根据userID查询
    	*/
    	public void OnBeRequestAddFriendNotify(String userID, String comments);

    	/*
    	* 功能：被添加为好友通知（不需要验证）
    	* @param userID：用户ID
    	* @param comments：备注或验证信息
    	*/
    	public void OnBeAddFriendNotify(String userID, String comments);
    	
    	/*
    	* 功能：处理被请求添加好友回调
    	* @param errorcode：错误码
    	* @param userID：用户ID
    	* @param comments：备注或验证信息
    	* @param dealResult：处理结果	0：同意	1：拒绝
    	*/
    	public void OnDealBeRequestAddFriend(Integer errorcode, String userID, String comments, Integer dealResult);

    	/*
    	* 功能：请求添加好友结果通知(需要好友验证，待被请求方处理后回调)
    	* @param userID：用户ID
    	* @param comments：备注或验证信息
    	* @param dealResult：处理结果	0：同意	1：拒绝
    	*/
    	public void OnRequestAddFriendResultNotify(String userID, String comments, Integer dealResult);

    	/*
    	* 功能：删除好友结果回调
    	* @param errorcode：错误码
    	* @param userID：用户ID
    	*/
    	public void OnDeleteFriend(Integer errorcode, String userID);

    	/*
    	* 功能：被好友删除通知
    	* @param userID：用户ID
    	*/
    	public void OnBeDeleteFriendNotify(String userID);

    	/*
    	* 功能：拉黑或解除拉黑好友回调
    	* @param errorcode：错误码
    	* @param type：0：拉黑	1：解除拉黑
    	* @param userID：用户ID
    	*/
    	public void OnBlackFriend(Integer errorcode, Integer type, String userID);    	

    	/*
    	* 功能：查询我的好友回调
    	* @param errorcode：错误码
    	* @param type：0：正常好友	1：被拉黑好友
    	* @param startIndex：起始序号
    	* @param hasMore：是否还有更多数据
    	* @param friends：好友列表
    	*/
    	public void OnQueryFriends(Integer errorcode, Integer type, Integer startIndex, ArrayList<YIMUserBriefInfo> friendList);

    	/*
    	* 功能：查询好友请求列表回调
    	* @param errorcode：错误码
    	* @param startIndex：起始序号
    	* @param hasMore：是否还有更多数据
    	* @param validateList：验证消息列表
    	*/
    	public void OnQueryFriendRequestList(Integer errorcode, Integer startIndex, ArrayList<YIMFriendRequestInfo> requestList);
    	
    }

    private final static String TAG = YIMService.class.getSimpleName();

    private boolean mIsExit = true;
    private static YIMService mInstance;
    private LoginListener mLoginListener = null;
    private MessageListener mMessageListener = null;
    private ChatRoomListener mChatRoomListener = null;
    private ContactListener mContactListener = null;
    private AudioPlayListener mAudioPlayListener = null;
    private LocationListen mLocationListen = null;
    private NoticeListener mNoticeListener = null;
    private ReconnectListener mReconnectListener = null;
    
    private UserProfileListener mUserProfileListener = null;
    private FriendListener mFriendListener = null;
    
    private Thread mThread = null;
    private Handler mHandler = null;

    private static boolean DEBUG = false;
    
    private Context m_context;

    private YIMService() {

    }

    public static void SetDebug(boolean isOn){
        YIMService.DEBUG = isOn;
    }

    public static YIMService getInstance() {
        if (null == mInstance) {
            mInstance = new YIMService();
        }
        return mInstance;
    }

    /**
     * 璁剧疆鐧诲綍鍥炶皟
     * @param l 鐧诲綍鍥炶皟鎺ュ彛
     */
    public void registerLoginListener(LoginListener l) {
        mLoginListener = l;
    }

    public void unregisterLoginListener() {
        mLoginListener = null;
    }

    /**
     * 璁剧疆娑堟伅鍥炶皟
     * @param l 娑堟伅鍥炶皟鎺ュ彛
     */
    public void registerMessageListener(MessageListener l) {
        mMessageListener = l;
    }

    public void unregisterMessageListener() {
        mMessageListener = null;
    }

    /**
     * 璁剧疆鑱婂ぉ瀹ゅ洖璋�
     * @param l 鑱婂ぉ瀹ゅ洖璋�
     */
    public void registerChatRoomListenr(ChatRoomListener l) {
        mChatRoomListener = l;
    }

    public void unregisterChatRoomListener() {
        mChatRoomListener = null;
    }

    /**
     * 璁剧疆鑾峰彇绉佽亰鑱旂郴浜哄巻鍙茬邯褰曞洖璋�
     * @param l 娑堟伅鍥炶皟鎺ュ彛
     */
    public void registerContactListener(ContactListener l) {
        mContactListener = l;
    }

    public void unregisterContactListener() {
        mContactListener = null;
    }

    /**
     * 璁剧疆鎾斁鍣ㄦ挱鏀剧粨鏉熶簨浠剁洃鍚�
     * @param l 娑堟伅鍥炶皟鎺ュ彛
     */
    public void registerAudioPlayListener(AudioPlayListener l) {
        mAudioPlayListener = l;
    }

    public void unregisterAudioPlayListener() {
        mAudioPlayListener = null;
    }

    /**
     * 璁剧疆lbs淇℃伅鍥炶皟
     * @param
     */
    public void registerLocationListen(LocationListen l){
        mLocationListen = l ;
    }

    public void unregisterLocationListen(){
        mLocationListen = null ;
    }
    
    // 璁剧疆鍏憡鍥炶皟
    public void registerNoticeListener(NoticeListener l){
        mNoticeListener = l ;
    }

    public void unregisterNoticeListener(){
    	mNoticeListener = null ;
    }
    

    // 璁剧疆閲嶈繛鍥炶皟
    public void registerReconnectListener(ReconnectListener l){
        mReconnectListener = l ;
    }
    
    public void unregisterReconnectListener(){
        mReconnectListener = null ;
    }    
    
    public void registerUserProfileListener(UserProfileListener l){
    	mUserProfileListener = l;
    }
    public void unregisterUserProfileListener(){
    	mUserProfileListener = null;
    }
    
    public void registerFriendListener(FriendListener l){
    	mFriendListener = l;
    }
    public void unregisterFriendListener(){
    	mFriendListener = null;
    }

    public void setAudioCachePath(String path) {
        VoiceManager.SetAudioRecordCacheDir(path);
    }

    public int init(Context context, String appKey, String secrectKey,int serverZone) {        
        if (null == context)
            return Errorcode.InvalidContext;
        if (!checkString("AppKey", appKey)){
            return Errorcode.InvalidAppKey;
        }
        if (!checkString("SecrectKey", secrectKey)) {
        	return Errorcode.InvaliddSecretKey;
        }
        int ret = IMEngine.IM_Init(context, appKey, secrectKey, serverZone);
        if (ret == -1){
        	return Errorcode.InitFailed;
        }
        return Errorcode.Success;
    }

    private static void initEngine(Context context) {
        IMEngine.init(context);

    }

    public void setMode(int mode ){
        IMEngine.IM_SetMode(mode);
    }
    
    public int setLoginAddress(String ip, int port){
    	return IMEngine.IM_SetLoginAddress(ip, port);
    }

    public int login(String userId, String password,String token) {
        int errcode = Errorcode.Fail;

        do {
            if (!checkString("UserId", userId)){
                errcode = Errorcode.InvalidUserId;
                break;
            }

            if (!checkString("Password", password)){
                errcode = Errorcode.InvalidPassword;
                break;
            }

            errcode = IMEngine.IM_Login(userId, password,token);
            if (DEBUG)
                Log.d(TAG, "errcode:" + errcode + ", mThread is null" + (null == mThread));
            if (Errorcode.Success == errcode) {
                mIsExit = false;
                if (null == mHandler) {
                    mHandler = new Handler();
                }
                if (null == mThread) {
                    mThread = new Thread(paresMessage);
                    mThread.start();
                }
            }
            break;
        }while(true);

        return errcode;
    }

    public int logout() {

        return IMEngine.IM_Logout();
    }

    /**
     * 鍔犲叆鑱婂ぉ瀹�
     * @param roomId 鑱婂ぉ瀹D
     * @return
     */
    public int joinChatRoom(String roomId) {
        if (!checkString("RoomID", roomId)){
            return Errorcode.InvalidRoomId;
        }
        return IMEngine.IM_JoinChatRoom(roomId);
    }

    /**
     * 閫�鍑鸿亰澶╁
     * @param roomId 鑱婂ぉ瀹D
     * @return
     */
    public int leaveChatRoom(String roomId) {
        if (!checkString("RoomID", roomId)){
            return Errorcode.InvalidRoomId;
        }
        return IMEngine.IM_LeaveChatRoom(roomId);
    }
    
    /**
     * 閫�鍑烘墍鏈夎亰澶╁     
     * @return
     */
    public int leaveAllChatRooms() {        
        return IMEngine.IM_LeaveAllChatRooms();
    }
    
    /*
     * 
	* 鍔熻兘锛氳幏鍙栨埧闂存垚鍛樻暟閲�
	* @param chatRoomID锛氭埧闂碔D(宸叉垚鍔熷姞鍏ヨ鎴块棿)
	* @return 閿欒鐮�
	*/
    public int getRoomMemberCount(String roomId) {
    	return IMEngine.IM_GetRoomMemberCount(roomId);
    }
    

    /**
     * 鍙戦�佹枃鏈秷鎭�
     * @param recvId 娑堟伅鎺ユ敹鑰匢D
     * @param chatType 鑱婂ぉ绫诲瀷锛岃瑙丆hatType
     * @param msgContent 娑堟伅鍐呭
     * @return
     */
    public MessageSendStatus sendTextMessage(String recvId, int chatType, String msgContent, String attachParam) {
        MessageSendStatus status = new MessageSendStatus();
        if (!checkString("ReceiverID", recvId)){
            status.setMessageId(0);
            status.setErrorCode(Errorcode.InvalidReceiver);
        }else{
            IMEngine.MessageRequestId messageRequestId = new IMEngine().new MessageRequestId();
            int errcode = IMEngine.IM_SendTextMessage(recvId, chatType, msgContent, attachParam, messageRequestId);
            status.setMessageId(messageRequestId.getId());
            status.setErrorCode(errcode);
        }
        return status;
    }

    /**
     * 寮�濮嬪綍闊�,甯﹁闊宠浆鏂囧瓧璇嗗埆
     * @param recvId 娑堟伅鎺ユ敹鑰匢D
     * @param chatType 鑱婂ぉ绫诲瀷锛岃瑙丆hatType
     * @return
     */
    public MessageSendStatus startRecordAudioMessage(String recvId, int chatType) {
        MessageSendStatus status = new MessageSendStatus();
        if (!checkString("ReceiverID", recvId)){
            status.setMessageId(0);
            status.setErrorCode(Errorcode.InvalidReceiver);
        }else{
            IMEngine.MessageRequestId messageRequestId = new IMEngine().new MessageRequestId();
            int errcode = IMEngine.IM_SendAudioMessage(recvId, chatType, messageRequestId);
            status.setMessageId(messageRequestId.getId());
            status.setErrorCode(errcode);
        }
        return status;
    }

    /**
     * 寮�濮嬪綍闊�,涓嶅甫璇煶杞枃瀛楀姛鑳�
     * @param recvId 娑堟伅鎺ユ敹鑰匢D
     * @param chatType 鑱婂ぉ绫诲瀷锛岃瑙丆hatType
     * @return
     */
    public MessageSendStatus startOnlyRecordAudioMessage(String recvId, int chatType) {
        MessageSendStatus status = new MessageSendStatus();
        if (!checkString("ReceiverID", recvId)){
            status.setMessageId(0);
            status.setErrorCode(Errorcode.InvalidReceiver);
        }else{
            IMEngine.MessageRequestId messageRequestId = new IMEngine().new MessageRequestId();
            int errcode = IMEngine.IM_SendOnlyAudioMessage(recvId, chatType, messageRequestId);
            status.setMessageId(messageRequestId.getId());
            status.setErrorCode(errcode);
        }
        return status;
    }

    /**
     * 鍋滄褰曢煶骞跺彂閫�
     * @param extraText 璇煶娑堟伅闄勫甫淇℃伅
     * @return
     */
    public int stopAndSendAudioMessage(String extraText) {
        return IMEngine.IM_StopAudioMessage(extraText);
    }

    /**
     * 鍙栨秷褰曢煶
     * @return
     */
    public int cancleAudioMessage() {
        return IMEngine.IM_CancleAudioMessage();
    }

    /**
     * 涓嬭浇璇煶鏂囦欢
     * @param serial 璇煶娑堟伅ID
     * @param savePath 鏈湴缂撳瓨璺緞锛屽繀椤讳繚璇佽璺緞鏈夊彲鍐欐潈闄�
     * @return
     */
    public int downloadAudioMessage(long serial, String savePath) {
        return IMEngine.IM_DownloadAudioFile(serial, savePath);
    }

    /**
     * 涓嬭浇鏂囦欢
     * @param serial 娑堟伅ID
     * @param savePath 鏈湴缂撳瓨璺緞锛屽繀椤讳繚璇佽璺緞鏈夊彲鍐欐潈闄�
     * @return
     */
    public int downloadFile(long serial, String savePath) {
        return IMEngine.IM_DownloadAudioFile(serial, savePath);
   }

    /**
     * 涓嬭浇鏂囦欢
     * @param fromUrl 涓嬭浇鍦板潃
     * @param savePath 鏈湴缂撳瓨璺緞锛屽繀椤讳繚璇佽璺緞鏈夊彲鍐欐潈闄�
     * @return
     */
    public int downloadFileByUrl( String fromUrl, String savePath, int fileType ){
    	return IMEngine.IM_DownloadFileByURL( fromUrl, savePath, fileType);
    }
    
    /**
     * 璁剧疆涓嬭浇鐩綍
     * @param path 涓嬭浇鐩綍璺緞
     * @return
     */
    public int setDownloadDir(String path){
        return IMEngine.IM_SetDownloadDir(path);
    }

    public MessageSendStatus sendCustomMessage(String recvId, int chatType, byte[] customMessage,int bufferLen) {
        MessageSendStatus status = new MessageSendStatus();
        if (!checkString("ReceiverID", recvId)){
            status.setMessageId(0);
            status.setErrorCode(Errorcode.InvalidReceiver);
        }else{
            IMEngine.MessageRequestId messageRequestId = new IMEngine().new MessageRequestId();
            int errcode = IMEngine.IM_SendCustomMessage(recvId, chatType, customMessage ,bufferLen, messageRequestId);
            status.setMessageId(messageRequestId.getId());
            status.setErrorCode(errcode);
        }
        return status;
    }

    public MessageSendStatus sendFile(String recvId, int chatType,String filePath,String extParam, int fileType) {
        MessageSendStatus status = new MessageSendStatus();
        if (!checkString("ReceiverID", recvId)){
            status.setMessageId(0);
            status.setErrorCode(Errorcode.InvalidReceiver);
        }else{
            IMEngine.MessageRequestId messageRequestId = new IMEngine().new MessageRequestId();
            int errcode = IMEngine.IM_SendFile(recvId, chatType, filePath ,extParam,fileType, messageRequestId);
            status.setMessageId(messageRequestId.getId());
            status.setErrorCode(errcode);
        }
        return status;
    }

    public String getFilterText(String strSource, IMEngine.IntegerVal level) {
        return IMEngine.IM_GetFilterText(strSource, level);
    }

    public void onPause(boolean pauseReceiveMessage) {
         IMEngine.IM_OnPause(pauseReceiveMessage);
    }

    public void onResume() {
         IMEngine.IM_OnResume();
    }

    public int getNewMessage(List<String> targets){
        JSONArray jsonArray = new JSONArray(targets);
        return IMEngine.IM_GetNewMessage(jsonArray.toString());
    }    
    public int setDownloadAudioMessageSwitch(boolean download){        
        return IMEngine.IM_SetDownloadAudioMessageSwitch(download);
    }

    public int setReceiveMessageSwitch(List<String> targets, boolean autoReceive){
        JSONArray jsonArray = new JSONArray(targets);
        return IMEngine.IM_SetReceiveMessageSwitch(jsonArray.toString(), autoReceive);
    }

    public int queryRoomHistoryMessageFromServer(String roomID, int count, int direction){
        return IMEngine.IM_QueryRoomHistoryMessageFromServer(roomID, count, direction);
    }

    public MessageSendStatus sendGift(String anchorId,String channel,int giftID,int giftCount,YIMExtraGifParam extParam){
        MessageSendStatus status = new MessageSendStatus();
        if (!checkString("Receive strChannel ", channel)){
            status.setMessageId(0);
            status.setErrorCode(Errorcode.InvalidReceiver);
        }else{
            IMEngine.MessageRequestId messageRequestId = new IMEngine().new MessageRequestId();
            GsonBuilder builder = new GsonBuilder();
            Gson gson = builder.create();
            if (DEBUG)
                Log.d(TAG, "gift extparam:" + gson.toJson(extParam));
            int errcode = IMEngine.IM_SendGift(anchorId,channel, giftID, giftCount ,gson.toJson(extParam), messageRequestId);
            status.setMessageId(messageRequestId.getId());
            status.setErrorCode(errcode);
        }
        return status;
    }

    public int multiSendTextMessage(List<String> recvLists,String strText){
        JSONArray jsonArray = new JSONArray(recvLists);
        return IMEngine.IM_MultiSendTextMessage(jsonArray.toString(),strText);
    }

    public int getHistoryContact(){
        return IMEngine.IM_GetHistoryContact();
    }

    public MessageSendStatus startAudioSpeech(boolean translateToText){
        MessageSendStatus status = new MessageSendStatus();
        IMEngine.MessageRequestId messageRequestId = new IMEngine().new MessageRequestId();
        int errcode = IMEngine.IM_StartAudioSpeech(messageRequestId,translateToText);
        status.setMessageId(messageRequestId.getId());
        status.setErrorCode(errcode);
        return status;
    }

    public int stopAudioSpeech(){
        return IMEngine.IM_StopAudioSpeech();
    }

    public int convertAMRToWav(String amrFilePath,String toWavFilePath){
        return IMEngine.IM_ConvertAMRToWav(amrFilePath,toWavFilePath);
    }

    public int queryHistoryMessage(String targetID, int chatType, long startMessageID, int count, int direction ) {
        return IMEngine.IM_QueryHistoryMessage(targetID,chatType,startMessageID,count,direction);
    }

    public int deleteHistoryMessageByID(long messageID){
        return IMEngine.IM_DeleteHistoryMessageByID(messageID);
    }

    public int deleteHistoryMessageBeforeTime(int chatType, long time){
        return IMEngine.IM_DeleteHistoryMessage(chatType, time);
    }
    
    /**
     * 删除指定用户的本地消息历史记录，保留指定的消息ID列表记录
     * @param targetID：指定的用户
     * @param chatType：聊天类型（指定私聊）
     * @param messageList：消息ID白名单
     */
    public int deleteSpecifiedHistoryMessage(String targetID, int chatType, long[] excludeMesList) {
        return IMEngine.IM_DeleteSpecifiedHistoryMessage(targetID, chatType, excludeMesList);
    }
    
    public int deleteHistoryMessageByTarget(String targetID, int chatType, long startMessageID, int count) {
    	return IMEngine.IM_DeleteHistoryMessageByTarget(targetID, chatType, startMessageID, count);
    }

    public int SetUserInfo(YIMExtraUserInfo userInfo){
        if(userInfo == null){
            return Errorcode.ParamInvalid;
        }
        GsonBuilder builder = new GsonBuilder();
        Gson gson = builder.create();
        return IMEngine.IM_SetUserInfo(gson.toJson(userInfo));
    }

    public int getUserInfo(String userID){
        return IMEngine.IM_GetUserInfo(userID);
    }

    public int SetRoomHistoryMessageSwitch(List<String> roomIDs,boolean isSave){
        JSONArray jsonArray = new JSONArray(roomIDs);
        return IMEngine.IM_SetRoomHistoryMessageSwitch(jsonArray.toString(),isSave);
    }

    public int StartPlayAudio(String audioPath){
        return IMEngine.IM_StartPlayAudio(audioPath);
    }

    public int StopPlayAudio(){
        return IMEngine.IM_StopPlayAudio();
    }

    public boolean IsPlaying(){
        return IMEngine.IM_IsPlaying();
    }

    public void SetVolume(float volume){
        if(volume<0.0f) volume=0;
        if(volume>1.0f) volume = 1.0f;
        IMEngine.IM_SetVolume(volume);
    }

    public String GetAudioCachePath(){
        return IMEngine.IM_GetAudioCachePath();
    }

    public boolean ClearAudioCachePath(){
        return IMEngine.IM_ClearAudioCachePath();
    }

    public int QueryUserStatus(String userID){
        return IMEngine.IM_QueryUserStatus(userID);
    }


    /**
     *  鏂囨湰缈昏瘧
     *  @pararm text 鍘熻瑷�瀛楃涓�
     *  @param destLangCode 鐩爣璇█
     *  @param srcLangCode 鍘熻瑷�
      */
    public TranslateMsgStatus translateText( String text, int destLangCode, int srcLangCode){
        TranslateMsgStatus status = new TranslateMsgStatus();
        IMEngine.IntegerVal val = new IMEngine().new IntegerVal();
        int errorCode = IMEngine.IM_TranslateText(val,text,destLangCode,srcLangCode) ;
        status.setErrorCode(errorCode);
        status.setMessageId(val.getValue());
        return status;
    }
    
    /*
	* 鍔熻兘锛氬睆钄�/瑙ｉ櫎灞忚斀鐢ㄦ埛娑堟伅
	* @param userID锛氱敤鎴稩D
	* @param block锛歵rue-灞忚斀 false-瑙ｉ櫎灞忚斀
	* @return 閿欒鐮�
	*/
	public int blockUser(String userID, boolean block){
		return IMEngine.IM_BlockUser(userID, block);
	}

	/*
	* 鍔熻兘锛氳В闄ゆ墍鏈夊凡灞忚斀鐢ㄦ埛
	* @return 閿欒鐮�
	*/
	public int unBlockAllUser(){
		return IMEngine.IM_UnBlockAllUser();
	}

	/*
	* 鍔熻兘锛氳幏鍙栬灞忚斀娑堟伅鐢ㄦ埛
	* @return 閿欒鐮�
	*/
	public int getBlockUsers(){
		return IMEngine.IM_GetBlockUsers();
	}
    

    /**
     * 鑾峰彇鍦扮悊瀹氫綅
     * @return
     */
    public int getCurrentLocation(){
        return IMEngine.IM_GetCurrentLocation();
    }

    /**
     * 鑾峰彇闄勮繎鐨勭洰鏍�	count:鑾峰彇鏁伴噺锛堜竴娆℃渶澶�200锛�
     * @param serverAreaID锛氬尯鏈�
     * @param districtlevel锛氳鏀垮尯鍒掔瓑绾�
     * @param resetStartDistance锛氭槸鍚﹂噸缃煡鎵捐捣濮嬭窛绂�
     */
    public int getNearbyObjects(int count, String serverAreaID, int districtlevel , boolean resetStartDistance ){
        return IMEngine.IM_GetNearbyObjects(count,serverAreaID,districtlevel,resetStartDistance) ;
    }
    
    /**
	* 功能：获取与指定用户距离
	* @param errorcode：错误码
	* @param userID 用户ID
	*/
    public int getDistance(String userID){
    	return IMEngine.IM_GetDistance(userID);
    }

    /**
     * 璁剧疆鍦扮悊瀹氫綅鏇存柊鏃堕棿锛堝垎閽燂級锛岄粯璁や笉鏇存柊
     * @param interval
     */
    public void setUpdateInterval(int interval) {
        IMEngine.IM_SetUpdateInterval(interval);
    }
    
    
    public void getMicrophoneStatus(){
    	IMEngine.IM_GetMicrophoneStatus();
    }
    
    /**
     * 璁剧疆璇煶璇嗗埆璇█
     * @param accent锛氳瑷�
     */
    public int setSpeechRecognizeLanguage(int language){
    	return IMEngine.IM_SetSpeechRecognizeLanguage(language);
    }
    
    /**
     * 璁剧疆浠呰瘑鍒闊虫枃瀛楋紝涓嶅彂閫佽闊虫秷鎭�; false:璇嗗埆璇煶鏂囧瓧骞跺彂閫佽闊虫秷鎭紝true:浠呰瘑鍒闊虫枃瀛�     
     */
    public int setOnlyRecognizeSpeechText(boolean recognition){
    	return IMEngine.IM_SetOnlyRecognizeSpeechText(recognition);
    }
    
    /**
     * 涓炬姤
     * @param userID锛氬尯鏈�
     * @param source锛氳鏀垮尯鍒掔瓑绾�
     * @param reason锛氬尯鏈�
     * @param description锛氳鏀垮尯鍒掔瓑绾�
     * @param extraParam锛氳鏀垮尯鍒掔瓑绾�
     */
    public int accusation(String userID, int source, int reason, String description, String extraParam){
    	return IMEngine.IM_Accusation(userID, source, reason, description, extraParam);
    }
    
    public int queryNotice(){
    	return IMEngine.IM_QueryNotice();
    }

    public int getForbiddenSpeakInfo()
    {
        return IMEngine.IM_GetForbiddenSpeakInfo();
    }
    
    public int setMessageRead(long messageID, boolean read)
    {
        return IMEngine.IM_SetMessageRead(messageID, read);
    }
    
    public int setAllMessageRead(String userID, boolean read)
    {
    	return IMEngine.IM_SetAllMessageRead(userID,read);
    }
    
    public int setVoiceMsgPlayed(long messageID, boolean played)
    {
    	return IMEngine.IM_SetVoiceMsgPlayed(messageID,played);
    }
    
    //用户信息接口
    public int setUserProfileInfo(YIMUserSettingInfo userInfo){
    	 if(userInfo == null){
             return Errorcode.ParamInvalid;
         } 
    	 JSONObject profileInfo = new JSONObject();
    	 try {
    		 profileInfo.put("NickName",userInfo.getNickName());
    		 
    		 profileInfo.put("Sex",String.valueOf(userInfo.getSex()));
    		 profileInfo.put("Signature",userInfo.getSignature());
    		 profileInfo.put("Country",userInfo.getCountry());
    		 profileInfo.put("Province",userInfo.getProvince());
    		 profileInfo.put("City",userInfo.getCity());
    		 profileInfo.put("ExtraInfo",userInfo.getExtraInfo());    		 
         } catch (JSONException e) {
             e.printStackTrace();
         }
    	 return IMEngine.IM_SetUserProfileInfo(profileInfo.toString());
    }
    
    public int setUserProfilePhoto(String photoPath)
    {    	
    	return IMEngine.IM_SetUserProfilePhoto(photoPath);
    }
    public int getUserProfileInfo(String userID)
    {
    	return IMEngine.IM_GetUserProfileInfo(userID);
    }
    public int switchUserStatus(String userID, int userStatus)
    {
    	return IMEngine.IM_SwitchUserStatus(userID, userStatus);
    }
    public int setAddPermission(boolean beFound, int beAddPermission)
    {
    	return IMEngine.IM_SetAddPermission(beFound, beAddPermission);
    }
    
    //好友接口
    public int findUser(int findType, String target)
    {
    	return IMEngine.IM_FindUser(findType, target);
    }
    public int requestAddFriend(List<String> users, String comments)
    {
    	JSONArray jsonArray = new JSONArray(users);        
    	return IMEngine.IM_RequestAddFriend(jsonArray.toString(), comments);
    }
    public int dealBeRequestAddFriend(String userID, int dealResult)
    {
    	return IMEngine.IM_DealBeRequestAddFriend(userID, dealResult);
    }
    public int deleteFriend(List<String> users, int deleteType)
    {
    	JSONArray jsonArray = new JSONArray(users);
    	return IMEngine.IM_DeleteFriend(jsonArray.toString(), deleteType);
    }
    public int blackFriend(int type, List<String> users)
    {
    	JSONArray jsonArray = new JSONArray(users);
    	return IMEngine.IM_BlackFriend(type, jsonArray.toString());
    }
    public int queryFriends(int type, int startIndex, int count)
    {
    	return IMEngine.IM_QueryFriends(type, startIndex, count);
    }
    public int queryFriendRequestList(int startIndex, int count)
    {
    	return IMEngine.IM_QueryFriendRequestList(startIndex, count);
    }
    
    
    public final static class RequestId {
        public long id;
    }

    public final static class MessageSendStatus {
        private int errCode;
        private long msgId;


        public MessageSendStatus() {

        }

        public MessageSendStatus(int errCode, long msgId) {
            this.errCode = errCode;
            this.msgId = msgId;
        }

        public void setErrorCode(int errCode) {
            this.errCode = errCode;
        }

        public void setMessageId(long msgId) {
            this.msgId = msgId;
        }

        public int getErrorCode() {
            return errCode;
        }

        public long getMessageId() {
            return msgId;
        }
    }

    public final static class TranslateMsgStatus{
        private int errCode;
        private long msgId;

        public void setErrorCode(int errCode) {
            this.errCode = errCode;
        }

        public void setMessageId(long msgId) {
            this.msgId = msgId;
        }

        public int getErrorCode() {
            return errCode;
        }

        public long getMessageId() {
            return msgId;
        }

    }

    private Runnable paresMessage = new Runnable() {
        @Override
        public void run() {
            String msg = null;
            while (true) {
                try {
                    byte[] bytes = IMEngine.IM_GetMessage();
                    msg = new String(bytes, "UTF-8"); // please debate what the safest charset should be?
                } catch (UnsupportedEncodingException e) {
                    Log.e(TAG, "Couldn't convert the jbyteArray to jstring");
                }
                if(null == msg){
                    continue;
                }
                if (mIsExit)
                    break;
                if (DEBUG) {
                    Log.d(TAG, "IM_GetMessage :" + msg);
                }
                handlerMessage(msg);
                IMEngine.IM_PopMessage();
            }
            mThread = null;
        }
    };

    private void handlerMessage(String msg) {
        if (null == msg ||  msg.equals("") )
            return;
        try {
            Gson gson = new Gson();
            YouMeIMJsonResponse jsonResponse = null;
            if (null != msg) {
                jsonResponse = gson.fromJson(msg, YouMeIMJsonResponse.class);
            }else{
                return;
            }
            switch (jsonResponse.command) {
                case CMD_RECEIVE_MESSAGE_NITIFY:
                {
                    try {
                        NewMessageNotifyObj newNotifier =  gson.fromJson(msg,NewMessageNotifyObj.class);
                        mHandler.post(new YIMCallBackProtocol(mMessageListener, "OnRecvNewMessage",
                                newNotifier.chatType, newNotifier.targetID));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_GET_RENCENT_CONTACTS:
                {
                    try {
                    	Contacts contacts = gson.fromJson(msg,Contacts.class);
                        mHandler.post(new YIMCallBackProtocol(mContactListener, "onGetContact",contacts.contacts));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_QUERY_HISTORY_MESSAGE:
                {
                    try {
                        YIMHistoryMessage historyMessage = gson.fromJson(msg,YIMHistoryMessage.class);
                        mHandler.post(new YIMCallBackProtocol(mMessageListener, "OnQueryHistoryMessage",jsonResponse.errcode,historyMessage));
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_GET_ROOM_HISTORY_MSG:
                {
                	RoomHistoryMessage roomHistoryMessages = gson.fromJson(msg, RoomHistoryMessage.class);
                	ArrayList<YIMMessage> messages = new ArrayList<YIMMessage>();
                	if (roomHistoryMessages.messageList != null && roomHistoryMessages.messageList.size() > 0){
                		for(Iterator<RecvMessage> it = roomHistoryMessages.messageList.iterator(); it.hasNext();) {
                    		YIMMessage message = new YIMMessage();
                            IYIMMessageBodyBase msgBody = null;
                            RecvMessage item = (RecvMessage)it.next();
                            message.setChatType(item.chatType);
                            message.setReceiveID(item.receiveId);
                            message.setSenderID(item.senderId);
                            message.setMeesageID(item.serial);
                            message.setMessageType(item.msgType);
                            message.setCreateTime(item.createTime);
                            message.setDistance(item.distance);
                            message.setRead(item.getIsRead());
                            if (item.msgType == YIMService.MessageBodyType.TXT) {
                                msgBody = new YIMMessageBodyText();
                                ((YIMMessageBodyText) msgBody).setMessageContent(item.content);
                                ((YIMMessageBodyText) msgBody).setAttachParam(item.attachParam);
                            } else if (item.msgType == YIMService.MessageBodyType.CustomMesssage) {
                                msgBody = new YIMMessageBodyCustom();
                                ((YIMMessageBodyCustom) msgBody).setMessageContent(item.content);
                            } else if (item.msgType == YIMService.MessageBodyType.Voice) {
                                msgBody = new YIMMessageBodyAudio();
                                ((YIMMessageBodyAudio) msgBody).setAudioTime(item.duration);
                                ((YIMMessageBodyAudio) msgBody).setParam(item.param);
                                ((YIMMessageBodyAudio) msgBody).setText(item.extraText);
                            }else if (item.msgType == MessageBodyType.File){
                                msgBody = new YIMMessageBodyFile();
                                ((YIMMessageBodyFile) msgBody).setFileType(item.fileType);
                                ((YIMMessageBodyFile) msgBody).setFileSize(item.fileSize);
                                ((YIMMessageBodyFile) msgBody).setFileExtension(item.fileExtension);
                                ((YIMMessageBodyFile) msgBody).setExtParam(item.extraParam);
                                ((YIMMessageBodyFile) msgBody).setFileName(item.fileName);
                            }else if (item.msgType == MessageBodyType.Gift){
                                msgBody = new YIMMessageBodyGift();
                                ((YIMMessageBodyGift) msgBody).setAnchor(item.anchor);
                                ((YIMMessageBodyGift) msgBody).setGiftCount(item.giftCount);
                                ((YIMMessageBodyGift) msgBody).setGiftID(item.giftID);
                                try {
                                    YIMExtraGifParam giftExtParam = gson.fromJson(item.param, YIMExtraGifParam.class);
                                    ((YIMMessageBodyGift) msgBody).setExtParam(giftExtParam);
                                }catch (Exception e){
                                    e.printStackTrace();
                                    return;
                                }
                            }
                            message.setMessageBody(msgBody);
                            messages.add(message);
                    	}
                	}
                	
                    try {
                         mHandler.post(new YIMCallBackProtocol(mMessageListener, "OnQueryRoomHistoryMessageFromServer", jsonResponse.errcode, roomHistoryMessages.roomID, roomHistoryMessages.remain, messages));
                     } catch (NullPointerException e) {
                         e.printStackTrace();
                     }
                }
                break;
                case CMD_STOP_AUDIOSPEECH:
                {
                    SpeechMessageInfo speachMessage = gson.fromJson(msg,SpeechMessageInfo.class);
                    try {
                        mHandler.post(new YIMCallBackProtocol(mMessageListener, "OnStopAudioSpeechStatus", jsonResponse.errcode,speachMessage.getRequestID(),speachMessage.getDownloadURL(),
                                speachMessage.getDuration(),speachMessage.getFileSize(),speachMessage.getLocalPath(),speachMessage.getText()));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_LOGIN: {
                    Login login = gson.fromJson(msg, Login.class);
                    try {
                        Log.d(TAG, "mLoginListener class:" + mLoginListener.getClass().getName());
                        mHandler.post(new YIMCallBackProtocol(mLoginListener, "onLogin", login.getYoumeId(), jsonResponse.errcode));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }

                }
                break;
                case CMD_LOGOUT:
                    try {
                        mHandler.post(new YIMCallBackProtocol(mLoginListener, "onLogout"));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }

                    break;
                case CMD_KICK_OFF:
                    try {
                        mHandler.post(new YIMCallBackProtocol(mLoginListener, "onKickOff"));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }

                    break;    
                case CMD_ENTER_ROOM: {
                    ChatRoom joinGroupResponse = gson.fromJson(msg, ChatRoom.class);

                    try {
                        mHandler.post(new YIMCallBackProtocol(mChatRoomListener, "onJoinChatRoom", jsonResponse.errcode, joinGroupResponse.groupId));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_LEAVE_ROOM: {
                    ChatRoom joinGroupResponse = gson.fromJson(msg, ChatRoom.class);

                    try {
                        mHandler.post(new YIMCallBackProtocol(mChatRoomListener, "onLeaveChatRoom", jsonResponse.errcode, joinGroupResponse.groupId));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;                
                case CMD_USER_ENTER_ROOM: {
                	UserChatRoom userChatRoom = gson.fromJson(msg, UserChatRoom.class);
					try {
					    mHandler.post(new YIMCallBackProtocol(mChatRoomListener, "onUserJoinChatRoom", userChatRoom.channelID, userChatRoom.userID));
					} catch (NullPointerException e) {
					    e.printStackTrace();
					}
                }
                break;
                case CMD_USER_LEAVE_ROOM: {
                	UserChatRoom userChatRoom = gson.fromJson(msg, UserChatRoom.class);
					try {
					    mHandler.post(new YIMCallBackProtocol(mChatRoomListener, "onUserLeaveChatRoom", userChatRoom.channelID, userChatRoom.userID));
					} catch (NullPointerException e) {
					    e.printStackTrace();
					}
                }
                break;
                case CMD_RECV_MESSAGE: {
                    YIMMessage message = new YIMMessage();
                    IYIMMessageBodyBase msgBody = null;
//                    Log.e("CMD_RECV_MESSAGE","msg = " + msg ) ;
                    RecvMessage recvMessage = gson.fromJson(msg, RecvMessage.class);
                    message.setChatType(recvMessage.chatType);
                    /*message.setGroupID(recvMessage.);*/
                    message.setReceiveID(recvMessage.receiveId);
                    message.setSenderID(recvMessage.senderId);
                    message.setMeesageID(recvMessage.serial);
                    message.setMessageType(recvMessage.msgType);
                    message.setCreateTime(recvMessage.createTime);
                    message.setDistance(recvMessage.distance);
                    message.setRead(recvMessage.getIsRead());
                    if (recvMessage.msgType == YIMService.MessageBodyType.TXT) {
                        msgBody = new YIMMessageBodyText();
                        ((YIMMessageBodyText) msgBody).setMessageContent(recvMessage.content);
                        ((YIMMessageBodyText) msgBody).setAttachParam(recvMessage.attachParam);
                    } else if (recvMessage.msgType == YIMService.MessageBodyType.CustomMesssage) {
                        msgBody = new YIMMessageBodyCustom();
                        ((YIMMessageBodyCustom) msgBody).setMessageContent(recvMessage.content);
                    } else if (recvMessage.msgType == YIMService.MessageBodyType.Voice) {
                        msgBody = new YIMMessageBodyAudio();
                        ((YIMMessageBodyAudio) msgBody).setAudioTime(recvMessage.duration);
                        ((YIMMessageBodyAudio) msgBody).setParam(recvMessage.param);
                        ((YIMMessageBodyAudio) msgBody).setText(recvMessage.extraText);
                    }else if (recvMessage.msgType == MessageBodyType.File)
                    {
                        msgBody = new YIMMessageBodyFile();
                        ((YIMMessageBodyFile) msgBody).setFileType(recvMessage.fileType);
                        ((YIMMessageBodyFile) msgBody).setFileSize(recvMessage.fileSize);
                        ((YIMMessageBodyFile) msgBody).setFileExtension(recvMessage.fileExtension);
                        ((YIMMessageBodyFile) msgBody).setExtParam(recvMessage.extraParam);
                        ((YIMMessageBodyFile) msgBody).setFileName(recvMessage.fileName);
                    }else if (recvMessage.msgType == MessageBodyType.Gift)
                    {

                        msgBody = new YIMMessageBodyGift();
                        ((YIMMessageBodyGift) msgBody).setAnchor(recvMessage.anchor);
                        ((YIMMessageBodyGift) msgBody).setGiftCount(recvMessage.giftCount);
                        ((YIMMessageBodyGift) msgBody).setGiftID(recvMessage.giftID);
                        try {
                            YIMExtraGifParam giftExtParam = gson.fromJson(recvMessage.param, YIMExtraGifParam.class);
                            if (DEBUG)
                                Log.d(TAG, "recv giftExtParam:" + giftExtParam.getNickName());
                            ((YIMMessageBodyGift) msgBody).setExtParam(giftExtParam);
                        }catch (Exception e){
                            e.printStackTrace();
                            return;
                        }
                    }

                    message.setMessageBody(msgBody);
                    try {
                        mHandler.post(new YIMCallBackProtocol(mMessageListener, "onRecvMessage", message));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }

                }
                break;
                case CMD_SND_VOICE_MSG: {
                    SendVoiceMsgInfo chatVoice = gson.fromJson(msg, SendVoiceMsgInfo.class);
                    try {
                        mHandler.post(new YIMCallBackProtocol(mMessageListener, "onSendAudioMessageStatus", chatVoice.getRequestId(), jsonResponse.errcode, chatVoice.getText(),
                                chatVoice.getLocalPath(), chatVoice.getDuration(), chatVoice.getSendTime(), chatVoice.getIsForbidRoom(), chatVoice.getReasonType(), chatVoice.getEndTime(), chatVoice.getMessageID() ));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }

                break;
                case CMD_SEND_MESSAGE_STATUS: {
                    SendMessage sendMessage = gson.fromJson(msg, SendMessage.class);
                    try {                   
                            mHandler.post(new YIMCallBackProtocol(mMessageListener, "onSendMessageStatus", sendMessage.getRequestId(), jsonResponse.errcode, sendMessage.getSendTime(),
                                sendMessage.getIsForbidRoom(), sendMessage.getReasonType(), sendMessage.getEndTime(), sendMessage.getMessageID() ));
                        
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_DOWNLOAD: {
                    Download download = gson.fromJson( msg, Download.class );

                    YIMMessage message = new YIMMessage();
                    IYIMMessageBodyBase msgBody = null;
                    RecvMessage recvMessage = gson.fromJson(msg, RecvMessage.class);
                    message.setChatType(recvMessage.chatType);

                    message.setReceiveID(recvMessage.receiveId);
                    message.setSenderID(recvMessage.senderId);
                    message.setMeesageID(recvMessage.serial);
                    message.setMessageType(recvMessage.msgType);
                    message.setCreateTime(recvMessage.createTime);
                    message.setDistance(recvMessage.distance);
                    message.setRead(recvMessage.getIsRead());

                    if (recvMessage.msgType == YIMService.MessageBodyType.Voice) {
                        msgBody = new YIMMessageBodyAudio();
                        ((YIMMessageBodyAudio) msgBody).setAudioTime(recvMessage.duration);
                        ((YIMMessageBodyAudio) msgBody).setParam(recvMessage.param);
                        ((YIMMessageBodyAudio) msgBody).setText(recvMessage.extraText);
                        ((YIMMessageBodyAudio) msgBody).setLocalPath(download.savePath);
                    }else if (recvMessage.msgType == MessageBodyType.File)
                    {
                        msgBody = new YIMMessageBodyFile();
                        ((YIMMessageBodyFile) msgBody).setFileType(recvMessage.fileType);
                        ((YIMMessageBodyFile) msgBody).setFileSize(recvMessage.fileSize);
                        ((YIMMessageBodyFile) msgBody).setFileExtension(recvMessage.fileExtension);
                        ((YIMMessageBodyFile) msgBody).setExtParam(recvMessage.extraParam);
                        ((YIMMessageBodyFile) msgBody).setFileName(recvMessage.fileName);
                        ((YIMMessageBodyFile) msgBody).setLocalPath(download.savePath);
                    }

                    message.setMessageBody(msgBody);

                    try {
                        mHandler.post(new YIMCallBackProtocol(mMessageListener, "onDownload", jsonResponse.errcode, message ,  download.savePath));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_DOWNLOAD_URL:{
                    DownloadUrl downloadurl = gson.fromJson( msg, DownloadUrl.class );
                    try{
                        mHandler.post(new YIMCallBackProtocol(mMessageListener, "onDownloadByUrl", jsonResponse.errcode, downloadurl.fromUrl, downloadurl.savePath,downloadurl.audioTime));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_GET_USR_INFO: {
                    UserInfoString userInfoString = gson.fromJson(msg, UserInfoString.class);
                    try {
                        YIMExtraUserInfo userInfo = gson.fromJson(userInfoString.userInfoString, YIMExtraUserInfo.class);
                        userInfo.setUserID(userInfoString.userID);
                        mHandler.post(new YIMCallBackProtocol(mContactListener, "onGetUserInfo",jsonResponse.errcode, userInfo));
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_QUERY_USER_STATUS: {
                    UserStatusInfo userStatusInfo = gson.fromJson(msg, UserStatusInfo.class);
                    try {
                        mHandler.post(new YIMCallBackProtocol(mContactListener, "onQueryUserStatus", jsonResponse.errcode,userStatusInfo));
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_AUDIO_PLAY_COMPLETE: {
                    OnPlayCompleteNotify onPlayCompleteNotify = gson.fromJson(msg, OnPlayCompleteNotify.class);
                    try {
                        mHandler.post(new YIMCallBackProtocol(mAudioPlayListener, "onPlayCompletion", jsonResponse.errcode,onPlayCompleteNotify.audioPath));
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_STOP_SEND_AUDIO: {
                    VoiceInfo chatVoice = gson.fromJson(msg, VoiceInfo.class);
                    try {
                        mHandler.post(new YIMCallBackProtocol(mMessageListener, "onStartSendAudioMessage", chatVoice.requestId, jsonResponse.errcode, chatVoice.extraText,
                                chatVoice.localPath, chatVoice.duration));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_GET_DISTRICT:{
                    Log.d("CMD_GET_DISTRICT","msg = " + msg);
                    GeographyLocation geographyLocation = gson.fromJson(msg,GeographyLocation.class) ;
                    try {
                        mHandler.post(new YIMCallBackProtocol(mLocationListen, "OnUpdateLocation",  jsonResponse.errcode, geographyLocation));
                    }catch(Exception e){
                        e.printStackTrace(); ;
                    }
                }
                break;
                case CMD_GET_PEOPLE_NEARBY:{
                    Log.d("CMD_GET_PEOPLE_NEARBY","msg = " + msg);
                    RelativeLocationInfo relativeLocations = gson.fromJson(msg,RelativeLocationInfo.class) ;
                    try{
                        mHandler.post(new YIMCallBackProtocol(mLocationListen, "OnGetNearbyObjects",  jsonResponse.errcode,relativeLocations.relativeLocations,relativeLocations.startDistance,relativeLocations.endDistance ));
                    }catch (Exception e){
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_GET_DISTANCE:{
                	UserDistanceInfo userDistance = gson.fromJson(msg, UserDistanceInfo.class) ;
                     try{
                         mHandler.post(new YIMCallBackProtocol(mLocationListen, "OnGetDistance",  jsonResponse.errcode, userDistance.userID, userDistance.distance));
                     }catch (Exception e){
                         e.printStackTrace();
                     }
                }
                break;
                case CMD_TRANSLATE_COMPLETE:{
                    Log.d("CMD_TRANSLATE_COMPLETE","msg = " + msg);
                    TranlateTextInfo tranlateInfo = gson.fromJson(msg,TranlateTextInfo.class) ;
                    try {
                        mHandler.post(new YIMCallBackProtocol(mMessageListener, "OnTranslateTextComplete", jsonResponse.errcode,tranlateInfo.requestID,tranlateInfo.text,tranlateInfo.srcLangCode,tranlateInfo.destLangCode));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_GET_MICROPHONE_STATUS: {
                	MicrophoneStatus MicrophoneStatus = gson.fromJson(msg,MicrophoneStatus.class) ;
                	try {
                        mHandler.post(new YIMCallBackProtocol(mAudioPlayListener, "onGetMicrophoneStatus", MicrophoneStatus.status));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_GET_TIPOFF_MSG: {
                	AccusationResult accusationResult = gson.fromJson(msg,AccusationResult.class) ;
                    try {
                        mHandler.post(new YIMCallBackProtocol(mMessageListener, "onAccusationResultNotify",accusationResult.iResult,accusationResult.strUserID,accusationResult.iAccusationTime));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_RECV_NOTICE: {
                	NoticeInfo noticeInfo = gson.fromJson(msg,NoticeInfo.class) ;
                    try {
                        mHandler.post(new YIMCallBackProtocol(mNoticeListener, "onRecvNotice",noticeInfo));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_CANCEL_NOTICE: {
                	NoticeCancelInfo noticeInfo = gson.fromJson(msg,NoticeCancelInfo.class) ;
                    try {
                        mHandler.post(new YIMCallBackProtocol(mNoticeListener, "onCancelNotice",noticeInfo.iNoticeID, noticeInfo.strChannelID));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_GET_FORBID_RECORD:{
                    Log.d("CMD_GET_PEOPLE_NEARBY","msg = " + msg);
                    YIMForbiddenSpeakList forbiddenList = gson.fromJson(msg,YIMForbiddenSpeakList.class) ;

                    try{
                        mHandler.post(new YIMCallBackProtocol(mMessageListener, "OnGetForbiddenSpeakInfo",  jsonResponse.errcode, forbiddenList.forbiddenList));
                    }catch (Exception e) {
                        e.printStackTrace();
                    }
                }
                break;
                case CMD_SET_MASK_USER_MSG:{
                    try {
                    	BlockUserInfo block = gson.fromJson(msg, BlockUserInfo.class);
                        mHandler.post(new YIMCallBackProtocol(mMessageListener, "OnBlockUser", jsonResponse.errcode,  block.getUserID(), block.getBlock()));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
				case CMD_GET_MASK_USER_MSG:{
					try {
						BlockUserList userList = gson.fromJson(msg, BlockUserList.class);
                        mHandler.post(new YIMCallBackProtocol(mMessageListener, "OnGetBlockUsers", jsonResponse.errcode, userList.userList));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }         	
                }
                break;
				case CMD_CLEAN_MASK_USER_MSG:{
					try {
                        mHandler.post(new YIMCallBackProtocol(mMessageListener, "OnUnBlockAllUser", jsonResponse.errcode));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
				}
				break;
				case CMD_GET_ROOM_INFO:{
					try {
						RoomInfo roomInfo = gson.fromJson(msg, RoomInfo.class);
                        mHandler.post(new YIMCallBackProtocol(mChatRoomListener, "OnGetRoomMemberCount", jsonResponse.errcode, roomInfo.roomID, roomInfo.memberCount));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
				}
				break;
				case CMD_GET_SPEECH_TEXT:{
					try {
						RecognizeSpeechText speechTextInfo = gson.fromJson(msg, RecognizeSpeechText.class);
                        mHandler.post(new YIMCallBackProtocol(mMessageListener, "OnGetRecognizeSpeechText", jsonResponse.errcode, speechTextInfo.requestId, speechTextInfo.text));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
				}
				break;	
				case CMD_LEAVE_ALL_ROOM: {  
                    try {
                        mHandler.post(new YIMCallBackProtocol(mChatRoomListener, "OnLeaveAllChatRooms", jsonResponse.errcode));
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                }
                break;
			case CMD_GET_RECONNECT_RESULT: {
				try {
					ReconnectResult reconnectResult = gson.fromJson(msg, ReconnectResult.class);
					mHandler.post(new YIMCallBackProtocol(mReconnectListener, "OnRecvReconnectResult",
							reconnectResult.iResult));
				} catch (NullPointerException e) {
					e.printStackTrace();
				}
			}
				break;
			case CMD_START_RECONNECT: {
				try {
					mHandler.post(new YIMCallBackProtocol(mReconnectListener, "OnStartReconnect"));
				} catch (NullPointerException e) {
					e.printStackTrace();
				}
			}
				break;
			case CMD_RECORD_VOLUME: {
				VolumeInfo volumeInfo = gson.fromJson(msg, VolumeInfo.class);
				try {
					mHandler.post(new YIMCallBackProtocol(mMessageListener, "OnRecordVolume", volumeInfo.iVolume));
				} catch (NullPointerException e) {
					e.printStackTrace();
				}
			}
				break;
			case CMD_GET_USER_PROFILE: {
				
				UserProfileInfo profileInfo = gson.fromJson(msg,UserProfileInfo.class);
				try { 
					mHandler.post(new YIMCallBackProtocol(mUserProfileListener, "OnQueryUserInfo", jsonResponse.errcode, profileInfo));
				} catch (NullPointerException e) {
					e.printStackTrace();
				}
			}
			    break;
			case CMD_SET_USER_PROFILE: {
				 try {
                     mHandler.post(new YIMCallBackProtocol(mUserProfileListener, "OnSetUserInfo", jsonResponse.errcode));
                 } catch (NullPointerException e) {
                     e.printStackTrace();
                 }
			}
			break;
			case CMD_SWITCH_USER_STATE: {
				 try {
                     mHandler.post(new YIMCallBackProtocol(mUserProfileListener, "OnSwitchUserOnlineState", jsonResponse.errcode));
                 } catch (NullPointerException e) {
                     e.printStackTrace();
                 }
			}
			break;
			case CMD_SET_USER_PHOTO: {
				 try {
					 PhotoUrlInfo photoUrlInfo = gson.fromJson(msg, PhotoUrlInfo.class);
                     mHandler.post(new YIMCallBackProtocol(mUserProfileListener, "OnSetPhotoUrl", jsonResponse.errcode, photoUrlInfo.photoUrl));
                 } catch (NullPointerException e) {
                     e.printStackTrace();
                 }
			}
			break;
			case CMD_FIND_FRIEND_BY_ID: {
				 try {
					 YIMUserBriefInfoList briefInfoList = gson.fromJson(msg, YIMUserBriefInfoList.class);
                    mHandler.post(new YIMCallBackProtocol(mFriendListener, "OnFindUser", jsonResponse.errcode, briefInfoList.userList));
                } catch (NullPointerException e) {
                    e.printStackTrace();
                }
			}
			break;
			case CMD_REQUEST_ADD_FRIEND: {
				 try {
					 YIMFriendUserID userID = gson.fromJson(msg, YIMFriendUserID.class);
                    mHandler.post(new YIMCallBackProtocol(mFriendListener, "OnRequestAddFriend", jsonResponse.errcode, userID.userID));
                } catch (NullPointerException e) {
                    e.printStackTrace();
                }
			}
			break;
			case CMD_REQUEST_ADD_FRIEND_NOTIFY: {
				 try {
					 YIMFriendCommon commonInfo = gson.fromJson(msg, YIMFriendCommon.class);
                    mHandler.post(new YIMCallBackProtocol(mFriendListener, "OnBeRequestAddFriendNotify", commonInfo.userID, commonInfo.comments));
                } catch (NullPointerException e) {
                    e.printStackTrace();
                }
			}
			break;
			case CMD_BE_ADD_FRIENT: {
				 try {
					 YIMFriendCommon commonInfo = gson.fromJson(msg, YIMFriendCommon.class);
                    mHandler.post(new YIMCallBackProtocol(mFriendListener, "OnBeAddFriendNotify", commonInfo.userID, commonInfo.comments));
                } catch (NullPointerException e) {
                    e.printStackTrace();
                }
			}
			break;
			case CMD_DEAL_ADD_FRIEND: {
				 try {
					 YIMFriendDealResult dealResult = gson.fromJson(msg, YIMFriendDealResult.class);
                    mHandler.post(new YIMCallBackProtocol(mFriendListener, "OnDealBeRequestAddFriend", jsonResponse.errcode, dealResult.userID, dealResult.comments, dealResult.dealResult));
                } catch (NullPointerException e) {
                    e.printStackTrace();
                }
			}
			break;
			case CMD_ADD_FRIENT_RESULT_NOTIFY: {
				 try {
					 YIMFriendDealResult dealResult = gson.fromJson(msg, YIMFriendDealResult.class);
                   mHandler.post(new YIMCallBackProtocol(mFriendListener, "OnRequestAddFriendResultNotify", dealResult.userID, dealResult.comments, dealResult.dealResult));
               } catch (NullPointerException e) {
                   e.printStackTrace();
               }
			}
			break;
			case CMD_DELETE_FRIEND: {
				 try {
					 YIMFriendUserID userID = gson.fromJson(msg, YIMFriendUserID.class);
                   mHandler.post(new YIMCallBackProtocol(mFriendListener, "OnDeleteFriend", jsonResponse.errcode, userID.userID));
               } catch (NullPointerException e) {
                   e.printStackTrace();
               }
			}
			break;
			case CMD_BE_DELETE_FRIEND_NOTIFY: {
				 try {
					 YIMFriendUserID userID = gson.fromJson(msg, YIMFriendUserID.class);
                   mHandler.post(new YIMCallBackProtocol(mFriendListener, "OnBeDeleteFriendNotify", userID.userID));
               } catch (NullPointerException e) {
                   e.printStackTrace();
               }
			}
			break;
			case CMD_BLACK_FRIEND: {
				 try {
					 YIMBlackFriendInfo blackInfo = gson.fromJson(msg, YIMBlackFriendInfo.class);
                  mHandler.post(new YIMCallBackProtocol(mFriendListener, "OnBlackFriend", jsonResponse.errcode, blackInfo.type, blackInfo.userID));
              } catch (NullPointerException e) {
                  e.printStackTrace();
              }
			}
			break;
			case CMD_BE_BLACK_FRIEND_NOTIFY: {
				 try {
					 YIMFriendUserID userID = gson.fromJson(msg, YIMFriendUserID.class);
                  mHandler.post(new YIMCallBackProtocol(mFriendListener, "OnBeBlackNotify", userID.userID));
              } catch (NullPointerException e) {
                  e.printStackTrace();
              }
			}
			break;
			case CMD_QUERY_FRIEND_LIST: {
				 try {
					 YIMFriendListInfo friendList = gson.fromJson(msg, YIMFriendListInfo.class);
                  mHandler.post(new YIMCallBackProtocol(mFriendListener, "OnQueryFriends", jsonResponse.errcode, friendList.type, friendList.startIndex, friendList.userList));
              } catch (NullPointerException e) {
                  e.printStackTrace();
              }
			}
			break;
			case CMD_QUERY_FRIEND_REQUEST_LIST: {
				 try {
					 YIMFriendRequestInfoList requestList = gson.fromJson(msg, YIMFriendRequestInfoList.class);
					 ArrayList<YIMFriendRequestInfo> tmp = requestList.userList;					 
                 mHandler.post(new YIMCallBackProtocol(mFriendListener, "OnQueryFriendRequestList", jsonResponse.errcode, requestList.startIndex, requestList.userList));
             } catch (NullPointerException e) {
                 e.printStackTrace();
             }
			}
			break;
			case CMD_HXR_USER_INFO_CHANGE_NOTIFY: {
				 try {					 
					 YIMFriendUserID userID = gson.fromJson(msg, YIMFriendUserID.class);
	                 mHandler.post(new YIMCallBackProtocol(mUserProfileListener, "OnUserInfoChangeNotify", userID.userID));
				 } catch (NullPointerException e) {
                 e.printStackTrace();
             }
			}
			break;
                default:
                    break;
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private boolean checkString(String type, String text){
        boolean res = true;
        if (TextUtils.isEmpty(type)){
            res = false;
        }else {
            if (TextUtils.isEmpty(text)){
                res = false;
                debugInfo(type + " is null");
            }
        }
        return res;
    }

    private void debugInfo(String info){
        if (DEBUG)
            Log.e(TAG, info);
    }   
    

}
