#include "UscSpeechRecognize.h"
#include "YouMeCommon/Log.h"
#include "YouMeIMEngine/Speech/VoiceInterface.h"


#ifdef _WIN64
#pragma comment(lib,"../../lib/usc/x64/libusc.lib") //x64
#else
#pragma comment(lib,"../../lib/usc/x86/libusc.lib") //x86
#endif

UscSpeechRecognize::UscSpeechRecognize() : m_uscHandle(NULL)
, m_nFrameSize(640)
, m_nSampleRate(SAMPLERATE_16K)
{

}

UscSpeechRecognize::~UscSpeechRecognize()
{
	UnInit();
}

int UscSpeechRecognize::Init(const std::string& appkey, const std::string& secret)
{
	if (m_uscHandle != NULL)
	{
		return 0;
	}
	int errrocode = usc_create_service(&m_uscHandle);
	if (errrocode != USC_ASR_OK)
	{
		YouMe_LOG_Error(__XT("create recognize service failed(%d)"), errrocode);
		return 0;
	}

	errrocode = usc_set_option(m_uscHandle, USC_OPT_ASR_APP_KEY, appkey.c_str());
	errrocode = usc_set_option(m_uscHandle, USC_OPT_USER_SECRET, secret.c_str());

	errrocode = usc_login_service(m_uscHandle);
	if (errrocode != USC_ASR_OK)
	{
		YouMe_LOG_Error(__XT("recognize service login error(%d)"), errrocode);
		return errrocode;
	}

	usc_set_option(m_uscHandle, USC_OPT_INPUT_AUDIO_FORMAT, AUDIO_FORMAT_PCM_16K);
	usc_set_option(m_uscHandle, USC_OPT_RECOGNITION_FIELD, RECOGNITION_FIELD_GENERAL);
	usc_set_option(m_uscHandle, USC_OPT_LANGUAGE_SELECT, LANGUAGE_CHINESE);

	return 0;
}

int UscSpeechRecognize::UnInit()
{
	if (m_uscHandle != NULL)
	{
		usc_release_service(m_uscHandle);
		m_uscHandle = NULL;
	}
	return 0;
}

void UscSpeechRecognize::SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize)
{
	if (m_nSampleRate == sampleRate)
	{
		return;
	}
	m_nSampleRate = sampleRate == SAMPLERATE_8K ? sampleRate : SAMPLERATE_16K;
	if (m_uscHandle != NULL)
	{
		usc_set_option(m_uscHandle, USC_OPT_INPUT_AUDIO_FORMAT, AUDIO_FORMAT_PCM_16K);
	}
	m_nFrameSize = m_nSampleRate * (sampleBitSize / 8) / 50;
}

void UscSpeechRecognize::SetRecognizeLanguage(RecognizeLanguage language)
{
	if (RECOGNIZELANG_YUEYU == language)
	{
		usc_set_option(m_uscHandle, USC_OPT_LANGUAGE_SELECT, LANGUAGE_CANTONESE);
	}
	else if (RECOGNIZELANG_ENGLISH == language)
	{
		usc_set_option(m_uscHandle, USC_OPT_LANGUAGE_SELECT, LANGUAGE_ENGLISH);
	}
	else
	{
		usc_set_option(m_uscHandle, USC_OPT_LANGUAGE_SELECT, LANGUAGE_CHINESE);
	}
}

