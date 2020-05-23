//
//  YIMImplement.m
//  YoumeIMUILib
//
//  Created by winnie on 18/8/22.
//  Copyright © 2018年 winnie. All rights reserved.
//

#import "YIMCallbackBlock.h"

@implementation YIMCallbackBlock

+(YIMCallbackBlock*)GetInstance{
    static YIMCallbackBlock *_instance = nil;
    @synchronized(self)
    {
        if (_instance == nil)
        {
            _instance = [[self alloc] init];
            _instance.loginCB = nil;
            _instance.logoutCB = nil;
            
            _instance.joinRoomCBBlocks = [[NSMutableDictionary alloc] initWithCapacity:5];
            _instance.leaveRoomCBBlocks = [[NSMutableDictionary alloc]initWithCapacity:5];

            _instance.getRoomMemberCountCBBlocks = [[NSMutableDictionary alloc]initWithCapacity:5];
            _instance.leaveAllRoomCB = nil;
            
            _instance.sendMessageCBBlocks = [[NSMutableDictionary alloc] initWithCapacity:5];
            _instance.uploadProgressCBBlocks = [[NSMutableDictionary alloc] initWithCapacity:5];
            _instance.sendAudioMsgCBBlocks = [[NSMutableDictionary alloc] initWithCapacity:5];
            _instance.startSendAudioMsgCBBlocks = [[NSMutableDictionary alloc] initWithCapacity:5];
            _instance.uploadSpeechStatusCBBlocks = [[NSMutableDictionary alloc] initWithCapacity:5];
            _instance.downloadCBBlocks = [[NSMutableDictionary alloc] initWithCapacity:5];
            _instance.downloadByUrlCBBlocks = [[NSMutableDictionary alloc] initWithCapacity:5];

            _instance.queryHistoryMsgCBBlocks = [[NSMutableDictionary alloc]initWithCapacity:5];
            _instance.queryRoomHistoryMsgCBBlocks = [[NSMutableDictionary alloc]initWithCapacity:5];
            _instance.translateTextCompleteCBBlocks = [[NSMutableDictionary alloc]initWithCapacity:5];

            _instance.blockUserCBBlocks = [[NSMutableDictionary alloc]initWithCapacity:5];
            _instance.unBlockAllUserCB = nil;
            _instance.getBlockUsersCB = nil;
            
            _instance.getContactsCB = nil;
            _instance.getUserInfoCBBlocks = [[NSMutableDictionary alloc]initWithCapacity:5];
            _instance.queryUserStatusCBBlocks = [[NSMutableDictionary alloc]initWithCapacity:5];

            _instance.playCompleteCBBlocks = [[NSMutableDictionary alloc]initWithCapacity:5];
            _instance.getMicStatusCB = nil;
            _instance.getNearbyObjectsCB = nil;
            _instance.getDistanceCBBlocks = [[NSMutableDictionary alloc]initWithCapacity:5];
            _instance.getForbiddenSpeakInfoCB = nil;

            _instance.queryUserProfileCBBlocks = [[NSMutableDictionary alloc]initWithCapacity:5];
            _instance.setUserProfileCB = nil;
            _instance.findUserCB = nil;
            _instance.requestAddFriendCB = nil;
            _instance.dealBeRequestAddFriendCBBlocks = [[NSMutableDictionary alloc]initWithCapacity:5];
            _instance.deleteFriendCB = nil;
            _instance.blackFriendCB = nil;
            _instance.queryFriendsCB = nil;
            _instance.queryFriendRequestListCB = nil;            
        }
    }
    return _instance;
}

@end
