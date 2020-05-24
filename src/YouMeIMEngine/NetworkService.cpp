#include "NetworkService.h"
#include "YouMeCommon/Log.h"
#include "YouMeCommon/TimeUtil.h"
#include "YouMeCommon/pb/youme_service_head.h"
#include "YouMeCommon/StringUtil.hpp"
#include "YouMeCommon/DNSUtil.h"
#include "YouMeCommon/network/socket_address.h"
#include "YouMeIMEngine/pb/server_protocol_code.h"
#ifdef WIN32
#include <iphlpapi.h>
#endif

#define PARSE_DOMAIN_TIMEOUT 3000
#define LOGIN_CONNECT_TIMEOUT 15000
#define DISPATCH_INTERVAL 1000
#define RECV_BUFFER_SIZE 64 * 1024

std::mutex* packtMutex = new std::mutex;
static std::map<XUINT64, UDPTCPPacketInfo>* s_allPacketMap = new std::map<XUINT64, UDPTCPPacketInfo>;			//保存所有待发送的包


bool CompareTask(const Task& first, const Task& second)
{
	return first.priority < second.priority;
}

NetworkService::NetworkService() : m_socket(INVALID_SOCKET)
, m_connectStatus(STATUS_DISCONNECTED)
, m_pEventCallback(NULL)
, m_bInit(false)
, m_bDispatchRunning(false)
, m_nRetrySendTimes(2)
{

}

NetworkService::~NetworkService()
{
	UnInit();
}

int NetworkService::Init(INetworkServiceEvent* callback, unsigned short connectTimeout /*= 5000*/)
{
	YouMe_LOG_Info(__XT("enter"));

	if (m_bInit)
	{
		return 0;
	}
	if (NULL == callback)
	{
		return -1;
	}

	m_pEventCallback = callback;

	m_bInit = true;

	YouMe_LOG_Info(__XT("leave"));

	return 0;
}

void NetworkService::UnInit()
{
	YouMe_LOG_Info(__XT("enter"));

	if (!m_bInit)
	{
		return;
	}

	DisconnectAndWait();

	{
		std::lock_guard<std::mutex> lock(m_taskMutex);
		m_taskList.clear();
	}
	{
		std::lock_guard<std::mutex> lock(*packtMutex);
		s_allPacketMap->clear();
	}
	m_dispatchPacketList.clear();
	m_bInit = false;

	YouMe_LOG_Info(__XT("leave"));
}

int NetworkService::Connect(IPPortItem addrItem, int connectType)
{
	YouMe_LOG_Info(__XT("enter"));

	if (m_connectStatus != STATUS_DISCONNECTED && m_connectStatus != STATUS_IO_ERROR)
	{
		YouMe_LOG_Error(__XT("connect status:%d"), m_connectStatus);
		return 0;
	}

	{
		std::lock_guard<std::mutex> lock(m_taskDealThreadMutex);
		if (m_taskDealThread.joinable())
		{
			m_taskDealThread.join();
		}
		m_taskDealThread = std::thread(&NetworkService::TaskThreadProc, this, addrItem, connectType);
	}

	YouMe_LOG_Info(__XT("leave"));

	return 0;
}

int NetworkService::DisconnectAndWait()
{
	YouMe_LOG_Info(__XT("enter"));

	//if (STATUS_DISCONNECTED == m_connectStatus || STATUS_WILL_DISCONNECT == m_connectStatus)
	//{
	//	YouMe_LOG_Info(__XT("leave status:%d"), m_connectStatus);
	//	return -1;
	//}
	if (STATUS_CONNECTING == m_connectStatus || STATUS_CONNECTED == m_connectStatus)
	{
		m_connectStatus = STATUS_WILL_DISCONNECT;
	}
	
	if (m_socket != INVALID_SOCKET)
	{
		socket_close(m_socket);
		m_socket = INVALID_SOCKET;
	}
	bool recreate = false;
	if (!m_connectBreaker.Break() || !m_readWriteBreaker.Break())
	{
		m_connectBreaker.Close();
		m_readWriteBreaker.Close();
		recreate = true;
	}

	m_bDispatchRunning = false;
	m_dispatchPacketWait.SetSignal();

	{
		std::lock_guard<std::mutex> lock(m_taskDealThreadMutex);
		if (m_taskDealThread.joinable())
		{
			m_taskDealThread.join();
		}
	}
	{
		std::lock_guard<std::mutex> lock(m_dispatchThreadMutex);
		if (m_dispatchThread.joinable())
		{
			m_dispatchThread.join();
		}
	}

	m_connectStatus = STATUS_DISCONNECTED;

	if (recreate)
	{
		m_connectBreaker.ReCreate();
		m_readWriteBreaker.ReCreate();
	}

	YouMe_LOG_Info(__XT("leave"));

	return 0;
}

