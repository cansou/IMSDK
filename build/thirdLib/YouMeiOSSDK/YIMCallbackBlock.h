//
//  YIMService.m
//  YoumeIMUILib
//
//  Created by winnie on 18/8/22.
//  Copyright © 2018年 winnie. All rights reserved.
//
#import "YIMCallbackProtocol.h"

@interface YIMCallbackBlock : NSObject

+(YIMCallbackBlock*)GetInstance;

typedef void(^loginCBType)(YIMErrorcodeOC errorcode, NSString* userID);
typedef void(^logoutCBType)(YIMErrorcodeOC errorcode);

typedef void(^joinRoomCBType)(YIMErrorcodeOC errorcode,NSString* roomID);
typedef void(^leaveRoomCBType)(YIMErrorcodeOC errorcode,NSString* roomID);
typedef void(^getRoomMemberCountCBType)(YIMErrorcodeOC errorcode, NSString* chatRoomID, unsigned int count);


typedef void(^leaveAllRoomCBType)(YIMErrorcodeOC errorcode);

typedef void(^sendMessageStatusCBType)(YIMErrorcodeOC errorcode, unsigned int sendTime, bool isForbidRoom,int reasonType,unsigned long long forbidEndTime,unsigned long long messageID);
typedef void(^uploadProgressCBType)(float percent);
typedef void(^sendAudioMsgStatusCBType)(YIMErrorcodeOC errorcode, NSString* text, NSString* audioPath, unsigned int audioTime, unsigned int sendTime, bool isForbidRoom, int reasonType, unsigned long long forbidEndTime,unsigned long long messageID);
typedef void(^startSendAudioMsgCBType)(YIMErrorcodeOC errorcode, NSString* text, NSString* audioPath, unsigned int audioTime);
typedef void(^uploadSpeechStatusCBType)(YIMErrorcodeOC errorcode,  AudioSpeechInfo* audioSpeechInfo);

typedef void(^downloadCBType)(YIMErrorcodeOC errorcode, YIMMessage* msg, NSString* savePath);
typedef void(^downloadByUrlCBType)(YIMErrorcodeOC errorcode, NSString* strFromUrl, NSString* savePath, int audioTime);

typedef void(^queryHistoryMsgCBType)(YIMErrorcodeOC errorcode, NSString* targetID, int remain, NSArray* messageList); //NSArray<YIMMessage*>
// 此处开始
typedef void(^queryRoomHistoryMsgCBType)(YIMErrorcodeOC errorcode, NSString* roomID, int remain, NSArray* messageList); //NSArray<YIMMessage*>

typedef void(^translateTextCompleteCBType)(YIMErrorcodeOC errorcode, unsigned int requestID, NSString* text, LanguageCodeOC srcLangCode, LanguageCodeOC destLangCode);
typedef void(^blockUserCBType)(YIMErrorcodeOC errorcode, NSString* userID, bool block);
typedef void(^unBlockAllUserCBType)(YIMErrorcodeOC errorcode);
typedef void(^getBlockUsersCBType)(YIMErrorcodeOC errorcode, NSArray* userList); //NSArray<NString*>

typedef void(^getRecentContactsCBType)(YIMErrorcodeOC errorcode, NSArray* contactList); //NSArray<YIMContactSessionInfoOC*>
typedef void(^getUserInfoCBType)(YIMErrorcodeOC errorcode, NSString* userID, NSString*  userInfo);
typedef void(^queryUserStatusCBType)(YIMErrorcodeOC errorcode, NSString* userID, YIMUserStatusOC status);
typedef void(^playCompleteCBType)(YIMErrorcodeOC errorcode, NSString* path);
typedef void(^getMicStatusCBType)(AudioDeviceStatusOC status);
typedef void(^getNearbyObjectsCBType)(YIMErrorcodeOC errorcode, NSArray* neighbourList, unsigned int startDistance, unsigned int endDistance); //NSArray<RelativeLocationOC*>
typedef void(^getDistanceCBType)(YIMErrorcodeOC errorcode, NSString* userID, unsigned int distance);
typedef void(^getForbiddenSpeakInfoCBType)(YIMErrorcodeOC errorcode, NSArray* vecForbiddenSpeakInfos); //NSArray<ForbiddenSpeakInfoOC*>

typedef void(^queryUserProfileCBType)(YIMErrorcodeOC errorcode, IMUserProfileInfoOC* userInfo);
typedef void(^setUserProfileCBType)(YIMErrorcodeOC errorcode);
typedef void(^switchUserOnlineStateCBType)(YIMErrorcodeOC errorcode);
typedef void(^setPhotoUrlCBType)(YIMErrorcodeOC errorcode, NSString* photoUrl);

