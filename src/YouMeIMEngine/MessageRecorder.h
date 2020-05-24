#pragma once

#include <thread>
#include <mutex>
#include <map>
#include <YouMeIMEngine/YIM.h>
#include <YouMeCommon/SqliteOperator.h>
#include <YouMeCommon/XAny.h>
#include <YouMeCommon/XSemaphore.h>

//最近联系人信息
class CContactsMessageInfo : public IYIMContactsMessageInfo
{
    friend class MessageRecorder;
public:
	CContactsMessageInfo() : m_createTime(0), m_messageType(MessageBodyType_Unknow){}
    //联系人
    virtual const XCHAR* GetContactID(){ return m_contactID.c_str(); }
    //消息时间
    virtual unsigned int GetCreateTime(){return static_cast<unsigned int>(m_createTime); }
    //消息内容
    virtual const XCHAR* GetMessageContent(){ return m_content.c_str(); }
    //消息类型
	virtual YIMMessageBodyType GetMessageType(){ return m_messageType; }
    //未读消息数量
    virtual unsigned int GetNotReadMsgNum(){ return m_notReadMsgNum; }
    //本地路径
    virtual const XCHAR* GetLocalPath(){return m_localPath.c_str();}
//    //额外信息
//    virtual const XCHAR* GetExtra(){return m_extra.c_str();}
    
	bool operator==(const CContactsMessageInfo& cmi)
	{
		return (cmi.m_senderID == m_receiverID && cmi.m_receiverID == m_senderID) || (cmi.m_senderID == m_senderID && cmi.m_receiverID == m_receiverID);
	}

private:
    XString m_contactID;
	XString m_content;
	XINT64 m_createTime;
	YIMMessageBodyType m_messageType;
	XString m_senderID;
	XString m_receiverID;
    unsigned int m_notReadMsgNum;
    XString m_localPath;
//    XString m_extra;
};

struct HistoryMessageInfo
{
	XUINT64 id;
	XUINT64 messageID;
	YIMMessageBodyType messageType;
	YIMChatType chatType;
	XINT64 createTime;
	XString senderID;
	XString receiverID;
	XString content;
	XString localPath;
	bool isRead;
    bool isPlayed;
	std::map<std::string, std::string> extend;

	HistoryMessageInfo() : id(0), messageID(0), messageType(MessageBodyType_Unknow), chatType(ChatType_Unknow), createTime(0), isRead(false){}
};

typedef struct unReadRecvId_t{
	std::string recvId;
	int chatType;
	uint64_t msgId;
}unReadRecvId_s;

enum OperateType
{
	TYPE_UNKNOW,
	TYPE_ADD_RECORD,
	TYPE_QUERY_RECORD,
	TYPE_DELETE_RECORD,
	TYPE_DELETE_RECORD2,
	TYPE_QUERY_CONTACTS,
	TYPE_SET_MESSAGE_READ,
    TYPE_DELETE_SPECIFIED_RECORD,
    TYPE_SET_ALL_MSG_READ,
	TYPE_SET_SENDER_ALL_MSG_READ_WITH_CHAT_TYPE,
	TYPE_SET_RECVER_ALL_MSG_READ_WITH_CHAT_TYPE,
    TYPE_SET_VOICE_MSG_PLAYED
};

struct MessageOperateInfo 
{
	OperateType type;
	std::map<std::string, youmecommon::CXAny> extend;
	MessageOperateInfo() : type(TYPE_UNKNOW){}
};

class MessageRecorder
{
public:
	MessageRecorder();
	~MessageRecorder();
	void Init();
	void UnInit();

	bool SaveHistoryMessage(HistoryMessageInfo& recordInfo, bool fromServerHistory=false);
	bool SaveHistoryMessage(IYIMMessage* message, bool fromServerHistory,XINT64 autoIncrementID,XINT64 currentTime);

	YIMErrorcode QueryHistoryMessage(XString targetID, XUINT64 startMessageID, short count, short direction, int chatType);
	bool UpdateMessagePath(XUINT64 messageID, const XString path,  XUINT64 autoIncrementID, XUINT64 currentTimeMs);
	YIMErrorcode DeleteHistoryMessage(int chatType, XUINT64 messageID, XUINT64 time);
	YIMErrorcode DeleteHistoryMessage(XString targetID, int chatType, XUINT64 startMessageID, unsigned int count);
	YIMErrorcode QueryRecentContacts();
	YIMErrorcode SetMessageRead(XUINT64 messageID, bool read);
    YIMErrorcode SetAllMessageRead(XString userID, bool read);
	YIMErrorcode SetSenderAllMessageReadWithChatType(XString userID, int chatType);
	YIMErrorcode SetRecverAllMessageReadWithChatType(XString userID, int chatType);
    YIMErrorcode SetVoiceMsgPlayed(XUINT64 messageID, bool played);
    YIMErrorcode DeleteSpecifiedHistoryMessage(XString targetID, int chatType, const std::vector<XUINT64>& excludeMesList);
	void getRecvId(int read, std::vector<unReadRecvId_s> &unReadRecvIds);

private:
	void MessageRecordThread();
	void QueryMessageRecord(XString targetID, XUINT64 startMessageID, short count, short direction, int chatType);
	void DeleteMessageRecord(YIMChatType chatType, XUINT64 messageID, XUINT64 time);
	void DeleteMessageRecord(XString& targetID, YIMChatType chatType, XUINT64 startMessageID, unsigned int count);
	void QueryContacts();
	void UpdateMessageRead(XUINT64 messageID, int read);
    void UpdateAllMessageRead(XString userID, int read);
	void UpdateRecverAllMessageReadWithChatType(XString sendId, int chatType);
	void UpdateSenderAllMessageReadWithChatType(XString recvId, int chatType);
    void UpdateVoiceMsgPlayed(XUINT64 messageID, int played);
    void DeleteSpecifiedMessageRecord(XString targetID, YIMChatType chatType, std::vector<XUINT64>& excludeMesList);
	bool checkContainRecvIdAndChatType(std::vector<unReadRecvId_s>unReadRecvId, XString recvId, int chatType);
	
	youmecommon::CSqliteDb m_sqliteDb;
	bool m_bInit;
	bool m_bExit;
	std::thread m_thread;
    std::mutex m_threadMutuex;
	std::mutex m_dbOperateMutuex;
	std::list<MessageOperateInfo> m_messageOperateMap;
	std::mutex m_messageOperateMutuex;
	youmecommon::CXSemaphore m_messageRecordWait;
};
