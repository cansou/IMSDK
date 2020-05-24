#include <YouMeIMEngine/MessageRecorder.h>
#include <YouMeCommon/XFile.h>
#include <YouMeCommon/CrossPlatformDefine/IYouMeSystemProvider.h>
#include <YouMeCommon/StringUtil.hpp>
#include <YouMeCommon/pb/youme_comm.pb.h>
#include <YouMeCommon/XSharedArray.h>
#include <YouMeIMEngine/YouMeIMManager.h>
#include <YouMeIMEngine/YouMeIMMessageManager.h>
#include <algorithm>


//#define MESSAGE_TABLE_SQL __XT("create table %s(id integer primary key,messageID integer,messageType integer,senderID text,receiverID text,chatType integer,createTime integer,content text,localPath text,extra blob);")
//#define MESSAGE_TABLE_SQL_V2 __XT("create table %s(id integer primary key,messageID integer,messageType integer,senderID text,receiverID text,chatType integer,createTime integer,content text,localPath text,extra blob,isRead integer);")
#define MESSAGE_TABLE_SQL_V3 __XT("create table %s(id integer primary key,messageID integer,messageType integer,senderID text,receiverID text,chatType integer,createTime integer,content text,localPath text,extra blob,isRead integer,isPlayed integer);")
#define MESSAGE_TABLE_SQL_V3_index __XT("CREATE INDEX IF NOT EXISTS messageID_idx_%s ON %s(messageID);")
#define MESSAGE_TABLE_SQL_V3_index2 __XT("CREATE INDEX IF NOT EXISTS createTime_idx_%s ON %s(createTime);")
//#define MESSAGE_TABLE_SQL_V3_index3 __XT("CREATE INDEX IF NOT EXISTS userid_idx_%s ON %s(senderID, receiverID);")
//#define MESSAGE_TABLE_SQL_V3_index4 __XT("CREATE UNIQUE INDEX IF NOT EXISTS uniq_idx_%s ON %s(senderID, createTime);")
#define MESSAGE_TABLE_SQL_V3_index4 __XT("DROP INDEX uniq_idx_%s;")
#define DEFAULT_ROW_NUM 30
#define MAX_ROW_NUM 100
#define MAX_CONTACT_NUM 100

extern IYouMeSystemProvider* g_pSystemProvider;


MessageRecorder::MessageRecorder() : m_bInit(false)
, m_bExit(false)
{

}

MessageRecorder::~MessageRecorder()
{
	UnInit();
}

void MessageRecorder::Init()
{
	if (m_bInit)
	{
		return;
	}
	XString strDBPath = youmecommon::CXFile::CombinePath(g_pSystemProvider->getDocumentPath(), __XT("youme_im_message.db"));
	m_sqliteDb.Open(strDBPath.c_str());

	YouMeIMManager* pYouMeIMManager = (YouMeIMManager*)YouMeIMManager::CreateInstance();
	if (NULL == pYouMeIMManager || pYouMeIMManager->GetCurrentUser().empty())
	{
		YouMe_LOG_Error(__XT("init failed"));
		return;
	}
	
	XString strTableName = CStringUtilT<XCHAR>::formatString(__XT("tb_message_%s"), pYouMeIMManager->GetCurrentUser().c_str());
    XString strUserID =  pYouMeIMManager->GetCurrentUser();
	{
		std::lock_guard<std::mutex> lock(m_dbOperateMutuex);
		if (!m_sqliteDb.IsTableExist(strTableName))
		{
			youmecommon::CSqliteOperator sqliteOperator(m_sqliteDb);
			XString strSql = CStringUtilT<XCHAR>::formatString(MESSAGE_TABLE_SQL_V3, strTableName.c_str());
			sqliteOperator.PrepareSQL(strSql);
			bool bRet = sqliteOperator.Execute();
			if (!bRet)
			{
				YouMe_LOG_Error(__XT("create table failed"));
				return;
			}

            strSql = CStringUtilT<XCHAR>::formatString(MESSAGE_TABLE_SQL_V3_index,strUserID.c_str(), strTableName.c_str());
            sqliteOperator.PrepareSQL(strSql);
            bRet = sqliteOperator.Execute();
            if (!bRet)
            {
                YouMe_LOG_Error(__XT("create table index failed"));
            }

            strSql = CStringUtilT<XCHAR>::formatString(MESSAGE_TABLE_SQL_V3_index2, strUserID.c_str(), strTableName.c_str());
            sqliteOperator.PrepareSQL(strSql);
            bRet = sqliteOperator.Execute();
            if (!bRet)
            {
                YouMe_LOG_Error(__XT("create table index2 failed"));
            }
            /*
            strSql = CStringUtilT<XCHAR>::formatString(MESSAGE_TABLE_SQL_V3_index3, strUserID.c_str(), strTableName.c_str());
            sqliteOperator.PrepareSQL(strSql);
            bRet = sqliteOperator.Execute();
            if (!bRet)
            {
                YouMe_LOG_Error(__XT("create table index3 failed"));
            }
             */
            
		}
		else
		{
			/*youmecommon::CSqliteOperator sqliteOperator(m_sqliteDb);
			XString strSql = CStringUtilT<XCHAR>::formatString(__XT("select name from sqlite_master where name=\'%s\' and sql like \'%%isRead integer%%\';"), strTableName.c_str());	// 查询isRead是否存在
			sqliteOperator.PrepareSQL(strSql);
			bool bRet = sqliteOperator.Execute();
			if (!bRet)
			{
				YouMe_LOG_Error(__XT("execute sql failed"));
				return;
			}
			XString name;
			if (sqliteOperator.Next())
			{
				sqliteOperator >> name;
			}
			if (name.empty())
			{*/
				youmecommon::CSqliteOperator sqliteOperator2(m_sqliteDb);
				XString strSql = CStringUtilT<XCHAR>::formatString(__XT("alter table %s add column isRead integer default 1"), strTableName.c_str());
				sqliteOperator2.PrepareSQL(strSql);
				bool bRet = sqliteOperator2.Execute();
				if (!bRet)
				{
//					YouMe_LOG_Error(__XT("exccute sql failed"));
				}
            
            strSql = CStringUtilT<XCHAR>::formatString(__XT("alter table %s add column isPlayed integer default 0"), strTableName.c_str());
            sqliteOperator2.PrepareSQL(strSql);
            bRet = sqliteOperator2.Execute();
            if (!bRet)
            {
//                YouMe_LOG_Error(__XT("exccute sql failed"));
            }
			//}
            
            strSql = CStringUtilT<XCHAR>::formatString(MESSAGE_TABLE_SQL_V3_index4, strUserID.c_str());
            sqliteOperator2.PrepareSQL(strSql);
            bRet = sqliteOperator2.Execute();
            if (!bRet)
            {
                YouMe_LOG_Info(__XT("drop index failed, may not exist"));
            }else{
                YouMe_LOG_Info(__XT("drop index success"));
            }
		}
	}

	{
		std::lock_guard<std::mutex> lock(m_threadMutuex);
		if (m_thread.joinable())
		{
			m_thread.join();
		}
		m_bExit = false;
		m_thread = std::thread(&MessageRecorder::MessageRecordThread, this);
	}

	m_bInit = true;
}

void MessageRecorder::UnInit()
{
	m_bExit = true;
	m_messageRecordWait.Increment();
	{
		std::lock_guard<std::mutex> lock(m_threadMutuex);
		if (m_thread.joinable())
		{
			m_thread.join();
		}
	}
	m_sqliteDb.Close();
	m_messageOperateMap.clear();
	m_bInit = false;
}

