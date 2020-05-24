#include "YouMeSpeechManager.h"
#include <thread>
#include <math.h>
#include <YouMeCommon/Log.h>
#include <YouMeCommon/XFile.h>
#include <YouMeCommon/StringUtil.hpp>
#include <YouMeCommon/XSharedArray.h>
#include <YouMeIMEngine/YouMeIMManager.h>
#include <YouMeIMEngine/Speech/AudioResample.h>
#include <YouMeIMEngine/pb/youme_ali_access_token.pb.h>
#ifdef WIN32
#include "Windows/WinVoiceManager.h"
#elif (OS_IOS || OS_IOSSIMULATOR)
#include "IOS/IOSVoiceManager.h"
#elif OS_OSX
#include "OSX/OSXVoiceManager.h"
#elif OS_ANDROID
#include "Android/AndroidVoiceManager.h"
#endif


#define SPEECH_RECOGNIZE_FAILED_COUNT 3
#define SILENCE_DB 60.0
#ifdef WIN32
#define IFLYTEK_APPID_DEFAULT __XT("5c2584a3")
#elif (OS_IOS || OS_IOSSIMULATOR)
#define IFLYTEK_APPID_DEFAULT __XT("5c2581d7")
#elif OS_ANDROID
#define IFLYTEK_APPID_DEFAULT __XT("5c2581f0")
#endif

#define USC_APPKEY_DEFAULT  __XT("33qgjqh22wxlk2oatbndn3ydxbswaikvtjcmiuyp")
#define USC_SECRET_DEFAULT  __XT("a6028879cf7b83ba258dc4f518e5bfe3")

#define ALI_APPKEY_DEFAULT  __XT("5hQi7z7mdufFPqvg")
#define ALI_SECRET_DEFAULT  __XT("tmMb4C5zctkZw4YVLPS0tzjOsoRguQ")


XString CYouMeSpeechManager::m_strAudioCacheDir;
XUINT64 CYouMeSpeechManager::m_ullAliTokenSerial = 0;


CYouMeSpeechManager::CYouMeSpeechManager() : m_pVoiceManager(NULL)
, m_pIManager(NULL)
, m_pMessageManager(NULL)
, m_pAudioPlayCallback(NULL)
, m_iSpeechRecognizeFailedCount(0)
, m_iSampleRate(SAMPLERATE_16K)
, m_recognizeType(RECOGNIZETYPE_IFLY_STREAM)
, m_speechLanguage(SPEECHLANG_MANDARIN)
, m_pAudioResample(NULL)
{
#ifdef WIN32
	m_pVoiceManager = new WinVoiceManager;
#elif (OS_IOS || OS_IOSSIMULATOR)
    m_pVoiceManager = new IOSVoiceManager;
#elif OS_OSX
    m_pVoiceManager = new OSXVoiceManager;
#elif OS_ANDROID
	m_pVoiceManager = new AndroidVoiceManager;
#endif
}

CYouMeSpeechManager::~CYouMeSpeechManager()
{
	if (m_pAudioResample != NULL)
	{
		delete m_pAudioResample;
		m_pAudioResample = NULL;
	}
    delete m_pVoiceManager;
}

