#pragma once

#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include "YouMeIMCommonDef.h"
#include <memory>
#include <map>
#include <mutex>
#include <thread>
#include <atomic>
#include <YouMeCommon/XSemaphore.h>
#include <YouMeCommon/DownloadUploadManager.h>
#include <YouMeIMEngine/pb/youme_getmsg.pb.h>
#include <YouMeIMEngine/pb/server_protocol_code.h>
#include <YouMeIMEngine/MessageRecorder.h>


#define BITRATE_16K 16000
#define BITRATE_25K 25000
#define BITRATE_32K 32000
#define BITRATE_48K 48000

struct MessageBufferInfo 
{
	YOUMEServiceProtocol::COMMOND_TYPE commandType;
	int chatType;
	std::string receiverID;
	std::string messageBuffer;
	std::map<std::string, std::string> extend;

	MessageBufferInfo() : commandType(YOUMEServiceProtocol::CMD_UNKNOW), chatType(0){}
};

enum BucketType
{
	BUCKET_TYPE_AWS,
	BUCKET_TYPE_OSS,
	BUCKET_TYPE_COS
};

struct FileUploadInfo
{
	XUINT64 msgSerial;
	YIMChatType chatType;
	YIMFileType fileType;
	unsigned int fileSize;
	BucketType bucketType;
	XString receiverID;
	XString localPath;
	XString uploadURL;
	XString downloadURL;
	XString extension;
	XString extraParam;
    XUINT64 autoIncrementID;
    XUINT64 startSendTime;
	std::map<std::string, std::string> httpHead;
	std::map<std::string, std::string> extend;

	FileUploadInfo() : msgSerial(0), chatType(ChatType_Unknow), fileType(FileType_Other), fileSize(0){}
};

struct FileDownloadInfo
{
	XUINT64 msgSerial;
	YIMFileType fileType;
	XString downloadURL;
	XString savePath;
    std::shared_ptr<IYIMMessage>  msg;
	std::map<std::string, std::string> extend;

	FileDownloadInfo() : msgSerial(0), fileType(FileType_Other){}
};


class CYouMeIMMessageBodyCustom :public BaseMessageBodyTypeT<IYIMMessageBodyCustom>
{
	friend class YouMeIMMessageManager;
public:
	CYouMeIMMessageBodyCustom()
	{
		m_messageType = MessageBodyType_CustomMesssage;
	}
	void SetCustomMessage(const std::string& strCustomMessage);
	virtual std::string GetCustomMessage() override;
	
private:
	std::string m_strData;
};


class CYouMeIMMessageBodyText : public BaseMessageBodyTypeT<IYIMMessageBodyText>
{
	friend class YouMeIMMessageManager;
public:
	CYouMeIMMessageBodyText()
	{
		m_messageType = MessageBodyType_TXT;
	}
	virtual const XCHAR* GetMessageContent() override;
    virtual const XCHAR* GetAttachParam() override;
	void SetMessageContent(const XString& strContent);
private:
	XString m_strContent;
    XString m_strAttachParam;
};

class CYouMeIMMessageBodyAudio: public BaseMessageBodyTypeT<IYIMMessageBodyAudio>
{
    friend class YouMeIMMessageManager;
public:
    CYouMeIMMessageBodyAudio()
    {
        m_messageType = MessageBodyType_Voice;
		m_iFileSize = 0;
		m_iTime = 0;
        m_isPlayed = false;
    }
	virtual const XCHAR* GetText() override;
	virtual const XCHAR* GetExtraParam() override;
	virtual unsigned int GetFileSize() override;
	virtual unsigned int GetAudioTime() override;
	virtual const XCHAR* GetLocalPath() override;
    virtual bool IsPlayed() override;
	void Play();
	XString GetDownloadURL(){ return m_strDownloadURL; }
private:
    XString m_strText;
	XString m_strDownloadURL;
    XString m_strLocalPath;
	XString m_strParam;
	unsigned int m_iFileSize;
	unsigned int m_iTime;
    bool m_isPlayed;
};

class CYouMeIMMessageBodyFile : public BaseMessageBodyTypeT<IYIMMessageBodyFile>
{
	friend class YouMeIMMessageManager;
public:
	CYouMeIMMessageBodyFile()
	{
		m_messageType = MessageBodyType_File;
		m_fileType = FileType_Other;
		m_iFileSize = 0;
	}