bool MessageRecorder::SaveHistoryMessage(HistoryMessageInfo& recordInfo, bool fromServerHistory)
{
	if (!m_bInit)
	{
		YouMe_LOG_Info(__XT("not init"));
		return false;
	}
	YouMeIMManager* pYouMeIMManager = (YouMeIMManager*)YouMeIMManager::CreateInstance();
	if (pYouMeIMManager == NULL)
	{
		return false;
	}

	YOUMECommonProtocol::MsgComment extend;
	for (std::map<std::string, std::string>::const_iterator itr = recordInfo.extend.begin(); itr != recordInfo.extend.end(); ++itr)
	{
		YOUMECommonProtocol::NameValue* pParam = extend.add_comment();
		if (pParam != NULL)
		{
			pParam->set_name(itr->first);
			pParam->set_value(itr->second);
			pParam->set_value_type(YOUMECommonProtocol::VALUE_STRING);
		}
	}
	std::string strExtra;
	extend.SerializeToString(&strExtra);
	youmecommon::CXSharedArray<byte> pExtra( (int)strExtra.length());
	memcpy(pExtra.Get(), strExtra.c_str(), strExtra.length());
	int read = 0;

	{
		std::lock_guard<std::mutex> lock(m_dbOperateMutuex);
        {
            youmecommon::CSqliteOperator sqliteOperator(m_sqliteDb);
            YouMe_LOG_Info(__XT("save history id %llu sender: %s msgType: %d"),recordInfo.messageID,recordInfo.senderID.c_str(),recordInfo.messageType);
            XString strSql = CStringUtilT<XCHAR>::formatString(__XT("insert into tb_message_%s(id,messageID,messageType,senderID,receiverID,chatType,createTime,content,localPath,extra,isRead,isPlayed) values(?1,?2,?3,?4,?5,?6,?7,?8,?9,?10,?11,?12)"), pYouMeIMManager->GetCurrentUser().c_str());
            sqliteOperator.PrepareSQL(strSql);
            sqliteOperator << (long long)recordInfo.id;
            sqliteOperator << (long long)recordInfo.messageID;
            sqliteOperator << recordInfo.messageType;
            sqliteOperator << recordInfo.senderID;
            sqliteOperator << recordInfo.receiverID;
            sqliteOperator << recordInfo.chatType;
            sqliteOperator << (long long)recordInfo.createTime;
            sqliteOperator << recordInfo.content;
            sqliteOperator << recordInfo.localPath;
            sqliteOperator << pExtra;
			sqliteOperator << read;
            int played = 0;
            if (recordInfo.messageType == MessageBodyType_Voice)
            {
                played = recordInfo.senderID == pYouMeIMManager->GetCurrentUser() ? 1 : 0;
                sqliteOperator << played;
            }
            else
            {
                sqliteOperator << played;
            }
            return sqliteOperator.Execute();
        }
	}
}

bool MessageRecorder::SaveHistoryMessage(IYIMMessage* message,bool fromServerHistory,XINT64 autoIncrementID,XINT64 currentTime)
{
	if (!m_bInit)
	{
		YouMe_LOG_Info(__XT("not init"));
		return false;
	}
	IYIMMessageBodyBase* pMessageBodyBase = message->GetMessageBody();
	if (NULL == pMessageBodyBase)
	{
		YouMe_LOG_Error(__XT("message body null"));
		return false;
	}
	HistoryMessageInfo info;
	info.messageID = message->GetMessageID();
	info.chatType = message->GetChatType();
	info.messageType = pMessageBodyBase->GetMessageType();
	info.senderID = XString(message->GetSenderID());
	info.receiverID = XString(message->GetReceiveID());
    info.createTime = currentTime /1000 ;//message->GetCreateTime();
    info.id = autoIncrementID;
	std::string strDistance = CStringUtilT<char>::to_string(message->GetDistance());
	switch (pMessageBodyBase->GetMessageType())
	{
	case MessageBodyType_TXT:
	{
		IYIMMessageBodyText* pMessageBody = dynamic_cast<IYIMMessageBodyText*>(pMessageBodyBase);
		if (pMessageBody == NULL)
		{
			YouMe_LOG_Error(__XT("message body null"));
			return false;
		}
        //info.messageID = autoIncrementID;
		info.content = pMessageBody->GetMessageContent();
		info.extend["Distance"] = strDistance;
		info.extend["AttachParam"] = XStringToUTF8(XString(pMessageBody->GetAttachParam()));
	}
	break;
	case MessageBodyType_CustomMesssage:
	{
		IYIMMessageBodyCustom* pMessageBody = dynamic_cast<IYIMMessageBodyCustom*>(pMessageBodyBase);
		if (pMessageBody == NULL)
		{
			YouMe_LOG_Error(__XT("message body null"));
			return false;
		}
        //info.messageID = autoIncrementID;
		info.extend["CustomContent"] = pMessageBody->GetCustomMessage();
		info.extend["Distance"] = strDistance;
	}
	break;
	case MessageBodyType_Voice:
	{
		CYouMeIMMessageBodyAudio* pMessageBody = dynamic_cast<CYouMeIMMessageBodyAudio*>(pMessageBodyBase);
		if (pMessageBody == NULL)
		{
			return false;
		}
		info.localPath = pMessageBody->GetDownloadURL();	//先保存url下载后更新为本地路径(有需求接收语音消息默认不下载，由用户触发下载，可能是从历史消息记录查询)
		info.content = XString(pMessageBody->GetText());
		info.extend["FileSize"] = CStringUtilT<char>::to_string(pMessageBody->GetFileSize());
		info.extend["Time"] = CStringUtilT<char>::to_string(pMessageBody->GetAudioTime());
		info.extend["Param"] = XStringToUTF8(XString(pMessageBody->GetExtraParam()));
		info.extend["Distance"] = strDistance;
	}
	break;
	case MessageBodyType_File:
	{
		IYIMMessageBodyFile* pMessageBody = dynamic_cast<IYIMMessageBodyFile*>(pMessageBodyBase);
		if (pMessageBody == NULL)
		{
			YouMe_LOG_Error(__XT("message body null"));
			return false;
		}
        info.localPath = pMessageBody->GetLocalPath();
		info.extend["FileSize"] = CStringUtilT<char>::to_string(pMessageBody->GetFileSize());
		info.extend["FileName"] = XStringToUTF8(XString(pMessageBody->GetFileName()));
		info.extend["FileExtension"] = XStringToUTF8(XString(pMessageBody->GetFileExtension()));
		info.extend["Param"] = XStringToUTF8(XString(pMessageBody->GetExtraParam()));
		info.extend["Distance"] = strDistance;
        info.extend["FileType"] = CStringUtilT<char>::to_string((unsigned int)pMessageBody->GetFileType());        
	}
	break;
	/*case MessageBodyType_Gift:
	{
		CYouMeIMMessageBodyGift* pMessageBody = dynamic_cast<CYouMeIMMessageBodyGift*>(pMessageBodyBase);
		if (pMessageBody == NULL)
		{
			return false;
		}
		std::map<std::string, std::string> extend = pMessageBody->getExtend();
		info.extend.insert(std::map<std::string, std::string>::value_type("id", CStringUtilT<char>::to_string(pMessageBody->GetGiftID())));
		info.extend.insert(std::map<std::string, std::string>::value_type("count", CStringUtilT<char>::to_string(pMessageBody->GetGiftCount())));
		for (std::map<std::string, std::string>::const_iterator itr = extend.begin(); itr != extend.end(); ++itr)
		{
			info.extend.insert(std::map<std::string, std::string>::value_type(itr->first, itr->second));
		}
	}
		break;*/
	default:
		return false;
	}

	return SaveHistoryMessage(info);
}