bool CYouMeSpeechManager::Init()
{
	if (NULL != m_pVoiceManager)
	{
		std::string appkey;
		std::string secret;		
#ifdef OS_OSX
        m_recognizeType = RECOGNIZETYPE_NO;
#elif (OS_IOS || OS_IOSSIMULATOR || WIN32 || OS_ANDROID)
        m_recognizeType = (AudioRecognizeType) m_pIManager->GetConfigure(CONFIG_RECOGNIZE_TYPE, 2);        // RECOGNIZETYPE_IFLY_STREAM
#endif
        if (RECOGNIZETYPE_IFLY_RECORD == m_recognizeType || RECOGNIZETYPE_IFLY_STREAM == m_recognizeType)
		{
			XString iflytekAppid;
#ifdef WIN32
			iflytekAppid = m_pIManager->GetConfigure(CONFIG_IFLYTEK_APPID_WIN, XString(IFLYTEK_APPID_DEFAULT));
#elif (OS_IOS || OS_IOSSIMULATOR)
			iflytekAppid = m_pIManager->GetConfigure(CONFIG_IFLYTEK_APPID_IOS, XString(IFLYTEK_APPID_DEFAULT));
#elif OS_ANDROID
			iflytekAppid = m_pIManager->GetConfigure(CONFIG_IFLYTEK_APPID_ANDROID, XString(IFLYTEK_APPID_DEFAULT));
#endif
			appkey = XStringToUTF8(iflytekAppid);
		}
		else if (RECOGNIZETYPE_USC == m_recognizeType)
		{
			XString strAppkey = m_pIManager->GetConfigure(CONFIG_RECOGNIZE_APPKEY, XString(USC_APPKEY_DEFAULT));
			XString strSecret = m_pIManager->GetConfigure(CONFIG_RECOGNIZE_APPSEC, XString(USC_SECRET_DEFAULT));
			appkey = XStringToUTF8(strAppkey);
			secret = XStringToUTF8(strSecret);
		}
		else if (RECOGNIZETYPE_ALI == m_recognizeType)
		{
			XString strAppkey = m_pIManager->GetConfigure(CONFIG_RECOGNIZE_APPKEY, XString(ALI_APPKEY_DEFAULT));
			XString strSecret = m_pIManager->GetConfigure(CONFIG_RECOGNIZE_APPSEC, XString(ALI_SECRET_DEFAULT));
			appkey = XStringToUTF8(strAppkey);
			secret = XStringToUTF8(strSecret);
		}
		
		bool ret = m_pVoiceManager->Init(appkey, secret, m_recognizeType);
		if (!ret)
		{
			YouMe_LOG_Error(__XT("voicemanager init failed"));
		}

		if (!m_strAudioCacheDir.empty())
		{
			SetAudioRecordCacheDir(m_strAudioCacheDir);
		}

		CodingFormat codeFormat = (CodingFormat)m_pIManager->GetConfigure<int>(CONFIG_CODING_FORMAT, CODING_AMR);	// CODING_AMR
		int sampleRate = m_pIManager->GetConfigure(CONFIG_AUDIO_SAMPLERATE, SAMPLERATE_16K);

		if (CODING_AMR == codeFormat && sampleRate > SAMPLERATE_16K)	// ARM编码不支持高采样
		{
			sampleRate = SAMPLERATE_16K;
		}
        if (CODING_OPUS == codeFormat && sampleRate > SAMPLERATE_48K)
        {
            sampleRate = SAMPLERATE_48K;
        }
		SetSampleRate(sampleRate);

		YouMe_LOG_Info(__XT("recognize type:%d samplerate:%d"), m_recognizeType, m_iSampleRate);

		FontCovertType type = (FontCovertType)m_pIManager->GetConfigure<int>(CONFIG_FONT_CONVERT_TYPE, CONVERT_SIMPLE_TO_TRADITION);
		XString lexiconMD5 = m_pIManager->GetConfigure<XString>(CONFIG_LEXICON_MD5, XString(__XT("")));
		XString lexiconURL = m_pIManager->GetConfigure<XString>(CONFIG_LEXICON_URL, XString(__XT("")));
		if (!lexiconMD5.empty() && !lexiconURL.empty())
		{
			m_fontConverter.Init(type, lexiconMD5, lexiconURL);
		}

		m_pVoiceManager->SetAudioListener(this);

		return ret;
	}
	return  false;
}

void CYouMeSpeechManager::UnInit()
{
	DestroyResample();
	if (NULL != m_pVoiceManager)
	{
		m_pVoiceManager->UnInit();
	}
}

YIMErrorcode CYouMeSpeechManager::SetSampleRate(int sampleRate)
{
	if (m_pVoiceManager == NULL)
	{
		YouMe_LOG_Error(__XT("voicemanager is not init"));
		return YIMErrorcode_PTT_InitFailed;
	}

	if (m_iSampleRate == sampleRate)
	{
		return YIMErrorcode_Success;
	}
	m_iSampleRate = sampleRate;

	m_pVoiceManager->SetAudioRecordParam(sampleRate, CHANNEL_NUMBER, SAMPLE_BIT_SIZE);
	if ((RECOGNIZETYPE_IFLY_STREAM == m_recognizeType || RECOGNIZETYPE_USC == m_recognizeType || RECOGNIZETYPE_ALI == m_recognizeType)
		&& sampleRate != SAMPLERATE_8K && sampleRate != SAMPLERATE_16K)
	{
		//讯飞音频流识别只支持8K和16K，高采样先重采样到16K
		InitResample(CHANNEL_NUMBER, sampleRate, SAMPLERATE_16K);
	}

	YouMe_LOG_Info(__XT("set samplerate %d"), sampleRate);

	return YIMErrorcode_Success;
}

