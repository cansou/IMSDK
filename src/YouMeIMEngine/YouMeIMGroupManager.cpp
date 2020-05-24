#include "YouMeIMGroupManager.h"
#include <YouMeCommon/StringUtil.hpp>
#include "YouMeIMManager.h"
#include <YouMeIMEngine/pb/youme_room.pb.h>
#include <YouMeCommon/StringUtil.hpp>
#include <YouMeIMEngine/pb/server_protocol_code.h>
#include <regex>

YouMeIMGroupManager::YouMeIMGroupManager(YouMeIMManager* pIManager) : IManagerBase(pIManager)
,m_pCallback(NULL)
,m_bInit(false)
{
}

YIMErrorcode YouMeIMGroupManager::Init()
{
	if (m_bInit)
	{
		return YIMErrorcode_Success;
	}
	m_pCallback = NULL;
	m_bInit = true;
	return YIMErrorcode_Success;
}

void YouMeIMGroupManager::UnInit()
{
	if (!m_bInit)
	{
		return;
	}
	m_pCallback = NULL;
	m_bInit = false;
}

void YouMeIMGroupManager::SetCallback(IYIMChatRoomCallback* pCallback)
{
	m_pCallback = pCallback;
}

void YouMeIMGroupManager::OnRecvPacket(ServerPacket& serverPacket)
{
    switch (serverPacket.commondID)
    {
        case YOUMEServiceProtocol::CMD_ENTER_ROOM:
        {
            OnJoinChatRoomRsp(serverPacket);
        }
            break;
        case YOUMEServiceProtocol::CMD_LEAVE_ROOM:
        {
            OnLeaveChatRoomRsp(serverPacket);
        }
            break;
        case YOUMEServiceProtocol::CMD_LEAVE_ALL_ROOM:
        {
            OnLeaveAllChatRoomsRsp(serverPacket);
        }
            break;
        case YOUMEServiceProtocol::CMD_GET_ROOM_INFO:
	    {
		    OnGetRoomMemberCountRsp(serverPacket);
	    }
		    break;
        default:
            break;
    }
}

YIMErrorcode YouMeIMGroupManager::ReqJoinChatRoom(const XString& chatRoomID, bool reconnect)
{
	if (m_pIManager == NULL)
	{
		return YIMErrorcode_NotLogin;
	}

	YOUMEServiceProtocol::EnterRoomReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_room_id(XStringToUTF8(chatRoomID));
	std::string strData;
	req.SerializeToString(&strData);
	XUINT64 msgSerial = 0;
	std::map<std::string, std::string> extend;
	extend["RoomID"] = req.room_id();
	if (reconnect)
	{
		extend["Reconnect"] = "1";
	}
	if (m_pIManager->SendData(YOUMEServiceProtocol::CMD_ENTER_ROOM, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		return YIMErrorcode_Success;
	}

	return YIMErrorcode_UnknowError;
}

YIMErrorcode YouMeIMGroupManager::JoinChatRoom(const XCHAR* chatRoomID)
{
	if (NULL == chatRoomID || XStrLen(chatRoomID) == 0)
	{
		return YIMErrorcode_ParamInvalid;
	}

	YouMe_LOG_Debug(__XT("join room %s loginstatus:%d"), chatRoomID, m_pIManager->GetLoginStatus());

	XString roomID = XString(chatRoomID);
	std::regex pattern("^(\\w|-|=|[+])+$");
	std::string strRoomID = XStringToUTF8(roomID);
	if (!std::regex_match(strRoomID, pattern))
	{
		return YIMErrorcode_ParamInvalid;
	}
	// 调用方在登录完成前joinroom，登录成功后自动join
	std::map<XString, bool>::const_iterator itr = m_chatRoomSet.find(roomID);
	if (itr == m_chatRoomSet.end())
	{
		m_chatRoomSet[roomID] = false;
	}
	if (m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YIMErrorcode_NotLogin;
	}

	return ReqJoinChatRoom(roomID);
}

void YouMeIMGroupManager::OnJoinChatRoomRsp(ServerPacket& serverPacket)
{
	if (NULL == m_pCallback)
	{
		YouMe_LOG_Error(__XT("group callback is null"));
		return;
	}
	XString strRoomID;
	std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("RoomID");
	if (itr != serverPacket.extend.end())
	{
		strRoomID = UTF8TOXString(itr->second);
	}
	else
	{
		YouMe_LOG_Error(__XT("RoomID not found"));
		return;
	}
	YIMErrorcode errorCode = YIMErrorcode_Success;
	if (serverPacket.result == -1)
	{
		errorCode = YIMErrorcode_TimeOut;
		YouMe_LOG_Warning(__XT("join room timeout"));
	}
	else
	{
		YOUMEServiceProtocol::EnterRoomRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
			errorCode = YIMErrorcode_ServerError;
		}
		else
		{
			int nRet = rsp.ret();
			if (nRet == 0)
			{
				std::map<XString, bool>::iterator itr = m_chatRoomSet.find(strRoomID);
				if (itr != m_chatRoomSet.end())
				{
					itr->second = true;
				}
				YouMe_LOG_Info(__XT("join room %s success"), strRoomID.c_str());
			}
			else
			{
				YouMe_LOG_Error(__XT("join chatroom error(%d) room:%s"), nRet, strRoomID.c_str());
				if (YOUMEServiceProtocol::EC_ROOM_NAME_TOO_LONG == nRet)
				{
					errorCode = YIMErrorcode_RoomIDTooLong;
				}
				else if (YOUMEServiceProtocol::EC_ROOM_OVERFLOW == nRet)
				{
					errorCode = YIMErrorcode_ChannelMemberOverflow;
				}
				else
				{
					errorCode = YIMErrorcode_ServerError;
				}
			}
		}
	}

	itr = serverPacket.extend.find("Reconnect");
	if (itr != serverPacket.extend.end())
	{
		return;
	}

	m_pCallback->OnJoinChatRoom(errorCode, strRoomID.c_str() );
}

