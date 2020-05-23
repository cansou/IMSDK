//
//  YoumeIMCallbackProtocal.h
//  YoumeIMUILib
//
//  Created by 余俊澎 on 16/8/11.
//  Copyright © 2016年 余俊澎. All rights reserved.
//
#import "YIMMessage.h"

#ifndef YoumeIMCallbackProtocal_h
#define YoumeIMCallbackProtocal_h

//对外的错误码
typedef enum
{
    //这上面是服务器的，要和服务器的一致，如果这里没有的话，会强制转换成一个数字
    YouMeIMCode_Success = 0,
    YouMeIMCode_EngineNotInit = 1,
    YouMeIMCode_NotLogin = 2,
    YouMeIMCode_ParamInvalid = 3,
    YouMeIMCode_TimeOut = 4,
    YouMeIMCode_StatusError = 5,
    YouMeIMCode_SDKInvalid = 6,
    YouMeIMCode_AlreadyLogin = 7,
    YouMeIMCode_ServerError = 8,
    YouMeIMCode_NetError = 9,
    YouMeIMCode_LoginSessionError = 10,
    YouMeIMCode_NotStartUp = 11,
    YouMeIMCode_FileNotExist = 12,
    YouMeIMCode_SendFileError = 13,
    YouMeIMCode_UploadFailed = 14,
    YouMeIMCode_UsernamePasswordError = 15,
    YouMeIMCode_UserStatusError = 16,   //用户状态错误(无效用户)
    YouMeIMCode_MessageTooLong = 17,	//消息太长
    YouMeIMCode_ReceiverTooLong = 18,	//接收方ID过长(检查房间名)
    YouMeIMCode_InvalidChatType = 19,	//无效聊天类型(私聊、聊天室)
    YouMeIMCode_InvalidReceiver = 20,	//无效用户ID
    YouMeIMCode_UnknowError = 21,
    YouMeIMCode_InvalidAppkey = 22,     //无效APPKEY
    YouMeIMCode_ForbiddenSpeak = 23,    //被禁言
    YouMeIMCode_CreateFileFailed = 24,
    YouMeIMCode_UnsupportFormat = 25,			//不支持的文件格式
    YouMeIMCode_ReceiverEmpty = 26,			    //接收方为空
    YouMeIMCode_RoomIDTooLong = 27,			    //房间名太长
    YouMeIMCode_ContentInvalid =28,			    //聊天内容严重非法
    YouMeIMCode_NoLocationAuthrize = 29,		//未打开定位权限
    YouMeIMCode_UnknowLocation = 30,			//未知位置
    YouMeIMCode_Unsupport = 31,				    //不支持该接口
    YouMeIMCode_NoAudioDevice = 32,			    //无音频设备
    YouMeIMCode_AudioDriver = 33,				//音频驱动问题
    YouMeIMCode_DeviceStatusInvalid = 34,		//设备状态错误
    YouMeIMCode_ResolveFileError = 35,			//文件解析错误
    YouMeIMCode_ReadWriteFileError = 36,		//文件读写错误
    YouMeIMCode_NoLangCode = 37,				//语言编码错误
    YouMeIMCode_TranslateUnable = 38,			//翻译接口不可用
    YouMeIMCode_SpeechAccentInvalid = 39,		//语音识别方言无效
    YouMeIMCode_SpeechLanguageInvalid = 40,	    //语音识别语言无效
    YouMeIMCode_HasIllegalText = 41,			//消息含非法字符
    YouMeIMCode_AdvertisementMessage = 42,		//消息涉嫌广告
    YouMeIMCode_AlreadyBlock = 43,				//用户已经被屏蔽
    YouMeIMCode_NotBlock = 44,					//用户未被屏蔽
	YouMeIMCode_MessageBlocked = 45,			//消息被屏蔽
    YouMeIMCode_LocationTimeout = 46,			//定位超时
    YouMeIMCode_NotJoinRoom = 47,				//未加入该房间
    YouMeIMCode_LoginTokenInvalid = 48,		    //登录token错误
    YouMeIMCode_CreateDirectoryFailed = 49,	    //创建目录失败
    
    YouMeIMCode_InitFailed = 50,				//初始化失败
    YouMeIMCode_Disconnect = 51,				//与服务器断开
    
    YouMeIMCode_TheSameParam = 52,             //设置参数相同
    YouMeIMCode_QueryUserInfoFail = 53,        //查询用户信息失败
    YouMeIMCode_SetUserInfoFail = 54,          //设置用户信息失败
    YouMeIMCode_UpdateUserOnlineStateFail = 55,//更新用户在线状态失败
    YouMeIMCode_NickNameTooLong = 56,          //昵称太长(> 64 bytes)
    YouMeIMCode_SignatureTooLong = 57,         //个性签名太长(> 120 bytes)
    YouMeIMCode_NeedFriendVerify = 58,			//需要好友验证信息
    YouMeIMCode_BeRefuse = 59,					//添加好友被拒绝
    YouMeIMCode_HasNotRegisterUserInfo = 60,    //未注册用户信息
    YouMeIMCode_AlreadyFriend = 61,			    //已经是好友
    YouMeIMCode_NotFriend = 62,				    //非好友
    YouMeIMCode_NotBlack = 63,					//不在黑名单中
    YouMeIMCode_PhotoUrlTooLong = 64,           //头像url过长(>500 bytes)
    YouMeIMCode_PhotoSizeTooLarge = 65,         //头像太大（>100 kb）
    
    YouMeIMCode_StopPlayFailBeforeStart = 66,
    
    //服务器的错误码
    YouMeIMCode_ALREADYFRIENDS = 1000,
    YouMeIMCode_LoginInvalid = 1001,
    
    //语音部分错误码
    YouMeIMCode_PTT_Start = 2000,
    YouMeIMCode_PTT_Fail = 2001,
    YouMeIMCode_PTT_DownloadFail = 2002,
    YouMeIMCode_PTT_GetUploadTokenFail = 2003,
    YouMeIMCode_PTT_UploadFail = 2004,
    YouMeIMCode_PTT_NotSpeech = 2005,               //未检测到语音
    YouMeIMCode_PTT_DeviceStatusError = 2006,       //音频设备状态错误
    YouMeIMCode_PTT_IsSpeeching = 2007,             //已开启语音
    YouMeIMCode_PTT_FileNotExist = 2008,
    YouMeIMCode_PTT_ReachMaxDuration = 2009,        //达到语音最大时长限制
    YouMeIMCode_PTT_SpeechTooShort = 2010,          //语音时长太短
    YouMeIMCode_PTT_StartAudioRecordFailed = 2011,  //启动语音失败
    YouMeIMCode_PTT_SpeechTimeout = 2012,           //音频输入超时
    YouMeIMCode_PTT_IsPlaying = 2013,				//正在播放
    YouMeIMCode_PTT_NotStartPlay = 2014,			//未开始播放
    YouMeIMCode_PTT_CancelPlay = 2015,				//主动取消播放
    YouMeIMCode_PTT_NotStartRecord = 2016,			//未开始语音
    YouMeIMCode_PTT_NotInit = 2017,				    // 未初始化
    YouMeIMCode_PTT_InitFailed = 2018,				// 初始化失败
    YouMeIMCode_PTT_Authorize = 2019,				// 录音权限
    YouMeIMCode_PTT_StartRecordFailed = 2020,		// 启动录音失败
    YouMeIMCode_PTT_StopRecordFailed = 2021,		// 停止录音失败
    YouMeIMCode_PTT_UnsupprtFormat = 2022,			// 不支持的格式
    YouMeIMCode_PTT_ResolveFileError = 2023,		// 解析文件错误
    YouMeIMCode_PTT_ReadWriteFileError = 2024,		// 读写文件错误
    YouMeIMCode_PTT_ConvertFileFailed = 2025,		// 文件转换失败
    YouMeIMCode_PTT_NoAudioDevice = 2026,			// 无音频设备
    YouMeIMCode_PTT_NoDriver = 2027,				// 驱动问题
    YouMeIMCode_PTT_StartPlayFailed = 2028,		    // 启动播放失败
    YouMeIMCode_PTT_StopPlayFailed = 2029,			// 停止播放失败
    
    YouMeIMCode_Fail = 10000
}YIMErrorcodeOC;