void CYouMeSpeechManager::SetAudioRecordCacheDir(const XString& path)
{
	YouMe_LOG_Info(__XT("set record cache dir:%s"), path.c_str());

	if (m_pVoiceManager != NULL)
	{
		XString strDir = path;
#ifdef WIN32
		strDir = CStringUtil::replace_text(strDir, __XT("/"), __XT("\\"));
#endif
		if (strDir[strDir.length() - 1] != __XT('/') && strDir[strDir.length() - 1] != __XT('\\'))
		{
			strDir.append(XPreferredSeparator);
		}
		youmecommon::CXFile::make_dir_tree(strDir.c_str());
		m_pVoiceManager->SetAudioRecordCacheDir(strDir);
	}
}

void CYouMeSpeechManager::SetAudioRecordCacheDir(const XCHAR* path)
{
	YouMe_LOG_Info(__XT("set record cache dir:%s"), path);
	m_strAudioCacheDir = XString(path);
}

XString CYouMeSpeechManager::GetAudioRecordCacheDir()
{
	XString audioRecordCacheDir;
	if (m_pVoiceManager != NULL)
	{
		audioRecordCacheDir = m_pVoiceManager->GetAudioRecordCacheDir();
	}
	return audioRecordCacheDir;
}

YIMErrorcode CYouMeSpeechManager::SetSpeechRecognizeLanguage(SpeechLanguage language)
{
	YouMe_LOG_Info(__XT("language:%d"), language);

	m_speechLanguage = language;

	YIMErrorcode errorcode = YIMErrorcode_Success;
	if (m_pVoiceManager != NULL)
	{
		m_pVoiceManager->SetRecognizeLanguage((RecognizeLanguage)language);
	}
	else
	{
		errorcode = YIMErrorcode_PTT_NotInit;
	}
	return errorcode;
}

YIMErrorcode CYouMeSpeechManager::StartSpeech(XUINT64 serial, bool speechRecognize)
{
	if (m_pVoiceManager == NULL)
	{
		YouMe_LOG_Error(__XT("voicemanager is not init"));
		return YIMErrorcode_PTT_InitFailed;
	}

    bool useRecognizer = false;
	int ret = 0;
	if (speechRecognize)
	{
		bool bCloseText = m_pIManager->GetConfigure(CONFIG_CLOSE_VOICE_TO_TEXT, false);	//是否使用语音识别
		if (bCloseText ||m_recognizeType == RECOGNIZETYPE_NO ) //|| m_iSpeechRecognizeFailedCount >= SPEECH_RECOGNIZE_FAILED_COUNT
		{
			ret = m_pVoiceManager->StartOnlySpeech(serial);
		}
		else
		{
			ret = m_pVoiceManager->StartSpeech(serial);
            useRecognizer = true;
		}
    }
	else
	{
		ret = m_pVoiceManager->StartOnlySpeech(serial);
	}
    
	YIMErrorcode errorcode = YIMErrorcode_PTT_Fail;
	if (AUDIOERROR_SUCCESS == ret)
	{
		errorcode = YIMErrorcode_Success;
	}
	else if (AUDIOERROR_INVALID_PARAM == ret)
	{
		errorcode = YIMErrorcode_ParamInvalid;
	}
	else if (AUDIOERROR_RECORDING == ret)
	{
		errorcode = YIMErrorcode_PTT_IsSpeeching;
	}
	else if (AUDIOERROR_NO_AUDIO_DEVICE == ret)
	{
		errorcode = YIMErrorcode_PTT_NoAudioDevice;
	}
	else if (AUDIOERROR_AUTHORIZE == ret)
	{
		errorcode = YIMErrorcode_PTT_Authorize;
	}
	else if (AUDIOERROR_START_RECORD_FAILED == ret)
	{
		errorcode = YIMErrorcode_PTT_StartAudioRecordFailed;
	}
	else if (AUDIOERROR_NOT_INIT == ret)
	{
		errorcode = YIMErrorcode_PTT_NotInit;
	}
	else if (AUDIOERROR_INIT_FAILED == ret)
	{
		errorcode = YIMErrorcode_PTT_InitFailed;
	}
	else if (20006 == ret || 20009 == ret)	// 讯飞录音失败
	{
		errorcode = YIMErrorcode_PTT_StartAudioRecordFailed;
//        ++m_iSpeechRecognizeFailedCount;
	}

	if (errorcode != YIMErrorcode_Success)
	{
		YouMe_LOG_Error(__XT("startspeech error(%d)"), ret);
	}

    if (useRecognizer)
    {
        ReportSpeechRecoginzeStatus(errorcode, m_recognizeType);
    }
	return errorcode;
}