YIMErrorcode YouMeIMGroupManager::LeaveChatRoom(const XCHAR* chatRoomID)
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YIMErrorcode_NotLogin;
	}
	if (NULL == chatRoomID || XStrLen(chatRoomID) == 0)
	{
		return YIMErrorcode_ParamInvalid;
	}
	XString roomID = XString(chatRoomID);
	if (m_chatRoomSet.find(roomID) == m_chatRoomSet.end())
	{
		return YIMErrorcode_NotJoinRoom;
	}
	
	YOUMEServiceProtocol::LeaveRoomReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_room_id(XStringToUTF8(roomID));
	std::string strData;
	req.SerializeToString(&strData);
	XUINT64 msgSerial = 0;
	std::map<std::string, std::string> extend;
	extend["RoomID"] = req.room_id();
	if (m_pIManager->SendData(YOUMEServiceProtocol::CMD_LEAVE_ROOM, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		return YIMErrorcode_Success;
	}

	return YIMErrorcode_UnknowError;
}

YIMErrorcode YouMeIMGroupManager::GetRoomMemberCount(const XCHAR* chatRoomID)
{
	if (NULL == chatRoomID)
	{
		return YIMErrorcode_ParamInvalid;
	}
	XString strRoomID = XString(chatRoomID);
	if (strRoomID.empty())
	{
		return YIMErrorcode_ParamInvalid;
	}
	
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YIMErrorcode_NotLogin;
	}

	YOUMEServiceProtocol::GetRoomInfoReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_room_id(XStringToUTF8(strRoomID));
	std::string strData;
	req.SerializeToString(&strData);
	XUINT64 msgSerial = 0;
	std::map<std::string, std::string> extend;
	extend["RoomID"] = req.room_id();
	if (m_pIManager->SendData(YOUMEServiceProtocol::CMD_GET_ROOM_INFO, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		return YIMErrorcode_Success;
	}
	return YIMErrorcode_ServerError;
}

void YouMeIMGroupManager::OnGetRoomMemberCountRsp(ServerPacket& serverPacket)
{
	if (NULL == m_pCallback)
	{
		YouMe_LOG_Error(__XT("group callback is null"));
		return;
	}
	std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("RoomID");
	if (itr == serverPacket.extend.end())
	{
		YouMe_LOG_Error(__XT("RoomID not found")); 
		return;
	}
	XString strRoomID = UTF8TOXString(itr->second);
	
	unsigned int memberCount = 0;
	YIMErrorcode errorCode = YIMErrorcode_Success;
	if (serverPacket.result == -1)
	{
		errorCode = YIMErrorcode_TimeOut;
		YouMe_LOG_Warning(__XT("get room member timeout"));
	}
	else
	{
		YOUMEServiceProtocol::GetRoomInfoRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
			errorCode = YIMErrorcode_ServerError;
		}
		else
		{
			int nRet = rsp.ret();
			if (nRet != 0)
			{
				YouMe_LOG_Error(__XT("get room member error(%d) room:%s"), nRet, strRoomID.c_str());
				errorCode = YIMErrorcode_ServerError;
			}
			else
			{
				memberCount = rsp.room_info().member_cnt();
			}
		}
	}
	m_pCallback->OnGetRoomMemberCount(errorCode, strRoomID.c_str(), memberCount);
}

