#pragma once
#include <string>
#include <YouMeCommon/DataReport.h>
#include <YouMeCommon/XAny.h>
#include <YouMeIMEngine/ReportService.h>


//�ڲ�������
enum ReportErrorCode
{
	REPORTCODE_SUCCESS = 0,							// �ɹ�
	REPORTCODE_UNKNOW_ERROR = 5000,					// ��������
	REPORTCODE_TIMEOUT = 5001,						// ��ʱ	
	REPORTCODE_UNPACK_EEROR = 5002,					// ���ʧ�� 
	REPORTCODE_SEND_PACKET_ERROR = 5003,			// ���������ʧ��
	REPORTCODE_PARSE_LOGIN_SECURET_EEROR = 5004,	// ������¼������Ϣ��Կʧ��
	REPORTCODE_GET_UPLOADTOKEN_ERROR = 5005,		// ��ȡtokenʧ��
	REPORTCODE_NO_HTTP_HEAD = 5006,					// û��HTTPͷ
	REPORTCODE_UPLOAD_FAILED = 5007,				// �ϴ�ʧ��
	REPORTCODE_DOWNLOAD_FAILED = 5008,				// ����ʧ��
	REPORTCODE_SDKVALIDATE_ERROR = 5009,			// SDK��ʧ��
	REPORTCODE_TRANSLATE_FAILED = 5010				// ����ʧ��
};

enum CommonReporotType
{
	REPORTTYPE_RECONNECT = 0, //��������
	REPORTTYPE_KICKOFF		//����
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


/*//��ʼ��
struct ReportBodySDKValidate
{
	//2000 ��ʼ��
	short status;						//��ʼ��״̬��0:�ɹ�  -1:��ʱ  ���������룩
	unsigned int time;					//ʱ��(MS)
	unsigned int SDKVersion;			//SDK�汾			
	unsigned short serverZone;			//����������
	unsigned short platform;			//ƽ̨��Unknow:0	Android:1	IOS:2	Windows:3	OSX:4	Linux:5��
	unsigned short networkType;			//�������ͣ�Unknow:0	Mobile:1	WIFI:2��
	std::string appkey;					//APPKEY
	std::string packageName;			//����
	std::string systemVersion;			//ϵͳ�汾
	std::string customSystemVersion;	//����ϵͳ�汾
	std::string deviceToken;			//�豸��ʶ
	std::string cpuArch;				//CPU�ܹ�
	std::string cpuChip;				//CPUоƬ
	std::string brand;					//Ʒ��
	std::string model;					//�ͺ�
	unsigned int createTime;			//�ϱ�ʱ��
	unsigned int serverIp;				//ip��ַ

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

//��¼
struct ReportBodyLogin
{
	//2001 ��¼
	std::string userID;			//�û�ID
	unsigned int appID;			//APP��ʶ
	short status;				//��¼״̬��0:�ɹ�	-1:��ʱ		���������룩
	unsigned short platform;	//ƽ̨��Unknow:0	Android:1	IOS:2	Windows:3	OSX:4	Linux:5��
	unsigned short networkType;	//�������ͣ�Unknow:0	Mobile:1	WIFI:2��
	unsigned int SDKVersion;	//SDK�汾
	unsigned int time;			//ʱ��(MS)
	unsigned int createTime;	//�ϱ�ʱ��
	unsigned int serverIp;		//ip��ַ

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

//��������
struct ReportBodyReconnect
{
	//2002 ��������
	std::string userID;			//�û�ID
	unsigned int appID;			//APP��ʶ
	short reason;				//����ԭ��
	short status;				//״̬��0:�ɹ�	-1:��ʱ		����:����˴����룩

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
	//2003 ����
	std::string userID;			//�û�ID
	unsigned int appID;			//APP��ʶ

	void LoadToRecord(youmecommon::CRecord& record) const
	{
		record.SetData(userID.c_str());
		record.SetData(appID);
	}
};

//���ͽ�����Ϣ(ֻ�ϱ�����ʧ��)
struct ReportBodyMessageStatus
{
	//2004 ���ͽ�����Ϣ(ֻ�ϱ�����ʧ��)
	std::string userID;			//�û�ID
	unsigned int appID;			//APP��ʶ
	short status;				//״̬��0:�ɹ�	-1:��ʱ		���������룩
	unsigned short messageType;	//��Ϣ����
	unsigned int time;			//ʱ��(MS)

	void LoadToRecord(youmecommon::CRecord& record) const
	{
		record.SetData(userID.c_str());
		record.SetData(appID);
		record.SetData(status);
		record.SetData(messageType);
		record.SetData(time);
	}
};

//�ɹ�����������Ϣ���Ʒ����ݣ�
struct ReportBodyRecvAudioMessage
{
	//2005 ������Ϣ
	std::string userID;			//�û�ID
	unsigned int appID;			//APP��ʶ

	void LoadToRecord(youmecommon::CRecord& record) const
	{
		record.SetData(userID.c_str());
		record.SetData(appID);
	}
};

//�����ϱ�	��type���־������ͣ�֮��ı����ֶθ���type����ͬ
struct ReportCommon
{
	//2006
	std::string userID;			//�û�ID
	unsigned int appID;			//APP��ʶ
	unsigned int SDKVersion;	//SDK�汾
	
	//0����������	reverse1:����ԭ��	reverse2:״̬��0:�ɹ�	-1:��ʱ		����:����˴����룩
	//1������
	int type;
	int reverse1;
	int reverse2;
	std::string reverse3;
	unsigned int createTime;	//�ϱ�ʱ��

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
	//2007 ��Ϣ״̬
	std::string userID;			//�û�ID
	unsigned int appID;			//APP��ʶ
	unsigned int SDKVersion;	//SDK�汾

	int type;					//0:������Ϣ	1:������Ϣ		2:����(URL)
	short status;				//״̬��0:�ɹ�	-1:��ʱ		���������룩
	unsigned short messageType;	//��Ϣ����
	unsigned int time;			//ʱ��(MS)
	unsigned int audioTime;		//����ʱ��(S)
	unsigned int createTime;	//�ϱ�ʱ��

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
	//2008 ����
	std::string userID;			//�û�ID
	unsigned int appID;			//APP��ʶ
	unsigned int SDKVersion;	//SDK�汾

	short status;				//״̬
	std::string srcLanguage;	//Դ����
	std::string destLanguage;	//Ŀ������
};

struct ReportMessageTimeCost
{
	//2012 ��Ϣ��ʱͳ��
	std::string userID;			//�û�ID
	unsigned int appID;			//APP��ʶ
	unsigned int SDKVersion;	//SDK�汾

	unsigned int costTime;		//��ʱ�����룩
	XUINT64 sendSerial;			//���Ͷ�����id
};

struct ReportSpeechRecognize
{
	//2013 ����ʶ�����ͳ��
	std::string userID;			//�û�ID
	unsigned int appID;			//APP��ʶ
	unsigned int SDKVersion;	//SDK�汾
	short type;					//����ʶ������ 0��Ѷ��
	int status;					//״̬
};

*/