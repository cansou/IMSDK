#include "YouMeIMEngine/YouMeFriendManager.h"
#include "YouMeCommon/StringUtil.hpp"
#include "YouMeIMEngine/YouMeIMManager.h"
#include "YouMeIMEngine/pb/server_protocol_code.h"
#include "YouMeIMEngine/pb/youme_rc_query_friend.pb.h"
#include "YouMeIMEngine/pb/youme_rc_add_friend.pb.h"
#include "YouMeIMEngine/pb/youme_rc_del_friend.pb.h"
#include "YouMeIMEngine/pb/youme_rc_black_friend.pb.h"


#define FIND_FRIEND_COUNT 50
#define FIND_TYPE_USERID 0
#define FIND_TYPE_NICKNAME 1
#define TYPE_BLACK_FRIEND 0
#define TYPE_UNBLACK_FRIEND 1
#define FRIEND_LIST_PAGE_SIZE 100
#define FRIEND_REQUEST_PAGE_SIZE 20
#define ADD_FRIEND_AGREE 0
#define ADD_FRIEND_REFUSE 1


void YouMeFriendManager::OnRecvPacket(ServerPacket& serverPacket)
{
	switch (serverPacket.commondID)
	{
	case YOUMEServiceProtocol::CMD_FIND_FRIEND_BY_ID:
	{
		OnFindFrirndByUserIDRsp(serverPacket);
	}
		break;
	case YOUMEServiceProtocol::CMD_FIND_FRIEND_BY_NICKNAME:
	{
		OnFindFrirndByNicknameRsp(serverPacket);
	}
		break;
	case YOUMEServiceProtocol::CMD_REQUEST_ADD_FRIEND:
	{
		OnAddFriendRsp(serverPacket);
	}
		break;
	case YOUMEServiceProtocol::CMD_FRIND_NOTIFY:
	{
		OnFriendNotify(serverPacket);
	}
		break;
	case YOUMEServiceProtocol::CMD_DEAL_ADD_FRIEND:
	{
		OnDealAddFriendRsp(serverPacket);
	}
		break;
	case YOUMEServiceProtocol::CMD_DELETE_FRIEND:
	{
		OnDeleteFriendRsp(serverPacket);
	}
		break;
	case YOUMEServiceProtocol::CMD_BLACK_FRIEND:
	{
		OnBlackFriendRsp(serverPacket);
	}
		break;
	case YOUMEServiceProtocol::CMD_UNBLACK_FRIEND:
	{
		OnUnBlackFriendRsp(serverPacket);
	}
		break;
	case YOUMEServiceProtocol::CMD_QUERY_FRIEND_LIST:
	{
		OnFriendListRsp(serverPacket);
	}
		break;
	case YOUMEServiceProtocol::CMD_QUERY_BLACK_FRIEND_LIST:
	{
		OnBlackFriendListRsp(serverPacket);
	}
		break;
	case YOUMEServiceProtocol::CMD_QUERY_FRIEND_REQUEST_LIST:
	{
		OnQueryFriendRequestRsp(serverPacket);
	}
		break;
	case YOUMEServiceProtocol::CMD_UPDATE_FRIEND_RQUEST_STATUS:
	{
		OnUpdateFriendRequestStatusRsp(serverPacket);
	}
		break;
	case YOUMEServiceProtocol::CMD_RELATION_CHAIN_HEARTBEAT:
	{
		OnRelationChainHeartBeatRsp(serverPacket);
	}
		break;
	default:
		break;
	}
}

YouMeFriendManager::YouMeFriendManager(YouMeIMManager* imManager) : IManagerBase(imManager)
, m_pIManager(imManager)
, m_pCallback(NULL)
{
}

YouMeFriendManager::~YouMeFriendManager()
{
}

YIMErrorcode YouMeFriendManager::FindUser(int findType, const XCHAR* target)
{
	if (NULL == target || XStrLen(target) == 0 || (findType != FIND_TYPE_USERID && findType != FIND_TYPE_NICKNAME))
	{
		return YIMErrorcode_ParamInvalid;
	}
	XString strTarget = XString(target);
	return RequestFindFriend(findType, strTarget, 0);
}