YIMErrorcode CYouMeSpeechManager::StopSpeech()
{
	if (m_pVoiceManager == NULL)
	{
		YouMe_LOG_Error(__XT("voicemanager is not init"));
		return YIMErrorcode_PTT_InitFailed;
	}
	YIMErrorcode errorcode = YIMErrorcode_PTT_Fail;
	int ret = m_pVoiceManager->StopSpeech();
	if (AUDIOERROR_SUCCESS == ret)
	{
		errorcode = YIMErrorcode_Success;
	}
	else if (AUDIOERROR_NOT_START_RECORD == ret)
	{
		errorcode = YIMErrorcode_PTT_NotSpeech;
	}
	else if (AUDIOERROR_STOP_RECORD_FAILED == ret)
	{
		errorcode = YIMErrorcode_PTT_StopRecordFailed;
	}

	if (errorcode != YIMErrorcode_Success)
	{
		YouMe_LOG_Error(__XT("StopSpeech error(%d)"), ret);
	}

	return errorcode;
}

YIMErrorcode CYouMeSpeechManager::CancleSpeech()
{
	if (m_pVoiceManager == NULL)
	{
		YouMe_LOG_Error(__XT("voicemanager is not init"));
		return YIMErrorcode_PTT_InitFailed;
	}

	YIMErrorcode errorcode = YIMErrorcode_PTT_Fail;
	int ret = m_pVoiceManager->CancleSpeech();
	if (AUDIOERROR_SUCCESS == ret)
	{
		errorcode = YIMErrorcode_Success;
	}
	else if (AUDIOERROR_NOT_START_RECORD == ret)
	{
		errorcode = YIMErrorcode_PTT_NotSpeech;
	}
	else if (AUDIOERROR_STOP_RECORD_FAILED == ret)
	{
		errorcode = YIMErrorcode_PTT_StopRecordFailed;
	}

	if (errorcode != YIMErrorcode_Success)
	{
		YouMe_LOG_Info(__XT("CancleSpeech error(%d)"), ret);
	}

	return errorcode;
}

void CYouMeSpeechManager::SetKeepRecordModel(bool keep)
{
    if (NULL != m_pVoiceManager)
	{
		m_pVoiceManager->SetKeepRecordModel(keep);
    }
}

void CYouMeSpeechManager::SetPlayVolume(float volume)
{
	if (m_pVoiceManager == NULL)
	{
		return;
	}
	float fVolume = volume;
	if (fVolume > 1.0f || fVolume < 0)
	{
		fVolume = 1.0f;
	}
	return m_pVoiceManager->SetPlayVolume(fVolume);
}

