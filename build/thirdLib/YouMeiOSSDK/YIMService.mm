//
//  YIMService.m
//  YoumeIMUILib
//
//  Created by 余俊澎 on 16/8/11.
//  Copyright © 2016年 余俊澎. All rights reserved.
//

#import "YIMService.h"
#import "YIMImplement.h"

@implementation YIMService

+(YIMService*)GetInstance{
    static YIMService *_instance = nil;
    @synchronized(self)
    {
        if (_instance == nil)
        {
            _instance = [[self alloc] init];
        }
    }
    return _instance;
}

+(NSString*) filterKeyWorks:(NSString*)originContent level:(int*) level {
    XString s = YIMManager::FilterKeyword([originContent UTF8String], level );
    return [NSString stringWithUTF8String:s.c_str()];
}

-(void)SetDelegate:(id<YIMCallbackProtocol>) outDelegate{
    YIMImplement::getInstance()->delegate = outDelegate;
}

-(void)RebindIMCallback{
    YIMImplement::getInstance()->RebindCallback();
}

-(YIMErrorcodeOC)InitWithAppKey:(NSString *)strAppKey appSecurityKey:(NSString*)strAppSecurity{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->Init([strAppKey UTF8String], [strAppSecurity UTF8String], "");
}

-(YIMErrorcodeOC) Login:(NSString *)userName password:(NSString *)password token:(NSString*) token{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->Login([userName UTF8String], [password UTF8String], [token UTF8String] );
}

-(YIMErrorcodeOC) JoinChatRoom:(NSString *)roomID{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetChatRoomManager()->JoinChatRoom([roomID UTF8String]);
}

-(YIMErrorcodeOC) LeaveChatRoom:(NSString *)roomID{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetChatRoomManager()->LeaveChatRoom([roomID UTF8String]);
}

-(YIMErrorcodeOC) LeaveAllChatRooms{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetChatRoomManager()->LeaveAllChatRooms();
}

-(YIMErrorcodeOC) GetRoomMemberCount:(NSString *)roomID{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetChatRoomManager()->GetRoomMemberCount([roomID UTF8String]);
}

-(YIMErrorcodeOC) SendTextMessage:(NSString *)receiverID chatType:(YIMChatTypeOC)chatType msgContent:(NSString *) msgContent attachParam:(NSString *)attachParam requestID:(unsigned long long *)requestID{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->SendTextMessage([receiverID UTF8String], (YIMChatType)chatType, [msgContent UTF8String], [attachParam UTF8String], requestID);
}

-(YIMErrorcodeOC) StartRecordAudioMessage:(NSString *)receiverID chatType:(YIMChatTypeOC)chatType requestID:(unsigned long long *)requestID{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->SendAudioMessage([receiverID UTF8String], (YIMChatType)chatType, requestID);
}

-(YIMErrorcodeOC) StartOnlyRecordAudioMessage:(NSString *)receiverID chatType:(YIMChatTypeOC)chatType requestID:(unsigned long long *)requestID{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->SendOnlyAudioMessage([receiverID UTF8String], (YIMChatType)chatType, requestID);
}

-(YIMErrorcodeOC) StopAndSendAudioMessage:(NSString *)attachMsg{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->StopAudioMessage([attachMsg UTF8String]);
}

-(YIMErrorcodeOC) CancleAudioMessage{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->CancleAudioMessage();
}

-(YIMErrorcodeOC) MultiSendTextMessage:( NSArray * )receivers  text:(NSString *) text{
    std::vector<XString> vecRooms;
    
    for( int i = 0 ; i < receivers.count; i++ ){
        XString roomID = [ receivers[i]  UTF8String ];
        vecRooms.push_back( roomID );
    }
    
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->MultiSendTextMessage( vecRooms, [text UTF8String]);
}

-(YIMErrorcodeOC) SendCustomMessage:(NSString*) receiverID chatType:(YIMChatTypeOC)chatType content:(NSData*)content requestID:(unsigned long long *)requestID{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->SendCustomMessage([receiverID UTF8String], (YIMChatType)chatType, (const char*)content.bytes, content.length, requestID);
}

-(YIMErrorcodeOC) SendFile:(NSString*) receiverID chatType:(YIMChatTypeOC)chatType filePath:(NSString*)filePath requestID:(unsigned long long *)requestID extraParam:(NSString*)extraParam fileType:(YIMFileTypeOC)fileType{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->SendFile([receiverID UTF8String], (YIMChatType)chatType, [filePath UTF8String], requestID, [extraParam UTF8String]);
}