YIMErrorcode YouMeFriendManager::RequestFindFriend(int findType, const XString& target, int startIndex)
{
	YouMe_LOG_Debug(__XT("find friend type:%d target:%s index:%d"), findType, target.c_str(), startIndex);

	std::string strTarget = XStringToUTF8(target);
	std::string strData;
	YOUMEServiceProtocol::COMMOND_TYPE command = YOUMEServiceProtocol::CMD_UNKNOW;
	if (FIND_TYPE_USERID == findType)
	{
		command = YOUMEServiceProtocol::CMD_FIND_FRIEND_BY_ID;
		YOUMEServiceProtocol::QueryRcFriendByUserNameReq req;
		req.set_version(PROTOCOL_VERSION);
		req.set_user_id(strTarget);
		req.set_start_idx(startIndex);
		req.set_len(FIND_FRIEND_COUNT);
		req.SerializeToString(&strData);
	}
	else if (FIND_TYPE_NICKNAME == findType)
	{
		command = YOUMEServiceProtocol::CMD_FIND_FRIEND_BY_NICKNAME;
		YOUMEServiceProtocol::QueryRcFriendByUserNicknameReq req;
		req.set_version(PROTOCOL_VERSION);
		req.set_nickname(strTarget);
		req.set_start_idx(startIndex);
		req.set_len(FIND_FRIEND_COUNT);
		req.SerializeToString(&strData);
	}
	XUINT64 msgSerial = 0;
	std::map<std::string, std::string> extend;
	extend["Target"] = strTarget;
	extend["StartIndex"] = CStringUtilT<char>::to_string(startIndex);
	if (!m_pIManager->SendData(command, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		YouMe_LOG_Error(__XT("send request failed"));
		return YIMErrorcode_NetError;
	}
	return YIMErrorcode_Success;
}

void YouMeFriendManager::OnFindFrirndByUserIDRsp(ServerPacket& serverPacket)
{
	if (m_pCallback == NULL)
	{
		YouMe_LOG_Error(__XT("callback is null"));
		return;
	}

	std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("Target");
	if (itr == serverPacket.extend.end())
	{
		YouMe_LOG_Error(__XT("Target not found"));
		return;
	}

	XString target = UTF8TOXString(itr->second);
	std::list<std::shared_ptr<IYIMUserBriefInfo> > userList;
	YIMErrorcode errorcode = YIMErrorcode_Success;
	if (serverPacket.result == -1)
	{
		YouMe_LOG_Error(__XT("find friend timeout"));
		errorcode = YIMErrorcode_TimeOut;
		std::shared_ptr<UserBriefInfo> userInfo(new UserBriefInfo);
		userInfo->userID = target;
		userList.push_back(userInfo);
	}
	else
	{
		YOUMEServiceProtocol::QueryRcFriendByUserNameRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
			errorcode = YIMErrorcode_ServerError;
		}
		else
		{
			int nRet = rsp.ret();
			if (nRet != 0)
			{
				YouMe_LOG_Error(__XT("find friend error(%d)"), nRet);
			}
			else
			{
				YouMe_LOG_Debug(__XT("query request list total:%d"), rsp.user_list_size());

				for (int i = 0; i < rsp.user_list_size(); i++)
				{
					YOUMEServiceProtocol::RcFriendBriefInfo info = rsp.user_list(i);
					std::shared_ptr<UserBriefInfo> userInfo(new UserBriefInfo);
					userInfo->userID = UTF8TOXString(info.user_id());
					userInfo->nickname = UTF8TOXString(info.nickname());
                    if (info.online_state() == YOUMEServiceProtocol::RC_ONLINE_STATE::RC_ONLINE)
                    {
                        userInfo->status = STATUS_ONLINE;
                    }
                    else if (info.online_state() == YOUMEServiceProtocol::RC_ONLINE_STATE::RC_OFFLINE)
                    {
                        userInfo->status = STATUS_OFFLINE;
                    }
                    else if (info.online_state() == YOUMEServiceProtocol::RC_ONLINE_STATE::RC_HIDDEN)
                    {
                        userInfo->status = STATUS_INVISIBLE;
                    }
					userList.push_back(userInfo);
				}

				if (rsp.user_list_size() == FIND_FRIEND_COUNT)	// 查询下一页
				{
					itr = serverPacket.extend.find("StartIndex");
					if (itr == serverPacket.extend.end())
					{
						int startIndex = CStringUtilT<char>::str_to_sint32(itr->second.c_str());
						RequestFindFriend(FIND_TYPE_USERID, target, startIndex + rsp.user_list_size());
					}
				}
			}
		}
	}
	m_pCallback->OnFindUser(errorcode, userList);
}

void YouMeFriendManager::OnFindFrirndByNicknameRsp(ServerPacket& serverPacket)
{
	if (m_pCallback == NULL)
	{
		YouMe_LOG_Error(__XT("callback is null"));
		return;
	}

	std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("Target");
	if (itr == serverPacket.extend.end())
	{
		YouMe_LOG_Error(__XT("Target not found"));
		return;
	}

	XString target = UTF8TOXString(itr->second);
	std::list<std::shared_ptr<IYIMUserBriefInfo> > userList;
	YIMErrorcode errorcode = YIMErrorcode_Success;
	if (serverPacket.result == -1)
	{
		YouMe_LOG_Error(__XT("find friend timeout"));
		errorcode = YIMErrorcode_TimeOut;
		std::shared_ptr<UserBriefInfo> userInfo(new UserBriefInfo);
		userInfo->nickname = target;
		userList.push_back(userInfo);
	}
	else
	{
		YOUMEServiceProtocol::QueryRcFriendByUserNicknameRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
			errorcode = YIMErrorcode_ServerError;
		}
		else
		{
			int nRet = rsp.ret();
			if (nRet != 0)
			{
				YouMe_LOG_Error(__XT("find friend error(%d)"), nRet);
			}
			else
			{
				YouMe_LOG_Debug(__XT("query request list total:%d"), rsp.user_list_size());

				for (int i = 0; i < rsp.user_list_size(); i++)
				{
					YOUMEServiceProtocol::RcFriendBriefInfo info = rsp.user_list(i);
					std::shared_ptr<UserBriefInfo> userInfo(new UserBriefInfo);
					userInfo->userID = UTF8TOXString(info.user_id());
					userInfo->nickname = UTF8TOXString(info.nickname());
					
                    if (info.online_state() == YOUMEServiceProtocol::RC_ONLINE_STATE::RC_ONLINE)
                    {
                        userInfo->status = STATUS_ONLINE;
                    }
                    else if (info.online_state() == YOUMEServiceProtocol::RC_ONLINE_STATE::RC_OFFLINE)
                    {
                        userInfo->status = STATUS_OFFLINE;
                    }
                    else if (info.online_state() == YOUMEServiceProtocol::RC_ONLINE_STATE::RC_HIDDEN)
                    {
                        userInfo->status = STATUS_INVISIBLE;
                    }
					userList.push_back(userInfo);
				}

				if (rsp.user_list_size() == FIND_FRIEND_COUNT)	// 查询下一页
				{
					itr = serverPacket.extend.find("StartIndex");
					if (itr == serverPacket.extend.end())
					{
						int startIndex = CStringUtilT<char>::str_to_sint32(itr->second.c_str());
						RequestFindFriend(FIND_TYPE_NICKNAME, target, startIndex + rsp.user_list_size());
					}
				}
			}
		}
	}

	m_pCallback->OnFindUser(errorcode, userList);
}

