//
//  YIMService.m
//  YoumeIMUILib
//
//  Created by 余俊澎 on 16/8/11.
//  Copyright © 2016年 余俊澎. All rights reserved.
//
#include "YIM.h"
#import "YIMCallbackProtocol.h"
#import "YIMCallbackBlock.h"

class YIMImplement : IYIMLoginCallback,IYIMChatRoomCallback,IYIMMessageCallback,IYIMDownloadCallback,IYIMContactCallback ,IYIMAudioPlayCallback, IYIMLocationCallback, IYIMNoticeCallback, IYIMReconnectCallback, IYIMUserProfileCallback, IYIMFriendCallback, IYIMUpdateReadStatusCallback
{
public:
    static YIMImplement *getInstance ();
    static void destroy ();
    id<YIMCallbackProtocol> delegate;
    bool m_updateReadStatusCallbackFlag;
    
public:
    //退出后重新登陆前需要重新绑定登陆回调
    void RebindCallback();
    
public:
    //IYIMLoginCallback 目前用到的回调
    virtual void OnLogin(YIMErrorcode errorcode,const XCHAR* userID) override;
    virtual void OnLogout(YIMErrorcode errorcode) override;
    virtual void OnJoinChatRoom(YIMErrorcode errorcode, const XCHAR* chatRoomID) override;
    //IYIMMessageCallback
    //发送消息状态
    virtual void OnSendMessageStatus(XUINT64 requestID, YIMErrorcode errorcode, unsigned int sendTime, bool isForbidRoom,  int reasonType, XUINT64 forbidEndTime, XUINT64 messageID) override;
    
	//上传显示进度
    virtual void OnUploadProgress(XUINT64 requestID, float percent) override;
    
    //接收端消息已读，更新发送端消息显示状态
    virtual void OnUpdateReadStatus(const XCHAR* recvId, int chatType, XUINT64 msgSerial) override;
    
	//接收到用户发来的消息
    virtual void OnRecvMessage( std::shared_ptr<IYIMMessage> pMessage) override;
    
    //语音消息的回掉
    virtual void OnSendAudioMessageStatus(XUINT64 requestID, YIMErrorcode errorcode, const XCHAR* text, const XCHAR* audioPath, unsigned int audioTime, unsigned int sendTime, bool isForbidRoom,  int reasonType, XUINT64 forbidEndTime, XUINT64 messageID) override;
    //停止语音回调（调用StopAudioMessage停止语音之后，发送语音消息之前）
    virtual void OnStartSendAudioMessage(XUINT64 requestID, YIMErrorcode errorcode, const XCHAR* text, const XCHAR* audioPath, unsigned int audioTime) override;
    
    virtual void OnQueryHistoryMessage(YIMErrorcode errorcode, const XCHAR* targetID, int remain, std::list<std::shared_ptr<IYIMMessage> > messageList) override;
    //获取房间历史纪录回调
    virtual void OnQueryRoomHistoryMessageFromServer(YIMErrorcode errorcode, const XCHAR* roomID, int remain, std::list<std::shared_ptr<IYIMMessage> >& messageList) override ;
    //语音上传后回调
    virtual void OnStopAudioSpeechStatus(YIMErrorcode errorcode,  std::shared_ptr<IAudioSpeechInfo> audioSpeechInfo) override ;
    
    virtual void OnReceiveMessageNotify(YIMChatType chatType, const XCHAR* targetID) override;
    
    //语音识别的文字
    virtual void OnGetRecognizeSpeechText(XUINT64 requestID, YIMErrorcode errorcode,const XCHAR* text) override;

    //IYIMContactCallback获取最近联系人回调
    virtual void OnGetRecentContacts(YIMErrorcode errorcode, std::list<std::shared_ptr<IYIMContactsMessageInfo> >& contactList)  override;
    //获取用户信息回调(用户信息为JSON格式)
    virtual void OnGetUserInfo(YIMErrorcode errorcode, const XCHAR* userID, const XCHAR* userInfo) override;
    //查询用户状态回调
    virtual void OnQueryUserStatus(YIMErrorcode errorcode, const XCHAR* userID, YIMUserStatus status) override;
    
