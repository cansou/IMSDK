#pragma once
#include <string>
#include "YouMeCommon/DataReport.h"

/**
 * ReportService.h
 * Created by python on 2017/04/20.
 * Copyright © 2017年 Youme. All rights reserved.
 * 数据上报服务
 * 数据上报命令字和版本
 * 命令字和版本的值须先从服务器配置后取得，请勿随意修改！
 * 配置地址：http://t.ops.nxit.us/home
 */

/**
 * 数据上报参数
 */
class ReportParam {
public:
    ReportParam() { }
    ~ReportParam() { }
    
public:
    static std::string m_strIdentify;
    static unsigned int m_uiTcpPort;
    static unsigned int m_uiUdpPort;
    static XString m_strDomain;

public:
    static void setParam(std::string identify, unsigned int tcpport, unsigned int uudpport, XString domain) {
        ReportParam::m_strIdentify = identify;
        ReportParam::m_uiTcpPort= tcpport;
        ReportParam::m_uiUdpPort = uudpport;
        ReportParam::m_strDomain = domain;
    }
    
    static void setIdentify(std::string identify) {
        ReportParam::m_strIdentify = identify;
    }
    
    static void setTcpPort(unsigned int tcpport) {
        ReportParam::m_uiTcpPort= tcpport;
    }
    
    static void setUdpPort(unsigned int uudpport) {
        ReportParam::m_uiUdpPort = uudpport;
    }
    
    static void setDomain(XString domain) {
        ReportParam::m_strDomain = domain;
    }
};

/**
 * 通用消息头
 */
class ReportMessage {
public:
    ReportMessage() {
		cmdid = 0;
		version = 1;
		bUseTcp = true;
	}
    ~ReportMessage() { }
    
public:
    unsigned short cmdid;   /* 命令字     */
    unsigned short version  ; /* 版本       */
    bool bUseTcp ;        /* 是否使用tcp*/

public:  /* RTC上报公共字段 */
    unsigned int  appid;
    std::string userid;

public:
    virtual void LoadToRecord(youmecommon::CRecord & record) const { };
};


/**
 * 业务逻辑上报，客户端使用时候主动触发
 * TCP
 */

/********************************************************************************/
/* [CMD:2000] [VERSION:1] */
class ReportSDKValidate : public ReportMessage {
public:
    ReportSDKValidate()
    {
        cmdid = 2000;
        version = 1;
        bUseTcp = true;
    }

    ~ReportSDKValidate() { }

public:
    unsigned short  status;  /* 初始化状态 */
    unsigned int  time;  /* 时长 */
    unsigned int  SDKVersion;  /* SDK版本 */
    unsigned short  serverZone;  /* 服务器区域 */
    unsigned short  platform;  /* 平台 */
    unsigned short  networkType;  /* 网络类型 */
    std::string  appkey;  /* APPKEY */
    std::string  packageName;  /* 包名 */
    std::string  systemVersion;  /* 系统版本 */
    std::string  customSystemVersion;  /* 定制系统版本 */
    std::string  deviceToken;  /* 设备标识 */
    std::string  cpuArch;  /* CPU架构 */
    std::string  cpuChip;  /* CPU芯片 */
    std::string  brand;  /* 品牌 */
    std::string  model;  /* 型号 */
    unsigned int  createTime;  /* 上报时间 */
    unsigned int  serverIp;  /* ip地址 */
     

public:
    void LoadToRecord(youmecommon::CRecord & record) const
    {
        record.SetData(appid);
        record.SetData(userid.c_str());
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
        record.SetData(createTime);
        record.SetData(serverIp);
        
    }
};

/**
 * 业务逻辑上报，客户端使用时候主动触发
 * TCP
 */

/********************************************************************************/
/* [CMD:2001] [VERSION:1] */
class ReportLogin : public ReportMessage {
public:
    ReportLogin()
    {
        cmdid = 2001;
        version = 1;
        bUseTcp = true;
    }

    ~ReportLogin() { }

public:
    unsigned short  status;  /* 登录状态 */
    unsigned short  platform;  /* 平台 */
    unsigned short  networkType;  /* 网络类型 */
    unsigned int  SDKVersion;  /* SDK版本 */
    unsigned int  time;  /* 时长 */
    unsigned int  createTime;  /* 上报时间 */
    unsigned int  serverIp;  /* ip地址 */
     

public:
    void LoadToRecord(youmecommon::CRecord & record) const
    {
        record.SetData(appid);
        record.SetData(userid.c_str());
        record.SetData(status);
        record.SetData(platform);
        record.SetData(networkType);
        record.SetData(SDKVersion);
        record.SetData(time);
        record.SetData(createTime);
        record.SetData(serverIp);
        
    }
};

/**
 * 业务逻辑上报，客户端使用时候主动触发
 * UDP
 */

/********************************************************************************/
/* [CMD:2006] [VERSION:1] */
class ReportCommon : public ReportMessage {
public:
    ReportCommon()
    {
        cmdid = 2006;
        version = 1;
        bUseTcp = false;
    }

    ~ReportCommon() { }

public:
    unsigned int  SDKVersion;  /* SDK版本 */
    unsigned int  type;  /* type */
    unsigned int  reverse1;  /* reverse1 */
    unsigned int  reverse2;  /* reverse2 */
    std::string  reverse3;  /* reverse3 */
    unsigned int  createTime;  /* 上报时间 */
     

public:
    void LoadToRecord(youmecommon::CRecord & record) const
    {
        record.SetData(appid);
        record.SetData(userid.c_str());
        record.SetData(SDKVersion);
        record.SetData(type);
        record.SetData(reverse1);
        record.SetData(reverse2);
        record.SetData(reverse3.c_str());
        record.SetData(createTime);
        
    }
};