YIMErrorcode YouMeFriendManager::RequestAddFriend(std::vector<XString>& users, const XCHAR* comments)
{
	if (users.size() == 0)
	{
		return YIMErrorcode_ParamInvalid;
	}
	if (comments != NULL && XString(comments).length() > 128)
	{
		return YIMErrorcode_ParamInvalid;
	}
	YIMErrorcode errorcode = YIMErrorcode_Success;
	for (std::vector<XString>::const_iterator itr = users.begin(); itr != users.end(); ++itr)
	{
		YIMErrorcode error = RequestAddFriend(*itr, comments);
		if (error != YIMErrorcode_Success)
		{
			errorcode = error;
		}
	}
	return errorcode;
}

YIMErrorcode YouMeFriendManager::RequestAddFriend(const XString& userID, const XString& comments)
{
	YouMe_LOG_Debug(__XT("add friend %s"), userID.c_str());

	if (userID.empty())
	{
		return YIMErrorcode_ParamInvalid;
	}

	std::string strUserID = XStringToUTF8(userID);

	YOUMEServiceProtocol::AddRcFriendReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_user_id(strUserID);
	req.set_verify_info(XStringToUTF8(comments));
	std::string strData;
	req.SerializeToString(&strData);
	XUINT64 msgSerial = 0;
	std::map<std::string, std::string> extend;
	extend["UserID"] = strUserID;
	if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_REQUEST_ADD_FRIEND, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		YouMe_LOG_Error(__XT("send request failed"));
		return YIMErrorcode_NetError;
	}
	return YIMErrorcode_Success;
}

void YouMeFriendManager::OnAddFriendRsp(ServerPacket& serverPacket)
{
	if (m_pCallback == NULL)
	{
		YouMe_LOG_Error(__XT("callback is null"));
		return;
	}

	std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("UserID");
	if (itr == serverPacket.extend.end())
	{
		YouMe_LOG_Error(__XT("UserID not found"));
		return;
	}

	XString userID = UTF8TOXString(itr->second);
	YIMErrorcode errorcode = YIMErrorcode_Success;
	if (serverPacket.result == -1)
	{
		YouMe_LOG_Error(__XT("add friend timeout"));
		errorcode = YIMErrorcode_TimeOut;
	}
	else
	{
		YOUMEServiceProtocol::AddRcFriendRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
			errorcode = YIMErrorcode_ServerError;
		}
		else
		{
			int nRet = rsp.ret();
			if (nRet != 0)
			{
				YouMe_LOG_Error(__XT("find friend error(%d)"), nRet);
				if (YOUMEServiceProtocol::ADDFRIEND_ALREADY_FRIEND == nRet)
				{
					errorcode = YIMErrorcode_AlreadyFriend;
				}
				else
				{
					errorcode = YIMErrorcode_ServerError;
				}
			}
			else
			{
				if (rsp.add_rslt() == YOUMEServiceProtocol::RC_ADD_NEED_VERIFY)
				{
					errorcode = YIMErrorcode_NeedFriendVerify;
				}
				else if (rsp.add_rslt() == YOUMEServiceProtocol::RC_ADD_REFUSED)
				{
					errorcode = YIMErrorcode_BeRefuse;
				}
			}
			YouMe_LOG_Debug(__XT("add friend %s %d %d"), userID.c_str(), rsp.ret(), rsp.add_rslt());
		}
	}
	m_pCallback->OnRequestAddFriend(errorcode, userID.c_str());
}

void YouMeFriendManager::OnFriendNotify(ServerPacket& serverPacket)
{
	if (m_pCallback == NULL)
	{
		YouMe_LOG_Error(__XT("callback is null"));
		return;
	}
	if (NULL == m_pIManager)
	{
		YouMe_LOG_Error(__XT("im manager is not init"));
		return;
	}

	YOUMEServiceProtocol::RcFriendReqNotifyRsp rsp;
	if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
	{
		YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
		return;
	}
	XString receiverID = UTF8TOXString(rsp.req_recver_id());
	XString senderID = UTF8TOXString(rsp.req_snder_id());
	XString verifyInfo = UTF8TOXString(rsp.notify_msg());

	YouMe_LOG_Debug(__XT("friend notify %s %s type:%d status:%d"), senderID.c_str(), receiverID.c_str(), rsp.req_type(), rsp.cur_state());

	if (rsp.req_type() == YOUMEServiceProtocol::TYPE_ADD)
	{
		YIMErrorcode errorcode = YIMErrorcode_Success;
		if (rsp.cur_state() == YOUMEServiceProtocol::ADD_WAIT_ME_VERIFY)
		{
			FriendValidateInfo info;
			info.requestID = rsp.req_id();
			info.verifyInfo = rsp.notify_msg();
            info.asker = senderID;
			m_requestAddFriendInfoMap.insert(std::make_pair(info.requestID, info));
			m_pCallback->OnBeRequestAddFriendNotify(senderID.c_str(), verifyInfo.c_str(), rsp.req_id());
			UpdateRequestAddFriendStatus(rsp.req_id(), YOUMEServiceProtocol::TYPE_BE_ADDED, rsp.req_snder_id());
		}
		else if (rsp.cur_state() == YOUMEServiceProtocol::ADD_SUCCESS && senderID == m_pIManager->GetCurrentUser())
		{
			m_pCallback->OnRequestAddFriendResultNotify(receiverID.c_str(), verifyInfo.c_str(), ADD_FRIEND_AGREE);
		}
		else if (rsp.cur_state() == YOUMEServiceProtocol::ADD_FAIL && senderID == m_pIManager->GetCurrentUser())
		{
			m_pCallback->OnRequestAddFriendResultNotify(receiverID.c_str(), verifyInfo.c_str(), ADD_FRIEND_REFUSE);
		}
		else if (rsp.cur_state() == YOUMEServiceProtocol::ADD_SUCCESS && receiverID == m_pIManager->GetCurrentUser())
		{
			m_pCallback->OnBeAddFriendNotify(senderID.c_str(), verifyInfo.c_str());
		}
	}
	else if (rsp.req_type() == YOUMEServiceProtocol::TYPE_BE_ADDED)
	{
		/*if (receiverID != m_pIManager->GetCurrentUser())
		{
			YouMe_LOG_Debug(__XT("userid not match"));
			return;
		}
		FriendValidateInfo info;
		info.requestID = rsp.req_id();
		info.verifyInfo = rsp.notify_msg();
		m_requestAddFriendInfoMap.insert(std::make_pair(senderID, info));

		m_pCallback->OnRequestAddFriendNotify(senderID, verifyInfo);

		UpdateRequestAddFriendStatus(rsp.req_id(), rsp.req_type(), rsp.req_snder_id());*/
	}
	else if (rsp.req_type() == YOUMEServiceProtocol::TYPE_BE_DELED || rsp.req_type() == YOUMEServiceProtocol::TYPE_DEL)
	{
		if (receiverID != m_pIManager->GetCurrentUser())
		{
			YouMe_LOG_Debug(__XT("userid not match"));
			return;
		}
		m_pCallback->OnBeDeleteFriendNotify(senderID.c_str());
	}
	else if (rsp.req_type() == YOUMEServiceProtocol::TYPE_BE_BLACKED)
	{
		if (receiverID != m_pIManager->GetCurrentUser())
		{
			YouMe_LOG_Debug(__XT("userid not match"));
			return;
		}
		//m_pCallback->OnBeBlackNotify(senderID);
	}
}

