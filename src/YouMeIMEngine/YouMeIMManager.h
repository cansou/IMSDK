#pragma once

#include <map>
#include <thread>
#include <mutex>
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeIMEngine/YouMeIMMessageManager.h>
#include <YouMeIMEngine/YouMeIMGroupManager.h>
#include <YouMeCommon/NetworkService.h>
#include <YouMeCommon/TimeUtil.h>
#include <YouMeCommon/XCondWait.h>
#include <YouMeCommon/pb/youme_service_head.h>
#include <YouMeIMEngine/pb/server_protocol_code.h>
#include <YouMeIMEngine/DataReportDefine.h>
#include <YouMeCommon/AccessValidate.h>
#include <YouMeIMEngine/NetworkService.h>

#include <YouMeIMEngine/YouMeIMUserInfoManager.h>


//引擎状态
enum IMManagerStatus
{
	IMManagerStatus_NotInit,	//没有初始化
	IMManagerStatus_Ing,		//正在初始化
	IMManagerStatus_Init,		//已经初始化
	IMManagerStatus_uniniting,	//正在反初始化
	IMManagerStatus_initfailed	//初始化失败了
};

enum IMManangerLoginStatus
{
	IMManangerLoginStatus_Not,//没有登陆
	IMManangerLoginStatus_Ing,//正在登陆
	IMManangerLoginStatus_Success,//登陆完成
	IMManangerLoginStatus_Logouting,//正在登出
	IMManangerLoginStatus_Reconnecting,//正在重连
};

enum EnviromentType
{
	ENVIROMENT_FORMAL = 0,
	ENVIROMENT_DEVELOP = 1,
	ENVIROMENT_TEST = 2,
	ENVIROMENT_BUSSINESS = 3
};

enum ReconnectReason
{
	REASON_LOSS_HEARTBEAT,	// 心跳丢失
	REASON_NETWORK_ERROR,	// 网络错误
	REASON_NETWORK_CHANGE,	// 网络切换
	REASON_FORCEGROUND,		// 切到前台
	REASON_REVALIDATE,		// 重新执行验证及登录
	REASON_RELOGIN			// 重新执行登录
};

struct ServerZoneInfo
{
	XString sdkValidataDomain;
	XString reportDomain;
};

struct PacketInfo
{
	YOUMEServiceProtocol::COMMOND_TYPE commandType;
	XUINT64 serial;
	youmecommon::CXSharedArray<char> buffer;
	std::map<std::string, std::string> extend;
};

class CYouMeDataChannel;
class YouMeLocationManager;
class NoticeManager;
class YouMeFriendManager;

class YouMeIMUserInfoManager;

class YouMeIMManager :public YIMManager, public youmecommon::INgnNetworkChangCallback, public youmecommon::SDKValidateCallback, public INetworkServiceEvent
{
	friend class CXUDP;
	friend class YouMeIMMessageManager;
	friend class YouMeIMGroupManager;
	friend class YouMeLocationManager;
	friend class CYouMeSpeechManager;
    
