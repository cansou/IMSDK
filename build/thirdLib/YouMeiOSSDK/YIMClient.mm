//
//  YIMClient.m
//  YoumeIMUILib
//
//  Created by 余俊澎 on 16/8/11.
//  Copyright © 2016年 余俊澎. All rights reserved.
//

#import "YIMClient.h"
#import "YIMImplement.h"

@implementation YIMClient

+(YIMClient*)GetInstance{
    static YIMClient *_instance = nil;
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

-(void) SetUpdateReadStatusCallbackFlag:(bool)flag{
    YIMImplement::getInstance()->m_updateReadStatusCallbackFlag = flag;
}

-(void)RebindIMCallback{
    YIMImplement::getInstance()->RebindCallback();
}

-(YIMErrorcodeOC)InitWithAppKey:(NSString *)strAppKey appSecurityKey:(NSString*)strAppSecurity serverZone:(YouMeIMServerZoneOC)serverZone{
    YIMManager::CreateInstance()->SetServerZone((ServerZone)(int)serverZone);
    
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->Init([strAppKey UTF8String], [strAppSecurity UTF8String], "");
}

-(void) Login:(NSString *)userName password:(NSString *)password token:(NSString*) token callback:(loginCBType) callback{
    [YIMCallbackBlock GetInstance].loginCB = callback;
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->Login([userName UTF8String], [password UTF8String], [token UTF8String] );
    
    if(code != YouMeIMCode_Success && [YIMCallbackBlock GetInstance].loginCB)
    {
        [YIMCallbackBlock GetInstance].loginCB(code,@"");
        [YIMCallbackBlock GetInstance].loginCB = nil;
    }
}

-(void) JoinChatRoom:(NSString *)roomID callback:(joinRoomCBType)callback{
    if(roomID == nil || [roomID isEqualToString:@""]){
        if(callback){
            callback(YouMeIMCode_ParamInvalid,roomID);
            callback = nil;
        }
    }
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetChatRoomManager()->JoinChatRoom([roomID UTF8String]);
    if(callback == nil)
    {
        return;
    }
    
    if( code == YouMeIMCode_Success )
    {
        [[YIMCallbackBlock GetInstance].joinRoomCBBlocks setObject:callback forKey:roomID];
    }
    else
    {
        callback(code,roomID);
    }
}

-(void) LeaveChatRoom:(NSString *)roomID callback:(leaveRoomCBType) callback{
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetChatRoomManager()->LeaveChatRoom([roomID UTF8String]);
    
    if(callback == nil)
    {
        return;
    }
    if( code == YouMeIMCode_Success )
    {
        [[YIMCallbackBlock GetInstance].leaveRoomCBBlocks setObject:callback forKey:roomID];
    }
    else
    {
        callback(code,roomID);
    }    
}

-(void) LeaveAllChatRooms:(leaveAllRoomCBType)callback{
    [YIMCallbackBlock GetInstance].leaveAllRoomCB = callback;
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetChatRoomManager()->LeaveAllChatRooms();
    
    if((code != YouMeIMCode_Success) && [YIMCallbackBlock GetInstance].leaveAllRoomCB)
    {
        callback(code);
        [YIMCallbackBlock GetInstance].leaveAllRoomCB = nil;
    }
}

-(void) GetRoomMemberCount:(NSString *)roomID callback:(getRoomMemberCountCBType) callback{
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetChatRoomManager()->GetRoomMemberCount([roomID UTF8String]);
    
    if(callback == nil)
    {
        return;
    }
    if( code == YouMeIMCode_Success )
    {
        [[YIMCallbackBlock GetInstance].getRoomMemberCountCBBlocks setObject:callback forKey:roomID];
    }
    else
    {
        callback(code, roomID, 0);
    }
}

-(void)setSendMessageCB2Cache:(sendMessageStatusCBType)callback requestid:(unsigned long long)requestid
{
    [[YIMCallbackBlock GetInstance].sendMessageCBBlocks setObject:callback forKey:[NSNumber numberWithUnsignedLongLong:requestid]];
}

-(void)setUploadProgressCB2Cache:(uploadProgressCBType)callback requestid:(unsigned long long)requestid
{
    [[YIMCallbackBlock GetInstance].uploadProgressCBBlocks setObject:callback forKey:[NSNumber numberWithUnsignedLongLong:requestid]];
}

-(unsigned long long) SendTextMessage:(NSString *)receiverID chatType:(YIMChatTypeOC)chatType msgContent:(NSString *) msgContent attachParam:(NSString *)attachParam callback:(sendMessageStatusCBType)callback{
    
    unsigned long long requestID = 0;
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->SendTextMessage([receiverID UTF8String], (YIMChatType)chatType, [msgContent UTF8String], [attachParam UTF8String], &requestID);
    
    if(callback == nil)
    {
        return requestID;
    }
    
    if(code == (int)YIMErrorcode_Success)
    {
        [self setSendMessageCB2Cache:callback requestid:requestID];
    }
    else
    {
       callback(code,0,false,0,0,0);
    }
    
    return requestID;
}

-(void) sendMessageReadStatus:(NSString*)sendId chatType:(YIMChatTypeOC)chatType msgId:(unsigned long long)msgId {
    if (sendId == nil) {
        NSLog(@"bruce >>> sendId is null");
        return;
    }
    
    YIMManager::CreateInstance()->GetMessageManager()->SendMessageReadStatus([sendId UTF8String], chatType, msgId);
}

-(unsigned long long) StartRecordAudioMessage:(NSString *)receiverID chatType:(YIMChatTypeOC)chatType recognizeText:(bool)recognizeText isOpenOnlyRecognizeText:(bool)isOpenOnlyRecognizeText callback:(sendAudioMsgStatusCBType)callback startSendCallback:(startSendAudioMsgCBType)startSendCallback{
    unsigned long long requestID = 0;
    
    YIMErrorcodeOC code = YouMeIMCode_Fail;
    if (recognizeText)
    {
        YIMManager::CreateInstance()->GetMessageManager()->SetOnlyRecognizeSpeechText(isOpenOnlyRecognizeText);
        
        code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->SendAudioMessage([receiverID UTF8String], (YIMChatType)chatType, &requestID);
    }
    else
    {
        code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->SendOnlyAudioMessage([receiverID UTF8String], (YIMChatType)chatType, &requestID);
    }
    
    if(callback == nil)
    {
        return requestID;
    }
    
    if(code == (int)YIMErrorcode_Success)
    {
        if (startSendCallback)
        {
            [[YIMCallbackBlock GetInstance].startSendAudioMsgCBBlocks setObject:startSendCallback forKey:[NSNumber numberWithUnsignedLongLong:requestID]];
        }
        
        [[YIMCallbackBlock GetInstance].sendAudioMsgCBBlocks setObject:callback forKey:[NSNumber numberWithUnsignedLongLong:requestID]];
    }
    else
    {
        callback(code,@"",@"",0,0,false,-1,0, 0);
    }
    
    return requestID;
}

//-(void) StartOnlyRecordAudioMessage:(NSString *)receiverID chatType:(YIMChatTypeOC)chatType callback:(sendAudioMsgStatusCBType)callback{
//    unsigned long long requestID = 0;
//
//    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->SendOnlyAudioMessage([receiverID UTF8String], (YIMChatType)chatType, &requestID);
//
//    if(code != YIMErrorcode_Success && callback){
//        callback(code,@"",@"",0,0,false,-1,0);
//    }
//    if(code == YIMErrorcode_Success && callback){
//        [self setSendAudioMsgCB2Cache:callback requestid:requestID];
//    }
//}

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

-(unsigned long long) SendCustomMessage:(NSString*) receiverID chatType:(YIMChatTypeOC)chatType content:(NSData*)content callback:(sendMessageStatusCBType)callback{
    
    unsigned long long requestID = 0;
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->SendCustomMessage([receiverID UTF8String], (YIMChatType)chatType, (const char*)content.bytes, content.length, &requestID);
    
    if(callback == nil)
    {
        return requestID;
    }
    
    if(code == YouMeIMCode_Success)
    {
        [self setSendMessageCB2Cache:callback requestid:requestID];
    }
    else
    {
        callback(code,0,false,0,0,0);
    }
    
    return requestID;
}

-(unsigned long long) SendFile:(NSString*) receiverID chatType:(YIMChatTypeOC)chatType filePath:(NSString*)filePath extraParam:(NSString*)extraParam fileType:(YIMFileTypeOC)fileType callback:(sendMessageStatusCBType)callback {
    
    unsigned long long requestID = 0;
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->SendFile([receiverID UTF8String], (YIMChatType)chatType, [filePath UTF8String], &requestID, [extraParam UTF8String],(YIMFileType)fileType);

    if(callback == nil)
    {
        return requestID;
    }

    if(code == YouMeIMCode_Success)
    {
        [self setSendMessageCB2Cache:callback requestid:requestID];
    }
    else
    {
        callback(code,0,false,0,0,0);
    }
    
    return requestID;
}

-(unsigned long long) SendFileWithProgress:(NSString*) receiverID chatType:(YIMChatTypeOC)chatType filePath:(NSString*)filePath extraParam:(NSString*)extraParam fileType:(YIMFileTypeOC)fileType callback:(sendMessageStatusCBType)callback uploadProgress:(uploadProgressCBType)progressCallback {
    unsigned long long requestID = 0;
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->SendFile([receiverID UTF8String], (YIMChatType)chatType, [filePath UTF8String], &requestID, [extraParam UTF8String],(YIMFileType)fileType);
    if(callback == nil)
    {
        return requestID;
    }

    if(code == YouMeIMCode_Success)
    {
        [self setSendMessageCB2Cache:callback requestid:requestID];
        [self setUploadProgressCB2Cache:progressCallback requestid:requestID];
    }
    else
    {
        callback(code,0,false,0,0,0);
    }
    
    return requestID;
}

//extraParam:附加参数 格式为json {"nickname":"","server_area":"","location":"","score":"","level":"","vip_level":"","extra":""}
-(unsigned long long) SendGift:(NSString*)anchorID channel:(NSString*)channel giftId:(int)giftId giftCount:(int)giftCount extraParam:(NSString*) extraParam callback:(sendMessageStatusCBType)callback{
    
    unsigned long long requestID = 0;
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->SendGift([anchorID UTF8String], [channel UTF8String], giftId, giftCount, [extraParam UTF8String], &requestID);
    
    if(callback == nil)
    {
        return requestID;
    }
    
    if(code == YIMErrorcode_Success)
    {
        [self setSendMessageCB2Cache:callback requestid:requestID];
    }
    else
    {
        callback(code,0,false,0,0,0);
    }
    
    return requestID;
}

//查询消息记录(direction 查询方向 0：向前查找	1：向后查找)
-(void) QueryHistoryMessage:(NSString*) targetID  chatType:(YIMChatTypeOC) chatType startMessageID:(unsigned long long)startMessageID count:(int)count direction:(int)direction callback:(queryHistoryMsgCBType)callback{
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->QueryHistoryMessage( [targetID UTF8String], (YIMChatType)chatType, startMessageID, count, direction );
    
    if(callback == nil)
    {
        return;
    }
    if( code == YouMeIMCode_Success )
    {
        [[YIMCallbackBlock GetInstance].queryHistoryMsgCBBlocks setObject:callback forKey:targetID];
    }
    else
    {
        callback(code, targetID, 0, nil);
    }
}

-(YIMErrorcodeOC) DeleteHistoryMessage:(YIMChatTypeOC)chatType time:(unsigned long long)time{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->DeleteHistoryMessage( (YIMChatType)chatType, time);
}

-(YIMErrorcodeOC) DeleteHistoryMessageByRecvId:(NSString*)recvId chatType:(YIMChatTypeOC)chatType startMessageID:(unsigned long long)startMessageID count:(int)count {
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->DeleteHistoryMessage([recvId UTF8String], (YIMChatType)chatType, startMessageID, count);
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

-(void) QueryRoomHistoryMessageFromServer:(NSString*)roomID count:(int)count direction:(int)direction callback:(queryRoomHistoryMsgCBType)callback{
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->QueryRoomHistoryMessageFromServer([roomID UTF8String], count, direction);
    
    if(callback == nil)
    {
        return;
    }
    if( code == YouMeIMCode_Success )
    {
        [[YIMCallbackBlock GetInstance].queryRoomHistoryMsgCBBlocks setObject:callback forKey:roomID];
    }
    else
    {
        callback(code, roomID, 0, nil);
    }
}

-(unsigned long long) StartAudioSpeech:(bool)translate callback:(uploadSpeechStatusCBType)callback{
    unsigned long long requestID = 0;
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->StartAudioSpeech(&requestID, translate);
    
    if(callback == nil)
    {
        return requestID;
    }
    if( code == YouMeIMCode_Success )
    {
        [[YIMCallbackBlock GetInstance].uploadSpeechStatusCBBlocks setObject:callback forKey:[NSNumber numberWithUnsignedLongLong:requestID]];
    }
    else
    {
        AudioSpeechInfo *audioSpeechInfo = [AudioSpeechInfo new];
        audioSpeechInfo.requestID = requestID;
        audioSpeechInfo.textContent = @"";
        audioSpeechInfo.fileSize = 0;
        audioSpeechInfo.audioTime = 0;
        audioSpeechInfo.downloadURL = @"";
        
        callback(code, audioSpeechInfo);
    }
    
    return requestID;
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

-(unsigned int) TranslateText:(NSString*)text  destLangCode:(LanguageCodeOC) destLangCode srcLangCode:(LanguageCodeOC) srcLangCode callback:(translateTextCompleteCBType)callback{
    unsigned int requestID = 0;
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->TranslateText( &requestID, [text UTF8String], (LanguageCode)destLangCode, (LanguageCode) srcLangCode );
    
    if(callback == nil)
    {
        return requestID;
    }
    if( code == YouMeIMCode_Success )
    {
        [[YIMCallbackBlock GetInstance].translateTextCompleteCBBlocks setObject:callback forKey:[NSNumber numberWithUnsignedLongLong:requestID]];
    }
    else
    {
        callback(code, requestID, text, srcLangCode, destLangCode);
    }

    return requestID;
}

-(void) BlockUser:(NSString*) userID block:(bool) block callback:(blockUserCBType)callback{
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->BlockUser([userID UTF8String], block);
    
    if(callback == nil)
    {
        return;
    }
    if( code == YouMeIMCode_Success )
    {
        NSString *strKey;
        
        strKey = [NSString stringWithFormat:@"%@_%@",userID,block?@"YES":@"NO"];
        NSLog(@"blockuser 拼接userID,block : %@",strKey);
        [[YIMCallbackBlock GetInstance].blockUserCBBlocks setObject:callback forKey:userID];
    }
    else
    {
        callback(code, userID, block);
    }
}

//解除所有已屏蔽用户
-(void) UnBlockAllUser:(unBlockAllUserCBType)callback{
    [YIMCallbackBlock GetInstance].unBlockAllUserCB = callback;
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->UnBlockAllUser();
    
    if(code != YouMeIMCode_Success && [YIMCallbackBlock GetInstance].unBlockAllUserCB){
        
        callback(code);
        [YIMCallbackBlock GetInstance].unBlockAllUserCB = nil;
    }
}

//获取被屏蔽消息用户
-(void) GetBlockUsers:(getBlockUsersCBType)callback{
    [YIMCallbackBlock GetInstance].getBlockUsersCB = callback;
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->GetBlockUsers();
    
    if(code != YouMeIMCode_Success && [YIMCallbackBlock GetInstance].getBlockUsersCB){
        
        callback(code, nil);
        [YIMCallbackBlock GetInstance].getBlockUsersCB = nil;
    }
}

-(void) GetRecentContacts:(getRecentContactsCBType)callback{
    [YIMCallbackBlock GetInstance].getContactsCB = callback;
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetRecentContacts();
    
    if(code != YouMeIMCode_Success && [YIMCallbackBlock GetInstance].getContactsCB)
    {
        
        callback(code, nil);
        [YIMCallbackBlock GetInstance].getContactsCB = nil;
    }
}

-(YIMErrorcodeOC) SetUserInfo:(NSString*) userInfo{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->SetUserInfo([userInfo UTF8String]);
}

-(void) GetUserInfo:( NSString * )userID callback:(getUserInfoCBType)callback{
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetUserInfo([userID UTF8String]);
    
    if(callback == nil)
    {
        return;
    }
    if( code == YouMeIMCode_Success )
    {
        [[YIMCallbackBlock GetInstance].getUserInfoCBBlocks setObject:callback forKey:userID];
    }
    else
    {
        callback(code,userID, @"");
    }
}

-(void) QueryUserStatus:( NSString * )userID callback:(queryUserStatusCBType)callback{
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->QueryUserStatus([userID UTF8String]);
    
    if(callback == nil)
    {
        return;
    }
    if( code == YouMeIMCode_Success )
    {
        [[YIMCallbackBlock GetInstance].queryUserStatusCBBlocks setObject:callback forKey:userID];
    }
    else
    {
        callback(code,userID, YIMSTATUS_OFFLINE);
    }
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
- (void) StartPlayAudio:(NSString* ) path callback:(playCompleteCBType)callback{
    
    bool ret = YIMManager::CreateInstance()->IsPlaying();
    if (ret) {
        YIMErrorcodeOC code_stop = (YIMErrorcodeOC)YIMManager::CreateInstance()->StopPlayAudio();
        if ((code_stop != YouMeIMCode_Success) && callback)
        {
            callback(YouMeIMCode_StopPlayFailBeforeStart, path);
        }
    }
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->StartPlayAudio( [path UTF8String] );
    
    if(callback == nil)
    {
        return;
    }
    if( code == YouMeIMCode_Success )
    {
        [[YIMCallbackBlock GetInstance].playCompleteCBBlocks setObject:callback forKey:path];
    }
    else
    {
        callback(code,path);
    }
}
//停止语音播放
- (YIMErrorcodeOC) StopPlayAudio{
    return (YIMErrorcodeOC)YIMManager::CreateInstance()->StopPlayAudio();
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

-(void) Logout:(logoutCBType)callback{
    [YIMCallbackBlock GetInstance].logoutCB = callback;
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->Logout();
    
    if(code != YouMeIMCode_Success && [YIMCallbackBlock GetInstance].logoutCB)
    {
        callback(code);
        [YIMCallbackBlock GetInstance].logoutCB = nil;
    }
}

-(void) DownloadAudio:(unsigned long long) ulSerial strSavePath:(NSString *)strSavePath callback:(downloadCBType)callback{
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->DownloadFile(ulSerial, [strSavePath UTF8String]);
    
    if(callback == nil)
    {
        return;
    }
    if( code == YouMeIMCode_Success )
    {
        [[YIMCallbackBlock GetInstance].downloadCBBlocks setObject:callback forKey:[NSNumber numberWithUnsignedLongLong:ulSerial]];
    }
    else
    {
        YIMMessage *msg = [YIMMessage new];
        callback(code, msg, @"");
    }
}

-(void) DownloadFileByUrl:(NSString*) downloadURL  strSavePath:(NSString*)strSavePath fileType:(YIMFileTypeOC)fileType callback:(downloadByUrlCBType)callback{
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetMessageManager()->DownloadFile([downloadURL UTF8String], [strSavePath UTF8String], (YIMFileType)fileType);
    
    if(callback == nil)
    {
        return;
    }
    
    if( code == YouMeIMCode_Success )
    {
        [[YIMCallbackBlock GetInstance].downloadByUrlCBBlocks setObject:callback forKey:downloadURL];
    }
    else
    {
        callback(code, downloadURL, @"",0);
    }
}

+(void) SetAudioCacheDir:(NSString*) path{
    YIMManager::CreateInstance()->SetAudioCacheDir([path UTF8String]);
}

// 获取当前地理位置
-(YIMErrorcodeOC) GetCurrentLocation{
    return  (YIMErrorcodeOC)YIMManager::CreateInstance()->GetLocationManager()->GetCurrentLocation();
}
// 获取附近的目标	count:获取数量（一次最大200） serverAreaID：区服	districtlevel：行政区划等级		resetStartDistance：是否重置查找起始距离
-(void) GetNearbyObjects:(int) count  serverAreaID:(NSString*)serverAreaID districtlevel:(YouMeDistrictLevelOC)districtlevel resetStartDistance:(bool)resetStartDistance callback:(getNearbyObjectsCBType)callback{
    
    [YIMCallbackBlock GetInstance].getNearbyObjectsCB = callback;
    
    YIMErrorcodeOC code =  (YIMErrorcodeOC)YIMManager::CreateInstance()->GetLocationManager()->GetNearbyObjects( count, [serverAreaID UTF8String], (DistrictLevel)districtlevel, resetStartDistance);
    
    if(code != YouMeIMCode_Success && [YIMCallbackBlock GetInstance].getNearbyObjectsCB){

        callback(code, nil, 0, 0);
        [YIMCallbackBlock GetInstance].getNearbyObjectsCB = nil;
    }
}

-(void) GetDistance:(NSString*)userID callback:(getDistanceCBType)callback
{
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetLocationManager()->GetDistance([userID UTF8String]);
    
    if(callback == nil)
    {
        return;
    }
    if( code == YouMeIMCode_Success )
    {
        [[YIMCallbackBlock GetInstance].getDistanceCBBlocks setObject:callback forKey:userID];
    }
    else
    {
        callback(code, userID, 0);
    }
}

// 设置位置更新间隔(单位：分钟)
-(void) SetUpdateInterval:(unsigned int) interval{
    YIMManager::CreateInstance()->GetLocationManager()->SetUpdateInterval( interval );
}

// 获取麦克风状态
- (void) GetMicrophoneStatus:(getMicStatusCBType)callback
{
    [YIMCallbackBlock GetInstance].getMicStatusCB = callback;
    
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

- (void) GetForbiddenSpeakInfo:(getForbiddenSpeakInfoCBType)callback
{
    [YIMCallbackBlock GetInstance].getForbiddenSpeakInfoCB = callback;
    
    YIMErrorcodeOC code = (YIMErrorcodeOC) YIMManager::CreateInstance()->GetMessageManager()->GetForbiddenSpeakInfo();
    
    if(code != YouMeIMCode_Success && [YIMCallbackBlock GetInstance].getForbiddenSpeakInfoCB)
    {        
        callback(code, nil);
        [YIMCallbackBlock GetInstance].getForbiddenSpeakInfoCB = nil;
    }
}

- (void) SetUserProfileInfo:(IMUserSettingInfoOC *)userInfo callback:(setUserProfileCBType)callback
{
    [YIMCallbackBlock GetInstance].setUserProfileCB = callback;
    
    if (YIMManager::CreateInstance()->GetUserProfileManager() == NULL) {
        callback(YouMeIMCode_NotLogin);
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
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetUserProfileManager()->SetUserProfileInfo(tmp);
    
    if(code != YouMeIMCode_Success && [YIMCallbackBlock GetInstance].setUserProfileCB){
        
        callback(code);
        [YIMCallbackBlock GetInstance].setUserProfileCB = nil;
    }
}

- (void) GetUserProfileInfo:(NSString*) userID callback:(queryUserProfileCBType)callback
{
    if(callback == nil)
    {
        return;
    }
    if (YIMManager::CreateInstance()->GetUserProfileManager() == NULL)
    {
        callback(YouMeIMCode_NotLogin, nil);
    }
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetUserProfileManager()->GetUserProfileInfo([userID UTF8String]);
    
    if( code == YouMeIMCode_Success )
    {
        [[YIMCallbackBlock GetInstance].queryUserProfileCBBlocks setObject:callback forKey:userID];
    }
    else
    {
        callback(code, nil);
    }
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

- (void) SetUserProfilePhoto:(NSString*) photoPath callback:(setPhotoUrlCBType)callback
{
    [YIMCallbackBlock GetInstance].setPhotoUrlCB = callback;
    
    if (YIMManager::CreateInstance()->GetUserProfileManager() == NULL) {
        callback(YouMeIMCode_NotLogin, @"");
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
    
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetUserProfileManager()->SetUserProfilePhoto([photoPath UTF8String]);
    
    if(code != YouMeIMCode_Success && [YIMCallbackBlock GetInstance].setPhotoUrlCB)
    {
        callback(code, @"");
        [YIMCallbackBlock GetInstance].setPhotoUrlCB = nil;
    }
}

- (void) SwitchUserStatus:(NSString*) userID  userStatus:(YIMUserStatusOC) userStatus callback:(switchUserOnlineStateCBType)callback
{
    [YIMCallbackBlock GetInstance].switchUserOnlineStateCB = callback;
    
    if (YIMManager::CreateInstance()->GetUserProfileManager() == NULL) {
        callback(YouMeIMCode_NotLogin);
    }
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetUserProfileManager()->SwitchUserStatus([userID UTF8String], (YIMUserStatus)userStatus);
    
    if(code != YouMeIMCode_Success && [YIMCallbackBlock GetInstance].switchUserOnlineStateCB)
    {
        
        callback(code);       
        [YIMCallbackBlock GetInstance].switchUserOnlineStateCB = nil;
    }
}

- (void) SetAddPermission:(bool) beFound beAddPermission:(IMUserBeAddPermissionOC) beAddPermission callback:(setUserProfileCBType)callback
{
    [YIMCallbackBlock GetInstance].setUserProfileCB = callback;
    
    if (YIMManager::CreateInstance()->GetUserProfileManager() == NULL) {
        callback(YouMeIMCode_NotLogin);
    }
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetUserProfileManager()->SetAddPermission(beFound, (IMUserBeAddPermission)beAddPermission);
    
    if(code != YouMeIMCode_Success && [YIMCallbackBlock GetInstance].setUserProfileCB)
    {
        
        callback(code);
        [YIMCallbackBlock GetInstance].setUserProfileCB = nil;
    }
}

- (void) FindUser:(int) findType target:(NSString*) target callback:(findUserCBType)callback
{
    [YIMCallbackBlock GetInstance].findUserCB = callback;
    
    if (YIMManager::CreateInstance()->GetFriendManager() == NULL) {
        callback(YouMeIMCode_NotLogin, nil);
    }
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetFriendManager()->FindUser(findType, [target UTF8String]);
    
    if(code != YouMeIMCode_Success && [YIMCallbackBlock GetInstance].findUserCB)
    {
        
        callback(code, nil);
        [YIMCallbackBlock GetInstance].findUserCB = nil;
    }
}

- (void) RequestAddFriend:(NSArray *) users comments:(NSString*) comments callback:(requestAddFriendCBType)callback
{
    [YIMCallbackBlock GetInstance].requestAddFriendCB = callback;
    
    if (YIMManager::CreateInstance()->GetFriendManager() == NULL) {
        callback(YouMeIMCode_NotLogin, @"");
    }
    std::vector<XString> vecUsers;
    for( int i = 0 ; i < users.count; i++ ){
        XString userID = [ users[i]  UTF8String ];
        vecUsers.push_back(userID);
    }
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetFriendManager()->RequestAddFriend(vecUsers, [comments UTF8String]);
    
    if(code != YouMeIMCode_Success && [YIMCallbackBlock GetInstance].requestAddFriendCB)
    {
        callback(code, @"");
        [YIMCallbackBlock GetInstance].requestAddFriendCB = nil;
    }
}

- (void) DealBeRequestAddFriend:(NSString*) userID dealResult:(int) dealResult callback:(dealBeRequestAddFriendCBType)callback reqID:(unsigned long long)reqID
{
    if (callback == nil)
    {
        return;
    }
    
    if (YIMManager::CreateInstance()->GetFriendManager() == NULL) {
        callback(YouMeIMCode_NotLogin, userID, @"", -1);
    }
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetFriendManager()->DealBeRequestAddFriend([userID UTF8String], dealResult, reqID);
    
    if( code == YouMeIMCode_Success )
    {
        [[YIMCallbackBlock GetInstance].dealBeRequestAddFriendCBBlocks setObject:callback forKey:userID];
    }
    else
    {
        callback(code, userID, @"", -1);
    }
}

- (void) DeleteFriend:(NSArray *) users deleteType:(int) deleteType callback:(deleteFriendCBType)callback
{
    [YIMCallbackBlock GetInstance].deleteFriendCB = callback;
    
    if (YIMManager::CreateInstance()->GetFriendManager() == NULL) {
        callback(YouMeIMCode_NotLogin, @"");
    }
    std::vector<XString> vecUsers;
    for( int i = 0 ; i < users.count; i++ ){
        XString userID = [ users[i]  UTF8String ];
        vecUsers.push_back(userID);
    }
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetFriendManager()->DeleteFriend(vecUsers,deleteType);
    
    if(code != YouMeIMCode_Success && [YIMCallbackBlock GetInstance].deleteFriendCB)
    {
        callback(code, @"");
        [YIMCallbackBlock GetInstance].deleteFriendCB = nil;
    }
}

- (void) BlackFriend:(int) type users:(NSArray *) users callback:(blackFriendCBType)callback
{
    [YIMCallbackBlock GetInstance].blackFriendCB = callback;
    
    if (YIMManager::CreateInstance()->GetFriendManager() == NULL) {
        callback(YouMeIMCode_NotLogin, type, @"");
    }
    std::vector<XString> vecUsers;
    for( int i = 0 ; i < users.count; i++ ){
        XString userID = [ users[i]  UTF8String ];
        vecUsers.push_back(userID);
    }
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetFriendManager()->BlackFriend(type, vecUsers);
    
    if(code != YouMeIMCode_Success && [YIMCallbackBlock GetInstance].blackFriendCB)
    {        
        callback(code, type, @"");
        [YIMCallbackBlock GetInstance].blackFriendCB = nil;
    }
}

- (void) QueryFriends:(int) type startIndex:(int) startIndex count:(int) count callback:(queryFriendsCBType)callback
{
    [YIMCallbackBlock GetInstance].queryFriendsCB = callback;
    
    if (YIMManager::CreateInstance()->GetFriendManager() == NULL) {
        callback(YouMeIMCode_NotLogin, type, startIndex, nil);
    }
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetFriendManager()->QueryFriends(type,startIndex,count);
    
    if(code != YouMeIMCode_Success && [YIMCallbackBlock GetInstance].queryFriendsCB)
    {
        callback(code, type, startIndex, nil);
        [YIMCallbackBlock GetInstance].queryFriendsCB = nil;
    }
}

- (void) QueryFriendRequestList:(int) startIndex count:(int) count callback:(queryFriendRequestListCBType)callback
{
    [YIMCallbackBlock GetInstance].queryFriendRequestListCB = callback;
    
    if (YIMManager::CreateInstance()->GetFriendManager() == NULL) {
        callback(YouMeIMCode_NotLogin, startIndex, nil);
    }
    YIMErrorcodeOC code = (YIMErrorcodeOC)YIMManager::CreateInstance()->GetFriendManager()->QueryFriendRequestList(startIndex,count);
    
    if(code != YouMeIMCode_Success && [YIMCallbackBlock GetInstance].queryFriendRequestListCB)
    {
        callback(code, startIndex, nil);
        [YIMCallbackBlock GetInstance].queryFriendRequestListCB = nil;
    }
}

@end
