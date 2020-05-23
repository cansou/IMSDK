//
//  YIMImplement.m
//  YoumeIMUILib
//
//  Created by 余俊澎 on 16/8/11.
//  Copyright © 2016年 余俊澎. All rights reserved.
//

#import "YIMImplement.h"

YIMImplement *YIMImplement::mPInstance = NULL;

YIMImplement *YIMImplement::getInstance ()
{
    if (mPInstance == NULL)
    {
        mPInstance = new YIMImplement();
    }
    return mPInstance;
}

YIMImplement::YIMImplement ()
{
    
    IMManager = YIMManager::CreateInstance();
    
    IMManager->SetLoginCallback(this);
    IMManager->SetMessageCallback(this);
    IMManager->SetChatRoomCallback(this);
    IMManager->SetDownloadCallback(this);
    IMManager->SetContactCallback( this );
    IMManager->SetAudioPlayCallback( this );
    IMManager->SetLocationCallback( this );
    IMManager->SetNoticeCallback(this);
    IMManager->SetReconnectCallback(this);
    
    IMManager->SetUserProfileCallback(this);
    IMManager->SetFriendCallback(this);
    
    if (m_updateReadStatusCallbackFlag) {
        IMManager->SetUpdateReadStatusCallback(this);
    }
}

void YIMImplement::RebindCallback(){
    
    IMManager->SetLoginCallback(this);
    IMManager->SetMessageCallback(this);
    IMManager->SetChatRoomCallback(this);
    IMManager->SetDownloadCallback(this);
    IMManager->SetContactCallback( this );
    IMManager->SetAudioPlayCallback( this );
    IMManager->SetLocationCallback( this );
    IMManager->SetNoticeCallback(this);
    IMManager->SetReconnectCallback(this);
    
    IMManager->SetUserProfileCallback(this);
    IMManager->SetFriendCallback(this);
}

void YIMImplement::destroy ()
{
    if(mPInstance!=NULL){
        delete mPInstance;
    }
    mPInstance = NULL;
}

YIMImplement::~YIMImplement ()
{
}

//implement callback
//IYIMLoginCallback 目前用到的回调
void YIMImplement::OnLogin(YIMErrorcode errorcode, const XCHAR* userID){
    NSString *uid=nil;
    if(errorcode == YIMErrorcode_Success){
        uid = [NSString stringWithCString:userID encoding:NSUTF8StringEncoding];
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnLogin:userID:)]){
            [delegate OnLogin:(YIMErrorcodeOC)errorcode userID:uid];
        }
        if([YIMCallbackBlock GetInstance].loginCB){
            [YIMCallbackBlock GetInstance].loginCB ((YIMErrorcodeOC)errorcode,uid);
            [YIMCallbackBlock GetInstance].loginCB = nil;
        }
    });
}

void YIMImplement::OnLogout(YIMErrorcode errorcode) {
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnLogout:)]){
            [delegate OnLogout:(YIMErrorcodeOC)errorcode];
        }
        if([YIMCallbackBlock GetInstance].logoutCB){
            [YIMCallbackBlock GetInstance].logoutCB((YIMErrorcodeOC)errorcode);
            [YIMCallbackBlock GetInstance].logoutCB = nil;
        }
    });
}
void YIMImplement::OnJoinChatRoom(YIMErrorcode errorcode,  const XCHAR* chatRoomID){
    NSString *groupID =nil;
    if(errorcode == YIMErrorcode_Success){
        groupID = [NSString stringWithUTF8String:chatRoomID];
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnJoinRoom:roomID:)]){
            [delegate OnJoinRoom:(YIMErrorcodeOC)errorcode roomID:groupID];
        }
        joinRoomCBType callblock = [[YIMCallbackBlock GetInstance].joinRoomCBBlocks objectForKey:groupID];
        if(callblock){
            callblock((YIMErrorcodeOC)errorcode,groupID);
            callblock = nil;
            [[YIMCallbackBlock GetInstance].joinRoomCBBlocks removeObjectForKey:groupID];
        }
    });
}

void YIMImplement::OnLeaveChatRoom(YIMErrorcode errorcode, const XCHAR* chatRoomID){
    NSString *groupID =nil;
    if(errorcode == YIMErrorcode_Success){
        groupID = [NSString stringWithUTF8String:chatRoomID];
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnLeaveRoom:roomID:)]){
            [delegate OnLeaveRoom:(YIMErrorcodeOC)errorcode roomID:groupID];
        }
        leaveRoomCBType callblock = [[YIMCallbackBlock GetInstance].leaveRoomCBBlocks objectForKey:groupID];
        if(callblock){
            callblock((YIMErrorcodeOC)errorcode,groupID);
            callblock = nil;
            [[YIMCallbackBlock GetInstance].leaveRoomCBBlocks removeObjectForKey:groupID];
        }
    });
}

void YIMImplement::OnLeaveAllChatRooms(YIMErrorcode errorcode){
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnLeaveAllRooms:)]){
            [delegate OnLeaveAllRooms:(YIMErrorcodeOC)errorcode];
        }
        if([YIMCallbackBlock GetInstance].leaveAllRoomCB){
            [YIMCallbackBlock GetInstance].leaveAllRoomCB((YIMErrorcodeOC)errorcode);
            [YIMCallbackBlock GetInstance].leaveAllRoomCB = nil;
        }
    });
}

//其他用户加入频道通知
void YIMImplement::OnUserJoinChatRoom(const XCHAR* chatRoomID, const XCHAR* userID)
{
    NSString *roomID = [NSString stringWithUTF8String:chatRoomID];
    NSString *user = [NSString stringWithUTF8String:userID];
    
    dispatch_sync(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnUserJoinRoom:userID:)]) {
            [delegate OnUserJoinRoom:roomID userID:user];
        }
    });
}

//其他用户退出频道通知
void YIMImplement::OnUserLeaveChatRoom(const XCHAR* chatRoomID, const XCHAR* userID)
{
    NSString *roomID = [NSString stringWithUTF8String:chatRoomID];
    NSString *user = [NSString stringWithUTF8String:userID];
    
    dispatch_sync(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnUserLeaveRoom:userID:)]) {
            [delegate OnUserLeaveRoom:roomID userID:user];
        }
    });
}