bool MessageRecorder::UpdateMessagePath(XUINT64 messageID, const XString path,  XUINT64 autoIncrementID, XUINT64 currentTimeMs)
{
	YouMeIMManager* pYouMeIMManager = (YouMeIMManager*)YouMeIMManager::CreateInstance();
	if (pYouMeIMManager == NULL)
	{
		return false;
	}

	{
		std::lock_guard<std::mutex> lock(m_dbOperateMutuex);
        // YouMe_LOG_Error(__XT("UpdateMessagePath id %llu"),currentTimeMs);
		youmecommon::CSqliteOperator sqliteOperator(m_sqliteDb);
		XString strSql = CStringUtilT<XCHAR>::formatString(__XT("update tb_message_%s set localPath=?1 where messageID=?2"), pYouMeIMManager->GetCurrentUser().c_str());
		sqliteOperator.PrepareSQL(strSql);
		sqliteOperator << path;
        //sqliteOperator << (XINT64)autoIncrementID;
        //sqliteOperator << (XINT64)currentTimeMs;
		sqliteOperator << (XINT64)messageID;
		return sqliteOperator.Execute();
	}
}

YIMErrorcode MessageRecorder::QueryHistoryMessage(XString targetID, XUINT64 startMessageID, short count, short direction, int chatType)
{
	if (!m_bInit)
	{
		YouMe_LOG_Info(__XT("not init"));
		return YIMErrorcode_StatusError;
	}

	MessageOperateInfo info;
	info.type = TYPE_QUERY_RECORD;
	info.extend["TargetID"] = targetID;
	info.extend["StartMessageID"] = startMessageID;
	info.extend["Count"] = count;
	info.extend["Direction"] = direction;
	info.extend["ChatType"] = chatType;
	{
		std::lock_guard<std::mutex> lock(m_messageOperateMutuex);
		m_messageOperateMap.push_back(info);
	}
	m_messageRecordWait.Increment();
    
	return YIMErrorcode_Success;
}

YIMErrorcode MessageRecorder::DeleteHistoryMessage(int chatType, XUINT64 messageID, XUINT64 time)
{
	if (!m_bInit)
	{
		YouMe_LOG_Info(__XT("not init"));
		return YIMErrorcode_StatusError;
	}

	MessageOperateInfo info;
	info.type = TYPE_DELETE_RECORD;
	info.extend["ChatType"] = chatType;
	info.extend["MessageID"] = messageID;
	info.extend["Time"] = time;
	{
		std::lock_guard<std::mutex> lock(m_messageOperateMutuex);
		m_messageOperateMap.push_back(info);
	}
	m_messageRecordWait.Increment();
    
	return YIMErrorcode_Success;
}

YIMErrorcode MessageRecorder::DeleteHistoryMessage(XString targetID, int chatType, XUINT64 startMessageID, unsigned int count)
{
	if (!m_bInit)
	{
		YouMe_LOG_Info(__XT("not init"));
		return YIMErrorcode_StatusError;
	}

	MessageOperateInfo info;
	info.type = TYPE_DELETE_RECORD2;
	info.extend["Target"] = targetID;
	info.extend["ChatType"] = chatType;
	info.extend["MessageID"] = startMessageID;
	info.extend["Count"] = count;
	{
		std::lock_guard<std::mutex> lock(m_messageOperateMutuex);
		m_messageOperateMap.push_back(info);
	}
	m_messageRecordWait.Increment();

	return YIMErrorcode_Success;
}

YIMErrorcode MessageRecorder::QueryRecentContacts()
{
	if (!m_bInit)
	{
		YouMe_LOG_Info(__XT("not init"));
		return YIMErrorcode_StatusError;
	}

	MessageOperateInfo info;
	info.type = TYPE_QUERY_CONTACTS;
	{
		std::lock_guard<std::mutex> lock(m_messageOperateMutuex);
		m_messageOperateMap.push_back(info);
	}
	m_messageRecordWait.Increment();

	return YIMErrorcode_Success;
}

YIMErrorcode MessageRecorder::SetMessageRead(XUINT64 messageID, bool read)
{
	if (!m_bInit)
	{
		YouMe_LOG_Info(__XT("not init"));
		return YIMErrorcode_StatusError;
	}

	MessageOperateInfo info;
	info.type = TYPE_SET_MESSAGE_READ;
	info.extend["MessageID"] = messageID;
	info.extend["Read"] = static_cast<int>(read);
	{
		std::lock_guard<std::mutex> lock(m_messageOperateMutuex);
		m_messageOperateMap.push_back(info);
	}
	m_messageRecordWait.Increment();

	return YIMErrorcode_Success;
}

YIMErrorcode MessageRecorder::SetAllMessageRead(XString userID, bool read)
{
    if (!m_bInit)
    {
        YouMe_LOG_Info(__XT("not init"));
        return YIMErrorcode_StatusError;
    }
    
    MessageOperateInfo info;
    info.type = TYPE_SET_ALL_MSG_READ;
    info.extend["UserID"] = userID;
    info.extend["Read"] = static_cast<int>(read);
    {
        std::lock_guard<std::mutex> lock(m_messageOperateMutuex);
        m_messageOperateMap.push_back(info);
    }
    m_messageRecordWait.Increment();
    
    return YIMErrorcode_Success;
}

YIMErrorcode MessageRecorder::SetSenderAllMessageReadWithChatType(XString userID, int chatType)
{
    if (!m_bInit)
    {
        YouMe_LOG_Info(__XT("not init"));
        return YIMErrorcode_StatusError;
    }
    
    MessageOperateInfo info;
    info.type = TYPE_SET_SENDER_ALL_MSG_READ_WITH_CHAT_TYPE;
    info.extend["UserID"] = userID;
    info.extend["ChatType"] = static_cast<int>(chatType);
    {
        std::lock_guard<std::mutex> lock(m_messageOperateMutuex);
        m_messageOperateMap.push_back(info);
    }
    m_messageRecordWait.Increment();
    
    return YIMErrorcode_Success;
}

YIMErrorcode MessageRecorder::SetRecverAllMessageReadWithChatType(XString userID, int chatType)
{
    if (!m_bInit)
    {
        YouMe_LOG_Info(__XT("not init"));
        return YIMErrorcode_StatusError;
    }
    
    MessageOperateInfo info;
    info.type = TYPE_SET_RECVER_ALL_MSG_READ_WITH_CHAT_TYPE;
    info.extend["UserID"] = userID;
    info.extend["ChatType"] = static_cast<int>(chatType);
    {
        std::lock_guard<std::mutex> lock(m_messageOperateMutuex);
        m_messageOperateMap.push_back(info);
    }
    m_messageRecordWait.Increment();
    
    return YIMErrorcode_Success;
}

YIMErrorcode MessageRecorder::SetVoiceMsgPlayed(XUINT64 messageID, bool played)
{
    if (!m_bInit)
    {
        YouMe_LOG_Info(__XT("not init"));
        return YIMErrorcode_StatusError;
    }
    
    MessageOperateInfo info;
    info.type = TYPE_SET_VOICE_MSG_PLAYED;
    info.extend["MessageID"] = messageID;
    info.extend["Played"] = static_cast<int>(played);
    {
        std::lock_guard<std::mutex> lock(m_messageOperateMutuex);
        m_messageOperateMap.push_back(info);
    }
    m_messageRecordWait.Increment();
    
    return YIMErrorcode_Success;
}

YIMErrorcode MessageRecorder::DeleteSpecifiedHistoryMessage(XString targetID, int chatType, const std::vector<XUINT64>& excludeMesList)
{
    if (!m_bInit)
    {
        YouMe_LOG_Info(__XT("not init"));
        return YIMErrorcode_StatusError;
    }
    
    MessageOperateInfo info;
    info.type = TYPE_DELETE_SPECIFIED_RECORD;
    info.extend["TargetID"] = targetID;
    info.extend["ChatType"] = chatType;
    info.extend["MessageList"] = excludeMesList;
    {
        std::lock_guard<std::mutex> lock(m_messageOperateMutuex);
        m_messageOperateMap.push_back(info);
    }
    m_messageRecordWait.Increment();
    
    return YIMErrorcode_Success;
}

