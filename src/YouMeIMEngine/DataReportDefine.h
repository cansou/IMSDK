#pragma once
#include <string>
#include <YouMeCommon/DataReport.h>
#include <YouMeCommon/XAny.h>
#include <YouMeIMEngine/ReportService.h>


//内部错误码
enum ReportErrorCode
{
	REPORTCODE_SUCCESS = 0,							// 成功
	REPORTCODE_UNKNOW_ERROR = 5000,					// 其他错误
	REPORTCODE_TIMEOUT = 5001,						// 超时	
	REPORTCODE_UNPACK_EEROR = 5002,					// 解包失败 
	REPORTCODE_SEND_PACKET_ERROR = 5003,			// 发送请求包失败
	REPORTCODE_PARSE_LOGIN_SECURET_EEROR = 5004,	// 解析登录返回消息秘钥失败
	REPORTCODE_GET_UPLOADTOKEN_ERROR = 5005,		// 获取token失败
	REPORTCODE_NO_HTTP_HEAD = 5006,					// 没有HTTP头
	REPORTCODE_UPLOAD_FAILED = 5007,				// 上传失败
	REPORTCODE_DOWNLOAD_FAILED = 5008,				// 下载失败
	REPORTCODE_SDKVALIDATE_ERROR = 5009,			// SDK验失败
	REPORTCODE_TRANSLATE_FAILED = 5010				// 翻译失败
};

enum CommonReporotType
{
	REPORTTYPE_RECONNECT = 0, //掉线重连
	REPORTTYPE_KICKOFF		//被踢
};

typedef CDataReport<ReportMessage, ReportParam> CDataReportInfo;

class CYouMeDataChannel;

class DataReport 
{
public:
	DataReport();
	~DataReport();
	bool Init(const std::string& addr, unsigned int tcpPort, unsigned int updPort, const std::string& defaultIP);
	void Report(const ReportMessage& reportMessage);
	void SetReportSignal();
	void AddReportInfo(ReportMessage& reportInfo);

private:
	void ReportThread();
#ifdef WIN32
	CYouMeDataChannel* m_pDataChannel;
#else
	static	CYouMeDataChannel* m_pDataChannel;
#endif
	
	bool m_bInit;
	bool m_bReportExit;
	std::thread m_reportThread;
	std::mutex m_mutex;
	youmecommon::CXCondWait m_reportWait;
	std::list<std::shared_ptr<ReportMessage> > m_reportList;
};