    //IYIMDownloadCallback
    virtual void OnDownload( YIMErrorcode errorcode, std::shared_ptr<IYIMMessage> msg, const XCHAR* savePath ) override;
    
    virtual void OnDownloadByUrl( YIMErrorcode errorcode, const XCHAR* strFromUrl, const XCHAR* savePath ,int iAudioTime ) override;
    
    //IYIMAudioPlayCallback
    virtual void OnPlayCompletion(YIMErrorcode errorcode, const XCHAR* path) override;
    //获取麦克风状态回调
    virtual void OnGetMicrophoneStatus(AudioDeviceStatus status) override;
    
    
    //离开分组
    virtual void OnLeaveChatRoom(YIMErrorcode errorcode, const XCHAR * chatRoomID) override;
    virtual void OnLeaveAllChatRooms(YIMErrorcode errorcode) override;
    //其他用户加入频道通知
    virtual void OnUserJoinChatRoom(const XCHAR* chatRoomID, const XCHAR* userID) override;
    //其他用户退出频道通知
    virtual void OnUserLeaveChatRoom(const XCHAR* chatRoomID, const XCHAR* userID) override;
    //获取房间成员数量回调
    virtual void OnGetRoomMemberCount(YIMErrorcode errorcode, const XCHAR* chatRoomID, unsigned int count) override;
    
    
    //文本翻译完成回调
    virtual void OnTranslateTextComplete(YIMErrorcode errorcode, unsigned int requestID, const XCHAR* text, LanguageCode srcLangCode, LanguageCode destLangCode) override ;
    
    //举报处理结果通知
    virtual void OnAccusationResultNotify(AccusationDealResult result, const XCHAR* userID, unsigned int accusationTime) override;
    
    virtual void OnGetForbiddenSpeakInfo( YIMErrorcode errorcode, std::vector< std::shared_ptr<IYIMForbidSpeakInfo> > vecForbiddenSpeakInfos ) override;
    //屏蔽/解除屏蔽用户消息回调
    virtual void OnBlockUser(YIMErrorcode errorcode, const XCHAR* userID, bool block) override;
    //解除所有已屏蔽用户回调
    virtual void OnUnBlockAllUser(YIMErrorcode errorcode) override;
    //获取被屏蔽消息用户回调
    virtual void OnGetBlockUsers(YIMErrorcode errorcode, std::list<XString> userList) override;
    // 音量回调
    virtual void OnRecordVolumeChange(float volume) override;
    
    //被踢
    virtual void OnKickOff() override;
    
    // 获取自己位置回调
    virtual void OnUpdateLocation(YIMErrorcode errorcode, std::shared_ptr<GeographyLocation> location) override ;
    // 获取附近目标回调
    virtual void OnGetNearbyObjects(YIMErrorcode errorcode, std::list< std::shared_ptr<RelativeLocation> >  neighbourList, unsigned int startDistance, unsigned int endDistance) override;
    virtual void OnGetDistance(YIMErrorcode errorcode, const XCHAR* userID, unsigned int distance) override;
    
    // 收到公告
    virtual void OnRecvNotice(YIMNotice* notice) override;
    // 撤销公告（置顶公告）
    virtual void OnCancelNotice(XUINT64 noticeID, const XCHAR* channelID) override;
    
    // 开始重连
    virtual void OnStartReconnect() override;
    // 收到重连结果
    virtual void OnRecvReconnectResult(ReconnectResult result) override;
    
    // 用户信息管理相关回调
    
    // 查询用户信息回调
    virtual void OnQueryUserInfo(YIMErrorcode errorcode, const IMUserProfileInfo &userInfo) override;
    // 设置用户信息回调
    virtual void OnSetUserInfo(YIMErrorcode errorcode) override;
    // 切换用户在线状态回调
    virtual void OnSwitchUserOnlineState(YIMErrorcode errorcode) override;
    // 设置头像回调
    virtual void OnSetPhotoUrl(YIMErrorcode errorcode, const XCHAR* photoUrl) override;
    
