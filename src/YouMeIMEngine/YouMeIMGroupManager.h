#pragma once
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include "YouMeIMCommonDef.h"
#include <YouMeIMEngine/pb/youme_notify.pb.h>
#include <memory>
#include <map>


class YouMeIMGroupManager :public YIMChatRoomManager,
	public IManagerBase
{
	friend class YouMeIMManager;

public:
	YouMeIMGroupManager(YouMeIMManager* pIManager);
	YIMErrorcode Init();
	void UnInit();
	void SetCallback(IYIMChatRoomCallback* pCallback);
	
	//聊天室接口
	YIMErrorcode JoinChatRoom(const XCHAR* chatRoomID) override;
	YIMErrorcode LeaveChatRoom(const XCHAR* chatRoomID) override;
    YIMErrorcode LeaveAllChatRooms() override;
    YIMErrorcode GetRoomMemberCount(const XCHAR* chatRoomID) override;

	//重新加入聊天室
	void ReJoinChatRoom(bool isReconnect);
	void ResetRoomStatus();

	void OnJoinLeaveRoomNotify(YOUMEServiceProtocol::NotifyReq& rsp);
private:
	virtual void OnRecvPacket(ServerPacket& serverPacket) override;
	YIMErrorcode ReqJoinChatRoom(const XString& chatRoomID, bool reconnect = false);
	void OnJoinChatRoomRsp(ServerPacket& serverPacket);
	void OnLeaveChatRoomRsp(ServerPacket& serverPacket);
    void OnLeaveAllChatRoomsRsp(ServerPacket& serverPacket);
    void OnGetRoomMemberCountRsp(ServerPacket& serverPacket);

	IYIMChatRoomCallback* m_pCallback;
	bool m_bInit;

	//保存一个 聊天室ID 集合，自动重连的时候需要再次进入
	//std::set<XString> m_chatRoomSet;
	std::map<XString, bool> m_chatRoomSet;	// 登录之前join，登录后自动join
};