YIMErrorcode CYouMeSpeechManager::StartPlayAudio(const XCHAR* path)
{
	if (m_pVoiceManager == NULL || path == NULL)
	{
		YouMe_LOG_Error(__XT("voicemanager is not init"));
		return YIMErrorcode_PTT_InitFailed;
	}
	YouMe_LOG_Info(__XT("StartPlayAudio path: %s"), path);
	XString strPath = XString(path);
#ifdef WIN32
	strPath = CStringUtil::replace_text(strPath, __XT("/"), __XT("\\"));
#endif
	if (!youmecommon::CXFile::IsFileExist(strPath))
	{
		YouMe_LOG_Error(__XT("StartPlayAudio path not exist:%s"), strPath.c_str());
		return YIMErrorcode_FileNotExist;
	}

	AudioErrorCode retCode = (AudioErrorCode)m_pVoiceManager->StartPlayAudio(strPath);
	YIMErrorcode errorcode = YIMErrorcode_PTT_Fail;
	if (AUDIOERROR_SUCCESS == retCode)
	{
		errorcode = YIMErrorcode_Success;
	}
    else if (AUDIOERROR_RECORDING == retCode)
    {
        errorcode = YIMErrorcode_PTT_IsSpeeching;
    }
	else if (AUDIOERROR_PLAYING == retCode)
	{
		errorcode = YIMErrorcode_PTT_IsPlaying;
	}
	else if (AUDIOERROR_START_PLAY_FAILED == retCode)
	{
		errorcode = YIMErrorcode_PTT_StartPlayFailed;
	}
	else if (AUDIOERROR_INVALID_PARAM == retCode)
	{
		errorcode = YIMErrorcode_ParamInvalid;
	}
	else if (AUDIOERROR_NO_AUDIO_DEVICE == retCode)
	{
		errorcode = YIMErrorcode_PTT_NoAudioDevice;
	}
	else if (AUDIOERROR_FILE_NOT_EXIT == retCode)
	{
		errorcode = YIMErrorcode_FileNotExist;
	}
	else if (AUDIOERROR_START_PLAY_FAILED == retCode)
	{
		errorcode = YIMErrorcode_PTT_StartPlayFailed;
	}
	
	if (errorcode != YIMErrorcode_Success)
	{
		YouMe_LOG_Error(__XT("StartPlayAudio error(%d)"), retCode);
	}

	return errorcode;
}

YIMErrorcode CYouMeSpeechManager::StopPlayAudio()
{
	if (m_pVoiceManager == NULL)
	{
		YouMe_LOG_Error(__XT("voicemanager is not init"));
		return YIMErrorcode_PTT_InitFailed;
	}

	AudioErrorCode retCode = (AudioErrorCode) m_pVoiceManager->StopPlayAudio();

	YIMErrorcode errorcode = YIMErrorcode_PTT_Fail;
	if (AUDIOERROR_SUCCESS == retCode)
	{
		errorcode = YIMErrorcode_Success;
	}
	else if (AUDIOERROR_NOT_START_PLAY == retCode)
	{
		errorcode = YIMErrorcode_PTT_NotStartPlay;
	}
	else if (AUDIOERROR_STOP_PLAY_FAILED == retCode)
	{
		errorcode = YIMErrorcode_PTT_StopPlayFailed;
	}
	else if (AUDIOERROR_DEVICE_STATUS_INVALID == retCode)
	{
		errorcode = YIMErrorcode_PTT_DeviceStatusError;
	}
	else if (AUDIOERROR_NODRIVER == retCode)
	{
		errorcode = YIMErrorcode_PTT_NoDriver;
	}

	if (errorcode != YIMErrorcode_Success)
	{
		YouMe_LOG_Error(__XT("StopPlayAudio error(%d)"), retCode);
	}

	return errorcode;
}

bool CYouMeSpeechManager::IsPlaying()
{
	if (m_pVoiceManager == NULL)
	{
		return false;
	}
	return m_pVoiceManager->IsPlaying();
}

int CYouMeSpeechManager::GetMicrophoneStatus()
{
	if (m_pVoiceManager == NULL)
	{
		return 0;
	}

	std::thread t = std::thread(&CYouMeSpeechManager::GetMicrophoneStatusThread, this);
	t.detach();

	return 0;
}

float CYouMeSpeechManager::CalculateVolume(byte* buffer, unsigned int size)
{
    if (NULL == buffer || size <= 1)
    {
        return 0;
    }
    unsigned int pcm_sum = 0;
    unsigned int pcm_count = 0;
    unsigned int pcm_avg = 0;
	for (unsigned int index = 0; index < size - 1; index += 2)
    {
        pcm_sum += abs((short)(buffer[index] | buffer[index+1] << 8));
        ++pcm_count;
    }
    if (pcm_count > 0)
    {
        pcm_avg = pcm_sum / pcm_count;
    }
    //分贝值
    float power_db = (20 * log10((double)pcm_avg / 32767));
    
    //显示的音量值
    float level = power_db + SILENCE_DB;
    if (level < 0)
    {
        level = 0.0;
    }
    if (level > SILENCE_DB)
    {
        level = SILENCE_DB;
    }
    level /= SILENCE_DB;
	level = ((float)((int) (level * 10 + 0.5) % 10)) / 10.0;
    
    return level;
}

