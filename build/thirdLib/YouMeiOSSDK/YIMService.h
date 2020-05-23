//
//  YIMService.m
//  YoumeIMUILib
//
//  Created by 余俊澎 on 16/8/11.
//  Copyright © 2016年 余俊澎. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "YIMCallbackProtocol.h"

#define YOUME_IM_NEW_MESSSAGE_NOTIFI  @"im.youme.YOUME_IM_NEW_MESSSAGE_NOTIFI"
#define YOUME_IM_AVATAR_CLICK_NOTIFI  @"im.youme.YOUME_IM_AVATAR_CLICK_NOTIFI"
#define YOUME_IM_CLOSE_NOTIFI  @"im.youme.YOUME_IM_CLOSE_NOTIFI"
#define YOUME_IM_SEND_MESSAGE_NOTIFI  @"im.youme.YOUME_IM_SEND_MESSAGE_NOTIFI"

#define YOUME_IM_PHOTO_SIZE 80

@interface YIMService : NSObject

+(YIMService*)GetInstance;
+(NSString*) filterKeyWorks:(NSString*)originContent  level:(int*) level;
//设置语言缓存目录
+(void) SetAudioCacheDir:(NSString*) path;
//设置服务器区域
+(void) SetServerZone:(YouMeIMServerZoneOC) zone;

-(void)SetDelegate:(id<YIMCallbackProtocol>) outDelegate;
//初始化
-(YIMErrorcodeOC)InitWithAppKey:(NSString *)strAppKey appSecurityKey:(NSString*)strAppSecurity;

-(YIMErrorcodeOC) Login:(NSString *)userName password:(NSString *)password token:(NSString*) token ;
-(YIMErrorcodeOC) JoinChatRoom:(NSString *)roomID;
-(YIMErrorcodeOC) LeaveChatRoom:(NSString *)roomID;
-(YIMErrorcodeOC) LeaveAllChatRooms;
-(YIMErrorcodeOC) GetRoomMemberCount:(NSString *)roomID;


//发送文本消息
-(YIMErrorcodeOC) SendTextMessage:(NSString *)receiverID chatType:(YIMChatTypeOC)chatType msgContent:(NSString *) msgContent attachParam:(NSString *)attachParam requestID:(unsigned long long *)requestID;
//发送语音消息（语音转文字）
-(YIMErrorcodeOC) StartRecordAudioMessage:(NSString *)receiverID chatType:(YIMChatTypeOC)chatType requestID:(unsigned long long *)requestID;
//发送语音消息（语音不转文字）
-(YIMErrorcodeOC) StartOnlyRecordAudioMessage:(NSString *)receiverID chatType:(YIMChatTypeOC)chatType requestID:(unsigned long long *)requestID;
//停止并发送语音
-(YIMErrorcodeOC) StopAndSendAudioMessage:(NSString *)attachMsg;
//取消语音
-(YIMErrorcodeOC) CancleAudioMessage;

//NSArray<NSString*>
-(YIMErrorcodeOC) MultiSendTextMessage:( NSArray * )receivers  text:(NSString *) text;

//发送自定义消息
-(YIMErrorcodeOC) SendCustomMessage:(NSString*) receiverID chatType:(YIMChatTypeOC)chatType content:(NSData*)content requestID:(unsigned long long *)requestID;

//发送文件
-(YIMErrorcodeOC) SendFile:(NSString*) receiverID chatType:(YIMChatTypeOC)chatType filePath:(NSString*)filePath requestID:(unsigned long long *)requestID extraParam:(NSString*)extraParam fileType:(YIMFileTypeOC)fileType;

//extraParam:附加参数 格式为json {"nickname":"","server_area":"","location":"","score":"","level":"","vip_level":"","extra":""}
-(YIMErrorcodeOC) SendGift:(NSString*)anchorID channel:(NSString*)channel giftId:(int)giftId giftCount:(int)giftCount extraParam:(NSString*) extraParam requestID:(unsigned long long *)requestID ;

//查询消息记录(direction 查询方向 0：向前查找	1：向后查找)
-(YIMErrorcodeOC) QueryHistoryMessage:(NSString*) targetID chatType:(YIMChatTypeOC) chatType startMessageID:(unsigned long long)startMessageID count:(int)count direction:(int)direction;

/*清理消息记录
	YIMChatType:私聊消息、房间消息
	time：删除指定时间之前的消息*/
-(YIMErrorcodeOC) DeleteHistoryMessage:(YIMChatTypeOC)chatType time:(unsigned long long)time;

//删除指定messageID对应消息
-(YIMErrorcodeOC) DeleteHistoryMessageByID:(unsigned long long)messageID;

//删除指定用户的本地消息历史记录，保留指定的消息ID列表记录
- (YIMErrorcodeOC) DeleteSpecifiedHistoryMessage:(NSString*)targetID chatType:(YIMChatTypeOC)chatType excludeMesList:(NSMutableArray *)excludeMesList;

