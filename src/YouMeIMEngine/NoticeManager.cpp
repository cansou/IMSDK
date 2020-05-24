#include <YouMeIMEngine/NoticeManager.h>
#include <regex>
#include <YouMeCommon/Log.h>
#include <YouMeCommon/StringUtil.hpp>
#include <YouMeCommon/XFile.h>
#include <YouMeIMEngine/pb/youme_tips.pb.h>
#include <YouMeIMEngine/pb/youme_getmsg.pb.h>
#include <YouMeIMEngine/pb/server_protocol_code.h>
#include <YouMeIMEngine/YouMeIMManager.h>


#define NOTICE_TABLE_SQL __XT("create table %s(noticeID integer,noticeType integer,loopType integer,channelID text,loopWeekday text,content text,linkText text,linkAddress text,beginTime integer,endTime integer,extra blob);")

extern IYouMeSystemProvider* g_pSystemProvider;
extern int g_iYouMeIMMode;
extern CProfileDB* g_pProfileDB;


YIMNotice::~YIMNotice()
{
}

YIMNoticeImpl::YIMNoticeImpl() : m_uNoticeID(0), m_noticeType(0), m_uBeginTime(0), m_uEndTime(0), m_nLoopType(0)
{
}
YIMNoticeImpl::~YIMNoticeImpl()
{
}
XUINT64 YIMNoticeImpl::GetNoticeID()
{
	return m_uNoticeID;
}
const XCHAR* YIMNoticeImpl::GetChannelID()
{
	return m_strChannelID.c_str();
}
int YIMNoticeImpl::GetNoticeType()
{
	return m_noticeType;
}
const XCHAR* YIMNoticeImpl::GetContent()
{
	return m_strContent.c_str();
}
const XCHAR* YIMNoticeImpl::GetLinkText()
{
	return m_strLinkeText.c_str();
}
const XCHAR* YIMNoticeImpl::GetLinkAddr()
{
	return m_strLinkAddr.c_str();
}
unsigned int YIMNoticeImpl::GetBeginTime()
{
	return m_uBeginTime;
}
unsigned int YIMNoticeImpl::GetEndTime()
{
	return m_uEndTime;
}


NoticeManager::NoticeManager() : m_pCallback(NULL),
m_bInit(false),
m_ullMaxNoticeID(0)
{

}

NoticeManager::~NoticeManager()
{
	Uninit();
}

void NoticeManager::Init()
{
	if (m_bInit)
	{
		return;
	}
	XString strDBPath = youmecommon::CXFile::CombinePath(g_pSystemProvider->getDocumentPath(), __XT("youme_im_notice.db"));
	m_sqliteDb.Open(strDBPath.c_str());

	YouMeIMManager* pYouMeIMManager = (YouMeIMManager*)YouMeIMManager::CreateInstance();
	if (pYouMeIMManager != NULL)
	{
		XString strTableName = CStringUtilT<XCHAR>::formatString(__XT("tb_notice_%s"), pYouMeIMManager->GetCurrentUser().c_str());
		{
			std::lock_guard<std::mutex> lock(m_dbOperateMutuex);
			if (!m_sqliteDb.IsTableExist(strTableName))
			{
				youmecommon::CSqliteOperator sqliteOperator(m_sqliteDb);
				XString strSql = CStringUtilT<XCHAR>::formatString(NOTICE_TABLE_SQL, strTableName.c_str());
				sqliteOperator.PrepareSQL(strSql);
				bool bRet = sqliteOperator.Execute();
				if (!bRet)
				{
					YouMe_LOG_Error(__XT("create table failed"));
				}
			}
		}
	}

	m_bInit = true;
}

void NoticeManager::Uninit()
{
	if (!m_bInit)
	{
		return;
	}
	m_sqliteDb.Close();
	m_bInit = false;
	m_pCallback = NULL;
}

void NoticeManager::SetNoticeCallback(IYIMNoticeCallback* callback)
{
	m_pCallback = callback;
}