/**
 * 业务逻辑上报，客户端使用时候主动触发
 * UDP
 */

/********************************************************************************/
/* [CMD:2007] [VERSION:1] */
class ReportMessageState : public ReportMessage {
public:
    ReportMessageState()
    {
        cmdid = 2007;
        version = 1;
        bUseTcp = true;
		audioTime = 0;
    }

    ~ReportMessageState() { }

public:
    unsigned int  SDKVersion;  /* SDK版本 */
    unsigned int  type;  /* type */
    unsigned short  status;  /* 状态 */
    unsigned short  messageType;  /* 消息类型 */
    unsigned int  time;  /* 时长 */
    unsigned int  audioTime;  /* 语音时长 */
    unsigned int  createTime;  /* 上报时间 */
     

public:
    void LoadToRecord(youmecommon::CRecord & record) const
    {
        record.SetData(appid);
        record.SetData(userid.c_str());
        record.SetData(SDKVersion);
        record.SetData(type);
        record.SetData(status);
        record.SetData(messageType);
        record.SetData(time);
        record.SetData(audioTime);
        record.SetData(createTime);
        
    }
};

/**
 * 业务逻辑上报，客户端使用时候主动触发
 * UDP
 */

/********************************************************************************/
/* [CMD:2008] [VERSION:1] */
class ReportTranslate : public ReportMessage {
public:
    ReportTranslate()
    {
        cmdid = 2008;
        version = 1;
        bUseTcp = true;
    }

    ~ReportTranslate() { }

public:
    unsigned int  SDKVersion;  /* SDK版本 */
    unsigned short  status;  /* 状态 */
    std::string  srcLanguage;  /* 源语言 */
    std::string  destLanguage;  /* 目标语言 */
     

public:
    void LoadToRecord(youmecommon::CRecord & record) const
    {
        record.SetData(appid);
        record.SetData(userid.c_str());
        record.SetData(SDKVersion);
        record.SetData(status);
        record.SetData(srcLanguage.c_str());
        record.SetData(destLanguage.c_str());
        
    }
};

/********************************************************************************/
/* [CMD:2014] [VERSION:1] */
class ReportTranslateInfo : public ReportMessage {
public:
    ReportTranslateInfo()
    {
        cmdid = 2014;
        version = 1;
        bUseTcp = true;
    }
    
    ~ReportTranslateInfo() { }
    
public:
    unsigned int  SDKVersion=0;  /* SDK版本 */
    unsigned short  status=0;  /* 状态 */
    std::string  srcLanguage="";  /* 源语言 */
    std::string  destLanguage="";  /* 目标语言 */
    unsigned long long  characterCount=0;  /* 字符数 */
    unsigned short  translateVersion=0;  /* 翻译使用版本 */
    
    
public:
    void LoadToRecord(youmecommon::CRecord & record) const
    {
        record.SetData(appid);
        record.SetData(userid.c_str());
        record.SetData(SDKVersion);
        record.SetData(status);
        record.SetData(srcLanguage.c_str());
        record.SetData(destLanguage.c_str());
        record.SetData(characterCount);
        record.SetData(translateVersion);
        
    }
};

//add
/**
 * 业务逻辑上报，客户端使用时候主动触发
 * UDP
 */

/********************************************************************************/
/* [CMD:2009] [VERSION:1] */
/*
class ReportSampleRate : public ReportMessage {
public:
    ReportSampleRate()
    {
        cmdid = 2009;
        version = 1;
        bUseTcp = true;
    }
    
    ~ReportSampleRate() { }
    
public:
//    unsigned int  SDKVersion;  / * SDK版本 * /
    unsigned int sampleRate; / * 录音采样率 * /
    
public:
    void LoadToRecord(youmecommon::CRecord & record) const
    {
        record.SetData(appid);
        record.SetData(userid.c_str());
//        record.SetData(SDKVersion);
        record.SetData(sampleRate);
    }
};*/

/********************************************************************************/
/* [CMD:2012] [VERSION:1] */
class ReportMessageTimeCost : public ReportMessage {
public:
	ReportMessageTimeCost()
	{
		cmdid = 2012;
		version = 1;
		bUseTcp = false;
	}

	~ReportMessageTimeCost() { }

public:
    unsigned int  SDKVersion=0;  /* SDK版本 */
	unsigned int  costTime = 0;  /* 耗时 */
	unsigned long long  sendSerial = 0;  /* 发送端请求id */


public:
	void LoadToRecord(youmecommon::CRecord & record) const
	{
		record.SetData(appid);
		record.SetData(userid.c_str());
        record.SetData(SDKVersion);
		record.SetData(costTime);
		record.SetData(sendSerial);

	}
};

/********************************************************************************/
/* [CMD:2013] [VERSION:1] */
class ReportSpeechRecognize : public ReportMessage {
public:
	ReportSpeechRecognize()
	{
		cmdid = 2013;
		version = 1;
		bUseTcp = false;
	}

	~ReportSpeechRecognize() { }

public:
	unsigned int  SDKVersion = 0;  /* SDK版本 */
	unsigned short  type = 0;  /* 语音识别类型 0讯飞 */
	unsigned int  status = 0;  /* 状态 */


public:
	void LoadToRecord(youmecommon::CRecord & record) const
	{
		record.SetData(appid);
		record.SetData(userid.c_str());
		record.SetData(SDKVersion);
		record.SetData(type);
		record.SetData(status);
	}
};
