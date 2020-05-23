//
//  YIMMessage.h
//  YoumeIMUILib
//
//  Created by 余俊澎 on 16/8/11.
//  Copyright © 2016年 余俊澎. All rights reserved.
//
#import <Foundation/Foundation.h>

#ifndef YoumeIMMessage_h
#define YoumeIMMessage_h

//消息传输类型
typedef enum
{
    YIMORIDINARY = 0,  //普通传输
    YIMCARBON_COPY = 1 //抄送
}YIMMsgTransTypeOC;

//聊天类型
typedef enum
{
    YIMChatType_Unknow = 0,
    YIMChatType_PrivateChat = 1,//私聊
    YIMChatType_RoomChat = 2,   //聊天室
    YIMChatType_Multi = 3,
}YIMChatTypeOC;

//用户状态
typedef enum
{
    YIMSTATUS_ONLINE,	//在线
    YIMSTATUS_OFFLINE,	//离线
    YIMSTATUS_INVISIBLE //隐身
}YIMUserStatusOC;

//文件类型
typedef enum
{
    YIMFileType_Other = 0,
    YIMFileType_Audio = 1,
    YIMFileType_Image = 2,
    YIMFileType_Video = 3
}YIMFileTypeOC;

//消息类型
typedef enum
{
    YIMMessageBodyType_Unknow = 0,
    YIMMessageBodyType_TXT = 1,
    YIMMessageBodyType_CustomMesssage = 2,
    YIMMessageBodyType_Emoji = 3,
    YIMMessageBodyType_Image = 4,
    YIMMessageBodyType_Voice = 5,
    YIMMessageBodyType_Video = 6,
    YIMMessageBodyType_File = 7,
    YIMMessageBodyType_Gift = 8
}YIMMessageBodyTypeOC;