void NetworkService::InterruptConnect()
{
	YouMe_LOG_Info(__XT("enter"));

	if (m_connectStatus != STATUS_CONNECTING)
	{
		return;
	}
	bool recreate = false;
	if (!m_connectBreaker.Break())
	{
		m_connectBreaker.Close();
		m_connectBreaker.ReCreate();
	}

	YouMe_LOG_Info(__XT("leave"));
}

bool NetworkService::SendData(int commandType, const char* buffer, int bufferSize, std::map<std::string, std::string>& extend, XUINT64 serial, bool secretEncrypt /*= true*/, XINT64 timeout /*= 10000*/)
{
	if (m_connectStatus != STATUS_CONNECTED)
	{
		YouMe_LOG_Warning(__XT("connect status error(%d)"), m_connectStatus);
		return false;
	}

	UDPTCPPacketInfo info;
	info.commandType = commandType;
	info.ulTimeout = timeout;
	info.extend = extend;
	info.pPacket = youmecommon::CXSharedArray<char>(bufferSize);
	memcpy(info.pPacket.Get(), buffer, bufferSize);
	info.encryptFlag = secretEncrypt;
	{
		std::lock_guard<std::mutex> lock(*packtMutex);
		(*s_allPacketMap)[serial] = info;
	}

	Task task;
	task.taskID = serial;
	if (commandType == YOUMEServiceProtocol::CMD_HEARTBEAT
		|| commandType == YOUMEServiceProtocol::CMD_LOGOUT
		|| commandType == YOUMEServiceProtocol::CMD_ENTER_ROOM
		|| commandType == YOUMEServiceProtocol::CMD_LEAVE_ROOM
		|| commandType == YOUMEServiceProtocol::CMD_RELOGIN
		|| commandType == YOUMEServiceProtocol::CMD_LOGIN)
	{
		
		task.priority = TASKPRIORITY_HIGH;
	}
	{
		std::lock_guard<std::mutex> lock(m_taskMutex);
		m_taskList.push_back(task);
		m_taskList.sort(CompareTask);
	}
	
	m_readWriteBreaker.Break();

	return true;
}

void NetworkService::CleanupPacket()
{
	std::lock_guard<std::mutex> lock(*packtMutex);
	for (std::map<XUINT64, UDPTCPPacketInfo>::iterator itr = s_allPacketMap->begin(); itr != s_allPacketMap->end(); ++itr)
	{
		ServerPacket serverPacket;
		serverPacket.commondID = itr->second.commandType;
		serverPacket.result = -1;
		serverPacket.reqSerial = itr->first;
		serverPacket.extend = itr->second.extend;
		m_pEventCallback->OnRecvPacket(serverPacket);
	}
	s_allPacketMap->clear();
}

void NetworkService::SetRetrySendTime(int retrySendTimes)
{
	m_nRetrySendTimes = retrySendTimes;
}

void NetworkService::SetSessionInfo(XString& userID, unsigned int serviceID, youmecommon::CXSharedArray<unsigned char>& secret)
{
	m_strCurrentUserID = XStringToUTF8(userID);
	m_uiServiceID = serviceID;
	if (secret.Get() != NULL)
	{
		m_pSecretbuffer.Allocate(secret.GetBufferLen());
		memcpy(m_pSecretbuffer.Get(), secret.Get(), secret.GetBufferLen());
	}
}

