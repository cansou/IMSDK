// stdafx.cpp : 只包括标准包含文件的源文件
// CrossPlatformDefine.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

/************************************************************************/
/*
											  _ooOoo_
                                           o8888888888o
                                        888    " . "   888
                                      (|        -_-       |)
                                     O\          =         /O
                                     ____/`     ---   '\____
                               .'  \\|                     |//  `.
                               /  \\|||          :          |||//  \
                               /  _|||||        -:-         |||||-  \
                          |   | \\\              -               /// |   |
                           | \_|             ''\---/''              |_/ |
                           \  .-\__             `-`              __/-. /
                     __    ___`. .'           /--.--\            `. . _____
                         ."" '<  `.___\_        <|>          _/___.'  >' "".
                   | | :  `- \`.;`             \ _ /              `;.`/ - ` : | |
                     \  \ `-.              \_ __\ /__ _/              .-` /  /
				    	 ========`-.____`-.___\_____/___.-`____.-'========
                                              `=---='
								      佛祖保佑       永无BUG
 * /
************************************************************************/

#include "YouMeIMManager.h"

#include <regex>

#include <YouMeCommon/CryptUtil.h>
#include <YouMeCommon/json/json.h>
#include <YouMeCommon/RSAUtil.h>
#include <YouMeCommon/StringUtil.hpp>
#include <YouMeCommon/XFile.h>
#include <YouMeCommon/DownloadUploadManager.h>
#include <YouMeCommon/minizip/MiniZip.h>
#include <YouMeCommon/minizip/MiniUnZip.h>
#include <YouMeCommon/KeywordFilter.h>
#include <YouMeCommon/TranslateUtil.h>
#include <YouMeCommon/DataReport.h>
#include <YouMeCommon/YouMeDataChannel.h>
#include <YouMeCommon/CrossPlatformDefine/IYouMeSystemProvider.h>
#include <YouMeCommon/DNSUtil.h>

#include <YouMeIMEngine/pb/youme_login.pb.h>
#include <YouMeIMEngine/pb/youme_logout.pb.h>
#include <YouMeIMEngine/pb/youme_heartbeat.pb.h>
#include <YouMeIMEngine/pb/server_protocol_code.h>
#include <YouMeIMEngine/pb/youme_kickoff.pb.h>
#include <YouMeIMEngine/pb/youme_check_online.pb.h>
#include <YouMeIMEngine/pb/youme_userinfo.pb.h>
#include <YouMeIMEngine/pb/youme_notify.pb.h>
#include <YouMeIMEngine/Speech/YouMeSpeechManager.h>
#include <YouMeIMEngine/Location/YouMeLocationManager.h>
#include <YouMeIMEngine/NoticeManager.h>
#include <YouMeIMEngine/YouMeFriendManager.h>


#define SDK_AUTH_URL "conn.youme.im"
#define SDK_AUTH_URL_DEV "d.conn.youme.im"
#define SDK_AUTH_URL_TEST "t.conn.youme.im"
#define SDK_AUTH_URL_BUSSINESS "b.conn.youme.im"
#define SDK_INVALIDATE_DEFAULT_IP "47.106.77.206"
#define SDK_INVALIDATE_DEFAULT_IP_2 "120.79.5.50"
#define SDK_INVALIDATE_PORT 8080
#define SDK_INVALIDATE_PORT2 8081
#define SDK_INVALIDATE_PORT3 5000

#define DATAREPORT_ADDR_DEV "d.dr.youme.im"
#define DATAREPORT_ADDR_TEST "t.dr.youme.im"
#define DATAREPORT_ADDR_BUSSINESS "cn.dr.youme.im"
#define REPORT_DEFAULT_ADDR "123.59.62.126"	//123.59.77.129		5574
#define REPORT_DEFAULT_ADDR_2 "47.89.13.3"
#define REPORT_PORT_TCP 8001
#define REPORT_PORT_UDP 8001

#define RECONNECT_SDK_COUNT 3
#define RECONNECT_COUNT 2
#define RECONNECT_TIMEOUT 5000
#define HEARTBEAT_INTERVAL 30
#define HEARTBEAT_TIMEOUT_COUNT 3
#define RETRY_SEND_TIMES 2			//重发次数
#define INITIATIVE_GET_MESSAGE_INTERVAL 60000

#ifdef WIN32

#define XSleep(x) Sleep(x)

#else

#include <unistd.h>
#define XSleep(x) usleep(x*1000)

#endif // not WIN32


#ifdef OS_ANDROID
extern int GetNetworkType();
#endif // OS_ANDROID

extern IYouMeSystemProvider* g_pSystemProvider;
extern CProfileDB* g_pProfileDB;
extern CKeywordFilter* g_pKeywordFilter;
extern CTranslateUtil* g_pTranslateUtil;
extern bool s_bYouMeStartup;
extern ServerZone g_ServerZone;
XUINT64 YouMeIMManager::s_iPacketSerial = 0;
std::mutex YouMeIMManager::m_serialMutex;

extern int g_iYouMeIMMode;
extern int g_iAppID;
extern CYouMeSpeechManager* g_pSpeechManager;

static std::string userInfoKeys[] = { "nickname", "server_area_id", "server_area", "location_id", "location", "level", "vip_level" };
std::map<ServerZone, std::string> YouMeIMManager::m_sdkDomainMap;

//全局的互斥，用于一些速度快，并且存在线程安全问题的锁，避免搞多个互斥。一定是速度快，否则会因为不必要的竞争导致性能问题
std::mutex* g_youMeGlobalMutex = new std::mutex;

static short initiativeGetMessageInterval[] = {1, 2, 4, 8};
IPPortItem YouMeIMManager::m_privateServerLoginAddress;
extern YouMeIMManager* g_pYouMeIMManager;

int g_YM_isPrivateServer=0;

void YouMeIMManager::SetLoginCallback(IYIMLoginCallback* pCallback)
{
    YouMe_LOG_Info(__XT("Enter"));
	m_pLoginCallback = pCallback;
}

//设置消息回调
void YouMeIMManager::SetMessageCallback(IYIMMessageCallback* pCallback)
{
    YouMe_LOG_Info(__XT("Enter"));
    m_pMessageCallback = pCallback;
	if (NULL != m_pMessageManager)
	{
		m_pMessageManager->SetMessageCallback(pCallback);
	}
}
//设置群组回调
void YouMeIMManager::SetChatRoomCallback(IYIMChatRoomCallback* pCallback)
{
    YouMe_LOG_Info(__XT("Enter"));
    m_pGroupCallback = pCallback;
	if (NULL != m_pGroupManager)
	{
		m_pGroupManager->SetCallback(pCallback);
	}
}

void YouMeIMManager::SetAudioPlayCallback(IYIMAudioPlayCallback* pCallback)
{
    YouMe_LOG_Info(__XT("Enter"));
	if (g_pSpeechManager != NULL)
	{
		g_pSpeechManager->SetAudioPlayCallback(pCallback);
	}
}

void YouMeIMManager::SetLocationCallback(IYIMLocationCallback* pCallback)
{
    YouMe_LOG_Info(__XT("Enter"));
	m_pLocationCallback = pCallback;
    if (m_pLocationManager != NULL)
	{
		m_pLocationManager->SetCallback(pCallback);
	}
}

void YouMeIMManager::SetNoticeCallback(IYIMNoticeCallback* pCallback)
{
    YouMe_LOG_Info(__XT("Enter"));
	m_pNoticeCallback = pCallback;
	if (m_pNoticeManager != NULL)
	{
		m_pNoticeManager->SetNoticeCallback(pCallback);
	}
}

//设置重连回调
void YouMeIMManager::SetReconnectCallback(IYIMReconnectCallback *pCallback)
{
    YouMe_LOG_Info(__XT("Enter"));
    m_pReconnectCallback = pCallback;
}

YIMErrorcode YouMeIMManager::Login(const XCHAR* userID, const XCHAR* password, const XCHAR* token)
{
	if (userID == NULL || XStrLen(userID) == 0 || password == NULL || token == NULL)
	{
		return YIMErrorcode_ParamInvalid;
	}
	std::regex pattern("^(\\w|-|=|[+])+$");
	std::string strUserID = XStringToUTF8(XString(userID));
	if (!std::regex_match(strUserID, pattern))
	{
		return YIMErrorcode_ParamInvalid;
	}

	YouMe_LOG_Info(__XT("login userID:%s initstatus:%d loginstatus:%d"), userID, m_initStatus, m_loginStatus);
    
	if (g_pSystemProvider == NULL || g_pSystemProvider->getAppKey().empty())
	{
		return YIMErrorcode_EngineNotInit;
	}
	if (IMManangerLoginStatus_Success == m_loginStatus)
	{
		return YIMErrorcode_AlreadyLogin;
	}
	else if (IMManangerLoginStatus_Logouting == m_loginStatus)
	{
		return YIMErrorcode_StatusError;
	}
	else if ((IMManangerLoginStatus_Ing == m_loginStatus && IMManagerStatus_Init == m_initStatus) || IMManangerLoginStatus_Reconnecting == m_loginStatus)
	{
		return YIMErrorcode_Success;
	}

	m_bIsCleanup = false;
	m_strCurUserID = XString(userID);
	m_strPasswd = XString(password);
	m_strToken = XString(token);

	m_heartBeatWait.Reset();
	m_logoutWait.Reset();
	m_loginStatus = IMManangerLoginStatus_Ing;
	m_iCurentLoginAddrIndex = 0;
	m_bConnectFaileRevalidate = true;

	YIMErrorcode errorcode = YIMErrorcode_Success;
	if (IMManagerStatus_Init == m_initStatus)
	{
		errorcode = ConnectLoginServer();
	}
	else if (IMManagerStatus_initfailed == m_initStatus)
	{
		youmecommon::SDKValidateErrorcode errorCode = ValidateSDK(youmecommon::VALIDATEREASON_INIT);
		if (errorCode == youmecommon::SDKValidateErrorcode_Success)
		{
			m_initStatus = IMManagerStatus_Ing;
		}
		else
		{
			m_initStatus = IMManagerStatus_initfailed;
			m_loginStatus = IMManangerLoginStatus_Not;
			YouMe_LOG_Error(__XT("SDK validate error(%d)"), errorCode);
			return YIMErrorcode_EngineNotInit;
		}
	}

	YouMe_LOG_Info(__XT("Leave"));
	return errorcode;
}

YIMErrorcode YouMeIMManager::ConnectLoginServer()
{
	if (m_loginAddrList.size() == 0 && g_YM_isPrivateServer == 0)
	{
		YouMe_LOG_Error(__XT("login addr is empty"));
		return YIMErrorcode_EngineNotInit;
    }else if(m_loginAddrList.size() == 0 && g_YM_isPrivateServer == 1)
    {
        m_loginAddrList.push_back( m_privateServerLoginAddress );
    }

	if (m_pNetworkServivce == NULL)
	{
		m_pNetworkServivce = new NetworkService;
		m_pNetworkServivce->Init(this);
		youmecommon::CXSharedArray<unsigned char> secretKey;
		m_pNetworkServivce->SetSessionInfo(m_strCurUserID, m_iAppID, secretKey);
	}

	if (m_pNetworkServivce->IsConnect())
	{
		return RequestLogin();
	}
	else 
	{
		if (m_iCurentLoginAddrIndex < m_loginAddrList.size())
		{
             YouMe_LOG_Debug(__XT("login addr index: %d ip:%s port:%d"),m_iCurentLoginAddrIndex,m_loginAddrList[m_iCurentLoginAddrIndex].addr.c_str(),m_loginAddrList[m_iCurentLoginAddrIndex].port );
			int ret = m_pNetworkServivce->Connect(m_loginAddrList[m_iCurentLoginAddrIndex++]);
			return ret == 0 ? YIMErrorcode_Success : YIMErrorcode_UnknowError;
		}
		else
		{
			YouMe_LOG_Error(__XT("login addr index error"));
			return YIMErrorcode_UnknowError;
		}
	}
}