void YIMImplement::OnGetRoomMemberCount(YIMErrorcode errorcode, const XCHAR* chatRoomID, unsigned int count)
{
    NSString *roomID = [NSString stringWithUTF8String:chatRoomID];
    dispatch_sync(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnGetRoomMemberCount:chatRoomID:count:)]) {
            [delegate OnGetRoomMemberCount:(YIMErrorcodeOC)errorcode chatRoomID:roomID count:count];
        }
        getRoomMemberCountCBType callblock = [[YIMCallbackBlock GetInstance].getRoomMemberCountCBBlocks objectForKey:roomID];
        if(callblock){
            callblock((YIMErrorcodeOC)errorcode,roomID,count);
            callblock = nil;
            [[YIMCallbackBlock GetInstance].getRoomMemberCountCBBlocks removeObjectForKey:roomID];
        }
    });
}


//文本翻译完成回调
void YIMImplement::OnTranslateTextComplete(YIMErrorcode errorcode, unsigned int requestID, const XCHAR* text, LanguageCode srcLangCode, LanguageCode destLangCode){
    dispatch_async(dispatch_get_main_queue(), ^{
        if( [delegate respondsToSelector:@selector( OnTranslateTextComplete:requestID:text:srcLangCode:destLangCode:)]){
            [delegate OnTranslateTextComplete:(YIMErrorcodeOC)errorcode requestID:requestID text:[NSString stringWithUTF8String:text] srcLangCode:(LanguageCodeOC)srcLangCode destLangCode:(LanguageCodeOC)destLangCode];
        }
        translateTextCompleteCBType callblock = [[YIMCallbackBlock GetInstance].translateTextCompleteCBBlocks objectForKey:[NSNumber numberWithUnsignedLongLong:requestID]];
        if(callblock){
            callblock((YIMErrorcodeOC)errorcode, requestID, [NSString stringWithUTF8String:text], (LanguageCodeOC)srcLangCode, (LanguageCodeOC)destLangCode);
            callblock = nil;
            [[YIMCallbackBlock GetInstance].translateTextCompleteCBBlocks removeObjectForKey:[NSNumber numberWithUnsignedLongLong:requestID]];
        }
    });
}

//举报处理结果通知
void YIMImplement::OnAccusationResultNotify(AccusationDealResult result, const XCHAR* userID, unsigned int accusationTime)
{
    NSString *user = [NSString stringWithUTF8String:userID];
    dispatch_sync(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnAccusationResultNotify:userID:accusationTime:)]) {
            [delegate OnAccusationResultNotify:(AccusationDealResultOC)result userID:user accusationTime:accusationTime];
        }
    });
}

void YIMImplement::OnGetForbiddenSpeakInfo( YIMErrorcode errorcode, std::vector< std::shared_ptr<IYIMForbidSpeakInfo> > vecForbiddenSpeakInfos ){
    NSMutableArray* arr = [NSMutableArray arrayWithCapacity:10];
    auto it = vecForbiddenSpeakInfos.begin();
    for(; it != vecForbiddenSpeakInfos.end(); ++it ){
        ForbiddenSpeakInfoOC* info = [ForbiddenSpeakInfoOC new];
        
        info.channelID = [NSString stringWithUTF8String:(*it)->GetChannelID()];
        info.isForbidRoom = (*it)->GetIsForbidRoom();
        info.reasonType = (*it)->GetReasonType();
        info.endTime = (*it)->GetEndTime();

        [arr addObject: info ];
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnGetForbiddenSpeakInfo:forbiddenSpeakArray:)]){
            [delegate OnGetForbiddenSpeakInfo:(YIMErrorcodeOC)errorcode  forbiddenSpeakArray:arr ];
        }
        if([YIMCallbackBlock GetInstance].getForbiddenSpeakInfoCB){
            [YIMCallbackBlock GetInstance].getForbiddenSpeakInfoCB((YIMErrorcodeOC)errorcode, arr);
            [YIMCallbackBlock GetInstance].getForbiddenSpeakInfoCB = nil;
        }
    });
}


//IYIMMessageCallback
//发送消息状态
void YIMImplement::OnSendMessageStatus(XUINT64 requestID, YIMErrorcode errorcode, unsigned int sendTime, bool isForbidRoom,  int reasonType, XUINT64 forbidEndTime, XUINT64 messageID) {
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnSendMessageStatus:errorcode:sendTime:isForbidRoom:reasonType:forbidEndTime:)]){
            [delegate OnSendMessageStatus:requestID errorcode:(YIMErrorcodeOC)errorcode sendTime:sendTime isForbidRoom:isForbidRoom reasonType:reasonType forbidEndTime:forbidEndTime];
        }
        sendMessageStatusCBType callblock = [[YIMCallbackBlock GetInstance].sendMessageCBBlocks objectForKey:[NSNumber numberWithUnsignedLongLong:requestID]];
        if(callblock){
            callblock((YIMErrorcodeOC)errorcode,sendTime,isForbidRoom,reasonType,forbidEndTime,messageID);
            callblock = nil;
            [[YIMCallbackBlock GetInstance].sendMessageCBBlocks removeObjectForKey:[NSNumber numberWithUnsignedLongLong:requestID]];
        }
    });
}

void YIMImplement::OnUploadProgress(XUINT64 requestID, float percent) {
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnUploadProgress:percent:)]){
            [delegate OnUploadProgress:requestID percent:percent];
        }

        uploadProgressCBType callblock = [[YIMCallbackBlock GetInstance].uploadProgressCBBlocks objectForKey:[NSNumber numberWithUnsignedLongLong:requestID]];
        if(callblock){
            callblock(percent);

            if (percent >= 100.00) {
                callblock = nil;
                [[YIMCallbackBlock GetInstance].uploadProgressCBBlocks removeObjectForKey:[NSNumber numberWithUnsignedLongLong:requestID]];
            }
        }
    });
}

//屏蔽/解除屏蔽用户消息回调
void YIMImplement::OnBlockUser(YIMErrorcode errorcode, const XCHAR* userID, bool block)
{
    NSString *user = [NSString stringWithUTF8String:userID];
    dispatch_sync(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnBlockUser:userID:block:)]) {
            [delegate OnBlockUser:(YIMErrorcodeOC)errorcode userID:user block:block];
        }
        NSString *strKey;
        strKey = [NSString stringWithFormat:@"%@_%d",user,block];
        NSLog(@"屏蔽用户回调，strKey:%@",strKey);
        blockUserCBType callblock = [[YIMCallbackBlock GetInstance].blockUserCBBlocks objectForKey:strKey];
        if(callblock)
        {
            callblock((YIMErrorcodeOC)errorcode, user, block);
            callblock = nil;
            [[YIMCallbackBlock GetInstance].blockUserCBBlocks removeObjectForKey:strKey];
        }
    });
}

