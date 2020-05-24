
#if 0
#include "IflySpeechRecognize.h"
#include "YouMeCommon/Log.h"
#include "YouMeIMEngine/Speech/VoiceInterface.h"
#include "kdxf/qisr.h"
#include "kdxf/msp_cmn.h"
#include "kdxf/msp_errors.h"


#ifdef _WIN64
#pragma comment(lib,"../../lib/ifly/msc_x64.lib") //x64
#else
#pragma comment(lib,"../../lib/ifly/msc.lib") //x86
#endif

#define SESSON_PARAM "sub=iat,domain=iat,language=%s,accent=%s,sample_rate=%d,result_type=plain,result_encoding=utf8"


IflySpeechRecognize::IflySpeechRecognize() :m_bAudioConvertInit(false)
, m_nSampleRate(SAMPLERATE_16K)
, m_nFrameSize(640)
{

}

IflySpeechRecognize::~IflySpeechRecognize()
{
	UnInit();
}

int IflySpeechRecognize::Init(const std::string& appkey, const std::string& secret)
{
	char szParam[128] = { 0 };
	sprintf_s(szParam, "appid = %s, work_dir = .", appkey.c_str());
	int errorcode = MSPLogin(NULL, NULL, szParam);
	if (MSP_SUCCESS != errorcode)
	{
		MSPLogout();
		return false;
	}
	SetRecognizeSession(ACCENT_DEFAULT, LANGUAGE_DEFAULT);

	m_bAudioConvertInit = true;

	return 0;
}

int IflySpeechRecognize::UnInit()
{
	if (m_bAudioConvertInit)
	{
		MSPLogout();
		m_bAudioConvertInit = false;
	}
	return 0;
}

void IflySpeechRecognize::SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize)
{
	/*if (m_nSampleRate == sampleRate)
	{
		return;
	}
	m_nSampleRate = SAMPLERATE_16K;	// windows����ʶ���֧��16K
	CalculateFrameSize();
	SetRecognizeSession();*/
}

void IflySpeechRecognize::SetRecognizeLanguage(RecognizeLanguage language)
{
	std::string strAccent = "mandarin";
	std::string strLanguage = "zh_cn";
	if (RECOGNIZELANG_YUEYU == language)
	{
		strAccent = "cantonese";
	}
	else if (RECOGNIZELANG_SICHUAN == language)
	{
		strAccent = "lmz";
	}
	else if (RECOGNIZELANG_HENAN == language)
	{
		strAccent = "henanese";
	}
	else if (RECOGNIZELANG_ENGLISH == language)
	{
		strLanguage = "en_us";
		strAccent = "mandarin";
	}
	SetRecognizeSession(strAccent, strLanguage);
}

bool IflySpeechRecognize::AudioRecognize(const std::wstring& audioPath, std::wstring& text)
{
	if (!m_bAudioConvertInit || audioPath.empty())
	{
		return false;
	}

	FILE* pFile = NULL;
	errno_t errCode = _wfopen_s(&pFile, audioPath.c_str(), __XT("rb"));
	if (errCode != 0 || NULL == pFile)
	{
		return false;
	}

	bool bRet = true;
	std::string audioText;

	fseek(pFile, 0, SEEK_END);
	long lAudioFileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

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

	int nErrorCode = MSP_SUCCESS;
	const char*	szSessionID = QISRSessionBegin(NULL, m_strSessionParam.c_str(), &nErrorCode); //��д����Ҫ�﷨����һ������ΪNULL
	if (MSP_SUCCESS != nErrorCode)
	{
		bRet = false;
		goto RECOGNIZE_EXIT;
	}

	int nAudioState = MSP_AUDIO_SAMPLE_CONTINUE;	//��Ƶ״̬
	int nEpState = MSP_EP_LOOKING_FOR_SPEECH;		//�˵���
	int	nRecognizeState = MSP_REC_STATUS_SUCCESS;	//ʶ��״̬
	int nRet = 0;
	long lSendSize = 0;

	while (1)
	{
		unsigned int nSize = 10 * m_nFrameSize; // ÿ��д��200ms��Ƶ(16k��16bit)��1֡��Ƶ20ms��10֡=200ms��16k�����ʵ�16λ��Ƶ��һ֡�Ĵ�СΪ640Byte
		if (lAudioFileSize < nSize)
		{
			nSize = lAudioFileSize;
		}
		if (nSize <= 0)
		{
			break;
		}
		nAudioState = MSP_AUDIO_SAMPLE_CONTINUE;
		if (0 == lSendSize)
		{
			nAudioState = MSP_AUDIO_SAMPLE_FIRST;
		}

		nRet = QISRAudioWrite(szSessionID, (const void *)&szAudioData[lSendSize], nSize, nAudioState, &nEpState, &nRecognizeState);
		if (MSP_SUCCESS != nRet)
		{
			bRet = false;
			goto RECOGNIZE_EXIT;
		}

		lSendSize += (long)nSize;
		lAudioFileSize -= (long)nSize;

		if (MSP_REC_STATUS_SUCCESS == nRecognizeState) //�Ѿ��в�����д���
		{
			const char* szText = QISRGetResult(szSessionID, &nRecognizeState, 0, &nErrorCode);
			if (MSP_SUCCESS != nErrorCode)
			{
				bRet = false;
				goto RECOGNIZE_EXIT;
			}
			if (NULL != szText && strlen(szText) > 0)
			{
				audioText.append(szText);
			}
		}

		if (MSP_EP_AFTER_SPEECH == nEpState)
		{
			break;
		}
	}
	nErrorCode = QISRAudioWrite(szSessionID, NULL, 0, MSP_AUDIO_SAMPLE_LAST, &nEpState, &nRecognizeState);
	if (MSP_SUCCESS != nErrorCode)
	{
		bRet = false;
		goto RECOGNIZE_EXIT;
	}

	while (MSP_REC_STATUS_COMPLETE != nRecognizeState)
	{
		const char* szText = QISRGetResult(szSessionID, &nRecognizeState, 0, &nErrorCode);
		if (MSP_SUCCESS != nErrorCode)
		{
			bRet = false;
			goto RECOGNIZE_EXIT;
		}
		if (NULL != szText && strlen(szText) > 0)
		{
			audioText.append(szText);
		}
		Sleep(30); //��ֹƵ��ռ��CPU
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

	char hints[1] = { 0 }; // �������λỰ��ԭ������ �û��Զ���
	QISRSessionEnd(szSessionID, hints);

	if (!audioText.empty())
	{
		text = UTF8TOXString(audioText);
	}

	return bRet;
}

void IflySpeechRecognize::SetRecognizeSession(const std::string& accent, const std::string& language)
{
	char szSessionParam[256] = { 0 };
	sprintf_s(szSessionParam, SESSON_PARAM, language.c_str(), accent.c_str(), m_nSampleRate);
	m_strSessionParam = std::string(szSessionParam);
}
#endif