void NetworkService::EncryDataXOR(char* pBuffer, int iBufferLen)
{
	if (m_pSecretbuffer.GetBufferLen() == 0)
	{
		return;
	}
	for (int i = 0, j = 0; i < iBufferLen; ++i, ++j)
	{
		pBuffer[i] ^= m_pSecretbuffer.Get()[j % m_pSecretbuffer.GetBufferLen()];
	}
}

void NetworkService::EncryDecryptPacketBody(unsigned char* buffer, int bufferLen, unsigned char* key, int keyLen)
{
	for (int i = 0; i < bufferLen; ++i)
	{
		buffer[i] ^= key[i % keyLen];
	}
}

void NetworkService::EncryptData(UDPTCPPacketInfo& packetInfo, XUINT64 serial, youmecommon::CXSharedArray<char>& buffer)
{
	int nUserIDLen = (int)m_strCurrentUserID.size();

	SYoumeConnHead connHead;
	connHead.m_usCmd = packetInfo.commandType;
	connHead.m_ullReqSeq = serial;
	connHead.m_uiServiceId = m_uiServiceID;

	buffer.Allocate(sizeof(SYoumeConnHead) + sizeof(unsigned char) + nUserIDLen + packetInfo.pPacket.GetBufferLen());
	memcpy(buffer.Get(), &connHead, sizeof(connHead));
	unsigned int nReqLen = sizeof(connHead);

	*(buffer.Get() + nReqLen) = (unsigned char)nUserIDLen;
	nReqLen += sizeof(unsigned char);
	unsigned int nUserIDBeginPos = nReqLen;
	memcpy(buffer.Get() + nReqLen, &m_strCurrentUserID[0], nUserIDLen);
	nReqLen += nUserIDLen;

	memcpy(buffer.Get() + nReqLen, packetInfo.pPacket.Get(), packetInfo.pPacket.GetBufferLen());
	nReqLen += packetInfo.pPacket.GetBufferLen();

	((SYoumeConnHead*)(buffer.Get()))->ToNetOrder();
	((SYoumeConnHead*)(buffer.Get()))->SetTotalLength((unsigned char*)buffer.Get(), nReqLen);

	if (packetInfo.encryptFlag)
	{
		EncryDecryptPacketBody((unsigned char*)buffer.Get() + nUserIDBeginPos, nUserIDLen, (unsigned char*)buffer.Get(), sizeof(SYoumeConnHead));
		EncryDataXOR(buffer.Get() + nUserIDBeginPos + nUserIDLen, packetInfo.pPacket.GetBufferLen());
	}
	else
	{
		EncryDecryptPacketBody((unsigned char*)buffer.Get() + nUserIDBeginPos, nUserIDLen, (unsigned char*)buffer.Get(), sizeof(SYoumeConnHead));
		EncryDecryptPacketBody((unsigned char*)buffer.Get() + nUserIDBeginPos + nUserIDLen, packetInfo.pPacket.GetBufferLen(), (unsigned char*)buffer.Get(), sizeof(SYoumeConnHead));
	}
}

