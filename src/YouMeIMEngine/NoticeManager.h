#pragma once


#include <ctime>
#include <YouMeIMEngine/YIM.h>
#include <YouMeIMEngine/YouMeIMCommonDef.h>
#include <YouMeIMEngine/pb/youme_notify.pb.h>
#include <YouMeCommon/SqliteOperator.h>

class YIMNoticeImpl : public YIMNotice
{
	friend class NoticeManager;
public:
	YIMNoticeImpl();
	~YIMNoticeImpl();
	virtual XUINT64 GetNoticeID() override;
	virtual const XCHAR* GetChannelID() override;
	virtual int GetNoticeType() override;
	virtual const XCHAR* GetContent() override;
	virtual const XCHAR* GetLinkText() override;
	virtual const XCHAR* GetLinkAddr() override;
	virtual unsigned int GetBeginTime() override;
	virtual unsigned int GetEndTime() override;

private:
	XUINT64 m_uNoticeID;
	int m_noticeType;
	XString m_strChannelID;
	XString m_strContent;
	XString m_strLinkeText;
	XString m_strLinkAddr;
	unsigned int m_uBeginTime;
	unsigned int m_uEndTime;
	XString m_strLoopWeekday;
	int m_nLoopType;
};

class YouMeIMManager;

class NoticeManager
{
public:
	NoticeManager();
	~NoticeManager();
	void Init();
	void Uninit();
	void SetNoticeCallback(IYIMNoticeCallback* callback);
	void OnReceNoticeNotify(YOUMEServiceProtocol::NotifyReq& rsp);
	YIMErrorcode QueryNotice(XUINT64 noticeID = 0);
	void OnQueryNoticeRsp(ServerPacket& serverPacket);

private:
	bool DeleteNotice(XUINT64 noticeID);
	void AddNotice(YIMNoticeImpl* notice);
	void QueryLocalNotice();
	unsigned int RelativeZeroHourSecond(std::string time);
	void UpdateMaxNoticeID(XUINT64 noticeID);
	std::time_t GetCurrentZeroHourStamp();
	unsigned int StringToTimestamp(std::string time);

	IYIMNoticeCallback* m_pCallback;
	youmecommon::CSqliteDb m_sqliteDb;
	bool m_bInit;
	std::mutex m_dbOperateMutuex;
	XUINT64 m_ullMaxNoticeID;
	std::vector<XUINT64> m_deleteNoticeIDs;
};