YIMErrorcode YouMeFriendManager::DealBeRequestAddFriend(const XCHAR* userID, int dealResult, XUINT64 reqID)
{
	YouMe_LOG_Debug(__XT("deal add friend %s %d"), userID, dealResult);

	if (NULL == userID || XStrLen(userID) == 0)
	{
		return YIMErrorcode_ParamInvalid;
	}

	std::map<XUINT64, FriendValidateInfo>::const_iterator itr = m_requestAddFriendInfoMap.find(reqID);
	if (itr == m_requestAddFriendInfoMap.end())
	{
		YouMe_LOG_Error(__XT("user %s not found"), userID);
		return YIMErrorcode_ParamInvalid;
	}
	std::string strUserID = XStringToUTF8(XString(userID));

	YOUMEServiceProtocol::VerifyRcFriendReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_user_id(strUserID);
	req.set_add_req_id(itr->second.requestID);
	req.set_verify_info(itr->second.verifyInfo);
	req.set_agree(dealResult);
	std::string strData;
	req.SerializeToString(&strData);
	XUINT64 msgSerial = 0;
	std::map<std::string, std::string> extend;
	extend["UserID"] = strUserID;
    extend["ReqID"] = CStringUtilT<char>::to_string(reqID);
	extend["Result"] = CStringUtilT<char>::to_string(dealResult);
	if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_DEAL_ADD_FRIEND, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		YouMe_LOG_Error(__XT("send request failed"));
		return YIMErrorcode_NetError;
	}
	return YIMErrorcode_Success;
}

void YouMeFriendManager::OnDealAddFriendRsp(ServerPacket& serverPacket)
{
	if (m_pCallback == NULL)
	{
		YouMe_LOG_Error(__XT("callback is null"));
		return;
	}

	XString userID;
    XUINT64 reqID=0;
	int result = ADD_FRIEND_AGREE;
	std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("UserID");
	if (itr != serverPacket.extend.end())
	{
		userID = UTF8TOXString(itr->second);
	}
    std::map<std::string, std::string>::const_iterator itrReq = serverPacket.extend.find("ReqID");
    if (itrReq != serverPacket.extend.end())
    {
        reqID = CStringUtilT<char>::str_to_uint64(itrReq->second);
    }
    
	itr = serverPacket.extend.find("Result");
	if (itr != serverPacket.extend.end())
	{
		result = CStringUtilT<char>::str_to_sint32(itr->second);
	}

	
	YIMErrorcode errorcode = YIMErrorcode_Success;
	if (serverPacket.result == -1)
	{
		YouMe_LOG_Error(__XT("add friend timeout"));
		errorcode = YIMErrorcode_TimeOut;
	}
	else
	{
		YOUMEServiceProtocol::VerifyRcFriendRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
			errorcode = YIMErrorcode_ServerError;
		}
		else
		{
			int nRet = rsp.ret();
			if (nRet == 0)
			{
				
			}
			else if (YOUMEServiceProtocol::VERIFYFRIEND_ALREADY_FRIEND == nRet)
			{
				errorcode = YIMErrorcode_AlreadyFriend;
			}
			else
			{
				errorcode = YIMErrorcode_ServerError;
			}
			YouMe_LOG_Debug(__XT("add friend %s %d %d"), userID.c_str(), rsp.ret(), rsp.add_rslt());
		}
	}

	XString verfiyInfo;
	std::map<XUINT64, FriendValidateInfo>::const_iterator itrVerfiyInfo = m_requestAddFriendInfoMap.find(reqID);
	if (itrVerfiyInfo != m_requestAddFriendInfoMap.end())
	{
		verfiyInfo = UTF8TOXString(itrVerfiyInfo->second.verifyInfo);
		if (YIMErrorcode_Success == errorcode || YIMErrorcode_AlreadyFriend == errorcode)
		{
			m_requestAddFriendInfoMap.erase(itrVerfiyInfo);
		}
	}

	m_pCallback->OnDealBeRequestAddFriend(errorcode, userID.c_str(), verfiyInfo.c_str(), result);
}