//解除所有已屏蔽用户回调
void YIMImplement::OnUnBlockAllUser(YIMErrorcode errorcode)
{
    dispatch_sync(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnUnBlockAllUser:)]) {
            [delegate OnUnBlockAllUser:(YIMErrorcodeOC)errorcode];
        }
        if([YIMCallbackBlock GetInstance].unBlockAllUserCB){
            [YIMCallbackBlock GetInstance].unBlockAllUserCB((YIMErrorcodeOC) errorcode);
            [YIMCallbackBlock GetInstance].unBlockAllUserCB = nil;
        }
    });
}

//获取被屏蔽消息用户回调
void YIMImplement::OnGetBlockUsers(YIMErrorcode errorcode, std::list<XString> userList)
{
    NSMutableArray* arr = [NSMutableArray arrayWithCapacity:10];
    for(std::list<XString>::const_iterator it = userList.begin(); it != userList.end(); ++it ){
        NSString *userID = [NSString stringWithUTF8String:it->c_str()];
        [arr addObject:userID];
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnGetBlockUsers:userList:)]){
            [delegate OnGetBlockUsers:(YIMErrorcodeOC)errorcode userList:arr];
        }
        if([YIMCallbackBlock GetInstance].getBlockUsersCB){
            [YIMCallbackBlock GetInstance].getBlockUsersCB((YIMErrorcodeOC) errorcode, arr);
            [YIMCallbackBlock GetInstance].getBlockUsersCB = nil;
        }
    });
}

//音量回调
void YIMImplement::OnRecordVolumeChange(float volume)
{
    if ([delegate respondsToSelector:@selector(OnRecordVolumeChange:)]) {
        [delegate OnRecordVolumeChange:volume];
    }
}

YIMMessage* ConvertMessageToOC( IYIMMessage* pMessage ){
    YIMMessageBodyType msgType = pMessage->GetMessageBody()->GetMessageType();
    YIMMessageBodyBase* msgBody = NULL;
    switch ( msgType ) {
        case YIMMessageBodyType::MessageBodyType_TXT:
        {
            YIMMessageBodyText *txtMsg = [YIMMessageBodyText new];
            txtMsg.messageType = (YIMMessageBodyTypeOC)pMessage->GetMessageBody()->GetMessageType();
            
            IYIMMessageBodyText * pMsgText =  (IYIMMessageBodyText *)pMessage->GetMessageBody();
            txtMsg.messageContent = [NSString stringWithUTF8String:pMsgText->GetMessageContent()];
            txtMsg.attachParam = [NSString stringWithUTF8String:pMsgText->GetAttachParam()];
            msgBody = txtMsg;
        }
            break;
        case YIMMessageBodyType::MessageBodyType_Voice:
        {
            YIMMessageBodyAudio *voiceMsg = [YIMMessageBodyAudio new];
            voiceMsg.messageType = (YIMMessageBodyTypeOC)pMessage->GetMessageBody()->GetMessageType();
            
            IYIMMessageBodyAudio * pMsgVoice =  (IYIMMessageBodyAudio *)pMessage->GetMessageBody();
            voiceMsg.textContent = [NSString stringWithUTF8String:pMsgVoice->GetText()];
            voiceMsg.customContent = [NSString stringWithUTF8String:pMsgVoice->GetExtraParam()];
            voiceMsg.fileSize = pMsgVoice->GetFileSize();
            voiceMsg.audioTime = pMsgVoice->GetAudioTime();

            msgBody = voiceMsg;
        }
            break;
        case YIMMessageBodyType::MessageBodyType_File:{
            YIMMessageBodyFile *fileMsg = [YIMMessageBodyFile new];
            fileMsg.messageType = (YIMMessageBodyTypeOC)pMessage->GetMessageBody()->GetMessageType();
            
            IYIMMessageBodyFile * pMsgFile =  (IYIMMessageBodyFile *)pMessage->GetMessageBody();
            fileMsg.fileSize = pMsgFile->GetFileSize();
            fileMsg.fileName = [NSString stringWithUTF8String:pMsgFile->GetFileName()];
            fileMsg.fileType = (YIMFileTypeOC)pMsgFile->GetFileType();
            
            fileMsg.fileExtension = [NSString stringWithUTF8String:pMsgFile->GetFileExtension()];
            fileMsg.extraParam = [NSString stringWithUTF8String:pMsgFile->GetExtraParam()];

            msgBody = fileMsg;
        }
            break;
        case YIMMessageBodyType::MessageBodyType_Gift:
        {
            YIMMessageBodyGift *giftMsg = [YIMMessageBodyGift new];
            giftMsg.messageType = (YIMMessageBodyTypeOC)pMessage->GetMessageBody()->GetMessageType();
            
            IYIMMessageGift * pMsgGift =  (IYIMMessageGift *)pMessage->GetMessageBody();
            giftMsg.giftID = pMsgGift->GetGiftID();
            giftMsg.giftCount = pMsgGift->GetGiftCount();
            giftMsg.anchor = [NSString stringWithUTF8String: pMsgGift->GetAnchor() ];
            giftMsg.extraParam = [NSString stringWithUTF8String: pMsgGift->GetExtraParam() ];
            
            msgBody = giftMsg;
        }
            break;
        case YIMMessageBodyType::MessageBodyType_CustomMesssage:
        {
            YIMMessageBodyCustom *customMsg = [YIMMessageBodyCustom new];
            customMsg.messageType = (YIMMessageBodyTypeOC)pMessage->GetMessageBody()->GetMessageType();
            
            IYIMMessageBodyCustom * pMsgCustom =  (IYIMMessageBodyCustom *)pMessage->GetMessageBody();
            
            std::string str = pMsgCustom->GetCustomMessage();
            customMsg.messageContent = [NSData dataWithBytes: str.c_str() length:str.length() ];
            
            msgBody = customMsg;
        }
            break;
        default:
            break;
    }
    
    YIMMessage* message = [YIMMessage new];
    message.receiverID = [NSString stringWithUTF8String:pMessage->GetReceiveID()];
    message.senderID = [NSString stringWithUTF8String:pMessage->GetSenderID()];
    message.chatType = (YIMChatTypeOC)pMessage->GetChatType();
    message.messageID = pMessage->GetMessageID();
    message.createTime = pMessage->GetCreateTime();
    message.messageBody = msgBody;
    message.distance = pMessage->GetDistance();
    message.isRead = pMessage->IsRead();

    return message;
}
//接收到用户发来的消息
void YIMImplement::OnRecvMessage( std::shared_ptr<IYIMMessage> pMessage) {
    YIMMessage* message = ConvertMessageToOC( pMessage.get() );
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnRecvMessage:)]){
            [delegate OnRecvMessage:message];
        }
    });

}