/************************************************************************/
/*                              服务器区域编码                          */
/************************************************************************/
typedef enum
{
    YouMeIMServerZone_China = 0,		// 中国
    YouMeIMServerZone_Singapore = 1,	// 新加坡
    YouMeIMServerZone_America = 2,		// 美国
    YouMeIMServerZone_HongKong = 3,	// 香港
    YouMeIMServerZone_Korea = 4,		// 韩国
    YouMeIMServerZone_Australia = 5,	// 澳洲
    YouMeIMServerZone_Deutschland = 6,	// 德国
    YouMeIMServerZone_Brazil = 7,		// 巴西
    YouMeIMServerZone_India = 8,		// 印度
    YouMeIMServerZone_Japan = 9,		// 日本
    YouMeIMServerZone_Ireland = 10,	// 爱尔兰
    YouMeIMServerZone_Thailand = 11,	// 泰国
    YouMeIMServerZone = 12,		// 台湾
    
    YouMeIMServerZone_Unknow = 9999
}YouMeIMServerZoneOC;



@protocol YIMCallbackProtocol <NSObject>

// 被踢回调
- (void) OnKickOff;

// 用户进出频道通知
- (void) OnUserJoinRoom:(NSString*)roomId userID:(NSString*)userId;
- (void) OnUserLeaveRoom:(NSString*)roomId userID:(NSString*)userId;