void NetworkService::TaskThreadProc(IPPortItem addrItem, int connectType)
{
	YouMe_LOG_Info(__XT("enter"));

	m_connectStatus = STATUS_CONNECTING;
	m_connectBreaker.Clear();
	m_readWriteBreaker.Clear();
	
	m_socket = ConectServer(addrItem);
	
	if (m_connectStatus == STATUS_WILL_DISCONNECT)
	{
		if (m_socket != INVALID_SOCKET)
		{
			socket_close(m_socket);
            m_socket = INVALID_SOCKET;
        }
		m_connectStatus = STATUS_DISCONNECTED;
		YouMe_LOG_Info(__XT("task thread exit"));
		return;
	}

	if (m_socket == INVALID_SOCKET)
	{
		YouMe_LOG_Error(__XT("connect failed"));

		m_connectStatus = STATUS_DISCONNECTED;
		if (m_pEventCallback != NULL)
		{
			m_pEventCallback->OnConnect(NETEVENT_CONNECT_FAILED, m_address, connectType);
		}
		return;
	}
	else
	{
		m_connectStatus = STATUS_CONNECTED;

		if (0 != socket_disable_nagle(m_socket, 1))
		{
			YouMe_LOG_Error(__XT("disable nagle error(%d)"), socket_errno);
		}
		if (m_pEventCallback != NULL)
		{
           m_pEventCallback->OnConnect(NETEVENT_CONNECT_SUCCESS, m_address, connectType);			
		}
	}

	if (!m_bDispatchRunning)
	{
		std::lock_guard<std::mutex> lock(m_dispatchThreadMutex);
		if (m_dispatchThread.joinable())
		{
			m_dispatchThread.join();
		}
		m_bDispatchRunning = true;
		m_dispatchPacketWait.Reset();
		m_dispatchThread = std::thread(&NetworkService::DispatchThreadProc, this);
	}

	youmecommon::AutoBuffer bufrecv;

	while (true)
	{
		youmecommon::SocketSelect selectReadWrite(m_readWriteBreaker, true);
		selectReadWrite.PreSelect();
		selectReadWrite.Exception_FD_SET(m_socket);
		selectReadWrite.Read_FD_SET(m_socket);
		{
			std::lock_guard<std::mutex> lock(m_taskMutex);
			if (!m_taskList.empty())
			{
				selectReadWrite.Write_FD_SET(m_socket);
			}
		}
		int selectRet = selectReadWrite.Select(300000);

		if (STATUS_WILL_DISCONNECT == m_connectStatus)
		{
			break;
		}

		if (0 == selectRet)
		{
			continue;
		}
		else if (0 > selectRet)
		{
			m_connectStatus = STATUS_IO_ERROR;
			if (m_pEventCallback != NULL)
			{
				m_pEventCallback->OnError(NETEVENT_NET_DISCONNECTED);
			}
			return;
		}

		if (selectReadWrite.Exception_FD_ISSET(m_socket))
		{
			int error_opt = 0;
			socklen_t error_len = sizeof(error_opt);
			if (0 == getsockopt(m_socket, SOL_SOCKET, SO_ERROR, (char*)&error_opt, &error_len))
			{
				YouMe_LOG_Error(__XT("network exception getsockopt error_opt=%d"), error_opt);
			}
			else
			{
				YouMe_LOG_Error(__XT("network exception getsockopt error=%d"), socket_errno);
			}
			m_connectStatus = STATUS_IO_ERROR;
			if (m_pEventCallback != NULL)
			{
				m_pEventCallback->OnError(NETEVENT_NET_IO_ERROR);
			}
			break;
		}

		if (selectReadWrite.Read_FD_ISSET(m_socket))
		{
			bufrecv.AllocWrite(RECV_BUFFER_SIZE, false);
			ssize_t recvSize = recv(m_socket, bufrecv.PosPtr(), RECV_BUFFER_SIZE, 0);
			if (0 == recvSize)
			{
				YouMe_LOG_Error(__XT("network disconnected"));
				m_connectStatus = STATUS_IO_ERROR;
				if (m_pEventCallback != NULL)
				{
					m_pEventCallback->OnError(NETEVENT_NET_DISCONNECTED);
				}
				break;
			}

			if (0 > recvSize && !IS_NOBLOCK_READ_ERRNO(socket_errno))
			{
				YouMe_LOG_Error(__XT("recv %d error:%d"), recvSize, socket_errno);
				if (m_pEventCallback != NULL)
				{
					m_pEventCallback->OnError(NETEVENT_NET_IO_ERROR);
				}
				break;
			}

			if (0 > recvSize)
			{
				recvSize = 0;
			}

			bufrecv.Length(bufrecv.Pos() + recvSize, bufrecv.Length() + recvSize);
			YouMe_LOG_Debug(__XT("socket len:%d, buff len:%d"), recvSize, bufrecv.Length());

			int ret = DealReceiveData(bufrecv);
			m_dispatchPacketWait.SetSignal();
			if (ret != 0)
			{
				break;
			}
		}

		if (selectReadWrite.Write_FD_ISSET(m_socket))
		{
			int sendSize = SendData();
			if (sendSize == 0 || (0 > sendSize && !IS_NOBLOCK_READ_ERRNO(socket_errno)))
			{
				YouMe_LOG_Error(__XT("send data error:%d"), socket_errno);
				if (m_pEventCallback != NULL)
				{
					m_pEventCallback->OnError(NETEVENT_NET_IO_ERROR);
				}
				break;
			}
		}
	}

	YouMe_LOG_Info(__XT("leave"));
}