YIMErrorcode YouMeIMManager::Logout()
{
	YouMe_LOG_Info(__XT("logout initstatus:%d loginstatus:%d"), m_initStatus, m_loginStatus);

	if (m_loginStatus == IMManangerLoginStatus_Not || m_loginStatus == IMManangerLoginStatus_Logouting)
	{
		return YIMErrorcode_StatusError;
	}

	m_loginStatus = IMManangerLoginStatus_Logouting;
	m_heartBeatWait.SetSignal();
	m_reconnectWait.SetSignal();
	m_strLastLoginedUserID = __XT("");

	if (m_logoutThread.joinable())
	{
		m_logoutThread.join();
	}
	m_logoutThread = std::thread(&YouMeIMManager::LogoutThread, this);

	YouMe_LOG_Info(__XT("Leave"));
	return YIMErrorcode_Success;
}

void YouMeIMManager::LogoutThread()
{
	YouMe_LOG_Info(__XT("Enter"));

	{
		std::lock_guard<std::mutex> lock(m_reconnectThreadMutex);
		if (m_reconnectThread.joinable())
		{
			m_reconnectThread.join();
		}
	}

	YOUMEServiceProtocol::LogoutReq req;
	req.set_version(PROTOCOL_VERSION);
	std::string strData;
	req.SerializeToString(&strData);
	XUINT64 msgSerail = 0;
	std::map<std::string, std::string> extend;
	if (!SendData(YOUMEServiceProtocol::CMD_LOGOUT, strData.c_str(), (int)strData.length(), extend, msgSerail))
	{
		std::thread cleanup(&YouMeIMManager::DealLogoutCleanup, this, true);
		cleanup.detach();
		YouMe_LOG_Error(__XT("send logout failed"));
	}

	YouMe_LOG_Info(__XT("Leave"));
}

YIMErrorcode YouMeIMManager::GetRecentContacts()
{
	if (m_loginStatus != IMManangerLoginStatus_Success && m_loginStatus != IMManangerLoginStatus_Reconnecting)
	{
		return YIMErrorcode_NotLogin;
	}
	if (m_pMessageManager != NULL)
	{
		return m_pMessageManager->GetRecentContacts();
	}
	return YIMErrorcode_UnknowError;
}

YIMErrorcode YouMeIMManager::QueryUserStatus(const XCHAR* userID)
{
	if (NULL == userID)
	{
		return YIMErrorcode_ParamInvalid;
	}
	XString strTemp = XString(userID);
	if (strTemp.empty())
	{
		return YIMErrorcode_ParamInvalid;
	}
	if (m_loginStatus != IMManangerLoginStatus_Success && m_loginStatus != IMManangerLoginStatus_Reconnecting)
	{
		return YIMErrorcode_NotLogin;
	}

	std::string strUserID = XStringToUTF8(strTemp);
	YOUMEServiceProtocol::CheckOnlineReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_user_id(strUserID);
	std::string strData;
	req.SerializeToString(&strData);
	XUINT64 msgSerail = 0;
	std::map<std::string, std::string> extend;
	extend["UserID"] = strUserID;
	if (!SendData(YOUMEServiceProtocol::CMD_CHECK_ONLINE, strData.c_str(), (int)strData.length(), extend, msgSerail))
	{
		return YIMErrorcode_UnknowError;
	}
	return YIMErrorcode_Success;
}

void YouMeIMManager::OnQueryUserStatusRsp(ServerPacket& serverPacket)
{
	if (NULL == m_pContactCallback)
	{
		YouMe_LOG_Error(__XT("contact callback is NULL"));
		return;
	}
	YIMErrorcode errorCode = YIMErrorcode_Success;
	YIMUserStatus status = STATUS_OFFLINE;
	XString strUserID;
	std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("UserID");
	if (itr != serverPacket.extend.end())
	{
		strUserID = UTF8TOXString(itr->second);
	}
	if (-1 == serverPacket.result)
	{
		YouMe_LOG_Error(__XT("QueryUserStatus timeout"));
		errorCode = YIMErrorcode_TimeOut;
	}
	else
	{
		YOUMEServiceProtocol::CheckOnlineRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("QueryUserStatus error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
			errorCode = YIMErrorcode_ServerError;
		}
		else
		{
			if (rsp.ret() != 0)
			{
				errorCode = YIMErrorcode_ServerError;
				YouMe_LOG_Error(__XT("logout error(%d)"), rsp.ret());
			}
			else
			{
				status = rsp.is_online() ? STATUS_ONLINE : STATUS_OFFLINE;
			}
		}
	}

	m_pContactCallback->OnQueryUserStatus(errorCode, strUserID.c_str(), status);
}

YIMErrorcode YouMeIMManager::SetUserInfo(const XCHAR* userInfo)
{
	if (NULL == userInfo)
	{
		return YIMErrorcode_ParamInvalid;
	}
	XString strUserInfo = XString(userInfo);
	if (strUserInfo.empty())
	{
		return YIMErrorcode_ParamInvalid;
	}
	if (m_loginStatus != IMManangerLoginStatus_Success && m_loginStatus != IMManangerLoginStatus_Reconnecting)
	{
		return YIMErrorcode_NotLogin;
	}

	std::string strInfo = XStringToUTF8(strUserInfo);
	youmecommon::Value value;
	youmecommon::Reader jsonReader;
	if (!jsonReader.parse(strInfo, value))
	{
		return YIMErrorcode_ParamInvalid;
	}
	std::vector<std::string> names = value.getMemberNames();
	int nKeyNum = sizeof(userInfoKeys) / sizeof(userInfoKeys[0]);
	for (int i = 0; i < nKeyNum; ++i)
	{
		if (find(names.begin(), names.end(), userInfoKeys[i]) == names.end())
		{
			YouMe_LOG_Info(__XT("SetUserInfo no key:%s"), UTF8TOXString(userInfoKeys[i]).c_str());
			return YIMErrorcode_ParamInvalid;
		}
	}

	if (IMManangerLoginStatus_Not == m_loginStatus)
	{
		return YIMErrorcode_NotLogin;
	}

	youmecommon::Value deviceVal;
	deviceVal["deviceId"] = g_pSystemProvider->getUUID();
#ifdef WIN32
	deviceVal["os"] = XString(__XT("windows"));
#elif OS_OSX
	deviceVal["os"] = XString(__XT("osx"));
#elif OS_IOS
	deviceVal["os"] = XString(__XT("ios"));
#elif OS_IOSSIMULATOR
	deviceVal["os"] = XString(__XT("ios"));
#elif OS_ANDROID
	deviceVal["os"] = XString(__XT("android"));
#endif
	deviceVal["fr"] = g_pSystemProvider->getSystemVersion();
	deviceVal["model"] = g_pSystemProvider->getBrand();
	deviceVal["brand"] = g_pSystemProvider->getModel();
	if (youmecommon::NetworkType_Unknow == m_nNetworkType)
	{
		deviceVal["net"] = XString(__XT("unknow"));
	}
	else if (youmecommon::NetworkType_3G == m_nNetworkType)
	{
		deviceVal["net"] = XString(__XT("mob"));
	}
	else if (youmecommon::NetworkType_Wifi == m_nNetworkType)
	{
		deviceVal["net"] = XString(__XT("wifi"));
	}
	if (m_pNetworkServivce != NULL)
	{
		deviceVal["ip"] = m_pNetworkServivce->LocalIP();
	}
	deviceVal["res"] = "";
	deviceVal["mac"] = "";
	deviceVal["imsi"] = "";
	value["device_info"] = deviceVal;
	std::string strTemp = value.toStyledString();

	YOUMEServiceProtocol::UpdateUserInfoReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_user_info(strTemp);
	std::string strData;
	req.SerializeToString(&strData);
	XUINT64 msgSerail = 0;
	std::map<std::string, std::string> extend;
	if (!SendData(YOUMEServiceProtocol::CMD_UPDATE_USER_INFO, strData.c_str(), (int)strData.length(), extend, msgSerail))
	{
		return YIMErrorcode_UnknowError;
	}
	return YIMErrorcode_Success;
}

void YouMeIMManager::OnSetUserInfoRsp(ServerPacket& serverPacket)
{
	if (-1 == serverPacket.result)
	{
		YouMe_LOG_Error(__XT("SetUserInfo timeout serial:%llu"), serverPacket.reqSerial);
	}
	else
	{
		YOUMEServiceProtocol::UpdateUserInfoRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("SetUserInfo error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
		}
		else
		{
			if (rsp.ret() != 0)
			{
				YouMe_LOG_Error(__XT("SetUserInfo error(%d)"), rsp.ret());
			}
		}
	}
}

YIMErrorcode YouMeIMManager::GetUserInfo(const XCHAR* userID)
{
	if (NULL == userID)
	{
		return YIMErrorcode_ParamInvalid;
	}
	XString strTemp = XString(userID);
	if (strTemp.empty())
	{
		return YIMErrorcode_ParamInvalid;
	}
	if (IMManangerLoginStatus_Not == m_loginStatus)
	{
		return YIMErrorcode_NotLogin;
	}

	std::string strUserID = XStringToUTF8(strTemp);
	YOUMEServiceProtocol::GetUserInfoReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_user_id(strUserID);
	std::string strData;
	req.SerializeToString(&strData);
	XUINT64 msgSerail = 0;
	std::map<std::string, std::string> extend;
	extend["UserID"] = strUserID;
	if (!SendData(YOUMEServiceProtocol::CMD_GET_USR_INFO, strData.c_str(), (int)strData.length(), extend, msgSerail))
	{
		return YIMErrorcode_UnknowError;
	}
	return YIMErrorcode_Success;
}

void YouMeIMManager::OnGetUserInfoRsp(ServerPacket& serverPacket)
{
	if (NULL == m_pContactCallback)
	{
		YouMe_LOG_Error(__XT("login callback is NULL"));
		return;
	}

	std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("UserID");
	if (itr == serverPacket.extend.end())
	{
		YouMe_LOG_Error(__XT("UserID not found"));
		return;
	}

	YIMErrorcode errorCode = YIMErrorcode_Success;
	XString strResult;
	if (-1 == serverPacket.result)
	{
		YouMe_LOG_Error(__XT("GetUserInfo timeout"));
		errorCode = YIMErrorcode_TimeOut;
	}
	else
	{
		YOUMEServiceProtocol::GetUserInfoRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("GetUserInfo error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
			errorCode = YIMErrorcode_ServerError;
		}
		else
		{
			strResult = UTF8TOXString(rsp.user_info());
		}
	}
	XString strUserID = UTF8TOXString(itr->second);

	m_pContactCallback->OnGetUserInfo(errorCode, strUserID.c_str(), strResult.c_str());
}