//查询房间历史消息(房间最近N条聊天记录)
-(YIMErrorcodeOC) QueryRoomHistoryMessageFromServer:(NSString*)roomID count:(int)count direction:(int)direction;

//开始语音（不通过游密发送该语音消息，由调用方发送，调用StopAudioSpeech完成语音及上传后会回调OnStopAudioSpeechStatus）
-(YIMErrorcodeOC) StartAudioSpeech:(unsigned long long *)requestID translate:(bool)translate;

//停止语音（不通过游密发送该语音消息，由调用方发送，完成语音及上传后会回调OnStopAudioSpeechStatus）
-(YIMErrorcodeOC) StopAudioSpeech;
//转换AMR格式到WAV格式
-(YIMErrorcodeOC) ConvertAMRToWav:(NSString*)amrFilePath wavFielPath:(NSString*)wavFielPath;

//是否自动下载语音消息(true: 自动下载   false: 不自动下载(默认))，下载的路径是默认路径，下载回调中可以得到
-(YIMErrorcodeOC) SetDownloadAudioMessageSwitch:(bool)download;

//是否自动接收消息(true:自动接收(默认)	false:不自动接收消息，有新消息达到时，SDK会发出OnReceiveMessageNotify回调，调用方需要调用GetMessage获取新消息)
//targets:房间ID(NSArray<NSString*>)
-(YIMErrorcodeOC) SetReceiveMessageSwitch:( NSArray * )targets receive:(bool)receive;
//获取新消息（只有SetReceiveMessageSwitch设置为不自动接收消息，才需要在收到OnReceiveMessageNotify回调时调用该函数）
//targets:房间ID(NSArray<NSString*>)
-(YIMErrorcodeOC) GetNewMessage:( NSArray * )targets ;
//是否保存房间消息记录（默认不保存）
//targets:房间ID(NSArray<NSString*>)
-(YIMErrorcodeOC) SetRoomHistoryMessageSwitch:( NSArray * )targets save:(bool)save;

// 文本翻译
-(YIMErrorcodeOC) TranslateText:(unsigned int*) requestID text:(NSString*)text  destLangCode:(LanguageCodeOC) destLangCode srcLangCode:(LanguageCodeOC) srcLangCode;

//屏蔽/解除屏蔽用户消息
-(YIMErrorcodeOC) BlockUser:(NSString*) userID block:(bool) block;

//解除所有已屏蔽用户
-(YIMErrorcodeOC) UnBlockAllUser;

//获取被屏蔽消息用户
-(YIMErrorcodeOC) GetBlockUsers;

//消息传输类型开关
-(YIMErrorcodeOC) SwitchMsgTransType:(YIMMsgTransTypeOC) transType;

//获取最近联系人(最大100条)
-(YIMErrorcodeOC) GetRecentContacts;
//设置用户信息
//格式为json {"nickname":"","server_area":"","location":"","level":"","vip_level":""} (以上五个必填，可以添加其他字段)
-(YIMErrorcodeOC) SetUserInfo:(NSString*) userInfo;
//查询用户信息
-(YIMErrorcodeOC) GetUserInfo:( NSString * )userID;
//查询用户在线状态
-(YIMErrorcodeOC) QueryUserStatus:( NSString * )userID;

//程序切到后台运行
-(void) OnPause:(bool)pauseReceiveMessage;
//程序切到前台运行
-(void) OnResume;

//设置播放语音音量(volume:0.0-1.0)
- (void) SetVolume:(float) volume;
//播放语音
- (YIMErrorcodeOC) StartPlayAudio:(NSString* ) path;
//停止语音播放
- (YIMErrorcodeOC) StopPlayAudio;
//查询播放状态
- (bool) IsPlaying;

//获取语音缓存目录
- (NSString*) GetAudioCachePath;
//清理语音缓存目录(注意清空语音缓存目录后历史记录中会无法读取到音频文件，调用清理历史记录接口也会自动删除对应的音频缓存文件)
- (bool) ClearAudioCachePath;


-(YIMErrorcodeOC) Logout;

//下载语音或者文件
-(YIMErrorcodeOC) DownloadAudio:(unsigned long long) ulSerial  strSavePath:(NSString*)strSavePath;
-(YIMErrorcodeOC) DownloadFileByUrl:(NSString*) downloadURL  strSavePath:(NSString*)strSavePath fileType:(YIMFileTypeOC)fileType;

// 获取当前地理位置
-(YIMErrorcodeOC) GetCurrentLocation;
// 获取附近的目标	count:获取数量（一次最大200） serverAreaID：区服	districtlevel：行政区划等级		resetStartDistance：是否重置查找起始距离
-(YIMErrorcodeOC) GetNearbyObjects:(int) count  serverAreaID:(NSString*)serverAreaID districtlevel:(YouMeDistrictLevelOC)districtlevel resetStartDistance:(bool)resetStartDistance;