void YouMeIMGroupManager::OnLeaveChatRoomRsp(ServerPacket& serverPacket)
{
	if (NULL == m_pCallback)
	{
		YouMe_LOG_Error(__XT("group callback is null"));
		return;
	}

	XString strRoomID;
	std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("RoomID");
	if (itr != serverPacket.extend.end())
	{
		strRoomID = UTF8TOXString(itr->second);
	}
	else
	{
		YouMe_LOG_Error(__XT("RoomID not found"));
		return;
	}
	YIMErrorcode errorCode = YIMErrorcode_Success;
	if (serverPacket.result == -1)
	{
		errorCode = YIMErrorcode_TimeOut;
		YouMe_LOG_Warning(__XT("leave room timeout"));
	}
	else
	{
		YOUMEServiceProtocol::LeaveRoomRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
			errorCode = YIMErrorcode_ServerError;
		}
		else
		{
			if (rsp.ret() != 0)
			{
				YouMe_LOG_Error(__XT("leave chatroom error(%d) room:%s"), rsp.ret(), strRoomID.c_str());
				errorCode = YIMErrorcode_ServerError;
			}
			else
			{
				YouMe_LOG_Info(__XT("leave room %s success"), strRoomID.c_str());
			}
		}
	}

	m_pCallback->OnLeaveChatRoom(errorCode, strRoomID.c_str() );

	std::map<XString, bool>::iterator itrRoom = m_chatRoomSet.find(strRoomID);
	if (itrRoom != m_chatRoomSet.end())
	{
		m_chatRoomSet.erase(itrRoom);
	}
}

YIMErrorcode YouMeIMGroupManager::LeaveAllChatRooms()
{
    if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
    {
        return YIMErrorcode_NotLogin;
    }
   
    YOUMEServiceProtocol::LeaveAllRoomReq req;
    req.set_version(PROTOCOL_VERSION);
    std::string strData;
    req.SerializeToString(&strData);
    XUINT64 msgSerial = 0;
    std::map<std::string, std::string> extend;
    if (m_pIManager->SendData(YOUMEServiceProtocol::CMD_LEAVE_ALL_ROOM, strData.c_str(), (int)strData.length(), extend, msgSerial))
    {
        return YIMErrorcode_Success;
    }
    
    return YIMErrorcode_UnknowError;
}

void YouMeIMGroupManager::OnLeaveAllChatRoomsRsp(ServerPacket& serverPacket)
{
    if (NULL == m_pCallback)
    {
        YouMe_LOG_Error(__XT("group callback is null"));
        return;
    }

    YIMErrorcode errorCode = YIMErrorcode_Success;
    if (serverPacket.result == -1)
    {
        errorCode = YIMErrorcode_TimeOut;
        YouMe_LOG_Warning(__XT("leave all room timeout"));
    }
    else
    {
        YOUMEServiceProtocol::LeaveAllRoomRsp rsp;
        if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
        {
            YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
            errorCode = YIMErrorcode_ServerError;
        }
        else
        {
            if (rsp.ret() != 0)
            {
                YouMe_LOG_Error(__XT("leave all chatroom error(%d)"), rsp.ret());
                errorCode = YIMErrorcode_ServerError;
            }
            else
            {
                YouMe_LOG_Info(__XT("leave all room success"));
            }
        }
    }

    m_pCallback->OnLeaveAllChatRooms(errorCode);
    
    m_chatRoomSet.clear();
}

void YouMeIMGroupManager::ReJoinChatRoom(bool isReconnect)
{
	for (std::map<XString, bool>::const_iterator itr = m_chatRoomSet.begin(); itr != m_chatRoomSet.end(); ++itr)
	{
		if (!itr->second)
		{
			YouMe_LOG_Info(__XT("rejoin room %s"), itr->first.c_str());
			ReqJoinChatRoom(itr->first, isReconnect);
		}
	}
}

void YouMeIMGroupManager::ResetRoomStatus()
{
	for (std::map<XString, bool>::iterator itr = m_chatRoomSet.begin(); itr != m_chatRoomSet.end(); ++itr)
	{
		itr->second = false;
	}
}

void YouMeIMGroupManager::OnJoinLeaveRoomNotify(YOUMEServiceProtocol::NotifyReq& rsp)
{
	if (NULL == m_pCallback)
	{
		YouMe_LOG_Error(__XT("group callback is null"));
        return;
	}
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return;
	}

	int type = rsp.notify_type();
	XString strUserID = UTF8TOXString(rsp.snder_id());
	XString strRoomID = UTF8TOXString(rsp.recver_id());
	if (4 == type)
	{
		m_pCallback->OnUserJoinChatRoom(strRoomID.c_str(), strUserID.c_str());
	}
	else if (5 == type)
	{
		m_pCallback->OnUserLeaveChatRoom(strRoomID.c_str(), strUserID.c_str());
	}

	YouMe_LOG_Debug(__XT("JoinLeaveRoomNotify type:%d roomID:%s userID:%s"), type, strRoomID.c_str(), strUserID.c_str());
}