    /*
     * 功能：用户资料变更通知
     * @param userID：用户ID
     * commonts：根据需要决定是否重新获取用户信息
     */
    virtual void OnUserInfoChangeNotify(const XCHAR* userID) override;
    
    //好友接口相关回调
    /*
     * 功能：查找用户回调
     * @param errorcode：错误码
     * @param users：用户简要信息
     */
    virtual void OnFindUser(YIMErrorcode errorcode, std::list<std::shared_ptr<IYIMUserBriefInfo> >& users) override;
    
    /*
     * 功能：请求添加好友回调
     * @param errorcode：错误码
     * @param userID：用户ID
     */
    virtual void OnRequestAddFriend(YIMErrorcode errorcode, const XCHAR* userID) override;
    
    /*
     * 功能：被邀请添加好友通知（需要验证）
     * @param userID：用户ID
     * @param comments：备注或验证信息
     * commonts：显示用户信息可以根据userID查询
     */
    virtual void OnBeRequestAddFriendNotify(const XCHAR* userID, const XCHAR* comments, XUINT64 reqID) override;
    
    /*
     * 功能：被添加为好友通知（不需要验证）
     * @param userID：用户ID
     * @param comments：备注或验证信息
     */
    virtual void OnBeAddFriendNotify(const XCHAR* userID, const XCHAR* comments) override;
    
    /*
     * 功能：处理被请求添加好友回调
     * @param errorcode：错误码
     * @param userID：用户ID
     * @param comments：备注或验证信息
     * @param dealResult：处理结果	0：同意	1：拒绝
     */
    virtual void OnDealBeRequestAddFriend(YIMErrorcode errorcode, const XCHAR* userID, const XCHAR* comments, int dealResult) override;
    
    /*
     * 功能：请求添加好友结果通知(需要好友验证，待被请求方处理后回调)
     * @param userID：用户ID
     * @param comments：备注或验证信息
     * @param dealResult：处理结果	0：同意	1：拒绝
     */
    virtual void OnRequestAddFriendResultNotify(const XCHAR* userID, const XCHAR* comments, int dealResult) override;
    
    /*
     * 功能：删除好友结果回调
     * @param errorcode：错误码
     * @param userID：用户ID
     */
    virtual void OnDeleteFriend(YIMErrorcode errorcode, const XCHAR* userID) override;
    
    /*
     * 功能：被好友删除通知
     * @param userID：用户ID
     */
    virtual void OnBeDeleteFriendNotify(const XCHAR* userID) override;
    
    /*
     * 功能：拉黑或解除拉黑好友回调
     * @param errorcode：错误码
     * @param type：0：拉黑	1：解除拉黑
     * @param userID：用户ID
     */
    virtual void OnBlackFriend(YIMErrorcode errorcode, int type, const XCHAR* userID) override;
    
    /*
     * 功能：查询我的好友回调
     * @param errorcode：错误码
     * @param type：0：正常好友	1：被拉黑好友
     * @param startIndex：起始序号
     * @param hasMore：是否还有更多数据
     * @param friends：好友列表
     */
    virtual void OnQueryFriends(YIMErrorcode errorcode, int type, int startIndex, std::list<std::shared_ptr<IYIMUserBriefInfo> >& friends) override;
    
    /*
     * 功能：查询好友请求列表回调
     * @param errorcode：错误码
     * @param startIndex：起始序号
     * @param hasMore：是否还有更多数据
     * @param validateList：验证消息列表
     */
    virtual void OnQueryFriendRequestList(YIMErrorcode errorcode, int startIndex, std::list<std::shared_ptr<IYIMFriendRequestInfo> >& requestList) override;
    
private:
    YIMImplement ();
    ~YIMImplement ();
    
private:
    static YIMImplement *mPInstance;
    YIMManager* IMManager;
};