YIMErrorcode YouMeFriendManager::DeleteFriend(std::vector<XString>& users, int deleteType)
{
	if (users.size() == 0)
	{
		return YIMErrorcode_ParamInvalid;
	}
	YIMErrorcode errorcode = YIMErrorcode_Success;
	for (std::vector<XString>::const_iterator itr = users.begin(); itr != users.end(); ++itr)
	{
		if (itr->empty())
		{
			continue;
		}
		std::string strUserID = XStringToUTF8((*itr));

		YOUMEServiceProtocol::DelRcFriendReq req;
		req.set_version(PROTOCOL_VERSION);
		req.set_user_id(strUserID);
		req.set_is_double_side(deleteType);
		std::string strData;
		req.SerializeToString(&strData);
		XUINT64 msgSerial = 0;
		std::map<std::string, std::string> extend;
		extend["UserID"] = strUserID;
		if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_DELETE_FRIEND, strData.c_str(), (int)strData.length(), extend, msgSerial))
		{
			YouMe_LOG_Error(__XT("send request failed"));
			errorcode = YIMErrorcode_NetError;
		}
	}
	return errorcode;
}

void YouMeFriendManager::OnDeleteFriendRsp(ServerPacket& serverPacket)
{
	if (m_pCallback == NULL)
	{
		YouMe_LOG_Error(__XT("callback is null"));
		return;
	}

	std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("UserID");
	if (itr == serverPacket.extend.end())
	{
		YouMe_LOG_Error(__XT("UserID not found"));
		return;
	}

	XString userID = UTF8TOXString(itr->second);
	YIMErrorcode errorcode = YIMErrorcode_Success;
	if (serverPacket.result == -1)
	{
		YouMe_LOG_Error(__XT("delete friend timeout"));
		errorcode = YIMErrorcode_TimeOut;
	}
	else
	{
		YOUMEServiceProtocol::DelRcFriendRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
			errorcode = YIMErrorcode_ServerError;
		}
		else
		{
			int nRet = rsp.ret();
			if (0 == nRet)
			{

			}
			else if (YOUMEServiceProtocol::DELETEFRIEND_NOT_FRIEND == nRet)
			{
				errorcode = YIMErrorcode_NotFriend;
			}
			if (nRet != 0)
			{
				errorcode = YIMErrorcode_ServerError;
			}
			YouMe_LOG_Error(__XT("delete friend error(%d)"), nRet);
		}
	}
	m_pCallback->OnDeleteFriend(errorcode, userID.c_str());
}

YIMErrorcode YouMeFriendManager::BlackFriend(int type, std::vector<XString>& users)
{
	if (users.size() == 0)
	{
		return YIMErrorcode_ParamInvalid;
	}
	YIMErrorcode errorcode = YIMErrorcode_Success;
	if (0 == type)
	{
		for (std::vector<XString>::const_iterator itr = users.begin(); itr != users.end(); ++itr)
		{
			RequestBlackFriend(*itr);
		}
	}
	else if (1 == type)
	{
		for (std::vector<XString>::const_iterator itr = users.begin(); itr != users.end(); ++itr)
		{
			RequestUnBlackFriend(*itr);
		}
	}
	else
	{
		errorcode = YIMErrorcode_ParamInvalid;
	}
	return errorcode;
}

YIMErrorcode YouMeFriendManager::RequestBlackFriend(const XString& userID)
{
	if (userID.empty())
	{
		return YIMErrorcode_ParamInvalid;
	}

	YouMe_LOG_Debug(__XT("black friend %s"), userID.c_str());

	std::string strUserID = XStringToUTF8(userID);
	YOUMEServiceProtocol::BlackRcFriendReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_user_id(strUserID);
	std::string strData;
	req.SerializeToString(&strData);
	XUINT64 msgSerial = 0;
	std::map<std::string, std::string> extend;
	extend["UserID"] = strUserID;
	if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_BLACK_FRIEND, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		YouMe_LOG_Error(__XT("send request failed"));
		 YIMErrorcode_NetError;
	}
	return YIMErrorcode_Success;
}

void YouMeFriendManager::OnBlackFriendRsp(ServerPacket& serverPacket)
{
	if (m_pCallback == NULL)
	{
		YouMe_LOG_Error(__XT("callback is null"));
		return;
	}

	std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("UserID");
	if (itr == serverPacket.extend.end())
	{
		YouMe_LOG_Error(__XT("UserID not found"));
		return;
	}

	XString userID = UTF8TOXString(itr->second);
	YIMErrorcode errorcode = YIMErrorcode_Success;
	if (serverPacket.result == -1)
	{
		YouMe_LOG_Error(__XT("black friend timeout"));
		errorcode = YIMErrorcode_TimeOut;
	}
	else
	{
		YOUMEServiceProtocol::BlackRcFriendRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
			errorcode = YIMErrorcode_ServerError;
		}
		else
		{
			int nRet = rsp.ret();
			if (0 == nRet)
			{

			}
			else if (YOUMEServiceProtocol::BLACKFRIEND_NOT_FRIEND == nRet)
			{
				errorcode = YIMErrorcode_NotFriend;
			}
			else
			{
				errorcode = YIMErrorcode_ServerError;
			}
			YouMe_LOG_Error(__XT("black friend error(%d)"), nRet);
		}
	}
	m_pCallback->OnBlackFriend(errorcode, TYPE_BLACK_FRIEND, userID.c_str());
}

YIMErrorcode YouMeFriendManager::RequestUnBlackFriend(const XString& userID)
{
	YouMe_LOG_Debug(__XT("unblack friend %s"), userID.c_str());

	std::string strUserID = XStringToUTF8(userID);
	YOUMEServiceProtocol::UnblackRcFriendReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_user_id(strUserID);
	std::string strData;
	req.SerializeToString(&strData);
	XUINT64 msgSerial = 0;
	std::map<std::string, std::string> extend;
	extend["UserID"] = strUserID;
	if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_UNBLACK_FRIEND, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		YouMe_LOG_Error(__XT("send request failed"));
		YIMErrorcode_NetError;
	}
	return YIMErrorcode_Success;
}

