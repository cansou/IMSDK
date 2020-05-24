#include "YouMeIMMessageManager.h"
#include <YouMeCommon/Log.h>
#include <YouMeCommon/XFile.h>
#include <YouMeCommon/DownloadUploadManager.h>
#include <YouMeCommon/json/json.h>
#include <YouMeCommon/CryptUtil.h>
#include <YouMeCommon/StringUtil.hpp>
#include <YouMeCommon/profiledb.h>
#include <YouMeCommon/amrFileCodec.h>
#include <YouMeCommon/opusFileCodec.hpp>
#include <YouMeCommon/KeywordFilter.h>
#include <YouMeCommon/TranslateUtil.h>
#include <YouMeCommon/utf8/checked.h>

#include <YouMeIMEngine/pb/youme_sndmsg.pb.h>
#include <YouMeIMEngine/pb/youme_notify.pb.h>
#include <YouMeIMEngine/pb/youme_getmsg.pb.h>
#include <YouMeIMEngine/pb/youme_upload_token.pb.h>
#include <YouMeIMEngine/pb/youme_forbid_record.pb.h>
#include <YouMeIMEngine/pb/youme_mask_usermsg.pb.h>
#include <YouMeIMEngine/pb/youme_social_access_token.pb.h>
#include <YouMeIMEngine/DataReportDefine.h>
#include <YouMeIMEngine/Speech/YouMeSpeechManager.h>
#include <YouMeIMEngine/Speech/SpeechUtil.h>
#include <YouMeIMEngine/Speech/agc/WavReader.h>
#include <YouMeIMEngine/Speech/agc/WavWriter.h>
#include <YouMeIMEngine/Speech/agc/gain_control.h>
#include <YouMeIMEngine/Speech/speex/speex_preprocess.h>
#include <YouMeIMEngine/Speech/agc/tdav_codec_audio_mixer.h>
#include <YouMeIMEngine/Location/YouMeLocationManager.h>
#include <YouMeIMEngine/Location/LocationUtil.h>
#include <YouMeIMEngine/YouMeIMManager.h>



extern std::mutex* g_youMeGlobalMutex;
extern IYouMeSystemProvider* g_pSystemProvider;

#define MESSAGE_REPORT_MAX_TIMES 2
#define CHANNEL_NUMBER 1
#define SAMPLE_BIT_SIZE 16
#define	MSG_PAGE_SIZE_DEFAULT 20
#define REFUSE_LIST_PAGE_SIZE 2
#define H5_WEIXIN_AUDIO_DOWNLOAD_URL "http://file.api.weixin.qq.com/cgi-bin/media/get?access_token=%s&media_id=%s"
#define MESSAGE_MAX_LENGTH 2048

#define MIC_USE_AGC 0
#define MIC_HIGHEST_VOLUME 1
#define MIC_AGC_AND_HIGHEST_VOLUME 2
#define MIC_NOT_USE_AGC 99


static std::string SendGiftParamKeys[] = { "nickname", "server_area", "location", "score", "level", "vip_level", "extra" };
std::set<XString> YouMeIMMessageManager::m_msgRecordRooms;
std::set<XString> YouMeIMMessageManager::m_msgShieldTargets;
unsigned int YouMeIMMessageManager::m_iTranslateRequestID = 0;
std::set<XString> YouMeIMMessageManager::m_blockUserList;
bool YouMeIMMessageManager::m_bRequestBlockList = false;
XString YouMeIMMessageManager::m_strLastUserID;
std::string YouMeIMMessageManager::m_strWeiXinTocken;
XUINT64 YouMeIMMessageManager::m_ullWeiXinTokenSerial = 0;
XString YouMeIMMessageManager::m_strDownloadDir;
extern CYouMeSpeechManager* g_pSpeechManager;
extern ServerZone g_ServerZone;
extern int g_iYouMeIMMode;
extern CProfileDB* g_pProfileDB;
extern CKeywordFilter* g_pKeywordFilter;
extern CTranslateUtil* g_pTranslateUtil;
extern int g_YM_isPrivateServer;

YIMChatType CYouMeIMMessage::GetChatType()
{
	return m_chatType;
}

void CYouMeIMMessage::SetChatType(YIMChatType chatType)
{
	m_chatType = chatType;
}

const XCHAR* CYouMeIMMessage::GetReceiveID()
{
	return m_strReceiveID.c_str();
}
XUINT64 CYouMeIMMessage::GetID()
{
    return m_ulID;
}
XUINT64 CYouMeIMMessage::GetMessageID()
{
	return m_ulMsgSerial;
}
void CYouMeIMMessage::SetReceiveID(XString& receiverID)
{
	m_strReceiveID = receiverID;
}

void CYouMeIMMessage::SetMessageBody(IYIMMessageBodyBase* pBody)
{
	if (NULL != m_pMessageBody)
	{
		delete m_pMessageBody;
	}
	m_pMessageBody = pBody;
}

IYIMMessageBodyBase* CYouMeIMMessage::GetMessageBody()
{
	return m_pMessageBody;
}

CYouMeIMMessage::CYouMeIMMessage()
{
	m_chatType = ChatType_Unknow;
	m_pMessageBody = NULL;
	m_iCreateTime = 0;
	m_iDistance = 0;
	m_bIsRead = false;
    m_ulID = 0;
}

void CYouMeIMMessage::SetSenderID(XString& senderID)
{
	m_strSenderid = senderID;
}

const XCHAR* CYouMeIMMessage::GetSenderID()
{
	return m_strSenderid.c_str();
}

CYouMeIMMessage::~CYouMeIMMessage()
{
	if (m_pMessageBody != NULL)
	{
		delete m_pMessageBody;
	}
}

unsigned int CYouMeIMMessage::GetCreateTime()
{
	return m_iCreateTime;
}

unsigned int CYouMeIMMessage::GetDistance()
{
	return m_iDistance;
}

bool CYouMeIMMessage::IsRead()
{
	return m_bIsRead;
}

void CYouMeIMMessage::SetTime(unsigned int createTime)
{
	m_iCreateTime = createTime;
}

//文本消息
const XCHAR* CYouMeIMMessageBodyText::GetMessageContent()
{
	return m_strContent.c_str();
}

const XCHAR* CYouMeIMMessageBodyText::GetAttachParam()
{
    return m_strAttachParam.c_str();
}

void CYouMeIMMessageBodyText::SetMessageContent(const XString& strContent)
{
	m_strContent = strContent;
}

//自定义消息
void CYouMeIMMessageBodyCustom::SetCustomMessage(const std::string& strCustomMessage)
{
	m_strData.clear();
	m_strData = strCustomMessage;
}

std::string CYouMeIMMessageBodyCustom::GetCustomMessage()
{
	return m_strData;
}

//语音消息
const XCHAR* CYouMeIMMessageBodyAudio::GetText()
{
	return m_strText.c_str();
}

const XCHAR* CYouMeIMMessageBodyAudio::GetExtraParam()
{
	return m_strParam.c_str();
}

unsigned int CYouMeIMMessageBodyAudio::GetFileSize()
{
	return m_iFileSize;
}

unsigned int CYouMeIMMessageBodyAudio::GetAudioTime()
{
	return m_iTime;
}

const XCHAR* CYouMeIMMessageBodyAudio::GetLocalPath()
{
	return m_strLocalPath.c_str();
}

bool CYouMeIMMessageBodyAudio::IsPlayed()
{
    return m_isPlayed;
}

unsigned int CYouMeIMMessageBodyFile::GetFileSize()
{
	return m_iFileSize;
}

const XCHAR* CYouMeIMMessageBodyFile::GetFileName()
{
	return m_strFileName.c_str();
}

const XCHAR* CYouMeIMMessageBodyFile::GetFileExtension()
{
	return m_strExtension.c_str();
}

YIMFileType CYouMeIMMessageBodyFile::GetFileType()
{
	return m_fileType;
}

const XCHAR* CYouMeIMMessageBodyFile::GetExtraParam()
{
	return m_strParam.c_str();
}

const XCHAR* CYouMeIMMessageBodyFile::GetLocalPath()
{
	return m_strLocalPath.c_str();
}


int CYouMeIMMessageBodyGift::GetGiftID()
{
	return m_iGiftID;
}

unsigned int CYouMeIMMessageBodyGift::GetGiftCount()
{
	return m_iCount;
}

const XCHAR* CYouMeIMMessageBodyGift::GetAnchor()
{
	return m_strAnchorID.c_str();
}

const XCHAR* CYouMeIMMessageBodyGift::GetExtraParam()
{
	return m_strParam.c_str();
}

YouMeIMMessageManager::YouMeIMMessageManager(YouMeIMManager* pIManager) : IManagerBase(pIManager)
, m_pDownloadCallback(NULL)
, m_bInit(false)
, m_bIsGetOfflineMsg(false)
, m_ullAudioMsgSerial(0)
, m_bGetMessageFinish(true)
, m_bMessageNotify(false)
, m_llLastGetMessageTime(0)
, m_ullLastMessageID(0)
, m_downloadFlag(false)
, m_pauseReceivingFlag(false)
, m_iSendReportCount(0)
, m_iRecvReportCount(0)
, m_bTranslateThreadExit(false)
, m_iMsgPageSize(MSG_PAGE_SIZE_DEFAULT)
, m_bMicVolumeGain( 0 )
, m_onlyRecognizeSpeechText(false)
, m_codingFormat(CODING_AMR)
, m_audioEncoderBitRate(BITRATE_16K)
, m_iUploadRetryTimes(1)
, m_msgTransType(0)
{
    m_langCodeMap[LANG_AUTO] = __XT("auto");
    m_langCodeMap[LANG_AF] = __XT("af");                        //南非荷兰语
    m_langCodeMap[LANG_AM] = __XT("am");                        //阿姆哈拉语
    m_langCodeMap[LANG_AR] = __XT("ar");                        //阿拉伯语
    m_langCodeMap[LANG_AR_AE] = __XT("ae");                    //阿拉伯语+阿拉伯联合酋长国 O
    m_langCodeMap[LANG_AR_BH] = __XT("bh");                    //阿拉伯语+巴林 O
    m_langCodeMap[LANG_AR_DZ] = __XT("dz");                    //阿拉伯语+阿尔及利亚 O
    m_langCodeMap[LANG_AR_KW] = __XT("kw");                    //阿拉伯语+科威特 O
    m_langCodeMap[LANG_AR_LB] = __XT("lb");                    //阿拉伯语+黎巴嫩 O
    m_langCodeMap[LANG_AR_OM] = __XT("om");                    //阿拉伯语+阿曼 O
    m_langCodeMap[LANG_AR_SA] = __XT("sa");                    //阿拉伯语+沙特阿拉伯 O
    m_langCodeMap[LANG_AR_SD] = __XT("sd");                    //阿拉伯语+苏丹 O
    m_langCodeMap[LANG_AR_TN] = __XT("tn");                    //阿拉伯语+突尼斯 O
    m_langCodeMap[LANG_AZ] = __XT("az");                        //阿塞拜疆
    m_langCodeMap[LANG_BE] = __XT("be");                        //白俄罗斯语
    m_langCodeMap[LANG_BG] = __XT("bg");                        //保加利亚语
    m_langCodeMap[LANG_BN] = __XT("bn");                        //孟加拉
    m_langCodeMap[LANG_BS] = __XT("bs");                        //波斯尼亚语
    m_langCodeMap[LANG_CA] = __XT("ca");                        //加泰罗尼亚语
    m_langCodeMap[LANG_CA_ES] = __XT("es");                    //加泰罗尼亚语+西班牙 O
    m_langCodeMap[LANG_CO] = __XT("co");                        //科西嘉
    m_langCodeMap[LANG_CS] = __XT("cs");                        //捷克语
    m_langCodeMap[LANG_CY] = __XT("cy");                        //威尔士语
    m_langCodeMap[LANG_DA] = __XT("da");                        //丹麦语
    m_langCodeMap[LANG_DE] = __XT("de");                        //德语
    m_langCodeMap[LANG_DE_CH] = __XT("ch");                    //德语+瑞士 O
    m_langCodeMap[LANG_DE_LU] = __XT("lu");                    //德语+卢森堡 O
    m_langCodeMap[LANG_EL] = __XT("el");                        //希腊语
    m_langCodeMap[LANG_EN] = __XT("en");                        //英语+英国
    m_langCodeMap[LANG_EN_CA] = __XT("ca");                    //英语+加拿大 O
    m_langCodeMap[LANG_EN_IE] = __XT("ie");                    //英语+爱尔兰 O
    m_langCodeMap[LANG_EN_ZA] = __XT("za");                    //英语+南非 O
    m_langCodeMap[LANG_EO] = __XT("eo");                        //世界语
    m_langCodeMap[LANG_ES] = __XT("es");                        //西班牙语
    m_langCodeMap[LANG_ES_BO] = __XT("bo");                    //西班牙语+玻利维亚 O
    m_langCodeMap[LANG_ES_AR] = __XT("ar");                    //西班牙语+阿根廷 O
    m_langCodeMap[LANG_ES_CO] = __XT("co");                    //西班牙语+哥伦比亚 O
    m_langCodeMap[LANG_ES_CR] = __XT("cr");                    //西班牙语+哥斯达黎加 O
    m_langCodeMap[LANG_ES_ES] = __XT("es");                    //西班牙语+西班牙 O X
    m_langCodeMap[LANG_ET] = __XT("et");                        //爱沙尼亚语
    m_langCodeMap[LANG_ES_PA] = __XT("pa");                    //西班牙语+巴拿马 O
    m_langCodeMap[LANG_ES_SV] = __XT("sv");                    //西班牙语+萨尔瓦多 O
    m_langCodeMap[LANG_ES_VE] = __XT("ve");                    //西班牙语+委内瑞拉 O
    m_langCodeMap[LANG_ET_EE] = __XT("ee");                    //爱沙尼亚语+爱沙尼亚 O
    m_langCodeMap[LANG_EU] = __XT("eu");                        //巴斯克
    m_langCodeMap[LANG_FA] = __XT("fa");                        //波斯语
    m_langCodeMap[LANG_FI] = __XT("fi");                        //芬兰语
    m_langCodeMap[LANG_FR] = __XT("fr");                        //法语
    m_langCodeMap[LANG_FR_BE] = __XT("be");                    //法语+比利时 O
    m_langCodeMap[LANG_FR_CA] = __XT("ca");                    //法语+加拿大 O
    m_langCodeMap[LANG_FR_CH] = __XT("ch");                    //法语+瑞士 O
    m_langCodeMap[LANG_FR_LU] = __XT("lu");                    //法语+卢森堡 O
    m_langCodeMap[LANG_FY] = __XT("fy");                        //弗里斯兰
    m_langCodeMap[LANG_GA] = __XT("ga");                        //爱尔兰语
    m_langCodeMap[LANG_GD] = __XT("gd");                        //苏格兰盖尔语
    m_langCodeMap[LANG_GL] = __XT("gl");                        //加利西亚
    m_langCodeMap[LANG_GU] = __XT("gu");                        //古吉拉特文
    m_langCodeMap[LANG_HA] = __XT("ha");                        //豪撒语
    m_langCodeMap[LANG_HI] = __XT("hi");                        //印地语
    m_langCodeMap[LANG_HR] = __XT("hr");                        //克罗地亚语
    m_langCodeMap[LANG_HT] = __XT("ht");                        //海地克里奥尔
    m_langCodeMap[LANG_HU] = __XT("hu");                        //匈牙利语
    m_langCodeMap[LANG_HY] = __XT("hy");                        //亚美尼亚
    m_langCodeMap[LANG_ID] = __XT("id");                        //印度尼西亚
    m_langCodeMap[LANG_IG] = __XT("ig");                        //伊博
    m_langCodeMap[LANG_IS] = __XT("is");                        //冰岛语
    m_langCodeMap[LANG_IT] = __XT("it");                        //意大利语
    m_langCodeMap[LANG_IT_CH] = __XT("ch");                    //意大利语+瑞士 O
    m_langCodeMap[LANG_JA] = __XT("ja");                        //日语
    m_langCodeMap[LANG_KA] = __XT("ka");                        //格鲁吉亚语
    m_langCodeMap[LANG_KK] = __XT("kk");                        //哈萨克语
    m_langCodeMap[LANG_KN] = __XT("kn");                        //卡纳达
    m_langCodeMap[LANG_KM] = __XT("km");                        //高棉语
    m_langCodeMap[LANG_KO] = __XT("ko");                        //朝鲜语
    m_langCodeMap[LANG_KO_KR] = __XT("kr");                    //朝鲜语+南朝鲜 O
    m_langCodeMap[LANG_KU] = __XT("ku");                        //库尔德
    m_langCodeMap[LANG_KY] = __XT("ky");                        //吉尔吉斯斯坦
    m_langCodeMap[LANG_LA] = __XT("la");                        //拉丁语
    m_langCodeMap[LANG_LB] = __XT("lb");                        //卢森堡语
    m_langCodeMap[LANG_LO] = __XT("lo");                        //老挝
    m_langCodeMap[LANG_LT] = __XT("lt");                        //立陶宛语
    m_langCodeMap[LANG_LV] = __XT("lv");                        //拉托维亚语+列托
    m_langCodeMap[LANG_MG] = __XT("mg");                        //马尔加什
    m_langCodeMap[LANG_MI] = __XT("mi");                        //毛利
    m_langCodeMap[LANG_MK] = __XT("mk");                        //马其顿语
    m_langCodeMap[LANG_ML] = __XT("ml");                        //马拉雅拉姆
    m_langCodeMap[LANG_MN] = __XT("mn");                        //蒙古
    m_langCodeMap[LANG_MR] = __XT("mr");                        //马拉地语
    m_langCodeMap[LANG_MS] = __XT("ms");                        //马来语
    m_langCodeMap[LANG_MT] = __XT("mt");                        //马耳他
    m_langCodeMap[LANG_MY] = __XT("my");                        //缅甸
    m_langCodeMap[LANG_NL] = __XT("nl");                        //荷兰语
    m_langCodeMap[LANG_NL_BE] = __XT("be");                    //荷兰语+比利时 O
    m_langCodeMap[LANG_NE] = __XT("ne");                        //尼泊尔
    m_langCodeMap[LANG_NO] = __XT("no");                        //挪威语
    m_langCodeMap[LANG_NY] = __XT("ny");                        //齐切瓦语
    m_langCodeMap[LANG_PL] = __XT("pl");                        //波兰语
    m_langCodeMap[LANG_PS] = __XT("ps");                        //普什图语
    m_langCodeMap[LANG_PT] = __XT("pt");                        //葡萄牙语
    m_langCodeMap[LANG_PT_BR] = __XT("br");                    //葡萄牙语+巴西 O
    m_langCodeMap[LANG_RO] = __XT("ro");                        //罗马尼亚语
    m_langCodeMap[LANG_RU] = __XT("ru");                        //俄语
    m_langCodeMap[LANG_SD] = __XT("sd");                        //信德
    m_langCodeMap[LANG_SI] = __XT("si");                        //僧伽罗语
    m_langCodeMap[LANG_SK] = __XT("sk");                        //斯洛伐克语
    m_langCodeMap[LANG_SL] = __XT("sl");                        //斯洛语尼亚语
    m_langCodeMap[LANG_SM] = __XT("sm");                        //萨摩亚
    m_langCodeMap[LANG_SN] = __XT("sn");                        //修纳
    m_langCodeMap[LANG_SO] = __XT("so");                        //索马里
    m_langCodeMap[LANG_SQ] = __XT("sq");                        //阿尔巴尼亚语
    m_langCodeMap[LANG_SR] = __XT("sr");                        //塞尔维亚语
    m_langCodeMap[LANG_ST] = __XT("st");                        //塞索托语
    m_langCodeMap[LANG_SU] = __XT("su");                        //巽他语
    m_langCodeMap[LANG_SV] = __XT("sv");                        //瑞典语
    m_langCodeMap[LANG_SV_SE] = __XT("se");                    //瑞典语+瑞典 O
    m_langCodeMap[LANG_SW] = __XT("sw");                        //斯瓦希里语
    m_langCodeMap[LANG_TA] = __XT("ta");                        //泰米尔
    m_langCodeMap[LANG_TE] = __XT("te");                        //泰卢固语
    m_langCodeMap[LANG_TG] = __XT("tg");                        //塔吉克斯坦
    m_langCodeMap[LANG_TH] = __XT("th");                        //泰语
    m_langCodeMap[LANG_TL] = __XT("tl");                        //菲律宾
    m_langCodeMap[LANG_TR] = __XT("tr");                        //土耳其语
    m_langCodeMap[LANG_UK] = __XT("uk");                        //乌克兰语
    m_langCodeMap[LANG_UR] = __XT("ur");                        //乌尔都语
    m_langCodeMap[LANG_UZ] = __XT("uz");                        //乌兹别克斯坦
    m_langCodeMap[LANG_VI] = __XT("vi");                        //越南
    m_langCodeMap[LANG_XH] = __XT("xh");                        //科萨
    m_langCodeMap[LANG_YID] = __XT("yi");                        //意第绪语
    m_langCodeMap[LANG_YO] = __XT("yo");                        //约鲁巴语
    m_langCodeMap[LANG_ZH] = __XT("zh-cn");                         //汉语
    m_langCodeMap[LANG_ZH_TW] = __XT("zh-tw");                   //繁体
    m_langCodeMap[LANG_ZU] = __XT("zu");                        //祖鲁语
}

YIMErrorcode YouMeIMMessageManager::Init()
{
	if (m_bInit)
	{
		return YIMErrorcode_Success;
	}
	m_bIsGetOfflineMsg = false;
	m_bExitUpload = false;
	m_bExitDownload = false;
	m_pCallback = NULL;
	m_pDownloadCallback = NULL;
	m_codingFormat = (CodingFormat)m_pIManager->GetConfigure<int>(CONFIG_CODING_FORMAT, CODING_AMR);
    m_audioEncoderBitRate = m_pIManager->GetConfigure<int>(CONFIG_AUDIO_ENCODE_BITRATE, BITRATE_16K);
    YouMe_LOG_Info(__XT("coding format:%d,audio encode bitrate:%d"), m_codingFormat,m_audioEncoderBitRate);
	m_uploadThread = std::thread(&YouMeIMMessageManager::UploadThreadProc, this);
	m_downloadThread = std::thread(&YouMeIMMessageManager::DownloadThreadProc, this);
    m_translateThread = std::thread(&YouMeIMMessageManager::TranslateThread, this);
	
	m_bInit = true;
	return YIMErrorcode_Success;
}