/*//初始化
struct ReportBodySDKValidate
{
	//2000 初始化
	short status;						//初始化状态（0:成功  -1:超时  其他错误码）
	unsigned int time;					//时长(MS)
	unsigned int SDKVersion;			//SDK版本			
	unsigned short serverZone;			//服务器区域
	unsigned short platform;			//平台（Unknow:0	Android:1	IOS:2	Windows:3	OSX:4	Linux:5）
	unsigned short networkType;			//网络类型（Unknow:0	Mobile:1	WIFI:2）
	std::string appkey;					//APPKEY
	std::string packageName;			//包名
	std::string systemVersion;			//系统版本
	std::string customSystemVersion;	//定制系统版本
	std::string deviceToken;			//设备标识
	std::string cpuArch;				//CPU架构
	std::string cpuChip;				//CPU芯片
	std::string brand;					//品牌
	std::string model;					//型号
	unsigned int createTime;			//上报时间
	unsigned int serverIp;				//ip地址

	void LoadToRecord(youmecommon::CRecord& record) const
	{
		record.SetData(status);
		record.SetData(time);
		record.SetData(SDKVersion);
		record.SetData(serverZone);
		record.SetData(platform);
		record.SetData(networkType);
		record.SetData(appkey.c_str());
		record.SetData(packageName.c_str());
		record.SetData(systemVersion.c_str());
		record.SetData(customSystemVersion.c_str());
		record.SetData(deviceToken.c_str());
		record.SetData(cpuArch.c_str());
		record.SetData(cpuChip.c_str());
		record.SetData(brand.c_str());
		record.SetData(model.c_str());
	}
};

//登录
struct ReportBodyLogin
{
	//2001 登录
	std::string userID;			//用户ID
	unsigned int appID;			//APP标识
	short status;				//登录状态（0:成功	-1:超时		其他错误码）
	unsigned short platform;	//平台（Unknow:0	Android:1	IOS:2	Windows:3	OSX:4	Linux:5）
	unsigned short networkType;	//网络类型（Unknow:0	Mobile:1	WIFI:2）
	unsigned int SDKVersion;	//SDK版本
	unsigned int time;			//时长(MS)
	unsigned int createTime;	//上报时间
	unsigned int serverIp;		//ip地址

	void LoadToRecord(youmecommon::CRecord& record) const
	{
		record.SetData(userID.c_str());
		record.SetData(appID);
		record.SetData(status);
		record.SetData(platform);
		record.SetData(networkType);
		record.SetData(SDKVersion);
		record.SetData(time);
	}
};

//掉线重连
struct ReportBodyReconnect
{
	//2002 掉线重连
	std::string userID;			//用户ID
	unsigned int appID;			//APP标识
	short reason;				//重连原因
	short status;				//状态（0:成功	-1:超时		其他:服务端错误码）

	void LoadToRecord(youmecommon::CRecord& record) const
	{
		record.SetData(userID.c_str());
		record.SetData(appID);
		record.SetData(reason);
		record.SetData(status);
	}
};


struct ReportBodyKickOff
{
	//2003 被踢
	std::string userID;			//用户ID
	unsigned int appID;			//APP标识

	void LoadToRecord(youmecommon::CRecord& record) const
	{
		record.SetData(userID.c_str());
		record.SetData(appID);
	}
};

//发送接收消息(只上报发送失败)
struct ReportBodyMessageStatus
{
	//2004 发送接收消息(只上报发送失败)
	std::string userID;			//用户ID
	unsigned int appID;			//APP标识
	short status;				//状态（0:成功	-1:超时		其他错误码）
	unsigned short messageType;	//消息类型
	unsigned int time;			//时长(MS)

	void LoadToRecord(youmecommon::CRecord& record) const
	{
		record.SetData(userID.c_str());
		record.SetData(appID);
		record.SetData(status);
		record.SetData(messageType);
		record.SetData(time);
	}
};

//成功接收语音消息（计费依据）
struct ReportBodyRecvAudioMessage
{
	//2005 语音消息
	std::string userID;			//用户ID
	unsigned int appID;			//APP标识

	void LoadToRecord(youmecommon::CRecord& record) const
	{
		record.SetData(userID.c_str());
		record.SetData(appID);
	}
};

//公用上报	以type区分具体类型，之后的保留字段根据type而不同
struct ReportCommon
{
	//2006
	std::string userID;			//用户ID
	unsigned int appID;			//APP标识
	unsigned int SDKVersion;	//SDK版本
	
	//0：掉线重连	reverse1:重连原因	reverse2:状态（0:成功	-1:超时		其他:服务端错误码）
	//1：被踢
	int type;
	int reverse1;
	int reverse2;
	std::string reverse3;
	unsigned int createTime;	//上报时间

	void LoadToRecord(youmecommon::CRecord& record) const
	{
		record.SetData(userID.c_str());
		record.SetData(appID);
		record.SetData(type);
		record.SetData(reverse1);
		record.SetData(reverse2);
		record.SetData(reverse3.c_str());
	}
};

struct ReportMessageState
{
	//2007 消息状态
	std::string userID;			//用户ID
	unsigned int appID;			//APP标识
	unsigned int SDKVersion;	//SDK版本

	int type;					//0:发送消息	1:接收消息		2:下载(URL)
	short status;				//状态（0:成功	-1:超时		其他错误码）
	unsigned short messageType;	//消息类型
	unsigned int time;			//时长(MS)
	unsigned int audioTime;		//语音时长(S)
	unsigned int createTime;	//上报时间

	void LoadToRecord(youmecommon::CRecord& record) const
	{
		record.SetData(userID.c_str());
		record.SetData(appID);
		record.SetData(status);
		record.SetData(messageType);
		record.SetData(time);
	}
};

struct ReportMessageState
{
	//2008 翻译
	std::string userID;			//用户ID
	unsigned int appID;			//APP标识
	unsigned int SDKVersion;	//SDK版本

	short status;				//状态
	std::string srcLanguage;	//源语言
	std::string destLanguage;	//目标语言
};

struct ReportMessageTimeCost
{
	//2012 消息耗时统计
	std::string userID;			//用户ID
	unsigned int appID;			//APP标识
	unsigned int SDKVersion;	//SDK版本

	unsigned int costTime;		//耗时（毫秒）
	XUINT64 sendSerial;			//发送端请求id
};

struct ReportSpeechRecognize
{
	//2013 语音识别调用统计
	std::string userID;			//用户ID
	unsigned int appID;			//APP标识
	unsigned int SDKVersion;	//SDK版本
	short type;					//语音识别类型 0：讯飞
	int status;					//状态
};

*/