void MessageRecorder::QueryMessageRecord(XString targetID, XUINT64 startMessageID, short count, short direction, int chatType)
{
	YouMe_LOG_Info(__XT("QueryMessageRecord targetID:%s startMessageID:%llu count:%d direction:%d chatType:%d"), targetID.c_str(), startMessageID, count, direction, chatType);

	YouMeIMManager* pYouMeIMManager = (YouMeIMManager*)YouMeIMManager::CreateInstance();
	if (pYouMeIMManager == NULL)
	{
		YouMe_LOG_Error(__XT("YouMeIMManager is null"));
	}
	short nCount = count == 0 ? DEFAULT_ROW_NUM : count;
	if (nCount > MAX_ROW_NUM)
	{
		nCount = MAX_ROW_NUM;
	}

	XINT64 msgID = startMessageID;
	int nRemain = 0;
	if (startMessageID == 0)
	{
		std::lock_guard<std::mutex> lock(m_dbOperateMutuex);
        {
            youmecommon::CSqliteOperator sqliteOperator(m_sqliteDb);
			XString strSql;
			if (ChatType_RoomChat == chatType)
			{
				strSql = CStringUtilT<XCHAR>::formatString(__XT("select max(messageID) from tb_message_%s where receiverID=?1 and chatType=?2"), pYouMeIMManager->GetCurrentUser().c_str());
				sqliteOperator.PrepareSQL(strSql);
				sqliteOperator << targetID;
				sqliteOperator << chatType;
			}
			else
			{
				strSql = CStringUtilT<XCHAR>::formatString(__XT("select max(messageID) from tb_message_%s where (receiverID=?1 and chatType=?2) or (senderID=?3 and chatType=?4)"), pYouMeIMManager->GetCurrentUser().c_str());
				sqliteOperator.PrepareSQL(strSql);
				sqliteOperator << targetID;
				sqliteOperator << chatType;
				sqliteOperator << targetID;
				sqliteOperator << chatType;
			}
            sqliteOperator.Execute();
            if (sqliteOperator.Next())
            {
                sqliteOperator >> msgID;
            }
        }
	}
	std::list<std::shared_ptr<HistoryMessageInfo> > records;
	if (msgID != 0)
	{
		XString strSql;
		XString strSqlCount;

		if (ChatType_RoomChat == chatType)
		{
			if (direction == 0)
			{
				strSql = CStringUtilT<XCHAR>::formatString(__XT("select id,messageID,messageType,senderID,receiverID,chatType,createTime,content,localPath,extra,isRead,isPlayed from tb_message_%s where receiverID=?1 and chatType=?2 and messageID<=?3 order by id desc limit ?4 offset ?5"), pYouMeIMManager->GetCurrentUser().c_str());
				strSqlCount = CStringUtilT<XCHAR>::formatString(__XT("select count(id) total from tb_message_%s where receiverID=?1 and chatType=?2 and messageID<=?3"), pYouMeIMManager->GetCurrentUser().c_str());
			}
			else
			{
				strSql = CStringUtilT<XCHAR>::formatString(__XT("select id,messageID,messageType,senderID,receiverID,chatType,createTime,content,localPath,extra,isRead,isPlayed from tb_message_%s where receiverID=?1 and chatType=?2 and messageID>=?3 limit ?4 offset ?5"), pYouMeIMManager->GetCurrentUser().c_str());
				strSqlCount = CStringUtilT<XCHAR>::formatString(__XT("select count(id) total from tb_message_%s where receiverID=?1 and chatType=?2 and messageID>=?3"), pYouMeIMManager->GetCurrentUser().c_str());
			}
		}
		else
		{
			if (direction == 0)
			{
				strSql = CStringUtilT<XCHAR>::formatString(__XT("select id,messageID,messageType,senderID,receiverID,chatType,createTime,content,localPath,extra,isRead,isPlayed from tb_message_%s where ((receiverID=?1 and chatType=?2) or (senderID=?3 and chatType=?4)) and messageID<=?5 order by id desc limit ?6 offset ?7"), pYouMeIMManager->GetCurrentUser().c_str());
				strSqlCount = CStringUtilT<XCHAR>::formatString(__XT("select count(id) total from tb_message_%s where (receiverID=?1 and chatType=?2) or (senderID=?3 and chatType=?4)) and messageID<=?5"), pYouMeIMManager->GetCurrentUser().c_str());
			}
			else
			{
				strSql = CStringUtilT<XCHAR>::formatString(__XT("select id,messageID,messageType,senderID,receiverID,chatType,createTime,content,localPath,extra,isRead,isPlayed from tb_message_%s where (receiverID=?1 and chatType=?2) or (senderID=?3 and chatType=?4)) and messageID>=?5 limit ?6 offset ?7"), pYouMeIMManager->GetCurrentUser().c_str());
				strSqlCount = CStringUtilT<XCHAR>::formatString(__XT("select count(id) total from tb_message_%s where (receiverID=?1 and chatType=?2) or (senderID=?3 and chatType=?4)) and messageID>=?5"), pYouMeIMManager->GetCurrentUser().c_str());
			}
		}

		{
			std::lock_guard<std::mutex> lock(m_dbOperateMutuex);
            {
                youmecommon::CSqliteOperator sqliteOperator(m_sqliteDb);
                sqliteOperator.PrepareSQL(strSql);
				if (ChatType_RoomChat == chatType)
				{
					sqliteOperator << targetID;
					sqliteOperator << chatType;
					sqliteOperator << msgID;
					sqliteOperator << count;
                    sqliteOperator << 0;
				}
				else
				{
					sqliteOperator << targetID;
					sqliteOperator << chatType;
					sqliteOperator << targetID;
					sqliteOperator << chatType;
					sqliteOperator << msgID;
					sqliteOperator << count;
                    sqliteOperator << 0;
				}
                sqliteOperator.Execute();
                int nRow = 0;
                while (sqliteOperator.Next() && nRow < nCount)
                {
                    std::shared_ptr<HistoryMessageInfo> pInfo(new HistoryMessageInfo);
                    youmecommon::CXSharedArray<byte> buffer;
					long long serialNo = 0;
                    long long messageID = 0;
                    int chatType = 0;
                    int messageType = 0;
					int read = 0;
                    int played = 0;
					sqliteOperator >> serialNo >> messageID >> messageType >> pInfo->senderID >> pInfo->receiverID >> chatType >> pInfo->createTime >> pInfo->content >> pInfo->localPath >> buffer >> read >> played;
                    pInfo->messageType = static_cast<YIMMessageBodyType>(messageType);
                    if ((pInfo->messageType == MessageBodyType_Voice || pInfo->messageType == MessageBodyType_File) && pInfo->localPath.empty())
                    {
                        continue;
                    }
					pInfo->id = serialNo;
                    pInfo->messageID = messageID;
                    pInfo->chatType = static_cast<YIMChatType>(chatType);
					pInfo->isRead = static_cast<bool>(read);
                    pInfo->isPlayed = static_cast<bool>(played);
                    
                    YOUMECommonProtocol::MsgComment extend;
                    extend.ParseFromArray(buffer.Get(), buffer.GetBufferLen());
                    for (int i = 0; i < extend.comment_size(); ++i)
                    {
                        pInfo->extend.insert(std::map<std::string, std::string>::value_type(extend.comment(i).name(), extend.comment(i).value()));
                    }

                    if (0 == direction)
                    {
                        records.push_front(pInfo);
                    }
                    else
                    {
                        records.push_back(pInfo);
                    }
                    ++nRow;
                }

				sqliteOperator.PrepareSQL(strSqlCount);
				if (ChatType_RoomChat == chatType)
				{
					sqliteOperator << targetID;
					sqliteOperator << chatType;
					sqliteOperator << msgID;
				}
				else
				{
					sqliteOperator << targetID;
					sqliteOperator << chatType;
					sqliteOperator << targetID;
					sqliteOperator << chatType;
					sqliteOperator << msgID;
				}
                sqliteOperator.Execute();
                if (sqliteOperator.Next())
                {
                    sqliteOperator >> nRemain;
                    if (nRemain >= (int)records.size())
                    {
                        nRemain -= records.size();
                    }
                }
            }
		}
	}	

	if (pYouMeIMManager->GetMessageManager() != NULL)
	{
        YouMe_LOG_Info(__XT("OnQueryHistoryMessage count:%d nRemain:%d"), (int)records.size(), nRemain);
		((YouMeIMMessageManager*)pYouMeIMManager->GetMessageManager())->OnQueryHistoryMessage(YIMErrorcode_Success, targetID, records, nRemain);
	}
}