-(YIMErrorcodeOC) GetDistance:(NSString*)userID;

// 设置位置更新间隔(单位：分钟)
-(void) SetUpdateInterval:(unsigned int) interval;

// 获取麦克风状态
- (void) GetMicrophoneStatus;

// 查询公告
- (YIMErrorcodeOC) QueryNotice;

//设置语音识别语言
- (YIMErrorcodeOC) SetSpeechRecognizeLanguage:(SpeechLanguageOC) language;

//设置仅识别语音文字，不发送语音消息; false:识别语音文字并发送语音消息，true:仅识别语音文字
- (YIMErrorcodeOC) SetOnlyRecognizeSpeechText:(bool)recognition;

//举报
// source:举报来源	reason:举报原因		description：举报详细描写	extraParam：附加参数（JSON格式 {"nickname":"","server_area":"","level":"","vip_level":""}）
- (YIMErrorcodeOC) Accusation:(NSString*)userID source:(YIMChatTypeOC)source reason:(int)reason description:(NSString*)description extraParam:(NSString*)extraParam;

- (YIMErrorcodeOC) GetForbiddenSpeakInfo;

- (YIMErrorcodeOC) SetDownloadDir:(NSString*) path;

- (YIMErrorcodeOC) SetMessageRead:(unsigned long long)msgID read:(bool)read;

//用户信息管理
// 设置用户信息 userInfo(昵称，性别，个性签名，国家，省份，城市，扩展信息)
- (YIMErrorcodeOC) SetUserProfileInfo:(IMUserSettingInfoOC*) userInfo;

// 查询用户信息 userID为“”时查询自己的信息
- (YIMErrorcodeOC) GetUserProfileInfo:(NSString*) userID;

- (UIImage *)ReSizeImage:(UIImage *)image toSize:(CGSize)resize;

// 设置用户头像 photoPath - 本地图片的绝对路径
- (YIMErrorcodeOC) SetUserProfilePhoto:(NSString*) photoPath;

// 切换用户在线状态  userStatus，0-在线 1-离线 2-隐身
- (YIMErrorcodeOC) SwitchUserStatus:(NSString*) userID  userStatus:(YIMUserStatusOC) userStatus;

// 设置被添加权限 beFound，true-能被查找，false-不能被查找
//  beAddPermission, 0-不允许被添加, 1-需要验证, 2-允许被添加，不需要验证
- (YIMErrorcodeOC) SetAddPermission:(bool) beFound beAddPermission:(IMUserBeAddPermissionOC) beAddPermission;

/*************  好友接口  *****************/
/*
	* 功能：查找添加好友（获取用户简要信息）
	* @param findType：查找类型	0：按ID查找	1：按昵称查找
	* @param target：对应查找类型用户ID或昵称
	* @return 错误码
	*/
- (YIMErrorcodeOC) FindUser:(int) findType target:(NSString*) target;

/*
	* 功能：添加好友
	* @param userID：用户ID(NSArray<NSString*>)
	* @param comments：备注或验证信息(长度最大128)
	* @return 错误码
	*/
- (YIMErrorcodeOC) RequestAddFriend:(NSArray *) users comments:(NSString*) comments;

/*
	* 功能：处理被请求添加好友
	* @param userID：用户ID
	* @param dealResult：处理结果	0：同意	1：拒绝
	* @return 错误码
	*/
- (YIMErrorcodeOC) DealBeRequestAddFriend:(NSString*) userID dealResult:(int) dealResult reqID:(unsigned long long)reqID;

/*
	* 功能：删除好友
	* @param users：用户ID(NSArray<NSString*>)
	* @param deleteType：删除类型	0：双向删除	1：单向删除(删除方在被删除方好友列表依然存在)
	* @return 错误码
	*/
- (YIMErrorcodeOC) DeleteFriend:(NSArray *) users deleteType:(int) deleteType;

/*
	* 功能：拉黑好友
	* @param type：0：拉黑	1：解除拉黑
	* @param users：用户ID(NSArray<NSString*>)
	* @return 错误码
	*/
- (YIMErrorcodeOC) BlackFriend:(int) type users:(NSArray *) users;

/*
	* 功能：查询我的好友
	* @param type：0：正常好友	1：被拉黑好友
	* @param startIndex：起始序号
	* @param count：数量（一次最大100）
	* @return 错误码
	*/
- (YIMErrorcodeOC) QueryFriends:(int) type startIndex:(int) startIndex count:(int) count;

/*
	* 功能：查询好友请求列表
	* @param startIndex：起始序号
	* @param count：数量（一次最大20）
	* @return 错误码
	*/
- (YIMErrorcodeOC) QueryFriendRequestList:(int) startIndex count:(int) count;

-(void)RebindIMCallback;

@end
