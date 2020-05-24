#pragma once


#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeIMEngine/YIM.h>
#include <YouMeCommon/XSharedArray.h>
#include <YouMeCommon/XAny.h>
#include <map>
#include <mutex>

#define SDK_MAIN_VER "2.3.1"
#define SDK_VER 11158			//SDK版本
#define SERVICE_ID 20150418
#define SDK_NAME "YouMeIM"
#define PACKET_ENCRYT_KEY_LEN 16
#define PROTOCOL_VERSION 1

#define PROTOCOL_VERSION_HTTPS 2

//服务端配置KEY
#define	ACCESS_SERVER_ADDR "ACCESS_SERVER_ADDR"
#define	ACCESS_SERVER_PORT "ACCESS_SERVER_PORT"
#define ACCESS_SERVER_ADDR_PORT_ALL "ACCESS_SERVER_ADDR_PORT_ALL"
#define	APP_SERVICE_ID "APP_SERVICE_ID"
#define CONFIG_LOG_LEVEL "LOG_LEVEL"
#define CONFIG_LOG_CONSOLE_LEVEL "LOG_CONSOLE_LEVEL"
#define CONFIG_KEYWORD_URL "KeywordAddressV2"
#define CONFIG_KEYWORD_MD5 "KeywordMD5V2"
#define CONFIG_SERVER_TIME "SERVER_TIME"
#define CONFIG_HEART_TIMEOUT "HEART_TIMEOUT"
#define CONFIG_HEART_TIMOUT_COUNT "HEART_TIMEOUT_COUNT"
#define CONFIG_RECONNECT_COUNT "RECONNECT_COUNT"
#define CONFIG_RETRY_SEND_TIMES "RETRY_SEND_TIMES"
#define CONFIG_IFLYTEK_APPID_WIN "IFLYTEK_APPID_WIN"
#define CONFIG_IFLYTEK_APPID_IOS "IFLYTEK_APPID_IOS"
#define CONFIG_IFLYTEK_APPID_ANDROID "IFLYTEK_APPID_ANDROID"
#define CONFIG_RECOGNIZE_APPKEY "RECOGNIZE_APPKEY"				// 语音识别appkey
#define CONFIG_RECOGNIZE_APPSEC "RECOGNIZE_APPSEC"				// 语音识别secret
#define CONFIG_SAVE_GROUP_HISTORY_MSG "SAVE_GROUP_HISTORY_MSG"
#define CONFIG_TRANSLATE_ENABLE "TRANSLATE_ENABLE"
#define CONFIG_TRANSLATE_HOST "TRANSLATE_HOST"
#define CONFIG_TRANSLATE_CGI "TRANSLATE_CGI"
#define CONFIG_TRANSLATE_REGULAR "TRANSLATE_REGULAR"

#define CONFIG_TRANSLATE_METHOD "TRANSLATE_METHOD"
#define CONFIG_TRANSLATE_HOST_V2 "TRANSLATE_HOST_V2"
#define CONFIG_TRANSLATE_CGI_V2 "TRANSLATE_CGI_V2"
#define CONFIG_TRANSLATE_GOOGLE_APIKEY "TRANSLATE_GOOGLE_APIKEY"
#define CONFIG_MSG_PAGE_SIZE "MSG_PAGE_SIZE"
#define CONFIG_NEARBY_MAX_DISTANCE "NEARBY_MAX_DISTANCE"
#define CONFIG_KEEP_RECORD_MODEL "KEEP_RECORD_MODEL"
#define CONFIG_CLOSE_VOICE_TO_TEXT "CLOSE_VOICE_TO_TEXT"
#define CONIFG_MIC_VOLUME_GAIN "MIC_VOLUME_GAIN"
#define CONFIG_AUDIO_SAMPLERATE "AUDIO_SAMPLERATE"			// 采样率 8000 16000 32000 44100 48000
#define CONFIG_RECOGNIZE_TYPE "RECOGNIZE_TYPE"				// 语音识别方式 1: 讯飞录音 2:音频流
#define CONFIG_CODING_FORMAT "CODING_FORMAT"				// 编码格式 0：opus 1：amr
#define CONFIG_FONT_CONVERT_TYPE "FONT_CONVERT_TYPE"
#define CONFIG_LEXICON_MD5 "LEXICON_MD5"					// 繁体字库MD5
#define CONFIG_LEXICON_URL "LEXICON_URL"					// 繁体字库URL
#define CONFIG_RELATION_HEARTBEAT "RELATION_HEARTBEAT"		// 关系链用户状态心跳
#define CONFIG_AUDIO_ENCODE_BITRATE "AUDIO_ENCODE_BITRATE"	// 音频编码码率 16000 25000 32000 48000
#define CONFIG_UPLOAD_RETRY_TIMES "UPLOAD_RETRY_TIMES"		// 上传重试次数
#define CONFIG_MESSAGE_COST_STATISTIC "MESSAGE_COST_STATISTIC"	// 消息耗时统计

