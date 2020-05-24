#include "DataReportDefine.h"
#include <YouMeCommon/DNSUtil.h>
#include <YouMeCommon/XFile.h>
#include <YouMeCommon/CrossPlatformDefine/IYouMeSystemProvider.h>
#include <YouMeCommon/XAny.h>


#define REPORT_INTERVAL 600000
#define REPORT_VERSION 1

extern IYouMeSystemProvider* g_pSystemProvider;


std::string ReportParam::m_strIdentify;
unsigned int ReportParam::m_uiTcpPort = 0;
unsigned int ReportParam::m_uiUdpPort = 0;
XString ReportParam::m_strDomain;
#ifndef WIN32
	CYouMeDataChannel* DataReport::m_pDataChannel = NULL;
#endif
DataReport::DataReport() : m_bInit(false)
, m_bReportExit(false)
#ifdef WIN32
	, m_pDataChannel(NULL)
#endif // !WIN32
{

}

DataReport::~DataReport()
{
	if (m_pDataChannel != NULL)
	{
		CYouMeDataChannel::DestroyInstance(m_pDataChannel);
		m_pDataChannel = NULL;
	}
}

bool DataReport::Init(const std::string& addr, unsigned int tcpPort, unsigned int updPort, const std::string& defaultIP)
{
	if (m_bInit)
	{
		return false;
	}
	if (g_pSystemProvider == NULL)
	{
		YouMe_LOG_Error(__XT("SystemProvider is null"));
		return false;
	}

	std::string strIP;
	std::vector<std::string> ipList;
	youmecommon::DNSUtil::Instance()->GetHostByNameAsync(addr, ipList, 3000);
	if (!ipList.empty())
	{
		strIP = ipList.at(0);
	}
	else
	{
		if (!defaultIP.empty())
		{
			strIP = defaultIP;
		}
		else
		{
			YouMe_LOG_Error(__XT("domain parse failed and no default IP"));
			return false;
		}
	}
	XString ip = UTF8TOXString(strIP);

	YouMe_LOG_Info(__XT("report addr:%s tcpPort:%d updPort:%d"), ip.c_str(), tcpPort, updPort);

	std::string strIdentity = XStringToUTF8(g_pSystemProvider->getUUID());
	ReportParam::setParam(strIdentity, tcpPort, updPort, ip);

	XString strReportPath = youmecommon::CXFile::CombinePath(g_pSystemProvider->getDocumentPath(), __XT("youme_im_report.db"));
	if (NULL == m_pDataChannel)
	{
		m_pDataChannel = CYouMeDataChannel::CreateInstance(strReportPath);
	}
	
	if (m_pDataChannel == NULL)
	{
		return false;
	}

	//m_reportWait.Reset();
	//m_reportThread = std::thread(&Report::ReportThread, this);

	m_bInit = true;

	return true;
}

void DataReport::Report(const ReportMessage& reportMessage)
{
	if (NULL == m_pDataChannel)
	{
		return;
	}
	CDataReportInfo report(m_pDataChannel, reportMessage.cmdid, reportMessage.version, reportMessage.bUseTcp);
	report.SetBody(reportMessage);
	report.Report();
}

void DataReport::SetReportSignal()
{
	m_reportWait.SetSignal();
}

void DataReport::AddReportInfo(ReportMessage& reportInfo)
{
	m_reportList.push_back(std::make_shared<ReportMessage>(reportInfo));
}

void DataReport::ReportThread()
{
	while (true)
	{
		if (m_bReportExit)
		{
			break;
		}

		m_reportWait.WaitTime(REPORT_INTERVAL);
		m_reportWait.Reset();

		if (NULL == m_pDataChannel)
		{
			return;
		}

		{
			std::lock_guard<std::mutex> lock(m_mutex);
			for (std::list<std::shared_ptr<ReportMessage> >::iterator itr = m_reportList.begin(); itr != m_reportList.end();)
			{
				CDataReportInfo report(m_pDataChannel, (*itr)->cmdid, (*itr)->version, (*itr)->bUseTcp);
				report.SetBody(**itr);
				report.Report();
				itr = m_reportList.erase(itr);
			}
		}
	}
	YouMe_LOG_Info(__XT("report thread exit"));
}