void MessageRecorder::DeleteMessageRecord(YIMChatType chatType, XUINT64 messageID, XUINT64 time)
{
    YouMe_LOG_Info(__XT("DeleteMessageRecord chatType:%d messageID:%llu time:%lld"), chatType, messageID, time);
	YouMeIMManager* pYouMeIMManager = (YouMeIMManager*)YouMeIMManager::CreateInstance();
	if (pYouMeIMManager == NULL)
	{
		return;
	}
	XString strSqlDel;
	XString strSqlPath;
	if (messageID != 0)
	{
		strSqlDel = CStringUtilT<XCHAR>::formatString(__XT("delete from tb_message_%s where messageID=%llu"), pYouMeIMManager->GetCurrentUser().c_str(), messageID);
		strSqlPath = CStringUtilT<XCHAR>::formatString(__XT("select localPath from tb_message_%s where messageID=%llu and localPath!=\'\'"), pYouMeIMManager->GetCurrentUser().c_str(), messageID);
	}
	else
	{
		if (chatType == ChatType_Unknow && time == 0)
		{
			if (time == 0)
			{
				strSqlDel = CStringUtilT<XCHAR>::formatString(__XT("drop table tb_message_%s"), pYouMeIMManager->GetCurrentUser().c_str());
				strSqlPath = CStringUtilT<XCHAR>::formatString(__XT("select localPath from tb_message_%s where localPath!=\'\'"), pYouMeIMManager->GetCurrentUser().c_str());
			}
			else
			{
				strSqlDel = CStringUtilT<XCHAR>::formatString(__XT("delete from tb_message_%s where createTime<=%llu"), pYouMeIMManager->GetCurrentUser().c_str(), time);
				strSqlPath = CStringUtilT<XCHAR>::formatString(__XT("select localPath from tb_message_%s where createTime<=%llu and localPath!=\'\'"), pYouMeIMManager->GetCurrentUser().c_str(), time);
			}
		}
		else
		{
			if (time == 0)
			{
				strSqlDel = CStringUtilT<XCHAR>::formatString(__XT("delete from tb_message_%s where chatType=%d"), pYouMeIMManager->GetCurrentUser().c_str(), chatType);
				strSqlPath = CStringUtilT<XCHAR>::formatString(__XT("select localPath from tb_message_%s where chatType=%d and localPath!=\'\'"), pYouMeIMManager->GetCurrentUser().c_str(), chatType);
			}
			else
			{
				strSqlDel = CStringUtilT<XCHAR>::formatString(__XT("delete from tb_message_%s where chatType=%d and createTime<=%llu"), pYouMeIMManager->GetCurrentUser().c_str(), chatType, time);
				strSqlPath = CStringUtilT<XCHAR>::formatString(__XT("select localPath from tb_message_%s where chatType=%d and createTime<=%llu and localPath!=\'\'"), pYouMeIMManager->GetCurrentUser().c_str(), chatType, time);
			}
		}
	}

	{
		std::lock_guard<std::mutex> lock(m_dbOperateMutuex);
		youmecommon::CSqliteOperator sqliteOperator(m_sqliteDb);
		sqliteOperator.PrepareSQL(strSqlPath);
		sqliteOperator.Execute();
		while (sqliteOperator.Next())
		{
			XString strLocalPath;
			sqliteOperator >> strLocalPath;
			youmecommon::CXFile::remove_file(strLocalPath);
		}
		sqliteOperator.PrepareSQL(strSqlDel);
		sqliteOperator.Execute();
	}
}

void MessageRecorder::DeleteMessageRecord(XString& targetID, YIMChatType chatType, XUINT64 startMessageID, unsigned int count)
{
    YouMe_LOG_Info(__XT("DeleteMessageRecord targetID:%s chatType:%d startMessageID:%llu count:%d"), targetID.c_str(), chatType, startMessageID, count);
	YouMeIMManager* pYouMeIMManager = (YouMeIMManager*)YouMeIMManager::CreateInstance();
	if (pYouMeIMManager == NULL)
	{
		return;
	}

	XString strSqlDel;
	XString strSqlPath;
	if (ChatType_RoomChat == chatType)
	{
		strSqlDel = CStringUtilT<XCHAR>::formatString(__XT("delete from tb_message_%s where id in (select id from tb_message_%s where receiverID=\'%s\' and chatType=%d"), pYouMeIMManager->GetCurrentUser().c_str(), pYouMeIMManager->GetCurrentUser().c_str(), targetID.c_str(), chatType);
		strSqlPath = CStringUtilT<XCHAR>::formatString(__XT("select localPath from tb_message_%s t1 join (select id from tb_message_%s where receiverID=\'%s\' and chatType=%d"), pYouMeIMManager->GetCurrentUser().c_str(), pYouMeIMManager->GetCurrentUser().c_str(), targetID.c_str(), chatType);
	}
	else
	{
		strSqlDel = CStringUtilT<XCHAR>::formatString(__XT("delete from tb_message_%s where id in (select id from tb_message_%s where ((senderID=\'%s\' and receiverID=\'%s\') or (senderID=\'%s\' and receiverID=\'%s\')) and chatType=%d"),
			pYouMeIMManager->GetCurrentUser().c_str(), pYouMeIMManager->GetCurrentUser().c_str(), targetID.c_str(), pYouMeIMManager->GetCurrentUser().c_str(), pYouMeIMManager->GetCurrentUser().c_str(), targetID.c_str(), chatType);
		strSqlPath = CStringUtilT<XCHAR>::formatString(__XT("select localPath from tb_message_%s t1 join (select id from tb_message_%s where ((senderID=\'%s\' and receiverID=\'%s\') or (senderID=\'%s\' and receiverID=\'%s\')) and chatType=%d"),
			pYouMeIMManager->GetCurrentUser().c_str(), pYouMeIMManager->GetCurrentUser().c_str(), targetID.c_str(), pYouMeIMManager->GetCurrentUser().c_str(), pYouMeIMManager->GetCurrentUser().c_str(), targetID.c_str(), chatType);
	}
	if (startMessageID != 0)
	{
		XString temp = CStringUtilT<XCHAR>::formatString(__XT(" and `id`<=%llu"), startMessageID);
		strSqlDel.append(temp);
		strSqlPath.append(temp);
	}
	if (count != 0)
	{
		XString temp = CStringUtilT<XCHAR>::formatString(__XT(" order by id desc limit %u offset 0"), count);
		strSqlDel.append(temp);
		strSqlPath.append(temp);
	}
	strSqlDel.append(__XT(")"));
	strSqlPath.append(__XT(") t2 on t1.id=t2.id and t1.localPath!=''"));
	
	{
		std::lock_guard<std::mutex> lock(m_dbOperateMutuex);
		youmecommon::CSqliteOperator sqliteOperator(m_sqliteDb);
		sqliteOperator.PrepareSQL(strSqlPath);
		sqliteOperator.Execute();
		while (sqliteOperator.Next())
		{
			XString strLocalPath;
			sqliteOperator >> strLocalPath;
			youmecommon::CXFile::remove_file(strLocalPath);
		}
		sqliteOperator.PrepareSQL(strSqlDel);
		sqliteOperator.Execute();
	}
}