	virtual unsigned int GetFileSize() override;
	virtual const XCHAR* GetFileName() override;
	virtual const XCHAR* GetFileExtension() override;
	virtual YIMFileType GetFileType() override;
	virtual const XCHAR* GetExtraParam() override;
	virtual const XCHAR* GetLocalPath() override;
private:
	XString m_strDownloadURL;
	XString m_strLocalPath;
	XString m_strExtension;
	XString m_strFileName;
	XString m_strParam;
	YIMFileType m_fileType;
	unsigned int m_iFileSize;
};

class CYouMeIMMessageBodyGift : public BaseMessageBodyTypeT<IYIMMessageGift>
{
	friend class YouMeIMMessageManager;
public:
	CYouMeIMMessageBodyGift() : m_iGiftID(0), m_iCount(0){}
	//礼物ID
	virtual int GetGiftID() override;
	//数量
	virtual unsigned int GetGiftCount() override;
	//主播
	virtual const XCHAR* GetAnchor() override;
	//附加字段
	virtual const XCHAR* GetExtraParam() override;
private:
	int m_iGiftID;
	unsigned int m_iCount;
	XString m_strAnchorID;
	XString m_strParam;
};

class CYouMeIMMessage :public IYIMMessage
{
    friend class YouMeIMMessageManager;
public:
	CYouMeIMMessage();
	~CYouMeIMMessage();
	//聊天类型， 私聊，群组，聊天室
	virtual YIMChatType GetChatType() override;
	void SetChatType(YIMChatType chatType);
	//聊天对象， 可以是好友ID， 群ID
	virtual const XCHAR* GetReceiveID() override;
	void SetReceiveID(XString& receiverID);
	//消息体
	void SetMessageBody(IYIMMessageBodyBase* pBody);
	virtual IYIMMessageBodyBase* GetMessageBody() override;
	//这个只能是发送者的ID
	void SetSenderID(XString& senderID);
	virtual const XCHAR* GetSenderID() override;
    //获取消息ID
	virtual XUINT64 GetMessageID() override;
    virtual XUINT64 GetID() override;
	//发送时间（秒）
	virtual unsigned int GetCreateTime() override;
	virtual unsigned int GetDistance() override;
	virtual bool IsRead() override;
	void SetTime(unsigned int createTime);
private:
	YIMChatType m_chatType;
	XString m_strReceiveID;
    XUINT64 m_ulID;
	XUINT64 m_ulMsgSerial;
	XString m_strSenderid;
	unsigned int m_iCreateTime;
	unsigned int m_iDistance;
	IYIMMessageBodyBase* m_pMessageBody;
	bool m_bIsRead;
};

class CAudioSpeechInfo : public IAudioSpeechInfo
{
	friend class YouMeIMMessageManager;
public:
	CAudioSpeechInfo() : m_iFileSize(0), m_iTime(0), m_requestID(0){}
	virtual XUINT64 GetRequestID(){ return m_requestID; }
	virtual const XCHAR* GetText(){ return m_strText.c_str(); }
	virtual unsigned int GetFileSize(){return m_iFileSize;}
	virtual unsigned int GetAudioTime(){ return m_iTime; }
	virtual const XCHAR* GetLocalPath(){ return m_strLocalPath.c_str(); }
	virtual const XCHAR* GetDownloadURL(){ return m_strDownloadURL.c_str(); }
private:
	unsigned int m_iFileSize;
	unsigned int m_iTime;
	XUINT64 m_requestID;
	XString m_strText;
	XString m_strDownloadURL;
	XString m_strLocalPath;
};

struct TranslateInfo 
{
	unsigned int requestID;
	LanguageCode srcLangCode;
	LanguageCode destLangCode;
	XString text;
	TranslateInfo() : requestID(0), srcLangCode(LANG_AUTO), destLangCode(LANG_AUTO){}
};

class CForbiddenSpeakInfo : public  IYIMForbidSpeakInfo
{
    friend class YouMeIMMessageManager;
public:
    const XCHAR* GetChannelID() { return m_channelID.c_str();}
    virtual bool GetIsForbidRoom() {return m_isForbidRoom;}
    virtual int GetReasonType() {return m_reasonType;}
    virtual XUINT64 GetEndTime() { return m_endTime ;}
private:
    XString m_channelID;
    bool m_isForbidRoom;
    int  m_reasonType;
    XUINT64 m_endTime;
};