//extraParam:附加参数 格式为json {"nickname":"","server_area":"","location":"","score":"","level":"","vip_level":"","extra":""}
-(YIMErrorcodeOC) SendGift:(NSString*)anchorID channel:(NSString*)channel giftId:(int)giftId giftCount:(int)giftCount extraParam:(NSString*) extraParam requestID:(unsigned long long *)requestID {
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->SendGift([anchorID UTF8String], [channel UTF8String], giftId, giftCount, [extraParam UTF8String], requestID);
}

//查询消息记录(direction 查询方向 0：向前查找	1：向后查找)
-(YIMErrorcodeOC) QueryHistoryMessage:(NSString*) targetID  chatType:(YIMChatTypeOC) chatType startMessageID:(unsigned long long)startMessageID count:(int)count direction:(int)direction{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->QueryHistoryMessage( [targetID UTF8String], (YIMChatType)chatType, startMessageID, count, direction );
}

-(YIMErrorcodeOC) DeleteHistoryMessage:(YIMChatTypeOC)chatType time:(unsigned long long)time{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->DeleteHistoryMessage( (YIMChatType)chatType, time);
}

-(YIMErrorcodeOC) DeleteHistoryMessageByID:(unsigned long long)messageID{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->DeleteHistoryMessageByID( messageID );
}

- (YIMErrorcodeOC) DeleteSpecifiedHistoryMessage:(NSString*)targetID chatType:(YIMChatTypeOC)chatType excludeMesList:(NSMutableArray *)excludeMesList
{
    if (YIMManager::CreateInstance()->GetMessageManager() == NULL) {
        return YouMeIMCode_NotLogin;
    }
    std::vector<XUINT64> vecMes;
    for (MessageIDRecordOC *tmp in excludeMesList)
    {
        XUINT64 messageID = tmp.messageID;
        vecMes.push_back( messageID );
    }
    
    return (YIMErrorcodeOC) YIMManager::CreateInstance()->GetMessageManager()->DeleteSpecifiedHistoryMessage([targetID UTF8String], (YIMChatType)chatType, vecMes);
}

-(YIMErrorcodeOC) QueryRoomHistoryMessageFromServer:(NSString*)roomID count:(int)count direction:(int)direction{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->QueryRoomHistoryMessageFromServer([roomID UTF8String], count, direction);
    
}

-(YIMErrorcodeOC) StartAudioSpeech:(unsigned long long *)requestID translate:(bool)translate{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->StartAudioSpeech(requestID, translate);
}

-(YIMErrorcodeOC) StopAudioSpeech{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->StopAudioSpeech();
}

-(YIMErrorcodeOC) ConvertAMRToWav:(NSString*)amrFilePath wavFielPath:(NSString*)wavFielPath{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->ConvertAMRToWav([amrFilePath UTF8String], [wavFielPath UTF8String]);
}

//是否自动下载语音消息(true: 自动下载语音消息， false:不自动下载语音消息(默认))，下载的路径是默认路径，下载回调中可以得到
-(YIMErrorcodeOC) SetDownloadAudioMessageSwitch:(bool)download{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->SetDownloadAudioMessageSwitch(download);
}

-(YIMErrorcodeOC) SetReceiveMessageSwitch:( NSArray * )targets receive:(bool)receive{
    std::vector<XString> vecRooms;
    
    for( int i = 0 ; i < targets.count; i++ ){
        XString roomID = [ targets[i]  UTF8String ];
        vecRooms.push_back( roomID );
    }
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->SetReceiveMessageSwitch( vecRooms, receive);
}

-(YIMErrorcodeOC) GetNewMessage:( NSArray * )targets {
    std::vector<XString> vecRooms;
    
    for( int i = 0 ; i < targets.count; i++ ){
        XString roomID = [ targets[i]  UTF8String ];
        vecRooms.push_back( roomID );
    }
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->GetNewMessage(vecRooms);
}

-(YIMErrorcodeOC) SetRoomHistoryMessageSwitch:( NSArray * )targets save:(bool)save{
    std::vector<XString> vecRooms;
    
    for( int i = 0 ; i < targets.count; i++ ){
        XString roomID = [ targets[i]  UTF8String ];
        vecRooms.push_back( roomID );
    }
    
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->SetRoomHistoryMessageSwitch( vecRooms, save);
}

-(YIMErrorcodeOC) TranslateText:(unsigned int*) requestID text:(NSString*)text  destLangCode:(LanguageCodeOC) destLangCode srcLangCode:(LanguageCodeOC) srcLangCode {
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->TranslateText( requestID, [text UTF8String], (LanguageCode)destLangCode, (LanguageCode) srcLangCode );
}