void YouMeIMMessageManager::UnInit()
{
	if (!m_bInit)
	{
		return;
	}
    {
        std::lock_guard<std::mutex> lock(m_uploadInfoLock);
        m_fileUploadMap.clear();
    }
    if (m_uploadThread.joinable())
    {
		m_bExitUpload = true;
		m_uploadSemaphore.Increment();
		m_uploadThread.join();
    }
	if (m_downloadThread.joinable())
	{
		m_bExitDownload = true;
		m_downloadSemaphore.Increment();
		m_downloadThread.join();
	}
	if (m_translateThread.joinable())
	{
		m_bTranslateThreadExit = true;
		m_translateSemaphore.Increment();
		m_translateThread.join();
	}
	
	m_messageRecorder.UnInit();
    m_bInit = false;
	m_pCallback = NULL; 
}

void YouMeIMMessageManager::SetMessageCallback(IYIMMessageCallback* pCallback)
{
	m_pCallback = pCallback;
}

YIMErrorcode YouMeIMMessageManager::SendTextMessage(const XCHAR* receiverID, YIMChatType chatType, const XCHAR* text, const XCHAR* attachParam, XUINT64* requestID)
{
	if (m_pIManager == NULL || (m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success && m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Reconnecting))
	{
		return YIMErrorcode_NotLogin;
	}
    
	if (NULL == receiverID || XStrLen(receiverID) == 0 || NULL == text || XStrLen(text) == 0)
	{
		return YIMErrorcode_ParamInvalid;
	}
    
    int maxLength = MESSAGE_MAX_LENGTH;
//#ifndef WIN32
//    maxLength *= 3;
//#endif
    XString strText = XString(text);
    if (strText.length() > maxLength)
    {
        return YIMErrorcode_MessageTooLong;
    }

	MessageBufferInfo messsageInfo;
	messsageInfo.commandType = YOUMEServiceProtocol::CMD_SND_TEXT_MSG;
	messsageInfo.chatType = chatType;
	messsageInfo.receiverID = XStringToUTF8(XString(receiverID));
	messsageInfo.messageBuffer = XStringToUTF8(XString(text));
    messsageInfo.extend["AttachParam"] = XStringToUTF8(XString(attachParam));
    messsageInfo.extend["autoid"] = CStringUtilT<char>::to_string(m_pIManager->GetAutoIncrementID());
    messsageInfo.extend["SendTime"] = CStringUtilT<char>::to_string(m_pIManager->GetServerValidTime());
    
	XUINT64 msgSerial = 0;
	bool bRet = SendMessage(messsageInfo, msgSerial);
	if (requestID != NULL)
	{
		*requestID = msgSerial;
	}

	return bRet ? YIMErrorcode_Success : YIMErrorcode_UnknowError;
}

YIMErrorcode YouMeIMMessageManager::MultiSendTextMessage(const std::vector<XString>& vecReceiver, const XCHAR* text)
{
	if (m_pIManager == NULL || (m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success && m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Reconnecting))
	{
		return YIMErrorcode_NotLogin;
	}
	if (vecReceiver.empty() || NULL == text || XStrLen(text) == 0)
	{
		return YIMErrorcode_ParamInvalid;
	}
    
    int maxLength = MESSAGE_MAX_LENGTH;
#ifndef WIN32
    maxLength *= 3;
#endif
	XString strText = XString(text);
	
	if (strText.length() > maxLength)
	{
		return YIMErrorcode_MessageTooLong;
	}
    
    unsigned long nTotal = (unsigned long)vecReceiver.size();
	for (unsigned long i = 0; i < nTotal;)
	{
		YOUMEServiceProtocol::RecverList recverList;
		for (int j = 0; j < 200 && i < nTotal; ++j, ++i)
		{
            std::string strReceiverID = XStringToUTF8(vecReceiver[i]);
			if (!strReceiverID.empty())
			{
				recverList.add_recver_id(strReceiverID);
			}
		}
		if (recverList.recver_id_size() > 0)
		{
			std::string strRecvID;
			recverList.SerializeToString(&strRecvID);
			MessageBufferInfo messsageInfo;
			messsageInfo.commandType = YOUMEServiceProtocol::CMD_SND_TEXT_MSG;
			messsageInfo.chatType = ChatType_Multi;
			messsageInfo.receiverID = strRecvID;
			messsageInfo.messageBuffer = XStringToUTF8(XString(text));
			messsageInfo.extend["MultiSendText"] = "";
			XUINT64 msgSerial = 0;
			SendMessage(messsageInfo, msgSerial);
		}
	}

	return YIMErrorcode_Success;
}

YIMErrorcode YouMeIMMessageManager::SendCustomMessage(const XCHAR* receiverID, YIMChatType chatType, const char* content, unsigned int size, XUINT64* requestID)
{
	if (m_pIManager == NULL || (m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success && m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Reconnecting))
	{
		return YIMErrorcode_NotLogin;
	}
    
    if(nullptr == receiverID || XString(receiverID).empty() )
    {
        return YIMErrorcode_ParamInvalid;
    }
    
    if( content == nullptr ){
        return YIMErrorcode_ParamInvalid;
    }
    
	if (size > 2 * 1024 )
	{
		return YIMErrorcode_ParamInvalid;
	}
	
	MessageBufferInfo messsageInfo;
	messsageInfo.commandType = YOUMEServiceProtocol::CMD_SND_BIN_MSG;
	messsageInfo.chatType = chatType;
	messsageInfo.receiverID = XStringToUTF8(XString(receiverID));
	messsageInfo.messageBuffer = std::string(content, size);
	XUINT64 msgSerial = 0;
	bool bRet = SendMessage(messsageInfo, msgSerial);
	if (requestID != NULL)
	{
		*requestID = msgSerial;
	}
	return bRet ? YIMErrorcode_Success : YIMErrorcode_UnknowError;
}

YIMErrorcode YouMeIMMessageManager::SendFile(const XCHAR* receiverID, YIMChatType chatType, const XCHAR* filePath, XUINT64* requestID, const XCHAR* extraParam, YIMFileType fileType)
{
	if (m_pIManager == NULL || (m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success && m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Reconnecting))
	{
		return YIMErrorcode_NotLogin;
	}
    
    if(nullptr == receiverID || XString(receiverID).empty() )
    {
        return YIMErrorcode_ParamInvalid;
    }
    
	if (NULL == filePath || XString(filePath).empty())
	{
		return YIMErrorcode_ParamInvalid;
	}
	youmecommon::CXFile file;
	if (file.LoadFile(filePath, youmecommon::CXFile::Mode_OpenExist_ReadOnly) != 0)
	{
		YouMe_LOG_Error(__XT("file not exist %s"), filePath);
		return YIMErrorcode_FileNotExist;
	}
	unsigned int iFileSize = static_cast<unsigned int>(file.GetFileSize());
	file.Close();

	XString strExtension = youmecommon::CXFile::GetFileExt(filePath);


	XUINT64 ullSerial = m_pIManager->GetUniqueSerial();
	std::shared_ptr<FileUploadInfo> pUploadInfo(new FileUploadInfo);
	pUploadInfo->msgSerial = ullSerial;
	pUploadInfo->receiverID = XString(receiverID);
	pUploadInfo->chatType = chatType;
	pUploadInfo->fileType = fileType;
	pUploadInfo->localPath = filePath;
	pUploadInfo->fileSize = iFileSize;
	pUploadInfo->extension = strExtension;
    pUploadInfo->autoIncrementID = m_pIManager->GetAutoIncrementID();
    pUploadInfo->startSendTime = m_pIManager->GetServerValidTime();
	if (NULL != extraParam)
	{
		pUploadInfo->extend["Param"] = XStringToUTF8(XString(extraParam));
	}

	{
		std::lock_guard<std::mutex> lock(m_uploadInfoLock);
		m_fileUploadMap[ullSerial] = pUploadInfo;
	}

	if (requestID != NULL)
	{
		*requestID = ullSerial;
	}

	YIMErrorcode errorCode = RequestUploadToken(*pUploadInfo);
	if (YIMErrorcode_Success != errorCode)
	{
		std::map<XUINT64, std::shared_ptr<FileUploadInfo> >::iterator itr = m_fileUploadMap.find(ullSerial);
		if (itr != m_fileUploadMap.end())
		{
			m_fileUploadMap.erase(itr);
		}
	}
	return errorCode;
}

YIMErrorcode YouMeIMMessageManager::SendAudioMessage(const XCHAR* receiverID, YIMChatType chatType, XUINT64* requestID)
{
	if (NULL == g_pSpeechManager || m_pIManager == NULL || (m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success && m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Reconnecting))
	{
		return YIMErrorcode_NotLogin;
	}
    if(nullptr == receiverID || XString(receiverID).empty() )
    {
        return YIMErrorcode_ParamInvalid;
    }

	YouMe_LOG_Info(__XT("Enter"));

    XUINT64 ulSerial = m_pIManager->GetUniqueSerial();
    if (NULL != requestID)
	{
        *requestID = ulSerial;
    }
    YIMErrorcode errorcode = YIMErrorcode_PTT_Fail;
#ifdef OS_OSX    
    errorcode = g_pSpeechManager->StartSpeech(ulSerial, false);
#elif (OS_IOS || OS_IOSSIMULATOR || WIN32 || OS_ANDROID)
    errorcode = g_pSpeechManager->StartSpeech(ulSerial, true);
#endif
	
	if (YIMErrorcode_Success == errorcode)
    {
		if (!m_onlyRecognizeSpeechText)
		{
			std::shared_ptr<FileUploadInfo> pUploadInfo(new FileUploadInfo);
			pUploadInfo->msgSerial = ulSerial;
			pUploadInfo->receiverID = XString(receiverID);
			pUploadInfo->chatType = chatType;
			pUploadInfo->fileType = FileType_Audio;
			{
				std::lock_guard<std::mutex> lock(m_uploadInfoLock);
				m_fileUploadMap[ulSerial] = pUploadInfo;
				m_ullAudioMsgSerial = ulSerial;
			}
            m_strSendAudioParam = __XT("");
		}
		YouMe_LOG_Info(__XT("Leave"));
    }
	else
	{
		YouMe_LOG_Info(__XT("Leave Fail, errorCode:%d"), errorcode);
	}
    
    return errorcode;
}

YIMErrorcode YouMeIMMessageManager::SendOnlyAudioMessage(const XCHAR* receiverID, YIMChatType chatType, XUINT64* requestID)
{
	if (NULL == g_pSpeechManager || m_pIManager == NULL || (m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success && m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Reconnecting))
	{
		return YIMErrorcode_NotLogin;
	}
    if(NULL == receiverID || XString(receiverID).empty() )
    {
        return YIMErrorcode_ParamInvalid;
    }

	YouMe_LOG_Info(__XT("Enter"));

	XUINT64 ulSerial = m_pIManager->GetUniqueSerial();
	if (NULL != requestID)
	{
		*requestID = ulSerial;
	}
	YIMErrorcode errorcode = g_pSpeechManager->StartSpeech(ulSerial, false);
	if (YIMErrorcode_Success == errorcode)
	{
		std::shared_ptr<FileUploadInfo> pUploadInfo(new FileUploadInfo);
		pUploadInfo->msgSerial = ulSerial;
		pUploadInfo->receiverID = XString(receiverID);
		pUploadInfo->chatType = chatType;
		pUploadInfo->fileType = FileType_Audio;
		{
			std::lock_guard<std::mutex> lock(m_uploadInfoLock);
			m_fileUploadMap[ulSerial] = pUploadInfo;
			m_ullAudioMsgSerial = ulSerial;
		}
        m_strSendAudioParam = __XT("");

		YouMe_LOG_Info(__XT("Leave"));
	}
	else
	{
		YouMe_LOG_Info(__XT("Leave Fail, errorCode:%d"), errorcode);
	}

	return errorcode;
}

YIMErrorcode YouMeIMMessageManager::StopAudioMessage(const XCHAR* extraParam)
{
	if (g_pSpeechManager == NULL || m_pIManager == NULL || (m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success && m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Reconnecting))
	{
		return YIMErrorcode_NotLogin;
	}

	YouMe_LOG_Info(__XT("Enter"));
	if (m_strSendAudioParam.empty() && NULL != extraParam)
	{
		m_strSendAudioParam = extraParam;
	}

	m_ullAudioMsgSerial = 0;
	return g_pSpeechManager->StopSpeech();
}

YIMErrorcode YouMeIMMessageManager::CancleAudioMessage()
{
	if (g_pSpeechManager == NULL || m_pIManager == NULL || (m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success && m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Reconnecting))
	{
		return YIMErrorcode_NotLogin;
	}

	YouMe_LOG_Info(__XT("Enter"));

	YIMErrorcode errorcode = g_pSpeechManager->CancleSpeech();
	{
		std::lock_guard<std::mutex> lock(m_uploadInfoLock);
		std::map<XUINT64, std::shared_ptr<FileUploadInfo> >::iterator itr = m_fileUploadMap.find(m_ullAudioMsgSerial);
		if (itr != m_fileUploadMap.end())
		{
			m_fileUploadMap.erase(itr);
		}
	}
	m_ullAudioMsgSerial = 0;

	return errorcode;
}

YIMErrorcode YouMeIMMessageManager::SendGift(const XCHAR* anchorID, const XCHAR* channel, int giftId, int giftCount, const char* extraParam, XUINT64* requestID)
{
	if (anchorID == NULL || XStrLen(anchorID) == 0
		|| channel == NULL || XStrLen(channel) == 0
		|| extraParam == NULL || strlen(extraParam) == 0
		|| giftCount <= 0)
	{
		return YIMErrorcode_ParamInvalid;
	}

	if (m_pIManager == NULL || (m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success && m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Reconnecting))
	{
		return YIMErrorcode_NotLogin;
	}

	youmecommon::Value extraParamValue;
	youmecommon::Reader jsonReader;
	if (!jsonReader.parse(extraParam, extraParamValue))
	{
		return YIMErrorcode_ParamInvalid;
	}
	if (!extraParamValue.isObject())
	{
		return YIMErrorcode_ParamInvalid;
	}
	std::vector<std::string> names = extraParamValue.getMemberNames();
	int nKeyNum = sizeof(SendGiftParamKeys) / sizeof(SendGiftParamKeys[0]);
	for (int i = 0; i < nKeyNum; ++i)
	{
		if (find(names.begin(), names.end(), SendGiftParamKeys[i]) == names.end())
		{
			YouMe_LOG_Info(__XT("send gift no key:%s"), UTF8TOXString(SendGiftParamKeys[i]).c_str());
			return YIMErrorcode_ParamInvalid;
		}
	}
	youmecommon::Value val;
	val["from"] = XStringToUTF8(m_pIManager->GetCurrentUser());
	val["to"] = XStringToUTF8(XString(anchorID));
	val["id"] = CStringUtilT<char>::to_string(giftId);
	val["count"] = CStringUtilT<char>::to_string(giftCount);
	std::string strGiftBuffer = val.toStyledString();

	MessageBufferInfo messsageInfo;
	messsageInfo.commandType = YOUMEServiceProtocol::CMD_SND_GIFT_MSG;
	messsageInfo.chatType = ChatType_RoomChat;
	messsageInfo.receiverID = XStringToUTF8(XString(channel));
	messsageInfo.messageBuffer = strGiftBuffer;
	for (std::vector<std::string>::const_iterator itr = names.begin(); itr != names.end(); ++itr)
	{
		std::string value = extraParamValue[*itr].asString();
		messsageInfo.extend.insert(std::map<std::string, std::string>::value_type(*itr, value));
	}
	XUINT64 msgSerial = 0;
	bool bRet = SendMessage(messsageInfo, msgSerial);
	if (requestID != NULL)
	{
		*requestID = msgSerial;
	}

	return bRet ? YIMErrorcode_Success : YIMErrorcode_UnknowError;
}

void YouMeIMMessageManager::OnSpeechResult(YIMErrorcode errorcode, const XString& path, unsigned long long serial, const XString& text)
{
	if (m_onlyRecognizeSpeechText && YIMErrorcode_Success == errorcode)
	{
		if (NULL != m_pCallback)
		{
			m_pCallback->OnGetRecognizeSpeechText(serial, errorcode, text.c_str());
		}
		return;
	}

	YIMErrorcode retErrorcode = errorcode;
	XString strReceiverID;
	int nTime = 0;
	do 
	{
		std::lock_guard<std::mutex> lock(m_uploadInfoLock);
		std::map<XUINT64, std::shared_ptr<FileUploadInfo> >::iterator itr = m_fileUploadMap.find(serial);
		if (itr == m_fileUploadMap.end())
		{
			YouMe_LOG_Error(__XT("upload map has not found serial:%llu total:%lu"), serial, m_fileUploadMap.size());
			retErrorcode = YIMErrorcode_PTT_FileNotExist;
			break;
		}
		strReceiverID = itr->second->receiverID;

		if (YIMErrorcode_Success == errorcode || YIMErrorcode_PTT_ReachMaxDuration == errorcode || YIMErrorcode_PTT_RecognizeFailed == errorcode)
		{
			youmecommon::CXFile file;
			if (file.LoadFile(path, youmecommon::CXFile::Mode_OpenExist_ReadOnly) != 0)
			{
				YouMe_LOG_Error(__XT("file not exist %s"), path.c_str());
				file.Close();
				m_fileUploadMap.erase(itr);
				retErrorcode = YIMErrorcode_FileNotExist;
				break;
			}
			unsigned int iFileSize = static_cast<unsigned int>(file.GetFileSize());
			file.Close();

			itr->second->localPath = path;
			itr->second->fileSize = iFileSize;
			if (m_codingFormat == CODING_OPUS)
			{
				itr->second->extension = __XT("opus");	//WAV格式后面都转换为OPUS格式
			}
			else if (m_codingFormat == CODING_AMR)
			{
				itr->second->extension = __XT("amr");	//WAV格式后面都转换为AMR格式
			}
			itr->second->extend["AudioText"] = XStringToUTF8(text);

			nTime = SpeechUtil::GetWavTime(path);
			itr->second->extend["Time"] = CStringUtilT<char>::to_string(nTime);

			if ((errorcode == YIMErrorcode_Success && serial == m_ullAudioMsgSerial) || YIMErrorcode_PTT_ReachMaxDuration == errorcode)
			{
				YouMe_LOG_Debug(__XT("reach speech maximum duration"));

				retErrorcode = YIMErrorcode_PTT_ReachMaxDuration;
				itr->second->extend["MaxDuration"] = "1";	//达到录音限制时长
			}

			YIMErrorcode result = RequestUploadToken(*(itr->second));
			if (result != YIMErrorcode_Success)
			{
				retErrorcode = result;
			}
		}
		else
		{
			m_fileUploadMap.erase(itr);
		}
	} while (0);

	if (NULL != m_pCallback)
	{
		if (retErrorcode != YIMErrorcode_Success && retErrorcode != YIMErrorcode_PTT_ReachMaxDuration)
		{
			if (strReceiverID == __XT(""))
			{
				std::shared_ptr<CAudioSpeechInfo> pAudioSpeechInfo(new CAudioSpeechInfo);
				pAudioSpeechInfo->m_requestID = serial;
				m_pCallback->OnStopAudioSpeechStatus(retErrorcode, pAudioSpeechInfo);
			}
			else
			{
				m_pCallback->OnSendAudioMessageStatus(serial, retErrorcode, __XT(""), __XT(""), 0, 0, false, 0, 0, 0);
			}
		}
		else
		{
			m_pCallback->OnStartSendAudioMessage(serial, retErrorcode, text.c_str(), path.c_str(), nTime);
		}
	}
}

void YouMeIMMessageManager::OnRecordVolumeChange(float volume)
{
	if (m_pCallback != NULL)
	{
		m_pCallback->OnRecordVolumeChange(volume);
	}
}

#include <chrono>
#include <iostream>
using namespace std;