//收到消息
- (void) OnRecvMessage:(YIMMessage*) pMessage;

//新消息通知（只有SetReceiveMessageSwitch设置为不自动接收消息，才会收到该回调）
- (void) OnReceiveMessageNotify:(YIMChatTypeOC)chatType targetID:(NSString*)targetID;

//接收端消息已读，更新发送端消息显示状态
- (void) OnUpdateReadStatus:(NSString*)recvId chatType:(YIMChatTypeOC)chatType msgSerial:(unsigned long long)msgSerial;

// 音量回调
- (void) OnRecordVolumeChange:(float) volume;

// 举报处理结果通知
- (void)OnAccusationResultNotify:(AccusationDealResultOC)result userID:(NSString*)userID accusationTime:(unsigned int)accusationTime;

//语音消息识别的文字返回
- (void)OnGetRecognizeSpeechText:(unsigned long long) iRequestID errorcode:(YIMErrorcodeOC)errorcode text:(NSString*)text;

//IYIMLocationCallback 地理位置回调
// 获取自己位置回调
- (void) OnUpdateLocation:(YIMErrorcodeOC) errorcode  location:(GeographyLocationOC*) location;

// 接收公告
- (void) OnRecvNotice:(YIMNoticeOC*) notice;
// 撤销公告（置顶公告）
- (void) OnCancelNotice:(unsigned long long) noticeID channelID:(NSString*)channelID;

// IYIMReconnectCallback 重连回调
// 开始重连
- (void) OnStartReconnect;
// 收到重连结果
- (void) OnRecvReconnectResult:(ReconnectResultOC)result;

/*
 * 功能：用户资料变更通知
 * @param userID：用户ID
 * commonts：根据需要决定是否重新获取用户信息
 */
- (void) OnUserInfoChangeNotify:(NSString*) userID;

/*
 * 功能：被邀请添加好友通知（需要验证）
 * @param userID：用户ID
 * @param comments：备注或验证信息
 * commonts：显示用户信息可以根据userID查询
 */
- (void) OnBeRequestAddFriendNotify:(NSString*) userID comments:(NSString*) comments reqID:(unsigned long long)reqID;

/*
 * 功能：被添加为好友通知（不需要验证）
 * @param userID：用户ID
 * @param comments：备注或验证信息
 */
- (void) OnBeAddFriendNotify:(NSString*) userID comments:(NSString*) comments;

/*
 * 功能：请求添加好友结果通知(需要好友验证，待被请求方处理后回调)
 * @param userID：用户ID
 * @param comments：备注或验证信息
 * @param dealResult：处理结果    0：同意    1：拒绝
 */
- (void) OnRequestAddFriendResultNotify:(NSString*) userID comments:(NSString*) comments dealResult:(int) dealResult;

/*
 * 功能：被好友删除通知（双向删除时才能收到）
 * @param userID：用户ID
 */