void YouMeIMManager::OnRecvPacket(ServerPacket& serverPacket)
{
	if (YOUMEServiceProtocol::CMD_HEARTBEAT == serverPacket.commondID)
	{
		YouMe_LOG_Debug(__XT("receive packet command:%d msgSerial:%llu bufferSize:%u"), serverPacket.commondID, serverPacket.reqSerial, serverPacket.packetSize);
	}
	else if (YOUMEServiceProtocol::CMD_RELATION_CHAIN_HEARTBEAT == serverPacket.commondID)
	{
	}
	else
	{
		YouMe_LOG_Info(__XT("receive packet command:%d msgSerial:%llu bufferSize:%u"), serverPacket.commondID, serverPacket.reqSerial, serverPacket.packetSize);
	}

	if (serverPacket.commondID != YOUMEServiceProtocol::CMD_LOGOUT)
	{
		if (m_loginStatus == IMManangerLoginStatus_Logouting)
		{
			return;
		}
	}

    switch (serverPacket.commondID)
    {
        case YOUMEServiceProtocol::CMD_LOGIN:
        {
            OnLoginRsp(serverPacket);
        }
            break;
        case YOUMEServiceProtocol::CMD_HEARTBEAT:
        {
            OnHeartBeatRsp(serverPacket);
        }
            break;
        case YOUMEServiceProtocol::CMD_LOGOUT:
        {
            OnLogoutRsp(serverPacket);
        }
            break;
        case YOUMEServiceProtocol::CMD_KICK_OFF:
        {
            OnKickOffRsp(serverPacket);
        }
            break;
        case YOUMEServiceProtocol::CMD_RELOGIN:
        {
            OnReloginNotify(serverPacket);
        }
            break;
        case YOUMEServiceProtocol::CMD_CHECK_ONLINE:
        {
            OnQueryUserStatusRsp(serverPacket);
        }
            break;
        case YOUMEServiceProtocol::CMD_UPDATE_USER_INFO:
        {
            OnSetUserInfoRsp(serverPacket);
        }
            break;
        case YOUMEServiceProtocol::CMD_GET_USR_INFO:
        {
            OnGetUserInfoRsp(serverPacket);
            if (m_pLocationManager != NULL)
            {
                m_pLocationManager->OnRecvPacket(serverPacket);
            }
        }
            break;
        case YOUMEServiceProtocol::NOTIFY_UPDATE_CONFIG:
        {
            if(g_YM_isPrivateServer == 0) OnUpdateConfigNotify(serverPacket);
        }
            break;
        case YOUMEServiceProtocol::NOTIFY_ROOM_GENERAL:
        {
            YOUMEServiceProtocol::NotifyReq rsp;
            if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
            {
                YouMe_LOG_Error(__XT("notify unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
                return;
            }
            if (rsp.notify_type() >= 1 && rsp.notify_type() <= 3)
            {
                if (m_pNoticeManager != NULL)
                {
                    m_pNoticeManager->OnReceNoticeNotify(rsp);
                }
            }
            else if (rsp.notify_type() == 4 || rsp.notify_type() == 5)
            {
                if (m_pGroupManager != NULL)
                {
                    m_pGroupManager->OnJoinLeaveRoomNotify(rsp);
                }
            }
        }
            break;
        case YOUMEServiceProtocol::CMD_QUERY_NOTICE:
        {
            if (m_pNoticeManager != NULL)
            {
                m_pNoticeManager->OnQueryNoticeRsp(serverPacket);
            }
        }
            break;
        case YOUMEServiceProtocol::CMD_GET_UPLOAD_TOKEN:
        {
			if (m_pUserProfileManager != NULL && m_pUserProfileManager->m_uploadImage)
            {
                m_pUserProfileManager->OnRecvPacket(serverPacket);
            }else{
                if (m_pMessageManager != NULL)
                {
                    m_pMessageManager->OnRecvPacket(serverPacket);
                }
            }
        }
            break;
		case YOUMEServiceProtocol::CMD_GET_ALI_TOKEN:
		{
			if (g_pSpeechManager != NULL)
			{
				g_pSpeechManager->OnAliAccessTokenRsp(serverPacket);
			}
		}
        default:
        {
            if (m_pMessageManager != NULL)
            {
                m_pMessageManager->OnRecvPacket(serverPacket);
            }
            if (m_pGroupManager != NULL)
            {
                m_pGroupManager->OnRecvPacket(serverPacket);
            }
            if (m_pUserProfileManager != NULL)
            {
                m_pUserProfileManager->OnRecvPacket(serverPacket);
            }
            if (m_pLocationManager != NULL)
            {
                m_pLocationManager->OnRecvPacket(serverPacket);
            }
			if (m_pFriendManager != NULL)
			{
				m_pFriendManager->OnRecvPacket(serverPacket);
			}
        }
            break;
    }
}

void YouMeIMManager::OnSDKValidteComplete(youmecommon::SDKValidateErrorcode errorcode, std::map<std::string, youmecommon::CXAny>& configurations, unsigned int constTime, youmecommon::ValidateReason reason, std::string& ip)
{
	YouMe_LOG_Info(__XT("SDK validate complete errorcode:%d time:%d reason:%d"), errorcode, constTime, reason);

	if (m_loginStatus == IMManangerLoginStatus_Logouting)
	{
		YouMe_LOG_Info(__XT("logouting validate exit"));
		return;
	}

	if (errorcode != youmecommon::SDKValidateErrorcode_Success)
	{
		YouMe_LOG_Error(__XT("SDK validate error(%d)"), errorcode);

		if (youmecommon::VALIDATEREASON_INIT == reason)
		{
			m_initStatus = IMManagerStatus_initfailed;
			IMManangerLoginStatus lastLoginStatus = m_loginStatus;
			m_loginStatus = IMManangerLoginStatus_Not;
			if (IMManangerLoginStatus_Ing == lastLoginStatus && NULL != m_pLoginCallback)
			{
				m_pLoginCallback->OnLogin(YIMErrorcode_InitFailed, m_strCurUserID.c_str());
			}
			ReportValidate(errorcode, constTime, ip);
		}
		else if (youmecommon::VALIDATEREASON_RECONNECT == reason)
		{
			m_iReconnectLoginTimes = 0;
			m_initStatus = IMManagerStatus_initfailed;
			m_loginStatus = IMManangerLoginStatus_Not;
			m_iCurentLoginAddrIndex = 0;
            m_strLastLoginedUserID = __XT("");

			if (m_pReconnectCallback != NULL)
			{
				m_pReconnectCallback->OnRecvReconnectResult(RECONNECTRESULT_FAIL);
			}

			if (NULL != m_pLoginCallback)
			{
				m_pLoginCallback->OnLogout(YIMErrorcode_Disconnect);
			}

		}

		return;
	}

	{
		std::lock_guard<std::mutex> lock(m_ConfigMutex);
		m_configurations = configurations;
	}

	// 登录地址
	m_iCurentLoginAddrIndex = 0;
	m_loginAddrList.clear();
	if (m_privateServerLoginAddress.addr.empty())
	{

		XString strAddrPorts = GetConfigure<XString>(ACCESS_SERVER_ADDR_PORT_ALL, __XT(""));
		std::vector<XString> loginAddr;
		CStringUtilT<XCHAR>::splitString(strAddrPorts, __XT(";"), loginAddr);
		for (std::vector<XString>::size_type i = 0; i < loginAddr.size(); ++i)
		{
			std::vector<XString> vecAddr;
			CStringUtilT<XCHAR>::splitString(loginAddr[i], __XT(","), vecAddr);
			if (vecAddr.size() != 2)
			{
				continue;
			}

			std::vector<std::string> ipList;
			std::string addr = XStringToUTF8(vecAddr[0]);
			youmecommon::DNSUtil::Instance()->GetHostByName(addr, ipList);
			for (std::vector<std::string>::const_iterator itr = ipList.begin(); itr != ipList.end(); ++itr)
			{
				IPPortItem item;
				item.addr = *itr;
				item.port = CStringUtil::str_to_uint16(vecAddr[1]);
				m_loginAddrList.push_back(item);
			}
		}

		if (m_loginAddrList.size() == 0)
		{
			YouMe_LOG_Error(__XT("ip list is empty"));
			if (NULL != m_pLoginCallback)
			{
				m_pLoginCallback->OnLogin(YIMErrorcode_InitFailed, m_strCurUserID.c_str());
			}
			return;
		}
		std::random_shuffle(m_loginAddrList.begin(), m_loginAddrList.end());	// 登录地址打乱顺序
	}
	else
	{
		// 私服部署登录，使用外部设置的ip和端口
		m_loginAddrList.push_back(m_privateServerLoginAddress);
		m_loginAddrList.push_back(m_privateServerLoginAddress);
	}

	if (youmecommon::VALIDATEREASON_INIT == reason)
	{
		InitReport();
		if (g_pSpeechManager != NULL)
		{
			g_pSpeechManager->SetIMManager(this);
			g_pSpeechManager->Init();
		}

		XUINT64 ullTimeStamp = GetConfigure(CONFIG_SERVER_TIME, 0llu);
		if (ullTimeStamp == 0)
		{
			ullTimeStamp = youmecommon::CTimeUtil::GetTimeOfDay_MS();
		}
		{
			std::lock_guard<std::mutex> lock(m_serialMutex);
			s_iPacketSerial = ullTimeStamp;
		}

		m_iAppID = GetConfigure<int>(APP_SERVICE_ID, 0);

		int nTranslateSwitch = GetConfigure<unsigned int>(CONFIG_TRANSLATE_ENABLE, TRANSLATE_SWITCH_DEFAULT);
        int nTranslateMethod = GetConfigure<unsigned int>(CONFIG_TRANSLATE_METHOD, TRANSLATE_METHOD_DEFAULT);
		if (nTranslateSwitch && g_pTranslateUtil != NULL)
		{
            XString strHost = __XT("");
            XString strCgi = __XT("");
            if (!nTranslateMethod)
            {
                strHost = GetConfigure<XString>(CONFIG_TRANSLATE_HOST, XString(TRANSLATE_HOST_DEFAULT));
                strCgi = GetConfigure<XString>(CONFIG_TRANSLATE_CGI, XString(TRANSLATE_CGI_PATH_DEFAULT));
                strCgi += __XT("t?");
            }
            else
            {
                strHost = GetConfigure<XString>(CONFIG_TRANSLATE_HOST_V2, XString(TRANSLATE_HOST_DEFAULT_V2));
                strCgi = GetConfigure<XString>(CONFIG_TRANSLATE_CGI_V2, XString(TRANSLATE_CGI_PATH_DEFAULT_V2));
                strCgi += __XT("v2?key=");
                XString strGoogleAPIKey = GetConfigure<XString>(CONFIG_TRANSLATE_GOOGLE_APIKEY, XString(TRANSLATE_GOOGLE_APIKEY_DEFAULT));
                strCgi += strGoogleAPIKey;
            }
			
			XString strRegularPattern = GetConfigure<XString>(CONFIG_TRANSLATE_REGULAR, XString(TRANSLATE_REGULAR_DEFAULT));
			g_pTranslateUtil->Init(strHost, strCgi, strRegularPattern);
		}

		m_localTime = youmecommon::CTimeUtil::GetTimeOfDay_MS();
		m_initStatus = IMManagerStatus_Init;

		if (IMManangerLoginStatus_Ing == m_loginStatus)
		{
			ConnectLoginServer();
		}
	}
	else if (youmecommon::VALIDATEREASON_RECONNECT == reason)
	{
		m_initStatus = IMManagerStatus_Init;
		++m_iReconnectLoginTimes;
		ConnectLoginServer();
	}

	UpdateServerConfig(youmecommon::VALIDATEREASON_INIT == reason);

	if (youmecommon::VALIDATEREASON_INIT == reason)
	{
		ReportValidate(errorcode, constTime, ip);
	}
}

void YouMeIMManager::OnConnect(NetEventErrorcode errorcode, IPPortItem& address, int connectType)
{
    if(errorcode != NETEVENT_CONNECT_SUCCESS)
    {
        YouMe_LOG_Error(__XT("OnConnect type:%d error:%d"), connectType, errorcode);
    }
    
	if (m_loginStatus == IMManangerLoginStatus_Logouting)
	{
		YouMe_LOG_Info(__XT("connect exit"));
		return;
	}

	if (NETEVENT_CONNECT_SUCCESS == errorcode)
	{
        YouMe_LOG_Info(__XT("OnConnect type:%d error:%d"), connectType, errorcode);
		if (m_loginAddrList.size() > 1)
		{
			std::vector<IPPortItem>::iterator itr = std::find(m_loginAddrList.begin(), m_loginAddrList.end(), address);
			if (itr != m_loginAddrList.begin() && itr != m_loginAddrList.end())
			{
				// 成功连接地址放在第一个，下次优先使用
				IPPortItem temp = *(m_loginAddrList.begin());
				*(m_loginAddrList.begin()) = address;
				*itr = temp;
			}
		}
		m_iCurentLoginAddrIndex = 0;
				
		RequestLogin();
	}
	else if (NETEVENT_CONNECT_FAILED == errorcode)
	{
        YouMe_LOG_Error(__XT("connect failed type:%d error:%d m_iCurentLoginAddrIndex:%d m_loginAddrList len:%d"), connectType, errorcode,m_iCurentLoginAddrIndex, m_loginAddrList.size());

		if (0 == connectType)		// 登录
		{
			if (m_iCurentLoginAddrIndex < m_loginAddrList.size())
			{
                XSleep(1000);// delay 5s
				std::thread t = std::thread(&YouMeIMManager::ConnectLoginServer, this);	// 新启线程，避免死锁
				t.detach();
			}
			else if (m_bConnectFaileRevalidate && g_YM_isPrivateServer==0)
			{
                XSleep(1000);// delay 5s
				m_bConnectFaileRevalidate = false;
                m_loginStatus = IMManangerLoginStatus_Ing;
				ValidateSDK(youmecommon::VALIDATEREASON_INIT);
			}
			else
			{
                m_loginStatus = IMManangerLoginStatus_Not;
				if (m_pLoginCallback != NULL)
				{
					m_pLoginCallback->OnLogin(YIMErrorcode_NetError, m_strCurUserID.c_str());
				}
			}
		}
		else if (2 == connectType)	// 重连
		{
			unsigned int reconnectCount = GetConfigure<unsigned int>(CONFIG_RECONNECT_COUNT, RECONNECT_COUNT);
			if (m_iCurentLoginAddrIndex < m_loginAddrList.size() || m_iReconnectLoginTimes < reconnectCount - 1)
			{
				if (m_iCurentLoginAddrIndex == m_loginAddrList.size())
				{
					m_iCurentLoginAddrIndex = 0;
					++m_iReconnectLoginTimes;
				}
                XSleep(1000);
                if (m_pReconnectCallback != NULL)
                {
                    m_pReconnectCallback->OnRecvReconnectResult(RECONNECTRESULT_FAIL_AGAIN); //重连失败,再次重连
                }
				ReconectProc(REASON_RELOGIN);
			}
			else if (m_bConnectFaileRevalidate)
			{
				m_bConnectFaileRevalidate = false;
				ReconectProc(REASON_REVALIDATE);
			}
			else
			{
				m_iReconnectLoginTimes = 0;
				m_loginStatus = IMManangerLoginStatus_Not;
                m_iCurentLoginAddrIndex = 0;
				m_strLastLoginedUserID = __XT("");
				
                if (m_pReconnectCallback != NULL)
                {
                    m_pReconnectCallback->OnRecvReconnectResult(RECONNECTRESULT_FAIL);
                }

				if (NULL != m_pLoginCallback)
				{
					m_pLoginCallback->OnLogout(YIMErrorcode_Disconnect);
				}
			}
		}
	}
}

void YouMeIMManager::OnError(NetEventErrorcode errorcode)
{
    IMManangerLoginStatus lastStatus = m_loginStatus;
    YouMe_LOG_Error(__XT("network error(%d) lastStatus:%d ,m_loginStatus %d"), errorcode,lastStatus,m_loginStatus);
	if (lastStatus != IMManangerLoginStatus_Ing)
	{
		m_loginStatus = IMManangerLoginStatus_Not;
	}
	if (IMManangerLoginStatus_Success == lastStatus || IMManangerLoginStatus_Ing == lastStatus)
	{
		ReconectProc(REASON_NETWORK_ERROR);
	}
}

YIMErrorcode YouMeIMManager::RequestLogin()
{
	YOUMEServiceProtocol::LoginReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_device_token(XStringToUTF8(g_pSystemProvider->getUUID()));
	req.set_pswd(XStringToUTF8(m_strPasswd));
	if (!m_strToken.empty())
	{
		req.set_auth_token(XStringToUTF8(m_strToken));
	}
	YOUMECommonProtocol::DevicePlatform plateform = YOUMECommonProtocol::Platform_Unknow;
#ifdef WIN32
	plateform = YOUMECommonProtocol::Platform_Windows;
#elif OS_IOS
	plateform = YOUMECommonProtocol::Platform_IOS;
#elif OS_IOSSIMULATOR
	plateform = YOUMECommonProtocol::Platform_IOS;
#elif OS_OSX
    plateform = YOUMECommonProtocol::Platform_OSX;
#elif OS_ANDROID
	plateform = YOUMECommonProtocol::Platform_Android;
#endif
	req.set_os_type(plateform);
	std::string strData;
	req.SerializeToString(&strData);

	m_llRequestLoginTime = youmecommon::CTimeUtil::GetTimeOfDay_MS();
	XUINT64 msgSerail = 0;
	std::map<std::string, std::string> extend;
	if (!SendDataRandKey(YOUMEServiceProtocol::CMD_LOGIN, strData.c_str(), (int)strData.length(), extend, msgSerail))
	{
		YouMe_LOG_Error(__XT("send login request failed"));
		if (m_pLoginCallback != NULL)
		{
			m_pLoginCallback->OnLogin(YIMErrorcode_NetError, m_strCurUserID.c_str());
		}
	}

	return YIMErrorcode_Success;
}

void YouMeIMManager::OnLoginRsp(ServerPacket& serverPacket)
{
	if (m_pLoginCallback == NULL)
	{
		YouMe_LOG_Error(__XT("login callback is null"));
		return;
	}

	XINT64 llLoginFinishTime = youmecommon::CTimeUtil::GetTimeOfDay_MS();
	int nReportErrorCode = REPORTCODE_SUCCESS;
	YIMErrorcode errorCode = YIMErrorcode_Success;
	youmecommon::CXSharedArray<unsigned char> secretKey;
	do 
	{
		if (-1 == serverPacket.result)
		{
			errorCode = YIMErrorcode_TimeOut;
			nReportErrorCode = REPORTCODE_TIMEOUT;
			YouMe_LOG_Error(__XT("login timeout"));
			break;
		}

		YOUMEServiceProtocol::LoginRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			errorCode = YIMErrorcode_ServerError;
			nReportErrorCode = REPORTCODE_UNPACK_EEROR;
			YouMe_LOG_Error(__XT("login unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
			break;
		}
		
		int nResult = rsp.ret();
		if (nResult != 0)
		{
			nReportErrorCode = nResult;
			errorCode = YIMErrorcode_ServerError;
			if (YOUMEServiceProtocol::EC_PSWD_ERR == nResult)
			{
				errorCode = YIMErrorcode_UsernamePasswordError;
			}
			else if (YOUMEServiceProtocol::EC_INVLIAD_USER == nResult)
			{
				errorCode = YIMErrorcode_UserStatusError;
			}
			else if (YOUMEServiceProtocol::EC_AUTH_ERR == nResult)
			{
				errorCode = YIMErrorcode_LoginTokenInvalid;
			}
			YouMe_LOG_Error(__XT("login error(%d)"), nResult);
			break;
		}
		if (ParseRSASecret(rsp.secret(), secretKey) != 0)
		{
			errorCode = YIMErrorcode_UnknowError;
			nReportErrorCode = REPORTCODE_PARSE_LOGIN_SECURET_EEROR;
			YouMe_LOG_Error(__XT("login parse secret failed"));
		}
	} while (0);

	bool isReconnect = !m_strLastLoginedUserID.empty();
	if (YIMErrorcode_Success != errorCode)
	{
		m_loginStatus = IMManangerLoginStatus_Not;
		m_strLastLoginedUserID = __XT("");

		if (isReconnect)
		{
			if (m_pReconnectCallback != NULL)
			{
				m_pReconnectCallback->OnRecvReconnectResult(RECONNECTRESULT_FAIL);
			}
			m_pLoginCallback->OnLogout(YIMErrorcode_Success);
		}
		else
		{
			m_pLoginCallback->OnLogin(errorCode, m_strCurUserID.c_str());
		}
		
		return;
	}
	m_strLastLoginedUserID = m_strCurUserID;

	YouMe_LOG_Info(__XT("login success ID:%s"), m_strCurUserID.c_str());
	
	m_pNetworkServivce->SetSessionInfo(m_strCurUserID, m_iAppID, secretKey);
	m_loginStatus = IMManangerLoginStatus_Success;
	m_iReconnectLoginTimes = 0;
	m_bConnectFaileRevalidate = true;

	if (GetMessageManager() != NULL)
	{
		m_pMessageManager->InitMessageManager();
	}
	
	//重新joinroom或登录前join的房间
	YouMeIMGroupManager* pGroupManager = (YouMeIMGroupManager*)GetChatRoomManager();
	pGroupManager->ReJoinChatRoom(isReconnect);

	if (!isReconnect) //lastLogStatus != IMManangerLoginStatus_Reconnecting/	//第一次登录过程中socket recv失败，触发重连，如果依赖m_loginStatus会没有login回调
	{
		InitNoticManager();
		m_pLoginCallback->OnLogin(errorCode, m_strCurUserID.c_str());
	}
		
	//启动心跳线程	
	m_nHeartbeatLossCount = 0;
	m_heartBeatWait.SetSignal();
	{
		std::lock_guard<std::mutex> lock(m_heartBeatThreadMutex);
		if (m_heartBeatThread.joinable())
		{
			m_heartBeatThread.join();
		}
		m_heartBeatWait.Reset();
		m_heartBeatThread = std::thread(&YouMeIMManager::HeartBeatThreadProc, this);
	}

	if (m_pMessageManager != NULL && m_pUpdateReadStatusCallback != NULL && !m_pMessageManager->m_pauseReceivingFlag)
	{
		m_pMessageManager->SynMsgReadStatus();
	}

	if (GetUserProfileManager())
	{
		m_pUserProfileManager->StartUserStatusHeartbeat();
	}

    //程序切到后台且暂停接收IM消息时不拉取消息
	if (m_pMessageManager != NULL && !m_pMessageManager->m_pauseReceivingFlag)
	{
		m_pMessageManager->GetNewMessage(false);
	}

	if (!isReconnect)
	{
		ReportDataLogin(nReportErrorCode, m_llRequestLoginTime != 0 ? static_cast<unsigned int>(llLoginFinishTime - m_llRequestLoginTime) : 0);
		m_llRequestLoginTime = 0;
		m_llLoginedTime = GetServerValidTime();
		if (GetFriendManager() != NULL)
		{
			m_pFriendManager->CheckHasAddFriendRequest();
		}
	}
	else
	{
		if (m_pReconnectCallback != NULL)
		{
			m_pReconnectCallback->OnRecvReconnectResult(RECONNECTRESULT_SUCCESS);
		}

		SendCacheData();
		ReportDataReconnect(nReportErrorCode, m_nReconnectReason);
	}

	if (g_pSpeechManager != NULL)
	{
		g_pSpeechManager->ReqAliAccessToken();
	}
}

void YouMeIMManager::OnLogoutRsp(ServerPacket& serverPacket)
{
	YouMe_LOG_Info(__XT("logout finish"));

	if (-1 == serverPacket.result)
	{
		YouMe_LOG_Error(__XT("logout timeout"));
	}
	else
	{
		YOUMEServiceProtocol::LogoutRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("logout unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
		}
		else
		{
			if (rsp.ret() != 0)
			{
				YouMe_LOG_Error(__XT("logout error(%d)"), rsp.ret());
			}
		}
	}

	m_strCurUserID = __XT("");
	std::thread cleanup(&YouMeIMManager::DealLogoutCleanup, this, true);
	cleanup.detach();
}

void YouMeIMManager::OnKickOffRsp(ServerPacket& serverPacket)
{
	YouMe_LOG_Info(__XT("user kick off"));

	YOUMEServiceProtocol::KickOffRsp rsp;
	if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
	{
		YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
		return;
	}

	m_heartBeatWait.SetSignal();
	m_strLastLoginedUserID = __XT("");

	std::thread cleanupThread(&YouMeIMManager::DealLogoutCleanup, this, false);
	cleanupThread.detach();

	if (m_pLoginCallback != NULL)
	{
		m_pLoginCallback->OnKickOff();
	}

	ReportCommon reportData;
	reportData.type = REPORTTYPE_KICKOFF;
	reportData.userid = XStringToUTF8(m_strCurUserID);
	reportData.appid = m_iAppID;
	reportData.SDKVersion = SDK_VER;
	reportData.reverse1 = 0;
	reportData.reverse2 = 0;
	reportData.createTime = static_cast<unsigned int>(GetServerValidTime() / 1000);

	m_report.Report(reportData);
}

void YouMeIMManager::OnReloginNotify(ServerPacket& serverPacket)
{
	YouMe_LOG_Info(__XT("relogin notify"));

	//重登通知服务端未新增协议，共用KickOffRsp
	YOUMEServiceProtocol::KickOffRsp rsp;
	if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
	{
		YouMe_LOG_Error(__XT("relogin unpack error serial:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
		return;
	}
    
	m_reconnectWait.SetSignal();
	m_heartBeatWait.SetSignal();
	{
		std::lock_guard<std::mutex> lock(m_reconnectThreadMutex);
		if (m_reconnectThread.joinable())
		{
			m_reconnectThread.join();
		}
	}
	{
		std::lock_guard<std::mutex> lock(m_heartBeatThreadMutex);
		if (m_heartBeatThread.joinable())
		{
			m_heartBeatThread.join();
		}
	}
	/*if (m_loginStatus != IMManangerLoginStatus_Success && m_loginStatus != IMManangerLoginStatus_Reconnecting)
	{
		YouMe_LOG_Info(__XT("Exit m_loginStatus:%d"), m_loginStatus);
		return;
	}*/
	m_loginStatus = IMManangerLoginStatus_Reconnecting;

	RequestLogin();
}

void YouMeIMManager::OnUpdateConfigNotify(ServerPacket& serverPacket)
{
	youmecommon::SDKValidateErrorcode errorCode = ValidateSDK(youmecommon::VALIDATEREASON_UPDATE_CONFIG);
	if (errorCode == youmecommon::SDKValidateErrorcode_Success)
	{
		UpdateServerConfig(false);
	}
}

void YouMeIMManager::UpdateServerConfig(bool isInit)
{
	YouMe_LOG_SetLevel(static_cast<YouMe_LOG_LEVEL>(GetConfigure<unsigned int>(CONFIG_LOG_LEVEL, LOG_LEVEL_INFO)),static_cast<YouMe_LOG_LEVEL>(GetConfigure<unsigned int>(CONFIG_LOG_CONSOLE_LEVEL, LOG_LEVEL_INFO)));
	m_iHeartbeatInterval = GetConfigure<unsigned int>(CONFIG_HEART_TIMEOUT, HEARTBEAT_INTERVAL) * 1000;
	m_iHeartbeatTimeoutCount = static_cast<unsigned short>(GetConfigure<unsigned int>(CONFIG_HEART_TIMOUT_COUNT, HEARTBEAT_TIMEOUT_COUNT));
	
	int nKeepRecordModel = GetConfigure<unsigned int>(CONFIG_KEEP_RECORD_MODEL, 0);
    SetKeepRecordModel(nKeepRecordModel == 0 ? false : true );

	if (m_pNetworkServivce != NULL)
	{
		m_pNetworkServivce->SetRetrySendTime(GetConfigure<int>(CONFIG_RETRY_SEND_TIMES, RETRY_SEND_TIMES));
	}
	if (m_pMessageManager != NULL)
	{
		m_pMessageManager->UpdateConfig();
	}

	if (isInit)
	{
		XString strURL = GetConfigure(CONFIG_KEYWORD_URL, XString(__XT("")));
		if (strURL != __XT(""))
		{
			XString strKeywordMD5 = GetConfigure(CONFIG_KEYWORD_MD5, XString(__XT("")));
			std::thread keywordHttpThread(&YouMeIMManager::DownloadKeyword, this, strURL, strKeywordMD5);
			keywordHttpThread.detach();
		}
	}
}

YIMErrorcode YouMeIMManager::Init(const XCHAR* appKey, const XCHAR* appSecurity, const XCHAR* packageName)
{
	YouMe_LOG_Info(__XT("Enter"));
   	// SetMode(2);
	// SetLoginAddress("192.168.154.48", 8080);
	// SetAppId(2964);
	if (NULL == appKey || appSecurity == NULL || XStrLen(appKey) == 0 || XStrLen(appSecurity) == 0)
	{
		YouMe_LOG_Warning(__XT("param error appKey:%p appSecurity:%p"), appKey, appSecurity);
		return YIMErrorcode_ParamInvalid;
	}

	if (!s_bYouMeStartup)
	{
		YouMe_LOG_Error(__XT("has not call Startup"));
		return YIMErrorcode_NotStartUp;
	}
	if (NULL == g_pProfileDB)
	{
		XString strDBPath = youmecommon::CXFile::CombinePath(g_pSystemProvider->getDocumentPath(), __XT("youmeimprofile.db"));
		g_pProfileDB = new CProfileDB(strDBPath);
	}
    
    if(youmecommon::CXFile::IsFileExist(youmecommon::CXFile::CombinePath(g_pSystemProvider->getCachePath(), __XT("youme_im_message.db"))))
    {
        //旧版本本地历史记录迁移
        XString strSrcPath = youmecommon::CXFile::CombinePath(g_pSystemProvider->getCachePath(), __XT("youme_im_message.db"));
        XString strDestPath = youmecommon::CXFile::CombinePath(g_pSystemProvider->getDocumentPath(), __XT("youme_im_message.db"));
        if(youmecommon::CXFile::rename_file(strSrcPath, strDestPath)){
            YouMe_LOG_Info(__XT("move old history to document dir success: %s %s"), strSrcPath.c_str(), strDestPath.c_str());
        }else{
            YouMe_LOG_Error(__XT("move old history to document dir failed: %s %s"), strSrcPath.c_str(), strDestPath.c_str());
        }
    }else{
        YouMe_LOG_Info(__XT("have no old history db: %s"),g_pSystemProvider->getCachePath().c_str());
    }
	
#ifdef WIN32
	g_pSystemProvider->setPackageName(__XT("com.youme.windows"));
#endif

	m_bIsCleanup = false;
	g_pSystemProvider->setAppKey(appKey);
	g_pSystemProvider->setAppSecret(appSecurity);
	g_pSystemProvider->setSDKVersion(SDK_VER);
#ifdef IFLY_RECOGNIZE
	int ifly = 1;
#else
	int ifly = 0;
#endif
#ifdef USC_RECOGNIZE
		int usc = 1;
#else
		int usc = 0;
#endif
#ifdef ALI_RECOGNIZE
		int ali = 1;
#else
		int ali = 0;
#endif

	YouMe_LOG_Info(__XT("------------------------------------Start---------------------------------------------"));
	YouMe_LOG_Info(__XT("\nappkey:%s\npackage:%s\nbrand:%s\nmodel:%s\niosver:%s\nsdkver:%d\ncpuarch:%s\ncpuchip:%s\nidentify:%s\nserverzone:%d\nmode:%d\nifly:%d\nusc:%d\nali:%d\n"), appKey,
		g_pSystemProvider->getPackageName().c_str(),
		g_pSystemProvider->getBrand().c_str(),
		g_pSystemProvider->getModel().c_str(),
		g_pSystemProvider->getSystemVersion().c_str(),
		g_pSystemProvider->getSDKVersion(),
		g_pSystemProvider->getCpuArchive().c_str(),
		g_pSystemProvider->getCpuChip().c_str(),
		g_pSystemProvider->getUUID().c_str(),
		g_ServerZone,
		g_iYouMeIMMode,
		ifly,
		usc,
		ali
);

	//初始化网络监听
	youmecommon::CNetworkService::Instance()->registerCallback(this);
	youmecommon::CNetworkService::Instance()->start();
    
    {
        std::lock_guard<std::mutex> lock(m_reconnectCheckThreadMutex);
        if (m_reconnectCheckThread.joinable())
        {
            m_reconnectCheckSemWait.Increment();
            m_bConnectCheckExit = true;
            
            m_reconnectCheckThread.join();
            m_bConnectCheckExit = false;
        }
        m_reconnectCheckThread = std::thread(&YouMeIMManager::ReconnectCheckThread, this);
    }

#ifdef OS_ANDROID
	m_nNetworkType = GetNetworkType();
	YouMe_LOG_Debug(__XT("NetworkType:%d"), m_nNetworkType);
#endif // OS_ANDROID
    if(g_YM_isPrivateServer == 0){
        youmecommon::SDKValidateErrorcode errorCode = ValidateSDK(youmecommon::VALIDATEREASON_INIT);
        if (errorCode == youmecommon::SDKValidateErrorcode_Success)
        {
            m_initStatus = IMManagerStatus_Ing;
        }
        else
        {
            m_initStatus = IMManagerStatus_initfailed;
            m_loginStatus = IMManangerLoginStatus_Not;
            YouMe_LOG_Error(__XT("SDK validate error(%d)"), errorCode);
            return YIMErrorcode_EngineNotInit;
        }
    }else{
        m_initStatus = IMManagerStatus_Init;
        m_iAppID = g_iAppID;
        m_configurations.insert(std::map<std::string, youmecommon::CXAny>::value_type(APP_SERVICE_ID, m_iAppID));
    }

	YouMe_LOG_Info(__XT("Leave"));
	return YIMErrorcode_Success;
}

void YouMeIMManager::UnInit()
{
	YouMe_LOG_Info(__XT("Enter initstatus:%d loginstatus:%d"),m_initStatus, m_loginStatus);
	if (m_initStatus == IMManagerStatus_NotInit || m_initStatus == IMManagerStatus_uniniting)
	{
		return;
	}
	m_initStatus = IMManagerStatus_uniniting;

	if (m_loginStatus == IMManangerLoginStatus_Success)
	{
		Logout();
		YouMe_LOG_Info(__XT("after logout"));
	}
	if (m_loginStatus == IMManangerLoginStatus_Logouting)
	{
		m_logoutWait.WaitTime(3000);
	}

	youmecommon::DNSUtil::Instance()->Cancel();

	m_reconnectWait.SetSignal();
	{
		std::lock_guard<std::mutex> lock(m_reconnectThreadMutex);
		if (m_reconnectThread.joinable())
		{
			m_reconnectThread.join();
		}
	}
	
	m_heartBeatWait.SetSignal();
	{
		std::lock_guard<std::mutex> lock(m_heartBeatThreadMutex);
		if (m_heartBeatThread.joinable())
		{
			m_heartBeatThread.join();
		}
	}

	if (m_logoutThread.joinable())
	{
		m_logoutThread.join();
	}

	m_initStatus = IMManagerStatus_NotInit;
	youmecommon::CNetworkService::Instance()->cancleCallback(this);

	YouMe_LOG_Info(__XT("Leave"));
}


YIMErrorcode YouMeIMManager::SetLoginAddress(const char* ip, unsigned short port)
{
	if (NULL == ip || strlen(ip) == 0)
	{
		return YIMErrorcode_ParamInvalid;
	}

	XString strIP = UTF8TOXString(std::string(ip));
	YouMe_LOG_Warning(__XT("set address ip:%s port:%d"), strIP.c_str(), port);

       std::regex pattern("^192\\.168\\.\\d{1,3}\\.\\d{1,3}$");
       std::regex pattern2("^172\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}$");
       std::regex pattern3("^10\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}$");
       if ( (!std::regex_match(ip, pattern))
           && (!std::regex_match(ip, pattern2))
           && (!std::regex_match(ip, pattern3))
           )
       {
           YouMe_LOG_Error(__XT("set address ip YIMErrorcode_ParamInvalid"));
           return YIMErrorcode_ParamInvalid;
       }

	m_privateServerLoginAddress.addr = ip;
	m_privateServerLoginAddress.port = port;

	if (g_pYouMeIMManager != NULL)
	{
		g_pYouMeIMManager->m_loginAddrList.clear();
		g_pYouMeIMManager->m_loginAddrList.push_back(m_privateServerLoginAddress);
	}
    g_YM_isPrivateServer = 1;
	return YIMErrorcode_Success;
}

YIMChatRoomManager* YouMeIMManager::GetChatRoomManager()
{
	if (NULL == m_pGroupManager)
	{
		std::lock_guard<std::mutex> lock(m_managerMutex);
		if (NULL == m_pGroupManager)
		{
			m_pGroupManager = new YouMeIMGroupManager(this);
			m_pGroupManager->Init();
            m_pGroupManager->SetCallback(m_pGroupCallback);
		}
	}
	return m_pGroupManager;
}

YIMMessageManager* YouMeIMManager::GetMessageManager()
{
	if (NULL == m_pMessageManager)
	{
		std::lock_guard<std::mutex> lock(m_managerMutex);
		if (NULL == m_pMessageManager)
		{
			m_pMessageManager = new YouMeIMMessageManager(this);
			m_pMessageManager->Init();
            m_pMessageManager->SetMessageCallback(m_pMessageCallback);
			m_pMessageManager->SetDownloadCallback(m_pDownloadCallback);
			m_pMessageManager->SetUpdateReadStatusCallback(m_pUpdateReadStatusCallback);
			if (g_pSpeechManager != NULL)
			{
				g_pSpeechManager->SetMessageManager(m_pMessageManager);
			}
		}
	}
	return m_pMessageManager;
}

YIMLocationManager* YouMeIMManager::GetLocationManager()
{
	if (NULL == m_pLocationManager)
	{
		std::lock_guard<std::mutex> lock(m_managerMutex);
		if (NULL == m_pLocationManager)
		{
			m_pLocationManager = new YouMeLocationManager(this);
			m_pLocationManager->SetCallback(m_pLocationCallback);
		}
	}
	return m_pLocationManager;
}


YIMUserProfileManager* YouMeIMManager::GetUserProfileManager()
{
    if (NULL == m_pUserProfileManager)
    {
        std::lock_guard<std::mutex> lock(m_managerMutex);
        if (NULL == m_pUserProfileManager)
        {
            m_pUserProfileManager = new YouMeIMUserInfoManager(this);
            m_pUserProfileManager->SetCallback(m_pUserProfileCallback);
        }
    }
    
    return m_pUserProfileManager;
}

void YouMeIMManager::SetUserProfileCallback(IYIMUserProfileCallback *pCallback)
{
    YouMe_LOG_Info(__XT("Enter"));
    m_pUserProfileCallback = pCallback;
    if (NULL != m_pUserProfileManager)
    {
        m_pUserProfileManager->SetCallback(pCallback);
    }
}


YIMFriendManager* YouMeIMManager::GetFriendManager()
{
	if (NULL == m_pFriendManager)
	{
		std::lock_guard<std::mutex> lock(m_managerMutex);
		if (NULL == m_pFriendManager)
		{
			m_pFriendManager = new YouMeFriendManager(this);
			m_pFriendManager->SetCallback(m_pFriendCallback);
		}
	}
	return m_pFriendManager;
}

void YouMeIMManager::SetFriendCallback(IYIMFriendCallback* pCallback)
{
	m_pFriendCallback = pCallback;
	if (m_pFriendManager != NULL)
	{
		m_pFriendManager->SetCallback(pCallback);
	}
}
youmecommon::SDKValidateErrorcode YouMeIMManager::ValidateSDK(youmecommon::ValidateReason reason)
{
	youmecommon::SDKValidateParam validateParam;
	validateParam.serviceID = SERVICE_ID;
	validateParam.serviceType = YOUMEServiceProtocol::SERVICE_IM;
	validateParam.sdkName = SDK_NAME;
	validateParam.protocolVersion = PROTOCOL_VERSION;
	validateParam.port.push_back(SDK_INVALIDATE_PORT);
	validateParam.port.push_back(SDK_INVALIDATE_PORT2);
	validateParam.port.push_back(SDK_INVALIDATE_PORT3);
	
	if (g_iYouMeIMMode == ENVIROMENT_FORMAL)
	{
		if (m_sdkDomainMap.find(g_ServerZone) == m_sdkDomainMap.end())
		{
			YouMe_LOG_Error(__XT("server zone incorrect %d"), g_ServerZone);
			return youmecommon::SDKValidateErrorcode_Fail;
		}
		XString strAppKey = g_pSystemProvider->getAppKey();
		if (strAppKey.length() < 8)
		{
			YouMe_LOG_Error(__XT("APPKey is invalid"));
			return youmecommon::SDKValidateErrorcode_Fail;
		}
		XString strDomain = strAppKey.substr(strAppKey.length() - 8);
		strDomain += __XT(".imcfg.youme.im");
		validateParam.domain = XStringToUTF8(strDomain);

		if (g_ServerZone == ServerZone_China || g_ServerZone == ServerZone_HongKong || g_ServerZone == ServerZone_Unknow)
		{
			validateParam.defaultIP.push_back(SDK_INVALIDATE_DEFAULT_IP);
			validateParam.defaultIP.push_back(SDK_INVALIDATE_DEFAULT_IP_2);
		}
		else
		{
			validateParam.defaultIP.push_back(SDK_INVALIDATE_DEFAULT_IP_2);
			validateParam.defaultIP.push_back(SDK_INVALIDATE_DEFAULT_IP);
		}
	}
	else if (g_iYouMeIMMode == ENVIROMENT_DEVELOP)
	{
		validateParam.domain = SDK_AUTH_URL_DEV;
	}
	else if (g_iYouMeIMMode == ENVIROMENT_TEST)
	{
		validateParam.domain = SDK_AUTH_URL_TEST;
	}
	else if (g_iYouMeIMMode == ENVIROMENT_BUSSINESS)
	{
		validateParam.domain = SDK_AUTH_URL_BUSSINESS;
	}
	YouMe_LOG_Info(__XT("SDK validate domain:%s"), UTF8TOXString(validateParam.domain).c_str());
	
	validateParam.serverZone = "cn";
	if (m_sdkDomainMap.find(g_ServerZone) != m_sdkDomainMap.end())
	{
		validateParam.serverZone = m_sdkDomainMap[g_ServerZone];
		if (validateParam.serverZone == "auto")
		{
			validateParam.serverZone = "cn";
		}
	}

	if (m_pSDKValidate == NULL)
	{
		m_pSDKValidate = new youmecommon::AccessValidate(g_pSystemProvider, g_pProfileDB, this);
	}
	return m_pSDKValidate->StartValidate(validateParam, reason);
}

void YouMeIMManager::HeartBeatThreadProc()
{
	YouMe_LOG_Info(__XT("heartbeat thread enter"));
	m_nHeartbeatLossCount = 0;
	while (true)
	{
		if (youmecommon::WaitResult_Timeout != m_heartBeatWait.WaitTime(m_iHeartbeatInterval))
		{
			YouMe_LOG_Info(__XT("received upper level notityfy,heartbeat thread exit"));
			break;
		}
		if (m_nHeartbeatLossCount >= m_iHeartbeatTimeoutCount)
		{
			YouMe_LOG_Warning(__XT("heartbeat packet not received, need reconnect"));
			ReconectProc(REASON_LOSS_HEARTBEAT);
			break;
		}
		YOUMEServiceProtocol::HeartbeatReq req;
		req.set_version(PROTOCOL_VERSION);
		req.set_tips_seq(GetMaxAccusationID());
		if (m_pMessageManager != NULL)
		{
			req.set_weixin_token_seq(m_pMessageManager->GetWeiXinTokenSerial());
		}
		if (g_pSpeechManager != NULL)
		{
			req.set_ali_token_seq(g_pSpeechManager->GetAliTokenSerial());
		}
		std::string strData;
		req.SerializeToString(&strData);
		XUINT64 msgSerail = 0;
		std::map<std::string, std::string> extend;
		SendData(YOUMEServiceProtocol::CMD_HEARTBEAT, strData.c_str(), (int)strData.length(), extend, msgSerail);
		++m_nHeartbeatLossCount;
	}
	YouMe_LOG_Info(__XT("heartbeat thread exit"));
}

void YouMeIMManager::OnHeartBeatRsp(ServerPacket& serverPacket)
{
	if (-1 == serverPacket.result)
	{
		YouMe_LOG_Error(__XT("heartbeat timeout %d"), m_nHeartbeatLossCount);
		return;
	}
	YOUMEServiceProtocol::HeartbeatRsp rsp;
	if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
	{
		YouMe_LOG_Error(__XT("heartbeat unpack error serial:%llu packetSize:%u"), serverPacket.reqSerial, serverPacket.packetSize);
		return;
	}
	if (rsp.ret() == 0)
	{
		m_nHeartbeatLossCount = 0;
		
		if (m_nInitiativeMessageRound != -1)
		{
			if (++m_nMessageHeartBeatCount == initiativeGetMessageInterval[m_nInitiativeMessageRound])
			{
                //不满足程序切到后台且暂停接收IM消息时不拉取消息
				if (m_pMessageManager != NULL && !m_pMessageManager->m_pauseReceivingFlag)
				{
					m_pMessageManager->GetNewMessage(true);
				}
				if (m_nInitiativeMessageRound + 1 < sizeof(initiativeGetMessageInterval) / sizeof(short))
				{
					++m_nInitiativeMessageRound;
				}
				m_nMessageHeartBeatCount = 0;
			}
		}
		else
		{
			XINT64 llCurrentTime = youmecommon::CTimeUtil::GetTimeOfDay_MS();
            //不满足程序切到后台且暂停接收IM消息时不拉取消息
			if (llCurrentTime - m_pMessageManager->LastGetMessageTime() > INITIATIVE_GET_MESSAGE_INTERVAL && m_pMessageManager != NULL && !m_pMessageManager->m_pauseReceivingFlag)
			{
				m_nMessageHeartBeatCount = 0;
				m_nInitiativeMessageRound = 0;                    
				m_pMessageManager->GetNewMessage(true);
			}
		}
		

		// 举报处理结果通知
		if (rsp.sync_flag() == 1 && m_pMessageManager != NULL)
		{
			m_pMessageManager->OnAccusationDealNotify();
		}
		//更新微信AccessToken
		if (g_YM_isPrivateServer == 0 && rsp.weixin_token_sync_flag() == 1 && m_pMessageManager != NULL)
		{
			m_pMessageManager->RequestWeiXinAccessToken();
		}
		if (rsp.ali_token_sync_flag() == 1 && g_pSpeechManager != NULL)
		{
			g_pSpeechManager->ReqAliAccessToken();
		}
	}

	if (m_pLocationManager != NULL)
	{
		m_pLocationManager->UpdateLocation();
	}
}

void YouMeIMManager::ReconectProc(ReconnectReason reconnectReason)
{
	YouMe_LOG_Info(__XT("Enter reason:%d loginStatus:%d"), reconnectReason, m_loginStatus);

	if ((m_loginStatus == IMManangerLoginStatus_Reconnecting && reconnectReason != REASON_NETWORK_ERROR && reconnectReason != REASON_REVALIDATE && reconnectReason != REASON_RELOGIN)	// 重连过程中，socket recv失败触发的reconnect应走下去
		|| (m_loginStatus == IMManangerLoginStatus_Ing && reconnectReason != REASON_NETWORK_ERROR)) // 登录过程中出现网络错误
	{
		YouMe_LOG_Info(__XT("Leave reason:%d loginStatus:%d"), reconnectReason, m_loginStatus);
		return;
	}
	if (!m_bApplicationForeground || m_strCurUserID.empty())
	{
		m_loginStatus = IMManangerLoginStatus_Not;	// 程序在后台运行触发reconnect，将状态改为IMManangerLoginStatus_Not，等下次onresume时执行reconnect
		if (IMManagerStatus_Ing == m_initStatus)
		{
			m_initStatus = IMManagerStatus_initfailed;
		}
		YouMe_LOG_Info(__XT("Leave background"));
		return;
	}
	if (m_loginStatus == IMManangerLoginStatus_Ing && reconnectReason == REASON_NETWORK_ERROR)
	{
		reconnectReason = REASON_RELOGIN;
	}

	if (m_bReconnecThreadRunning)
	{
		YouMe_LOG_Info(__XT("Leave reconnecting"));
		return;
	}

	m_loginStatus = IMManangerLoginStatus_Reconnecting;
	m_nReconnectReason = reconnectReason;
	m_heartBeatWait.SetSignal();

    m_reconnectCheckSemWait.Increment();

	YouMe_LOG_Info(__XT("Leave"));
}

void YouMeIMManager::ReconnectCheckThread()
{
    YouMe_LOG_Info(__XT("Enter"));
    while (true)
    {
        m_reconnectCheckSemWait.Decrement();
        if (m_bConnectCheckExit)
        {
            break;
        }
        
        {
            std::lock_guard<std::mutex> lock(m_reconnectThreadMutex);
            if (m_reconnectThread.joinable())
            {
                m_reconnectThread.join();
            }
            if (IMManangerLoginStatus_Success == m_loginStatus)
            {
                YouMe_LOG_Info(__XT("login success exit"));
                return;
            }
            m_reconnectWait.Reset();
            m_reconnectThread = std::thread(&YouMeIMManager::ReconnectThread, this, m_nReconnectReason);
        }
    };
    
    YouMe_LOG_Info(__XT("Leave"));
}

void YouMeIMManager::ReconnectThread(ReconnectReason reconnectReason)
{
	YouMe_LOG_Info(__XT("Enter"));

	m_bReconnecThreadRunning = true;

	if (m_iReconnectLoginTimes != 0)
	{
		if (youmecommon::WaitResult_Timeout != m_reconnectWait.WaitTime(RECONNECT_TIMEOUT))
		{
			if (m_initStatus != IMManagerStatus_Init)
			{
				m_initStatus = IMManagerStatus_initfailed;
			}
			m_bReconnecThreadRunning = false;
			YouMe_LOG_Info(__XT("received upper level notityfy reconnect exit"));
			return;
		}
		if (IMManangerLoginStatus_Success == m_loginStatus)
		{
			m_bReconnecThreadRunning = false;
			YouMe_LOG_Info(__XT("login success exit"));
			return;
		}
	}
	YouMe_LOG_Info(__XT("reconnect login:%d index:%d"), m_iReconnectLoginTimes, m_iCurentLoginAddrIndex);

	if (m_pNetworkServivce != NULL)
	{
		m_pNetworkServivce->DisconnectAndWait();
        
	}

	if (m_pGroupManager != NULL)
	{
		m_pGroupManager->ResetRoomStatus();
	}

	if (reconnectReason == REASON_REVALIDATE && g_YM_isPrivateServer == 0)
	{
		youmecommon::SDKValidateErrorcode errorCode = ValidateSDK(youmecommon::VALIDATEREASON_RECONNECT);
		if (errorCode == youmecommon::SDKValidateErrorcode_Success)
		{
			m_initStatus = IMManagerStatus_Ing;
		}
		else
		{
			m_initStatus = IMManagerStatus_initfailed;
			m_loginStatus = IMManangerLoginStatus_Not;
			YouMe_LOG_Error(__XT("reconnect SDK validate error(%d)"), errorCode);
		}
	}
	else
	{
		if (0 == m_iReconnectLoginTimes && 0 == m_iCurentLoginAddrIndex && m_pReconnectCallback != NULL)
		{
			m_pReconnectCallback->OnStartReconnect();
		}

		if (m_pNetworkServivce != NULL && m_iCurentLoginAddrIndex < m_loginAddrList.size())
		{
			m_pNetworkServivce->Connect(m_loginAddrList[m_iCurentLoginAddrIndex++], 2);
		}
	}

	m_bReconnecThreadRunning = false;

	YouMe_LOG_Info(__XT("Leave"));
}

void YouMeIMManager::DealLogoutCleanup(bool isLogout)
{
    YouMe_LOG_Info(__XT("Enter"));
	
	m_loginStatus = IMManangerLoginStatus_Logouting;
	m_strLastLoginedUserID = __XT("");
	if (m_bIsCleanup)
	{
		YouMe_LOG_Warning(__XT("repeated  logoutcleanup "));
		return;
	}
	m_bIsCleanup = true;

	{
		std::lock_guard<std::mutex> lock(m_heartBeatThreadMutex);
		if (m_heartBeatThread.joinable())
		{
			m_heartBeatThread.join();
		}
	}

	YouMe_LOG_Info(__XT("sdkvalidate unit"));
	if (m_pSDKValidate != NULL)
    {     
        if (m_initStatus == IMManagerStatus_Init)
        {
            m_pSDKValidate->Disconnect();
        }
        delete m_pSDKValidate;
        m_pSDKValidate = NULL;
    }

	YouMe_LOG_Info(__XT("network service unit start"));
	if (m_pNetworkServivce != NULL)
	{
		m_pNetworkServivce->UnInit();
		delete m_pNetworkServivce;
		m_pNetworkServivce = NULL;
	}
	
//    m_pMessageCallback = NULL;
//    m_pGroupCallback = NULL;
	m_logoutWait.SetSignal();
    
    {
        std::lock_guard<std::mutex> lock(m_managerMutex);
        YouMe_LOG_Info(__XT("message unit start"));
        if (NULL != m_pMessageManager)
        {
            m_pMessageManager->UnInit();
            delete m_pMessageManager;
            m_pMessageManager = NULL;
        }
        YouMe_LOG_Info(__XT("message unit end"));
        if (NULL != m_pGroupManager)
        {
            m_pGroupManager->UnInit();
            delete m_pGroupManager;
            m_pGroupManager = NULL;
        }
        YouMe_LOG_Info(__XT("group unit end"));
        if (NULL != m_pNoticeManager)
        {
            m_pNoticeManager->Uninit();
            delete m_pNoticeManager;
            m_pNoticeManager = NULL;
        }
        YouMe_LOG_Info(__XT("notice unit end"));
        if (NULL != m_pUserProfileManager)
        {
            delete m_pUserProfileManager;
            m_pUserProfileManager = NULL;        
        }
		if (NULL != m_pFriendManager)
		{
			delete m_pFriendManager;
			m_pFriendManager = NULL;
		}
        YouMe_LOG_Info(__XT("user profile manager unit end"));
        
        
        m_loginStatus = IMManangerLoginStatus_Not;
    }
    
	if (IMManagerStatus_Ing == m_initStatus || IMManagerStatus_uniniting == m_initStatus)
	{
		m_initStatus = IMManagerStatus_initfailed;
	}

	if (NULL != m_pLoginCallback && isLogout)
	{
		m_pLoginCallback->OnLogout(YIMErrorcode_Success);
	}
	m_strCurUserID = __XT("");
    YouMe_LOG_Info(__XT("Leave"));
}

YouMeIMManager::~YouMeIMManager()
{
	if (m_pLocationManager != NULL)
	{
		delete m_pLocationManager;
		m_pLocationManager = NULL;
	}

	if (m_pSDKValidate != NULL)
	{
		m_pSDKValidate->Disconnect();
		delete m_pSDKValidate;
		m_pSDKValidate = NULL;
	}

	YouMe_LOG_Info(__XT("network service unit start"));
	if (m_pNetworkServivce != NULL)
	{
		m_pNetworkServivce->UnInit();
		delete m_pNetworkServivce;
		m_pNetworkServivce = NULL;
	}
    
    {
        std::lock_guard<std::mutex> lock(m_reconnectCheckThreadMutex);
        if (m_reconnectCheckThread.joinable())
        {
            m_reconnectCheckSemWait.Increment();
            m_bConnectCheckExit = true;

            m_reconnectCheckThread.join();
        }
    }

	//UnInit();
}

void YouMeIMManager::onNetWorkChanged(youmecommon::NetworkType type)
{
	YouMe_LOG_Info(__XT("Enter NetworkType:%d loginStatus:%d initStatus:%d"), type, m_loginStatus, m_initStatus);

	if (m_strCurUserID.empty())
	{
		return;
	}
	
	XINT64 llCurrentTime = youmecommon::CTimeUtil::GetTimeOfDay_MS();
	XINT64 oldTime = m_llLastNetworkChangeTime;
	if (m_nNetworkType == type && llCurrentTime - oldTime < 2000)
	{
		YouMe_LOG_Debug(__XT("network status changed interval too small"));
		return;
	}
    
	m_llLastNetworkChangeTime = llCurrentTime;
	m_nNetworkType = type;
	
	if (m_loginStatus != IMManangerLoginStatus_Reconnecting &&
        (type == youmecommon::NetworkType_Wifi || type == youmecommon::NetworkType_3G) &&
		!m_strLastLoginedUserID.empty())
	{
        if(IMManagerStatus_initfailed == m_initStatus)
        {
            ReconectProc(REASON_REVALIDATE);
        }
		else if(IMManagerStatus_Init == m_initStatus)
		{
            ReconectProc(REASON_NETWORK_CHANGE);
        }		
	}
}

int YouMeIMManager::ParseRSASecret(const std::string& secret, youmecommon::CXSharedArray<unsigned char>& secretKey)
{
	youmecommon::CRSAUtil rsa;
	youmecommon::CXSharedArray<char> pDecodeBuffer;
	if (!youmecommon::CCryptUtil::Base64Decoder(XStringToUTF8(g_pSystemProvider->getAppSecret()), pDecodeBuffer))
	{
		return -1;
	}
	if (131 != pDecodeBuffer.GetBufferLen())
	{
		return -1;
	}
	youmecommon::CXSharedArray<unsigned char> publicKey;	
	youmecommon::CXSharedArray<unsigned char> module;	//模数
	module.Allocate(128);	
	memcpy(module.Get(), pDecodeBuffer.Get(), 128);
	publicKey.Allocate(3);
	memcpy(publicKey.Get(), pDecodeBuffer.Get() + 128, 3);
	if (!rsa.SetPublicKey(publicKey, module))
	{
		YouMe_LOG_Error(__XT("login rsa uncode failed"));
		return -1;
	}
	if (!rsa.DecryptByPublicKey((const unsigned char *)secret.c_str(), (int)secret.length(), secretKey))
	{
		YouMe_LOG_Error(__XT("login decrypt failed"));
		return -1;
	}
	if (secretKey.GetBufferLen() <= 0)
	{
		YouMe_LOG_Error(__XT("login decrypt secret lenth error"));
		return -1;
	}

	return 0;
}

XUINT64 YouMeIMManager::GetServerValidTime()
{
	XUINT64 ulCurTime = youmecommon::CTimeUtil::GetTimeOfDay_MS();
	XUINT64 ulServerTime = GetConfigure(CONFIG_SERVER_TIME, (XUINT64)m_localTime);
	return ulCurTime + (ulServerTime - m_localTime);
}

XUINT64 YouMeIMManager::GetAutoIncrementID()
{
    std::lock_guard<std::mutex> lock(m_audoIncrementIdMutex);
    if(m_autoInCrementID == 0){
        m_autoInCrementID = GetServerValidTime() - 1540000000000 ;
    }
    return ++m_autoInCrementID;
}

void YouMeIMManager::SetInitiativeGetMessageCount()
{
	m_nInitiativeMessageRound = -1;
	m_nMessageHeartBeatCount = 0;
}

void YouMeIMManager::SetDownloadCallback(IYIMDownloadCallback* pCallback)
{
    YouMe_LOG_Info(__XT("Enter"));
	m_pDownloadCallback = pCallback;
	if (NULL != m_pMessageManager)
	{
		m_pMessageManager->SetDownloadCallback(m_pDownloadCallback);
	}
}

void YouMeIMManager::SetUpdateReadStatusCallback(IYIMUpdateReadStatusCallback* pCallback)
{
    YouMe_LOG_Info(__XT("Enter"));
	m_pUpdateReadStatusCallback = pCallback;
	if (NULL != m_pMessageManager)
	{
		m_pMessageManager->SetUpdateReadStatusCallback(m_pUpdateReadStatusCallback);
	}
}

void YouMeIMManager::SetContactCallback(IYIMContactCallback* pCallback)
{
    YouMe_LOG_Info(__XT("Enter"));
	m_pContactCallback = pCallback;
}

void YouMeIMManager::DownloadKeyword(const XString& strURL, const XString& strMD5)
{
	XString strSavePath = youmecommon::CXFile::CombinePath(g_pSystemProvider->getCachePath(), __XT("keyword.zip"));
	XString strKeywordPath = youmecommon::CXFile::CombinePath(g_pSystemProvider->getCachePath(), __XT("keyword.txt"));
	bool download = false;
	if (!youmecommon::CXFile::IsFileExist(strSavePath))
	{
		download = true;
	}
	else
	{
		XString srtFileMD5 = youmecommon::CCryptUtil::MD5File(strSavePath);
		if (CStringUtil::compare_nocase(strMD5.c_str(), srtFileMD5.c_str()) != 0)
		{
			download = true;
			YouMe_LOG_Info(__XT("local md5:%s"), srtFileMD5.c_str());
		}
	}
	if (download)
	{
		if (!CDownloadUploadManager::DownloadFile(strURL, strSavePath))
		{
			YouMe_LOG_Error(__XT("download failde url:%s"), strURL.c_str());
			return;
		}
		youmecommon::CXFile::remove_file(strKeywordPath);
	}	
	if (!youmecommon::CXFile::IsFileExist(strKeywordPath))
	{
		CMiniUnZip unzip;
		if (!unzip.Open(strSavePath))
		{
			YouMe_LOG_Error(__XT("open keyword zip failed %s"), strSavePath.c_str());
			return;
		}
		if (!unzip.UnZipToDir(g_pSystemProvider->getCachePath()))
		{
			YouMe_LOG_Error(__XT("unzip failed %s"), strSavePath.c_str());
			return;
		}
	}
	g_pKeywordFilter->LoadFromFile(strKeywordPath);
}

XUINT64 YouMeIMManager::GetMaxAccusationID()
{
	if (0 == m_ullMaxAccusationID && g_pProfileDB != NULL)
	{
		XString strMaxAccusationID;
		std::string strKey = CStringUtilT<char>::formatString("MaxAccusationID_%d_%s_%d", GetAppID(), XStringToUTF8(m_strCurUserID).c_str(), g_iYouMeIMMode);
		g_pProfileDB->getSetting(strKey, strMaxAccusationID);
		if (!strMaxAccusationID.empty())
		{
			m_ullMaxAccusationID = CStringUtil::str_to_sint64(strMaxAccusationID);
		}
	}
	return m_ullMaxAccusationID;
}

bool YouMeIMManager::SendData(YOUMEServiceProtocol::COMMOND_TYPE command, const char* buffer, int bufferSize, std::map<std::string, std::string>& extend, XUINT64& serial)
{
	if (m_pNetworkServivce == NULL || buffer == NULL || bufferSize == 0)
	{
		return false;
	}

	if (m_loginStatus == IMManangerLoginStatus_Reconnecting)
	{
		std::lock_guard<std::mutex> lock(m_sendCacheMutex);
		if (m_packetCache.size() >= 100 || m_packetCache.find(serial) != m_packetCache.end())
		{
			YouMe_LOG_Info(__XT("pachet cache full"));
			return false;
		}
		PacketInfo packetInfo;
		packetInfo.commandType = command;
		packetInfo.buffer = youmecommon::CXSharedArray<char>(bufferSize);
		if (packetInfo.buffer.Get() == NULL)
		{
			YouMe_LOG_Warning(__XT("build CXSharedArray failed"));
			return false;
		}
		memcpy(packetInfo.buffer.Get(), buffer, bufferSize);
		if (0 == serial)
		{
			serial = GetUniqueSerial();
		}
		packetInfo.serial = serial;
		packetInfo.extend = extend;

		m_packetCache[serial] = packetInfo;
		return true;
	}

	
	if (0 == serial)
	{
		serial = GetUniqueSerial();
	}
    
    if(command == YOUMEServiceProtocol::CMD_LOGOUT){
        return m_pNetworkServivce->SendData(command, buffer, bufferSize, extend, serial, true, 1000);
    }else{
        return m_pNetworkServivce->SendData(command, buffer, bufferSize, extend, serial);
    }
}

void YouMeIMManager::SendCacheData()
{
	std::lock_guard<std::mutex> lock(m_sendCacheMutex);
	for (std::map<XUINT64, PacketInfo>::iterator itr = m_packetCache.begin(); itr != m_packetCache.end();)
	{
		if (m_loginStatus != IMManangerLoginStatus_Success)
		{
			return;
		}
		SendData(itr->second.commandType, itr->second.buffer.Get(), itr->second.buffer.GetBufferLen(), itr->second.extend, itr->second.serial);
		itr = m_packetCache.erase(itr);
	}
}

bool YouMeIMManager::SendDataRandKey(YOUMEServiceProtocol::COMMOND_TYPE command, const char* buffer, int bufferSize, std::map<std::string, std::string>& extend, XUINT64& serial)
{
	if (m_pNetworkServivce == NULL)
	{
		return false;
	}

	serial = GetUniqueSerial();
	return m_pNetworkServivce->SendData(command, buffer, bufferSize, extend, serial, false);
}

void YouMeIMManager::InitReport()
{
	YouMe_LOG_Info(__XT("Enter"));

	std::string strReportAddr;
	std::string strDefaultIP = REPORT_DEFAULT_ADDR;
	if (g_iYouMeIMMode == ENVIROMENT_FORMAL)
	{
		if (m_sdkDomainMap.find(g_ServerZone) != m_sdkDomainMap.end())
		{
			XString strAppKey = g_pSystemProvider->getAppKey();
			if (strAppKey.length() < 8)
			{
				YouMe_LOG_Error(__XT("APPKey is invalid"));
				return;
			}
			std::string appkey = XStringToUTF8(strAppKey);
			strReportAddr = appkey.substr(strAppKey.length() - 8) + ".dr.youme.im";
			if (!(g_ServerZone == ServerZone_China || g_ServerZone == ServerZone_HongKong || g_ServerZone == ServerZone_Unknow))
			{
				strDefaultIP = REPORT_DEFAULT_ADDR_2;
			}
		}
	}
	else if (g_iYouMeIMMode == ENVIROMENT_DEVELOP)
	{
		strReportAddr = DATAREPORT_ADDR_DEV;
	}
	else if (g_iYouMeIMMode == ENVIROMENT_TEST)
	{
		strReportAddr = DATAREPORT_ADDR_TEST;
	}
	else if (g_iYouMeIMMode == ENVIROMENT_BUSSINESS)
	{
		strReportAddr = DATAREPORT_ADDR_BUSSINESS;
	}
	if (!strReportAddr.empty())
	{
		m_report.Init(strReportAddr, REPORT_PORT_TCP, REPORT_PORT_UDP, strDefaultIP);
	}

	YouMe_LOG_Info(__XT("Leave"));
}

void YouMeIMManager::ReportDataLogin(int status, unsigned int time)
{
	ReportLogin reportData;
	reportData.userid = XStringToUTF8(m_strCurUserID);
	reportData.appid = m_iAppID;
	reportData.status = status;
#ifdef WIN32
	reportData.platform = YOUMECommonProtocol::Platform_Windows;
#elif OS_OSX
	reportData.platform = YOUMECommonProtocol::Platform_OSX;
#elif OS_IOS
	reportData.platform = YOUMECommonProtocol::Platform_IOS;
#elif OS_IOSSIMULATOR
	reportData.platform = YOUMECommonProtocol::Platform_IOS;
#elif OS_ANDROID
	reportData.platform = YOUMECommonProtocol::Platform_Android;
#elif OS_LINUX
	reportData.platform = YOUMECommonProtocol::Platform_Linux;
#endif
	reportData.networkType = m_nNetworkType;
	reportData.SDKVersion = g_pSystemProvider->getSDKVersion();
	reportData.time = time;
	reportData.createTime = static_cast<unsigned int>(GetServerValidTime() / 1000);
	XString strAddr = GetConfigure<XString>(ACCESS_SERVER_ADDR, __XT(""));
	reportData.serverIp = CStringUtilT<char>::IPToInt(XStringToUTF8(strAddr).c_str());

	m_report.Report(reportData);
}

void YouMeIMManager::ReportDataReconnect(int status, short reason)
{
	ReportCommon reportData;
	reportData.type = REPORTTYPE_RECONNECT;
	reportData.userid = XStringToUTF8(m_strCurUserID);
	reportData.appid = m_iAppID;
	reportData.SDKVersion = SDK_VER;
	reportData.reverse1 = reason;
	reportData.reverse2 = status;
	reportData.createTime = static_cast<unsigned int>(GetServerValidTime() / 1000);
	m_report.Report(reportData);
}

void YouMeIMManager::ReportValidate(int status, unsigned int time, std::string& ip)
{
	//初始化上报
	ReportSDKValidate reportData;
	reportData.status = status;
	reportData.time = static_cast<unsigned int>(time);
	reportData.SDKVersion = g_pSystemProvider->getSDKVersion();
	reportData.serverZone = g_ServerZone;
#ifdef WIN32
	reportData.platform = YOUMECommonProtocol::Platform_Windows;
#elif OS_OSX
	reportData.platform = YOUMECommonProtocol::Platform_OSX;
#elif OS_IOS
	reportData.platform = YOUMECommonProtocol::Platform_IOS;
#elif OS_IOSSIMULATOR
	reportData.platform = YOUMECommonProtocol::Platform_IOS;
#elif OS_ANDROID
	reportData.platform = YOUMECommonProtocol::Platform_Android;
#elif OS_LINUX
	reportData.platform = YOUMECommonProtocol::Platform_Linux;
#endif
	reportData.networkType = m_nNetworkType;
	reportData.appkey = XStringToUTF8(g_pSystemProvider->getAppKey());
	reportData.packageName = XStringToUTF8(g_pSystemProvider->getPackageName());
	reportData.systemVersion = XStringToUTF8(g_pSystemProvider->getSystemVersion());
	reportData.customSystemVersion = "";
    reportData.deviceToken = XStringToUTF8(g_pSystemProvider->getUUID());
	reportData.cpuArch = XStringToUTF8(g_pSystemProvider->getCpuArchive());
	reportData.cpuChip = XStringToUTF8(g_pSystemProvider->getCpuChip());
	reportData.brand = XStringToUTF8(g_pSystemProvider->getBrand());
	reportData.model = XStringToUTF8(g_pSystemProvider->getModel());
	reportData.createTime = static_cast<unsigned int>(GetServerValidTime() / 1000);
	reportData.serverIp = CStringUtilT<char>::IPToInt(ip.c_str());
	reportData.appid = m_iAppID;

	m_report.Report(reportData);
}

void YouMeIMManager::ReportData(ReportMessage& data)
{
	m_report.Report(data);
}

void YouMeIMManager::OnPause(bool pauseReceiveMessage)
{
    if (m_pMessageManager != NULL)
    {
        m_pMessageManager->PauseReceiveMessage(pauseReceiveMessage);
        YouMe_LOG_Info(__XT("OnPause, pauseRecvMesFlag: %d"),pauseReceiveMessage);
    }else{
        YouMe_LOG_Info(__XT("OnPause, pauseRecvMesFlag: %d"),0);
    }
	m_bApplicationForeground = false;
}

void YouMeIMManager::OnResume()
{
    bool pauseRecvMes = false;
    if (m_pMessageManager != NULL)
    {
        pauseRecvMes = m_pMessageManager->m_pauseReceivingFlag;
        //恢复接收IM的消息
        m_pMessageManager->PauseReceiveMessage(false);
    }
    
	m_bApplicationForeground = true;
	YouMe_LOG_Info(__XT("OnResume loginStatus:%d initStatus:%d"), m_loginStatus, m_initStatus);

	if (m_strCurUserID.empty())
	{
		return;
	}
	if (m_loginStatus == IMManangerLoginStatus_Not && (IMManagerStatus_Init == m_initStatus || IMManagerStatus_initfailed == m_initStatus) && !m_strLastLoginedUserID.empty())
	{
		ReconectProc(REASON_FORCEGROUND);
	}
    //程序切换到后台若设置暂停接收消息，重新拉取消息
    if (m_pMessageManager != NULL && pauseRecvMes)
    {
        m_pMessageManager->GetNewMessage(true);
    }
}

YIMErrorcode YouMeIMManager::QueryNotice()
{
	if (m_pNoticeManager != NULL)
	{
		return m_pNoticeManager->QueryNotice();
	}

	return YIMErrorcode_Success;
}

void YouMeIMManager::InitNoticManager()
{
	if (NULL == m_pNoticeManager)
	{
		m_pNoticeManager = new NoticeManager();
		m_pNoticeManager->Init();
		m_pNoticeManager->SetNoticeCallback(m_pNoticeCallback);
	}
}