void YouMeFriendManager::OnUnBlackFriendRsp(ServerPacket& serverPacket)
{
	if (m_pCallback == NULL)
	{
		YouMe_LOG_Error(__XT("callback is null"));
		return;
	}

	std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("UserID");
	if (itr == serverPacket.extend.end())
	{
		YouMe_LOG_Error(__XT("UserID not found"));
		return;
	}

	XString userID = UTF8TOXString(itr->second);
	YIMErrorcode errorcode = YIMErrorcode_Success;
	if (serverPacket.result == -1)
	{
		YouMe_LOG_Error(__XT("unblack friend timeout"));
		errorcode = YIMErrorcode_TimeOut;
	}
	else
	{
		YOUMEServiceProtocol::UnblackRcFriendRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
			errorcode = YIMErrorcode_ServerError;
		}
		else
		{
			int nRet = rsp.ret();
			if (nRet == 0)
			{
							
			}
			else if (YOUMEServiceProtocol::BLACKFRIEND_NOT_BLACK == nRet)
			{
				errorcode = YIMErrorcode_NotBlack;
			}
			else
			{
				errorcode = YIMErrorcode_ServerError;
			}
			YouMe_LOG_Error(__XT("unblack friend error(%d)"), nRet);
		}
	}
	m_pCallback->OnBlackFriend(errorcode, TYPE_UNBLACK_FRIEND, userID.c_str());
}

YIMErrorcode YouMeFriendManager::QueryFriends(int type, int startIndex, int count)
{
	YouMe_LOG_Debug(__XT("query friends type:%d start:%d count:%d"), type, startIndex, count);

	int start = startIndex < 0 ? 0 : startIndex;
	int number = count;
	if (number <= 0 || number > FRIEND_REQUEST_PAGE_SIZE)
	{
		number = FRIEND_REQUEST_PAGE_SIZE;
	}

	if (0 == type)
	{
		RequestFriendList(start, number);
	}
	else if (1 == type)
	{
		RequestBlackFriendList(start, number);
	}
	else
	{
		return YIMErrorcode_ParamInvalid;
	}

	return YIMErrorcode_Success;
}

YIMErrorcode YouMeFriendManager::RequestFriendList(int startIndex, int count)
{
	YOUMEServiceProtocol::GetRcFriendListReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_start_idx(startIndex);
	req.set_len(count);
	std::string strData;
	req.SerializeToString(&strData);
	XUINT64 msgSerial = 0;
	std::map<std::string, std::string> extend;
	extend["StartIndex"] = CStringUtilT<char>::to_string(startIndex);
	extend["Count"] = CStringUtilT<char>::to_string(count);
	if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_QUERY_FRIEND_LIST, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		YouMe_LOG_Error(__XT("send request failed"));
		return YIMErrorcode_NetError;
	}
	return YIMErrorcode_Success;
}

void YouMeFriendManager::OnFriendListRsp(ServerPacket& serverPacket)
{
	if (m_pCallback == NULL)
	{
		YouMe_LOG_Error(__XT("callback is null"));
		return;
	}

	int startIndex = 0;
	std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("StartIndex");
	if (itr != serverPacket.extend.end())
	{
		startIndex = CStringUtilT<char>::str_to_sint32(itr->second.c_str());
	}

	std::list<std::shared_ptr<IYIMUserBriefInfo> > userList;
	YIMErrorcode errorcode = YIMErrorcode_Success;
	if (serverPacket.result == -1)
	{
		YouMe_LOG_Error(__XT("query friends timeout"));
		errorcode = YIMErrorcode_TimeOut;
	}
	else
	{
		YOUMEServiceProtocol::GetRcFriendListRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
			errorcode = YIMErrorcode_ServerError;
		}
		else
		{
			int nRet = rsp.ret();
			if (nRet != 0)
			{
				YouMe_LOG_Error(__XT("query friends error(%d)"), nRet);
				errorcode = YIMErrorcode_ServerError;
			}
			else
			{
				YouMe_LOG_Debug(__XT("query request list total:%d"), rsp.friend_list_size());

				for (int i = 0; i < rsp.friend_list_size(); i++)
				{
					YOUMEServiceProtocol::RcFriendBriefInfo info = rsp.friend_list(i);
					std::shared_ptr<UserBriefInfo> userInfo(new UserBriefInfo);
					userInfo->userID = UTF8TOXString(info.user_id());
					userInfo->nickname = UTF8TOXString(info.nickname());
                    
                    if (info.online_state() == YOUMEServiceProtocol::RC_ONLINE_STATE::RC_ONLINE)
                    {
                        userInfo->status = STATUS_ONLINE;
                    }
                    else if (info.online_state() == YOUMEServiceProtocol::RC_ONLINE_STATE::RC_OFFLINE)
                    {
                        userInfo->status = STATUS_OFFLINE;
                    }
                    else if (info.online_state() == YOUMEServiceProtocol::RC_ONLINE_STATE::RC_HIDDEN)
                    {
                        userInfo->status = STATUS_INVISIBLE;
                    }

					userList.push_back(userInfo);
				}
			}
		}
	}

	m_pCallback->OnQueryFriends(errorcode, 0, startIndex, userList);
}

YIMErrorcode YouMeFriendManager::RequestBlackFriendList(int startIndex, int count)
{
	YOUMEServiceProtocol::GetRcBlackListReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_start_idx(startIndex);
	req.set_len(count);
	std::string strData;
	req.SerializeToString(&strData);
	XUINT64 msgSerial = 0;
	std::map<std::string, std::string> extend;
	extend["StartIndex"] = CStringUtilT<char>::to_string(startIndex);
	extend["Count"] = CStringUtilT<char>::to_string(count);
	if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_QUERY_BLACK_FRIEND_LIST, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		YouMe_LOG_Error(__XT("send request failed"));
		return YIMErrorcode_NetError;
	}
	return YIMErrorcode_Success;
}

