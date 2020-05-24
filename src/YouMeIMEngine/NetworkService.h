#ifndef NETWORK_SERVICE_H
#define NETWORK_SERVICE_H


#include <string>
#include <vector>
#include <map>
#include <thread>
#include "YouMeCommon/network/unix_socket.h"
#include "YouMeCommon/XCondWait.h"
#include "YouMeCommon/network/autobuffer.h"
#ifdef WIN32
#include "YouMeCommon/network/windows/socketselect2.h"
#else
#include "YouMeCommon/network/unix/socketselect.h"
#endif
#include "YouMeIMEngine/YouMeIMCommonDef.h"


enum NetEventErrorcode
{
	NETEVENT_CONNECT_SUCCESS,	// 初始化连接成功
	NETEVENT_CONNECT_FAILED,	// 初始化连接失败
	NETEVENT_NET_DISCONNECTED,	// 网络断开
	NETEVENT_NET_IO_ERROR,

	NETEVENT_ERROR,
};

struct IPPortItem
{
	std::string addr;
	unsigned short port;
	//IPPortItem() : addr(""), port(0){}

	bool operator< (const IPPortItem& r)
	{
		return port < r.port;
	}

	bool operator== (const IPPortItem& r)
	{
		return addr == r.addr && port == r.port;
	}
};

class INetworkServiceEvent
{
public:
	virtual void OnConnect(NetEventErrorcode errorcode, IPPortItem& address, int connectType) = 0;	// connectType 0:初始化连接 1:心跳重连 2:重新验证重连
	virtual void OnError(NetEventErrorcode errorcode) = 0;
	virtual void OnRecvPacket(ServerPacket& serverPacket) = 0;
};

enum TaskPriority
{
	TASKPRIORITY_HIGH,
	TASKPRIORITY_NORMAL
};

struct Task 
{
	XUINT64 taskID;
	TaskPriority priority;

	Task() : taskID(0), priority(TASKPRIORITY_NORMAL){}
};

class NetworkService
{
public:
	NetworkService();
	~NetworkService();

	int Init(INetworkServiceEvent* callback, unsigned short connectTimeout = 5000);
	void UnInit();
	int Connect(IPPortItem addrItem, int connectType = 0);	// connectType 0:初始化连接 1:心跳重连 2:重新验证重连
	bool IsConnect(){ return m_connectStatus == STATUS_CONNECTED; }
	XString LocalIP(){ return m_strLocalIP; }
	int DisconnectAndWait();
	void InterruptConnect();
	bool SendData(int commandType, const char* buffer, int bufferSize, std::map<std::string, std::string>& extend, XUINT64 serial, bool secretEncrypt = true, XINT64 timeout = 10000);
	void CleanupPacket();
	void SetSessionInfo(XString& userID, unsigned int serviceID, youmecommon::CXSharedArray<unsigned char>& secret);
	void SetRetrySendTime(int retrySendTimes);

private:
	enum ServiceStatus
	{
		STATUS_DISCONNECTED,
		STATUS_CONNECTING,
		STATUS_CONNECTED,
		STATUS_IO_ERROR,
		STATUS_WILL_DISCONNECT
	};

	struct ResponsePacket
	{
		uint16_t commondID;
		short result;
		uint64_t reqSerial;
		youmecommon::CXSharedArray<char> buffer;
		std::map<std::string, std::string> extend;

		ResponsePacket() : commondID(0), result(0), reqSerial(0){}
	};

	void EncryptData(UDPTCPPacketInfo& packetInfo, XUINT64 serial, youmecommon::CXSharedArray<char>& buffer);
	void EncryDataXOR(char* pBuffer, int iBufferLen);
	void EncryDecryptPacketBody(unsigned char* buffer, int bufferLen, unsigned char* key, int keyLen);

	void TaskThreadProc(IPPortItem addrItem, int connectType);
	SOCKET ConectServer(IPPortItem& addrItem);
	SOCKET ConectServer(const std::string& ip, unsigned short port);
	int DealReceiveData(youmecommon::AutoBuffer& buffer);
	int SendData();
	void DispatchThreadProc();

	bool m_bInit;
	int m_nRetrySendTimes;
	unsigned int m_uiServiceID;
	std::string m_strCurrentUserID;
	XString m_strLocalIP;
	youmecommon::CXSharedArray<unsigned char> m_pSecretbuffer;
	INetworkServiceEvent* m_pEventCallback;

	youmecommon::SocketBreaker m_connectBreaker;
	youmecommon::SocketBreaker m_readWriteBreaker;
	SOCKET m_socket;
	IPPortItem m_address;
	ServiceStatus m_connectStatus;

	std::list<Task> m_taskList;
	std::mutex m_taskMutex;
	std::thread m_taskDealThread;
	std::thread m_dispatchThread;
	std::mutex m_taskDealThreadMutex;
	std::mutex m_dispatchThreadMutex;
	std::list<ResponsePacket> m_dispatchPacketList;
	std::mutex m_dispatchMutex;
	youmecommon::CXCondWait m_dispatchPacketWait;
	bool m_bDispatchRunning;
	std::multimap<std::string, std::string> loginAddresMap;
};

#endif