typedef enum
{
    YIMLANG_AUTO,
    YIMLANG_AF,			// 南非荷兰语
    YIMLANG_AM,			// 阿姆哈拉语
    YIMLANG_AR,			// 阿拉伯语
    YIMLANG_AR_AE,			// 阿拉伯语+阿拉伯联合酋长国
    YIMLANG_AR_BH,			// 阿拉伯语+巴林
    YIMLANG_AR_DZ,			// 阿拉伯语+阿尔及利亚
    YIMLANG_AR_KW,			// 阿拉伯语+科威特
    YIMLANG_AR_LB,			// 阿拉伯语+黎巴嫩
    YIMLANG_AR_OM,			// 阿拉伯语+阿曼
    YIMLANG_AR_SA,			// 阿拉伯语+沙特阿拉伯
    YIMLANG_AR_SD,			// 阿拉伯语+苏丹
    YIMLANG_AR_TN,			// 阿拉伯语+突尼斯
    YIMLANG_AZ,			// 阿塞拜疆
    YIMLANG_BE,			// 白俄罗斯语
    YIMLANG_BG,			// 保加利亚语
    YIMLANG_BN,			// 孟加拉
    YIMLANG_BS,			// 波斯尼亚语
    YIMLANG_CA,			// 加泰罗尼亚语
    YIMLANG_CA_ES,			// 加泰罗尼亚语+西班牙
    YIMLANG_CO,			// 科西嘉
    YIMLANG_CS,			// 捷克语
    YIMLANG_CY,			// 威尔士语
    YIMLANG_DA,			// 丹麦语
    YIMLANG_DE,			// 德语
    YIMLANG_DE_CH,			// 德语+瑞士
    YIMLANG_DE_LU,			// 德语+卢森堡
    YIMLANG_EL,			// 希腊语
    YIMLANG_EN,			// 英语
    YIMLANG_EN_CA,			// 英语+加拿大
    YIMLANG_EN_IE,			// 英语+爱尔兰
    YIMLANG_EN_ZA,			// 英语+南非
    YIMLANG_EO,			// 世界语
    YIMLANG_ES,			// 西班牙语
    YIMLANG_ES_BO,			// 西班牙语+玻利维亚
    YIMLANG_ES_AR,			// 西班牙语+阿根廷
    YIMLANG_ES_CO,			// 西班牙语+哥伦比亚
    YIMLANG_ES_CR,			// 西班牙语+哥斯达黎加
    YIMLANG_ES_ES,			// 西班牙语+西班牙
    YIMLANG_ET,			// 爱沙尼亚语
    YIMLANG_ES_PA,			// 西班牙语+巴拿马
    YIMLANG_ES_SV,			// 西班牙语+萨尔瓦多    
    YIMLANG_ES_VE,			// 西班牙语+委内瑞拉
    YIMLANG_ET_EE,			// 爱沙尼亚语+爱沙尼亚
    YIMLANG_EU,			// 巴斯克
    YIMLANG_FA,			// 波斯语
    YIMLANG_FI,			// 芬兰语
    YIMLANG_FR,			// 法语
    YIMLANG_FR_BE,			// 法语+比利时
    YIMLANG_FR_CA,			// 法语+加拿大
    YIMLANG_FR_CH,			// 法语+瑞士
    YIMLANG_FR_LU,			// 法语+卢森堡
    YIMLANG_FY,			// 弗里斯兰
    YIMLANG_GA,			// 爱尔兰语
    YIMLANG_GD,			// 苏格兰盖尔语
    YIMLANG_GL,			// 加利西亚
    YIMLANG_GU,			// 古吉拉特文
    YIMLANG_HA,			// 豪撒语
    YIMLANG_HI,			// 印地语
    YIMLANG_HR,			// 克罗地亚语
    YIMLANG_HT,			// 海地克里奥尔
    YIMLANG_HU,			// 匈牙利语
    YIMLANG_HY,			// 亚美尼亚
    YIMLANG_ID,			// 印度尼西亚
    YIMLANG_IG,			// 伊博
    YIMLANG_IS,			// 冰岛语
    YIMLANG_IT,			// 意大利语
    YIMLANG_IT_CH,			// 意大利语+瑞士
    YIMLANG_JA,			// 日语
    YIMLANG_KA,			// 格鲁吉亚语
    YIMLANG_KK,			// 哈萨克语
    YIMLANG_KN,			// 卡纳达
    YIMLANG_KM,			// 高棉语
    YIMLANG_KO,			// 朝鲜语
    YIMLANG_KO_KR,			// 朝鲜语+南朝鲜
    YIMLANG_KU,			// 库尔德
    YIMLANG_KY,			// 吉尔吉斯斯坦
    YIMLANG_LA,			// 拉丁语
    YIMLANG_LB,			// 卢森堡语
    YIMLANG_LO,			// 老挝
    YIMLANG_LT,			// 立陶宛语
    YIMLANG_LV,			// 拉托维亚语+列托
    YIMLANG_MG,			// 马尔加什
    YIMLANG_MI,			// 毛利
    YIMLANG_MK,			// 马其顿语
    YIMLANG_ML,			// 马拉雅拉姆
    YIMLANG_MN,			// 蒙古
    YIMLANG_MR,			// 马拉地语
    YIMLANG_MS,			// 马来语
    YIMLANG_MT,			// 马耳他
    YIMLANG_MY,			// 缅甸
    YIMLANG_NL,			// 荷兰语
    YIMLANG_NL_BE,			// 荷兰语+比利时
    YIMLANG_NE,			// 尼泊尔
    YIMLANG_NO,			// 挪威语
    YIMLANG_NY,			// 齐切瓦语
    YIMLANG_PL,			// 波兰语
    YIMLANG_PS,			// 普什图语
    YIMLANG_PT,			// 葡萄牙语
    YIMLANG_PT_BR,			// 葡萄牙语+巴西
    YIMLANG_RO,			// 罗马尼亚语
    YIMLANG_RU,			// 俄语
    YIMLANG_SD,			// 信德
    YIMLANG_SI,			// 僧伽罗语
    YIMLANG_SK,			// 斯洛伐克语
    YIMLANG_SL,			// 斯洛语尼亚语
    YIMLANG_SM,			// 萨摩亚
    YIMLANG_SN,			// 修纳
    YIMLANG_SO,			// 索马里
    YIMLANG_SQ,			// 阿尔巴尼亚语
    YIMLANG_SR,			// 塞尔维亚语
    YIMLANG_ST,			// 塞索托语
    YIMLANG_SU,			// 巽他语
    YIMLANG_SV,			// 瑞典语
    YIMLANG_SV_SE,			// 瑞典语+瑞典
    YIMLANG_SW,			// 斯瓦希里语
    YIMLANG_TA,			// 泰米尔
    YIMLANG_TE,			// 泰卢固语
    YIMLANG_TG,			// 塔吉克斯坦
    YIMLANG_TH,         // 泰语
    YIMLANG_TL,			// 菲律宾
    YIMLANG_TR,			// 土耳其语
    YIMLANG_UK,			// 乌克兰语
    YIMLANG_UR,			// 乌尔都语
    YIMLANG_UZ,			// 乌兹别克斯坦
    YIMLANG_VI,			// 越南
    YIMLANG_XH,			// 科萨
    YIMLANG_YID,			// 意第绪语
    YIMLANG_YO,			// 约鲁巴语
    YIMLANG_ZH,			// 汉语
    YIMLANG_ZH_TW,         // 繁体
    YIMLANG_ZU			// 祖鲁语
}LanguageCodeOC;