int GetIPType()
{
#ifdef WIN32
	FIXED_INFO* pFixedInfo = (FIXED_INFO*)malloc(sizeof(FIXED_INFO));
	if (pFixedInfo == NULL)
	{
		return AF_INET;
	}
	ULONG ulOutBufLen = sizeof(FIXED_INFO);
	if (GetNetworkParams(pFixedInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(pFixedInfo);
		pFixedInfo = (FIXED_INFO *)malloc(ulOutBufLen);
		if (pFixedInfo == NULL)
		{
			return AF_INET;
		}
	}
	std::string strDNSAddr;
	if (GetNetworkParams(pFixedInfo, &ulOutBufLen) == NO_ERROR)
	{
		strDNSAddr = pFixedInfo->DnsServerList.IpAddress.String;
	}
	if (pFixedInfo)
	{
		free(pFixedInfo);
	}

	struct hostent* pHostent = gethostbyname(strDNSAddr.c_str());
	if (pHostent != NULL)
	{
		return pHostent->h_addrtype;
	}
	return AF_INET;
#else
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	struct addrinfo *result = NULL;
	int iRet = getaddrinfo("localhost", NULL, &hints, &result);
	if (iRet != 0)
	{
		if (nullptr != result) {
			freeaddrinfo(result);
		}
		return AF_INET;
	}
	int iFamily = result->ai_family;
	freeaddrinfo(result);
	return iFamily;
#endif // WIN32
}

SOCKET NetworkService::ConectServer(IPPortItem& addrItem)
{
	SOCKET socket = INVALID_SOCKET;
	std::vector<std::string> ipList;
	youmecommon::DNSUtil::Instance()->GetHostByName(addrItem.addr, ipList);
	for (std::vector<std::string>::const_iterator itr = ipList.begin(); itr != ipList.end(); ++itr)
	{
		socket = ConectServer(*itr, addrItem.port);
		if (socket != INVALID_SOCKET)
		{
			break;
		}
	}	
	return socket;
}

SOCKET NetworkService::ConectServer(const std::string& ip, unsigned short port)
{
	XString strIP = UTF8TOXString(ip);
	YouMe_LOG_Info(__XT("connect ip:%s port:%d"), strIP.c_str(), port);

	youmecommon::socket_address _addr = youmecommon::socket_address(ip.c_str(), port);
	int af = ((sockaddr*)&_addr)->sa_family;
	SOCKET socket_ = socket(af, SOCK_STREAM, IPPROTO_TCP);
	if (socket_ == INVALID_SOCKET)
	{
		YouMe_LOG_Error(__XT("m_socket errno=%d"), socket_errno);
		return INVALID_SOCKET;
	}

#ifdef _WIN32
	if (0 != socket_ipv6only(socket_, 0))
	{
		YouMe_LOG_Error(__XT("set ipv6only failed. error:%d"), socket_errno);
	}
#endif
	int ret = socket_set_nobio(socket_);
	if (ret != 0)
	{
		YouMe_LOG_Error(__XT("socket_set_nobio:%d, %d"), socket_errno, socket_strerror(socket_errno));
	}

	if (af == AF_INET)
	{
		//sockaddr_in addr = *(struct sockaddr_in*)(&_addr.address());
		//ret = ::connect(socket_, (sockaddr*)&addr, sizeof(addr));

		YouMe_LOG_Info(__XT("connect v4"));

		int ipType = GetIPType();

		if (AF_INET6 == ipType)
		{
			YouMe_LOG_Info(__XT("connect adapte"));

			struct addrinfo* addrinfoResult;
			struct addrinfo hints;
			memset(&hints, 0, sizeof(hints));
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			if (getaddrinfo(ip.c_str(), "http", &hints, &addrinfoResult) != 0)
			{
				YouMe_LOG_Debug(__XT("getaddrinfo failed"));
				return INVALID_SOCKET;
			}

			for (struct addrinfo* res = addrinfoResult; res; res = res->ai_next)
			{
				socket_ = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
				if (socket_ < 0)
				{
					freeaddrinfo(addrinfoResult);
					YouMe_LOG_Debug(__XT("getaddrinfo failed"));
					return INVALID_SOCKET;
				}
				ret = socket_set_nobio(socket_);
				if (ret != 0)
				{
					YouMe_LOG_Error(__XT("socket_set_nobio:%d, %d"), socket_errno, socket_strerror(socket_errno));
				}
				if (res->ai_family == AF_INET)
				{
					struct sockaddr_in addr_in = *((sockaddr_in*)res->ai_addr);
					addr_in.sin_port = htons(port);
					ret = connect(socket_, (sockaddr*)&addr_in, sizeof(sockaddr_in));
					YouMe_LOG_Debug(__XT("connected  AF_INET: %d"), ret);
				}
				else if (res->ai_family == AF_INET6)
				{
					struct sockaddr_in6 addr_in6 = *((sockaddr_in6*)res->ai_addr);
					addr_in6.sin6_port = htons(port);
					ret = connect(socket_, (sockaddr*)&addr_in6, sizeof(sockaddr_in6));
					YouMe_LOG_Debug(__XT("connected AF_INET6: %d"), ret);
				}
				else
				{
					ret = -2;
					YouMe_LOG_Debug(__XT("can not deal protocal"));
				}
				break;
			}
			freeaddrinfo(addrinfoResult);
		}
		else
		{
			sockaddr_in addr = *(struct sockaddr_in*)(&_addr.address());
			ret = ::connect(socket_, (sockaddr*)&addr, sizeof(addr));
		}

	}
	else if (af == AF_INET6)
	{
		YouMe_LOG_Info(__XT("connect v6"));

		sockaddr_in6 addr = *(struct sockaddr_in6*)(&_addr.address());
		ret = ::connect(socket_, (sockaddr*)&addr, sizeof(addr));
	}

	if (0 > ret && !IS_NOBLOCK_CONNECT_ERRNO(socket_errno))
	{
		YouMe_LOG_Error(__XT("connect errno=%d ret=%d"), socket_errno, ret);
		return INVALID_SOCKET;
	}

	youmecommon::SocketSelect select_connect(m_connectBreaker, true);
	select_connect.PreSelect();
	select_connect.Exception_FD_SET(socket_);
	select_connect.Write_FD_SET(socket_);

	int selectRet = select_connect.Select(LOGIN_CONNECT_TIMEOUT);
	if (0 == selectRet)
	{
		YouMe_LOG_Error(__XT("conenct failed errno=%d %d"), socket_errno, SOCKET_ERRNO(ETIMEDOUT));
		return INVALID_SOCKET;
	}

	if (selectRet < 0) {
		YouMe_LOG_Error(__XT("conenct failed errno=%d"), socket_errno);
		return INVALID_SOCKET;
	}

	if (select_connect.Exception_FD_ISSET(socket_)) {
		YouMe_LOG_Error(__XT("conenct failed errno=%d"), socket_error(socket_));
		return INVALID_SOCKET;
	}

	if (select_connect.Write_FD_ISSET(socket_) && 0 != socket_error(socket_)) {
		YouMe_LOG_Error(__XT("conenct failed errno=%d"), socket_error(socket_));
		return INVALID_SOCKET;
	}
    
    int so_error = 0;
    socklen_t len = sizeof(so_error);
    getsockopt(socket_, SOL_SOCKET, SO_ERROR, &so_error, &len);
    
    if( so_error == ECONNREFUSED){
        YouMe_LOG_Error(__XT("conenct refused errno=%d"), so_error);
        return INVALID_SOCKET;
    }

	m_address.addr = ip;
	m_address.port = port;

	m_strLocalIP = UTF8TOXString(std::string(youmecommon::socket_address::getsockname(socket_).ip()));
	YouMe_LOG_Info(__XT("connected local:%s"), m_strLocalIP.c_str());

	return socket_;
}

int NetworkService::DealReceiveData(youmecommon::AutoBuffer& buffer)
{
	int ret = 0;
	while (buffer.Length() > 0)
	{
		if (buffer.Length() < sizeof(SYoumeConnHead))
		{
			ret = 0;
			break;
		}

		SYoumeConnHead head;
		unsigned char key[PACKET_ENCRYT_KEY_LEN] = { 0 };
		memcpy(key, buffer.Ptr(), sizeof(SYoumeConnHead));
		memcpy(&head, buffer.Ptr(), sizeof(SYoumeConnHead));
		head.Unpack((unsigned char*)&head);
		if (head.m_usLen < sizeof(SYoumeConnHead))
		{
			YouMe_LOG_Warning(__XT("receive data size error command:%d serial:%llu packetSize:%u"), head.m_usCmd, head.m_ullReqSeq, head.m_usLen);
			ret = -1;
			break;
		}
		if (buffer.Length() < head.m_usLen)
		{
			ret = 0;
			break;
		}

		ResponsePacket responsePacket;

		{
			std::lock_guard<std::mutex> lock(*packtMutex);
			std::map<XUINT64, UDPTCPPacketInfo>::iterator itr = s_allPacketMap->find(head.m_ullReqSeq);
			if (itr != s_allPacketMap->end())
			{
				responsePacket.extend = itr->second.extend;
				s_allPacketMap->erase(itr);
			}
			else
			{
				if (!(head.m_usCmd >= YOUMEServiceProtocol::NOTIFY_LOGIN && head.m_usCmd < YOUMEServiceProtocol::NOTIFY_MAX_ID)
					&& head.m_usCmd != YOUMEServiceProtocol::CMD_KICK_OFF
					&& head.m_usCmd != YOUMEServiceProtocol::CMD_RELOGIN
					&& head.m_usCmd != YOUMEServiceProtocol::CMD_FRIND_NOTIFY
					&& head.m_usCmd != YOUMEServiceProtocol::CMD_HXR_USER_INFO_CHANGE_NOTIFY)
				{
					YouMe_LOG_Error(__XT("receive packet not found reqseq:%llu"), head.m_ullReqSeq);
					buffer.Move(-(int)(head.m_usLen));
					continue;
				}
			}
		}

		responsePacket.commondID = head.m_usCmd;
		responsePacket.reqSerial = head.m_ullReqSeq;
		int iPacketBodyLen = head.m_usLen - sizeof(SYoumeConnHead);
		if (iPacketBodyLen > 0)
		{
			responsePacket.buffer.Allocate(iPacketBodyLen);
			memcpy(responsePacket.buffer.Get(), buffer.Ptr(sizeof(SYoumeConnHead)), iPacketBodyLen);
			if (head.m_usCmd == YOUMEServiceProtocol::CMD_LOGIN || head.m_usCmd == YOUMEServiceProtocol::CMD_RELOGIN)
			{
				EncryDecryptPacketBody((unsigned char*)responsePacket.buffer.Get(), responsePacket.buffer.GetBufferLen(), key, sizeof(SYoumeConnHead));
			}
			else
			{
				EncryDataXOR(responsePacket.buffer.Get(), responsePacket.buffer.GetBufferLen());
			}
		}

		{
			std::lock_guard<std::mutex> lock(m_dispatchMutex);
			m_dispatchPacketList.push_back(responsePacket);
		}

		buffer.Move(-(int)(head.m_usLen));
	}

	return ret;
}

int NetworkService::SendData()
{
	XUINT64 ullSerial = 0;
	{
		std::lock_guard<std::mutex> lock(m_taskMutex);
		if (m_taskList.empty())
		{
			return 1;
		}
		std::list<Task>::iterator itr = m_taskList.begin();
		ullSerial = itr->taskID;
		m_taskList.erase(itr);
	}

	XINT64 nowTime = youmecommon::CTimeUtil::GetTimeOfDay_MS();
	UDPTCPPacketInfo packetInfo;
	{
		std::lock_guard<std::mutex> lock(*packtMutex);
		std::map<XUINT64, UDPTCPPacketInfo >::iterator itr = s_allPacketMap->find(ullSerial);
		if (itr == s_allPacketMap->end())
		{
			return 1;
		}
		itr->second.ulSendTime = nowTime;
		itr->second.retryTimes += 1;
		packetInfo = itr->second;
	}
	if (packetInfo.pPacket.Get() == NULL)
	{
		return 1;
	}

	youmecommon::CXSharedArray<char> buffer;
	EncryptData(packetInfo, ullSerial, buffer);

	int sendSize = send(m_socket, (const char*)buffer.Get(), buffer.GetBufferLen(), 0);

	if (YOUMEServiceProtocol::CMD_HEARTBEAT != packetInfo.commandType && YOUMEServiceProtocol::CMD_RELATION_CHAIN_HEARTBEAT != packetInfo.commandType)
	{
		YouMe_LOG_Info(__XT("send packet command:%d serial:%llu size:%d"), packetInfo.commandType, ullSerial, packetInfo.pPacket.GetBufferLen());
	}
	return sendSize;
}

void NetworkService::DispatchThreadProc()
{
	YouMe_LOG_Info(__XT("enter"));

	if (NULL == m_pEventCallback)
	{
		YouMe_LOG_Info(__XT("callback is null, thread exit"));
		return;
	}

	while (true)
	{
		m_dispatchPacketWait.WaitTime(DISPATCH_INTERVAL);

		if (!m_bDispatchRunning)
		{
			break;
		}

		XINT64 curTime = youmecommon::CTimeUtil::GetTimeOfDay_MS();
		{
			// 超时检查
			std::lock_guard<std::mutex> lock(*packtMutex);
			for (std::map<XUINT64, UDPTCPPacketInfo>::iterator itr = s_allPacketMap->begin(); itr != s_allPacketMap->end();)
			{
				if (itr->second.ulSendTime == 0)	//未发送
				{
					++itr;
					continue;
				}
				if (curTime - itr->second.ulSendTime >= itr->second.ulTimeout)
				{
					if (YOUMEServiceProtocol::CMD_HEARTBEAT == itr->second.commandType
						|| YOUMEServiceProtocol::CMD_LOGOUT == itr->second.commandType
						|| itr->second.retryTimes >= m_nRetrySendTimes)
					{
						ServerPacket serverPacket;
						serverPacket.commondID = itr->second.commandType;
						serverPacket.result = -1;
						serverPacket.reqSerial = itr->first;
						serverPacket.extend = itr->second.extend;

						m_pEventCallback->OnRecvPacket(serverPacket);

						itr = s_allPacketMap->erase(itr);
					}
					else
					{
						itr->second.ulSendTime = 0;

						Task task;
						task.taskID = itr->first;
						if (itr->second.commandType == YOUMEServiceProtocol::CMD_HEARTBEAT
							|| itr->second.commandType == YOUMEServiceProtocol::CMD_LOGOUT
							|| itr->second.commandType == YOUMEServiceProtocol::CMD_ENTER_ROOM
							|| itr->second.commandType == YOUMEServiceProtocol::CMD_LEAVE_ROOM
							|| itr->second.commandType == YOUMEServiceProtocol::CMD_RELOGIN
							|| itr->second.commandType == YOUMEServiceProtocol::CMD_LOGIN)
						{
							task.priority = TASKPRIORITY_HIGH;
						}
						{
							std::lock_guard<std::mutex> lock(m_taskMutex);
							m_taskList.push_front(task);
						}
						m_readWriteBreaker.Break();

						++itr;						
					}
				}
				else
				{
					++itr;
				}
			}
		}

		while (true)
		{
			ServerPacket serverPacket;
			{
				if (m_dispatchPacketList.empty())
				{
					break;
				}
				std::lock_guard<std::mutex> lock(m_dispatchMutex);
				std::list<ResponsePacket>::iterator itr = m_dispatchPacketList.begin();
				serverPacket.commondID = itr->commondID;
				serverPacket.result = itr->result;
				serverPacket.reqSerial = itr->reqSerial;
				serverPacket.packetBuffer = itr->buffer.Get();
				serverPacket.packetSize = itr->buffer.GetBufferLen();
				serverPacket.extend = itr->extend;
			}
			m_pEventCallback->OnRecvPacket(serverPacket);

			{
				std::lock_guard<std::mutex> lock(m_dispatchMutex);
				m_dispatchPacketList.pop_front();
				if (m_dispatchPacketList.empty())
				{
					break;
				}
			}
		}
	}

	YouMe_LOG_Info(__XT("leave"));
}