//语音消息的回掉
void YIMImplement::OnSendAudioMessageStatus(XUINT64 requestID, YIMErrorcode errorcode, const XCHAR* text,  const XCHAR* audioPath, unsigned int audioTime, unsigned int sendTime, bool isForbidRoom,  int reasonType, XUINT64 forbidEndTime,XUINT64 messageID)
{
    NSString* messageTxt = [NSString stringWithUTF8String:text];
    NSString* audioPathStr = [NSString stringWithUTF8String:audioPath];
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnSendAudioMessageStatus:errorcode:strMessage:strAudioPath:audioTime:sendTime:isForbidRoom:reasonType:forbidEndTime:)]){
            [delegate OnSendAudioMessageStatus:requestID errorcode:(YIMErrorcodeOC)errorcode strMessage:messageTxt strAudioPath:audioPathStr audioTime:audioTime sendTime:sendTime isForbidRoom:isForbidRoom reasonType:reasonType forbidEndTime:forbidEndTime];
        }
        
        sendAudioMsgStatusCBType callblock = [[YIMCallbackBlock GetInstance].sendAudioMsgCBBlocks objectForKey:[NSNumber numberWithUnsignedLongLong:requestID]];
        if(callblock)
        {
            callblock((YIMErrorcodeOC)errorcode,messageTxt,audioPathStr,audioTime,sendTime,isForbidRoom,reasonType,forbidEndTime, messageID);
            callblock = nil;
            [[YIMCallbackBlock GetInstance].sendAudioMsgCBBlocks removeObjectForKey:[NSNumber numberWithUnsignedLongLong:requestID]];
        }
    });
}

void YIMImplement::OnStartSendAudioMessage(XUINT64 requestID, YIMErrorcode errorcode,  const XCHAR* text, const XCHAR* audioPath, unsigned int audioTime) {
    NSString* messageTxt = [NSString stringWithUTF8String:text];
    NSString* audioPathStr = [NSString stringWithUTF8String:audioPath];
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnStartSendAudioMessage:errorcode:strMessage:strAudioPath:audioTime:)]){
            [delegate OnStartSendAudioMessage:requestID errorcode:(YIMErrorcodeOC)errorcode strMessage:messageTxt strAudioPath:audioPathStr audioTime:audioTime ];
        }
        startSendAudioMsgCBType callblock = [[YIMCallbackBlock GetInstance].startSendAudioMsgCBBlocks objectForKey:[NSNumber numberWithUnsignedLongLong:requestID]];
        if(callblock){
            callblock((YIMErrorcodeOC)errorcode,messageTxt,audioPathStr,audioTime);
            callblock = nil;
            [[YIMCallbackBlock GetInstance].startSendAudioMsgCBBlocks removeObjectForKey:[NSNumber numberWithUnsignedLongLong:requestID]];
        }
    });
}

void YIMImplement::OnQueryHistoryMessage(YIMErrorcode errorcode, const XCHAR* targetID, int remain, std::list<std::shared_ptr<IYIMMessage> > messageList){
    NSString* target = [NSString stringWithUTF8String:targetID];
    
    NSMutableArray* arr = [NSMutableArray arrayWithCapacity:10];
    std::list<std::shared_ptr<IYIMMessage> >::iterator it = messageList.begin();
    for(; it != messageList.end(); ++it ){
        YIMMessage* msg = ConvertMessageToOC( it->get() );
        [arr addObject: msg ];
    }

    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnQueryHistoryMessage:targetID:remain:messageArray:)]){
            [delegate OnQueryHistoryMessage:(YIMErrorcodeOC)errorcode targetID:target remain:remain messageArray:arr ];
        }
        queryHistoryMsgCBType callblock = [[YIMCallbackBlock GetInstance].queryHistoryMsgCBBlocks objectForKey:target];
        if(callblock)
        {
           callblock((YIMErrorcodeOC)errorcode, target, remain, arr);
        }
    });
}

//获取房间历史纪录回调
void YIMImplement::OnQueryRoomHistoryMessageFromServer(YIMErrorcode errorcode, const XCHAR* roomID, int remain, std::list<std::shared_ptr<IYIMMessage> >& messageList)
{
    NSString* nsRoomID = [NSString stringWithUTF8String:roomID];
    NSMutableArray* messageArray = [NSMutableArray arrayWithCapacity:messageList.size()];
    for (std::list<std::shared_ptr<IYIMMessage> >::iterator it = messageList.begin(); it != messageList.end(); ++it)
    {
        YIMMessage* msg = ConvertMessageToOC(it->get());
        [messageArray addObject: msg];
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnQueryRoomHistoryMessageFromServer:roomID:remain:messageArray:)]){
            [delegate OnQueryRoomHistoryMessageFromServer:(YIMErrorcodeOC)errorcode roomID:nsRoomID remain:remain messageArray:messageArray];
        }
        queryRoomHistoryMsgCBType callblock = [[YIMCallbackBlock GetInstance].queryRoomHistoryMsgCBBlocks objectForKey:nsRoomID];
        if(callblock)
        {
            callblock((YIMErrorcodeOC)errorcode, nsRoomID, remain, messageArray);
        }
    });
}

//语音上传后回调
void YIMImplement::OnStopAudioSpeechStatus(YIMErrorcode errorcode, std::shared_ptr<IAudioSpeechInfo> audioSpeechInfo){
    AudioSpeechInfo* info = [AudioSpeechInfo new ];
    info.audioTime = audioSpeechInfo->GetAudioTime();
    info.requestID = audioSpeechInfo->GetRequestID();
    info.fileSize = audioSpeechInfo->GetFileSize();
    
    info.textContent = [NSString stringWithUTF8String:audioSpeechInfo->GetText()];
    info.downloadURL =  [NSString stringWithUTF8String:audioSpeechInfo->GetDownloadURL()];
    info.localPath = [NSString stringWithUTF8String:audioSpeechInfo->GetLocalPath()];

    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnStopAudioSpeechStatus:audioSpeechInfo:)]){
            [delegate OnStopAudioSpeechStatus:(YIMErrorcodeOC)errorcode  audioSpeechInfo:info ];
        }
        uploadSpeechStatusCBType callblock = [[YIMCallbackBlock GetInstance].uploadSpeechStatusCBBlocks objectForKey:[NSNumber numberWithUnsignedLongLong:info.requestID]];
        if(callblock){
            callblock((YIMErrorcodeOC)errorcode,info);
            callblock = nil;
            [[YIMCallbackBlock GetInstance].uploadSpeechStatusCBBlocks removeObjectForKey:[NSNumber numberWithUnsignedLongLong:info.requestID]];
        }
    });
}