-(YIMErrorcodeOC) BlockUser:(NSString*) userID block:(bool) block{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->BlockUser([userID UTF8String], block);
}

//解除所有已屏蔽用户
-(YIMErrorcodeOC) UnBlockAllUser{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->UnBlockAllUser();
}

//获取被屏蔽消息用户
-(YIMErrorcodeOC) GetBlockUsers{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->GetBlockUsers();
}

-(YIMErrorcodeOC) GetRecentContacts{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetRecentContacts();
}

-(YIMErrorcodeOC) SetUserInfo:(NSString*) userInfo{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->SetUserInfo([userInfo UTF8String]);
}

-(YIMErrorcodeOC) GetUserInfo:( NSString * )userID{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetUserInfo([userID UTF8String]);
}

-(YIMErrorcodeOC) QueryUserStatus:( NSString * )userID{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->QueryUserStatus([userID UTF8String]);

}


//程序切到后台运行
-(void) OnPause:(bool) pauseReceiveMessage{
    YIMManager::CreateInstance()->OnPause(pauseReceiveMessage);
}
//程序切到前台运行
-(void) OnResume{
    YIMManager::CreateInstance()->OnResume();
}

//设置播放语音音量(volume:0.0-1.0)
- (void) SetVolume:(float) volume{
    YIMManager::CreateInstance()->SetVolume( volume );
}
//播放语音
- (YIMErrorcodeOC) StartPlayAudio:(NSString* ) path{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->StartPlayAudio( [path UTF8String] );
}
//停止语音播放
- (YIMErrorcodeOC) StopPlayAudio{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->StopPlayAudio();
}
//查询播放状态
- (bool) IsPlaying{
    return YIMManager::CreateInstance()->IsPlaying();
}

//获取语音缓存目录
- (NSString*) GetAudioCachePath{
    XString s = YIMManager::CreateInstance()->GetAudioCachePath();
    return [NSString stringWithUTF8String:s.c_str()];
}
//清理语音缓存目录(注意清空语音缓存目录后历史记录中会无法读取到音频文件，调用清理历史记录接口也会自动删除对应的音频缓存文件)
- (bool) ClearAudioCachePath{
    return YIMManager::CreateInstance()->ClearAudioCachePath();
}


-(YIMErrorcodeOC) Logout{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->Logout();
}

-(YIMErrorcodeOC) DownloadAudio:(unsigned long long) messageID  strSavePath:(NSString*)strSavePath{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->DownloadFile(messageID, [strSavePath UTF8String]);
}

-(YIMErrorcodeOC) DownloadFileByUrl:(NSString*) downloadURL  strSavePath:(NSString*)strSavePath fileType:(YIMFileTypeOC)fileType{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->DownloadFile([downloadURL UTF8String], [strSavePath UTF8String], (YIMFileType)fileType);
}

+(void) SetAudioCacheDir:(NSString*) path{
    YIMManager::CreateInstance()->SetAudioCacheDir([path UTF8String]);
}

+(void) SetServerZone:(YouMeIMServerZoneOC) zone{
    YIMManager::CreateInstance()->SetServerZone((ServerZone)(int)zone);
}

// 获取当前地理位置
-(YIMErrorcodeOC) GetCurrentLocation{
    return  (YIMErrorcodeOC)YIMManager::CreateInstance()->GetLocationManager()->GetCurrentLocation();
}
// 获取附近的目标	count:获取数量（一次最大200） serverAreaID：区服	districtlevel：行政区划等级		resetStartDistance：是否重置查找起始距离
-(YIMErrorcodeOC) GetNearbyObjects:(int) count  serverAreaID:(NSString*)serverAreaID districtlevel:(YouMeDistrictLevelOC)districtlevel resetStartDistance:(bool)resetStartDistance{
    return  (YIMErrorcodeOC)YIMManager::CreateInstance()->GetLocationManager()->GetNearbyObjects( count, [serverAreaID UTF8String], (DistrictLevel)districtlevel, resetStartDistance);
}

-(YIMErrorcodeOC) GetDistance:(NSString*)userID
{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetLocationManager()->GetDistance([userID UTF8String]);
}

// 设置位置更新间隔(单位：分钟)
-(void) SetUpdateInterval:(unsigned int) interval{
    YIMManager::CreateInstance()->GetLocationManager()->SetUpdateInterval( interval );
}

// 获取麦克风状态
- (void) GetMicrophoneStatus
{
    YIMManager::CreateInstance()->GetMicrophoneStatus();
}