void YouMeFriendManager::OnBlackFriendListRsp(ServerPacket& serverPacket)
{
	if (m_pCallback == NULL)
	{
		YouMe_LOG_Error(__XT("callback is null"));
		return;
	}
	
	int startIndex = 0;
	std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("StartIndex");
	if (itr != serverPacket.extend.end())
	{
		startIndex = CStringUtilT<char>::str_to_sint32(itr->second.c_str());
	}

	std::list<std::shared_ptr<IYIMUserBriefInfo> > userList;
	YIMErrorcode errorcode = YIMErrorcode_Success;
	if (serverPacket.result == -1)
	{
		YouMe_LOG_Error(__XT("query blacks timeout"));
		errorcode = YIMErrorcode_TimeOut;
	}
	else
	{
		YOUMEServiceProtocol::GetRcBlackListRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
			errorcode = YIMErrorcode_ServerError;
		}
		else
		{
			int nRet = rsp.ret();
			if (nRet != 0)
			{
				YouMe_LOG_Error(__XT("query balcks error(%d)"), nRet);
				errorcode = YIMErrorcode_ServerError;
			}
			else
			{
				YouMe_LOG_Debug(__XT("query request list total:%d"), rsp.black_list_size());

				for (int i = 0; i < rsp.black_list_size(); i++)
				{
					YOUMEServiceProtocol::RcFriendBriefInfo info = rsp.black_list(i);
					std::shared_ptr<UserBriefInfo> userInfo(new UserBriefInfo);
					userInfo->userID = UTF8TOXString(info.user_id());
					userInfo->nickname = UTF8TOXString(info.nickname());
					userInfo->status = info.online_state() == YOUMEServiceProtocol::RC_ONLINE ? STATUS_ONLINE : STATUS_OFFLINE;
					userList.push_back(userInfo);
				}
			}
		}
	}

	m_pCallback->OnQueryFriends(errorcode, 1, startIndex, userList);
}

YIMErrorcode YouMeFriendManager::QueryFriendRequestList(int startIndex, int count)
{
	int requestCount = count;
	if (requestCount < 0 || requestCount > FRIEND_REQUEST_PAGE_SIZE)
	{
		requestCount = FRIEND_REQUEST_PAGE_SIZE;
	}
	return QueryFriendRequestList(startIndex, requestCount, false);
}

YIMErrorcode YouMeFriendManager::QueryFriendRequestList(int startIndex, int count, bool isCheckNotify)
{
	YOUMEServiceProtocol::GetRcFriendReqListReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_start_idx(startIndex);
	req.set_len(count);
	std::string strData;
	req.SerializeToString(&strData);
	XUINT64 msgSerial = 0;
	std::map<std::string, std::string> extend;
	extend["StartIndex"] = CStringUtilT<char>::to_string(startIndex);
	extend["Count"] = CStringUtilT<char>::to_string(count);
	if (isCheckNotify)
	{
		extend["CheckNotify"] = "1";
	}
	if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_QUERY_FRIEND_REQUEST_LIST, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		YouMe_LOG_Error(__XT("send request failed"));
		return YIMErrorcode_NetError;
	}
	return YIMErrorcode_Success;
}

void YouMeFriendManager::CheckHasAddFriendRequest()
{
	QueryFriendRequestList(0, 30, true);
}

void YouMeFriendManager::OnQueryFriendRequestRsp(ServerPacket& serverPacket)
{
	if (m_pCallback == NULL)
	{
		YouMe_LOG_Error(__XT("callback is null"));
		return;
	}

	std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("CheckNotify");
	if (itr != serverPacket.extend.end())
	{
		OfflineFriendRequestNotify(serverPacket);
		return;
	}

	int startIndex = 0;
	itr = serverPacket.extend.find("StartIndex");
	if (itr == serverPacket.extend.end())
	{
		startIndex = CStringUtilT<char>::str_to_sint32(itr->second.c_str());
	}

	std::list<std::shared_ptr<IYIMFriendRequestInfo> > friendRequestList;
	YIMErrorcode errorcode = YIMErrorcode_Success;
	if (serverPacket.result == -1)
	{
		YouMe_LOG_Error(__XT("query request list timeout"));
		errorcode = YIMErrorcode_TimeOut;
	}
	else
	{
		YOUMEServiceProtocol::GetRcFriendReqListRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
			errorcode = YIMErrorcode_ServerError;
		}
		else
		{
			int nRet = rsp.ret();
			if (nRet != 0)
			{
				YouMe_LOG_Error(__XT("query request list error(%d)"), nRet);
				errorcode = YIMErrorcode_ServerError;
			}
			else
			{
				YouMe_LOG_Debug(__XT("query request list total:%d"), rsp.friend_req_list_size());

				for (int i = 0; i < rsp.friend_req_list_size(); i++)
				{
					YOUMEServiceProtocol::RcFriendReqInfo info = rsp.friend_req_list(i);
					std::shared_ptr<FriendRequestInfo> requestInfo(new FriendRequestInfo);
					if (info.req_type() == YOUMEServiceProtocol::TYPE_ADD)
					{
						requestInfo->askerID = UTF8TOXString(info.myself());
						requestInfo->askerNickname = UTF8TOXString(info.myself_nickname());
						requestInfo->inviteeID = UTF8TOXString(info.other_side());
						requestInfo->inviteeNickname = UTF8TOXString(info.other_side_nickname());
					}
					else if (info.req_type() == YOUMEServiceProtocol::TYPE_BE_ADDED)
					{
						requestInfo->askerID = UTF8TOXString(info.other_side());
						requestInfo->askerNickname = UTF8TOXString(info.other_side_nickname());
						requestInfo->inviteeID = UTF8TOXString(info.myself());
						requestInfo->inviteeNickname = UTF8TOXString(info.myself_nickname());
					}

					FriendValidateInfo fvi;
					fvi.requestID = info.req_id();
					fvi.verifyInfo = info.ext_info();
                    fvi.asker = requestInfo->askerID;
					m_requestAddFriendInfoMap.insert(std::make_pair(fvi.requestID, fvi));

					requestInfo->validateInfo = UTF8TOXString(info.ext_info());
					if (info.cur_state() == YOUMEServiceProtocol::ADD_SUCCESS)
					{
						requestInfo->status = STATUS_ADD_SUCCESS;
					}
					else if (info.cur_state() == YOUMEServiceProtocol::ADD_WAIT_OTHER_VERIFY)
					{
						requestInfo->status = STATUS_WAIT_OTHER_VALIDATE;
					}
					else if (info.cur_state() == YOUMEServiceProtocol::ADD_WAIT_ME_VERIFY)
					{
						requestInfo->status = STATUS_WAIT_ME_VALIDATE;
					}
					else if (info.cur_state() == YOUMEServiceProtocol::ADD_FAIL)
					{
						requestInfo->status = STATUS_ADD_FAILED;
					}
					requestInfo->createTime = info.create_time();
					requestInfo->hasNotify  = info.is_browsed_by_app();
                    requestInfo->reqID      = info.req_id();
					friendRequestList.push_back(requestInfo);
				}
			}
		}
	}
	
	m_pCallback->OnQueryFriendRequestList(errorcode, startIndex, friendRequestList);
}