void applyAGC( const XString& strWavOrignPath ,  unsigned int bVolumeGain )
{
    if (bVolumeGain == MIC_NOT_USE_AGC)
    {
        return;
    }
    YouMe_LOG_Info(__XT("start agc,bVolumeGain：%d"),bVolumeGain);
    //AGC
    XString strWavAGCPath= strWavOrignPath + __XT(".agc");
    do{
        //AGC
        youmecommon::SpeexPreprocessState *preprocess = NULL;
        int denoise_enabled=1;
        int noiseSuppress = -35;
        
        youmecommon::WavReader *pReader = new youmecommon::WavReader();
        if(0 == pReader){
            break;
        }
        bool bRet = pReader->Open(strWavOrignPath.c_str());
        if(true != bRet){
            delete pReader;
            break;
        }
        
        int32_t nLen = pReader->m_nSampleRate / 100 ;
        int32_t nSampleRate = pReader->m_nSampleRate;
        
        youmecommon::WavWriter *pWriter = new youmecommon::WavWriter();
        if(0 == pWriter){
            break;
        }
        bRet = pWriter->Open(strWavAGCPath.c_str(), nSampleRate, SAMPLE_BIT_SIZE, CHANNEL_NUMBER);
        if(true != bRet){
            delete pWriter;
            break;
        }
        
        preprocess = youmecommon::speex_preprocess_state_init(nLen, nSampleRate);
        youmecommon::speex_preprocess_ctl(preprocess, SPEEX_PREPROCESS_SET_DENOISE, &denoise_enabled);
        //                    youmecommon::speex_preprocess_ctl(preprocess, SPEEX_PREPROCESS_SET_AGC, &agc_enabled);
        youmecommon::speex_preprocess_ctl(preprocess, SPEEX_PREPROCESS_SET_NOISE_SUPPRESS, &noiseSuppress); //设置噪声的dB
        //                    youmecommon::speex_preprocess_ctl(preprocess, SPEEX_PREPROCESS_SET_VAD, &vad_enabled);
        //speex_preprocess_ctl(preprocess, SPEEX_PREPROCESS_SET_PROB_START , &vadProbStart);//所需VAD设定概率从沉默到语音
        //speex_preprocess_ctl(preprocess, SPEEX_PREPROCESS_SET_PROB_CONTINUE, &vadProbContinue);
        
        int nReadLen = nLen;
        //音量拉伸幅度
        float volumeGain = 0;
        
        if ((bVolumeGain == MIC_HIGHEST_VOLUME) || (bVolumeGain == MIC_AGC_AND_HIGHEST_VOLUME))
        {
            //计算强制拉伸的最大幅度
            int max_swing = 0;
            
            if( bVolumeGain )
            {
                while (nReadLen > 0)
                {
                    int16_t *szPcm = new int16_t[nLen];
                    nReadLen = pReader->Read(szPcm, nLen); // 10ms data length
                    
                    if (nReadLen > 0)
                    {
                        youmecommon::tdav_codec_swing_max(szPcm, nReadLen * 2, 2, max_swing);
                    }
                    
                    delete[]szPcm;
                }
                
                pReader->ResetToHead();
                
                volumeGain = youmecommon::tdav_getVolumeGain(max_swing);
            }
        }
        
        //执行AGC/强制拉伸
        int agcSkipFrame = 0;
        nReadLen = nLen;
        int preCount = 0;
        void *pAgcInst = youmecommon::WebRtcAgc_Create_YM();
        youmecommon::WebRtcAgc_Init(pAgcInst, 0, 255, 2, nSampleRate);
        
        while (nReadLen > 0)
        {
            int16_t *szPcm = new int16_t[nLen];
            
            nReadLen = pReader->Read(szPcm, nLen); // 10ms data length
            
            if (nReadLen > 0)
            {
                preCount ++;
                
                youmecommon::speex_preprocess_run(preprocess, szPcm);
                //处理agc
                if ((bVolumeGain == MIC_USE_AGC) || (bVolumeGain == MIC_AGC_AND_HIGHEST_VOLUME))
                {
                    if(preCount>agcSkipFrame){
                        youmecommon::WebRtcAgc_Process(pAgcInst, szPcm, nLen, szPcm);
                    }
                }
                
                //实现强制拉伸
                if((bVolumeGain == MIC_HIGHEST_VOLUME) || (bVolumeGain == MIC_AGC_AND_HIGHEST_VOLUME))
                {
					youmecommon::tdav_codec_apply_volume_add(volumeGain, szPcm, nReadLen * 2, 2);  //单声道，16位，目前
                }
                
                pWriter->Write(szPcm, nReadLen);
            }
            
            delete[]szPcm;
        }
        
        delete pWriter;
        
        youmecommon::speex_preprocess_state_destroy(preprocess);
        
        youmecommon::WebRtcAgc_Free(pAgcInst);
        
        delete pReader;
        
        //移除AGC之前的文件
        youmecommon::CXFile::remove_file(strWavOrignPath);
        youmecommon::CXFile::rename_file(strWavAGCPath,strWavOrignPath);
        
    }while (0);
    YouMe_LOG_Info(__XT("end agc"));
}

void YouMeIMMessageManager::onUploadProgressCallback(void * param, float percent) {
	if (param == NULL) {
		return;
	}

	uint64_t msgSerial = *(uint64_t*)param;
	if (m_pCallback != nullptr) {
		m_pCallback->OnUploadProgress(msgSerial, percent);
	}
}

void YouMeIMMessageManager::UploadThreadProc()
{
	while (m_uploadSemaphore.Decrement())
	{
		if (m_bExitUpload)
		{
			break;
		}

		FileUploadInfo uploadInfo;
		{
			std::lock_guard<std::mutex> lock(m_uploadInfoLock);
			if (m_fileUploadMap.size() == 0)
			{
				continue;
			}
			for (std::map<XUINT64, std::shared_ptr<FileUploadInfo> >::iterator itr = m_fileUploadMap.begin(); itr != m_fileUploadMap.end(); ++itr)
			{
				if (!itr->second->uploadURL.empty())
				{
					uploadInfo = *(itr->second);
					m_fileUploadMap.erase(itr);
					break;
				}
			}
		}

		XString strTmpPath = uploadInfo.localPath;
		bool bDelTempFile = false;
		if (uploadInfo.fileType == FileType_Audio)
		{
			FileFormat audioFormat = SpeechUtil::GetFileFormat(uploadInfo.localPath);
			if (audioFormat == FileFormat_WAV)
			{
                YouMe_LOG_Info(__XT("start agc"));
                //AGC
                XString strWavOrignPath= uploadInfo.localPath;
                
                applyAGC( strWavOrignPath , m_bMicVolumeGain );

                if (m_codingFormat == CODING_AMR)
                {
                    //AMR Encode
                    YouMe_LOG_Info(__XT("start wav to amr"));
                    strTmpPath = uploadInfo.localPath + __XT(".amr");
                    EncodeWAVEFileToAMRFile(uploadInfo.localPath, strTmpPath, 1, 16);
                    YouMe_LOG_Info(__XT("end wav to amr"));
                }
                else if (m_codingFormat == CODING_OPUS)
                {
                    //OPUS Encode
                    YouMe_LOG_Info(__XT("start wav to opus"));
                    strTmpPath = uploadInfo.localPath + __XT(".opus");
                    EncodeWAVFileToOPUSFile(uploadInfo.localPath, strTmpPath,m_audioEncoderBitRate);
                    YouMe_LOG_Info(__XT("end wav to opus"));
                }

				bDelTempFile = true;
			}
		}

		std::string strResponse;
		YouMe_LOG_Info(__XT("upload file start"));
		bool bRet = true;
		int uploadTimes = 0;
UPLOAD_LABEL:
		bRet = CDownloadUploadManager::UploadFile(uploadInfo.uploadURL, strTmpPath, uploadInfo.httpHead, (void *)&uploadInfo.msgSerial, strResponse, uploadInfo.bucketType, this);
		if (!bRet && ++uploadTimes < m_iUploadRetryTimes)
		{
			goto UPLOAD_LABEL;
		}
		YouMe_LOG_Info(__XT("upload file end path:%s result:%s"), strTmpPath.c_str(), bRet ? __XT("success") : __XT("failed"));
		if (bDelTempFile)
		{
			youmecommon::CXFile::remove_file(strTmpPath);
		}

		if (FileType_Audio == uploadInfo.fileType && uploadInfo.receiverID == __XT("") && m_pCallback != NULL)
		{
			//仅录音和上传
			
			int nTime = 0;
			std::map<std::string, std::string>::const_iterator itrTime = uploadInfo.extend.find("Time");
			if (itrTime != uploadInfo.extend.end())
			{
				nTime = CStringUtilT<char>::str_to_uint32(itrTime->second);
			}
			std::shared_ptr<CAudioSpeechInfo> pAudioSpeechInfo(new CAudioSpeechInfo);
			pAudioSpeechInfo->m_iFileSize = uploadInfo.fileSize;
			pAudioSpeechInfo->m_iTime = nTime;
			pAudioSpeechInfo->m_requestID = uploadInfo.msgSerial;
			std::map<std::string, std::string>::const_iterator itrText = uploadInfo.extend.find("AudioText");
			if (itrText != uploadInfo.extend.end())
			{
				pAudioSpeechInfo->m_strText = UTF8TOXString(itrText->second);
			}
			pAudioSpeechInfo->m_strLocalPath = uploadInfo.localPath;
			pAudioSpeechInfo->m_strDownloadURL = uploadInfo.downloadURL;
			m_pCallback->OnStopAudioSpeechStatus(bRet ? YIMErrorcode_Success : YIMErrorcode_UploadFailed, pAudioSpeechInfo);
			continue;
		}

		int nReportErrorCode = REPORTCODE_UNKNOW_ERROR;
		YIMErrorcode errorCode = YIMErrorcode_Success;
		if (bRet)
		{
			if (uploadInfo.fileType == FileType_Audio)
			{
//				if (!uploadInfo.extraParam.empty())
                if (!m_strSendAudioParam.empty())
				{
					uploadInfo.extend["Param"] = XStringToUTF8(m_strSendAudioParam);
				}
				errorCode = SendAudioMessage(uploadInfo);
			}
			else
			{
				errorCode = SendFileMessage(uploadInfo,uploadInfo.autoIncrementID,uploadInfo.startSendTime);
			}
		}
		else
		{
			errorCode = YIMErrorcode_UploadFailed;
			nReportErrorCode = REPORTCODE_UPLOAD_FAILED;
		}

		if (errorCode != YIMErrorcode_Success)
		{
			YouMe_LOG_Error(__XT("send message error(%d)"), errorCode);

			if (m_pCallback != NULL)
			{
				if (uploadInfo.fileType == FileType_Audio)
				{
					m_pCallback->OnSendAudioMessageStatus(uploadInfo.msgSerial, errorCode, __XT(""), __XT(""), 0, 0, false, 0, 0, 0);
				}
				else
				{
					m_pCallback->OnSendMessageStatus(uploadInfo.msgSerial, errorCode,false, 0, 0, 0, 0);
				}
			}

			ReportDataMessageStatus(0, uploadInfo.fileType == FileType_Audio ? YOUMEServiceProtocol::CMD_SND_VOICE_MSG : YOUMEServiceProtocol::CMD_SND_FILE_MSG, nReportErrorCode, 0, 0);
		}
	}
	YouMe_LOG_Info(__XT("Leave"));
}

void YouMeIMMessageManager::UpdateRecvMessageSerial(XUINT64 messageSerial)
{
	if (NULL == m_pIManager)
	{
		YouMe_LOG_Error(__XT("IMManager is null"));
		return;
	}
	std::string strSerial = CStringUtilT<char>::to_string(messageSerial);
	XString strMessageSerial = UTF8TOXString(strSerial);
	std::string strKey = CStringUtilT<char>::formatString("LastMsgSerial_%d_%s_%d", m_pIManager->GetAppID(), XStringToUTF8(m_pIManager->GetCurrentUser()).c_str(), g_iYouMeIMMode);
	bool bRet = g_pProfileDB->setSetting(strKey, strMessageSerial);
	if (!bRet)
	{
		YouMe_LOG_Error(__XT("update message serial failed"));
	}
}

YIMErrorcode YouMeIMMessageManager::DownloadFile(XUINT64 messageID, const XCHAR* savePath)
{
	if ((NULL == savePath || XStrLen(savePath) == 0) && m_strDownloadDir.empty())
	{
		YouMe_LOG_Error(__XT("ParamInvalid"));
		return YIMErrorcode_ParamInvalid;
	}
	
	FileDownloadInfo downloadInfo;
	{
		std::lock_guard<std::mutex> lock(m_DownloadInfoLock);
		std::map<XUINT64, FileDownloadInfo>::iterator it = m_downloadInfo.find(messageID);
		if (it == m_downloadInfo.end())
		{ 
            YouMe_LOG_Error(__XT("ParamInvalid not found messageID: %llu"), messageID);
			return YIMErrorcode_ParamInvalid;
		}
		downloadInfo = it->second;
		m_downloadInfo.erase(it);
	}
	XString strSavePath;
	if (savePath != NULL)
	{
		strSavePath = XString(savePath);
	}
	downloadInfo.savePath = GetSavePath(downloadInfo.downloadURL, strSavePath);
	m_downloadList.push_back(downloadInfo);
	m_downloadSemaphore.Increment();
	return YIMErrorcode_Success;
}

YIMErrorcode YouMeIMMessageManager::DownloadFile(const XCHAR* downloadURL, const XCHAR* savePath, YIMFileType fileType)
{
	if (NULL == downloadURL || XStrLen(downloadURL) == 0)
	{
		YouMe_LOG_Error(__XT("ParamInvalid url null or empty"));
		return YIMErrorcode_ParamInvalid;
	}
	if ((NULL == savePath || XStrLen(savePath) == 0) && m_strDownloadDir.empty())
	{
		YouMe_LOG_Error(__XT("ParamInvalid savePath is null"));
		return YIMErrorcode_ParamInvalid;
	}

	XString strSavePath;
	if (savePath != NULL)
	{
		YouMe_LOG_Info(__XT("DownloadFile saveTo: %s"), savePath);
		strSavePath = XString(savePath);
	}

	YouMe_LOG_Debug(__XT("DownloadFile url: %s"),downloadURL);

	FileDownloadInfo downloadInfo;
	downloadInfo.downloadURL = downloadURL;
	downloadInfo.msgSerial = 0;
	downloadInfo.fileType = fileType;
	downloadInfo.msg = NULL;
	downloadInfo.savePath = GetSavePath(downloadInfo.downloadURL, strSavePath);;
	{
		std::lock_guard<std::mutex> lock(m_DownloadInfoLock);
		m_downloadList.push_back(downloadInfo);
	}
	
	m_downloadSemaphore.Increment();
	return YIMErrorcode_Success;
}

XString YouMeIMMessageManager::GetSavePath(const XString& url, const XString savePath)
{
	XString strSavePath;
	if (savePath.empty() || youmecommon::CXFile::is_dir(savePath.c_str()))
	{
		if (savePath.empty())
		{
			strSavePath = m_strDownloadDir;
		}
		else
		{
			strSavePath = XString(savePath);
#ifdef WIN32
			strSavePath = CStringUtil::replace_text(strSavePath, __XT("/"), __XT("\\"));
#endif
			if (strSavePath[strSavePath.length() - 1] != __XT('/') && strSavePath[strSavePath.length() - 1] != __XT('\\'))
			{
				strSavePath.append(XPreferredSeparator);
			}
		}

		XString filename;
		XString::size_type pos = url.find_last_of(__XT("/"));
		if (pos != XString::npos)
		{
			filename = url.substr(pos + 1);
		}
		if (filename.empty() || filename.length() > 32)
		{
			filename = CStringUtil::to_string(m_pIManager->GetServerValidTime());
		}

		strSavePath += filename;
		strSavePath += __XT(".wav");
	}
	else
	{
		strSavePath = XString(savePath);
#ifdef WIN32
		strSavePath = CStringUtil::replace_text(strSavePath, __XT("/"), __XT("\\"));
#endif
	}
	return strSavePath;
}

YIMErrorcode YouMeIMMessageManager::SetDownloadDir(const XCHAR* path)
{
	if (NULL == path || XStrLen(path) == 0)
	{
		return YIMErrorcode_ParamInvalid;
	}
	XString downloadDir = XString(path);
#ifdef WIN32
	downloadDir = CStringUtil::replace_text(downloadDir, __XT("/"), __XT("\\"));
#endif
	if (downloadDir[downloadDir.length() - 1] != __XT('/') && downloadDir[downloadDir.length() - 1] != __XT('\\'))
	{
		downloadDir.append(XPreferredSeparator);
	}
	if (!youmecommon::CXFile::is_dir(downloadDir.c_str()))
	{
		if (!youmecommon::CXFile::make_dir_tree(downloadDir.c_str()))
		{
			YouMe_LOG_Error(__XT("set download dir failed"));
			return YIMErrorcode_CreateDirectoryFailed;
		}
	}
	m_strDownloadDir = downloadDir;
	return YIMErrorcode_Success;
}

YIMErrorcode YouMeIMMessageManager::DownloadFileSync(XUINT64 iMessageID, const XString& strSavePath)
{
	FileDownloadInfo downloadInfo;
	{
		std::lock_guard<std::mutex> lock(m_DownloadInfoLock);
		std::map<XUINT64, FileDownloadInfo>::iterator itr = m_downloadInfo.find(iMessageID);
		if (itr == m_downloadInfo.end())
		{
			return YIMErrorcode_ParamInvalid;
		}
		downloadInfo = itr->second;
		m_downloadInfo.erase(itr);
	}
	
	bool bDelTempFile = false;
	XString strTmpPath = strSavePath;
	if (FileType_Audio == downloadInfo.fileType)
	{
        strTmpPath.append(__XT(".public"));
	}
	bool bSuccess = CDownloadUploadManager::DownloadFile(downloadInfo.downloadURL, strTmpPath);
	if (bSuccess && FileType_Audio == downloadInfo.fileType)
	{
		FileFormat audioFormat = SpeechUtil::GetFileFormat(strTmpPath);
		if (FileFormat_AMR == audioFormat)
        {            
            XString strAmrPath = strSavePath + __XT(".amr");
            youmecommon::CXFile::remove_file(strAmrPath);
            youmecommon::CXFile::rename_file(strTmpPath, strAmrPath);
            DecodeAMRFileToWAVEFile(strAmrPath, strSavePath);
            youmecommon::CXFile::remove_file(strAmrPath);
            bDelTempFile = true;
        }
		else if (FileFormat_OPUS == audioFormat)
        {            
            XString strOpusPath = strSavePath + __XT(".opus");
            youmecommon::CXFile::remove_file(strOpusPath);
            youmecommon::CXFile::rename_file(strTmpPath, strOpusPath);
            DecodeOPUSFileToWAVFile(strOpusPath, strSavePath);            
            youmecommon::CXFile::remove_file(strOpusPath);
            bDelTempFile = true;
        }
        else
        {            
            youmecommon::CXFile::rename_file(strTmpPath, strSavePath);
        }
	}
	if (bDelTempFile)
	{
		youmecommon::CXFile::remove_file(strTmpPath);
	}

	return bSuccess ? YIMErrorcode_Success : YIMErrorcode_PTT_DownloadFail;
}

void YouMeIMMessageManager::SetDownloadCallback(IYIMDownloadCallback* pCallback)
{
	m_pDownloadCallback = pCallback;
}

void YouMeIMMessageManager::SetUpdateReadStatusCallback(IYIMUpdateReadStatusCallback* pCallback)
{
	m_pUpdateReadStatusCallback = pCallback;
}

void YouMeIMMessageManager::DownloadThreadProc()
{
	YouMe_LOG_Info(__XT("Enter"));

	int retryTimes = 0;
	while (m_downloadSemaphore.Decrement())
	{
		if (m_bExitDownload)
		{
			break;
		}
		FileDownloadInfo downloadInfo;
		{
			std::lock_guard<std::mutex> lock(m_DownloadInfoLock);
			if (m_downloadList.empty())
			{
				continue;
			}
			downloadInfo = m_downloadList.front();
			m_downloadList.pop_front();
		}
		XString strTmpPath = downloadInfo.savePath;
		int nAudioTime = 0;
		int nReportErrorcode = REPORTCODE_SUCCESS;
		YIMErrorcode errorcode = YIMErrorcode_Success;

		bool bSuccess = true;
		retryTimes = 0;

DOWNLOAD:
		bSuccess = CDownloadUploadManager::DownloadFile(downloadInfo.downloadURL, strTmpPath);
		if (!bSuccess)
		{
			// 下载失败 马上重试一次
			if (++retryTimes < 2)	
			{
				goto DOWNLOAD;
			}
			else
			{
				nReportErrorcode = REPORTCODE_DOWNLOAD_FAILED;
				errorcode = YIMErrorcode_PTT_DownloadFail;
			}
		}
		else
		{
			if (FileType_Audio == downloadInfo.fileType)
			{
				FileFormat audioFormat = SpeechUtil::GetFileFormat(strTmpPath);
				if (FileFormat_AMR == audioFormat)
				{
					XString strAmrPath = downloadInfo.savePath + __XT(".amr");
					youmecommon::CXFile::remove_file(strAmrPath);
					youmecommon::CXFile::rename_file(strTmpPath, strAmrPath);
					DecodeAMRFileToWAVEFile(strAmrPath, downloadInfo.savePath);
					youmecommon::CXFile::remove_file(strAmrPath);
				}
				else if (FileFormat_OPUS == audioFormat)
				{
					XString strOpusPath = downloadInfo.savePath + __XT(".opus");
					youmecommon::CXFile::remove_file(strOpusPath);
					youmecommon::CXFile::rename_file(strTmpPath, strOpusPath);
					DecodeOPUSFileToWAVFile(strOpusPath, downloadInfo.savePath);
					youmecommon::CXFile::remove_file(strOpusPath);
				}
				else
				{
					youmecommon::CXFile::rename_file(strTmpPath, downloadInfo.savePath);
				}

				if (downloadInfo.extend.find("Time") != downloadInfo.extend.end())
				{
					nAudioTime = atoi(downloadInfo.extend["Time"].c_str());
				}else if(FileFormat_Unknow != audioFormat)
                {
                    nAudioTime = SpeechUtil::GetWavTime(downloadInfo.savePath);
                }
			}
			if (downloadInfo.msgSerial != 0)
			{
//				XUINT64 messageID = downloadInfo.msgSerial;
//				std::map<std::string, std::string>::const_iterator itr = downloadInfo.extend.find("MessageID");
//				if (itr != downloadInfo.extend.end())
//				{
//					//历史消息记录中未下载的语音消息，因为历史消息记录的消息id并非真正的messageid，而是自增id
//					messageID = CStringUtilT<char>::str_to_uint64(itr->second);
//				}
				m_messageRecorder.UpdateMessagePath(downloadInfo.msgSerial, downloadInfo.savePath ,m_pIManager->GetAutoIncrementID(),m_pIManager->GetServerValidTime());
			}
		}

		if (m_pDownloadCallback != NULL)
		{
            if( downloadInfo.msg != NULL )
			{
                m_pDownloadCallback->OnDownload( errorcode,  downloadInfo.msg , downloadInfo.savePath.c_str() );
            }
            else
			{
                m_pDownloadCallback->OnDownloadByUrl( errorcode , downloadInfo.downloadURL.c_str(), downloadInfo.savePath.c_str(), nAudioTime );
            }
		}
		
        int reportType = downloadInfo.msgSerial == 0 ? 2 : 1;
		int nMessageType = FileType_Audio == downloadInfo.fileType ? YOUMEServiceProtocol::CMD_SND_VOICE_MSG : YOUMEServiceProtocol::CMD_SND_FILE_MSG;
		ReportDataMessageStatus(reportType, nMessageType, nReportErrorcode, 0, nAudioTime);
	}

	YouMe_LOG_Info(__XT("Leave"));
}