// 查询公告
- (YIMErrorcodeOC) QueryNotice
{
    return (YIMErrorcodeOC) YIMManager::CreateInstance()->QueryNotice();
}

//设置语音识别语言
- (YIMErrorcodeOC) SetSpeechRecognizeLanguage:(SpeechLanguageOC) language
{
    if (YIMManager::CreateInstance()->GetMessageManager() == NULL) {
        return YouMeIMCode_NotLogin;
    }
    return (YIMErrorcodeOC) YIMManager::CreateInstance()->GetMessageManager()->SetSpeechRecognizeLanguage((SpeechLanguage)language);
}

- (YIMErrorcodeOC) SetOnlyRecognizeSpeechText:(bool)recognition
{
    if (YIMManager::CreateInstance()->GetMessageManager() == NULL) {
        return YouMeIMCode_NotLogin;
    }
    return (YIMErrorcodeOC) YIMManager::CreateInstance()->GetMessageManager()->SetOnlyRecognizeSpeechText(recognition);
}

//举报
- (YIMErrorcodeOC) Accusation:(NSString*)userID source:(YIMChatTypeOC)source reason:(int)reason description:(NSString*)description extraParam:(NSString*)extraParam
{
    if (YIMManager::CreateInstance()->GetMessageManager() == NULL) {
        return YouMeIMCode_NotLogin;
    }
    return (YIMErrorcodeOC) YIMManager::CreateInstance()->GetMessageManager()->Accusation([userID UTF8String], (YIMChatType)source, reason, [description UTF8String], [extraParam UTF8String]);
}

- (YIMErrorcodeOC) SetMessageRead:(unsigned long long)msgID read:(bool)read
{
    return (YIMErrorcodeOC) YIMManager::CreateInstance()->GetMessageManager()->SetMessageRead(msgID, read);
}

- (YIMErrorcodeOC) SetDownloadDir:(NSString*) path
{
    return (YIMErrorcodeOC) YIMManager::CreateInstance()->GetMessageManager()->SetDownloadDir([path UTF8String]);
}

- (YIMErrorcodeOC) GetForbiddenSpeakInfo
{
    return (YIMErrorcodeOC) YIMManager::CreateInstance()->GetMessageManager()->GetForbiddenSpeakInfo();
}

- (YIMErrorcodeOC) SetUserProfileInfo:(IMUserSettingInfoOC *)userInfo
{
    if (YIMManager::CreateInstance()->GetUserProfileManager() == NULL) {
        return YouMeIMCode_NotLogin;
    }
    
    IMUserSettingInfo tmp;
    if ([userInfo.nickName UTF8String] != nil)
    {
        tmp.nickName = [userInfo.nickName UTF8String];
    }
    tmp.sex = (IMUserSex)userInfo.sex;
    if ([userInfo.personalSignature UTF8String] != nil)
    {
        tmp.personalSignature = [userInfo.personalSignature UTF8String];
    }
    if ([userInfo.country UTF8String] != nil)
    {
        tmp.country = [userInfo.country UTF8String];
    }
    if ([userInfo.province UTF8String] != nil)
    {
        tmp.province = [userInfo.province UTF8String];
    }
    if ([userInfo.city UTF8String] != nil)
    {
        tmp.city = [userInfo.city UTF8String];
    }
    if ([userInfo.extraInfo UTF8String] != nil)
    {
        tmp.extraInfo = [userInfo.extraInfo UTF8String];
    }
    
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetUserProfileManager()->SetUserProfileInfo(tmp);
}

- (YIMErrorcodeOC) GetUserProfileInfo:(NSString*) userID
{
    if (YIMManager::CreateInstance()->GetUserProfileManager() == NULL) {
        return YouMeIMCode_NotLogin;
    }
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetUserProfileManager()->GetUserProfileInfo([userID UTF8String]);
}

//自定义长宽
- (UIImage *)ReSizeImage:(UIImage *)image toSize:(CGSize)resize
{
    UIGraphicsBeginImageContext(CGSizeMake(resize.width, resize.height));
    [image drawInRect:CGRectMake(0, 0, resize.width, resize.height)];
    UIImage *reSizeImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    return reSizeImage;
}