- (void) OnBeDeleteFriendNotify:(NSString*) userID;


/***********  非必须实现的回调接口  **********/

// 登录 登出 回调
- (void) OnLogin:(YIMErrorcodeOC) errorcode userID:(NSString*) userID;
- (void) OnLogout:(YIMErrorcodeOC) errorcode;

//IYIMMessageCallback,消息相关回调
//发送消息回调
- (void) OnSendMessageStatus:(unsigned long long) iRequestID errorcode:(YIMErrorcodeOC) errorcode sendTime:(unsigned int)sendTime isForbidRoom:(bool) isForbidRoom   reasonType:(int)reasonType forbidEndTime:(unsigned long long)forbidEndTime;

- (void) OnUploadProgress:(unsigned long long) iRequestID percent:(float)percent;
//发送语音消息回调
- (void) OnSendAudioMessageStatus:(unsigned long long) iRequestID errorcode:(YIMErrorcodeOC) errorcode strMessage:(NSString*)strMessage strAudioPath:(NSString*) strAudioPath audioTime:(unsigned int)audioTime sendTime:(unsigned int)sendTime isForbidRoom:(bool) isForbidRoom   reasonType:(int)reasonType forbidEndTime:(unsigned long long)forbidEndTime;
//停止语音回调（调用StopAudioMessage停止语音之后，发送语音消息之前）
- (void) OnStartSendAudioMessage:(unsigned long long) iRequestID errorcode:(YIMErrorcodeOC) errorcode
                       strMessage:(NSString*)strMessage strAudioPath:(NSString*) strAudioPath audioTime:(unsigned int)audioTime ;

//获取消息历史纪录回调NSArray<YIMMessage*>
- (void) OnQueryHistoryMessage:(YIMErrorcodeOC)errorcode targetID:(NSString*)targetID remain:(int) remain messageArray:(NSArray*) messageArray;

- (void) OnQueryRoomHistoryMessageFromServer:(YIMErrorcodeOC)errorcode roomID:(NSString*)roomId remain:(int)remain messageArray:(NSArray*) messageArray;
//语音上传后回调
- (void) OnStopAudioSpeechStatus:(YIMErrorcodeOC)errorcode audioSpeechInfo:(AudioSpeechInfo*) audioSpeechInfo;

//文本翻译完成回调
- (void) OnTranslateTextComplete:(YIMErrorcodeOC) errorcode requestID:(unsigned int) requestID  text:(NSString*)text srcLangCode:(LanguageCodeOC) srcLangCode destLangCode:(LanguageCodeOC) destLangCode;

//屏蔽/解除屏蔽用户消息回调
- (void) OnBlockUser:(YIMErrorcodeOC) errorcode userID:(NSString*)userID block:(bool)block;
//解除所有已屏蔽用户回调
- (void) OnUnBlockAllUser:(YIMErrorcodeOC) errorcode;
//获取被屏蔽消息用户回调
- (void) OnGetBlockUsers:(YIMErrorcodeOC)errorcode userList:(NSArray*) userList;

//IYIMContactCallback获取最近联系人回调
//NSArray<NSString*>
- (void) OnGetRecentContacts:(YIMErrorcodeOC)errorcode contactArray:(NSArray*) contactArray;
//获取用户信息回调(用户信息为JSON格式)
- (void) OnGetUserInfo:(YIMErrorcodeOC)errorcode userID:(NSString*)userID userInfo:(NSString*) userInfo;
//查询用户状态回调
- (void) OnQueryUserStatus:(YIMErrorcodeOC)errorcode userID:(NSString*)userID status:(YIMUserStatusOC) status;

//IYIMDownloadCallback，下载回调
- (void) OnDownload:(YIMErrorcodeOC)errorcode  pMessage:(YIMMessage*) pMessage strSavePath:(NSString*)strSavePath ;

- (void) OnDownloadByUrl:(YIMErrorcodeOC)errorcode strFromUrl:(NSString*)strFromUrl strSavePath:(NSString*)strSavePath;


//频道回调
- (void) OnJoinRoom:(YIMErrorcodeOC) errorcode roomID:(NSString*) roomID;
- (void) OnLeaveRoom:(YIMErrorcodeOC) errorcode roomID:(NSString*) roomID;
- (void) OnLeaveAllRooms:(YIMErrorcodeOC) errorcode;