    friend class YouMeIMUserInfoManager;
    
public:
	~YouMeIMManager();
	YouMeIMManager()
	{
		m_nHeartbeatLossCount = 0;
		m_localTime = youmecommon::CTimeUtil::GetTimeOfDay_MS();
		s_iPacketSerial = 10000;
		m_pMessageManager = NULL;
		m_pGroupManager = NULL;
		m_pLocationManager = NULL;
                
        m_pUserProfileManager = NULL;
        m_pUserProfileCallback = NULL;
        
		m_loginStatus = IMManangerLoginStatus_Not;
		m_pFriendManager = NULL;
        m_pLoginCallback = NULL;
        m_pGroupCallback = NULL;
        m_pMessageCallback = NULL;
		m_pContactCallback = NULL;
		m_pLocationCallback = NULL;
		m_pNoticeCallback = NULL;
        m_pReconnectCallback = NULL;
		m_pFriendCallback = NULL;
		m_initStatus = IMManagerStatus_NotInit;
		m_loginStatus = IMManangerLoginStatus_Not;
		m_llRequestLoginTime = 0;
		m_nReconnectReason = REASON_LOSS_HEARTBEAT;
		m_nNetworkType = 0;
		m_bApplicationForeground = true;
		m_iAppID = 0;
		m_llLastNetworkChangeTime = 0;
		m_nInitiativeMessageRound = 0;
		m_nMessageHeartBeatCount = 0;
		m_ullMaxAccusationID = 0;
		m_pNoticeManager = NULL;
		m_pSDKValidate = NULL;
		m_pNetworkServivce = NULL;
		m_iReconnectLoginTimes = 0;
		m_iCurentLoginAddrIndex = 0;
		m_bConnectFaileRevalidate = true;
		m_bReconnecThreadRunning = false;
		m_llLoginedTime = 0;
        m_autoInCrementID = 0;
        m_bConnectCheckExit = false;

		// 中国		cn
		m_sdkDomainMap[ServerZone_China] = std::string("cn");
		// 新加坡	sg
		m_sdkDomainMap[ServerZone_Singapore] = std::string("sg");
		// 美国		us
		m_sdkDomainMap[ServerZone_America] = std::string("us");
		// 香港		hk
		m_sdkDomainMap[ServerZone_HongKong] = std::string("hk");
		// 韩国		kr
		m_sdkDomainMap[ServerZone_Korea] = std::string("kr");
		// 澳洲		au
		m_sdkDomainMap[ServerZone_Australia] = std::string("au");
		// 德国		de
		m_sdkDomainMap[ServerZone_Deutschland] = std::string("de");
		// 巴西		br
		m_sdkDomainMap[ServerZone_Brazil] = std::string("br");
		// 印度		in
		m_sdkDomainMap[ServerZone_India] = std::string("in");
		// 日本		jp
		m_sdkDomainMap[ServerZone_Japan] = std::string("jp");
		// 爱尔兰	ie
		m_sdkDomainMap[ServerZone_Ireland] = std::string("ie");
		// 泰国		th
		m_sdkDomainMap[ServerZone_Thailand] = std::string("th");
		// 台湾		tw
		m_sdkDomainMap[ServerZone_Taiwan] = std::string("tw");
		m_sdkDomainMap[ServerZone_Unknow] = std::string("auto");
	}
	
    //初始化和反初始化
	virtual YIMErrorcode Init(const XCHAR* appKey, const XCHAR* appSecurity, const XCHAR* packageName) override;
	void UnInit();
	static YIMErrorcode SetLoginAddress(const char* ip, unsigned short port);

	//登陆和登出
	virtual void SetLoginCallback(IYIMLoginCallback* pCallback)override;
    //设置消息回调
    virtual void SetMessageCallback(IYIMMessageCallback* pCallback) override;
    //设置群组回调
    virtual void SetChatRoomCallback(IYIMChatRoomCallback* pCallback) override;
    //设置下载回掉
	virtual void SetDownloadCallback(IYIMDownloadCallback* pCallback) override;
	//设置消息已读回掉
	virtual void SetUpdateReadStatusCallback(IYIMUpdateReadStatusCallback* pCallback) override;
	//设置联系人回调
	virtual void SetContactCallback(IYIMContactCallback* pCallback) override;
	IYIMContactCallback* GetContactCallback(){return m_pContactCallback;}
	//设置语音播放回调
	virtual void SetAudioPlayCallback(IYIMAudioPlayCallback* pCallback) override;
	// 设置地理位置回调
	virtual void SetLocationCallback(IYIMLocationCallback* pCallback) override;
	virtual void SetNoticeCallback(IYIMNoticeCallback* pCallback) override;
    // 设置重连回调
    virtual void SetReconnectCallback(IYIMReconnectCallback* pCallback) override;
	// 设置好友回调
	virtual void SetFriendCallback(IYIMFriendCallback* pCallback) override;

     // 设置用户信息回调
    virtual void SetUserProfileCallback(IYIMUserProfileCallback* pCallback) override;

	virtual YIMErrorcode Login(const XCHAR* userID, const XCHAR* password, const XCHAR* token) override;
	virtual YIMErrorcode Logout() override;

	virtual YIMErrorcode GetRecentContacts() override;

	virtual YIMErrorcode SetUserInfo(const XCHAR* userInfo) override;
	virtual YIMErrorcode GetUserInfo(const XCHAR* userID) override;
	virtual YIMErrorcode QueryUserStatus(const XCHAR* userID) override;

	virtual YIMErrorcode QueryNotice() override;