void CYouMeSpeechManager::GetMicrophoneStatusThread()
{
	AudioDeviceState status = (AudioDeviceState)m_pVoiceManager->GetMicrophoneStatus();
	if (m_pAudioPlayCallback)
	{
		m_pAudioPlayCallback->OnGetMicrophoneStatus(static_cast<AudioDeviceStatus>(status));
	}
}

YIMErrorcode CYouMeSpeechManager::GetErrorcode(int audioErrorcode)
{
	YIMErrorcode errorCode = YIMErrorcode_PTT_Fail;
	if (AUDIOERROR_SUCCESS == audioErrorcode)
	{
		errorCode = YIMErrorcode_Success;
	}
	else if (AUDIOERROR_NOT_INIT == audioErrorcode)
	{
		errorCode = YIMErrorcode_PTT_NotInit;
	}
	else if (AUDIOERROR_INIT_FAILED == audioErrorcode)
	{
		errorCode = YIMErrorcode_PTT_InitFailed;
	}
	else if (AUDIOERROR_INVALID_PARAM == audioErrorcode)
	{
		errorCode = YIMErrorcode_ParamInvalid;
	}
	else if (AUDIOERROR_FILE_NOT_EXIT == audioErrorcode)
	{
		errorCode = YIMErrorcode_PTT_FileNotExist;
	}
	else if (AUDIOERROR_READWRITE_FILE_ERROR == audioErrorcode)
	{
		errorCode = YIMErrorcode_PTT_ReadWriteFileError;
	}
	else if (AUDIOERROR_CREATE_FILE_FAILED == audioErrorcode)
	{
		errorCode = YIMErrorcode_CreateFileFailed;
	}
	else if (AUDIOERROR_NO_AUDIO_DEVICE == audioErrorcode)
	{
		errorCode = YIMErrorcode_PTT_NoAudioDevice;
	}
	else if (AUDIOERROR_NODRIVER == audioErrorcode)
	{
		errorCode = YIMErrorcode_PTT_NoDriver;
	}
	else if (AUDIOERROR_DEVICE_STATUS_INVALID == audioErrorcode)
	{
		errorCode = YIMErrorcode_PTT_DeviceStatusError;
	}
	else if (AUDIOERROR_UNSUPPORT_FORMAT == audioErrorcode)
	{
		errorCode = YIMErrorcode_PTT_UnsupprtFormat;
	}
	else if (AUDIOERROR_RESOLVE_FILE_ERROR == audioErrorcode)
	{
		errorCode = YIMErrorcode_PTT_ResolveFileError;
	}
	else if (AUDIOERROR_AUTHORIZE == audioErrorcode)
	{
		errorCode = YIMErrorcode_PTT_Authorize;
	}
	else if (AUDIOERROR_RECORDING == audioErrorcode)
	{
		errorCode = YIMErrorcode_PTT_IsSpeeching;
	}
	else if (AUDIOERROR_NOT_START_RECORD == audioErrorcode)
	{
		errorCode = YIMErrorcode_PTT_NotSpeech;
	}
	else if (AUDIOERROR_START_RECORD_FAILED == audioErrorcode)
	{
		errorCode = YIMErrorcode_PTT_StartRecordFailed;
	}
	else if (AUDIOERROR_STOP_RECORD_FAILED == audioErrorcode)
	{
		errorCode = YIMErrorcode_PTT_StopRecordFailed;
	}
	else if (AUDIOERROR_WRITE_WAV_FAILED == audioErrorcode)
	{
		errorCode = YIMErrorcode_PTT_ConvertFileFailed;
	}
	else if (AUDIOERROR_RECORD_TIMEOUT == audioErrorcode)
	{
		errorCode = YIMErrorcode_PTT_ReachMaxDuration;
	}
	else if (AUDIOERROR_RECORD_TIME_TOO_SHORT == audioErrorcode)
	{
		errorCode = YIMErrorcode_PTT_SpeechTooShort;
	}
	/*else if (AUDIOERROR_RECOGNIZE_FAILED == audioErrorcode)
	{
		errorCode = YIMErrorcode_PTT_RecognizeFailed;
	}*/
	else if (AUDIOERROR_NOT_START_PLAY == audioErrorcode)
	{
		errorCode = YIMErrorcode_PTT_NotStartPlay;
	}
	else if (AUDIOERROR_PLAYING == audioErrorcode)
	{
		errorCode = YIMErrorcode_PTT_IsPlaying;
	}
	else if (AUDIOERROR_START_PLAY_FAILED == audioErrorcode)
	{
		errorCode = YIMErrorcode_PTT_StartPlayFailed;
	}
	else if (AUDIOERROR_STOP_PLAY_FAILED == audioErrorcode)
	{
		errorCode = YIMErrorcode_PTT_StopPlayFailed;
	}

	return errorCode;
}