#define TRANSLATE_SWITCH_DEFAULT 0
#define TRANSLATE_METHOD_DEFAULT 0   // google翻译调用方法，0 - 破解版， 1 - 付费版
#define TRANSLATE_HOST_DEFAULT __XT("https://translate.google.com")
#define TRANSLATE_HOST_DEFAULT_V2 __XT("https://translation.googleapis.com")
#define TRANSLATE_CGI_PATH_DEFAULT __XT("/translate_a/")
#define TRANSLATE_CGI_PATH_DEFAULT_V2 __XT("/language/translate/")
#define TRANSLATE_REGULAR_DEFAULT __XT(".*TKK\\=eval\\(\\'\\(\\(function\\(\\)\\{var\\s+a\\\\x3d(-?\\d+);var\\s+b\\\\x3d(-?\\d+);return\\s+(\\d+)\\+.*")

#define TRANSLATE_GOOGLE_APIKEY_DEFAULT __XT("AIzaSyDv8POwQOtuIp6dIMxRmqL4RVNvMicaGy0")

struct ServerPacket
{
	uint16_t commondID;
	short result;
	uint64_t reqSerial;
	char* packetBuffer;
	uint32_t packetSize;
	std::map<std::string, std::string> extend;

	ServerPacket() : commondID(0), result(0), reqSerial(0), packetBuffer(NULL), packetSize(0){}
};

class YouMeIMManager;

class IPacketCallback
{
public:
	virtual void OnRecvPacket(ServerPacket& serverPacket) = 0;
};

//消息类型实现模板类
template <class T>
class BaseMessageBodyTypeT :public T
{
public:
	virtual YIMMessageBodyType GetMessageType()override
	{
		return m_messageType;
	}
protected:
	YIMMessageBodyType m_messageType;
};
//提供给外部的很多接口都有properties 这东西，抽象出来一个模板类，方便使用
template <class T>
class BasePropertiesT :public T
{
	friend class YouMeIMMessageManager;
public:
	virtual void SetProperties(const std::string& strKey, const XString& strValue) override
	{
		m_properties[strKey] = strValue;
	}
	virtual bool GetProperties(const std::string& strKey, XString& strValue) override
	{
		std::map<std::string, XString>::iterator it = m_properties.find(strKey);
		if (it == m_properties.end())
		{
			return false;
		}
		strValue = it->second;
		return true;
	}
private:
	std::map<std::string, XString> m_properties;
};

struct UDPTCPPacketInfo
{
	XINT64 ulSendTime;	//上一次发送时间,unix time 时间戳,单位ms
	int commandType;
	XINT64 ulTimeout;
	short retryTimes;	//重试次数
	bool encryptFlag;	// true:服务端下发秘钥加密	false:SYoumeConnHead加密
	youmecommon::CXSharedArray<char> pPacket;
	std::map<std::string, std::string> extend;
	UDPTCPPacketInfo()
	{
		commandType = 0;
		ulSendTime = 0;
		ulTimeout = 0;
		retryTimes = 0;
		encryptFlag = false;
	}
};

//标准管理器处理基类
class IManagerBase
{
public:
    
	IManagerBase(YouMeIMManager* pIManager)
	{
		m_pIManager = pIManager;
	}
	virtual void OnRecvPacket(ServerPacket& serverPacket) = 0;
    
    virtual ~IManagerBase(){};
	
protected:
	YouMeIMManager* m_pIManager;
};