void MessageRecorder::QueryContacts()
{
    YouMeIMManager* pYouMeIMManager = (YouMeIMManager*)YouMeIMManager::CreateInstance();
    if (pYouMeIMManager == NULL)
    {
        return;
    }
	IYIMContactCallback* pCallback = pYouMeIMManager->GetContactCallback();
	if (pCallback == NULL)
	{
		YouMe_LOG_Error(__XT("callback is null"));
		return;
	}

    XString strCurUserID = pYouMeIMManager->GetCurrentUser();
	std::list<CContactsMessageInfo> contactsList;
	XString strSql = CStringUtilT<XCHAR>::formatString(__XT("select senderID,receiverID,messageType,createTime,content,localPath from tb_message_%s where id in(select max(id) from tb_message_%s where chatType=1 group by senderID,receiverID) order by id desc limit %d offset 0"), strCurUserID.c_str(), strCurUserID.c_str(), MAX_CONTACT_NUM * 2);
	{
		std::lock_guard<std::mutex> lock(m_dbOperateMutuex);
		youmecommon::CSqliteOperator sqliteOperator(m_sqliteDb);
		sqliteOperator.PrepareSQL(strSql);
		sqliteOperator.Execute();
		while (sqliteOperator.Next())
		{
			CContactsMessageInfo cmi;
			int messageType = 0;
			sqliteOperator >> cmi.m_senderID >> cmi.m_receiverID >> messageType >> cmi.m_createTime >> cmi.m_content >> cmi.m_localPath;
			if (find(contactsList.begin(), contactsList.end(), cmi) != contactsList.end())
			{
				continue;
			}
			cmi.m_messageType = (YIMMessageBodyType)messageType;
			cmi.m_contactID = cmi.m_senderID == strCurUserID ? cmi.m_receiverID : cmi.m_senderID;
			contactsList.push_back(cmi);
			if (contactsList.size() >= MAX_CONTACT_NUM)
			{
				break;
			}
		}
	}
	std::list<std::shared_ptr<IYIMContactsMessageInfo> > contacts;
	for (std::list<CContactsMessageInfo>::const_iterator itr = contactsList.begin(); itr != contactsList.end(); ++itr)
	{
        XString strSql = CStringUtilT<XCHAR>::formatString(__XT("select COUNT(1) from tb_message_%s where isRead=0 and chatType=1 and senderID=\'%s\'"), strCurUserID.c_str(), itr->m_contactID.c_str());
        std::lock_guard<std::mutex> lock(m_dbOperateMutuex);
        youmecommon::CSqliteOperator sqliteOperator(m_sqliteDb);
        sqliteOperator.PrepareSQL(strSql);
        sqliteOperator.Execute();
        
        int notReadNum;
        while (sqliteOperator.Next())
        {
            sqliteOperator >> notReadNum;
        }
        
		std::shared_ptr<CContactsMessageInfo> pContactMsg(new CContactsMessageInfo);
		pContactMsg->m_contactID = itr->m_contactID;
		pContactMsg->m_messageType = itr->m_messageType;
		pContactMsg->m_content = itr->m_content;
		pContactMsg->m_createTime = itr->m_createTime;
        pContactMsg->m_notReadMsgNum = (unsigned int)notReadNum;
        
        pContactMsg->m_localPath = itr->m_localPath;
                
		contacts.push_back(pContactMsg);
	}

	YouMe_LOG_Debug(__XT("contacts total:%lu"), contacts.size());
	pCallback->OnGetRecentContacts(YIMErrorcode_Success, contacts);
}

void MessageRecorder::UpdateMessageRead(XUINT64 messageID, int read)
{
	YouMeIMManager* pYouMeIMManager = (YouMeIMManager*)YouMeIMManager::CreateInstance();
	if (pYouMeIMManager == NULL)
	{
		return;
	}

	XINT64 id = -1;
	{
		std::lock_guard<std::mutex> lock(m_dbOperateMutuex);
		youmecommon::CSqliteOperator sqliteOperator(m_sqliteDb);
		XString strSql = CStringUtilT<XCHAR>::formatString(__XT("select id from tb_message_%s where messageID=?1"), pYouMeIMManager->GetCurrentUser().c_str());
		sqliteOperator.PrepareSQL(strSql);
		sqliteOperator << (XINT64)messageID;
		if (sqliteOperator.Next())
		{
			sqliteOperator >> id;
		}
		if (-1 != id)
		{
			strSql = CStringUtilT<XCHAR>::formatString(__XT("update tb_message_%s set isRead=?1 where messageID=?2"), pYouMeIMManager->GetCurrentUser().c_str());
			sqliteOperator.PrepareSQL(strSql);
			sqliteOperator << read;
			sqliteOperator << (XINT64)messageID;
			sqliteOperator.Execute();
		}
		else
		{
			strSql = CStringUtilT<XCHAR>::formatString(__XT("update tb_message_%s set isRead=?1 where id=?2"), pYouMeIMManager->GetCurrentUser().c_str());
			sqliteOperator.PrepareSQL(strSql);
			sqliteOperator << read;
			sqliteOperator << (XINT64)messageID;
			sqliteOperator.Execute();
		}
	}
}

void MessageRecorder::getRecvId(int read, std::vector<unReadRecvId_s> &unReadRecvIds) {
	//YouMe_LOG_Error(__XT("bruce >>> enter"));
	unReadRecvId_s unReadRecver;
	XString recvId;
	int chatType;
	XINT64 msgId;

	YouMeIMManager* pYouMeIMManager = (YouMeIMManager*)YouMeIMManager::CreateInstance();
    if (pYouMeIMManager == NULL)
    {
        return;
    }

	std::lock_guard<std::mutex> lock(m_dbOperateMutuex);
    youmecommon::CSqliteOperator sqliteOperator(m_sqliteDb);
	XString strSql = CStringUtilT<XCHAR>::formatString(__XT("select receiverID,chatType,messageID from tb_message_%s where senderID=?1 and isRead=?2"), pYouMeIMManager->GetCurrentUser().c_str());
	sqliteOperator.PrepareSQL(strSql);
	sqliteOperator << pYouMeIMManager->GetCurrentUser();
	sqliteOperator << read;
	while (sqliteOperator.Next()) {
		sqliteOperator >> recvId >> chatType >> msgId;
		if (ChatType_RoomChat == chatType) {
			unReadRecver.msgId = (uint64_t)msgId;
			unReadRecver.recvId = XStringToUTF8(recvId);
			unReadRecver.chatType = chatType;
			unReadRecvIds.push_back(unReadRecver);
		} else {
			if (checkContainRecvIdAndChatType(unReadRecvIds, recvId, chatType)) {
				continue;
			}
			unReadRecver.recvId = XStringToUTF8(recvId);
			unReadRecver.chatType = chatType;;
			unReadRecvIds.push_back(unReadRecver);
			//YouMe_LOG_Error(__XT("bruce >>> getRecvId size:%d"), unReadRecvIds.size());
			//for (int i=0; i < unReadRecvIds.size(); ++i) {
			//	YouMe_LOG_Error(__XT("bruce >>> recv_id:%s"), unReadRecvIds.at(i).recvId.c_str());
			//}
		}
	}
}

bool MessageRecorder::checkContainRecvIdAndChatType(std::vector<unReadRecvId_s>unReadRecvId, XString recvId, int chatType) {
	for(std::vector<unReadRecvId_s>::iterator itr = unReadRecvId.begin(); itr != unReadRecvId.end(); itr++) {
		if (itr->recvId == XStringToUTF8(recvId) && itr->chatType == chatType) {
			return true;
		}
	}
	return false;
}

void MessageRecorder::UpdateAllMessageRead(XString userID, int read)
{
    YouMeIMManager* pYouMeIMManager = (YouMeIMManager*)YouMeIMManager::CreateInstance();
    if (pYouMeIMManager == NULL)
    {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_dbOperateMutuex);
    youmecommon::CSqliteOperator sqliteOperator(m_sqliteDb);
    if (userID == __XT(""))
    {
        XString strSql = CStringUtilT<XCHAR>::formatString(__XT("update tb_message_%s set isRead=?1"), pYouMeIMManager->GetCurrentUser().c_str());
        sqliteOperator.PrepareSQL(strSql);
        sqliteOperator << read;
        sqliteOperator.Execute();
    }
    else
    {
        XString strSql = CStringUtilT<XCHAR>::formatString(__XT("update tb_message_%s set isRead=?1 where senderID=?2"), pYouMeIMManager->GetCurrentUser().c_str());
        sqliteOperator.PrepareSQL(strSql);
        sqliteOperator << read;
        sqliteOperator << userID;
        sqliteOperator.Execute();
    }
}

