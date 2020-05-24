#include "XUDP.h"
#include <assert.h>
#include <time.h>
#include <YouMeCommon/Log.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <YouMeCommon/TimeUtil.h>
#include <YouMeCommon/StringUtil.hpp>
#include <YouMeIMEngine/YouMeIMManager.h>


std::mutex* _youmeim_m_mutex=new std::mutex;
//std::map<XUINT64, UDPTCPPacketInfo> m_needSendPacketMap;	//保存需要发送的包
//std::map<XUINT64, UDPTCPPacketInfo> m_allPacketMap;			//保存所有待发送的包

static std::map<XUINT64, UDPTCPPacketInfo>* m_needSendPacketMap = new std::map<XUINT64, UDPTCPPacketInfo>;            //保存所有待发送的包
static std::map<XUINT64, UDPTCPPacketInfo>* m_allPacketMap = new std::map<XUINT64, UDPTCPPacketInfo>;            //保存所有待发送的包


int CXUDP::m_nRetrySendTimes = 2;

CXUDP::CXUDP(YouMeIMManager* pIManager)
{
	m_bInit = false;
	m_bSendThreadExit = false;
	m_bRecvThreadExit = false;
	m_pIManager = pIManager;
	m_uiServiceID = 0;
    m_pPacketCallback = NULL;
}

CXUDP::~CXUDP()
{
    YouMe_LOG_Info(__XT("Enter"));
	UnInit();
    YouMe_LOG_Info(__XT("Leave"));
}

//需要判断wifi 和 3G 网卡，绑定ip 地址优先wifi
bool CXUDP::Init(const XString&strServerIP, int iPort, IPacketCallback* pCallback)
{
	YouMe_LOG_Info(__XT("Enter m_binit:%d"), (int)m_bInit);
	if (m_bInit)
	{
		return m_bInit;
	}
	m_pPacketCallback = pCallback;
	m_bSendThreadExit = false;
	m_client.Init(strServerIP, iPort);
	if (!m_client.Connect(5))
	{
		YouMe_LOG_Error(__XT("connect error"));
		return false;
	}
	m_sendThread = std::thread(&CXUDP::SendThread, this);
	//一开始设置位有信号

	m_bInit = true;
	YouMe_LOG_Info(__XT("Leave"));
	return true;
}

void CXUDP::UnInit()
{
	YouMe_LOG_Info(__XT("Enter m_bInit:%d"), (int)m_bInit);
	if (!m_bInit)
	{
		return;
	}
	m_bInit = false;
	m_bSendThreadExit = true;
	m_sendCondWait.Increment();
	m_pPacketCallback = NULL;

	
	if (m_sendThread.joinable())
	{
		m_sendThread.join();
	}
	YouMe_LOG_Info(__XT("send thread exit"));
	if (m_recvThread.joinable())
	{
		m_recvThread.join();
	}
	m_client.UnInit();

	std::map<XUINT64, UDPTCPPacketInfo> tempPackets;
	for (std::map<XUINT64, UDPTCPPacketInfo >::iterator itr = m_allPacketMap->begin(); itr != m_allPacketMap->end(); ++itr)
	{
		if (itr->second.commandType == YOUMEServiceProtocol::CMD_SND_TEXT_MSG ||
			itr->second.commandType == YOUMEServiceProtocol::CMD_SND_VOICE_MSG ||
			itr->second.commandType == YOUMEServiceProtocol::CMD_SND_FILE_MSG ||
			itr->second.commandType == YOUMEServiceProtocol::CMD_SND_BIN_MSG ||
			itr->second.commandType == YOUMEServiceProtocol::CMD_SND_GIFT_MSG)
		{
			tempPackets[itr->first] = itr->second;
		}
	}
	m_allPacketMap->clear();

	{
		std::lock_guard<std::mutex> lock(*_youmeim_m_mutex);
		for (std::map<XUINT64, UDPTCPPacketInfo >::iterator itr = m_needSendPacketMap->begin(); itr != m_needSendPacketMap->end();)
		{
			if (itr->second.commandType == YOUMEServiceProtocol::CMD_SND_TEXT_MSG ||
				itr->second.commandType == YOUMEServiceProtocol::CMD_SND_VOICE_MSG ||
				itr->second.commandType == YOUMEServiceProtocol::CMD_SND_FILE_MSG ||
				itr->second.commandType == YOUMEServiceProtocol::CMD_SND_BIN_MSG ||
				itr->second.commandType == YOUMEServiceProtocol::CMD_SND_GIFT_MSG)
			{
				itr->second.ulSendTime = 0;
				itr->second.retryTimes = 0;
				(*m_allPacketMap)[itr->first] = itr->second;
			}
			itr = m_needSendPacketMap->erase(itr);
		}
	}
	if (tempPackets.size() > 0)
	{
		m_allPacketMap->insert(tempPackets.begin(), tempPackets.end());
	}

	YouMe_LOG_Info(__XT("Leave"));
}