void NoticeManager::OnReceNoticeNotify(YOUMEServiceProtocol::NotifyReq& rsp)
{
	if (NULL == m_pCallback)
	{
		YouMe_LOG_Error(__XT("notice callback is null"));
	}

	XUINT64 ullNoticeID = 0;
	XUINT64 ullOldNoticeID = 0;
	int iLoopType = 0;
	if ((rsp.notify_property() & 4) != 0)	//撤销公告
	{
		XString strChannelID = UTF8TOXString(rsp.recver_id());
		for (int i = 0; i < rsp.notify_args_size(); ++i)
		{
			std::string strKey = rsp.notify_args(i).name();
			if ("notice_id" == strKey)
			{
				ullNoticeID = CStringUtilT<char>::str_to_uint64(rsp.notify_args(i).value());
			}
			else if ("notice_id_old" == strKey)
			{
				ullOldNoticeID = CStringUtilT<char>::str_to_uint64(rsp.notify_args(i).value());
			}
		}

		if (ullNoticeID != 0)
		{
			DeleteNotice(ullNoticeID);
		}
		if (ullOldNoticeID != 0)
		{
			DeleteNotice(ullOldNoticeID);
			m_pCallback->OnCancelNotice(ullOldNoticeID, strChannelID.c_str());
		}
	}
	else
	{
		std::shared_ptr<YIMNoticeImpl> pNotice = std::shared_ptr<YIMNoticeImpl>(new YIMNoticeImpl);
		pNotice->m_noticeType = rsp.notify_type();		// 公告类型 1 跑马灯，2 聊天框，3 置顶
		pNotice->m_strContent = UTF8TOXString(rsp.notify_msg());
		pNotice->m_strChannelID = UTF8TOXString(rsp.recver_id());
		for (int i = 0; i < rsp.notify_args_size(); ++i)
		{
			std::string strKey = rsp.notify_args(i).name();
			if ("notice_id" == strKey)
			{
				pNotice->m_uNoticeID = CStringUtilT<char>::str_to_uint64(rsp.notify_args(i).value());
				ullNoticeID = pNotice->m_uNoticeID;
			}
			if ("notice_id_old" == strKey)
			{
				ullOldNoticeID = CStringUtilT<char>::str_to_uint64(rsp.notify_args(i).value());
			}
			else if ("link_keywords" == strKey)
			{
				pNotice->m_strLinkeText = UTF8TOXString(rsp.notify_args(i).value());
			}
			else if ("link_addr" == strKey)
			{
				pNotice->m_strLinkAddr = UTF8TOXString(rsp.notify_args(i).value());
			}
			else if ("start_timestamp" == strKey)	//start_timestamp和end_timestamp置顶一次性公告才有，时间戳
			{
				pNotice->m_uBeginTime = CStringUtilT<char>::str_to_uint32(rsp.notify_args(i).value());
			}
			else if ("end_timestamp" == strKey)
			{
				pNotice->m_uEndTime = CStringUtilT<char>::str_to_uint32(rsp.notify_args(i).value());
			}
			else if ("elapsed_start" == strKey)	//elapsed_start和elapsed_end置顶周期性公告才有，相对于0点的秒数
			{
				pNotice->m_uBeginTime = CStringUtilT<char>::str_to_uint32(rsp.notify_args(i).value());
			}
			else if ("elapsed_end" == strKey)
			{
				pNotice->m_uEndTime = CStringUtilT<char>::str_to_uint32(rsp.notify_args(i).value());
			}
			else if ("send_weekday" == strKey)
			{
				pNotice->m_strLoopWeekday = UTF8TOXString(rsp.notify_args(i).value());
			}
			else if ("loop_type" == strKey)
			{
				pNotice->m_nLoopType = CStringUtilT<char>::str_to_sint32(rsp.notify_args(i).value());
				iLoopType = pNotice->m_nLoopType;
			}
		}

		if (m_ullMaxNoticeID < ullNoticeID)
		{
			m_ullMaxNoticeID = ullNoticeID;
			UpdateMaxNoticeID(m_ullMaxNoticeID);
		}

		if (rsp.notify_type() == 3)	// 置顶
		{
			AddNotice(pNotice.get());

			if (ullOldNoticeID != 0)	// 公告修改，开启等会产生新的noticeid
			{
				DeleteNotice(ullOldNoticeID);
			}

			if (2 == pNotice->m_nLoopType)
			{
				std::time_t currentZeroHour = GetCurrentZeroHourStamp();
				pNotice->m_uBeginTime += (unsigned int)currentZeroHour;
				pNotice->m_uEndTime += (unsigned int)currentZeroHour;
			}
		}

		m_pCallback->OnRecvNotice(pNotice.get());
	}

	YouMe_LOG_Debug(__XT("notice notify type:%d looptype:%d property:%d noticeID:%llu oldNoticeID:%llu"), rsp.notify_type(), iLoopType, rsp.notify_property(), ullNoticeID, ullOldNoticeID);
}

