#ifndef YOUME_FRIEND_MANAGER_H
#define YOUME_FRIEND_MANAGER_H

#include <thread>
#include "YouMeCommon/XCondWait.h"
#include "YouMeIMEngine/YIM.h"
#include "YouMeIMEngine/YouMeIMCommonDef.h"
#include "YouMeIMEngine/pb/youme_rc_get_friend_list.pb.h"


class UserBriefInfo : public IYIMUserBriefInfo
{
public:
	virtual const XCHAR* GetUserID()
	{
		return userID.c_str();
	}
	virtual const XCHAR* GetNickname()
	{
		return nickname.c_str();
	}
	virtual YIMUserStatus GetUserStatus()
	{
		return status;
	}

	UserBriefInfo() : status(STATUS_OFFLINE){}

	XString userID;
	XString nickname;
	YIMUserStatus status;
};

class FriendRequestInfo : public IYIMFriendRequestInfo
{
public:
	virtual const XCHAR* GetAskerID()
	{
		return askerID.c_str(); 
	}
	virtual const XCHAR* GetAskerNickname()
	{
		return askerNickname.c_str();
	}
	virtual const XCHAR* GetInviteeID()
	{
		return inviteeID.c_str(); 
	}
	virtual const XCHAR* GetInviteeNickname()
	{
		return inviteeNickname.c_str();
	}
	virtual const XCHAR* GetValidateInfo()
	{ 
		return validateInfo.c_str();
	}
	virtual YIMAddFriendStatus GetStatus()
	{
		return status;
	}
	virtual unsigned int GetCreateTime()
	{
		return createTime;
	}
    
    virtual XUINT64 GetReqID()
    {
        return reqID;
    }

	XString askerID;
	XString askerNickname;
	XString inviteeID;
	XString inviteeNickname;
	XString validateInfo;
	YIMAddFriendStatus status;
    XUINT64 reqID;
	unsigned int createTime;
	bool hasNotify;
};


struct FriendValidateInfo
{
	XUINT64 requestID;
    XString asker;
	std::string verifyInfo;
};


class YouMeIMManager;

class YouMeFriendManager : public YIMFriendManager, public IManagerBase
{
public:
	YouMeFriendManager(YouMeIMManager* imManager);
	~YouMeFriendManager();

	virtual void OnRecvPacket(ServerPacket& serverPacket) override;

	virtual YIMErrorcode FindUser(int findType, const XCHAR* target) override;
	virtual YIMErrorcode RequestAddFriend(std::vector<XString>& users, const XCHAR* comments) override;
	virtual YIMErrorcode DealBeRequestAddFriend(const XCHAR* userID, int dealResult, XUINT64 reqID) override;
	virtual YIMErrorcode DeleteFriend(std::vector<XString>& users, int deleteType = 0) override;
	virtual YIMErrorcode BlackFriend(int type, std::vector<XString>& users) override;
	virtual YIMErrorcode QueryFriends(int type = 0, int startIndex = 0, int count = 50) override;
	virtual YIMErrorcode QueryFriendRequestList(int startIndex = 0, int count = 20) override;

	void SetCallback(IYIMFriendCallback* callback){ m_pCallback = callback; }
	void CheckHasAddFriendRequest();

private:
	YIMErrorcode RequestFindFriend(int findType, const XString& target, int startIndex);
	void OnFindFrirndByUserIDRsp(ServerPacket& serverPacket);
	void OnFindFrirndByNicknameRsp(ServerPacket& serverPacket);
	YIMErrorcode RequestAddFriend(const XString& userID, const XString& comments);
	void OnAddFriendRsp(ServerPacket& serverPacket);
	void OnFriendNotify(ServerPacket& serverPacket);
	void OnDealAddFriendRsp(ServerPacket& serverPacket);
	void OnDeleteFriendRsp(ServerPacket& serverPacket);
	YIMErrorcode RequestBlackFriend(const XString& userID);
	void OnBlackFriendRsp(ServerPacket& serverPacket);
	YIMErrorcode RequestUnBlackFriend(const XString& userID);
	void OnUnBlackFriendRsp(ServerPacket& serverPacket);
	YIMErrorcode RequestFriendList(int startIndex, int count);
	void OnFriendListRsp(ServerPacket& serverPacket);
	YIMErrorcode RequestBlackFriendList(int startIndex, int count);
	void OnBlackFriendListRsp(ServerPacket& serverPacket);
	YIMErrorcode QueryFriendRequestList(int startIndex, int count, bool isCheckNotify);
	void OnQueryFriendRequestRsp(ServerPacket& serverPacket);
	void OnUpdateFriendRequestStatusRsp(ServerPacket& serverPacket);
	void OfflineFriendRequestNotify(ServerPacket& serverPacket);
	void OnRelationChainHeartBeatRsp(ServerPacket& serverPacket);
	void UpdateRequestAddFriendStatus(XUINT64 requestID, YOUMEServiceProtocol::RC_FRIEND_REQ_TYPE requestType, const std::string& otherSide);

	YouMeIMManager* m_pIManager;
	IYIMFriendCallback* m_pCallback;
	std::map<XUINT64, FriendValidateInfo> m_requestAddFriendInfoMap;	// 保存好友请求验证信息
};


#endif