void YouMeFriendManager::OfflineFriendRequestNotify(ServerPacket& serverPacket)
{
	if (serverPacket.result == -1)
	{
		YouMe_LOG_Error(__XT("query request list timeout"));
		return;
	}
	YOUMEServiceProtocol::GetRcFriendReqListRsp rsp;
	if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
	{
		YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
		return;
	}

	YOUMEServiceProtocol::AppBrowseRcFriendReqListReq friendRequestStatusReq;
	friendRequestStatusReq.set_version(PROTOCOL_VERSION);
	for (int i = 0; i < rsp.friend_req_list_size(); i++)
	{
		YOUMEServiceProtocol::RcFriendReqInfo info = rsp.friend_req_list(i);
		if (info.req_type() == YOUMEServiceProtocol::TYPE_BE_ADDED && !info.is_browsed_by_app())
		{
			XString askerID = UTF8TOXString(info.other_side());

			FriendValidateInfo requestInfo;
			requestInfo.requestID = info.req_id();
			requestInfo.verifyInfo = info.ext_info();
            requestInfo.asker = askerID;
			m_requestAddFriendInfoMap.insert(std::make_pair(requestInfo.requestID, requestInfo));

			m_pCallback->OnBeRequestAddFriendNotify(askerID.c_str(), UTF8TOXString(info.ext_info()).c_str(), info.req_id());

			YOUMEServiceProtocol::RcFriendReqIdx* fri = friendRequestStatusReq.add_friend_req_list();
			fri->set_app_id(info.app_id());
			fri->set_req_id(info.req_id());
			fri->set_req_type(info.req_type());
			fri->set_other_side(info.other_side());
		}
	}

	if (friendRequestStatusReq.friend_req_list_size() > 0)	// 修改好友请求通知状态
	{
		std::string strData;
		friendRequestStatusReq.SerializeToString(&strData);
		XUINT64 msgSerial = 0;
		std::map<std::string, std::string> extend;
		if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_UPDATE_FRIEND_RQUEST_STATUS, strData.c_str(), (int)strData.length(), extend, msgSerial))
		{
			YouMe_LOG_Error(__XT("send request failed"));
		}
	}
}

void YouMeFriendManager::OnUpdateFriendRequestStatusRsp(ServerPacket& serverPacket)
{
	if (serverPacket.result == -1)
	{
		YouMe_LOG_Error(__XT("update friend request status timeout"));
	}
	else
	{
		YOUMEServiceProtocol::AppBrowseRcFriendReqListRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
		}
		else
		{
			int nRet = rsp.ret();
			if (nRet != 0)
			{
				YouMe_LOG_Error(__XT("update friend request status error(%d)"), nRet);
			}
		}
	}
}

void YouMeFriendManager::OnRelationChainHeartBeatRsp(ServerPacket& serverPacket)
{
	if (serverPacket.result == -1)
	{
		YouMe_LOG_Info(__XT("update friend request status timeout"));
	}
	else
	{
		YOUMEServiceProtocol::RcHeartbeatRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
		}
		else
		{
			int nRet = rsp.ret();
			if (nRet != 0)
			{
				YouMe_LOG_Warning(__XT("relation heartbeat error(%d)"), nRet);
			}
		}
	}
}

void YouMeFriendManager::UpdateRequestAddFriendStatus(XUINT64 requestID, YOUMEServiceProtocol::RC_FRIEND_REQ_TYPE requestType, const std::string& otherSide)
{
	if (NULL == m_pIManager)
	{
		return;
	}
	YOUMEServiceProtocol::AppBrowseRcFriendReqListReq friendRequestStatusReq;
	friendRequestStatusReq.set_version(PROTOCOL_VERSION);
	YOUMEServiceProtocol::RcFriendReqIdx* fri = friendRequestStatusReq.add_friend_req_list();
	fri->set_app_id(m_pIManager->GetAppID());
	fri->set_req_id(requestID);
	fri->set_req_type(requestType);
	fri->set_other_side(otherSide);
	std::string strData;
	friendRequestStatusReq.SerializeToString(&strData);
	XUINT64 msgSerial = 0;
	std::map<std::string, std::string> extend;
	if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_UPDATE_FRIEND_RQUEST_STATUS, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		YouMe_LOG_Error(__XT("send request failed"));
	}
}