/************************************************************************/
/*                              地理位置接口							*/
/************************************************************************/

typedef enum
{
    YouMeDISTRICT_UNKNOW,
    YouMeDISTRICT_COUNTRY,	// 国家
    YouMeDISTRICT_PROVINCE,	// 省份
    YouMeDISTRICT_CITY,		// 市
    YouMeDISTRICT_COUNTY,	// 区县
    YouMeDISTRICT_STREET		// 街道
}YouMeDistrictLevelOC;

typedef enum
{
    YIMForbidReason_Unkown = 0,         //未知
    YIMForbidReason_AD      = 1,        //发广告
    YIMForbidReason_Insult  = 2,        //侮辱
    YIMForbidReason_Politics  = 3,      //政治敏感
    YIMForbidReason_Terrorism  = 4,     //恐怖主义
    YIMForbidReason_Reaction  = 5,      //反动
    YIMForbidReason_Sexy  = 6,          //色情
    YIMForbidReason_Other  = 7,         //其他
}YIMForbidSpeakReasonOC;




//所有消息的基类
@interface YIMMessageBodyBase : NSObject

@property (nonatomic, assign) YIMMessageBodyTypeOC messageType;

@end


//文本消息
@interface YIMMessageBodyText : YIMMessageBodyBase

@property (nonatomic, retain) NSString* messageContent;
@property (nonatomic, retain) NSString* attachParam;

@end

//语音消息
@interface YIMMessageBodyAudio : YIMMessageBodyBase
//语音翻译文字
@property (nonatomic, retain) NSString* textContent;
//发送语音附加信息（StopAudioMessage传入，格式及如何解析由调用方自定）
@property (nonatomic, retain) NSString* customContent;
//语音大小（单位：字节）
@property (nonatomic, assign) unsigned int  fileSize;
//语音时长（单位：秒）
@property (nonatomic, assign) unsigned int  audioTime;

@end

//自定义消息
@interface YIMMessageBodyCustom : YIMMessageBodyBase

@property (nonatomic, retain) NSData* messageContent;

@end

//文件消息
@interface YIMMessageBodyFile : YIMMessageBodyBase
//文件大小（单位：字节）
@property (nonatomic, assign) unsigned int  fileSize;
//原文件名
@property (nonatomic, retain) NSString* fileName;
//文件扩展名
@property (nonatomic, retain) NSString* fileExtension;
//文件类型
@property (nonatomic, assign) YIMFileTypeOC  fileType;
//发送文件附加信息（SendFile传入，格式及如何解析由调用方自定）
@property (nonatomic, retain) NSString* extraParam;

@end

//礼物
@interface YIMMessageBodyGift : YIMMessageBodyBase
//礼物ID
@property (nonatomic, assign) int giftID;
//数量
@property (nonatomic, assign) unsigned int giftCount;
//主播
@property (nonatomic, retain) NSString* anchor;
//附加字段
@property (nonatomic, retain) NSString* extraParam;

@end


//完整消息
@interface YIMMessage: NSObject
//消息ID
@property (nonatomic, assign) unsigned long long messageID;
//聊天类型
@property (nonatomic, assign) YIMChatTypeOC chatType;
//接收者(聊天室：频道ID)
@property (nonatomic, retain) NSString* receiverID;
//发送者
@property (nonatomic, retain) NSString* senderID;
//消息体
@property (nonatomic, retain) YIMMessageBodyBase* messageBody;
//发送时间（秒）
@property (nonatomic, assign) unsigned int createTime;
//距离
@property (nonatomic, assign) int distance;