bool CXUDP::DealRead()
{
    /*http://linux.die.net/man/2/recvfrom
     All three routines return the length of the message on successful completion. If a message is too long to fit in the supplied buffer, excess bytes may be discarded depending on the type of socket the message is received from
     */
	youmecommon::CXSharedArray<char> headBuf;
	m_client.RecvDataByLen(sizeof(SYoumeConnHead), headBuf);
	SYoumeConnHead *pConnHead = (SYoumeConnHead*)headBuf.Get();
	unsigned char key[PACKET_ENCRYT_KEY_LEN] = { 0 };
	memcpy(key, headBuf.Get(), sizeof(SYoumeConnHead));
	pConnHead->Unpack((unsigned char*)headBuf.Get());
	if (pConnHead->m_usLen < sizeof(SYoumeConnHead))
	{
		XString strHead = CStringUtil::bytes_to_hex_string((unsigned char*)headBuf.Get(), headBuf.GetBufferLen());
		YouMe_LOG_Warning(__XT("receive data size error command:%d serial:%llu packetSize:%u data:%s"), pConnHead->m_usCmd, pConnHead->m_ullReqSeq, pConnHead->m_usLen, strHead.c_str());
		return false;
	}

	ServerPacket serverPacket;
	serverPacket.commondID = pConnHead->m_usCmd;
	serverPacket.reqSerial = pConnHead->m_ullReqSeq;

	youmecommon::CXSharedArray<char> packetBodyBuffer;
	int iPacketBodyLen = pConnHead->m_usLen - sizeof(SYoumeConnHead);
	if (iPacketBodyLen > 0)
	{
		int nRecvLen = m_client.RecvDataByLen(iPacketBodyLen, packetBodyBuffer);
		if (nRecvLen != iPacketBodyLen)
		{
			YouMe_LOG_Warning(__XT("receive packet uncomplete command:%d serial:%llu packetSize:%u receive:%u"), pConnHead->m_usCmd, pConnHead->m_ullReqSeq, pConnHead->m_usLen, nRecvLen);
			return false;
		}

		if (pConnHead->m_usCmd == YOUMEServiceProtocol::CMD_LOGIN || pConnHead->m_usCmd == YOUMEServiceProtocol::CMD_RELOGIN)
		{
			EncryDecryptPacketBody((unsigned char*)packetBodyBuffer.Get(), packetBodyBuffer.GetBufferLen(), key, sizeof(SYoumeConnHead));
			serverPacket.packetBuffer = packetBodyBuffer.Get();
			serverPacket.packetSize = packetBodyBuffer.GetBufferLen();
		}
		else
		{
			EncryDataXOR(packetBodyBuffer.Get(), packetBodyBuffer.GetBufferLen());
			serverPacket.packetBuffer = packetBodyBuffer.Get();
			serverPacket.packetSize = packetBodyBuffer.GetBufferLen();
		}
	}

	{
		std::lock_guard<std::mutex> lock(*_youmeim_m_mutex);
		std::map<XUINT64, UDPTCPPacketInfo>::iterator itr = m_needSendPacketMap->find(pConnHead->m_ullReqSeq);
		if (itr != m_needSendPacketMap->end())
		{
			serverPacket.extend = itr->second.extend;
			m_needSendPacketMap->erase(itr);
		}
		if (m_needSendPacketMap->size() == 0)
		{
			ConvertMsgPacket();
		}
	}

	if (NULL != m_pPacketCallback)
	{
		m_pPacketCallback->OnRecvPacket(serverPacket);
	}
	return true;
}