void YIMImplement::OnReceiveMessageNotify(YIMChatType chatType, const XCHAR* targetID){
    NSString* target =  [NSString stringWithUTF8String:targetID];

    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnReceiveMessageNotify:targetID:)]){
            [delegate OnReceiveMessageNotify:(YIMChatTypeOC)chatType targetID:target];
        }
    });
}

void  YIMImplement::OnUpdateReadStatus(const XCHAR* recvId, int chatType, XUINT64 msgSerial) {
    NSString* recvID = [NSString stringWithUTF8String:recvId];
    dispatch_async(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnUpdateReadStatus:chatType:msgSerial:)]) {
            [delegate OnUpdateReadStatus:recvID chatType:(YIMChatTypeOC)chatType msgSerial:msgSerial];
        }
    });
}

//语音文本识别回调
void YIMImplement::OnGetRecognizeSpeechText(XUINT64 requestID, YIMErrorcode errorcode,const XCHAR* text) {
    NSString* content = [NSString stringWithUTF8String:text];
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnGetRecognizeSpeechText:errorcode:text:)]){
            [delegate OnGetRecognizeSpeechText:requestID errorcode:(YIMErrorcodeOC)errorcode text:content];
        }
    });
}

void YIMImplement::OnGetRecentContacts(YIMErrorcode errorcode, std::list<std::shared_ptr<IYIMContactsMessageInfo> >& contactList){
    NSMutableArray* contactsArray = [NSMutableArray arrayWithCapacity:contactList.size()];
    std::list<std::shared_ptr<IYIMContactsMessageInfo> >::iterator it = contactList.begin();
    for(; it != contactList.end(); ++it){
        YIMContactSessionInfoOC* contactSession = [YIMContactSessionInfoOC new];
        contactSession.contactID = [NSString stringWithUTF8String:(*it)->GetContactID()];
        contactSession.messageType = (YIMMessageBodyTypeOC)(*it)->GetMessageType();
        contactSession.messageContent = [NSString stringWithUTF8String:(*it)->GetMessageContent()];
        contactSession.createTime = (*it)->GetCreateTime();
        contactSession.notReadMsgNum = (*it)->GetNotReadMsgNum();
        [contactsArray addObject: contactSession];
    }

    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnGetRecentContacts:contactArray:)]){
            [delegate OnGetRecentContacts:(YIMErrorcodeOC)errorcode  contactArray:contactsArray];
        }
        if([YIMCallbackBlock GetInstance].getContactsCB){
            [YIMCallbackBlock GetInstance].getContactsCB((YIMErrorcodeOC) errorcode, contactsArray);
            [YIMCallbackBlock GetInstance].getContactsCB = nil;
        }
    });
}

//获取用户信息回调(用户信息为JSON格式)
void YIMImplement::OnGetUserInfo(YIMErrorcode errorcode, const XCHAR* userID, const XCHAR* userInfo){
    NSString* info = [NSString stringWithUTF8String:userInfo];
    NSString* user = [NSString stringWithUTF8String:userID];
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnGetUserInfo:userID:userInfo:)]){
            [delegate OnGetUserInfo:(YIMErrorcodeOC)errorcode userID:user  userInfo:info ];
        }
        getUserInfoCBType callblock = [[YIMCallbackBlock GetInstance].getUserInfoCBBlocks objectForKey:user];
        if(callblock)
        {
            callblock((YIMErrorcodeOC)errorcode, user, info);
            callblock = nil;
            [[YIMCallbackBlock GetInstance].getUserInfoCBBlocks removeObjectForKey:user];
        }
    });
}
//查询用户状态回调
void YIMImplement::OnQueryUserStatus(YIMErrorcode errorcode, const XCHAR* userID, YIMUserStatus status){
    NSString* user = [NSString stringWithUTF8String:userID];
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnQueryUserStatus:userID:status:)]){
            [delegate OnQueryUserStatus:(YIMErrorcodeOC)errorcode  userID:user status:(YIMUserStatusOC)status];
        }
        queryUserStatusCBType callblock = [[YIMCallbackBlock GetInstance].queryUserStatusCBBlocks objectForKey:user];
        if(callblock)
        {
            callblock((YIMErrorcodeOC)errorcode, user, (YIMUserStatusOC)status);
            callblock = nil;
            [[YIMCallbackBlock GetInstance].getUserInfoCBBlocks removeObjectForKey:user];
        }
    });
}


//IYIMDownloadCallback
void YIMImplement::OnDownload(YIMErrorcode errorcode, std::shared_ptr<IYIMMessage> msg, const XCHAR* savePath){
    YIMMessage* message = ConvertMessageToOC( msg.get() );
    NSString* path = [NSString stringWithUTF8String:savePath];
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnDownload:pMessage:strSavePath:)]){
            [delegate OnDownload:(YIMErrorcodeOC)errorcode pMessage:message strSavePath:path];
        }
        downloadCBType callblock = [[YIMCallbackBlock GetInstance].downloadCBBlocks objectForKey:[NSNumber numberWithUnsignedLongLong:msg->GetMessageID()]];
        if(callblock){
            callblock((YIMErrorcodeOC)errorcode,message,path);
            callblock = nil;
            [[YIMCallbackBlock GetInstance].downloadCBBlocks removeObjectForKey:[NSNumber numberWithUnsignedLongLong:msg->GetMessageID()]];
        }
    });
}

void YIMImplement::OnDownloadByUrl( YIMErrorcode errorcode, const XCHAR* strFromUrl, const XCHAR* savePath, int iAudioTime ) {
    NSString* url = [NSString stringWithUTF8String:strFromUrl];
    NSString* path = [NSString stringWithUTF8String:savePath];
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnDownloadByUrl:strFromUrl:strSavePath:)]){
            [delegate OnDownloadByUrl: (YIMErrorcodeOC)errorcode strFromUrl:url strSavePath:path];
        }
        downloadByUrlCBType callblock = [[YIMCallbackBlock GetInstance].downloadByUrlCBBlocks objectForKey:url];
        if(callblock){
            callblock((YIMErrorcodeOC)errorcode,url,path,iAudioTime);
            callblock = nil;
            [[YIMCallbackBlock GetInstance].downloadByUrlCBBlocks removeObjectForKey:url];
        }
    });
}

