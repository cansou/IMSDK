#pragma once


#include <mutex>
#include <thread>
#include <map>
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeIMEngine/YouMeIMCommonDef.h>
#include <YouMeCommon/SyncTCP.h>
#include <YouMeCommon/XSemaphore.h>
#include <YouMeCommon/XCondWait.h>
#ifdef WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#endif


class YouMeIMManager;

class CXUDP
{
public:
	CXUDP(YouMeIMManager* pIManager);
	~CXUDP();
	bool Init(const XString&strServerIP, int iPort, IPacketCallback* pCallback);
	void UnInit();
	bool SendData(int commandType, const char* buffer, int bufferSize, std::map<std::string, std::string>& extend, XUINT64 serial, bool secretEncrypt = true, XINT64 timeout = 10000);
	void CleanupPacket();
	static void SetRetrySendTime(int retrySendTime);
	void SetSessionInfo(XString& userID, unsigned int serviceID, youmecommon::CXSharedArray<unsigned char>& secret);

private:
	void SendThread();
	void UDPRecvThread();
	bool DealRead();
	bool DealSend();
	
	void ConvertMsgPacket();

	void EncryptData(UDPTCPPacketInfo& packetInfo, XUINT64 serial, youmecommon::CXSharedArray<char>& buffer);
	void EncryDataXOR(char* pBuffer, int iBufferLen);
	void EncryDecryptPacketBody(unsigned char* buffer, int bufferLen, unsigned char* key, int keyLen);

	bool m_bInit;
	youmecommon::CSyncTCP m_client;
	IPacketCallback* m_pPacketCallback;
	YouMeIMManager* m_pIManager;
	std::thread m_sendThread;
	std::thread m_recvThread;
	bool m_bRecvThreadExit;
	bool m_bSendThreadExit;
	youmecommon::CXSemaphore m_sendCondWait;

	static int m_nRetrySendTimes; //重发次数
	//std::mutex m_mutex;
	//std::map<XUINT64, UDPTCPPacketInfo >m_needSendPacketMap;	//保存需要发送的包
	//std::map<XUINT64, UDPTCPPacketInfo >m_allPacketMap;	//保存所有待发送的包
	//std::set<XINT64> m_recvPackSerial;					//保存所有收到包的序号，以防止重复

	unsigned int m_uiServiceID;
	std::string m_strCurrentUserID;
	youmecommon::CXSharedArray<unsigned char> m_pSecretbuffer;
};