void CXUDP::SendThread()
{	
	YouMe_LOG_Info(__XT("Enter"));
	/*if (!m_client.Connect())
	{
		YouMe_LOG_Error(__XT("connect error"));
		return;
	}*/
	m_recvThread = std::thread(&CXUDP::UDPRecvThread, this);
	fd_set  send_set;
	while (true)
	{
		if (m_bSendThreadExit)
		{
			break;
		}
		struct timeval  tv;
		tv.tv_sec = 0;
		tv.tv_usec = 200 * 1000;

		FD_ZERO(&send_set);
		FD_SET(m_client.RawSocket(), &send_set);

		// http://linux.die.net/man/2/select
		// https://msdn.microsoft.com/en-us/library/windows/desktop/ms740141
		// Return Value
		// On success, select() and pselect() return the number of file descriptors contained in the three returned descriptor sets (that is, the total number of bits that are set in readfds, writefds, exceptfds) which may be zero if the timeout expires before anything interesting happens. On error, -1 is returned, and errno is set appropriately; the sets and timeout become undefined, so do not rely on their contents after an error.
		int ret = select(m_client.RawSocket() + 1,NULL, &send_set, NULL, &tv);	//类unix 系统中，第一个参数必须是client+1
		if (-1 == ret)
		{
			break;	//socket 错误直接退出，可以通过关闭socket 来达到退出目的
		}
		else if (0 == ret)
		{
			continue;	//超时
		}
		if (FD_ISSET(m_client.RawSocket(), &send_set))
		{
			DealSend();
		}
	}
	YouMe_LOG_Info(__XT("send thread exit"));
}

bool CXUDP::DealSend()
{
	//等待 当然，如果有需要发送的数据，上层会直接给个信号过来，然后就立即处理发送请求
	m_sendCondWait.Decrement();
	if (m_bSendThreadExit)
	{
		return true;
	}
	
	XINT64 nowUnixTime = youmecommon::CTimeUtil::GetTimeOfDay_MS();
	XUINT64 ullSerial = 0;
	UDPTCPPacketInfo packetInfo;
	{
		std::lock_guard<std::mutex> lock(*_youmeim_m_mutex);
		std::map<XUINT64, UDPTCPPacketInfo >::iterator begin = m_needSendPacketMap->begin();
		for (; begin != m_needSendPacketMap->end(); ++begin)
		{
			if (begin->second.ulSendTime == 0)
			{
				begin->second.ulSendTime = nowUnixTime;
				begin->second.retryTimes += 1;
				packetInfo = begin->second;
				ullSerial = begin->first;
				break;
			}
		}
	}
	if (packetInfo.pPacket.Get() == NULL)
	{
		return false;
	}

	youmecommon::CXSharedArray<char> buffer;
	EncryptData(packetInfo, ullSerial, buffer);

	m_client.SendBufferData((const char*)buffer.Get(), buffer.GetBufferLen());

	if (YOUMEServiceProtocol::CMD_HEARTBEAT != packetInfo.commandType)
	{
		YouMe_LOG_Info(__XT("send packet command:%d serial:%llu size:%d"), packetInfo.commandType, ullSerial, packetInfo.pPacket.GetBufferLen());
	}

	return true;
}
//外部统一加锁
void CXUDP::ConvertMsgPacket()
{
	//取出一个来放到发送队列
	std::map<XUINT64, UDPTCPPacketInfo >::iterator begin = m_allPacketMap->begin();
	for (; begin != m_allPacketMap->end(); ++begin)
	{
		if (begin->second.ulSendTime == 0)
		{
			(*m_needSendPacketMap)[begin->first] = begin->second;
			m_allPacketMap->erase(begin);
			m_sendCondWait.Increment();
			break;
		}
	}
}