@property (nonatomic, assign) BOOL isRead;


@end

@interface AudioSpeechInfo: NSObject
//requestID(StartAudioSpeech返回)
@property (nonatomic, assign) unsigned long long requestID;
//语音翻译文字
@property (nonatomic, retain) NSString* textContent;
//语音大小（单位：字节）
@property (nonatomic, assign) unsigned int  fileSize;
//语音时长（单位：秒）
@property (nonatomic, assign) unsigned int  audioTime;
//语音下载URL
@property (nonatomic, retain) NSString*  downloadURL;
//语音本地路径
@property (nonatomic, retain) NSString*  localPath;

@end

//地理位置信息
@interface GeographyLocationOC: NSObject
// 经度
@property (nonatomic, assign) double longitude;
// 纬度
@property (nonatomic, assign) double latitude;
// 行政区域编码
@property (nonatomic, assign) unsigned int  districtCode;
// 国家
@property (nonatomic, retain) NSString* country;
// 省份
@property (nonatomic, retain) NSString* province;
// 城市
@property (nonatomic, retain) NSString* city;
// 区县
@property (nonatomic, retain) NSString* districtCounty;
// 区县
@property (nonatomic, retain) NSString* street;

@end

// 相对地理位置信息
@interface RelativeLocationOC: NSObject

// UserID
@property (nonatomic, retain) NSString* userID;
// 与对方的距离(千米)
@property (nonatomic, assign) unsigned int distance;
// 经度
@property (nonatomic, assign) double longitude;
// 纬度
@property (nonatomic, assign) double latitude;
// 国家
@property (nonatomic, retain) NSString* country;
// 省份
@property (nonatomic, retain) NSString* province;
// 城市
@property (nonatomic, retain) NSString* city;
// 区县
@property (nonatomic, retain) NSString* districtCounty;
// 区县
@property (nonatomic, retain) NSString* street;

@end


typedef enum
{
    YIMSPEECHLANG_MANDARIN,     // 普通话(Android IOS Windows)
    YIMSPEECHLANG_YUEYU,		// 粤语(Android IOS Windows)
    YIMSPEECHLANG_SICHUAN,		// 四川话(Android IOS)
    YIMSPEECHLANG_HENAN,		// 河南话(IOS)
    YIMSPEECHLANG_ENGLISH,		// 英语(Android IOS Windows)
    YIMSPEECHLANG_TRADITIONAL	// 繁体中文(Android IOS Windows)
} SpeechLanguageOC;

typedef enum
{
    YIMSTATUS_AVAILABLE,		// 可用
    YIMSTATUS_NO_AUTHORITY,     // 无权限
    YIMSTATUS_MUTE,             // 静音
    YIMSTATUS_UNAVAILABLE		// 不可用
}AudioDeviceStatusOC;

// 举报处理结果
typedef enum
{
    YIMACCUSATIONRESULT_IGNORE,			// 忽略
    YIMACCUSATIONRESULT_WARNING,		// 警告
    YIMACCUSATIONRESULT_FROBIDDEN_SPEAK	// 禁言
}AccusationDealResultOC;

// 网络重连结果
typedef enum
{
    YIMReconnectResult_Success = 0,       // 重连成功
    YIMReconnectResult_Fail_Again = 1,    // 重连失败，再次重连
    YIMReconnectResult_Fail = 2           // 重连失败
}ReconnectResultOC;

@interface YIMNoticeOC : NSObject

// 公告id
@property (nonatomic, assign) unsigned long long noticeID;
// 公告类型
@property (nonatomic, assign) int noticeType;
// 频道id
@property (nonatomic, retain) NSString* channelID;
// 公告内容
@property (nonatomic, retain) NSString* content;
// 链接关键字
@property (nonatomic, retain) NSString* linkeText;
// 链接地址
@property (nonatomic, retain) NSString* linkAddr;
// 起始时间(置顶公告)
@property (nonatomic, assign) unsigned int beginTime;
// 结束时间(置顶公告)
@property (nonatomic, assign) unsigned int endTime;

@end