//获取房间成员数量回调
- (void) OnGetRoomMemberCount:(YIMErrorcodeOC)errorcode chatRoomID:(NSString*)chatRoomID count:(unsigned int)count;


//IYIMAudioPlayCallback 播放回调
- (void) OnPlayCompletion:(YIMErrorcodeOC) errorcode  path:(NSString*) path;
- (void) OnGetMicrophoneStatus:(AudioDeviceStatusOC)status;

// 获取附近目标回调neighbourList:(NSArray<RelativeLocationOC*>)
- (void) OnGetNearbyObjects:(YIMErrorcodeOC) errorcode neighbourList:(NSArray*) neighbourList  startDistance:(unsigned int) startDistance  endDistance:(unsigned int) endDistance ;

- (void) OnGetDistance:(YIMErrorcodeOC)errorcode userID:(NSString*)userID distance:(unsigned int)distance;

- (void) OnGetForbiddenSpeakInfo:(YIMErrorcodeOC)errorcode  forbiddenSpeakArray:(NSArray*) forbiddenSpeakArray;

// IYIMUserProfileCallback 用户信息管理相关回调
// 查询用户信息回调
- (void) OnQueryUserInfo:(YIMErrorcodeOC)errorcode userInfo:(IMUserProfileInfoOC*) userInfo;

// 设置用户信息回调
- (void) OnSetUserInfo:(YIMErrorcodeOC)errorcode;

// 切换用户在线状态回调
- (void) OnSwitchUserOnlineState:(YIMErrorcodeOC)errorcode;

// 设置头像回调
- (void) OnSetPhotoUrl:(YIMErrorcodeOC)errorcode photoUrl:(NSString*)photoUrl;

/*
	* 功能：查找用户回调
	* @param errorcode：错误码
	* @param users：用户简要信息列表 (NSArray<IMUserBriefInfoOC*>)
	*/
- (void) OnFindUser:(YIMErrorcodeOC) errorcode users:(NSArray*)users;

/*
	* 功能：请求添加好友回调
	* @param errorcode：错误码
	* @param userID：用户ID
	*/
- (void) OnRequestAddFriend:(YIMErrorcodeOC) errorcode userID:(NSString*) userID;

/*
	* 功能：处理被请求添加好友回调
	* @param errorcode：错误码
	* @param userID：用户ID
	* @param comments：备注或验证信息
	* @param dealResult：处理结果	0：同意	1：拒绝
	*/
- (void) OnDealBeRequestAddFriend:(YIMErrorcodeOC) errorcode userID:(NSString*) userID comments:(NSString*) comments dealResult:(int) dealResult;

/*
	* 功能：删除好友结果回调
	* @param errorcode：错误码
	* @param userID：用户ID
	*/
- (void) OnDeleteFriend:(YIMErrorcodeOC) errorcode userID:(NSString*) userID;

/*
	* 功能：拉黑或解除拉黑好友回调
	* @param errorcode：错误码
	* @param type：0：拉黑	1：解除拉黑
	* @param userID：用户ID
	*/
- (void) OnBlackFriend:(YIMErrorcodeOC) errorcode type:(int) type userID:(NSString*) userID;

/*
	* 功能：查询我的好友回调
	* @param errorcode：错误码
	* @param type：0：正常好友	1：被拉黑好友
	* @param startIndex：起始序号
	* @param hasMore：是否还有更多数据
	* @param friends：好友列表(NSArray<IMUserBriefInfoOC>)
	*/
- (void) OnQueryFriends:(YIMErrorcodeOC) errorcode type:(int) type startIndex:(int) startIndex friends:(NSArray*) friends;

/*
	* 功能：查询好友请求列表回调
	* @param errorcode：错误码
	* @param startIndex：起始序号
	* @param hasMore：是否还有更多数据
	* @param validateList：验证消息列表(NSArray<IMFriendRequestInfoOC>)
	*/
- (void) OnQueryFriendRequestList:(YIMErrorcodeOC) errorcode startIndex:(int) startIndex requestList:(NSArray*) requestList;

@end


#endif /* YoumeIMCallbackProtocal_h */