void CXUDP::UDPRecvThread()
{
	YouMe_LOG_Info(__XT("Enter"));
	fd_set  read_set;
	while (true)
	{
		if (m_bSendThreadExit)
		{
			break;
		}
		struct timeval  tv;
		tv.tv_sec = 0;
		tv.tv_usec = 200 * 1000;
		FD_ZERO(&read_set);
		FD_SET(m_client.RawSocket(), &read_set);
		// http://linux.die.net/man/2/select
		// https://msdn.microsoft.com/en-us/library/windows/desktop/ms740141
		// Return Value
		// On success, select() and pselect() return the number of file descriptors contained in the three returned descriptor sets (that is, the total number of bits that are set in readfds, writefds, exceptfds) which may be zero if the timeout expires before anything interesting happens. On error, -1 is returned, and errno is set appropriately; the sets and timeout become undefined, so do not rely on their contents after an error.
		int ret = select(m_client.RawSocket() + 1, &read_set, NULL, NULL, &tv);	//类unix 系统中，第一个参数必须是client+1
		if (-1 == ret)
		{
			
			break;	//socket 错误直接退出，可以通过关闭socket 来达到退出目的
		}
		//判断一下有没有因为服务器没有回业务包超时的
		
		{
			XINT64 curTime = youmecommon::CTimeUtil::GetTimeOfDay_MS();
			std::lock_guard<std::mutex> lock(*_youmeim_m_mutex);
			for (std::map<XUINT64, UDPTCPPacketInfo>::iterator itr = m_needSendPacketMap->begin(); itr != m_needSendPacketMap->end();)
			{
				if (itr->second.ulSendTime == 0)	//未发送
				{
					++itr;
					continue;
				}
				if ((curTime - itr->second.ulSendTime) >= itr->second.ulTimeout)
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
						if (NULL != m_pPacketCallback)
						{
							m_pPacketCallback->OnRecvPacket(serverPacket);
						}

						itr = m_needSendPacketMap->erase(itr);
					}
					else
					{
						itr->second.ulSendTime = 0;
						m_sendCondWait.Increment();
						++itr;
					}
				}
				else
				{
					++itr;
				}
			}
			if (m_needSendPacketMap->size() == 0)
			{

				ConvertMsgPacket();
			}
		}

		
		if (0 == ret)	//超时
		{
			continue;
		}
		//其他情况表示已经触发了可用的条件
		if (FD_ISSET(m_client.RawSocket(), &read_set))
		{
			//设置信号，可以继续发消息
			if (!DealRead())
			{
				//通知一遍所有的消息，超时，并且清空
				m_bRecvThreadExit = true;
				/*if (NULL != m_pPacketCallback)
				{
					std::lock_guard<std::mutex> lock(_youmeim_m_mutex);
					for (std::map<XUINT64, UDPTCPPacketInfo>::iterator itr = m_needSendPacketMap->begin(); itr != m_needSendPacketMap->end(); ++itr)
					{
						ServerPacket serverPacket;
						serverPacket.commondID = itr->second.commandType;
						serverPacket.result = -1;
						serverPacket.reqSerial = itr->first;
						serverPacket.extend = itr->second.extend;
						m_pPacketCallback->OnRecvPacket(serverPacket);	//超时
					}
					m_needSendPacketMap->clear();
				}*/
				if (!m_bSendThreadExit)
				{
					//启动一次重连
					YouMe_LOG_Info(__XT("tcp recv failed start reconnect"));
					m_pIManager->ReconectProc(REASON_NETWORK_ERROR);
				}
				YouMe_LOG_Info(__XT("receive data failed exit receive thread"));
				break;
			}
		}

	}
	m_bRecvThreadExit = true;
	YouMe_LOG_Info(__XT("Leave"));
}

void CXUDP::CleanupPacket()
{
	std::lock_guard<std::mutex> lock(*_youmeim_m_mutex);
	for (std::map<XUINT64, UDPTCPPacketInfo>::iterator itr = m_needSendPacketMap->begin(); itr != m_needSendPacketMap->end(); ++itr)
	{
		ServerPacket serverPacket;
		serverPacket.commondID = itr->second.commandType;
		serverPacket.result = -1;
		serverPacket.reqSerial = itr->first;
		serverPacket.extend = itr->second.extend;
		m_pPacketCallback->OnRecvPacket(serverPacket);
	}
	m_needSendPacketMap->clear();

	for (std::map<XUINT64, UDPTCPPacketInfo>::iterator itr = m_allPacketMap->begin(); itr != m_allPacketMap->end(); ++itr)
	{
		ServerPacket serverPacket;
		serverPacket.commondID = itr->second.commandType;
		serverPacket.result = -1;
		serverPacket.reqSerial = itr->first;
		serverPacket.extend = itr->second.extend;
		m_pPacketCallback->OnRecvPacket(serverPacket);
	}
	m_allPacketMap->clear();
}