bool UscSpeechRecognize::AudioRecognize(const std::wstring& audioPath, std::wstring& text)
{
	FILE* pFile = NULL;
	errno_t errCode = _wfopen_s(&pFile, audioPath.c_str(), __XT("rb"));
	if (errCode != 0 || NULL == pFile)
	{
		return false;
	}

	fseek(pFile, 0, SEEK_END);
	long lAudioFileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	bool bRet = true;
	std::string audioText;
	char* szAudioData = new char[lAudioFileSize];
	if (NULL == szAudioData)
	{
		bRet = false;
		goto RECOGNIZE_EXIT;
	}
	if (fread((void *)szAudioData, 1, lAudioFileSize, pFile) != lAudioFileSize)
	{
		bRet = false;
		goto RECOGNIZE_EXIT;
	}
	
	int errorcode = usc_start_recognizer(m_uscHandle);
	if (errorcode != USC_ASR_OK)
	{
		YouMe_LOG_Error(__XT("start recognize error(%d)"), errorcode);
		bRet = false;
		goto RECOGNIZE_EXIT;
	}
	long lSendSize = 0;

	while (1)
	{
		unsigned int nSize = 10 * m_nFrameSize; // 每次写入200ms音频(16k，16bit)：1帧音频20ms，10帧=200ms。16k采样率的16位音频，一帧的大小为640Byte
		if (lAudioFileSize < nSize)
		{
			nSize = lAudioFileSize;
		}
		if (nSize <= 0)
		{
			break;
		}
		errorcode = usc_feed_buffer(m_uscHandle, &szAudioData[lSendSize], nSize);
		if (errorcode == USC_RECOGNIZER_PARTIAL_RESULT || errorcode == USC_RECOGNIZER_SPEAK_END)
		{
			// 获取中间部分识别结果
			audioText.append(usc_get_result(m_uscHandle));
		}
		else if (errorcode < 0)
		{
			// 网络出现错误退出
			YouMe_LOG_Error(__XT("recognize error(%d)"), errorcode);
			bRet = false;
			goto RECOGNIZE_EXIT;
		}

		lSendSize += (long)nSize;
		lAudioFileSize -= (long)nSize;
	}
	// 停止语音输入
	errorcode = usc_stop_recognizer(m_uscHandle);
	if (errorcode == 0)
	{
		audioText.append(usc_get_result(m_uscHandle));
	}
	else
	{
		YouMe_LOG_Error(__XT("stop recognize error(%d)"), errorcode);
	}

RECOGNIZE_EXIT:
	if (NULL != pFile)
	{
		fclose(pFile);
		pFile = NULL;
	}
	if (NULL != szAudioData)
	{
		delete[] szAudioData;
		szAudioData = NULL;
	}
	if (!audioText.empty())
	{
		text = UTF8TOXString(audioText);
	}
	return bRet;
}

bool UscSpeechRecognize::IsSupportStreamAudio()
{
	return true;
}
bool UscSpeechRecognize::StartStreamAudioRecognize()
{
	m_curAudioText = "";
	int errorcode = usc_start_recognizer(m_uscHandle);
	if (errorcode != USC_ASR_OK)
	{
		YouMe_LOG_Error(__XT("start recognize error(%d)"), errorcode);
		return false;
	}
	else{
		return true;
	}

}
bool UscSpeechRecognize::StopStreamAudioRecognize( std::wstring& text )
{
	// 停止语音输入
	auto errorcode = usc_stop_recognizer(m_uscHandle);
	if (errorcode == 0)
	{
		m_curAudioText.append(usc_get_result(m_uscHandle));
		text = UTF8TOXString(m_curAudioText);
		return true;
	}
	else
	{
		YouMe_LOG_Error(__XT("stop recognize error(%d)"), errorcode);
		return false;
	}
}

bool UscSpeechRecognize::CancelStreamAudioRecognize()
{
	m_curAudioText = "";
	auto errorcode = usc_stop_recognizer(m_uscHandle);
	if (errorcode == 0)
	{
		return true;
	}
	else
	{
		YouMe_LOG_Error(__XT("stop recognize error(%d)"), errorcode);
		return false;
	}
}

bool UscSpeechRecognize::InputAudioData(char* data, int data_size, bool isLast)
{
	if (NULL == data || data_size <= 1)
    {
        return true;
    }
	auto errorcode = usc_feed_buffer(m_uscHandle, data, data_size );
	if (errorcode == USC_RECOGNIZER_PARTIAL_RESULT || errorcode == USC_RECOGNIZER_SPEAK_END)
	{
		// 获取中间部分识别结果
		m_curAudioText.append(usc_get_result(m_uscHandle));

		YouMe_LOG_Error(__XT("InputAudioData recognize result:%s"), UTF8TOXString(m_curAudioText).c_str());
		return true;
	}
	else if (errorcode < 0)
	{
		// 网络出现错误退出
		YouMe_LOG_Error(__XT("recognize error(%d)"), errorcode);
		return false;
	}
	else{
		return true;
	}
}