void CYouMeSpeechManager::OnRecordFinish(int errorcode, const XString& path, unsigned long long serial, const XString& text)
{
	YouMe_LOG_Info(__XT("speech stop Serial:%llu Errorcode:%d Path:%s text:%d"), serial, errorcode, path.c_str(), text.length());

	if (m_pMessageManager == NULL)
	{
		YouMe_LOG_Error(__XT("message manager is null"));
		return;
	}

	YIMErrorcode retErrorcode = YIMErrorcode_Success;
	//-----------------ifly error begin-----------------
	if (10118 == errorcode || 20007 == errorcode)
	{
		retErrorcode = YIMErrorcode_PTT_NotSpeech;
	}
	else if (20008 == errorcode)
	{
		retErrorcode = YIMErrorcode_PTT_SpeechTimeout;
	}
	else if (20006 == errorcode || 20009 == errorcode)
	{
		retErrorcode = YIMErrorcode_PTT_StartAudioRecordFailed;
//        ++m_iSpeechRecognizeFailedCount;
	}
	else if (AUDIOERROR_RECOGNIZE_FAILED == errorcode)
	{
//        ++m_iSpeechRecognizeFailedCount;
		//retErrorcode = YIMErrorcode_PTT_RecognizeFailed;
	}
	//-----------------ifly error end-----------------
	else
	{
		retErrorcode = GetErrorcode(errorcode);
	}

//    if (AUDIOERROR_SUCCESS == errorcode && (m_iSpeechRecognizeFailedCount > 0 )) //&& m_iSpeechRecognizeFailedCount < SPEECH_RECOGNIZE_FAILED_COUNT
//    {
//        m_iSpeechRecognizeFailedCount = 0;
//    }
	
	if (SPEECHLANG_TRADITIONAL == m_speechLanguage)
	{
		XString speechText;
		m_fontConverter.Convert(text, speechText);
		m_pMessageManager->OnSpeechResult(retErrorcode, path, serial, speechText);
	}
	else
	{
		m_pMessageManager->OnSpeechResult(retErrorcode, path, serial, text);
	}
}

void CYouMeSpeechManager::OnAudioRecordData(unsigned char* buffer, unsigned int size)
{
	// 直接用语音识别的音量回调
	if (RECOGNIZETYPE_IFLY_STREAM == m_recognizeType)
	{
		return;
	}

	// 最大200ms一次音量回调
	static XINT64 lastTime = 0;
	XINT64 currentTime = youmecommon::CTimeUtil::GetTimeOfDay_MS();
	if (currentTime - lastTime < 200)
	{
		return;
	}
	lastTime = currentTime;

	if (m_pMessageManager != NULL)
	{
		float volume = CalculateVolume(buffer, size);
		m_pMessageManager->OnRecordVolumeChange(volume);
	}
}

void CYouMeSpeechManager::OnAudioPlayData(unsigned char* buffer, unsigned int size)
{
	//float volume = CalculateVolume(buffer, size);
	//YouMe_LOG_Debug(__XT("play volume:%f size:%u"), volume, size);
}

void CYouMeSpeechManager::OnPlayFinish(int errorcode, const XString& path)
{
	if (NULL == m_pAudioPlayCallback)
	{
		YouMe_LOG_Error(__XT("audio callback is null"));
		return;
	}

	YIMErrorcode retCode = GetErrorcode(errorcode);
	if (retCode != YIMErrorcode_Success)
	{
		YouMe_LOG_Error(__XT("OnPlayFinish error:%d path:%s"), retCode, path.c_str());
	}

	m_pAudioPlayCallback->OnPlayCompletion(retCode, path.c_str());
}