typedef void(^findUserCBType)(YIMErrorcodeOC errorcode, NSArray* users); // NSArray<IMUserBriefInfoOC*>
typedef void(^requestAddFriendCBType)(YIMErrorcodeOC errorcode, NSString* userID);
typedef void(^dealBeRequestAddFriendCBType)(YIMErrorcodeOC errorcode, NSString* userID, NSString* comments, int dealResult);
typedef void(^deleteFriendCBType)(YIMErrorcodeOC errorcode, NSString* userID);
typedef void(^blackFriendCBType)(YIMErrorcodeOC errorcode, int type, NSString* userID);
typedef void(^queryFriendsCBType)(YIMErrorcodeOC errorcode, int type, int startIndex, NSArray* friends); //NSArray<IMUserBriefInfoOC>
typedef void(^queryFriendRequestListCBType)(YIMErrorcodeOC errorcode, int startIndex, NSArray* requestList); //NSArray<IMFriendRequestInfoOC>


@property (nonatomic, copy) loginCBType loginCB;
@property (nonatomic, copy) logoutCBType logoutCB;

@property (nonatomic,retain) NSMutableDictionary<NSString*,joinRoomCBType>* joinRoomCBBlocks;
@property (nonatomic,retain) NSMutableDictionary<NSString*,leaveRoomCBType>* leaveRoomCBBlocks;
@property (nonatomic,retain) NSMutableDictionary<NSString*,getRoomMemberCountCBType>* getRoomMemberCountCBBlocks;

@property (nonatomic, copy) leaveAllRoomCBType leaveAllRoomCB;

@property (nonatomic,retain) NSMutableDictionary<NSNumber*,sendMessageStatusCBType>* sendMessageCBBlocks;
@property (nonatomic,retain) NSMutableDictionary<NSNumber*,uploadProgressCBType>* uploadProgressCBBlocks;
@property (nonatomic,retain) NSMutableDictionary<NSNumber*,sendAudioMsgStatusCBType>* sendAudioMsgCBBlocks;
@property (nonatomic,retain) NSMutableDictionary<NSNumber*,startSendAudioMsgCBType>* startSendAudioMsgCBBlocks;

@property (nonatomic,retain) NSMutableDictionary<NSNumber*,uploadSpeechStatusCBType>* uploadSpeechStatusCBBlocks;

@property (nonatomic,retain) NSMutableDictionary<NSNumber*,downloadCBType>* downloadCBBlocks;
@property (nonatomic,retain) NSMutableDictionary<NSString*,downloadByUrlCBType>* downloadByUrlCBBlocks;

@property (nonatomic,retain) NSMutableDictionary<NSString*,queryHistoryMsgCBType>* queryHistoryMsgCBBlocks;
@property (nonatomic,retain) NSMutableDictionary<NSString*,queryRoomHistoryMsgCBType>* queryRoomHistoryMsgCBBlocks;

@property (nonatomic,retain) NSMutableDictionary<NSNumber*,translateTextCompleteCBType>* translateTextCompleteCBBlocks;
@property (nonatomic,retain) NSMutableDictionary<NSString*,blockUserCBType>* blockUserCBBlocks;

@property (nonatomic, copy) unBlockAllUserCBType unBlockAllUserCB;
@property (nonatomic, copy) getBlockUsersCBType getBlockUsersCB;
@property (nonatomic, copy) getRecentContactsCBType getContactsCB;
@property (nonatomic,retain) NSMutableDictionary<NSString*,getUserInfoCBType>* getUserInfoCBBlocks;

@property (nonatomic,retain) NSMutableDictionary<NSString*,queryUserStatusCBType>* queryUserStatusCBBlocks;

@property (nonatomic,retain) NSMutableDictionary<NSString*,playCompleteCBType>* playCompleteCBBlocks;

@property (nonatomic, copy) getMicStatusCBType getMicStatusCB;
@property (nonatomic, copy) getNearbyObjectsCBType getNearbyObjectsCB;
@property (nonatomic,retain) NSMutableDictionary<NSString*,getDistanceCBType>* getDistanceCBBlocks;
@property (nonatomic, copy) getForbiddenSpeakInfoCBType getForbiddenSpeakInfoCB;

@property (nonatomic,retain) NSMutableDictionary<NSString*,queryUserProfileCBType>* queryUserProfileCBBlocks;
@property (nonatomic, copy) setUserProfileCBType setUserProfileCB;
@property (nonatomic, copy) switchUserOnlineStateCBType switchUserOnlineStateCB;
@property (nonatomic, copy) setPhotoUrlCBType setPhotoUrlCB;
@property (nonatomic, copy) findUserCBType findUserCB;
@property (nonatomic, copy) requestAddFriendCBType requestAddFriendCB;
@property (nonatomic,retain) NSMutableDictionary<NSString*,dealBeRequestAddFriendCBType>* dealBeRequestAddFriendCBBlocks;

@property (nonatomic, copy) deleteFriendCBType deleteFriendCB;
@property (nonatomic, copy) blackFriendCBType blackFriendCB;
@property (nonatomic, copy) queryFriendsCBType queryFriendsCB;
@property (nonatomic, copy) queryFriendRequestListCBType queryFriendRequestListCB;

@end