void YIMImplement::OnPlayCompletion(YIMErrorcode errorcode, const XCHAR* path){
    NSString* playPath = [NSString stringWithUTF8String:path];
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnPlayCompletion:path:)]){
            [delegate OnPlayCompletion:(YIMErrorcodeOC)errorcode  path:playPath ];
        }
        playCompleteCBType callblock = [[YIMCallbackBlock GetInstance].playCompleteCBBlocks objectForKey:playPath];
        if(callblock){
            callblock((YIMErrorcodeOC)errorcode, playPath);
            callblock = nil;
            [[YIMCallbackBlock GetInstance].playCompleteCBBlocks removeObjectForKey:playPath];
        }
    });
}

void YIMImplement::OnGetMicrophoneStatus(AudioDeviceStatus status)
{
    dispatch_async(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnGetMicrophoneStatus:)]) {
            [delegate OnGetMicrophoneStatus:(AudioDeviceStatusOC)status];
        }
        if([YIMCallbackBlock GetInstance].getMicStatusCB){
            [YIMCallbackBlock GetInstance].getMicStatusCB((AudioDeviceStatusOC)status);
            [YIMCallbackBlock GetInstance].getMicStatusCB = nil;
        }
    });
}


void YIMImplement::OnUpdateLocation(YIMErrorcode errorcode, std::shared_ptr<GeographyLocation> location) {
    GeographyLocationOC* info = [GeographyLocationOC new ];
    info.longitude = location->GetLongitude();
    info.latitude = location->GetLatitude();
    info.districtCode = location->GetDistrictCode();
    info.country =  [NSString stringWithUTF8String: location->GetCountry() ];
    info.province = [NSString stringWithUTF8String: location->GetProvince() ];
    info.city = [NSString stringWithUTF8String: location->GetCity() ];
    info.districtCounty = [ NSString stringWithUTF8String: location->GetDistrictCounty() ];
    info.street = [ NSString stringWithUTF8String: location->GetStreet() ];
    
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnUpdateLocation:location:)]){
            [delegate OnUpdateLocation:(YIMErrorcodeOC)errorcode  location:info ];
        }
    });

}
// 获取附近目标回调
void YIMImplement::OnGetNearbyObjects(YIMErrorcode errorcode,  std::list< std::shared_ptr<RelativeLocation> > neighbourList, unsigned int startDistance, unsigned int endDistance) {
    NSMutableArray* arr = [NSMutableArray arrayWithCapacity:10];
    std::list<  std::shared_ptr<RelativeLocation> >::iterator it = neighbourList.begin();
    for(; it != neighbourList.end(); ++it ){
        RelativeLocationOC* info = [RelativeLocationOC new ];
        auto location = *it;
        info.userID = [ NSString stringWithUTF8String: location->GetUserID()];
        info.distance = location->GetDistance();
        info.longitude = location->GetLongitude();
        info.latitude = location->GetLatitude();
        info.country =  [NSString stringWithUTF8String: location->GetCountry() ];
        info.province = [NSString stringWithUTF8String: location->GetProvince() ];
        info.city = [NSString stringWithUTF8String: location->GetCity() ];
        info.districtCounty = [ NSString stringWithUTF8String: location->GetDistrictCounty() ];
        info.street = [ NSString stringWithUTF8String: location->GetStreet() ];
        
        [arr addObject: info ];
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnGetNearbyObjects:neighbourList:startDistance:endDistance:)]){
            [delegate OnGetNearbyObjects:(YIMErrorcodeOC)errorcode  neighbourList:arr startDistance:startDistance  endDistance:endDistance];
        }
        if([YIMCallbackBlock GetInstance].getNearbyObjectsCB){
            [YIMCallbackBlock GetInstance].getNearbyObjectsCB((YIMErrorcodeOC)errorcode, arr, startDistance, endDistance);
            [YIMCallbackBlock GetInstance].getNearbyObjectsCB = nil;
        }
    });
}

void YIMImplement::OnGetDistance(YIMErrorcode errorcode, const XCHAR* userID, unsigned int distance)
{
    NSString *nsUserID = [NSString stringWithUTF8String:userID];
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnGetDistance:userID:distance:)]){
            [delegate OnGetDistance:(YIMErrorcodeOC)errorcode userID:nsUserID distance:distance];
        }
        getDistanceCBType callblock = [[YIMCallbackBlock GetInstance].getDistanceCBBlocks objectForKey:nsUserID];
        if(callblock){
            callblock((YIMErrorcodeOC)errorcode, nsUserID, distance);
            callblock = nil;
            [[YIMCallbackBlock GetInstance].getDistanceCBBlocks removeObjectForKey:nsUserID];
        }
    });
}


// 收到公告
void YIMImplement::OnRecvNotice(YIMNotice* notice)
{
    YIMNoticeOC *noticeInfo = [YIMNoticeOC new];
    noticeInfo.noticeID = notice->GetNoticeID();
    noticeInfo.noticeType = notice->GetNoticeType();
    noticeInfo.channelID = [NSString stringWithUTF8String:notice->GetChannelID()];
    noticeInfo.content = [NSString stringWithUTF8String:notice->GetContent()];
    noticeInfo.linkeText = [NSString stringWithUTF8String:notice->GetLinkText()];
    noticeInfo.linkAddr = [NSString stringWithUTF8String:notice->GetLinkAddr()];
    noticeInfo.beginTime = notice->GetBeginTime();
    noticeInfo.endTime = notice->GetEndTime();
    
    dispatch_async(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnRecvNotice:)]) {
            [delegate OnRecvNotice:noticeInfo];
        }
    });
}

// 撤销公告（置顶公告）
void YIMImplement::OnCancelNotice(XUINT64 noticeID, const XCHAR* channelID)
{
    NSString *channel = [NSString stringWithUTF8String:channelID];
    dispatch_async(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnCancelNotice:channelID:)]) {
            [delegate OnCancelNotice:noticeID channelID:channel];
        }
    });
}


void YIMImplement::OnKickOff(){
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnKickOff)]){
            [delegate OnKickOff];
        }
    });
}

// 开始重连
void YIMImplement::OnStartReconnect()
{
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnStartReconnect)]){
            [delegate OnStartReconnect];
        }
    });
}

// 收到重连结果
void YIMImplement::OnRecvReconnectResult(ReconnectResult result)
{
    dispatch_sync(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnRecvReconnectResult:)]) {
            [delegate OnRecvReconnectResult:(ReconnectResultOC)result];
        }
    });    
}