void CYouMeSpeechManager::OnRecordVolumeChange(int volume, int type)
{
	if (m_pMessageManager == NULL)
	{
		return;
	}
	
	// 最大200ms一次音量回调
	static XINT64 lastTime = 0;
	XINT64 currentTime = youmecommon::CTimeUtil::GetTimeOfDay_MS();
	if (currentTime - lastTime < 200)
	{
		return;
	}
	lastTime = currentTime;

	float recordVolume = 0;
	if (RECOGNIZETYPE_IFLY_RECORD == type)
	{
		// volume范围0-30，为统一转换为0-1
		recordVolume = ((float)(volume * 2 / 10)) / 10;
	}
	else if (RECOGNIZETYPE_USC)
	{
		// volume范围0-100
		recordVolume = (float)volume / 100;
	}
	if (recordVolume > 1.0f)
	{
		recordVolume = 1.0f;
	}

	m_pMessageManager->OnRecordVolumeChange(recordVolume);
}

int CYouMeSpeechManager::InitResample(int channels, int inSampleRate, int outSampleRate)
{
	if (NULL == m_pAudioResample)
	{
		m_pAudioResample = new AudioResample;
	}
	else
	{
		m_pAudioResample->DestroyResample();
	}
	return m_pAudioResample->InitResample(channels, inSampleRate, outSampleRate);
}

int CYouMeSpeechManager::DestroyResample()
{
	if (NULL == m_pAudioResample)
	{
		return -1;
	}
	return m_pAudioResample->DestroyResample();
}

void CYouMeSpeechManager::ReportSpeechRecoginzeStatus(int status, short recognizeType)
{
    if (m_pIManager == NULL)
    {
        return;
    }
    ReportSpeechRecognize reportData;
    reportData.userid = XStringToUTF8(m_pIManager->GetCurrentUser());
    reportData.appid = m_pIManager->GetAppID();
    reportData.SDKVersion = SDK_VER;
    reportData.type = recognizeType;    // 2-讯飞， 3-云知声  4-阿里
    reportData.status = status;
    m_pIManager->ReportData(reportData);
}


unsigned char* CYouMeSpeechManager::ResampleAudioData(unsigned char* inBuffer, unsigned int inSize, unsigned int* outSize)
{
	if (NULL == m_pAudioResample)
	{
		YouMe_LOG_Error(__XT("resample not init"));
		return NULL;
	}
	
	static youmecommon::CXSharedArray<short> outBuffer;
	outBuffer.ReSize(inSize / sizeof(short));
	//short* outBuffer = new short[inSize / sizeof(short)];
	int size = m_pAudioResample->ResampleAudioData((short*)inBuffer, inSize / sizeof(short), outBuffer.Get());
	*outSize = size * sizeof(short);
    //YouMe_LOG_Debug(__XT("resample insize:%d outsize:%u"), inSize, *outSize);
	return (unsigned char*) outBuffer.Get();
}

void CYouMeSpeechManager::ReqAliAccessToken()
{
	if (m_recognizeType != RECOGNIZETYPE_ALI)
	{
		return;
	}
	YOUMEServiceProtocol::GetALiAccessTokenReq req;
	req.set_version(PROTOCOL_VERSION);
	std::string strData;
	req.SerializeToString(&strData);
	std::map<std::string, std::string> extend;
	XUINT64 msgSerial = 0;
	if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_GET_ALI_TOKEN, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		YouMe_LOG_Error(__XT("request ali token failed"));
	}
}

void CYouMeSpeechManager::OnAliAccessTokenRsp(ServerPacket& serverPacket)
{
	if (serverPacket.result == -1)
	{
		YouMe_LOG_Error(__XT("get ali tocken timeout serial:"), serverPacket.reqSerial);
		return;
	}
	YOUMEServiceProtocol::GetALiAccessTokenRsp rsp;
	if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
	{
		YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
		return;
	}
	if (rsp.ret() == 0)
	{
		YouMe_LOG_Info(__XT("update recognize token"));
		m_ullAliTokenSerial = rsp.token_seq();
		if (m_pVoiceManager)
		{
			m_pVoiceManager->UpdateToken(rsp.token());
		}
	}
	else
	{
		YouMe_LOG_Error(__XT("get ali tocken error(%d)"), rsp.ret());
	}
}