@interface ForbiddenSpeakInfoOC : NSObject
// 频道id
@property (nonatomic, retain) NSString* channelID;
// 是否禁止全频道
@property (nonatomic, assign) bool isForbidRoom;
// 禁言原因类型
@property (nonatomic, assign) int reasonType;
// 禁言结束时间
@property (nonatomic, assign) unsigned long long endTime;

@end


//联系人会话信息
@interface YIMContactSessionInfoOC: NSObject

//联系人ID
@property (nonatomic, retain) NSString* contactID;
//消息类型
@property (nonatomic, assign) YIMMessageBodyTypeOC messageType;
//消息内容
@property (nonatomic, retain) NSString* messageContent;
//会话时间（秒）
@property (nonatomic, assign) unsigned int createTime;
//未读消息数量
@property (nonatomic, assign) unsigned int notReadMsgNum;

@end

//消息ID信息
@interface MessageIDRecordOC : NSObject
//消息ID
@property(nonatomic ,assign)long messageID;

@end


//typedef enum
//{
//    IM_USER_STATUS_ONLINE,	  //在线
//    IM_USER_STATUS_OFFLINE,  //离线
//    IM_USER_STATUS_INVISIBLE //隐身
//}IMUserStatusOC;

typedef enum
{
    IM_SEX_UNKNOWN, //未知性别
    IM_SEX_MALE,    //男性
    IM_SEX_FEMALE   //女性
}IMUserSexOC;

typedef enum
{
    IM_NOT_ALLOW_ADD,     //不允许被添加
    IM_NEED_VALIDATE,     //需要验证
    IM_NO_ADD_PERMISSION  //允许被添加，不需要验证
}IMUserBeAddPermissionOC;

typedef enum
{
    IM_CAN_BE_FOUND,        //能被其它用户查找到
    IM_CAN_NOT_BE_FOUND     //不能被其它用户查找到
}IMUserFoundPermissionOC;

//用户基本资料
@interface IMUserSettingInfoOC : NSObject
//昵称
@property (nonatomic, retain) NSString* nickName;
//性别
@property (nonatomic, assign) IMUserSexOC sex;
//个性签名
@property (nonatomic, retain) NSString* personalSignature;
//国家
@property (nonatomic, retain) NSString* country;
//省份
@property (nonatomic, retain) NSString* province;
//城市
@property (nonatomic, retain) NSString* city;
//扩展信息
@property (nonatomic, retain) NSString* extraInfo;

@end

@interface IMUserProfileInfoOC : NSObject
//用户ID
@property (nonatomic, retain) NSString* userID;
//头像url
@property (nonatomic, retain) NSString* photoURL;
//在线状态
@property (nonatomic, assign) YIMUserStatusOC onlineState;
//被好友添加权限
@property (nonatomic, assign) IMUserBeAddPermissionOC beAddPermission;
//能否被查找权限
@property (nonatomic, assign) IMUserFoundPermissionOC foundPermission;
//用户基本信息
@property (nonatomic, retain) IMUserSettingInfoOC* settingInfo;

@end

typedef enum
{
    YIMSTATUS_ADD_SUCCESS = 0,			// 添加完成
    YIMSTATUS_ADD_FAILED = 1,			// 添加失败
    YIMSTATUS_WAIT_OTHER_VALIDATE = 2,	// 等待对方验证
    YIMSTATUS_WAIT_ME_VALIDATE = 3		// 等待我验证
}YIMAddFriendStatusOC;

@interface IMUserBriefInfoOC : NSObject
//用户ID
@property (nonatomic, retain) NSString* userID;
//用户昵称
@property (nonatomic, retain) NSString* nickName;
//用户状态
@property (nonatomic, assign) YIMUserStatusOC userStatus;

@end


@interface IMFriendRequestInfoOC : NSObject
//请求方ID
@property (nonatomic, retain) NSString* askerID;
//请求方昵称
@property (nonatomic, retain) NSString* askerNickname;
//受邀方
@property (nonatomic, retain) NSString* inviteeID;
//受邀方昵称
@property (nonatomic, retain) NSString* inviteeNickname;
//验证信息
@property (nonatomic, retain) NSString* validateInfo;
//状态
@property (nonatomic, assign) YIMAddFriendStatusOC status;
// 时间
@property (nonatomic, assign) unsigned int createTime;

@end


#endif /* YoumeIMMessage_h */