// 用户信息管理回调
void YIMImplement::OnSetUserInfo(YIMErrorcode errorcode)
{
    dispatch_sync(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnSetUserInfo:)]) {
            [delegate OnSetUserInfo:(YIMErrorcodeOC)errorcode];
        }
        if([YIMCallbackBlock GetInstance].setUserProfileCB){
            [YIMCallbackBlock GetInstance].setUserProfileCB((YIMErrorcodeOC)errorcode);
            [YIMCallbackBlock GetInstance].setUserProfileCB = nil;
        }
    });
}
void YIMImplement::OnQueryUserInfo(YIMErrorcode errorcode, const IMUserProfileInfo &userInfo)
{
    IMUserProfileInfoOC *tmp_info = [IMUserProfileInfoOC new];
    tmp_info.userID = [NSString stringWithUTF8String:userInfo.userID.c_str()];
    tmp_info.photoURL = [NSString stringWithUTF8String:userInfo.photoURL.c_str()];
    tmp_info.onlineState = (YIMUserStatusOC)userInfo.onlineState;
    tmp_info.beAddPermission = (IMUserBeAddPermissionOC)userInfo.beAddPermission;
    tmp_info.foundPermission = (IMUserFoundPermissionOC)userInfo.foundPermission;
    
    IMUserSettingInfoOC *settingInfo = [IMUserSettingInfoOC new];
    settingInfo.nickName = [NSString stringWithUTF8String:userInfo.settingInfo.nickName.c_str()];
    settingInfo.sex = (IMUserSexOC)userInfo.settingInfo.sex;
    settingInfo.personalSignature = [NSString stringWithUTF8String:userInfo.settingInfo.personalSignature.c_str()];
    settingInfo.country = [NSString stringWithUTF8String:userInfo.settingInfo.country.c_str()];
    settingInfo.province = [NSString stringWithUTF8String:userInfo.settingInfo.province.c_str()];
    settingInfo.city = [NSString stringWithUTF8String:userInfo.settingInfo.city.c_str()];
    settingInfo.extraInfo = [NSString stringWithUTF8String:userInfo.settingInfo.extraInfo.c_str()];
    
    tmp_info.settingInfo = settingInfo;
    
    dispatch_sync(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnQueryUserInfo:userInfo:)]) {
            [delegate OnQueryUserInfo:(YIMErrorcodeOC)errorcode userInfo:tmp_info];
        }
        queryUserProfileCBType callblock = [[YIMCallbackBlock GetInstance].queryUserProfileCBBlocks objectForKey:tmp_info.userID];
        if(callblock){
            callblock((YIMErrorcodeOC)errorcode, tmp_info);
            callblock = nil;
            [[YIMCallbackBlock GetInstance].queryUserProfileCBBlocks removeObjectForKey:tmp_info.userID];
        }
    });
}
void YIMImplement::OnSwitchUserOnlineState(YIMErrorcode errorcode)
{
    dispatch_sync(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnSwitchUserOnlineState:)]) {
            [delegate OnSwitchUserOnlineState:(YIMErrorcodeOC)errorcode];
        }
        if([YIMCallbackBlock GetInstance].switchUserOnlineStateCB){
            [YIMCallbackBlock GetInstance].switchUserOnlineStateCB((YIMErrorcodeOC)errorcode);
            [YIMCallbackBlock GetInstance].switchUserOnlineStateCB = nil;
        }
    });
}
void YIMImplement::OnSetPhotoUrl(YIMErrorcode errorcode, const XCHAR* photoUrl)
{
    NSString *photoURL = [NSString stringWithUTF8String:photoUrl];
    dispatch_sync(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnSetPhotoUrl:photoUrl:)]) {
            [delegate OnSetPhotoUrl:(YIMErrorcodeOC)errorcode photoUrl:photoURL];
        }
        if([YIMCallbackBlock GetInstance].setPhotoUrlCB){
            [YIMCallbackBlock GetInstance].setPhotoUrlCB((YIMErrorcodeOC)errorcode, photoURL);
            [YIMCallbackBlock GetInstance].setPhotoUrlCB = nil;
        }
    });
}

void YIMImplement::OnUserInfoChangeNotify(const XCHAR* userID)
{
    NSString *strUserID = [NSString stringWithUTF8String:userID];
    
    dispatch_sync(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnUserInfoChangeNotify:)]) {
            [delegate OnUserInfoChangeNotify:strUserID];
        }
    });
}

void YIMImplement::OnFindUser(YIMErrorcode errorcode, std::list<std::shared_ptr<IYIMUserBriefInfo> >& users)
{
    NSMutableArray* usersArray = [NSMutableArray arrayWithCapacity:users.size()];
    std::list<std::shared_ptr<IYIMUserBriefInfo> >::iterator it = users.begin();
    for(; it != users.end(); ++it){
        IMUserBriefInfoOC* briefInfo = [IMUserBriefInfoOC new];
        briefInfo.userID = [NSString stringWithUTF8String:(*it)->GetUserID()];
        briefInfo.nickName = [NSString stringWithUTF8String:(*it)->GetNickname()];
        briefInfo.userStatus = (YIMUserStatusOC)(*it)->GetUserStatus();
        
        [usersArray addObject: briefInfo];
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnFindUser:users:)]){
            [delegate OnFindUser:(YIMErrorcodeOC)errorcode users:usersArray];
        }
        if([YIMCallbackBlock GetInstance].findUserCB){
            [YIMCallbackBlock GetInstance].findUserCB((YIMErrorcodeOC)errorcode, usersArray);
            [YIMCallbackBlock GetInstance].findUserCB = nil;
        }
    });
}

void YIMImplement::OnRequestAddFriend(YIMErrorcode errorcode, const XCHAR* userID)
{
    NSString *strUserID = [NSString stringWithUTF8String:userID];
    dispatch_sync(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnRequestAddFriend:userID:)]) {
            [delegate OnRequestAddFriend:(YIMErrorcodeOC)errorcode userID:strUserID];
        }
        if([YIMCallbackBlock GetInstance].requestAddFriendCB){
            [YIMCallbackBlock GetInstance].requestAddFriendCB((YIMErrorcodeOC)errorcode, strUserID);
            [YIMCallbackBlock GetInstance].requestAddFriendCB = nil;
        }
    });
}

void YIMImplement::OnBeRequestAddFriendNotify(const XCHAR* userID, const XCHAR* comments, XUINT64 reqID)
{
    NSString *strUserID = [NSString stringWithUTF8String:userID];
    NSString *strComments = [NSString stringWithUTF8String:comments];
    
    dispatch_sync(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnBeRequestAddFriendNotify:comments:reqID:)]) {
            [delegate OnBeRequestAddFriendNotify:strUserID comments:strComments reqID: reqID];
        }
    });
}

void YIMImplement::OnBeAddFriendNotify(const XCHAR* userID, const XCHAR* comments)
{
    NSString *strUserID = [NSString stringWithUTF8String:userID];
    NSString *strComments = [NSString stringWithUTF8String:comments];
    
    dispatch_sync(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnBeAddFriendNotify:comments:)]) {
            [delegate OnBeAddFriendNotify:strUserID comments:strComments];
        }
    });
}