YIMErrorcode NoticeManager::QueryNotice(XUINT64 noticeID)
{

	YouMeIMManager* pYouMeIMManager = (YouMeIMManager*)YouMeIMManager::CreateInstance();
	if (pYouMeIMManager == NULL)
	{
		return YIMErrorcode_EngineNotInit;
	}

	if (m_ullMaxNoticeID == 0)
	{
		XString strID;
		std::string strKey = CStringUtilT<char>::formatString("NoticeID_%d_%s_%d", pYouMeIMManager->GetAppID(), XStringToUTF8(pYouMeIMManager->GetCurrentUser()).c_str(), g_iYouMeIMMode);
		g_pProfileDB->getSetting(strKey, strID);
		if (!strID.empty())
		{
			m_ullMaxNoticeID = CStringUtil::str_to_sint64(strID);
		}
	}

	YOUMEServiceProtocol::GetMsgReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_msgid(m_ullMaxNoticeID);
	std::string strData;
	req.SerializeToString(&strData);
	XUINT64 msgSerial = 0;
	std::map<std::string, std::string> extend;
	if (!pYouMeIMManager->SendData(YOUMEServiceProtocol::CMD_QUERY_NOTICE, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		YouMe_LOG_Error(__XT("send query notice failed"));
		return YIMErrorcode_NetError;
	}

	YouMe_LOG_Debug(__XT("QueryNotice id:%llu"), m_ullMaxNoticeID);

	return YIMErrorcode_Success;
}

void NoticeManager::OnQueryNoticeRsp(ServerPacket& serverPacket)
{
	if (NULL == m_pCallback)
	{
		YouMe_LOG_Error(__XT("notice callback is null"));
		return;
	}
	YouMeIMManager* pYouMeIMManager = (YouMeIMManager*)YouMeIMManager::CreateInstance();
	if (pYouMeIMManager == NULL || pYouMeIMManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return;
	}

	bool bRet = true;
	YOUMEServiceProtocol::GetTipsRsp rsp;
	do 
	{
		if (-1 == serverPacket.result)
		{
			YouMe_LOG_Error(__XT("OnQueryNoticeRsp timeout"));
			bRet = false;
			break;
		}
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("query notice unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
			bRet = false;
			break;
		}
		if (rsp.ret() != 0)
		{
			YouMe_LOG_Error(__XT("query notice error(%d)"), rsp.ret());
			bRet = false;
			break;
		}
	} while (0);
	
	if (!bRet)
	{
		m_deleteNoticeIDs.clear();
		QueryLocalNotice();
		for (std::vector<XUINT64>::const_iterator itr = m_deleteNoticeIDs.begin(); itr != m_deleteNoticeIDs.end(); ++itr)
		{
			DeleteNotice(*itr);
		}
		return;
	}

	YouMe_LOG_Debug(__XT("OnQueryNoticeRsp total:%d"), rsp.tips_list_size());

	XUINT64 ullMaxNoticeID = m_ullMaxNoticeID;
	for (int i = 0; i < rsp.tips_list_size(); ++i)
	{
		YOUMEServiceProtocol::AppTips tips = rsp.tips_list(i);

		if (1 == tips.enable())	// 停用
		{
			DeleteNotice(tips.old_id());
			YouMe_LOG_Debug(__XT("notice id:%llu type:%d status:%d"), tips.tips_id(), tips.tips_type(), tips.enable());
		}
		else
		{
			std::shared_ptr<YIMNoticeImpl> pNotice = std::shared_ptr<YIMNoticeImpl>(new YIMNoticeImpl);
			pNotice->m_uNoticeID = tips.tips_id();
			pNotice->m_noticeType = tips.tips_type();
			pNotice->m_strContent = UTF8TOXString(tips.content());
			pNotice->m_strChannelID = UTF8TOXString(tips.channel_id());
			pNotice->m_strLinkeText = UTF8TOXString(tips.link_keywords());
			pNotice->m_strLinkAddr = UTF8TOXString(tips.link_addr());
			pNotice->m_strLoopWeekday = UTF8TOXString(tips.loop_weekday());
			pNotice->m_nLoopType = tips.loop_type();

			if (3 == pNotice->m_noticeType)	// 置顶公告
			{
				if (2 == pNotice->m_nLoopType)	// 周期性
				{
					pNotice->m_uBeginTime = RelativeZeroHourSecond(tips.start_datetime());	//周期性公告起始结束时间为相对于0点的时间
					pNotice->m_uEndTime = RelativeZeroHourSecond(tips.end_datetime());
					AddNotice(pNotice.get());
				}
				else // 一次性公告
				{
					XINT64 currentTime = pYouMeIMManager->GetServerValidTime() / 1000;
					pNotice->m_uBeginTime = StringToTimestamp(tips.start_datetime());
					pNotice->m_uEndTime = StringToTimestamp(tips.end_datetime());
					if (currentTime < pNotice->m_uEndTime - 3)
					{
						AddNotice(pNotice.get());
					}
				}

				if (pNotice->m_uNoticeID != tips.old_id())	// 如果修改公告，会生成新的noticeid，删除旧的公告
				{
					DeleteNotice(tips.old_id());
				}
			}

			YouMe_LOG_Debug(__XT("notice id:%llu type:%d room:%s status:%d"), pNotice->GetNoticeID(), pNotice->GetNoticeType(), pNotice->GetChannelID(), tips.enable());
		}

		if (ullMaxNoticeID < tips.tips_id())
		{
			ullMaxNoticeID = tips.tips_id();
		}
	}

	if (ullMaxNoticeID != m_ullMaxNoticeID)
	{
		m_ullMaxNoticeID = ullMaxNoticeID;
		UpdateMaxNoticeID(m_ullMaxNoticeID);
	}

	if (rsp.tips_list_size() > 0)
	{
		QueryNotice(ullMaxNoticeID);
	}
	else
	{
		m_deleteNoticeIDs.clear();

		QueryLocalNotice();

		for (std::vector<XUINT64>::const_iterator itr = m_deleteNoticeIDs.begin(); itr != m_deleteNoticeIDs.end(); ++itr)
		{
			DeleteNotice(*itr);
		}
	}
}

bool NoticeManager::DeleteNotice(XUINT64 noticeID)
{
	YouMeIMManager* pYouMeIMManager = (YouMeIMManager*)YouMeIMManager::CreateInstance();
	if (pYouMeIMManager == NULL)
	{
		return false;
	}
	XString strSql = CStringUtilT<XCHAR>::formatString(__XT("delete from tb_notice_%s where noticeID=%llu"), pYouMeIMManager->GetCurrentUser().c_str(), noticeID);
	{
		std::lock_guard<std::mutex> lock(m_dbOperateMutuex);
		youmecommon::CSqliteOperator sqliteOperator(m_sqliteDb);
		sqliteOperator.PrepareSQL(strSql);
		bool ret = sqliteOperator.Execute();
		if (!ret)
		{
			YouMe_LOG_Error(__XT("delte notice failed(%llu)"), noticeID);
		}
		return ret;
	}
}

void NoticeManager::AddNotice(YIMNoticeImpl* notice)
{
	YouMeIMManager* pYouMeIMManager = (YouMeIMManager*)YouMeIMManager::CreateInstance();
	if (pYouMeIMManager == NULL || notice == NULL)
	{
		return;
	}

	int nCount = 0;
	XString strSql = CStringUtilT<XCHAR>::formatString(__XT("select count(noticeID) total from tb_notice_%s where noticeID=?1"), pYouMeIMManager->GetCurrentUser().c_str());
	{
		std::lock_guard<std::mutex> lock(m_dbOperateMutuex);
		youmecommon::CSqliteOperator sqliteOperator(m_sqliteDb);
		sqliteOperator.PrepareSQL(strSql);
		sqliteOperator << (long long)notice->m_uNoticeID;
		sqliteOperator.Execute();
		if (sqliteOperator.Next())
		{
			sqliteOperator >> nCount;
		}
	}
	if (nCount != 0)
	{
		return;
	}
	
	strSql = CStringUtilT<XCHAR>::formatString(__XT("insert into tb_notice_%s(noticeID,noticeType,loopType,channelID,loopWeekday,content,linkText,linkAddress,beginTime,endTime) values(?1,?2,?3,?4,?5,?6,?7,?8,?9,?10)"), pYouMeIMManager->GetCurrentUser().c_str());
	{
		std::lock_guard<std::mutex> lock(m_dbOperateMutuex);
		youmecommon::CSqliteOperator sqliteOperator(m_sqliteDb);
		sqliteOperator.PrepareSQL(strSql);
		sqliteOperator << (long long)notice->m_uNoticeID;
		sqliteOperator << notice->m_noticeType;
		sqliteOperator << notice->m_nLoopType;
		sqliteOperator << notice->m_strChannelID;
		sqliteOperator << notice->m_strLoopWeekday;
		sqliteOperator << notice->m_strContent;
		sqliteOperator << notice->m_strLinkeText;
		sqliteOperator << notice->m_strLinkAddr;
		sqliteOperator << (long long)notice->m_uBeginTime;
		sqliteOperator << (long long)notice->m_uEndTime;
		bool ret = sqliteOperator.Execute();
		if (!ret)
		{
			YouMe_LOG_Error(__XT("add notice failed(%llu)"), notice->m_uNoticeID);
		}
	}
}

void NoticeManager::QueryLocalNotice()
{
	if (NULL == m_pCallback)
	{
		YouMe_LOG_Error(__XT("notice callback is null"));
		return;
	}
	YouMeIMManager* pYouMeIMManager = (YouMeIMManager*)YouMeIMManager::CreateInstance();
	if (pYouMeIMManager == NULL)
	{
		return;
	}
	XString strSql = CStringUtilT<XCHAR>::formatString(__XT("select noticeID,noticeType,loopType,channelID,loopWeekday,content,linkText,linkAddress,beginTime,endTime from tb_notice_%s"), pYouMeIMManager->GetCurrentUser().c_str());
	std::list<std::shared_ptr<YIMNoticeImpl> > noticeList;
	std::lock_guard<std::mutex> lock(m_dbOperateMutuex);
	{
		youmecommon::CSqliteOperator sqliteOperator(m_sqliteDb);
		sqliteOperator.PrepareSQL(strSql);
		sqliteOperator << 0;
		sqliteOperator.Execute();

		while (sqliteOperator.Next())
		{
			long long noticeID = 0;
			int noticeType = 0;
			int loopType = 0;
			XString channelID;
			XString loopWeekday;
			XString content;
			XString linkeText;
			XString linkAddress;
			long long beginTime;
			long long endTime;
			sqliteOperator >> noticeID >> noticeType >> loopType >> channelID >> loopWeekday >> content >> linkeText >> linkAddress >> beginTime >> endTime;

			if (1 == loopType)
			{
				// 置顶一次性公告
				XINT64 currentTime = pYouMeIMManager->GetServerValidTime() / 1000;
				if (currentTime < beginTime)
				{
					continue;
				}
				if (currentTime > endTime - 3)
				{
					m_deleteNoticeIDs.push_back(noticeID);
					continue;
				}
			}
			else
			{
				// 置顶周期性公告
				std::vector<XString> weekdays;
				CStringUtilT<XCHAR>::splitString(loopWeekday, __XT(","), weekdays);
				std::time_t currentTime = pYouMeIMManager->GetServerValidTime() / 1000;
                struct tm current_tm;
                youmecommon::CTimeUtil::GetLocalTime(&currentTime, &current_tm);
                
				int weekday = current_tm.tm_wday == 0 ? 7 : current_tm.tm_wday;
				XString strWeekday = CStringUtilT<XCHAR>::to_string(weekday);
				if (std::find(weekdays.begin(), weekdays.end(), strWeekday) == weekdays.end())
				{
					continue;;
				}
				int currentSecond = current_tm.tm_hour * 3600 + current_tm.tm_min * 60 + current_tm.tm_sec;
				if (currentSecond <= beginTime || currentSecond >= endTime)
				{
					continue;
				}

				std::time_t currentZeroHour = GetCurrentZeroHourStamp();
				beginTime = currentZeroHour + beginTime;
				endTime = currentZeroHour + endTime;
			}

			std::shared_ptr<YIMNoticeImpl> pNotice = std::shared_ptr<YIMNoticeImpl>(new YIMNoticeImpl);
			pNotice->m_uNoticeID = noticeID;
			pNotice->m_noticeType = noticeType;
			pNotice->m_strContent = content;
			pNotice->m_strChannelID = channelID;
			pNotice->m_strLinkeText = linkeText;
			pNotice->m_strLinkAddr = linkAddress;
			pNotice->m_uBeginTime = (unsigned int)beginTime;
			pNotice->m_uEndTime = (unsigned int)endTime;
			noticeList.push_back(pNotice);
		}
	}

	for (std::list<std::shared_ptr<YIMNoticeImpl> >::const_iterator itr = noticeList.begin(); itr != noticeList.end(); ++itr)
	{
		m_pCallback->OnRecvNotice(itr->get());

		YouMe_LOG_Debug(__XT("noticeID:%llu channel:%s begin:%u end:%u"), (*itr)->GetNoticeID(), (*itr)->GetChannelID(), (*itr)->GetBeginTime(), (*itr)->GetEndTime());
	}
}

unsigned int NoticeManager::RelativeZeroHourSecond(std::string time)
{
	std::regex pattern(".* (\\d+):(\\d+):(\\d+).*");
	std::match_results<std::string::const_iterator> result;
	bool bRet = std::regex_match(time, result, pattern);
	if (!bRet || result.size() != 4)
	{
		return 0;
	}

	return atoi(result.str(1).c_str()) * 3600 + atoi(result.str(2).c_str()) * 60 + atoi(result.str(3).c_str());
}

unsigned int NoticeManager::StringToTimestamp(std::string time)
{
	std::regex pattern("(\\d{4})-(\\d{2})-(\\d{2}) (\\d+):(\\d+):(\\d+).*");
	std::match_results<std::string::const_iterator> result;
	bool bRet = std::regex_match(time, result, pattern);
	if (!bRet || result.size() != 7)
	{
		return 0;
	}

	if (atoi(result.str(1).c_str()) < 1900)
	{
		return 0;
	}

	std::tm t;
	t.tm_year = atoi(result.str(1).c_str()) - 1900;
	t.tm_mon = atoi(result.str(2).c_str()) - 1;
	t.tm_mday = atoi(result.str(3).c_str());
	t.tm_hour = atoi(result.str(4).c_str());
	t.tm_min = atoi(result.str(5).c_str());
	t.tm_sec = atoi(result.str(6).c_str());

	return (unsigned int )std::mktime(&t);
}

void NoticeManager::UpdateMaxNoticeID(XUINT64 noticeID)
{
	YouMeIMManager* pYouMeIMManager = (YouMeIMManager*)YouMeIMManager::CreateInstance();
	if (pYouMeIMManager == NULL || g_pProfileDB == NULL)
	{
		return;
	}

	std::string strNoticeID = CStringUtilT<char>::to_string(noticeID);
	XString strID = UTF8TOXString(strNoticeID);
	std::string strKey = CStringUtilT<char>::formatString("NoticeID_%d_%s_%d", pYouMeIMManager->GetAppID(), XStringToUTF8(pYouMeIMManager->GetCurrentUser()).c_str(), g_iYouMeIMMode);
	bool bRet = g_pProfileDB->setSetting(strKey, strID);
	if (!bRet)
	{
		YouMe_LOG_Error(__XT("update message serial failed"));
	}
}

std::time_t NoticeManager::GetCurrentZeroHourStamp()
{
	YouMeIMManager* pYouMeIMManager = (YouMeIMManager*)YouMeIMManager::CreateInstance();
	if (pYouMeIMManager == NULL)
	{
		return 0;
	}

	std::time_t currentTime = pYouMeIMManager->GetServerValidTime() / 1000;
    struct tm current_tm;
    youmecommon::CTimeUtil::GetLocalTime(&currentTime, &current_tm);

	current_tm.tm_hour = 0;
	current_tm.tm_min = 0;
	current_tm.tm_sec = 0;
	return std::mktime( &current_tm);
}