bool YouMeIMMessageManager::SendMessage(MessageBufferInfo& messageInfo, XUINT64& msgSerial)
{
	if (m_pIManager == NULL)
	{
		return false;
	}

	YOUMEServiceProtocol::SendMsgReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_chat_type(messageInfo.chatType);
	req.set_recv_id(messageInfo.receiverID);
	req.set_msg_content(messageInfo.messageBuffer.c_str(), messageInfo.messageBuffer.length());
	req.set_tran_type(m_msgTransType);  //消息传送类型设置
	for (std::map<std::string, std::string>::const_iterator itr = messageInfo.extend.begin(); itr != messageInfo.extend.end(); ++itr)
	{
		YOUMECommonProtocol::NameValue* pParam = req.mutable_comment()->add_comment();
		if (pParam != NULL)
		{
			if (!strcmp(itr->first.c_str(), "msg_id")) {
				pParam->set_name(itr->first);
				pParam->set_value(itr->second);
				pParam->set_value_type(YOUMECommonProtocol::VALUE_UINT64);
			} else {
				pParam->set_name(itr->first);
				pParam->set_value(itr->second);
				pParam->set_value_type(YOUMECommonProtocol::VALUE_STRING);
			}
		}
	}

	if (m_pIManager->GetLocationManager() != NULL)
	{
		YouMeLocationManager* pLocationManager = dynamic_cast<YouMeLocationManager*>(m_pIManager->GetLocationManager());
		if (pLocationManager)
		{
			LocationInfo locationInfo = pLocationManager->GetCurrentUserLocationInfo();
			if (locationInfo.longitude != 0 && locationInfo.latitude != 0)
			{
				YOUMECommonProtocol::NameValue* pParamLongitude = req.mutable_comment()->add_comment();
				if (pParamLongitude != NULL)
				{
					pParamLongitude->set_name("Longitude");
					pParamLongitude->set_value(CStringUtilT<char>::to_string(locationInfo.longitude));
					pParamLongitude->set_value_type(YOUMECommonProtocol::VALUE_STRING);
				}
				YOUMECommonProtocol::NameValue* pParamLatitude = req.mutable_comment()->add_comment();
				if (pParamLatitude != NULL)
				{
					pParamLatitude->set_name("Latitude");
					pParamLatitude->set_value(CStringUtilT<char>::to_string(locationInfo.latitude));
					pParamLatitude->set_value_type(YOUMECommonProtocol::VALUE_STRING);
				}
			}
		}
	}

	//消息耗时统计
	int messageCostProbability = m_pIManager->GetConfigure<int>(CONFIG_MESSAGE_COST_STATISTIC, 0);
	if (messageCostProbability != 0)
	{
		srand(time(NULL));
		if (rand() % messageCostProbability == 0)
		{
			YOUMECommonProtocol::NameValue* sendTime = req.mutable_comment()->add_comment();
			if (sendTime != NULL)
			{
				sendTime->set_name("SendTime");
				sendTime->set_value(CStringUtilT<char>::to_string(m_pIManager->GetServerValidTime()));
				sendTime->set_value_type(YOUMECommonProtocol::VALUE_STRING);
			}
		}
	}

    std::map<std::string, std::string>::const_iterator itr = messageInfo.extend.find("SendTime");
    if (itr == messageInfo.extend.end() )
    {
		messageInfo.extend["SendTime"] = CStringUtilT<char>::to_string(m_pIManager->GetServerValidTime());
    }

	std::string strData;
	req.SerializeToString(&strData);
	if (m_pIManager->SendData(messageInfo.commandType, strData.c_str(), (int)strData.length(), messageInfo.extend, msgSerial))
	{
		{
			std::lock_guard<std::mutex> lock(m_messageRecordLock);
			m_sendMessageRecord[msgSerial] = messageInfo;
		}
		YouMe_LOG_Debug(__XT("sendmessage receiver:%s chattype:%d serial:%llu"), UTF8TOXString(messageInfo.receiverID).c_str(), messageInfo.chatType, msgSerial);

		return true;
	}
	else
	{
		ReportDataMessageStatus(0, messageInfo.commandType, REPORTCODE_SEND_PACKET_ERROR, 0, 0);
		return false;
	}
}

void YouMeIMMessageManager::OnRecvPacket(ServerPacket& serverPacket)
{
	switch (serverPacket.commondID)
	{
		case YOUMEServiceProtocol::CMD_SND_TEXT_MSG:
		case YOUMEServiceProtocol::CMD_SND_VOICE_MSG:
		case YOUMEServiceProtocol::CMD_SND_FILE_MSG:
		case YOUMEServiceProtocol::CMD_SND_BIN_MSG:
		case YOUMEServiceProtocol::CMD_SND_GIFT_MSG:
		{
			OnSendMessageRsp(serverPacket);
		}
			break;
		case YOUMEServiceProtocol::CMD_GET_MSG:
		{
			OnReceiveMessageRsp(serverPacket);
		}
			break;
		case YOUMEServiceProtocol::CMD_GET_UPLOAD_TOKEN:
		{
			OnGetUploadTokenRsp(serverPacket);
		}
			break;
		case YOUMEServiceProtocol::NOTIFY_PRIVATE_MSG:
		case YOUMEServiceProtocol::NOTIFY_ROOM_MSG:
		case YOUMEServiceProtocol::NOTIFY_PRIVATE_MSG_V2:
		case YOUMEServiceProtocol::NOTIFY_ROOM_MSG_V2:
		{
			OnReceiveMessageNotify(serverPacket);
		}
			break;
		case YOUMEServiceProtocol::NOTIFY_READ_MSG:
		{
			OnReceiveMessageReadStatusNotify(serverPacket);
		}
			break;

		case YOUMEServiceProtocol::CMD_SYN_READ_STATUS:
		{
			OnSynReadMsgStatus(serverPacket);
		}
			break;
		case YOUMEServiceProtocol::CMD_GET_ROOM_HISTORY_MSG:
		{
			OnRoomHistoryMessageRsp(serverPacket);
		}
			break;
		case YOUMEServiceProtocol::CMD_SND_TIPOFF_MSG:
		{
			OnAccusationRsp(serverPacket);
		}
			break;
		case YOUMEServiceProtocol::CMD_GET_TIPOFF_MSG:
		{
			OnGetAccusationDealResultRsp(serverPacket);
		}
			break;
		case YOUMEServiceProtocol::CMD_GET_FORBID_RECORD:
		{
			OnGetForbiddenSpeakInfo(serverPacket);
		}
        	break;
		case YOUMEServiceProtocol::CMD_SET_MASK_USER_MSG:
		{
			OnBlockUserRsp(serverPacket);
		}
			break;
		case YOUMEServiceProtocol::CMD_GET_MASK_USER_MSG:
		{
			OnGetBlockUserListRsp(serverPacket);
		}
			break;
		case YOUMEServiceProtocol::CMD_CLEAN_MASK_USER_MSG:
		{
			OnUnBlockAllUserRsp(serverPacket);
		}
			break;
		case YOUMEServiceProtocol::CMD_GET_WEIXIN_TOKEN:
		{
			OnWeiXinAccessTokenRsp(serverPacket);
		}
			break;
		case YOUMEServiceProtocol::CMD_SND_READ_STATUS:
		{
			SetAllMessageReadRsp(serverPacket);
		}
			break;
		default:
			break;
	}
}

void YouMeIMMessageManager::SetAllMessageReadRsp(ServerPacket& serverPacket)
{
	if (NULL == m_pCallback)
	{
		YouMe_LOG_Warning(__XT("message callback is NULL"));
		return;
	}

	MessageBufferInfo messageInfo;
	{
		std::lock_guard<std::mutex> lock(m_messageRecordLock);
		std::map<XUINT64, MessageBufferInfo>::iterator itr = m_sendMessageRecord.find(serverPacket.reqSerial);
		if (itr != m_sendMessageRecord.end())
		{
			messageInfo = itr->second;
			m_sendMessageRecord.erase(itr);
		}
	}

	m_messageRecorder.SetRecverAllMessageReadWithChatType(UTF8TOXString(messageInfo.receiverID), messageInfo.chatType);
}