void YIMImplement::OnDealBeRequestAddFriend(YIMErrorcode errorcode, const XCHAR* userID, const XCHAR* comments, int dealResult)
{
    NSString *strUserID = [NSString stringWithUTF8String:userID];
    NSString *strComments = [NSString stringWithUTF8String:comments];
    
    dispatch_sync(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnDealBeRequestAddFriend:userID:comments:dealResult:)]) {
            [delegate OnDealBeRequestAddFriend:(YIMErrorcodeOC)errorcode userID:strUserID comments:strComments dealResult:dealResult];
        }
        dealBeRequestAddFriendCBType callblock = [[YIMCallbackBlock GetInstance].dealBeRequestAddFriendCBBlocks objectForKey:strUserID];
        if(callblock){
            callblock((YIMErrorcodeOC)errorcode, strUserID, strComments, dealResult);
            callblock = nil;
            [[YIMCallbackBlock GetInstance].dealBeRequestAddFriendCBBlocks removeObjectForKey:strUserID];
        }
    });
}

void YIMImplement::OnRequestAddFriendResultNotify(const XCHAR* userID, const XCHAR* comments, int dealResult)
{
    NSString *strUserID = [NSString stringWithUTF8String:userID];
    NSString *strComments = [NSString stringWithUTF8String:comments];
    
    dispatch_sync(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnRequestAddFriendResultNotify:comments:dealResult:)]) {
            [delegate OnRequestAddFriendResultNotify:strUserID comments:strComments dealResult:dealResult];
        }
    });
}

void YIMImplement::OnDeleteFriend(YIMErrorcode errorcode, const XCHAR* userID)
{
    NSString *strUserID = [NSString stringWithUTF8String:userID];
    
    dispatch_sync(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnDeleteFriend:userID:)]) {
            [delegate OnDeleteFriend:(YIMErrorcodeOC)errorcode userID:strUserID];
        }
        if([YIMCallbackBlock GetInstance].deleteFriendCB){
            [YIMCallbackBlock GetInstance].deleteFriendCB((YIMErrorcodeOC)errorcode, strUserID);
            [YIMCallbackBlock GetInstance].deleteFriendCB = nil;
        }
    });
}

void YIMImplement::OnBeDeleteFriendNotify(const XCHAR* userID)
{
    NSString *strUserID = [NSString stringWithUTF8String:userID];
    
    dispatch_sync(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnBeDeleteFriendNotify:)]) {
            [delegate OnBeDeleteFriendNotify:strUserID];
        }
    });
}

void YIMImplement::OnBlackFriend(YIMErrorcode errorcode, int type, const XCHAR* userID)
{
    NSString *strUserID = [NSString stringWithUTF8String:userID];
    
    dispatch_sync(dispatch_get_main_queue(), ^{
        if ([delegate respondsToSelector:@selector(OnBlackFriend:type:userID:)]) {
            [delegate OnBlackFriend:(YIMErrorcodeOC)errorcode type:type userID:strUserID];
        }
        if([YIMCallbackBlock GetInstance].blackFriendCB){
            [YIMCallbackBlock GetInstance].blackFriendCB((YIMErrorcodeOC)errorcode, type, strUserID);
            [YIMCallbackBlock GetInstance].blackFriendCB = nil;
        }
    });
}

void YIMImplement::OnQueryFriends(YIMErrorcode errorcode, int type, int startIndex, std::list<std::shared_ptr<IYIMUserBriefInfo> >& friends)
{
    NSMutableArray* friendsArray = [NSMutableArray arrayWithCapacity:friends.size()];
    std::list<std::shared_ptr<IYIMUserBriefInfo> >::iterator it = friends.begin();
    for(; it != friends.end(); ++it){
        IMUserBriefInfoOC* briefInfo = [IMUserBriefInfoOC new];
        briefInfo.userID = [NSString stringWithUTF8String:(*it)->GetUserID()];
        briefInfo.nickName = [NSString stringWithUTF8String:(*it)->GetNickname()];
        briefInfo.userStatus = (YIMUserStatusOC)(*it)->GetUserStatus();
        
        [friendsArray addObject: briefInfo];
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnQueryFriends:type:startIndex:friends:)]){
            [delegate OnQueryFriends:(YIMErrorcodeOC)errorcode type:type startIndex:startIndex friends:friendsArray];
        }
        if([YIMCallbackBlock GetInstance].queryFriendsCB){
            [YIMCallbackBlock GetInstance].queryFriendsCB((YIMErrorcodeOC)errorcode, type, startIndex, friendsArray);
            [YIMCallbackBlock GetInstance].queryFriendsCB = nil;
        }
    });
}

void YIMImplement::OnQueryFriendRequestList(YIMErrorcode errorcode, int startIndex, std::list<std::shared_ptr<IYIMFriendRequestInfo> >& requestList)
{
    NSMutableArray* requestsArray = [NSMutableArray arrayWithCapacity:requestList.size()];
    std::list<std::shared_ptr<IYIMFriendRequestInfo> >::iterator it = requestList.begin();
    for(; it != requestList.end(); ++it){
        IMFriendRequestInfoOC* requestInfo = [IMFriendRequestInfoOC new];
        requestInfo.askerID = [NSString stringWithUTF8String:(*it)->GetAskerID()];
        requestInfo.askerNickname = [NSString stringWithUTF8String:(*it)->GetAskerNickname()];
        requestInfo.inviteeID = [NSString stringWithUTF8String:(*it)->GetInviteeID()];
        requestInfo.inviteeNickname = [NSString stringWithUTF8String:(*it)->GetInviteeNickname()];
        requestInfo.validateInfo = [NSString stringWithUTF8String:(*it)->GetValidateInfo()];
        requestInfo.status = (YIMAddFriendStatusOC)(*it)->GetStatus();
        requestInfo.createTime = (*it)->GetCreateTime();
        
        [requestsArray addObject: requestInfo];
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        if([delegate respondsToSelector:@selector(OnQueryFriendRequestList:startIndex:requestList:)]){
            [delegate OnQueryFriendRequestList:(YIMErrorcodeOC)errorcode startIndex:startIndex requestList:requestsArray];
        }
        if([YIMCallbackBlock GetInstance].queryFriendRequestListCB){
            [YIMCallbackBlock GetInstance].queryFriendRequestListCB((YIMErrorcodeOC)errorcode, startIndex, requestsArray);
            [YIMCallbackBlock GetInstance].queryFriendRequestListCB = nil;
        }
    });
}