void MessageRecorder::UpdateRecverAllMessageReadWithChatType(XString sendId, int chatType)
{
    YouMeIMManager* pYouMeIMManager = (YouMeIMManager*)YouMeIMManager::CreateInstance();
    if (pYouMeIMManager == NULL)
    {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_dbOperateMutuex);
    youmecommon::CSqliteOperator sqliteOperator(m_sqliteDb);

	int read = 1;
	XString strSql = CStringUtilT<XCHAR>::formatString(__XT("update tb_message_%s set isRead=?1 where senderID=?2 and chatType=?3"), pYouMeIMManager->GetCurrentUser().c_str());
	sqliteOperator.PrepareSQL(strSql);
	sqliteOperator << read;
	sqliteOperator << sendId;
	sqliteOperator << chatType;
	sqliteOperator.Execute();
}

void MessageRecorder::UpdateSenderAllMessageReadWithChatType(XString recvId, int chatType)
{
    YouMeIMManager* pYouMeIMManager = (YouMeIMManager*)YouMeIMManager::CreateInstance();
    if (pYouMeIMManager == NULL)
    {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_dbOperateMutuex);
    youmecommon::CSqliteOperator sqliteOperator(m_sqliteDb);

	int read = 1;
	XString strSql = CStringUtilT<XCHAR>::formatString(__XT("update tb_message_%s set isRead=?1 where receiverID=?2 and chatType=?3"), pYouMeIMManager->GetCurrentUser().c_str());
	sqliteOperator.PrepareSQL(strSql);
	sqliteOperator << read;
	sqliteOperator << recvId;
	sqliteOperator << chatType;
	sqliteOperator.Execute();
}

void MessageRecorder::UpdateVoiceMsgPlayed(XUINT64 messageID, int played)
{
    YouMeIMManager* pYouMeIMManager = (YouMeIMManager*)YouMeIMManager::CreateInstance();
    if (pYouMeIMManager == NULL)
    {
        return;
    }
    
    XINT64 id = -1;
    int messageType = 0;
    {
        std::lock_guard<std::mutex> lock(m_dbOperateMutuex);
        youmecommon::CSqliteOperator sqliteOperator(m_sqliteDb);
        XString strSql = CStringUtilT<XCHAR>::formatString(__XT("select id,messageType from tb_message_%s where messageID=?1"), pYouMeIMManager->GetCurrentUser().c_str());
        sqliteOperator.PrepareSQL(strSql);
        sqliteOperator << (XINT64)messageID;
        if (sqliteOperator.Next())
        {
            sqliteOperator >> id >> messageType;
        }
        if (messageType == MessageBodyType_Voice)
        {
            if (-1 != id)
            {
                strSql = CStringUtilT<XCHAR>::formatString(__XT("update tb_message_%s set isPlayed=?1 where messageID=?2"), pYouMeIMManager->GetCurrentUser().c_str());
                sqliteOperator.PrepareSQL(strSql);
                sqliteOperator << played;
                sqliteOperator << (XINT64)messageID;
                sqliteOperator.Execute();
            }
            else
            {
                strSql = CStringUtilT<XCHAR>::formatString(__XT("update tb_message_%s set isPlayed=?1 where id=?2"), pYouMeIMManager->GetCurrentUser().c_str());
                sqliteOperator.PrepareSQL(strSql);
                sqliteOperator << played;
                sqliteOperator << (XINT64)messageID;
                sqliteOperator.Execute();
            }
        }
    }
}

void MessageRecorder::DeleteSpecifiedMessageRecord(XString targetID, YIMChatType chatType, std::vector<XUINT64>& excludeMesList)
{
    XString mesList;
    for (auto tmp:excludeMesList)
    {
        mesList += CStringUtilT<XCHAR>::to_string(tmp) + __XT(",");
    }
    YouMe_LOG_Debug(__XT("DeleteSpecifiedMessageRecord targetID:%s chatType:%d excludeMesList:%s"), targetID.c_str(), chatType,mesList.c_str());
    YouMeIMManager* pYouMeIMManager = (YouMeIMManager*)YouMeIMManager::CreateInstance();
    if (pYouMeIMManager == NULL)
    {
        return;
    }
    XString strSqlDel;
    XString strSqlPath;
    
    if (excludeMesList.empty())
    {
        strSqlDel = CStringUtilT<XCHAR>::formatString(__XT("delete from tb_message_%s where chatType=%d and senderID=\'%s\' or (senderID=\'%s\' and receiverID=\'%s\')"), pYouMeIMManager->GetCurrentUser().c_str(), chatType, targetID.c_str(), pYouMeIMManager->GetCurrentUser().c_str(), targetID.c_str());
        strSqlPath = CStringUtilT<XCHAR>::formatString(__XT("select localPath from tb_message_%s where chatType=%d and (senderID=\'%s\' or (senderID=\'%s\' and receiverID=\'%s\')) and localPath!=\'\'"), pYouMeIMManager->GetCurrentUser().c_str(), chatType, targetID.c_str(), pYouMeIMManager->GetCurrentUser().c_str(), targetID.c_str());
    }
    else
    {
        XString tmp = __XT("(");
        for(auto itr:excludeMesList)
        {
            tmp += __XT("'") + CStringUtilT<XCHAR>::to_string(itr) + __XT("',");
        }
        XString condition = tmp.substr(0,tmp.size()-1);
        condition += __XT(")");
        
        strSqlDel = CStringUtilT<XCHAR>::formatString(__XT("delete from tb_message_%s where chatType=%d and (senderID=\'%s\' or (senderID=\'%s\' and receiverID=\'%s\')) and messageID not in%s"), pYouMeIMManager->GetCurrentUser().c_str(), chatType, targetID.c_str(), pYouMeIMManager->GetCurrentUser().c_str(), targetID.c_str(), condition.c_str());
        strSqlPath = CStringUtilT<XCHAR>::formatString(__XT("select localPath from tb_message_%s where chatType=%d and (senderID=\'%s\' or (senderID=\'%s\' and receiverID=\'%s\')) and messageID not in%s and localPath!=\'\'"), pYouMeIMManager->GetCurrentUser().c_str(), chatType, targetID.c_str(),pYouMeIMManager->GetCurrentUser().c_str(), targetID.c_str(), condition.c_str());
    }
    
    {
        std::lock_guard<std::mutex> lock(m_dbOperateMutuex);
        youmecommon::CSqliteOperator sqliteOperator(m_sqliteDb);
        sqliteOperator.PrepareSQL(strSqlPath);
        sqliteOperator.Execute();
        while (sqliteOperator.Next())
        {
            XString strLocalPath;
            sqliteOperator >> strLocalPath;
            youmecommon::CXFile::remove_file(strLocalPath);
        }
        sqliteOperator.PrepareSQL(strSqlDel);
        sqliteOperator.Execute();
    }
}