//编码格式
enum CodingFormat
{
	CODING_OPUS = 0,  //opus编解码
	CODING_AMR = 1    //amr编解码
};

class YouMeIMMessageManager:
	public YIMMessageManager,
	public IManagerBase,
	public IUploadProgressCallback
{
	friend class YouMeIMManager;
    friend class CYouMeIMMessage;
public:
	YouMeIMMessageManager(YouMeIMManager* pIManager);
	YIMErrorcode Init();
	void UnInit();
   //设置回掉
	void SetMessageCallback(IYIMMessageCallback* pCallback);
	IYIMMessageCallback* GetMessageCallback(){ return m_pCallback; }
	void SetDownloadCallback(IYIMDownloadCallback* pCallback);
	void SetUpdateReadStatusCallback(IYIMUpdateReadStatusCallback* pCallback);

	//一些高级接口，直接调用就可以发送，
	YIMErrorcode SendTextMessage(const XCHAR* receiverID, YIMChatType chatType, const XCHAR* text, const XCHAR* attachParam, XUINT64* requestID) override;
	YIMErrorcode MultiSendTextMessage(const std::vector<XString>& vecReceiver, const XCHAR* text) override;
	YIMErrorcode SendCustomMessage(const XCHAR* receiverID, YIMChatType chatType, const char* content, unsigned int size, XUINT64* requestID) override;
	void onUploadProgressCallback(void * param, float percent) override;
	YIMErrorcode SendFile(const XCHAR* receiverID, YIMChatType chatType, const XCHAR* filePath, XUINT64* requestID, const XCHAR* extraParam, YIMFileType fileType) override;
    //语音PTT
	YIMErrorcode SendAudioMessage(const XCHAR* receiverID, YIMChatType chatType, XUINT64* requestID) override;
	YIMErrorcode SendOnlyAudioMessage(const XCHAR* receiverID, YIMChatType chatType, XUINT64* requestID) override;
	YIMErrorcode StopAudioMessage(const XCHAR* extraParam) override;
    YIMErrorcode CancleAudioMessage() override;
	YIMErrorcode SendGift(const XCHAR* anchorID, const XCHAR* channel, int giftId, int giftCount, const char* extraParam, XUINT64* requestID) override;
	YIMErrorcode DownloadFile(XUINT64 messageID, const XCHAR* savePath) override;
	YIMErrorcode DownloadFile(const XCHAR* downloadURL, const XCHAR* savePath,YIMFileType fileType=FileType_Audio) override;
	YIMErrorcode SetDownloadDir(const XCHAR* path) override;
	//同步下载文件
	YIMErrorcode DownloadFileSync(XUINT64 iMessageID, const XString& strSavePath);

	YIMErrorcode StartAudioSpeech(XUINT64* requestID, bool translate = true) override;
	YIMErrorcode StopAudioSpeech() override;
	YIMErrorcode ConvertAMRToWav(const XCHAR* amrFilePath, const XCHAR* wavFielPath = NULL) override;
	
	YIMErrorcode QueryHistoryMessage(const XCHAR* targetID, YIMChatType chatType, XUINT64 startMessageID = 0, int count = 30, int direction = 0) override;
	YIMErrorcode DeleteHistoryMessage(YIMChatType chatType = ChatType_Unknow, XUINT64 time = 0) override;
	YIMErrorcode DeleteHistoryMessage(const XCHAR* targetID, YIMChatType chatType, XUINT64 startMessageID = 0, unsigned int count = 0) override;
	YIMErrorcode DeleteHistoryMessageByID(XUINT64 messageID) override;
    //删除指定用户的本地聊天历史记录，保留指定的消息ID列表记录
    YIMErrorcode DeleteSpecifiedHistoryMessage(const XCHAR* targetID, YIMChatType chatType, const std::vector<XUINT64>& excludeMesList) override;
	YIMErrorcode QueryRoomHistoryMessageFromServer(const XCHAR* roomID, int count, int direction) override;
	YIMErrorcode GetNewMessage(const std::vector<XString>& vecRoomIDs) override;
	void SynMsgReadStatus() override;
    
    //设置自动下载语音消息
    YIMErrorcode SetDownloadAudioMessageSwitch(bool download) override
    {
        m_downloadFlag = download;
        YouMe_LOG_Info(__XT("set download audio message automatically: %d"), download);
        return YIMErrorcode_Success;
    }
    
	YIMErrorcode SetReceiveMessageSwitch(const std::vector<XString>& vecRoomIDs, bool receive) override;
	YIMErrorcode SetRoomHistoryMessageSwitch(const std::vector<XString>& vecRoomIDs, bool save) override;
	YIMErrorcode SetRoomHistoryMessageSwitch(const XCHAR* roomID, bool save) override;

	YIMErrorcode SetMessageRead(XUINT64 messageID, bool read) override;
    YIMErrorcode SetAllMessageRead(const XCHAR* userID, bool read) override;
	YIMErrorcode SendMessageReadStatus(const XCHAR* userID, int chatType, XUINT64 messageID) override;
    
    // 设置语音消息为已播放,true-已播放，false-未播放
    YIMErrorcode SetVoiceMsgPlayed(XUINT64 messageID, bool played) override;
    
    //是否暂停IM的消息接收(true: 暂停IM的消息接收(默认)， false: 恢复IM的暂停接收)
    YIMErrorcode PauseReceiveMessage(bool pause) override;
    
	YIMErrorcode TranslateText(unsigned int* requestID, const XCHAR* text, LanguageCode destLangCode, LanguageCode srcLangCode) override;

	YIMErrorcode SetSpeechRecognizeLanguage(SpeechLanguage language) override;
    
    //设置仅识别语音文字，不发送语音消息; false:识别语音文字并发送语音消息，true:仅识别语音文字
    YIMErrorcode SetOnlyRecognizeSpeechText(bool recognition) override;

	YIMErrorcode Accusation(const XCHAR* userID, YIMChatType source, int reason, const XCHAR* description, const XCHAR* extraParam) override;
    
    YIMErrorcode GetForbiddenSpeakInfo() override;

	YIMErrorcode BlockUser(const XCHAR* userID, bool block) override;

	YIMErrorcode UnBlockAllUser() override;

	YIMErrorcode GetBlockUsers() override;

	YIMErrorcode SwitchMsgTransType(YIMMsgTransType transType) override;

	YIMErrorcode GetRecentContacts();
	void OnQueryHistoryMessage(YIMErrorcode errorcode, XString targetID, std::list<std::shared_ptr<HistoryMessageInfo> >& messageRecord, int remain);

	XINT64 LastGetMessageTime(){ return m_llLastGetMessageTime; }
	void GetNewMessage(bool initiative);

	void UpdateConfig();
	void InitMessageManager();

	void OnSpeechResult(YIMErrorcode errorcode, const XString& path, unsigned long long serial, const XString& text);
	void OnRecordVolumeChange(float volume);
	YIMErrorcode RequestWeiXinAccessToken();
	XUINT64 GetWeiXinTokenSerial(){ return m_ullWeiXinTokenSerial; }

private:
	virtual void OnRecvPacket(ServerPacket& serverPacket) override;

	bool SendMessage(MessageBufferInfo& messageInfo, XUINT64& msgSerial);
	void OnSendMessageRsp(ServerPacket& serverPacket);
	void OnReceiveMessageNotify(ServerPacket& serverPacket);
	void OnReceiveMessageRsp(ServerPacket& serverPacket);
	bool ReceiveMessage(const YOUMEServiceProtocol::YoumeMsg& message, bool roomHistoryMessage = false);
	bool ResolveMessage(const YOUMEServiceProtocol::YoumeMsg& message, std::shared_ptr<CYouMeIMMessage>& pMessage);
	YIMErrorcode RequestUploadToken(FileUploadInfo& uploadInfo);
	void OnGetUploadTokenRsp(ServerPacket& serverPacket);
	YIMErrorcode SendAudioMessage(FileUploadInfo& uploadInfo);
	YIMErrorcode SendFileMessage(FileUploadInfo& uploadInfo,XUINT64 audoid,XUINT64 startSendtime);
	YIMErrorcode RequestGetMessage(XUINT64 messageSerial, std::string& targets, bool flag = false);
	YIMErrorcode RequestRoomHistoryMessage(XString roomID);
	void OnRoomHistoryMessageRsp(ServerPacket& serverPacket);
	void OnAccusationRsp(ServerPacket& serverPacket);
	void OnAccusationDealNotify();
	void OnGetAccusationDealResultRsp(ServerPacket& serverPacket);
	void OnBlockUserRsp(ServerPacket& serverPacket);
	void RequestBlockUserList(XUINT64 serialNo, bool internalCall = false);
	void OnGetBlockUserListRsp(ServerPacket& serverPacket);
	void OnUnBlockAllUserRsp(ServerPacket& serverPacket);
    void OnGetForbiddenSpeakInfo( ServerPacket& serverPacket );
	void OnWeiXinAccessTokenRsp(ServerPacket& serverPacket);
	void SetAllMessageReadRsp(ServerPacket& serverPacket);
	void OnReceiveMessageReadStatusNotify(ServerPacket& serverPacket);
	void OnSynReadMsgStatus(ServerPacket& serverPacket);

	void UpdateRecvMessageSerial(XUINT64 messageSerial);
	XString GetSavePath(const XString& url, const XString savePath);
    
	void UploadThreadProc();
	void DownloadThreadProc();
	
	void SaveSenderHistoryMessage(XUINT64 messageSerial, MessageBufferInfo& messageInfo);

	void TranslateThread();

	void ReportDataMessageStatus(int type, int messageType, int status, unsigned int time, unsigned int audioTime);
	void ReportTranslateStatus(short status, const XString& srcLanguage, const XString& destLanguage, XUINT64 chaterCount, int translateMethod);
	void ReportDataMessageTimeCost(unsigned int timeCost, XUINT64 sendSerial);

	IYIMMessageCallback* m_pCallback;
	IYIMDownloadCallback* m_pDownloadCallback;
	IYIMUpdateReadStatusCallback* m_pUpdateReadStatusCallback = NULL;
	bool m_bInit;

	//上传文件
	bool m_bExitUpload;
	std::thread m_uploadThread;
	youmecommon::CXSemaphore m_uploadSemaphore;
	std::mutex m_uploadInfoLock;
	std::map<XUINT64, std::shared_ptr<FileUploadInfo> > m_fileUploadMap;

	bool m_bExitDownload;
	std::thread m_downloadThread;
	std::mutex m_DownloadInfoLock;
	youmecommon::CXSemaphore m_downloadSemaphore;
	std::map<XUINT64, FileDownloadInfo> m_downloadInfo;
	std::list<FileDownloadInfo> m_downloadList;
	static XString m_strDownloadDir;
	
	//离线消息只能获取一次。否则就要出bug了
	bool m_bIsGetOfflineMsg;
	//语音 结束的时候带上的参数，由于每次只能启动一个，所以用一个成员变量
	XString m_strSendAudioParam;
	XUINT64 m_ullAudioMsgSerial;

	bool m_bGetMessageFinish;
	bool m_bMessageNotify;
	std::map<std::string, std::set<XUINT64> > m_senderSerialMap;	//保存发送者message serial，防止重复接收消息
	std::set<XUINT64> m_messageIdSet;	// 保存当前用户MessageID，过滤重复消息
	XINT64 m_llLastGetMessageTime;
	XUINT64 m_ullLastMessageID;

	std::map<XUINT64, MessageBufferInfo> m_sendMessageRecord;
	std::mutex m_messageRecordLock;
	MessageRecorder m_messageRecorder;
	static std::set<XString> m_msgRecordRooms;
	static std::set<XString> m_msgShieldTargets;	//屏蔽自动接收消息
    
    bool m_downloadFlag; //是否自动下载的标志
    bool m_pauseReceivingFlag; //是否暂停接收IM消息的标志

	int m_iSendReportCount;
	int m_iRecvReportCount;

	std::map<LanguageCode, XString> m_langCodeMap;
	std::thread m_translateThread;
	std::mutex m_translateLock;
	static unsigned int m_iTranslateRequestID;
	std::list<TranslateInfo> m_translateInfoList;
	youmecommon::CXSemaphore m_translateSemaphore;
	
	bool m_bTranslateThreadExit;
    
	short m_iMsgPageSize;
    
    unsigned int m_bMicVolumeGain;      //麦克风采集的声音是否启用增益
	int m_iUploadRetryTimes;	//上传重试次数
        
    bool m_onlyRecognizeSpeechText; //是否只识别语音
	unsigned int m_msgTransType;    //消息传输类型，0-普通，1-抄送
    CodingFormat m_codingFormat;    //编解码格式
    int m_audioEncoderBitRate;      //音频编码器码率

	static std::set<XString> m_blockUserList;
	static bool m_bRequestBlockList;
	static XString m_strLastUserID;

	static std::string m_strWeiXinTocken;
	static XUINT64 m_ullWeiXinTokenSerial;
};