	virtual YIMChatRoomManager* GetChatRoomManager() override;
	virtual YIMMessageManager* GetMessageManager() override;
    virtual YIMLocationManager* GetLocationManager() override;
    
    virtual YIMUserProfileManager* GetUserProfileManager() override;
	virtual YIMFriendManager* GetFriendManager() override;

	virtual void OnPause(bool pauseReceiveMessage) override;
	virtual void OnResume() override;

	//获取一个唯一的标识
	static XUINT64 GetUniqueSerial()
	{
		XUINT64 iTmp = 0;
		{
			std::lock_guard<std::mutex> lock(m_serialMutex);
			iTmp = ++s_iPacketSerial;
		}
		return iTmp;
	}
	
	IMManangerLoginStatus GetLoginStatus(){ return m_loginStatus; }
	XString GetCurrentUser(){ return m_strCurUserID; }
	int GetAppID(){ return m_iAppID; }
	XINT64 GetLoginTime(){ return m_llLoginedTime; }
	void ReportData(ReportMessage& data);
	XUINT64 GetServerValidTime();	//获取一个和服务器对过时的时间
    XUINT64 GetAutoIncrementID();//获取自增id
	void SetInitiativeGetMessageCount();
	void SetMaxAccusationID(XUINT64 accusationID){ m_ullMaxAccusationID = accusationID; }
	XUINT64 GetMaxAccusationID();

	bool SendData(YOUMEServiceProtocol::COMMOND_TYPE command, const char* buffer, int bufferSize, std::map<std::string, std::string>& extend, XUINT64& serial);

private:
	virtual void onNetWorkChanged(youmecommon::NetworkType type) override;

	virtual void OnSDKValidteComplete(youmecommon::SDKValidateErrorcode errorcode, std::map<std::string, youmecommon::CXAny>& configurations, unsigned int constTime, youmecommon::ValidateReason reason, std::string& ip) override;

	virtual void OnConnect(NetEventErrorcode errorcode, IPPortItem& address, int connectType) override;
	virtual void OnError(NetEventErrorcode errorcode) override;
	virtual void OnRecvPacket(ServerPacket& serverPacket) override;

	bool SendDataRandKey(YOUMEServiceProtocol::COMMOND_TYPE command, const char* buffer, int bufferSize, std::map<std::string, std::string>& extend, XUINT64& serial);
	void SendCacheData();
	youmecommon::SDKValidateErrorcode ValidateSDK(youmecommon::ValidateReason reason);

	void OnHeartBeatRsp(ServerPacket& serverPacket);
	YIMErrorcode ConnectLoginServer();
	YIMErrorcode RequestLogin();
	void OnLoginRsp(ServerPacket& serverPacket);
	void OnLogoutRsp(ServerPacket& serverPacket);
	void OnKickOffRsp(ServerPacket& serverPacket);
	void OnReloginNotify(ServerPacket& serverPacket);
	void OnUpdateConfigNotify(ServerPacket& serverPacket);
	void OnSetUserInfoRsp(ServerPacket& serverPacket);
	void OnGetUserInfoRsp(ServerPacket& serverPacket);
	void OnQueryUserStatusRsp(ServerPacket& serverPacket);

	int ParseRSASecret(const std::string& secret, youmecommon::CXSharedArray<unsigned char>& secretKey);
	void DownloadKeyword(const XString& strURL, const XString& strMD5);
	void UpdateServerConfig(bool isInit);

	//获取SDK下发的配置
	template <class T>
	T GetConfigure(const std::string& strKey,const T& defaultValue)
	{
		std::lock_guard<std::mutex> lock(m_ConfigMutex);
		std::map<std::string, youmecommon::CXAny>::iterator it =  m_configurations.find(strKey);
		if (it != m_configurations.end())
		{
			return youmecommon::CXAny::XAny_Cast<T>(it->second);
		}
		return defaultValue;
	}

	void InitReport();
	void ReportDataLogin(int status, unsigned int time);
	void ReportDataReconnect(int status, short reason);
	void ReportValidate(int status, unsigned int time, std::string& ip);