void MessageRecorder::MessageRecordThread()
{
    while (true)
    {
        m_messageRecordWait.Decrement();
        if (m_bExit)
        {
            break;
        }
        MessageOperateInfo info;
        {
            std::lock_guard<std::mutex> lock(m_messageOperateMutuex);
            if (m_messageOperateMap.size() == 0)
            {
                continue;
            }
            std::list<MessageOperateInfo>::iterator begin = m_messageOperateMap.begin();
            if (begin != m_messageOperateMap.end())
            {
                info = *begin;
                m_messageOperateMap.erase(begin);
            }
        }
        
        if (TYPE_UNKNOW == info.type)
        {
            continue;
        }
        
        switch (info.type)
        {
                /*case TYPE_ADD_RECORD:
                 {
                 std::list<HistoryMessageInfo>::iterator itr = m_histroyMessageList.begin();
                 if (itr != m_histroyMessageList.end())
                 {
                 SaveHistoryMessage(*itr);
                 m_histroyMessageList.erase(itr);
                 }
                 }
                 break;*/
            case TYPE_QUERY_RECORD:
            {
                XString targetID;
                XUINT64 startMessageID = 0;
                short count = DEFAULT_ROW_NUM;
                short direction = 0;
                short chatType = 0;
                std::map<std::string, youmecommon::CXAny>::iterator itr = info.extend.find("TargetID");
                if (itr != info.extend.end())
                {
                    targetID = youmecommon::CXAny::XAny_Cast<XString>(itr->second);
                }
                itr = info.extend.find("StartMessageID");
                if (itr != info.extend.end())
                {
                    startMessageID = youmecommon::CXAny::XAny_Cast<XUINT64>(itr->second);
                }
                itr = info.extend.find("Count");
                if (itr != info.extend.end())
                {
                    count = youmecommon::CXAny::XAny_Cast<short>(itr->second);
                }
                itr = info.extend.find("Direction");
                if (itr != info.extend.end())
                {
                    direction = youmecommon::CXAny::XAny_Cast<short>(itr->second);
                }
                itr = info.extend.find("ChatType");
                if (itr != info.extend.end())
                {
                    chatType = youmecommon::CXAny::XAny_Cast<int>(itr->second);
                }
                
                QueryMessageRecord(targetID, startMessageID, count, direction, chatType);
            }
                break;
            case TYPE_DELETE_RECORD:
            {
                YIMChatType chatType = ChatType_Unknow;
                XUINT64 messageID = 0;
                XUINT64 time = 0;
                std::map<std::string, youmecommon::CXAny>::iterator itr = info.extend.find("ChatType");
                if (itr != info.extend.end())
                {
                    chatType = (YIMChatType)youmecommon::CXAny::XAny_Cast<int>(itr->second);
                }
                itr = info.extend.find("MessageID");
                if (itr != info.extend.end())
                {
                    messageID = youmecommon::CXAny::XAny_Cast<XUINT64>(itr->second);
                }
                itr = info.extend.find("Time");
                if (itr != info.extend.end())
                {
                    time = youmecommon::CXAny::XAny_Cast<XUINT64>(itr->second);
                }
                DeleteMessageRecord(chatType, messageID, time);
            }
                break;
            case TYPE_DELETE_RECORD2:
            {
                XString targetID;
                YIMChatType chatType = ChatType_Unknow;
                XUINT64 messageID = 0;
                int count = 0;
                std::map<std::string, youmecommon::CXAny>::iterator itr = info.extend.find("Target");
                if (itr != info.extend.end())
                {
                    targetID = youmecommon::CXAny::XAny_Cast<XString>(itr->second);
                }
                itr = info.extend.find("ChatType");
                if (itr != info.extend.end())
                {
                    chatType = (YIMChatType)youmecommon::CXAny::XAny_Cast<int>(itr->second);
                }
                itr = info.extend.find("MessageID");
                if (itr != info.extend.end())
                {
                    messageID = youmecommon::CXAny::XAny_Cast<XUINT64>(itr->second);
                }
                itr = info.extend.find("Count");
                if (itr != info.extend.end())
                {
                    count = youmecommon::CXAny::XAny_Cast<unsigned int>(itr->second);
                }
                DeleteMessageRecord(targetID, chatType, messageID, count);
            }
                break;
            case TYPE_QUERY_CONTACTS:
            {
                QueryContacts();
            }
                break;
            case TYPE_SET_MESSAGE_READ:
            {
                XUINT64 messageID = 0;
                int read = 0;
                std::map<std::string, youmecommon::CXAny>::iterator itr = info.extend.find("MessageID");
                if (itr != info.extend.end())
                {
                    messageID = youmecommon::CXAny::XAny_Cast<XUINT64>(itr->second);
                }
                itr = info.extend.find("Read");
                if (itr != info.extend.end())
                {
                    read = (YIMChatType)youmecommon::CXAny::XAny_Cast<int>(itr->second);
                }
                UpdateMessageRead(messageID, read);
            }
                break;
            case TYPE_DELETE_SPECIFIED_RECORD:
            {
                XString userID;
                YIMChatType chatType = ChatType_PrivateChat;
                std::vector<XUINT64> excludeMesList;
                std::map<std::string, youmecommon::CXAny>::iterator itr = info.extend.find("TargetID");
                if (itr != info.extend.end())
                {
                    userID = youmecommon::CXAny::XAny_Cast<XString>(itr->second);
                }
                itr = info.extend.find("ChatType");
                if (itr != info.extend.end())
                {
                    chatType = (YIMChatType)youmecommon::CXAny::XAny_Cast<int>(itr->second);
                }
                itr = info.extend.find("MessageList");
                if (itr != info.extend.end())
                {
                    excludeMesList = youmecommon::CXAny::XAny_Cast<std::vector<XUINT64>>(itr->second);
                }
                DeleteSpecifiedMessageRecord(userID, chatType, excludeMesList);
            }
                break;
            case TYPE_SET_ALL_MSG_READ:
            {
                int read = 1;
                XString userID;
                
                std::map<std::string, youmecommon::CXAny>::iterator itr = info.extend.find("UserID");
                if (itr != info.extend.end())
                {
                    userID = youmecommon::CXAny::XAny_Cast<XString>(itr->second);
                }
                itr = info.extend.find("Read");
                if (itr != info.extend.end())
                {
                    read = (YIMChatType)youmecommon::CXAny::XAny_Cast<int>(itr->second);
                }
                UpdateAllMessageRead(userID,read);
            }
			case TYPE_SET_SENDER_ALL_MSG_READ_WITH_CHAT_TYPE:
            {
                int chatType = 1;
                XString recvId;
                
                std::map<std::string, youmecommon::CXAny>::iterator itr = info.extend.find("UserID");
                if (itr != info.extend.end())
                {
                    recvId = youmecommon::CXAny::XAny_Cast<XString>(itr->second);
                }
                itr = info.extend.find("ChatType");
                if (itr != info.extend.end())
                {
                    chatType = (YIMChatType)youmecommon::CXAny::XAny_Cast<int>(itr->second);
                }
                UpdateSenderAllMessageReadWithChatType(recvId, chatType);
            }
                break;
			case TYPE_SET_RECVER_ALL_MSG_READ_WITH_CHAT_TYPE:
            {
                int chatType = 1;
                XString sendId;
                
                std::map<std::string, youmecommon::CXAny>::iterator itr = info.extend.find("UserID");
                if (itr != info.extend.end())
                {
                    sendId = youmecommon::CXAny::XAny_Cast<XString>(itr->second);
                }
                itr = info.extend.find("ChatType");
                if (itr != info.extend.end())
                {
                    chatType = (YIMChatType)youmecommon::CXAny::XAny_Cast<int>(itr->second);
                }
                UpdateRecverAllMessageReadWithChatType(sendId, chatType);
            }
                break;
            case TYPE_SET_VOICE_MSG_PLAYED:
            {
                int played = 1;
                XUINT64 messageID = 0;
                
                std::map<std::string, youmecommon::CXAny>::iterator itr = info.extend.find("MessageID");
                if (itr != info.extend.end())
                {
                    messageID = youmecommon::CXAny::XAny_Cast<XUINT64>(itr->second);
                }
                itr = info.extend.find("Played");
                if (itr != info.extend.end())
                {
                    played = (YIMChatType)youmecommon::CXAny::XAny_Cast<int>(itr->second);
                }
                UpdateVoiceMsgPlayed(messageID,played);
            }
                break;
            default:
                break;
        }
    }
}