void CXUDP::SetRetrySendTime(int retrySendTimes)
{
	m_nRetrySendTimes = retrySendTimes;
}

void CXUDP::SetSessionInfo(XString& userID, unsigned int serviceID, youmecommon::CXSharedArray<unsigned char>& secret)
{
	m_strCurrentUserID = XStringToUTF8(userID);
	m_uiServiceID = serviceID;
	if (secret.Get() != NULL)
	{
		m_pSecretbuffer.Allocate(secret.GetBufferLen());
		memcpy(m_pSecretbuffer.Get(), secret.Get(), secret.GetBufferLen());
	}
}

bool CXUDP::SendData(int commandType, const char* buffer, int bufferSize, std::map<std::string, std::string>& extend, XUINT64 serial, bool secretEncrypt, XINT64 timeout)
{
	if (m_bRecvThreadExit)
	{
		YouMe_LOG_Warning(__XT("recv thread has exit"));
		return false;
	}

	UDPTCPPacketInfo info;
	info.commandType = commandType;
	info.ulTimeout = timeout;
	info.extend = extend;
	info.pPacket = youmecommon::CXSharedArray<char>(bufferSize);
	memcpy(info.pPacket.Get(), buffer, bufferSize);
	info.encryptFlag = secretEncrypt;

	//心跳包和登出包直接放需要发送的队列
	if ((m_needSendPacketMap->size() == 0)
		|| (commandType == YOUMEServiceProtocol::CMD_HEARTBEAT)
		|| (commandType == YOUMEServiceProtocol::CMD_LOGOUT)
		|| (commandType == YOUMEServiceProtocol::CMD_ENTER_ROOM)
		|| (commandType == YOUMEServiceProtocol::CMD_LEAVE_ROOM)
		|| (commandType == YOUMEServiceProtocol::CMD_RELOGIN)
		|| (commandType == YOUMEServiceProtocol::CMD_LOGIN))
	{
		std::lock_guard<std::mutex> lock(*_youmeim_m_mutex);
		(*m_needSendPacketMap)[serial] = info;
		m_sendCondWait.Increment();
	}
	else
	{
		//放到待发送队列
		std::lock_guard<std::mutex> lock(*_youmeim_m_mutex);
		(*m_allPacketMap)[serial] = info;
	}

	return true;
}

void CXUDP::EncryDataXOR(char* pBuffer, int iBufferLen)
{
	if (m_pSecretbuffer.GetBufferLen() == 0)
	{
		return;
	}
	int j = 0;
	for (int i = 0; i < iBufferLen; i++)
	{
		pBuffer[i] ^= m_pSecretbuffer.Get()[j % m_pSecretbuffer.GetBufferLen()];
		j++;
	}
}

void CXUDP::EncryDecryptPacketBody(unsigned char* buffer, int bufferLen, unsigned char* key, int keyLen)
{
	for (int i = 0; i < bufferLen; ++i)
	{
		buffer[i] ^= key[i % keyLen];
	}
}

void CXUDP::EncryptData(UDPTCPPacketInfo& packetInfo, XUINT64 serial, youmecommon::CXSharedArray<char>& buffer)
{
	unsigned int nReqLen = 0;
	unsigned int nUserIDBeginPos = 0;
	int nUserIDLen = (int)m_strCurrentUserID.size();

	SYoumeConnHead connHead;
	connHead.m_usCmd = packetInfo.commandType;
	connHead.m_ullReqSeq = serial;
	connHead.m_uiServiceId = m_uiServiceID;

	buffer.Allocate(sizeof(SYoumeConnHead) + sizeof(unsigned char) + nUserIDLen + packetInfo.pPacket.GetBufferLen());
	memcpy(buffer.Get(), &connHead, sizeof(connHead));
	nReqLen += sizeof(connHead);

	*(buffer.Get() + nReqLen) = (unsigned char)nUserIDLen;
	nReqLen += sizeof(unsigned char);
	nUserIDBeginPos = nReqLen;
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