- (YIMErrorcodeOC) SetUserProfilePhoto:(NSString*) photoPath
{
    if (YIMManager::CreateInstance()->GetUserProfileManager() == NULL) {
        return YouMeIMCode_NotLogin;
    }
//    //处理图片大小
//    UIImage *image = [UIImage imageWithContentsOfFile:photoPath];
//    
//    CGSize size;
//    size.width = YOUME_IM_PHOTO_SIZE;
//    size.height = YOUME_IM_PHOTO_SIZE;
//    UIImage *reSizeImage = [self ReSizeImage:image toSize:size];
//    
//    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
//    NSString *imageCacheDir = [paths objectAtIndex:0];
//    NSString *imageName = [NSString stringWithFormat:@"%u.png",arc4random()];
//    NSString *imageSavepath = [imageCacheDir stringByAppendingString:imageName];
//    NSLog(@"image save path: %@",imageSavepath);
//    [UIImagePNGRepresentation(reSizeImage) writeToFile:imageSavepath atomically:YES];
    
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetUserProfileManager()->SetUserProfilePhoto([photoPath UTF8String]);
}

- (YIMErrorcodeOC) SwitchUserStatus:(NSString*) userID  userStatus:(YIMUserStatusOC) userStatus
{
    if (YIMManager::CreateInstance()->GetUserProfileManager() == NULL) {
        return YouMeIMCode_NotLogin;
    }
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetUserProfileManager()->SwitchUserStatus([userID UTF8String], (YIMUserStatus)userStatus);
}

- (YIMErrorcodeOC) SetAddPermission:(bool) beFound beAddPermission:(IMUserBeAddPermissionOC) beAddPermission
{
    if (YIMManager::CreateInstance()->GetUserProfileManager() == NULL) {
        return YouMeIMCode_NotLogin;
    }
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetUserProfileManager()->SetAddPermission(beFound, (IMUserBeAddPermission)beAddPermission);
}

- (YIMErrorcodeOC) FindUser:(int) findType target:(NSString*) target
{
    if (YIMManager::CreateInstance()->GetFriendManager() == NULL) {
        return YouMeIMCode_NotLogin;
    }
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetFriendManager()->FindUser(findType, [target UTF8String]);
}

- (YIMErrorcodeOC) RequestAddFriend:(NSArray *) users comments:(NSString*) comments
{
    if (YIMManager::CreateInstance()->GetFriendManager() == NULL) {
        return YouMeIMCode_NotLogin;
    }
    std::vector<XString> vecUsers;
    for( int i = 0 ; i < users.count; i++ ){
        XString userID = [ users[i]  UTF8String ];
        vecUsers.push_back(userID);
    }
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetFriendManager()->RequestAddFriend(vecUsers, [comments UTF8String]);
}

- (YIMErrorcodeOC) DealBeRequestAddFriend:(NSString*) userID dealResult:(int) dealResult reqID:(unsigned long long)reqID
{
    if (YIMManager::CreateInstance()->GetFriendManager() == NULL) {
        return YouMeIMCode_NotLogin;
    }
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetFriendManager()->DealBeRequestAddFriend([userID UTF8String], dealResult, reqID);
}

- (YIMErrorcodeOC) DeleteFriend:(NSArray *) users deleteType:(int) deleteType
{
    if (YIMManager::CreateInstance()->GetFriendManager() == NULL) {
        return YouMeIMCode_NotLogin;
    }
    std::vector<XString> vecUsers;
    for( int i = 0 ; i < users.count; i++ ){
        XString userID = [ users[i]  UTF8String ];
        vecUsers.push_back(userID);
    }
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetFriendManager()->DeleteFriend(vecUsers,deleteType);
}

- (YIMErrorcodeOC) BlackFriend:(int) type users:(NSArray *) users
{
    if (YIMManager::CreateInstance()->GetFriendManager() == NULL) {
        return YouMeIMCode_NotLogin;
    }
    std::vector<XString> vecUsers;
    for( int i = 0 ; i < users.count; i++ ){
        XString userID = [ users[i]  UTF8String ];
        vecUsers.push_back(userID);
    }
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetFriendManager()->BlackFriend(type, vecUsers);
}

- (YIMErrorcodeOC) QueryFriends:(int) type startIndex:(int) startIndex count:(int) count
{
    if (YIMManager::CreateInstance()->GetFriendManager() == NULL) {
        return YouMeIMCode_NotLogin;
    }
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetFriendManager()->QueryFriends(type,startIndex,count);
}

- (YIMErrorcodeOC) QueryFriendRequestList:(int) startIndex count:(int) count
{
    if (YIMManager::CreateInstance()->GetFriendManager() == NULL) {
        return YouMeIMCode_NotLogin;
    }
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetFriendManager()->QueryFriendRequestList(startIndex,count);
}

-(YIMErrorcodeOC) SwitchMsgTransType:(YIMMsgTransTypeOC) transType{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->SwitchMsgTransType((YIMMsgTransType)transType);
}

@end