void YouMeIMMessageManager::OnSynReadMsgStatus(ServerPacket& serverPacket) {
	YOUMEServiceProtocol::SynMsgStatusRsp rsp;
	if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
	{
		YouMe_LOG_Error(__XT("send message response unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
		return;
	}

	int num = 0;
	XUINT64 ullMsgSerial = 0;
	int chatType = 0;
	std::string recvId;
	if (rsp.ret() == 0) {
		num = rsp.recv_rsp_list_size();
		//YouMe_LOG_Warning(__XT("bruce >>> num:%d"), num);
		for (int i = 0; i < num; i++) {
			YOUMEServiceProtocol::RecvTypeRsp recvTypeRsp = rsp.recv_rsp_list(i);
			ullMsgSerial = recvTypeRsp.msg_id();
			chatType = recvTypeRsp.chat_type();
			recvId = recvTypeRsp.recv_id();
			if (ullMsgSerial != 0 && m_pUpdateReadStatusCallback != NULL) {
				//向应用层上报消息已读
				m_pUpdateReadStatusCallback->OnUpdateReadStatus(UTF8TOXString(recvId).c_str(), chatType, ullMsgSerial);
			
				//更新本地历史记录消息为已读
				m_messageRecorder.SetSenderAllMessageReadWithChatType(UTF8TOXString(recvId), chatType);
			}
		}
	}
}

void YouMeIMMessageManager::OnReceiveMessageReadStatusNotify(ServerPacket& serverPacket) {
	YouMe_LOG_Warning(__XT("begin"));

	YOUMEServiceProtocol::NotifyReadRsp rsp;
	if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
	{
		YouMe_LOG_Error(__XT("send message response unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
		return;
	}

	XUINT64 ullMsgSerial = 0;
	int chatType = 0;
	std::string recvId;

	ullMsgSerial = rsp.msg_id();
	chatType = rsp.chat_type();
	recvId = rsp.snder_id();
	if (m_pUpdateReadStatusCallback != NULL) {
		m_pUpdateReadStatusCallback->OnUpdateReadStatus(UTF8TOXString(recvId).c_str(), chatType, ullMsgSerial);	
		// 本地历史记录中，发送消息的状态更新为已读
		m_messageRecorder.SetSenderAllMessageReadWithChatType(UTF8TOXString(recvId), chatType);
	}

	YouMe_LOG_Warning(__XT("end"));
}

void YouMeIMMessageManager::OnSendMessageRsp(ServerPacket& serverPacket)
{
	YIMErrorcode errorCode = YIMErrorcode_Success;
    bool isForbidRoom = false;
    int reasonType = 0;
    XUINT64 forbidEndTime = 0;
    XUINT64 svrMsgID = 0;
	do 
	{
		if (serverPacket.result == -1)
		{
			errorCode = YIMErrorcode_TimeOut;
			YouMe_LOG_Error(__XT("send message timeout"));
			break;
		}
		YOUMEServiceProtocol::SendMsgRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("send message response unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
			errorCode = YIMErrorcode_ServerError;
			break;
		}
		int nRet = rsp.ret();
        svrMsgID = rsp.svr_msgid();
		if (nRet != 0)
		{
			YouMe_LOG_Error(__XT("send message error(%d)"), nRet);
			errorCode = YIMErrorcode_ServerError;
			if (YOUMEServiceProtocol::EC_MSG_TOO_LONG == nRet)
			{
				errorCode = YIMErrorcode_MessageTooLong;
			}
			else if (YOUMEServiceProtocol::EC_RECVID_TOO_LONG == nRet)
			{
				errorCode = YIMErrorcode_ReceiverTooLong;
			}
			else if (YOUMEServiceProtocol::EC_INVALID_CHAT_TYPE == nRet)
			{
				errorCode = YIMErrorcode_InvalidChatType;
			}
			else if (YOUMEServiceProtocol::EC_INVALID_RECV_ID == nRet)
			{
				errorCode = YIMErrorcode_InvalidReceiver;
			}
			else if (YOUMEServiceProtocol::EC_RECVID_EMPTY == nRet)
			{
				errorCode = YIMErrorcode_ReceiverEmpty;
			}
			else if (YOUMEServiceProtocol::EC_USER_GAGED == nRet)
			{
				errorCode = YIMErrorcode_ForbiddenSpeak;
                
                isForbidRoom = rsp.forbid_room();
                reasonType = rsp.reason_type();
                forbidEndTime = rsp.expire_time();
			}
			else if (YOUMEServiceProtocol::EC_ILLEGAL_MESSAGE == nRet)
			{
				errorCode = YIMErrorcode_HasIllegalText;
			}
			else if (YOUMEServiceProtocol::EC_ADVERTISEMENT_MESSAGE == nRet)
			{
				errorCode = YIMErrorcode_AdvertisementMessage;
			}
			else if (YOUMEServiceProtocol::EC_MASK_MSG == nRet)
			{
				errorCode = YIMErrorcode_MessageBlocked;
			}
        }

		YouMe_LOG_Debug(__XT("sendmessage finished messageID:%llu"), rsp.svr_msgid());
	} while (0);

	MessageBufferInfo messageInfo;
	XUINT64 ullMessageSerial = 0;
	{
		std::lock_guard<std::mutex> lock(m_messageRecordLock);
		std::map<XUINT64, MessageBufferInfo>::iterator itr = m_sendMessageRecord.find(serverPacket.reqSerial);
		if (itr != m_sendMessageRecord.end())
		{
			ullMessageSerial = itr->first;
			messageInfo = itr->second;
			m_sendMessageRecord.erase(itr);
		}
	}

	if (YOUMEServiceProtocol::CMD_SND_VOICE_MSG == serverPacket.commondID)
	{
		unsigned int nTime = 0;
		int nAudioTime = 0;
		std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("SendTime");
		if (itr != serverPacket.extend.end() && m_pIManager != NULL)
		{
			XUINT64 nSendTime = CStringUtilT<char>::str_to_uint64(itr->second);
			nTime = (unsigned int)(m_pIManager->GetServerValidTime() - nSendTime);
		}

		itr = serverPacket.extend.find("Time");
		if (itr != serverPacket.extend.end())
		{
			nAudioTime = CStringUtilT<char>::str_to_uint32(itr->second);
		}
		ReportDataMessageStatus(0, serverPacket.commondID, REPORTCODE_SUCCESS, nTime, nAudioTime);
	}
	else
	{
		if (m_iSendReportCount < MESSAGE_REPORT_MAX_TIMES)
		{
			++m_iSendReportCount;
			unsigned int nTime = 0;
			std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("SendTime");
			if (itr != serverPacket.extend.end() && m_pIManager != NULL)
			{
				XUINT64 nSendTime = CStringUtilT<char>::str_to_uint64(itr->second);
				nTime = (unsigned int)(m_pIManager->GetServerValidTime() - nSendTime);
			}
			ReportDataMessageStatus(0, serverPacket.commondID, REPORTCODE_SUCCESS, nTime, 0);
		}
	}

	//群发接口不回调
	if (serverPacket.extend.find("MultiSendText") != serverPacket.extend.end())
	{
		return;
	}

	if (m_pCallback == NULL)
	{
		YouMe_LOG_Error(__XT("message callback is null"));
		return;
	}

	XUINT64 sendTime = 0;
    std::map<std::string, std::string>::const_iterator itr = messageInfo.extend.find("SendTime");
    if (itr != messageInfo.extend.end())
    {
        sendTime = CStringUtilT<char>::str_to_uint64(itr->second)/1000;
        
    }else{
		messageInfo.extend["SendTime"] = CStringUtilT<char>::to_string(m_pIManager->GetServerValidTime());
		sendTime =  m_pIManager->GetServerValidTime()/1000;
    }
    
    std::map<std::string, std::string>::const_iterator itr2 = messageInfo.extend.find("autoid");
    if (itr2 != messageInfo.extend.end())
    {
        
    }else{
        YouMe_LOG_Error(__XT("auto id not found, create new"));
        messageInfo.extend["autoid"] =  CStringUtilT<char>::to_string(m_pIManager->GetAutoIncrementID());
    }

	if (serverPacket.commondID == YOUMEServiceProtocol::CMD_SND_VOICE_MSG)
	{
		XString strText;
		XString strPath;
		int nTime = 0;
		std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("AudioText");
		if (itr != serverPacket.extend.end())
		{
			strText = UTF8TOXString(itr->second);
		}
		itr = serverPacket.extend.find("LocalPath");
		if (itr != serverPacket.extend.end())
		{
			strPath = UTF8TOXString(itr->second);
		}
		itr = serverPacket.extend.find("Time");
		if (itr != serverPacket.extend.end())
		{
			nTime = CStringUtilT<char>::str_to_uint32(itr->second.c_str());
		}
		itr = serverPacket.extend.find("MaxDuration");
		if (itr != serverPacket.extend.end())
		{
			errorCode = YIMErrorcode_PTT_ReachMaxDuration;
		}
        
        if (!strPath.empty())
        {
			FileFormat audioFormat = SpeechUtil::GetFileFormat(strPath);
			if (FileFormat_AMR == audioFormat)
            {
                //amr 转成wav
                XString strWavTmpPath = strPath + __XT(".wav");
                DecodeAMRFileToWAVEFile(strPath, strWavTmpPath);
                youmecommon::CXFile::remove_file(strPath);
                strPath = strWavTmpPath;
            }
			else if (FileFormat_OPUS == audioFormat)
            {
                //opus 转成wav
                XString strWavTmpPath = strPath + __XT(".wav");               
                DecodeOPUSFileToWAVFile(strPath, strWavTmpPath);
                youmecommon::CXFile::remove_file(strPath);
                strPath = strWavTmpPath;
            }
        }        
		m_pCallback->OnSendAudioMessageStatus(serverPacket.reqSerial, errorCode, strText.c_str(), strPath.c_str(), nTime, sendTime, isForbidRoom, reasonType, forbidEndTime,svrMsgID);
	}
	else
	{
		m_pCallback->OnSendMessageStatus(serverPacket.reqSerial, errorCode, sendTime, isForbidRoom, reasonType, forbidEndTime,svrMsgID);
	}

	if (ullMessageSerial != 0 && (YIMErrorcode_Success == errorCode || YIMErrorcode_PTT_ReachMaxDuration == errorCode))
	{
		if (messageInfo.chatType == ChatType_PrivateChat)
		{
			SaveSenderHistoryMessage(svrMsgID, messageInfo);
		}
		else if (messageInfo.chatType == ChatType_RoomChat)
		{
			if (m_msgRecordRooms.find(UTF8TOXString(messageInfo.receiverID)) != m_msgRecordRooms.end())
			{
				SaveSenderHistoryMessage(svrMsgID, messageInfo);
			}
		}
	}
}

void YouMeIMMessageManager::OnReceiveMessageNotify(ServerPacket& serverPacket)
{
	if (NULL == m_pCallback)
	{
		YouMe_LOG_Warning(__XT("message callback is NULL"));
		return;
	}
	
	YOUMEServiceProtocol::NotifyRsp rsp;
	if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
	{
		YouMe_LOG_Error(__XT("send message response unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
		return;
	}
    // 为暂停标志且收到消息时
    if (m_pauseReceivingFlag && (YOUMEServiceProtocol::NOTIFY_PRIVATE_MSG == serverPacket.commondID || YOUMEServiceProtocol::NOTIFY_ROOM_MSG == serverPacket.commondID || YOUMEServiceProtocol::NOTIFY_PRIVATE_MSG_V2 == serverPacket.commondID || YOUMEServiceProtocol::NOTIFY_ROOM_MSG_V2 == serverPacket.commondID || YOUMEServiceProtocol::NOTIFY_ROOM_GENERAL == serverPacket.commondID))
    {
        YouMe_LOG_Debug(__XT("Enter background, pauses to receive messages."));
        return;
    }

	XUINT64 ullMsgSerial = 0;
	XString strTarget;

	for (int i = 0; i < rsp.args_size(); i++)
	{
		std::string strName = rsp.args(i).name();
		if (strName == "msg_id")
		{
			ullMsgSerial = CStringUtilT<char>::str_to_uint64(rsp.args(i).value().c_str());
		}
		else if (strName == "msg_for")
		{
			strTarget = UTF8TOXString(rsp.args(i).value());
		}
	}

	if (YOUMEServiceProtocol::NOTIFY_ROOM_MSG == serverPacket.commondID || YOUMEServiceProtocol::NOTIFY_ROOM_MSG_V2 == serverPacket.commondID)
	{
		if (m_msgShieldTargets.find(strTarget) != m_msgShieldTargets.end())
		{
			m_pCallback->OnReceiveMessageNotify(ChatType_RoomChat, strTarget.c_str());
			return;
		}
	}

	if (!m_bGetMessageFinish)
	{
		m_bMessageNotify = true;
		return;
	}

	if (YOUMEServiceProtocol::NOTIFY_PRIVATE_MSG_V2 == serverPacket.commondID || YOUMEServiceProtocol::NOTIFY_ROOM_MSG_V2 == serverPacket.commondID || rsp.version() == 2)
	{
		ullMsgSerial = serverPacket.reqSerial;
	}
	if (0 == ullMsgSerial || m_pIManager == NULL)
	{
		return;
	}

	XUINT64 ullCurMsgSerial = m_ullLastMessageID;
	if (0 == ullCurMsgSerial)
	{
		XString strLastMsgSerial;
		std::string strKey = CStringUtilT<char>::formatString("LastMsgSerial_%d_%s_%d", m_pIManager->GetAppID(), XStringToUTF8(m_pIManager->GetCurrentUser()).c_str(), g_iYouMeIMMode);
		g_pProfileDB->getSetting(strKey, strLastMsgSerial);
		if (!strLastMsgSerial.empty())
		{
			ullCurMsgSerial = CStringUtil::str_to_sint64(strLastMsgSerial);
		}
	}
	
	YouMe_LOG_Debug(__XT("get message notify ullMsgSerial:%lld ullCurMsgSerial:%llu"), ullMsgSerial, ullCurMsgSerial);
	if (ullMsgSerial > ullCurMsgSerial)
	{
		std::string targes;
		RequestGetMessage(ullCurMsgSerial, targes);
	}
}

YIMErrorcode YouMeIMMessageManager::RequestGetMessage(XUINT64 messageSerial, std::string& targets, bool flag)
{
	std::string strRooms;
	XString strTargets = UTF8TOXString(targets);
	YOUMEServiceProtocol::RoomList roomList;
	for (std::set<XString>::const_iterator itr = m_msgShieldTargets.begin(); itr != m_msgShieldTargets.end(); ++itr)
	{
		if (strTargets.find(*itr) == XString::npos)
		{
			roomList.add_room_name(XStringToUTF8((*itr)));
		}
	}
	if (roomList.room_name_size() > 0)
	{
		roomList.SerializeToString(&strRooms);
	}
	
	YOUMEServiceProtocol::GetMsgReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_msgid(messageSerial);
	req.set_room_mask(strRooms.c_str());
	std::string strData;
	req.SerializeToString(&strData);
	XUINT64 msgSerial = 0;
	std::map<std::string, std::string> extend;
	extend["Targets"] = targets;
	if (flag)	//主动拉取
	{
		extend["Initiative"] = "1";
	}
	if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_GET_MSG, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		YouMe_LOG_Error(__XT("request get message failed"));
		return YIMErrorcode_UnknowError;
	}
	m_bGetMessageFinish = false;
	YouMe_LOG_Debug(__XT("request getmessage messageID:%llu"), messageSerial);
	return YIMErrorcode_Success;
}

void YouMeIMMessageManager::OnReceiveMessageRsp(ServerPacket& serverPacket)
{
	m_bGetMessageFinish = true;
	if (serverPacket.result == -1)
	{
		YouMe_LOG_Error(__XT("get message timeout"));
		return;
	}

	YOUMEServiceProtocol::GetMsgRsp rsp;
	if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
	{
		YouMe_LOG_Error(__XT("receive message unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
		ReportDataMessageStatus(1, 0, REPORTCODE_UNPACK_EEROR, 0, 0);
		return;
	}
	if (rsp.ret() != 0)
	{
		YouMe_LOG_Error(__XT("receive message error(%d)"), rsp.ret());
		ReportDataMessageStatus(1, 0, rsp.ret(), 0, 0);
		return;
	}

	XUINT64 ullMaxMessageID = 0;
	for (int i = 0; i < rsp.msg_list_size(); ++i)
	{
		XUINT64 ullMessageID = rsp.msg_list(i).msgid();
		XString senderID = UTF8TOXString(rsp.msg_list(i).sender_id());
		if (m_blockUserList.count(senderID) > 0)	// 屏蔽消息
		{
			if (ullMessageID > ullMaxMessageID)
			{
				ullMaxMessageID = ullMessageID;
			}
			continue;
		}

		ReceiveMessage(rsp.msg_list(i));

		if (m_messageIdSet.find(ullMessageID) != m_messageIdSet.end())
		{
			YouMe_LOG_Debug(__XT("duplicate message messageID:%llu"), ullMessageID);
			continue;
		}
		if (ullMessageID != 0)
		{
			m_messageIdSet.insert(ullMessageID);
		}
		if (ullMessageID > ullMaxMessageID)
		{
			ullMaxMessageID = ullMessageID;
		}
	}
	if (ullMaxMessageID != 0)
	{
		UpdateRecvMessageSerial(ullMaxMessageID);
		m_ullLastMessageID = ullMaxMessageID;
	}

	bool bIsInitiative = false;	//是否主动拉取消息
	std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("Initiative");
	if (itr != serverPacket.extend.end())
	{
		bIsInitiative = true;
	}
	bool bGetMessage = false;
	if (rsp.msg_cnt() >= m_iMsgPageSize)
	{
		std::string targes;
		std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("Targets");
		if (itr != serverPacket.extend.end())
		{
			targes = itr->second;
		}
		if (ullMaxMessageID != 0)
		{
			RequestGetMessage(ullMaxMessageID, targes);
		}

		bGetMessage = true;
	}
	else
	{
		if (m_bMessageNotify)
		{
			m_bMessageNotify = false;

			XUINT64 ullCurMsgSerial = m_ullLastMessageID;
			if (0 == ullCurMsgSerial)
			{
				XString strLastMsgSerial;
				std::string strKey = CStringUtilT<char>::formatString("LastMsgSerial_%d_%s_%d", m_pIManager->GetAppID(), XStringToUTF8(m_pIManager->GetCurrentUser()).c_str(), g_iYouMeIMMode);
				g_pProfileDB->getSetting(strKey, strLastMsgSerial);
				if (!strLastMsgSerial.empty())
				{
					ullCurMsgSerial = CStringUtil::str_to_sint64(strLastMsgSerial);
				}
			}
			
			std::string targes;
			std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("Targets");
			if (itr != serverPacket.extend.end())
			{
				targes = itr->second;
			}
			RequestGetMessage(ullCurMsgSerial, targes);
		}
	}

	if (bGetMessage || !bIsInitiative)
	{
		m_pIManager->SetInitiativeGetMessageCount();
		m_llLastGetMessageTime = youmecommon::CTimeUtil::GetTimeOfDay_MS();
	}

	YouMe_LOG_Debug(__XT("receive message total:%d maxMessageID:%llu"), rsp.msg_list_size(), ullMaxMessageID);
}

bool YouMeIMMessageManager::ReceiveMessage(const YOUMEServiceProtocol::YoumeMsg& message, bool roomHistoryMessage)
{
    XString strSenderID = UTF8TOXString(message.sender_id());
	YouMe_LOG_Debug(__XT("receive message messageID:%llu type:%d chatType:%d sender:%s sendSerial:%llu serverMessageID:%llu"), message.msgid(), message.msg_type(), message.chat_type(), strSenderID.c_str(), message.sender_serial(), message.svr_msgid());

	// 房间历史消息msgid为0
	if (!roomHistoryMessage && (message.msgid() == 0 || m_pCallback == NULL))
	{
		YouMe_LOG_Error(__XT("messageID:%llu or callback is null"), message.msgid());
		return true;
	}
	/*if ((m_pIManager != NULL && message.sender_id() == m_pIManager->GetCurrentUser()))
	{
		YouMe_LOG_Debug(__XT("receive self message"));
		UpdateRecvMessageSerial(message.msgid());
		return;
	}*/

	if (!roomHistoryMessage)
	{
		std::map<std::string, std::set<XUINT64> >::iterator itrSender = m_senderSerialMap.find(message.sender_id());
		if (itrSender != m_senderSerialMap.end())
		{
			std::set<XUINT64>::const_iterator itrSerial = itrSender->second.find(message.sender_serial());
			if (itrSerial != itrSender->second.end())
			{
				YouMe_LOG_Debug(__XT("duplicate message messageID:%llu sender:%s sendSerial:%llu"), message.msgid(), strSenderID.c_str(), message.sender_serial());
				return false;
			}
			else
			{
				itrSender->second.insert(message.sender_serial());
			}
		}
		else
		{
			std::set<XUINT64> serialSet;
			serialSet.insert(message.sender_serial());
			m_senderSerialMap.insert(std::map<std::string, std::set<XUINT64> >::value_type(message.sender_id(), serialSet));
		}
	}

	std::shared_ptr<CYouMeIMMessage> pMessage = NULL;
	if (!ResolveMessage(message, pMessage) || pMessage == NULL)
	{
		YouMe_LOG_Error(__XT("resolve message error"));
		return false;
	}
    XUINT64 autoID = m_pIManager->GetAutoIncrementID();
//    if(pMessage->GetMessageBody()->GetMessageType() == MessageBodyType_TXT){
//        pMessage->m_ulMsgSerial = autoID;
//    }
    
    m_pCallback->OnRecvMessage(pMessage);
    
    if (pMessage->GetChatType() == ChatType_PrivateChat)
    {
        m_messageRecorder.SaveHistoryMessage(pMessage.get(),false,autoID,m_pIManager->GetServerValidTime());
    }
    else if (pMessage->GetChatType() == ChatType_RoomChat && !roomHistoryMessage)
    {
        if (m_msgRecordRooms.find(pMessage->m_strReceiveID) != m_msgRecordRooms.end())
        {
            m_messageRecorder.SaveHistoryMessage(pMessage.get(),false,autoID,m_pIManager->GetServerValidTime());
        }
    }
    
    //发文本、自定义、礼物消息上报前5条
    if (m_iRecvReportCount < MESSAGE_REPORT_MAX_TIMES && (YOUMEServiceProtocol::CMD_SND_TEXT_MSG == message.msg_type() || YOUMEServiceProtocol::CMD_SND_BIN_MSG == message.msg_type() || YOUMEServiceProtocol::CMD_SND_GIFT_MSG == message.msg_type()))
    {
        ++m_iRecvReportCount;
        ReportDataMessageStatus(1, message.msg_type(), REPORTCODE_SUCCESS, 0, 0);
    }

	// 消息耗时统计
	if (m_pIManager != NULL)
	{
		XINT64 currentTime = m_pIManager->GetServerValidTime();
		if (m_pIManager->GetLoginTime() != 0 && currentTime - m_pIManager->GetLoginTime() > 120000)	// 登录2分钟之后的，避免离线消息错误数据
		{
			for (int i = 0; i < message.comment().comment_size(); i++)
			{
				std::string key = message.comment().comment(i).name();
				if ("SendTime" == key)
				{
					XINT64 sendTime = CStringUtilT<char>::str_to_sint64(message.comment().comment(i).value());
					XINT64 costTime = currentTime - sendTime;
					ReportDataMessageTimeCost(costTime, message.sender_serial());
					break;
				}
			}
		}
	}
	
    return true;
}

bool YouMeIMMessageManager::ResolveMessage(const YOUMEServiceProtocol::YoumeMsg& message, std::shared_ptr<CYouMeIMMessage>& pMessage)
{
	pMessage = std::shared_ptr<CYouMeIMMessage>(new CYouMeIMMessage);
	pMessage->m_chatType = (YIMChatType)message.chat_type();
	pMessage->m_strReceiveID = UTF8TOXString(message.recver_id());
	if (pMessage->m_strReceiveID == __XT("iMtUlLuTmIsB"))
	{
		pMessage->m_strReceiveID = m_pIManager->GetCurrentUser();
	}
	pMessage->m_strSenderid = UTF8TOXString(message.sender_id());
	if (message.svr_msgid() != 0)
	{
		pMessage->m_ulMsgSerial = message.svr_msgid();
	}
	else
	{
		pMessage->m_ulMsgSerial = m_pIManager->GetUniqueSerial();
	}

	pMessage->m_iCreateTime = message.create_time();

	// 距离
	YouMeLocationManager* pLocationManager = dynamic_cast<YouMeLocationManager*>(m_pIManager->GetLocationManager());
	if (pLocationManager)
	{
		LocationInfo locationInfo = pLocationManager->GetCurrentUserLocationInfo();
		double senderLongitude = 0;
		double senderLatitude = 0;

		for (int i = 0; i < message.comment().comment_size(); i++)
		{
			std::string strKey = message.comment().comment(i).name();
			if ("Longitude" == strKey)
			{
				senderLongitude = CStringUtilT<char>::str_to_double(message.comment().comment(i).value());
			}
			if ("Latitude" == strKey)
			{
				senderLatitude = CStringUtilT<char>::str_to_double(message.comment().comment(i).value());
			}
		}
		if (locationInfo.longitude != 0 && locationInfo.latitude != 0 && senderLongitude != 0 && senderLatitude != 0)
		{
			pMessage->m_iDistance = LocationUtil::GetDistanceSimplify(Point(locationInfo.longitude, locationInfo.latitude), Point(senderLongitude, senderLatitude));
		}
	}

	switch (message.msg_type())
	{
	case YOUMEServiceProtocol::CMD_SND_TEXT_MSG:
	{
		CYouMeIMMessageBodyText* pMsgBody = new CYouMeIMMessageBodyText;
		pMsgBody->m_messageType = MessageBodyType_TXT;
		pMsgBody->SetMessageContent(UTF8TOXString(message.msg_content()).c_str());
        for (int i = 0; i < message.comment().comment_size(); i++)
        {
            std::string strKey = message.comment().comment(i).name();
            if ("AttachParam" == strKey)
            {
                pMsgBody->m_strAttachParam = UTF8TOXString(message.comment().comment(i).value());
            }
        }
		pMessage->SetMessageBody(pMsgBody);
	}
	break;
	case YOUMEServiceProtocol::CMD_SND_VOICE_MSG:
	{
		std::string strAudioTime("0");
		CYouMeIMMessageBodyAudio* pMsgBody = new CYouMeIMMessageBodyAudio;
		pMsgBody->m_messageType = MessageBodyType_Voice;

		std::string content = message.msg_content();
		if (strncmp("http", &content[0], 4) == 0)
		{
            YouMe_LOG_Info(__XT("app voice"));
			pMsgBody->m_strDownloadURL = UTF8TOXString(content);
		}
		else
		{
			youmecommon::Value value;
			youmecommon::Reader jsonReader;
			if (jsonReader.parse(content, value))
			{
				if (value.isMember("mediaid"))	// H5微信语音
				{
                    YouMe_LOG_Info(__XT("h5 weixin"));
					std::string strMediaID = value["mediaid"].asString();
					if (!strMediaID.empty())
					{
						std::string downloadUrl = CStringUtilT<char>::formatString(H5_WEIXIN_AUDIO_DOWNLOAD_URL, m_strWeiXinTocken.c_str(), strMediaID.c_str());
                        YouMe_LOG_Info(__XT("Download url: %s"),downloadUrl.c_str());
						pMsgBody->m_strDownloadURL = UTF8TOXString(downloadUrl);
					}
				}
				else if (value.isMember("downloadurl"))	// WEBH5
				{
                    YouMe_LOG_Info(__XT("h5 web"));
					std::string url = value["downloadurl"].asString();
					if (strncmp(url.c_str(), "http", 4) != 0)	//web不是全路径
					{
						url = "https:" + url;
					}
					pMsgBody->m_strDownloadURL = UTF8TOXString(url);
				}

				if (value.isMember("voicetime"))
				{
					pMsgBody->m_iTime = value["voicetime"].asInt();
					strAudioTime = CStringUtilT<char>::to_string(pMsgBody->m_iTime);
				}
			}
		}

		for (int i = 0; i < message.comment().comment_size(); i++)
		{
			std::string strKey = message.comment().comment(i).name();
			if ("AudioText" == strKey)
			{
				pMsgBody->m_strText = UTF8TOXString(message.comment().comment(i).value());
			}
			else if ("FileSize" == strKey)
			{
				pMsgBody->m_iFileSize = CStringUtilT<char>::str_to_uint32(message.comment().comment(i).value());
			}
			else if ("Time" == strKey)
			{
				pMsgBody->m_iTime = CStringUtilT<char>::str_to_uint32(message.comment().comment(i).value());
				strAudioTime = message.comment().comment(i).value();
			}
			else if ("Param" == strKey)
			{
				pMsgBody->m_strParam = UTF8TOXString(message.comment().comment(i).value());
			}
		}
		pMessage->SetMessageBody(pMsgBody);

		FileDownloadInfo downloadInfo;
		downloadInfo.downloadURL = pMsgBody->m_strDownloadURL;
		downloadInfo.msgSerial = pMessage->m_ulMsgSerial;
		downloadInfo.fileType = FileType_Audio;
		downloadInfo.extend["Time"] = strAudioTime;
		downloadInfo.msg = pMessage;
		{
			std::lock_guard<std::mutex> lock(m_DownloadInfoLock);
			m_downloadInfo[pMessage->m_ulMsgSerial] = downloadInfo;
		}
        
        //自动下载语音消息
        if (m_downloadFlag)
        {
            XString pretermitPath = m_pIManager->GetAudioCachePath() + CStringUtilT<XCHAR>::to_string(pMessage->GetMessageID()) + __XT(".wav");
            
            YIMErrorcode error_download = DownloadFile(pMessage->GetMessageID(), pretermitPath.c_str());
            if (error_download == YIMErrorcode_Success)
            {
                YouMe_LOG_Info(__XT("Download voice messages automatically success, save path: %s"),pretermitPath.c_str());
            }
            else
            {
                YouMe_LOG_Error(__XT("Download voice messages fail, errorCode: %d"), error_download);
            }
        }
	}
	break;
	case YOUMEServiceProtocol::CMD_SND_FILE_MSG:
	{
		CYouMeIMMessageBodyFile* pMsgBody = new CYouMeIMMessageBodyFile;
		pMsgBody->m_messageType = MessageBodyType_File;
		pMsgBody->m_strDownloadURL = UTF8TOXString(message.msg_content());
		for (int i = 0; i < message.comment().comment_size(); i++)
		{
			std::string strKey = message.comment().comment(i).name();
			if ("FileSize" == strKey)
			{
				pMsgBody->m_iFileSize = CStringUtilT<char>::str_to_uint32(message.comment().comment(i).value());
			}
			else if ("Extension" == strKey)
			{
				pMsgBody->m_strExtension = UTF8TOXString(message.comment().comment(i).value());
			}
			else if ("FileName" == strKey)
			{
				pMsgBody->m_strFileName = UTF8TOXString(message.comment().comment(i).value());
			}
			else if ("Param" == strKey)
			{
				pMsgBody->m_strParam = UTF8TOXString(message.comment().comment(i).value());
			}
			else if ("FileType" == strKey)
			{
				pMsgBody->m_fileType = (YIMFileType)CStringUtilT<char>::str_to_uint32(message.comment().comment(i).value());
			}
		}
		pMessage->SetMessageBody(pMsgBody);

		FileDownloadInfo downloadInfo;
		downloadInfo.downloadURL = pMsgBody->m_strDownloadURL;
		downloadInfo.msgSerial = pMessage->m_ulMsgSerial;
		downloadInfo.fileType = pMsgBody->m_fileType;
		downloadInfo.msg = pMessage;
		{
			std::lock_guard<std::mutex> lock(m_DownloadInfoLock);
			m_downloadInfo[pMessage->m_ulMsgSerial] = downloadInfo;
		}
	}
	break;
	case YOUMEServiceProtocol::CMD_SND_BIN_MSG:
	{
		CYouMeIMMessageBodyCustom* pMsgBody = new CYouMeIMMessageBodyCustom;
		pMsgBody->m_messageType = MessageBodyType_CustomMesssage;
		pMsgBody->SetCustomMessage(message.msg_content());
		pMessage->SetMessageBody(pMsgBody);
	}
	break;
	case YOUMEServiceProtocol::CMD_SND_GIFT_MSG:
	{
		CYouMeIMMessageBodyGift* pMsgBody = new CYouMeIMMessageBodyGift;
		pMsgBody->m_messageType = MessageBodyType_Gift;

		youmecommon::Value value;
		youmecommon::Reader reader;
		if (reader.parse(message.msg_content(), value))
		{
			std::string strSender = value["from"].asString();
			if (!strSender.empty())
			{
				pMessage->m_strSenderid = UTF8TOXString(strSender);
			}
			std::string strReceiver = value["to"].asString();
			if (!strReceiver.empty())
			{
				pMsgBody->m_strAnchorID = UTF8TOXString(strReceiver);
			}
			pMsgBody->m_iGiftID = CStringUtilT<char>::str_to_sint32(value["id"].asString());
			pMsgBody->m_iCount = CStringUtilT<char>::str_to_uint32(value["count"].asString());
		}

		youmecommon::Value jsonVal;
		for (int i = 0; i < message.comment().comment_size(); ++i)
		{
			jsonVal[message.comment().comment(i).name()] = message.comment().comment(i).value();
		}
		std::string strResult = jsonVal.toStyledString();
		if (!strResult.empty())
		{
			pMsgBody->m_strParam = UTF8TOXString(strResult);
		}

		pMessage->SetMessageBody(pMsgBody);
	}
	break;
	default:
	{
		YouMe_LOG_Error(__XT("unknow message type %d"), message.msg_type());
		return false;
	}
	}

	return true;
}

YIMErrorcode YouMeIMMessageManager::RequestUploadToken(FileUploadInfo& uploadInfo)
{
	XString srtFileMD5 = youmecommon::CCryptUtil::MD5File(uploadInfo.localPath);
	YOUMEServiceProtocol::GetUploadTokenReq req;
	req.set_version(PROTOCOL_VERSION_HTTPS);
	req.set_file_size(uploadInfo.fileSize);
	req.set_file_md5(XStringToUTF8(srtFileMD5));
	req.set_file_suffix(XStringToUTF8(uploadInfo.extension));
	std::string strData;
	req.SerializeToString(&strData);
	XUINT64 msgSerial = 0;
	std::map<std::string, std::string> extend;
	extend["uploadSerial"] = CStringUtilT<char>::to_string(uploadInfo.msgSerial);
	if (m_pIManager->SendData(YOUMEServiceProtocol::CMD_GET_UPLOAD_TOKEN, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		return YIMErrorcode_Success;
	}
	return YIMErrorcode_NetError;
}

void YouMeIMMessageManager::OnGetUploadTokenRsp(ServerPacket& serverPacket)
{
	YouMe_LOG_Info(__XT("get upload token"));

	YIMErrorcode errorCode = YIMErrorcode_UnknowError;
	int nReportErrorCode = REPORTCODE_SUCCESS;
	XUINT64 ullUploadSerial = 0;
	do 
	{
		std::map<std::string, std::string>::const_iterator itrSerial = serverPacket.extend.find("uploadSerial");
		if (itrSerial == serverPacket.extend.end())
		{
			YouMe_LOG_Error(__XT("uploadSerial not found %llu"), serverPacket.reqSerial);
			errorCode = YIMErrorcode_UnknowError;
			nReportErrorCode = REPORTCODE_UNKNOW_ERROR;
			break;
		}
		ullUploadSerial = CStringUtilT<char>::str_to_uint64(itrSerial->second);

		if (serverPacket.result == -1)
		{
			YouMe_LOG_Error(__XT("get token timeout serial:%llu"), ullUploadSerial);
			errorCode = YIMErrorcode_TimeOut;
			nReportErrorCode = REPORTCODE_TIMEOUT;
			break;
		}

		YOUMEServiceProtocol::GetUploadTokenRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
			errorCode = YIMErrorcode_ServerError;
			nReportErrorCode = REPORTCODE_UNPACK_EEROR;
			break;
		}
		if (rsp.ret() != 0)
		{
			YouMe_LOG_Error(__XT("get upload token error(%d) serial:%llu"), rsp.ret(), ullUploadSerial);
			errorCode = YIMErrorcode_ServerError;
			nReportErrorCode = rsp.ret();
			break;
		}

		std::lock_guard<std::mutex> lock(m_uploadInfoLock);
		std::map<XUINT64, std::shared_ptr<FileUploadInfo> >::iterator itr = m_fileUploadMap.find(ullUploadSerial);
		if (itr == m_fileUploadMap.end())
		{
			YouMe_LOG_Error(__XT("has not found in upload map serial:%llu"), ullUploadSerial);
			errorCode = YIMErrorcode_UnknowError;
			nReportErrorCode = REPORTCODE_UNKNOW_ERROR;
			break;
		}
		
		if (rsp.headers_size() > 0)
		{
			errorCode = YIMErrorcode_Success;
			itr->second->uploadURL = UTF8TOXString(rsp.token());
			itr->second->downloadURL = UTF8TOXString(rsp.download_url());
			itr->second->bucketType = (BucketType) rsp.bucket_type();

			for (int i = 0; i < rsp.headers_size(); ++i)
			{
				itr->second->httpHead.insert(std::map<std::string, std::string>::value_type(rsp.headers(i).name(), rsp.headers(i).value()));
			}
			if (itr->second->bucketType == BUCKET_TYPE_COS)
			{
				itr->second->httpHead.insert(std::map<std::string, std::string>::value_type("Accept", "*/*"));
				itr->second->httpHead.insert(std::map<std::string, std::string>::value_type("Connection", "Keep-Alive"));
				itr->second->httpHead.insert(std::map<std::string, std::string>::value_type("User-Agent", "cos-cpp-sdk-v4.2"));
			}

			m_uploadSemaphore.Increment();
		}
		else
		{
			YouMe_LOG_Error(__XT("no http head serial:%llu"), ullUploadSerial);
			errorCode = YIMErrorcode_ServerError;
			nReportErrorCode = REPORTCODE_NO_HTTP_HEAD;
		}
	} while (0);
	
	if (errorCode != YIMErrorcode_Success && ullUploadSerial != 0)
	{
		YIMFileType fileType = FileType_Other;
		XString strReceiverID(__XT(""));
		{
			std::lock_guard<std::mutex> lock(m_uploadInfoLock);
			std::map<XUINT64, std::shared_ptr<FileUploadInfo> >::iterator itr = m_fileUploadMap.find(ullUploadSerial);
			if (itr != m_fileUploadMap.end())
			{
				fileType = itr->second->fileType;
				strReceiverID = itr->second->receiverID;
				m_fileUploadMap.erase(itr);
			}
		}
		if (m_pCallback != NULL)
		{
			if (fileType == FileType_Audio)
			{
				if (strReceiverID == __XT(""))
				{
					std::shared_ptr<CAudioSpeechInfo> pAudioSpeechInfo(new CAudioSpeechInfo);
					pAudioSpeechInfo->m_requestID = ullUploadSerial;
					m_pCallback->OnStopAudioSpeechStatus(errorCode, pAudioSpeechInfo);
				}
				else
				{
					m_pCallback->OnSendAudioMessageStatus(ullUploadSerial, errorCode, __XT(""), __XT(""), 0, 0, false, 0, 0, 0);
				}
			}
			else
			{
				m_pCallback->OnSendMessageStatus(ullUploadSerial, errorCode,false, 0, 0, 0, 0);
			}
		}
	}

	if (nReportErrorCode != REPORTCODE_SUCCESS)
	{
		ReportDataMessageStatus(0, serverPacket.commondID, nReportErrorCode, 0, 0);
	}
}

YIMErrorcode YouMeIMMessageManager::QueryRoomHistoryMessageFromServer(const XCHAR* roomID, int count, int direction)
{
	if (g_pSpeechManager == NULL || m_pIManager == NULL || (m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success && m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Reconnecting))
	{
		return YIMErrorcode_NotLogin;
	}

	if (NULL == roomID || XStrLen(roomID) == 0)
	{
		return YIMErrorcode_ParamInvalid;
	}

	std::string strRoomID = XStringToUTF8(XString(roomID));
	YOUMEServiceProtocol::GetRoomHistoryMsgReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_room_name(strRoomID);
	std::string strData;
	req.SerializeToString(&strData);
	XUINT64 msgSerial = 0;
	std::map<std::string, std::string> extend;
	extend["Count"] = CStringUtilT<char>::to_string(count);
	extend["Direction"] = CStringUtilT<char>::to_string(direction);
	extend["RoomID"] = strRoomID;
	if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_GET_ROOM_HISTORY_MSG, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		YouMe_LOG_Error(__XT("request get message failed"));
		return YIMErrorcode_UnknowError;
	}
	return YIMErrorcode_Success;
}

YIMErrorcode YouMeIMMessageManager::GetNewMessage(const std::vector<XString>& vecRoomIDs)
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YIMErrorcode_NotLogin;
	}
    
    if( vecRoomIDs.empty())
	{
        return YIMErrorcode_ParamInvalid;
    }

	std::string roomIDs;
    for (unsigned int i = 0; i < vecRoomIDs.size(); ++i)
    {
        std::string strTemp = XStringToUTF8(vecRoomIDs[i]);
        if (!strTemp.empty())
        {
            roomIDs += strTemp + ";";
        }
    }

	XUINT64 ullCurMsgSerial = m_ullLastMessageID;
	if (0 == ullCurMsgSerial)
	{
		XString strLastMsgSerial;
		std::string strKey = CStringUtilT<char>::formatString("LastMsgSerial_%d_%s_%d", m_pIManager->GetAppID(), XStringToUTF8(m_pIManager->GetCurrentUser()).c_str(), g_iYouMeIMMode);
		g_pProfileDB->getSetting(strKey, strLastMsgSerial);
		if (!strLastMsgSerial.empty())
		{
			ullCurMsgSerial = CStringUtil::str_to_sint64(strLastMsgSerial);
		}
	}
	return RequestGetMessage(ullCurMsgSerial, roomIDs);
}

void YouMeIMMessageManager::GetNewMessage(bool initiative)
{
	if (NULL == m_pIManager)
	{
		return;
	}
	XUINT64 ullCurMsgSerial = m_ullLastMessageID;
	if (0 == ullCurMsgSerial)
	{
		XString strLastMsgSerial;
		std::string strKey = CStringUtilT<char>::formatString("LastMsgSerial_%d_%s_%d", m_pIManager->GetAppID(), XStringToUTF8(m_pIManager->GetCurrentUser()).c_str(), g_iYouMeIMMode);
		g_pProfileDB->getSetting(strKey, strLastMsgSerial);
		if (!strLastMsgSerial.empty())
		{
			ullCurMsgSerial = CStringUtil::str_to_sint64(strLastMsgSerial);
		}
	}
	
	std::string targets;
	RequestGetMessage(ullCurMsgSerial, targets, initiative);
	YouMe_LOG_Debug(__XT("initiative get message serial:%llu"), ullCurMsgSerial);
}

void YouMeIMMessageManager::UpdateConfig()
{
	if (m_pIManager == NULL)
	{
		return;
	}
	m_bMicVolumeGain = m_pIManager->GetConfigure<unsigned int>(CONIFG_MIC_VOLUME_GAIN, 0);
	m_iUploadRetryTimes = m_pIManager->GetConfigure<int>(CONFIG_UPLOAD_RETRY_TIMES, 1);
}

void YouMeIMMessageManager::InitMessageManager()
{
	m_messageRecorder.Init();
	if (m_pIManager != NULL)
	{
		m_iMsgPageSize = m_pIManager->GetConfigure<unsigned int>(CONFIG_MSG_PAGE_SIZE, MSG_PAGE_SIZE_DEFAULT);
	}
	UpdateConfig();
	if (m_strLastUserID != m_pIManager->GetCurrentUser())
	{
		m_bRequestBlockList = false;
		m_blockUserList.clear();
		RequestBlockUserList(0, true);
	}
	if (m_strWeiXinTocken.empty() && g_YM_isPrivateServer==0)
	{
		RequestWeiXinAccessToken();
	}

	if (m_strDownloadDir.empty())
	{
#ifdef WIN32
		wchar_t wszPath[MAX_PATH] = { 0 };
		if (GetTempPathW(MAX_PATH, wszPath) > 0)
		{
			m_strDownloadDir = std::wstring(wszPath);
			m_strDownloadDir.append(L"AudioRecv\\");
		}
#elif (OS_IOS || OS_IOSSIMULATOR)
		if (g_pSystemProvider != NULL)
		{
			m_strDownloadDir = youmecommon::CXFile::CombinePath(g_pSystemProvider->getCachePath(), __XT("AudioRecv/"));
		}
#elif OS_ANDROID
		if (g_pSystemProvider != NULL)
		{
			m_strDownloadDir = youmecommon::CXFile::CombinePath(g_pSystemProvider->getCachePath(), __XT("AudioRecv/"));
		}
#endif
	}

	SetDownloadDir(m_strDownloadDir.c_str());
}

YIMErrorcode YouMeIMMessageManager::GetRecentContacts()
{
	return m_messageRecorder.QueryRecentContacts();
}

void YouMeIMMessageManager::OnRoomHistoryMessageRsp(ServerPacket& serverPacket)
{
	if (m_pCallback == NULL)
	{
		YouMe_LOG_Error(__XT("message callback is null"));
		return;
	}

	XString roomID;
	std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("RoomID");
	if (itr == serverPacket.extend.end())
	{
		YouMe_LOG_Error(__XT("room id not found"));
		return;
	}
	roomID = UTF8TOXString(itr->second);

	if (serverPacket.result == -1)
	{
		YouMe_LOG_Error(__XT("get room history message timeout"));

		std::list< std::shared_ptr<IYIMMessage> > messageList;
		m_pCallback->OnQueryRoomHistoryMessageFromServer(YIMErrorcode_TimeOut, roomID.c_str(), 0, messageList);
		return;
	}

	YOUMEServiceProtocol::GetMsgRsp rsp;
	if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
	{
		YouMe_LOG_Error(__XT("receive room history message unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
		return;
	}
	if (rsp.ret() != 0)
	{
		YouMe_LOG_Error(__XT("receive room history message error(%d)"), rsp.ret());
		return;
	}
	YouMe_LOG_Debug(__XT("room history message total:%d"), rsp.msg_list_size());

	std::list< std::shared_ptr<IYIMMessage> > messageList;
	if (0 == rsp.msg_list_size())
	{
		m_pCallback->OnQueryRoomHistoryMessageFromServer(YIMErrorcode_Success, roomID.c_str(), 0, messageList);
	}
	else
	{
		int count = 0;
		int direction = 0;
		itr = serverPacket.extend.find("Count");
		if (itr != serverPacket.extend.end())
		{
			count = CStringUtilT<char>::str_to_sint32(itr->second.c_str());
		}
		itr = serverPacket.extend.find("Direction");
		if (itr != serverPacket.extend.end())
		{
			direction = CStringUtilT<char>::str_to_sint32(itr->second.c_str());
		}

		if (0 == count || count > rsp.msg_list_size())
		{
			count = rsp.msg_list_size();
		}
		if (0 == direction)
		{
			int startIndex = rsp.msg_list_size() - count;
			for (int i = 0; i < count;)
			{
				std::shared_ptr<CYouMeIMMessage> pMessage = NULL;
				if (ResolveMessage(rsp.msg_list(i + startIndex), pMessage) || pMessage != NULL)
				{
					messageList.push_back(pMessage);
                    
					/*if (pMessage->GetChatType() == ChatType_PrivateChat)
                    {
                        m_messageRecorder.SaveHistoryMessage(pMessage.get(), true, 
                                                             m_pIManager->GetAutoIncrementID(),
                                                             m_pIManager->GetServerValidTime());
                    }
                    else if (pMessage->GetChatType() == ChatType_RoomChat)
                    {
                        if (m_msgRecordRooms.find(pMessage->m_strReceiveID) != m_msgRecordRooms.end())
                        {
                            m_messageRecorder.SaveHistoryMessage(pMessage.get(), true, 
                                                                 m_pIManager->GetAutoIncrementID(), 
                                                                 m_pIManager->GetServerValidTime());
                        }
                    }
					*/
				}

				if (++i % 30 == 0 || i == count)
				{
					m_pCallback->OnQueryRoomHistoryMessageFromServer(YIMErrorcode_Success, roomID.c_str(), count - i, messageList);
					messageList.clear();
				}
			}
		}
		else
		{
			int endIndex = rsp.msg_list_size() - 1;
			for (int i = 0; i < count;)
			{
				std::shared_ptr<CYouMeIMMessage> pMessage = NULL;
				if (ResolveMessage(rsp.msg_list(endIndex - i), pMessage) && pMessage != NULL)
				{
					messageList.push_back(pMessage);

					/*
                    if (pMessage->GetChatType() == ChatType_PrivateChat)
                    {
                        m_messageRecorder.SaveHistoryMessage(pMessage.get(), true, 
                                                             m_pIManager->GetAutoIncrementID(), 
                                                             m_pIManager->GetServerValidTime());
                    }
                    else if (pMessage->GetChatType() == ChatType_RoomChat)
                    {
                        if (m_msgRecordRooms.find(pMessage->m_strReceiveID) != m_msgRecordRooms.end())
                        {
                            m_messageRecorder.SaveHistoryMessage(pMessage.get(), true, 
                                                                 m_pIManager->GetAutoIncrementID(), 
                                                                 m_pIManager->GetServerValidTime());
                        }
                    }
					*/
				}
                
				if (++i % 30 == 0 || i == count)
				{
					m_pCallback->OnQueryRoomHistoryMessageFromServer(YIMErrorcode_Success, roomID.c_str(), count - i, messageList);
					messageList.clear();
				}
			}
		}
	}
}

YIMErrorcode YouMeIMMessageManager::SendAudioMessage(FileUploadInfo& uploadInfo)
{
	MessageBufferInfo messsageInfo;
	messsageInfo.commandType = YOUMEServiceProtocol::CMD_SND_VOICE_MSG;
	messsageInfo.chatType = uploadInfo.chatType;
	messsageInfo.receiverID = XStringToUTF8(uploadInfo.receiverID);
	messsageInfo.messageBuffer = XStringToUTF8(uploadInfo.downloadURL);
	messsageInfo.extend["FileSize"] = CStringUtilT<char>::to_string(uploadInfo.fileSize);
	messsageInfo.extend["LocalPath"] = XStringToUTF8(uploadInfo.localPath);
	if (uploadInfo.extend.size() > 0)
	{
		messsageInfo.extend.insert(uploadInfo.extend.begin(), uploadInfo.extend.end());
	}
	
	if (SendMessage(messsageInfo, uploadInfo.msgSerial))
	{
		return YIMErrorcode_Success;
	}

	return YIMErrorcode_NetError;
}

YIMErrorcode YouMeIMMessageManager::SendFileMessage(FileUploadInfo& uploadInfo, XUINT64 autoid,XUINT64 startSendtime)
{
	MessageBufferInfo messsageInfo;
	messsageInfo.commandType = YOUMEServiceProtocol::CMD_SND_FILE_MSG;
	messsageInfo.chatType = uploadInfo.chatType;
	messsageInfo.receiverID = XStringToUTF8(uploadInfo.receiverID);
	messsageInfo.messageBuffer = XStringToUTF8(uploadInfo.downloadURL);
	messsageInfo.extend["FileSize"] = CStringUtilT<char>::to_string(uploadInfo.fileSize);
	messsageInfo.extend["Extension"] = XStringToUTF8(uploadInfo.extension);
	messsageInfo.extend["FileType"] = CStringUtilT<char>::to_string(uploadInfo.fileType);
	XString strFileName = youmecommon::CXFile::GetFileName(uploadInfo.localPath.c_str());
	if (!strFileName.empty())
	{
		messsageInfo.extend["FileName"] = XStringToUTF8(strFileName);
	}
	messsageInfo.extend["LocalPath"] = XStringToUTF8(uploadInfo.localPath);
    messsageInfo.extend["autoid"] = CStringUtilT<char>::to_string(autoid);
    messsageInfo.extend["SendTime"] = CStringUtilT<char>::to_string(startSendtime);
    
	if (uploadInfo.extend.size() > 0)
	{
		messsageInfo.extend.insert(uploadInfo.extend.begin(), uploadInfo.extend.end());
	}
	
	if (SendMessage(messsageInfo, uploadInfo.msgSerial))
	{
		return YIMErrorcode_Success;
	}

	return YIMErrorcode_NetError;
}

// type: 0:发送消息	1:接收消息 2:下载(URL)
void YouMeIMMessageManager::ReportDataMessageStatus(int type, int messageType, int status, unsigned int time, unsigned int audioTime)
{
	if (m_pIManager == NULL)
	{
		return;
	}
	ReportMessageState reportData;
	reportData.userid = XStringToUTF8(m_pIManager->GetCurrentUser());
	reportData.appid= m_pIManager->GetAppID();
	reportData.SDKVersion = SDK_VER;
	reportData.type = type;
	reportData.status = status;
	reportData.messageType = messageType;
	reportData.time = time;
	reportData.audioTime = audioTime;
	reportData.createTime = static_cast<unsigned int>(m_pIManager->GetServerValidTime() / 1000);
	m_pIManager->ReportData(reportData);
}

YIMErrorcode YouMeIMMessageManager::StartAudioSpeech(XUINT64* requestID, bool translate)
{
	if (g_pSpeechManager == NULL || m_pIManager == NULL || (m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success && m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Reconnecting))
	{
		return YIMErrorcode_NotLogin;
	}

	YouMe_LOG_Info(__XT("Enter %d"),translate);

	XUINT64 ulSerial = m_pIManager->GetUniqueSerial();
	if (NULL != requestID)
	{
		*requestID = ulSerial;
	}
	YIMErrorcode errorcode = g_pSpeechManager->StartSpeech(ulSerial, translate);
	if (YIMErrorcode_Success == errorcode)
	{
		std::shared_ptr<FileUploadInfo> pUploadInfo(new FileUploadInfo);
		pUploadInfo->msgSerial = ulSerial;
		pUploadInfo->receiverID = __XT("");
		pUploadInfo->fileType = FileType_Audio;
		{
			std::lock_guard<std::mutex> lock(m_uploadInfoLock);
			m_fileUploadMap[ulSerial] = pUploadInfo;
			m_ullAudioMsgSerial = ulSerial;
		}
	}
	else
	{
		YouMe_LOG_Info(__XT("Leave Fail, errorCode:%d"), errorcode);
	}

    return errorcode;
}

YIMErrorcode YouMeIMMessageManager::StopAudioSpeech()
{
    YouMe_LOG_Info(__XT("Enter"));
	if (g_pSpeechManager == NULL || m_pIManager == NULL || (m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success && m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Reconnecting))
	{
		return YIMErrorcode_NotLogin;
	}
	m_ullAudioMsgSerial = 0;    
	return g_pSpeechManager->StopSpeech();
}

YIMErrorcode YouMeIMMessageManager::ConvertAMRToWav(const XCHAR* amrFilePath, const XCHAR* wavFielPath)
{
	if (amrFilePath == NULL || XString(amrFilePath).empty())
	{
		return YIMErrorcode_ParamInvalid;
	}
	XString strSrcFilePath = XString(amrFilePath);
	if (!youmecommon::CXFile::IsFileExist(strSrcFilePath) || SpeechUtil::GetFileFormat(strSrcFilePath) != FileFormat_AMR)
	{
		return YIMErrorcode_PTT_FileNotExist;
	}
	XString strDestFilePath;
	if (wavFielPath != NULL && XString(wavFielPath).length() > 0)
	{
		strDestFilePath = XString(wavFielPath);
		XString strDir = strDestFilePath.substr(0, strDestFilePath.find_last_of(XPreferredSeparator));
		if (!youmecommon::CXFile::make_dir_tree(strDir.c_str()))
		{
			return YIMErrorcode_CreateFileFailed;
		}
	}
	else
	{
		strDestFilePath = strSrcFilePath + __XT(".wav");
	}

	int nFrame = DecodeAMRFileToWAVEFile(strSrcFilePath, strDestFilePath);
	return nFrame > 0 ? YIMErrorcode_Success : YIMErrorcode_UnknowError;
}

YIMErrorcode YouMeIMMessageManager::QueryHistoryMessage(const XCHAR* targetID, YIMChatType chatType, XUINT64 startMessageID, int count, int direction)
{
    YouMe_LOG_Info(__XT("enter"));
	if (m_pIManager == NULL || (m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success && m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Reconnecting))
	{
		return YIMErrorcode_NotLogin;
	}
    
    if (targetID == NULL || XStrLen(targetID) == 0 || (chatType != ChatType_PrivateChat && chatType != ChatType_RoomChat))
	{
        return YIMErrorcode_ParamInvalid;
    }
    
	return m_messageRecorder.QueryHistoryMessage(targetID, startMessageID, count, direction, chatType);
}

YIMErrorcode YouMeIMMessageManager::SwitchMsgTransType(YIMMsgTransType transType)
{
    YouMe_LOG_Info(__XT("msg trans type:%d"),transType);
    if (ORDINARY == transType)
    {
        m_msgTransType = 0;
    }
    else if (CARBON_COPY == transType)
    {
        m_msgTransType = 1;
    }
    else
    {
        return YIMErrorcode_ParamInvalid;
    }
    return YIMErrorcode_Success;
}

void YouMeIMMessageManager::SaveSenderHistoryMessage(XUINT64 messageSerialFromServer, MessageBufferInfo& messageInfo)
{
	if (NULL == m_pIManager)
	{
		return;
	}
	YIMMessageBodyType messageType = MessageBodyType_Unknow;
	if (YOUMEServiceProtocol::CMD_SND_TEXT_MSG == messageInfo.commandType)
	{
		messageType = MessageBodyType_TXT;
	}
	else if (YOUMEServiceProtocol::CMD_SND_VOICE_MSG == messageInfo.commandType)
	{
		messageType = MessageBodyType_Voice;
	}
	else if (YOUMEServiceProtocol::CMD_SND_FILE_MSG == messageInfo.commandType)
	{
		messageType = MessageBodyType_File;
	}
	else if (YOUMEServiceProtocol::CMD_SND_BIN_MSG == messageInfo.commandType)
	{
		messageType = MessageBodyType_CustomMesssage;
	}
	HistoryMessageInfo info;
	info.messageID = messageSerialFromServer;
	info.chatType = (YIMChatType)messageInfo.chatType;
	info.messageType = messageType;
	info.senderID = m_pIManager->GetCurrentUser();
	info.receiverID = UTF8TOXString(messageInfo.receiverID);
	if (MessageBodyType_TXT == messageType)
	{
		info.content = UTF8TOXString(messageInfo.messageBuffer);
	}
	else if (MessageBodyType_CustomMesssage == messageType)
	{
		info.extend.insert(std::map<std::string, std::string>::value_type("CustomContent", messageInfo.messageBuffer));
	}
	else if (MessageBodyType_Voice == messageType || MessageBodyType_File == messageType)
	{
		std::map<std::string, std::string>::const_iterator itr = messageInfo.extend.find("LocalPath");
		if (itr != messageInfo.extend.end())
		{
			info.localPath = UTF8TOXString(itr->second);
		}
	}
	std::map<std::string, std::string>::const_iterator itr = messageInfo.extend.find("SendTime");
	if (itr != messageInfo.extend.end())
	{
        info.createTime = CStringUtilT<char>::str_to_uint64(itr->second) / 1000; 
    }
    std::map<std::string, std::string>::const_iterator itr2 = messageInfo.extend.find("autoid");
    if (itr2 != messageInfo.extend.end())
    {
        YouMe_LOG_Info(__XT("sender server msgId: %llu"),info.messageID);
        info.id =  CStringUtilT<char>::str_to_uint64(itr2->second) ;
        YouMe_LOG_Info(__XT("save order id: %llu"),info.id);
    }
    if (MessageBodyType_TXT == messageType)
    {
        itr = messageInfo.extend.find("AttachParam");
        if (itr != messageInfo.extend.end())
        {
            info.extend.insert(std::map<std::string, std::string>::value_type(itr->first, itr->second));
        }
    }else if (MessageBodyType_Voice == messageType)
	{
		itr = messageInfo.extend.find("AudioText");
		if (itr != messageInfo.extend.end())
		{
			info.content = UTF8TOXString(itr->second);
		}
		itr = messageInfo.extend.find("FileSize");
		if (itr != messageInfo.extend.end())
		{
			info.extend.insert(std::map<std::string, std::string>::value_type(itr->first, itr->second));
		}
		itr = messageInfo.extend.find("Time");
		if (itr != messageInfo.extend.end())
		{
			info.extend.insert(std::map<std::string, std::string>::value_type(itr->first, itr->second));
		}
		itr = messageInfo.extend.find("Param");
		if (itr != messageInfo.extend.end())
		{
			info.extend.insert(std::map<std::string, std::string>::value_type(itr->first, itr->second));
		}
	}
	else if (MessageBodyType_File == messageType)
	{
		itr = messageInfo.extend.find("FileSize");
		if (itr != messageInfo.extend.end())
		{
			info.extend.insert(std::map<std::string, std::string>::value_type(itr->first, itr->second));
		}
		itr = messageInfo.extend.find("FileName");
		if (itr != messageInfo.extend.end())
		{
			info.extend.insert(std::map<std::string, std::string>::value_type(itr->first, itr->second));
		}
		itr = messageInfo.extend.find("FileExtension");
		if (itr != messageInfo.extend.end())
		{
			info.extend.insert(std::map<std::string, std::string>::value_type(itr->first, itr->second));
		}
		itr = messageInfo.extend.find("Param");
		if (itr != messageInfo.extend.end())
		{
			info.extend.insert(std::map<std::string, std::string>::value_type(itr->first, itr->second));
		}
        
        itr = messageInfo.extend.find("FileType");
        if (itr != messageInfo.extend.end())
        {
            info.extend.insert(std::map<std::string, std::string>::value_type(itr->first, itr->second));
        }
	}
	m_messageRecorder.SaveHistoryMessage(info);
}

void YouMeIMMessageManager::SynMsgReadStatus() {
	if (m_pIManager == NULL) {
		return;
	}

	std::vector<unReadRecvId_s> recvIdVec;
	// 从本地数据库里面获取发送端已发送的未读消息的接收者Id
	m_messageRecorder.getRecvId(false, recvIdVec);

	YOUMEServiceProtocol::SynMsgStatusReq req;
	req.set_version(PROTOCOL_VERSION);
	for (std::vector<unReadRecvId_s>::iterator itr = recvIdVec.begin(); itr != recvIdVec.end(); itr++) {
		YOUMEServiceProtocol::RecvType *recv = req.add_recv_list();
		recv->set_recv_id(itr->recvId);
		recv->set_chat_type(itr->chatType);
		if (itr->chatType == 2) {
			recv->set_msg_id(itr->msgId);
		}
	}

	std::map<std::string, std::string> extend;
	extend["SendTime"] = CStringUtilT<char>::to_string(m_pIManager->GetServerValidTime());

	XUINT64 msgSerial = 0;
	std::string strData;
	req.SerializeToString(&strData);
	m_pIManager->SendData(YOUMEServiceProtocol::CMD_SYN_READ_STATUS, strData.c_str(), (int)strData.length(), extend, msgSerial);
}

void YouMeIMMessageManager::OnQueryHistoryMessage(YIMErrorcode errorcode, XString targetID, std::list<std::shared_ptr<HistoryMessageInfo> >& messageRecord, int remain)
{
	if (m_pCallback == NULL)
	{
		return;
	}
	std::list<std::shared_ptr<CYouMeIMMessage> > tempList;
	std::list< std::shared_ptr<IYIMMessage> > messageList;
	for (std::list<std::shared_ptr<HistoryMessageInfo> >::const_iterator itr = messageRecord.begin(); itr != messageRecord.end(); ++itr)
	{
		const HistoryMessageInfo* pMessageRecord = itr->get();
		std::shared_ptr<CYouMeIMMessage> pMessage(new CYouMeIMMessage);
        pMessage->m_ulID = pMessageRecord->id;
		pMessage->m_chatType = pMessageRecord->chatType;
		pMessage->m_strReceiveID = pMessageRecord->receiverID;
		pMessage->m_strSenderid = pMessageRecord->senderID;
		pMessage->m_ulMsgSerial = pMessageRecord->messageID;
		pMessage->m_iCreateTime = static_cast<unsigned int>(pMessageRecord->createTime);
		pMessage->m_bIsRead = pMessageRecord->isRead;
		std::map<std::string, std::string>::const_iterator itrDistance = pMessageRecord->extend.find("Distance");
		if (itrDistance != pMessageRecord->extend.end())
		{
			pMessage->m_iDistance = CStringUtilT<char>::str_to_uint32(itrDistance->second);
		}

		switch (pMessageRecord->messageType)
		{
		case MessageBodyType_TXT:
		{
			CYouMeIMMessageBodyText* pMsgBody = new CYouMeIMMessageBodyText;
			pMsgBody->m_messageType = pMessageRecord->messageType;
			pMsgBody->SetMessageContent(pMessageRecord->content);
            std::map<std::string, std::string>::const_iterator itr = pMessageRecord->extend.find("AttachParam");
            if (itr != pMessageRecord->extend.end())
            {
                pMsgBody->m_strAttachParam = UTF8TOXString(itr->second);
            }
            
			pMessage->SetMessageBody(pMsgBody);
		}
		break;
		case MessageBodyType_Voice:
		{
			CYouMeIMMessageBodyAudio* pMsgBody = new CYouMeIMMessageBodyAudio;
			pMsgBody->m_messageType = pMessageRecord->messageType;
			pMsgBody->m_strText = pMessageRecord->content;
			pMsgBody->m_strLocalPath = pMessageRecord->localPath;
			pMsgBody->m_isPlayed = pMessageRecord->isPlayed;
            
            std::map<std::string, std::string>::const_iterator itr = pMessageRecord->extend.find("FileSize");
			if (itr != pMessageRecord->extend.end())
			{
				pMsgBody->m_iFileSize = CStringUtilT<char>::str_to_uint32(itr->second);
			}
			itr = pMessageRecord->extend.find("Time");
			if (itr != pMessageRecord->extend.end())
			{
				pMsgBody->m_iTime = CStringUtilT<char>::str_to_uint32(itr->second);
			}
			itr = pMessageRecord->extend.find("Param");
			if (itr != pMessageRecord->extend.end())
			{
				pMsgBody->m_strParam = UTF8TOXString(itr->second);
			}
			pMessage->SetMessageBody(pMsgBody);

			// 未下载的语音消息
			if (XStrncmp(pMsgBody->m_strLocalPath.c_str(), __XT("http"), 4) == 0)
			{
				pMessage->m_ulMsgSerial = pMessageRecord->messageID;	//原采用自增id，

				FileDownloadInfo downloadInfo;
				downloadInfo.downloadURL = pMessageRecord->localPath;
				downloadInfo.msgSerial = pMessage->m_ulMsgSerial;
				downloadInfo.fileType = FileType_Audio;
				downloadInfo.extend["MessageID"] = CStringUtilT<char>::to_string(pMessageRecord->messageID);	//下载完成后更新localpath
				downloadInfo.msg = pMessage;
				{
					std::lock_guard<std::mutex> lock(m_DownloadInfoLock);
					m_downloadInfo[pMessage->m_ulMsgSerial] = downloadInfo;
				}
			}
		}
		break;
		case MessageBodyType_File:
		{
			CYouMeIMMessageBodyFile* pMsgBody = new CYouMeIMMessageBodyFile;
			pMsgBody->m_messageType = pMessageRecord->messageType;
			pMsgBody->m_strLocalPath = pMessageRecord->localPath;
			std::map<std::string, std::string>::const_iterator itr = pMessageRecord->extend.find("FileSize");
			if (itr != pMessageRecord->extend.end())
			{
				pMsgBody->m_iFileSize = CStringUtilT<char>::str_to_uint32(itr->second);
			}
			itr = pMessageRecord->extend.find("FileName");
			if (itr != pMessageRecord->extend.end())
			{
				pMsgBody->m_strFileName = UTF8TOXString(itr->second);
			}
			itr = pMessageRecord->extend.find("FileExtension");
			if (itr != pMessageRecord->extend.end())
			{
				pMsgBody->m_strExtension = UTF8TOXString(itr->second);
			}
			itr = pMessageRecord->extend.find("Param");
			if (itr != pMessageRecord->extend.end())
			{
				pMsgBody->m_strParam = UTF8TOXString(itr->second);
			}
            itr = pMessageRecord->extend.find("FileType");
            if (itr != pMessageRecord->extend.end())
            {
                pMsgBody->m_fileType = (YIMFileType)CStringUtilT<char>::str_to_uint32(itr->second);
            }
			pMessage->SetMessageBody(pMsgBody);
		}
		break;
		case MessageBodyType_CustomMesssage:
		{
			CYouMeIMMessageBodyCustom* pMsgBody = new CYouMeIMMessageBodyCustom;
			pMsgBody->m_messageType = pMessageRecord->messageType;
			std::map<std::string, std::string>::const_iterator itr = pMessageRecord->extend.find("CustomContent");
			if (itr != pMessageRecord->extend.end())
			{
				pMsgBody->SetCustomMessage(itr->second);
			}
			pMessage->SetMessageBody(pMsgBody);
		}
		break;
		/*case MessageBodyType_Gift:
		{
			CYouMeIMMessageBodyGift* pMsgBody = new CYouMeIMMessageBodyGift;
			pMsgBody->m_messageType = MessageBodyType_Gift;
			for (std::map<std::string, std::string>::const_iterator itr = pMessageRecord->extend.begin(); itr != pMessageRecord->extend.end(); ++itr)
			{
				if (itr->first == "id")
				{
					pMsgBody->m_iGiftID = CStringUtilT<char>::str_to_sint32(itr->second);
					
				}
				else if (itr->first == "count")
				{
					pMsgBody->m_iCount = CStringUtilT<char>::str_to_uint32(itr->second);
				}
				else
				{
					pMsgBody->m_extend.insert(std::map<std::string, std::string>::value_type(itr->first, itr->second));
				}
			}
			pMessage->SetMessageBody(pMsgBody);
		}
		break;*/
		default:
			continue;
		}
		tempList.push_back(pMessage);
        messageList.push_back( pMessage );
	}

	m_pCallback->OnQueryHistoryMessage(errorcode, targetID.c_str(), remain, messageList);
}

YIMErrorcode YouMeIMMessageManager::DeleteHistoryMessage(YIMChatType chatType, XUINT64 time)
{
    YouMe_LOG_Info(__XT("DeleteHistoryMessage chatType:%d time:%lld"), chatType, time);
	if (m_pIManager == NULL || (m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success && m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Reconnecting))
	{
		return YIMErrorcode_NotLogin;
	}
	return m_messageRecorder.DeleteHistoryMessage(chatType, 0, time);
}

YIMErrorcode YouMeIMMessageManager::DeleteHistoryMessage(const XCHAR* targetID, YIMChatType chatType, XUINT64 startMessageID, unsigned int count)
{	
    if (targetID == NULL || XStrLen(targetID) == 0 || (chatType != ChatType_PrivateChat && chatType != ChatType_RoomChat))
    {
        return YIMErrorcode_ParamInvalid;
    }
    YouMe_LOG_Info(__XT("DeleteHistoryMessage targetID:%s chatType:%d startMessageID:%lld count:%d"),targetID, chatType, startMessageID, count);
	if (m_pIManager == NULL || (m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success && m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Reconnecting))
	{
		return YIMErrorcode_NotLogin;
	}
	return m_messageRecorder.DeleteHistoryMessage(targetID, chatType, startMessageID, count);
}

YIMErrorcode YouMeIMMessageManager::DeleteHistoryMessageByID(XUINT64 messageID)
{
    YouMe_LOG_Info(__XT("DeleteHistoryMessageByID messageID:%lld"),messageID);
	if ( m_pIManager == NULL || (m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success && m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Reconnecting))
	{
		return YIMErrorcode_NotLogin;
	}
	return m_messageRecorder.DeleteHistoryMessage(ChatType_Unknow, messageID, 0);
}

YIMErrorcode YouMeIMMessageManager::DeleteSpecifiedHistoryMessage(const XCHAR* targetID, YIMChatType chatType, const std::vector<XUINT64>& excludeMesList)
{
    YouMe_LOG_Info(__XT("DeleteSpecifiedHistoryMessage targetID:%s chatType:%d"),targetID, chatType);
    if (targetID == NULL || XStrLen(targetID) == 0 || (chatType != ChatType_PrivateChat && chatType != ChatType_RoomChat))
    {
        return YIMErrorcode_ParamInvalid;
    }
    
    if (g_pSpeechManager == NULL || m_pIManager == NULL || (m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success && m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Reconnecting))
    {
        return YIMErrorcode_NotLogin;
    }
    
    return m_messageRecorder.DeleteSpecifiedHistoryMessage(targetID, chatType, excludeMesList);
}

YIMErrorcode YouMeIMMessageManager::SetReceiveMessageSwitch(const std::vector<XString>& vecRoomIDs, bool receive)
{
	if (vecRoomIDs.empty())
	{
		return YIMErrorcode_ParamInvalid;
	}

	XString rooms;
	for (std::vector<XString>::const_iterator itr = vecRoomIDs.begin(); itr != vecRoomIDs.end(); ++itr)
	{
		if (receive)
		{
			std::set<XString>::iterator itrShield = m_msgShieldTargets.find(*itr);
			if (itrShield != m_msgShieldTargets.end())
			{
				m_msgShieldTargets.erase(itrShield);
			}
		}
		else
		{
			m_msgShieldTargets.insert(*itr);
		}
		rooms += *itr + __XT(" ");
	}
	YouMe_LOG_Debug(__XT("SetReceiveMessageSwitch %d %s"), receive, rooms.c_str());

	return YIMErrorcode_Success;
}

YIMErrorcode YouMeIMMessageManager::SetRoomHistoryMessageSwitch(const std::vector<XString>& vecRoomIDs, bool save)
{
	if ( vecRoomIDs.empty() )
	{
		return YIMErrorcode_ParamInvalid;
	}

	for (unsigned int i = 0; i < vecRoomIDs.size(); ++i)
	{
        XString room = vecRoomIDs[i];
        YouMe_LOG_Debug(__XT("SetRoomHistoryMessageSwitch room:%s save:%d"), room.c_str(), save);
		if (save)
		{
			m_msgRecordRooms.insert(room);
		}
		else
		{
			std::set<XString>::iterator itr = m_msgRecordRooms.find(room);
			if (itr != m_msgRecordRooms.end())
			{
				m_msgRecordRooms.erase(itr);
			}
		}
	}
	return YIMErrorcode_Success;
}

YIMErrorcode YouMeIMMessageManager::SetRoomHistoryMessageSwitch(const XCHAR* roomID, bool save)
{
	if (NULL == roomID)
	{
		return YIMErrorcode_ParamInvalid;
	}

    YouMe_LOG_Debug(__XT("SetRoomHistoryMessageSwitch room:%s save:%d"), roomID, save);
	XString room(roomID);
	if (save)
	{
		m_msgRecordRooms.insert(room);
	}
	else
	{
		std::set<XString>::iterator itr = m_msgRecordRooms.find(room);
		if (itr != m_msgRecordRooms.end())
		{
			m_msgRecordRooms.erase(itr);
		}
	}

	return YIMErrorcode_Success;
}

YIMErrorcode YouMeIMMessageManager::SetMessageRead(XUINT64 messageID, bool read)
{
    YouMe_LOG_Debug(__XT("SetMessageRead room:%lld read:%d"), messageID, read);
	return m_messageRecorder.SetMessageRead(messageID, read);
}

// 设置某用户的所有消息为已读
YIMErrorcode YouMeIMMessageManager::SetAllMessageRead(const XCHAR* userID, bool read)
{
    if (NULL == userID)
    {
        return YIMErrorcode_ParamInvalid;
    }
    YouMe_LOG_Debug(__XT("SetAllMessageRead userID:%s read:%d"), userID, read);
    return m_messageRecorder.SetAllMessageRead(userID,read);
}

// 通知对端所有消息为已读
YIMErrorcode YouMeIMMessageManager::SendMessageReadStatus(const XCHAR* userID, int chatType, XUINT64 messageID)
{
	if (m_pIManager == NULL || (m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success && m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Reconnecting))
	{
		return YIMErrorcode_NotLogin;
	}
    
	if (NULL == userID || XStrLen(userID) == 0)
	{
		return YIMErrorcode_ParamInvalid;
	}

    YouMe_LOG_Debug(__XT("sendMessageReadStatus userID:%s chatType:%d, messageID:%llu"), userID, chatType, messageID);

	XUINT64 msgSerial = 0;
	MessageBufferInfo messsageInfo;
	messsageInfo.commandType = YOUMEServiceProtocol::CMD_SND_READ_STATUS;
	messsageInfo.chatType = chatType;
	messsageInfo.receiverID = XStringToUTF8(XString(userID));
    messsageInfo.extend["SendTime"] = CStringUtilT<char>::to_string(m_pIManager->GetServerValidTime());
	messsageInfo.extend["msg_id"] = CStringUtilT<char>::to_string(messageID);
	bool bRet = SendMessage(messsageInfo, msgSerial);
	return bRet ? YIMErrorcode_Success : YIMErrorcode_UnknowError;
}


// 设置语音消息为已播放,true-已播放，false-未播放
YIMErrorcode YouMeIMMessageManager::SetVoiceMsgPlayed(XUINT64 messageID, bool played)
{
    YouMe_LOG_Debug(__XT("SetVoiceMsgPlayed room:%lld played:%d"), messageID, played);
    return m_messageRecorder.SetVoiceMsgPlayed(messageID,played);
}

YIMErrorcode YouMeIMMessageManager::PauseReceiveMessage(bool pause)
{
    YouMe_LOG_Debug(__XT("PauseReceiveMessage pause:%d"),pause);
    m_pauseReceivingFlag = pause;
    return YIMErrorcode_Success;
}

YIMErrorcode YouMeIMMessageManager::TranslateText(unsigned int* requestID, const XCHAR* text, LanguageCode destLangCode, LanguageCode srcLangCode)
{
    YouMe_LOG_Debug(__XT("enter TranslateText"));
	if (text == NULL || XString(text).empty())
	{
		return YIMErrorcode_ParamInvalid;
	}
	if (m_pIManager == NULL)
	{
		return YIMErrorcode_EngineNotInit;
	}
    if (!m_bInit)
    {
        return YIMErrorcode_StatusError;
    }
	int nTranslateSwitch = m_pIManager->GetConfigure<unsigned int>(CONFIG_TRANSLATE_ENABLE, TRANSLATE_SWITCH_DEFAULT);
	if (!nTranslateSwitch)
	{
		return YIMErrorcode_TranslateUnable;
	}

	std::map<LanguageCode, XString>::const_iterator itr = m_langCodeMap.find(destLangCode);
	if (itr == m_langCodeMap.end())
	{
		return YIMErrorcode_NoLangCode;
	}
    
     YouMe_LOG_Debug(__XT("start TranslateText"));
    
	TranslateInfo info;
	info.srcLangCode = srcLangCode;
	info.destLangCode = destLangCode;
	info.text = text;
	info.requestID = ++m_iTranslateRequestID;
	if (requestID != NULL)
	{
		*requestID = info.requestID;
	}
	{
		std::lock_guard<std::mutex> lock(m_translateLock);
		m_translateInfoList.push_back(info);
	}
	m_translateSemaphore.Increment();

	return YIMErrorcode_Success;
}

void YouMeIMMessageManager::TranslateThread()
{
    YouMe_LOG_Debug(__XT("enter"));
	while (m_translateSemaphore.Decrement())
	{
		if (m_bTranslateThreadExit)
		{
			break;
		}

		TranslateInfo info;
		{
			std::lock_guard<std::mutex> lock(m_translateLock);
			info = m_translateInfoList.front();
			m_translateInfoList.pop_front();
		}
		std::map<LanguageCode, XString>::const_iterator itrDestLang = m_langCodeMap.find(info.destLangCode);
		std::map<LanguageCode, XString>::const_iterator itrSrcLang = m_langCodeMap.find(info.srcLangCode);
		if (itrDestLang == m_langCodeMap.end() || itrSrcLang == m_langCodeMap.end())
		{
			continue;
		}
		XString strSrcLanguage;
        int nTranslateMethod = m_pIManager->GetConfigure<unsigned int>(CONFIG_TRANSLATE_METHOD, TRANSLATE_METHOD_DEFAULT);
        XString strResult;
        if (!nTranslateMethod)
        {
            strResult = g_pTranslateUtil->Translate(info.text, itrSrcLang->second, itrDestLang->second, strSrcLanguage);
        }
        else
        {
            strResult = g_pTranslateUtil->TranslateV2(info.text, itrSrcLang->second, itrDestLang->second, strSrcLanguage);
        }
		
		if (m_pCallback)
		{
			std::transform(strSrcLanguage.begin(), strSrcLanguage.end(), strSrcLanguage.begin(), ::tolower);
			LanguageCode srcLanguageCode = info.srcLangCode;
			if (srcLanguageCode == LANG_AUTO)
			{
				for (std::map<LanguageCode, XString>::const_iterator itr = m_langCodeMap.begin(); itr != m_langCodeMap.end(); ++itr)
				{
					if (itr->second == strSrcLanguage)
					{
						srcLanguageCode = itr->first;
						break;
					}
				}
			}
			m_pCallback->OnTranslateTextComplete(strResult.empty() ? YIMErrorcode_NetError : YIMErrorcode_Success, info.requestID, strResult.c_str(), srcLanguageCode, info.destLangCode);
		}
        
        // count character
        XString strNewWord = CStringUtil::replace_text(info.text, __XT("\r"), __XT(""));
        
        std::vector<unsigned long long> u16_cvt_str;
#ifdef WIN32
        for (int i = 0; i < strNewWord.size(); i++)
        {
            u16_cvt_str.push_back(strNewWord.at(i));
        }
#else
        youmecommon::utf8to16(strNewWord.begin(), strNewWord.end(), std::back_inserter(u16_cvt_str));
#endif // WIN32

        XUINT64 chaterCount = 0;
        if (strResult == __XT(""))
        {
            chaterCount = 1;
        }
        else
        {
            chaterCount = (XUINT64)u16_cvt_str.size();
        }
        
        ReportTranslateStatus(strResult.empty() ? REPORTCODE_TRANSLATE_FAILED : REPORTCODE_SUCCESS, itrSrcLang->second, itrDestLang->second, chaterCount, nTranslateMethod);
	}
     YouMe_LOG_Debug(__XT("leave"));
}

void YouMeIMMessageManager::ReportTranslateStatus(short status, const XString& srcLanguage, const XString& destLanguage, XUINT64 chaterCount, int translateMethod)
{
    if (m_pIManager == NULL)
    {
        return;
    }
    ReportTranslateInfo reportData;
    reportData.userid = XStringToUTF8(m_pIManager->GetCurrentUser());
    reportData.appid = m_pIManager->GetAppID();
    reportData.SDKVersion = SDK_VER;
    reportData.status = status;
    reportData.srcLanguage = XStringToUTF8(srcLanguage);
    reportData.destLanguage = XStringToUTF8(destLanguage);
    reportData.characterCount = chaterCount;
    reportData.translateVersion = translateMethod;
    
    m_pIManager->ReportData(reportData);
}


void YouMeIMMessageManager::ReportDataMessageTimeCost(unsigned int timeCost, XUINT64 sendSerial)
{
	if (m_pIManager == NULL)
	{
		return;
	}
	ReportMessageTimeCost reportData;
	reportData.userid = XStringToUTF8(m_pIManager->GetCurrentUser());
	reportData.appid = m_pIManager->GetAppID();
	reportData.SDKVersion = SDK_VER;
	reportData.costTime = timeCost;
	reportData.sendSerial = sendSerial;
	m_pIManager->ReportData(reportData);
}

YIMErrorcode YouMeIMMessageManager::SetSpeechRecognizeLanguage(SpeechLanguage language)
{
	if (g_pSpeechManager == NULL)
	{
		return YIMErrorcode_EngineNotInit;
	}
	return g_pSpeechManager->SetSpeechRecognizeLanguage(language);
}

//设置仅识别语音文字，不发送语音消息，默认为false:识别语音文字并发送语音消息，true:仅识别语音文字
YIMErrorcode YouMeIMMessageManager::SetOnlyRecognizeSpeechText(bool recognition)
{
    m_onlyRecognizeSpeechText = recognition;
    YouMe_LOG_Info(__XT("set only recognize speech text:%d"), recognition);
    
    return YIMErrorcode_Success;
}

YIMErrorcode YouMeIMMessageManager::Accusation(const XCHAR* userID, YIMChatType source, int reason, const XCHAR* description, const XCHAR* extraParam)
{
    YouMe_LOG_Warning(__XT("enter Accusation"));
	if (NULL == userID || XString(userID).empty())
	{
		return YIMErrorcode_ParamInvalid;
	}
	if (m_pIManager == NULL)
	{
		return YIMErrorcode_EngineNotInit;
	}
	if (description != NULL && XString(description).length() > 360)
	{
		YouMe_LOG_Warning(__XT("accusation description too long"));
		return YIMErrorcode_ParamInvalid;
	}
	std::string strDescription;
	if (description != NULL)
	{
		strDescription = XStringToUTF8(XString(description));
	}
	youmecommon::Value val;
	val["Reason"] = CStringUtil::to_string(reason);
	val["Description"] = strDescription;
	std::string strContent = val.toStyledString();

	YOUMEServiceProtocol::SendMsgReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_chat_type(source);
	req.set_recv_id(XStringToUTF8(XString(userID)).c_str());
	req.set_msg_content(strContent);
	if (extraParam != NULL)
	{
		std::string strExtraParam = XStringToUTF8(XString(extraParam));
		youmecommon::Value value;
		youmecommon::Reader jsonReader;
		if (jsonReader.parse(strExtraParam, value))
		{
			std::vector<std::string> names = value.getMemberNames();
			for (std::vector<std::string>::const_iterator itr = names.begin(); itr != names.end(); ++itr)
			{
				YOUMECommonProtocol::NameValue* pParam = req.mutable_comment()->add_comment();
				if (pParam != NULL)
				{
					pParam->set_name(*itr);
					pParam->set_value(value[*itr].toStyledString());
					pParam->set_value_type(YOUMECommonProtocol::VALUE_STRING);
				}
			}
		}
	}
	
	std::string strData;
	req.SerializeToString(&strData);
	std::map<std::string, std::string> extend;
	XUINT64 msgSerial = 0;
	if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_SND_TIPOFF_MSG, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		return YIMErrorcode_NetError;
	}
	return YIMErrorcode_Success;
}

void YouMeIMMessageManager::OnAccusationRsp(ServerPacket& serverPacket)
{
	if (serverPacket.result == -1)
	{
		YouMe_LOG_Error(__XT("accusation timeout"));
		return;
	}
	YOUMEServiceProtocol::SendMsgRsp rsp;
	if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
	{
		YouMe_LOG_Error(__XT("accusation response unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
		return;
	}
	int nRet = rsp.ret();
	if (nRet != 0)
	{
		YouMe_LOG_Error(__XT("accusation error(%d)"), nRet);
		return;
	}
}

void YouMeIMMessageManager::OnAccusationDealNotify()
{
	if (m_pIManager == NULL)
	{
		return;
	}
	YOUMEServiceProtocol::GetMsgReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_msgid(m_pIManager->GetMaxAccusationID());
	std::string strData;
	req.SerializeToString(&strData);
	XUINT64 msgSerial = 0;
	std::map<std::string, std::string> extend;
	if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_GET_TIPOFF_MSG, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		YouMe_LOG_Error(__XT("send get accusation deal result failed"));
	}
	YouMe_LOG_Debug(__XT("Accusation notify accusationID:%llu"), req.msgid());
}

void YouMeIMMessageManager::OnGetAccusationDealResultRsp(ServerPacket& serverPacket)
{
	if (NULL == m_pCallback)
	{
		YouMe_LOG_Warning(__XT("callback is null"));
		return;
	}
	YOUMEServiceProtocol::GetMsgRsp rsp;
	if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
	{
		YouMe_LOG_Error(__XT("get accusation deal result unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
		return;
	}
	if (rsp.ret() != 0)
	{
		YouMe_LOG_Error(__XT("get accusation deal result error(%d)"), rsp.ret());
		return;
	}

	XUINT64 maxAccusationID = 0;
	XUINT64 ullCurMaxAccusationID = m_pIManager->GetMaxAccusationID();
	for (int i = 0; i < rsp.msg_list_size(); ++i)
	{
		YOUMEServiceProtocol::YoumeMsg info = rsp.msg_list(i);
		AccusationDealResult result = (AccusationDealResult)info.msg_type();
		XString userID = UTF8TOXString(info.recver_id());
		unsigned int createTime = info.create_time();

		if (info.msgid() > ullCurMaxAccusationID)
		{
			maxAccusationID = info.msgid();
			m_pCallback->OnAccusationResultNotify(result, userID.c_str(), createTime);
		}
		YouMe_LOG_Debug(__XT("accusation deal result:%d userID:%s time:%u"), result, userID.c_str(), createTime);
	}

	if (maxAccusationID > ullCurMaxAccusationID && g_pProfileDB != NULL &&  m_pIManager != NULL)
	{
		m_pIManager->SetMaxAccusationID(maxAccusationID);

		XString strMaxAccusationID = CStringUtilT<XCHAR>::to_string(maxAccusationID);
		std::string strKey = CStringUtilT<char>::formatString("MaxAccusationID_%d_%s_%d", m_pIManager->GetAppID(), XStringToUTF8(m_pIManager->GetCurrentUser()).c_str(), g_iYouMeIMMode);
		bool bRet = g_pProfileDB->setSetting(strKey, strMaxAccusationID);
		if (!bRet)
		{
			YouMe_LOG_Warning(__XT("save MaxAccusationID failed"));
		}
	}
}

#define ForbidRecord_Num_Limit 20

YIMErrorcode YouMeIMMessageManager::GetForbiddenSpeakInfo() {
    if (m_pIManager == NULL)
    {
        return YIMErrorcode_EngineNotInit;
    }
    
    YOUMEServiceProtocol::ForbidRecordReq req;
    req.set_version(PROTOCOL_VERSION);

    req.set_user_right(0);
    req.set_room_seq(0);
    req.set_limit( ForbidRecord_Num_Limit );
  
    
    std::string strData;
    req.SerializeToString(&strData);
    std::map<std::string, std::string> extend;
    XUINT64 msgSerial = 0;
    if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_GET_FORBID_RECORD, strData.c_str(), (int)strData.length(), extend, msgSerial))
    {
        return YIMErrorcode_NetError;
    }
    return YIMErrorcode_Success;
}

YIMErrorcode YouMeIMMessageManager::BlockUser(const XCHAR* userID, bool block)
{
	if (NULL == userID)
	{
		return YIMErrorcode_ParamInvalid;
	}
	XString strUserID = XString(userID);
	if (strUserID.empty())
	{
		return YIMErrorcode_ParamInvalid;
	}
	if (block && m_blockUserList.count(strUserID) > 0)
	{
		YouMe_LOG_Warning(__XT("%s already bolock"), strUserID.c_str());
		return YIMErrorcode_AlreadyBlock;
	}
	if (!block && m_blockUserList.count(strUserID) == 0)
	{
		YouMe_LOG_Warning(__XT("%s already bolock"), strUserID.c_str());
		return YIMErrorcode_NotBlock;
	}

	std::string user = XStringToUTF8(strUserID);
	YOUMEServiceProtocol::SetMaskMsgUserReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_room_id("");
	req.set_mask_right(block);
	req.add_mask_user_list(user.c_str());
	std::string strData;
	req.SerializeToString(&strData);
	std::map<std::string, std::string> extend;
	extend["UserID"] = user;
	extend["Block"] = CStringUtilT<char>::to_string((int)block);
	XUINT64 msgSerial = 0;
	if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_SET_MASK_USER_MSG, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		YouMe_LOG_Error(__XT("request blockuser failed"));
		return YIMErrorcode_NetError;
	}
	YouMe_LOG_Info(__XT("bolckuser:%s block:%d"), strUserID.c_str(), block);

	return YIMErrorcode_Success;
}

void YouMeIMMessageManager::OnBlockUserRsp(ServerPacket& serverPacket)
{
	if (NULL == m_pCallback)
	{
		YouMe_LOG_Warning(__XT("callback is null"));
		return;
	}
	std::map<std::string, std::string>::const_iterator itrUserID = serverPacket.extend.find("UserID");
	std::map<std::string, std::string>::const_iterator itrBlock = serverPacket.extend.find("Block");
	if (itrUserID == serverPacket.extend.end() || itrBlock == serverPacket.extend.end())
	{
		YouMe_LOG_Error(__XT("user not found"));
		return;
	}
	XString strUserID = UTF8TOXString(itrUserID->second);
	bool block = CStringUtilT<char>::str_to_uint32(itrBlock->second);
	
	YIMErrorcode errorcode = YIMErrorcode_Success;
	if (serverPacket.result == -1)
	{
		YouMe_LOG_Error(__XT("block user timeout serial:%llu"), serverPacket.reqSerial);
		errorcode = YIMErrorcode_TimeOut;
	}
	else
	{
		YOUMEServiceProtocol::SetMaskMsgUserRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
			return;
		}
		if (rsp.ret() == 0)
		{
			if (block)
			{
				m_blockUserList.insert(strUserID);
			}
			else
			{
				m_blockUserList.erase(strUserID);
			}
		}
		else
		{
			YouMe_LOG_Error(__XT("block user error(%d)"), rsp.ret());
			errorcode = YIMErrorcode_ServerError;
		}
	}

	m_pCallback->OnBlockUser(errorcode, strUserID.c_str(), block);
}

YIMErrorcode YouMeIMMessageManager::GetBlockUsers()
{
	RequestBlockUserList(0);
	return YIMErrorcode_Success;
}

void YouMeIMMessageManager::RequestBlockUserList(XUINT64 serialNo, bool internalCall)
{
	YOUMEServiceProtocol::GetMaskMsgUserReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_room_id("@_private");
	req.set_mask_seq(serialNo);
	req.set_number_limit(REFUSE_LIST_PAGE_SIZE);
	req.set_mask_right(1);
	std::string strData;
	req.SerializeToString(&strData);
	std::map<std::string, std::string> extend;
	if (internalCall)
	{
		extend["InternalCall"] = "1";
	}
	XUINT64 msgSerial = 0;
	if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_GET_MASK_USER_MSG, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		YouMe_LOG_Error(__XT("request block users failed"));
	}
}

void YouMeIMMessageManager::OnGetBlockUserListRsp(ServerPacket& serverPacket)
{
	if (m_bRequestBlockList)
	{
		if (m_pCallback != NULL)
		{
			std::list<XString> users;
			users.insert(users.end(), m_blockUserList.begin(), m_blockUserList.end());
			m_pCallback->OnGetBlockUsers(YIMErrorcode_Success, users);
		}
		return;
	}

	YIMErrorcode errorcode = YIMErrorcode_Success;
	bool internalCall = false;
	YOUMEServiceProtocol::GetMaskMsgUserRsp rsp;
	do 
	{
		if (serverPacket.result == -1)
		{
			YouMe_LOG_Error(__XT("get block list timeout serial:"), serverPacket.reqSerial);
			errorcode = YIMErrorcode_TimeOut;
			break;
		}
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
			errorcode = YIMErrorcode_ServerError;
			break;
		}
		if (rsp.ret() != 0)
		{
			YouMe_LOG_Error(__XT("refuse message error(%d)"), rsp.ret());
			errorcode = YIMErrorcode_ServerError;
			break;
		}
		for (int i = 0; i < rsp.mask_user_list_size(); ++i)
		{
			std::string strName = rsp.mask_user_list(i);
			m_blockUserList.insert(UTF8TOXString(strName));
		}
		std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("InternalCall");
		if (itr != serverPacket.extend.end())
		{
			internalCall = true;
		}
		if (rsp.mask_user_list_size() > 0)
		{
			RequestBlockUserList(rsp.mask_seq(), internalCall);
			return;
		}
	} while (0);
	
	m_bRequestBlockList = true;
	m_strLastUserID = m_pIManager->GetCurrentUser();

	if (!internalCall && m_pCallback != NULL)
	{
		std::list<XString> users;
		users.insert(users.end(), m_blockUserList.begin(), m_blockUserList.end());
		m_pCallback->OnGetBlockUsers(errorcode, users);
	}
}

YIMErrorcode YouMeIMMessageManager::UnBlockAllUser()
{
	if (m_blockUserList.size() == 0)
	{
		return YIMErrorcode_NotBlock;
	}

	YOUMEServiceProtocol::CleanMaskMsgUserReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_room_id("");
	std::string strData;
	req.SerializeToString(&strData);
	std::map<std::string, std::string> extend;
	XUINT64 msgSerial = 0;
	if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_CLEAN_MASK_USER_MSG, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		YouMe_LOG_Error(__XT("send block user failed"));
		return YIMErrorcode_NetError;
	}
	return YIMErrorcode_Success;
}

void YouMeIMMessageManager::OnUnBlockAllUserRsp(ServerPacket& serverPacket)
{
	YIMErrorcode errorcode = YIMErrorcode_Success;
	if (serverPacket.result == -1)
	{
		YouMe_LOG_Error(__XT("get refuse list timeout serial:"), serverPacket.reqSerial);
		errorcode = YIMErrorcode_TimeOut;
	}
	else
	{
		YOUMEServiceProtocol::CleanMaskMsgUserRsp rsp;
		if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
		{
			YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
			errorcode = YIMErrorcode_ServerError;
		}
		if (rsp.ret() == 0)
		{
			m_blockUserList.clear();
		}
		else
		{
			YouMe_LOG_Error(__XT("block all user error(%d)"), rsp.ret());
			errorcode = YIMErrorcode_ServerError;
		}
	}
	if (m_pCallback != NULL)
	{
		m_pCallback->OnUnBlockAllUser(errorcode);
	}
}

void YouMeIMMessageManager::OnGetForbiddenSpeakInfo(ServerPacket& serverPacket){
    if (serverPacket.result == -1)
    {
        YouMe_LOG_Error(__XT("OnGetForbiddenSpeakInfo timeout"));
        return;
    }
    YOUMEServiceProtocol::ForbidRecordRsp rsp;
    if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
    {
        YouMe_LOG_Error(__XT("ForbiddenSpeak response unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
        return;
    }
    
    if( m_pCallback == NULL ){
        return;
    }
    
    
    std::vector<std::shared_ptr<IYIMForbidSpeakInfo>> vecForbiddenSpeakInfos;
    
    if( rsp.ret() != 0 ){
        m_pCallback->OnGetForbiddenSpeakInfo( YIMErrorcode_Fail, vecForbiddenSpeakInfos );
        YouMe_LOG_Error(__XT("ForbiddenSpeak response error ret:%d"), rsp.ret() );
        return ;
    }
    
    int max_seq = 0;    //下一次请求的起始位置
    //给出回调
    for( int i = 0 ; i < rsp.record_list_size(); i++ ){
        YOUMEServiceProtocol::YoumeForbidRecord record = rsp.record_list( i );
        
        CForbiddenSpeakInfo * pInfo = new CForbiddenSpeakInfo();
        
        pInfo->m_channelID = UTF8TOXString(record.roomid());
        pInfo->m_reasonType  = record.reason_type();
        pInfo->m_endTime = record.expire_time();
        pInfo->m_isForbidRoom = record.forbid_room();
        
        if( i == rsp.record_list_size() - 1 ){
            max_seq = record.room_seq();
        }
        
        std::shared_ptr<IYIMForbidSpeakInfo> pAutoInfo( pInfo );

        vecForbiddenSpeakInfos.push_back( pAutoInfo );
    }
    
    if( rsp.record_list_size() != 0 && m_pCallback != NULL ){
        m_pCallback->OnGetForbiddenSpeakInfo( YIMErrorcode_Success, vecForbiddenSpeakInfos );
    }
    
    //拿到的条数小于限制，说明取完了，给出结束回调
    if( rsp.record_list_size() <  ForbidRecord_Num_Limit ){
        std::vector<std::shared_ptr<IYIMForbidSpeakInfo>> vecEmpty;
        if( m_pCallback != NULL ){
            m_pCallback->OnGetForbiddenSpeakInfo( YIMErrorcode_Success, vecEmpty );
        }
    }
    //如果没完，就需要继续请求
    else{
        YOUMEServiceProtocol::ForbidRecordReq req;
        req.set_version(PROTOCOL_VERSION);

        req.set_user_right(0);
        req.set_room_seq( max_seq );
        req.set_limit( ForbidRecord_Num_Limit );
        
        
        std::string strData;
        req.SerializeToString(&strData);
        std::map<std::string, std::string> extend;
        XUINT64 msgSerial = 0;
        if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_GET_FORBID_RECORD, strData.c_str(), (int)strData.length(), extend, msgSerial))
        {
            std::vector<std::shared_ptr<IYIMForbidSpeakInfo>> vecEmpty;
            m_pCallback->OnGetForbiddenSpeakInfo( YIMErrorcode_Fail, vecForbiddenSpeakInfos );
            YouMe_LOG_Error(__XT("ForbiddenSpeak send data fail "));
        }
    }
}

YIMErrorcode YouMeIMMessageManager::RequestWeiXinAccessToken()
{
	YOUMEServiceProtocol::GetWeiXinAccessTokenReq req;
	req.set_version(PROTOCOL_VERSION);
	std::string strData;
	req.SerializeToString(&strData);
	std::map<std::string, std::string> extend;
	XUINT64 msgSerial = 0;
	if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_GET_WEIXIN_TOKEN, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		YouMe_LOG_Error(__XT("request weixin token failed"));
		return YIMErrorcode_NetError;
	}

	return YIMErrorcode_Success;
}

void YouMeIMMessageManager::OnWeiXinAccessTokenRsp(ServerPacket& serverPacket)
{
 	if (serverPacket.result == -1)
	{
		YouMe_LOG_Error(__XT("get weixin tocken timeout. serial:%llu"), serverPacket.reqSerial);
		return;
	}
	YOUMEServiceProtocol::GetWeiXinAccessTokenRsp rsp;
	if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
	{
		YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
		return;
	}
	if (rsp.ret() == 0)
	{
        YouMe_LOG_Info(__XT("get weixin token"));
		m_strWeiXinTocken = rsp.token();
		m_ullWeiXinTokenSerial = rsp.token_seq();
	}
	else
	{
		YouMe_LOG_Error(__XT("get weixin tocken error(%d)"), rsp.ret());
	}
}