	void InitNoticManager();

private:
	IYIMLoginCallback* m_pLoginCallback;
	IYIMMessageCallback* m_pMessageCallback;
	IYIMDownloadCallback* m_pDownloadCallback;
	IYIMUpdateReadStatusCallback* m_pUpdateReadStatusCallback = NULL;
	IYIMChatRoomCallback* m_pGroupCallback;
	IYIMContactCallback* m_pContactCallback;
	IYIMLocationCallback* m_pLocationCallback;
	IYIMNoticeCallback* m_pNoticeCallback;
    IYIMReconnectCallback* m_pReconnectCallback;
	IYIMFriendCallback* m_pFriendCallback;

	std::mutex m_managerMutex;	//获取管理器的锁
	YouMeIMMessageManager* m_pMessageManager;
	YouMeIMGroupManager* m_pGroupManager;
	YouMeLocationManager* m_pLocationManager;
	NoticeManager* m_pNoticeManager;
    
    
    IYIMUserProfileCallback* m_pUserProfileCallback;
    YouMeIMUserInfoManager* m_pUserProfileManager;
	YouMeFriendManager* m_pFriendManager;
	XString m_strCurUserID;
	XString m_strLastLoginedUserID;
	XString m_strPasswd;
	XString m_strToken;
	int m_iAppID;
	IMManagerStatus m_initStatus;
	IMManangerLoginStatus m_loginStatus;
	int m_iCurentLoginAddrIndex;
	std::vector<IPPortItem> m_loginAddrList;	// 接入返回登录地址
	bool m_bApplicationForeground;

	static XUINT64 s_iPacketSerial;	//发包序列号
	static std::mutex m_serialMutex;
	XINT64 m_localTime;	//本地时间，用来和服务器对时（毫秒）
	static std::map<ServerZone, std::string> m_sdkDomainMap;		//SDK验证域名
	std::map<std::string, youmecommon::CXAny> m_configurations;	//SDK验证下发的配置
	std::mutex m_ConfigMutex;
	
	void ReconectProc(ReconnectReason reconnectReason);
	void ReconnectThread(ReconnectReason reconnectReason);
    
    void ReconnectCheckThread();
    youmecommon::CXSemaphore m_reconnectCheckSemWait;    //触发重连处理的信号
    //增加一个检查线程吧， NetworkService的TaskDeal线程和reconnect线程互相等待，本来想把reconnect线程改为一直存在，
    //但发现还有其他线程在等它，不敢改了，新加一个线程来去掉这个循环等待把
    std::thread m_reconnectCheckThread;
    std::mutex m_reconnectCheckThreadMutex;
    bool  m_bConnectCheckExit;
    
	std::thread m_reconnectThread;
	std::mutex m_reconnectThreadMutex;
	youmecommon::CXCondWait m_reconnectWait;
	int m_iReconnectLoginTimes;			// 重连登录次数
	bool m_bConnectFaileRevalidate;		// 缓存登录地址连接失败，再执行一次SDK验证
	XINT64 m_llLastNetworkChangeTime;
	bool m_bReconnecThreadRunning;

	unsigned int m_iHeartbeatInterval = 5000;			//心跳间隔（second）
	unsigned short m_iHeartbeatTimeoutCount = 3;	//心跳包丢失次数(超过该次数需要重连)
	std::thread m_heartBeatThread;
	std::mutex m_heartBeatThreadMutex;
	void HeartBeatThreadProc();
	youmecommon::CXCondWait m_heartBeatWait;
	int m_nHeartbeatLossCount;
	short m_nInitiativeMessageRound;	//主动消息轮数
	short m_nMessageHeartBeatCount;

    //处理登录的善后工作
	void LogoutThread();
	std::thread m_logoutThread;
	youmecommon::CXCondWait m_logoutWait;
	void DealLogoutCleanup(bool isLogout);

	//上报
	XINT64 m_llRequestLoginTime;
	ReconnectReason m_nReconnectReason;
	int m_nNetworkType;
	DataReport m_report;
	XINT64 m_llLoginedTime;
    XUINT64 m_autoInCrementID;
	
	std::map<XUINT64, PacketInfo> m_packetCache;
	std::mutex m_sendCacheMutex;
    std::mutex m_audoIncrementIdMutex;
	bool m_bIsCleanup = false;

	XUINT64 m_ullMaxAccusationID;	//已通知的最大举报消息ID

	youmecommon::AccessValidate* m_pSDKValidate;
	NetworkService* m_pNetworkServivce;

	static IPPortItem m_privateServerLoginAddress;
};
