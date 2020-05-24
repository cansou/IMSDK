#include "AliSpeechRecognize.h"
#include "YouMeCommon/Log.h"
#include "YouMeCommon/json/json.h"
#include "YouMeIMEngine/Speech/VoiceInterface.h"
#include "ali/nlsClient.h"


#ifdef _WIN64
#pragma comment(lib,"../../lib/ali/x64/nlscppsdk.lib")
#else
#pragma comment(lib,"../../lib/ali/x86/nlscppsdk.lib")
#endif


std::string AliSpeechRecognize::m_audioText;
HANDLE AliSpeechRecognize::m_recognizeEvent = NULL;

AliSpeechRecognize::AliSpeechRecognize() : m_nFrameSize(640)
, m_nSampleRate(SAMPLERATE_16K)
, m_pCallback(NULL)
, m_recognizeRequest(NULL)
{

}

AliSpeechRecognize::~AliSpeechRecognize()
{
	UnInit();
}

int AliSpeechRecognize::Init(const std::string& appkey, const std::string& secret)
{
	m_pCallback = new AlibabaNls::SpeechRecognizerCallback();
	m_pCallback->setOnRecognitionStarted(AliSpeechRecognize::OnRecognitionStarted, NULL);	// start()�ɹ��ص�
	m_pCallback->setOnTaskFailed(OnRecognitionTaskFailed, NULL);							// �쳣ʶ��ص�
	m_pCallback->setOnChannelClosed(OnRecognitionChannelCloseed, NULL);						// ʶ��ͨ���رջص�
	m_pCallback->setOnRecognitionResultChanged(OnRecognitionResultChanged, NULL);			// �м����ص�
	m_pCallback->setOnRecognitionCompleted(OnRecognitionCompleted, NULL);					// ����ʶ������ص�


	/*m_recognizeRequest = AlibabaNls::NlsClient::getInstance()->createRecognizerRequest(m_pCallback);
	if (NULL == m_recognizeRequest)
	{
		return AUDIOERROR_INIT_FAILED;
	}
	//m_recognizeRequest->setUrl("wss://nls-gateway.cn-shanghai.aliyuncs.com/ws/v1");		// ���÷����url, �������
	m_recognizeRequest->setAppKey(appkey.c_str());
	m_recognizeRequest->setToken("a3b754471d1f447fbfcbcadd9a0f7e89");
	m_recognizeRequest->setFormat("pcm");												// ������Ƶ���ݱ����ʽ, ��ѡ����, Ŀǰ֧��pcm, opus, opu. Ĭ����pcm
	m_recognizeRequest->setSampleRate(m_nSampleRate);									// ������Ƶ���ݲ�����, ��ѡ����, Ŀǰ֧��16000, 8000. Ĭ����16000
	m_recognizeRequest->setIntermediateResult("false");									// �����Ƿ񷵻��м�ʶ����, ��ѡ����. Ĭ��false
	m_recognizeRequest->setPunctuationPrediction("false");								// �����Ƿ��ں�������ӱ��, ��ѡ����. Ĭ��false
	m_recognizeRequest->setInverseTextNormalization("false");							// �����Ƿ��ں�����ִ��ITN, ��ѡ����. Ĭ��false
	m_recognizeRequest->setPunctuationPrediction("true");								// �Ƿ��ں�������ӱ��, ��ѡ����. Ĭ��false*/

	if (m_recognizeEvent == NULL)
	{
		m_recognizeEvent = ::CreateEvent(0, FALSE, FALSE, 0);
	}

	return 0;
}

int AliSpeechRecognize::UnInit()
{
	if (m_recognizeRequest != NULL)
	{
		AlibabaNls::NlsClient::getInstance()->releaseRecognizerRequest(m_recognizeRequest);
		m_recognizeRequest = NULL;
	}
	AlibabaNls::NlsClient::releaseInstance();

	if (m_pCallback != NULL)
	{
		delete m_pCallback;
		m_pCallback = NULL;
	}
	if (m_recognizeEvent != NULL)
	{
		if (CloseHandle(m_recognizeEvent))
		{
			m_recognizeEvent = NULL;
		}
	}
	return 0;
}

void AliSpeechRecognize::SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize)
{
	/*if (m_nSampleRate == sampleRate)
	{
		return;
	}
	m_nSampleRate = sampleRate == SAMPLERATE_8K ? sampleRate : SAMPLERATE_16K;
	if (m_recognizeRequest != NULL)
	{
		m_recognizeRequest->setSampleRate(m_nSampleRate);
	}
	m_nFrameSize = m_nSampleRate * (sampleBitSize / 8) / 50;*/
}

void AliSpeechRecognize::SetRecognizeLanguage(RecognizeLanguage language)
{
	
}

void AliSpeechRecognize::UpdateToken(const std::string& token)
{
	//m_recognizeRequest->setToken(token.c_str());
}

bool AliSpeechRecognize::AudioRecognize(const std::wstring& audioPath, std::wstring& text)
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
	
	m_audioText = "";


	if (m_recognizeRequest != NULL)
	{
		AlibabaNls::NlsClient::getInstance()->releaseRecognizerRequest(m_recognizeRequest);
	}
	m_recognizeRequest = AlibabaNls::NlsClient::getInstance()->createRecognizerRequest(m_pCallback);
	if (NULL == m_recognizeRequest)
	{
		return AUDIOERROR_INIT_FAILED;
	}
	m_recognizeRequest->setAppKey("5hQi7z7mdufFPqvg");
	m_recognizeRequest->setToken("a3b754471d1f447fbfcbcadd9a0f7e89");
	m_recognizeRequest->setFormat("pcm");												// ������Ƶ���ݱ����ʽ, ��ѡ����, Ŀǰ֧��pcm, opus, opu. Ĭ����pcm
	m_recognizeRequest->setSampleRate(m_nSampleRate);									// ������Ƶ���ݲ�����, ��ѡ����, Ŀǰ֧��16000, 8000. Ĭ����16000
	m_recognizeRequest->setIntermediateResult("false");									// �����Ƿ񷵻��м�ʶ����, ��ѡ����. Ĭ��false
	m_recognizeRequest->setPunctuationPrediction("false");								// �����Ƿ��ں�������ӱ��, ��ѡ����. Ĭ��false
	m_recognizeRequest->setInverseTextNormalization("false");							// �����Ƿ��ں�����ִ��ITN, ��ѡ����. Ĭ��false
	m_recognizeRequest->setPunctuationPrediction("true");								// �Ƿ��ں�������ӱ��, ��ѡ����. Ĭ��false

	int errorcode = m_recognizeRequest->start();
	if (errorcode < 0)
	{
		YouMe_LOG_Error(__XT("start recognize error(%d)"), errorcode);
		bRet = false;
		goto RECOGNIZE_EXIT;
	}
	long lSendSize = 0;

	while (1)
	{
		int nSize = 10 * m_nFrameSize; // ÿ��д��200ms��Ƶ(16k��16bit)��1֡��Ƶ20ms��10֡=200ms��16k�����ʵ�16λ��Ƶ��һ֡�Ĵ�СΪ640Byte
		if (lAudioFileSize < nSize)
		{
			nSize = lAudioFileSize;
		}
		if (nSize <= 0)
		{
			break;
		}

		if (m_recognizeRequest->sendAudio(&szAudioData[lSendSize], nSize, false) < 0)
		{
			// ����ʧ��
			YouMe_LOG_Error(__XT("recognize error"));
			bRet = false;
			goto RECOGNIZE_EXIT;
		}

		lSendSize += (long)nSize;
		lAudioFileSize -= (long)nSize;

		// ����������ʵʱ��, ����sleep��������, ֱ�ӷ��ͼ���.
		// �������������ļ�, ����ʱ��Ҫ��������, ʹ��λʱ���ڷ��͵����ݴ�С�ӽ���λʱ��ԭʼ�������ݴ洢�Ĵ�С.
		//unsigned sleepMs = GetSendAudioSleepTime(nSize, m_nSampleRate, 1);
		//Sleep(sleepMs);
		Sleep(30);
	}
	// ֹͣ��������
	//DWORD begin2 = GetTickCount();
	errorcode = m_recognizeRequest->stop();
	//int cost2 = GetTickCount() - begin2;
	//YouMe_LOG_Info(__XT("-------------------------stop cost:%lu"), cost2);

	if (errorcode == 0)
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

	if (bRet && m_recognizeEvent != NULL)
	{
		::WaitForSingleObject(m_recognizeEvent, INFINITE);
	}
	if (!m_audioText.empty())
	{
		text = Local_to_Unicode(m_audioText.c_str(), m_audioText.length());
	}

	return bRet;
}

/*
* @brief ��ȡsendAudio������ʱʱ��
* @param dataSize ���������ݴ�С
* @param sampleRate ������ 16k/8K
* @param compressRate ����ѹ���ʣ�����ѹ����Ϊ10:1��16k opus���룬��ʱΪ10����ѹ��������Ϊ1
* @return ����sendAudio֮����Ҫsleep��ʱ��
* @note ����8k pcm ��������, 16λ����������ÿ����3200�ֽ� sleep 200 ms.
����16k pcm ��������, 16λ����������ÿ����6400�ֽ� sleep 200 ms.
�������������ʽ������, �û�����ѹ����, ���й���, ����ѹ����Ϊ10:1�� 16k opus,��Ҫÿ����6400/10=640 sleep 200ms.
*/
unsigned int AliSpeechRecognize::GetSendAudioSleepTime(const int dataSize, const int sampleRate, const int compressRate)
{
	const int sampleBytes = 16;	// ��֧��16λ����
	const int soundChannel = 1;	// ��֧�ֵ�ͨ��
	int bytes = (sampleRate * sampleBytes * soundChannel) / 8;	// ��ǰ�����ʣ�����λ����ÿ��������ݵĴ�С
	int bytesMs = bytes / 1000;									// ��ǰ�����ʣ�����λ����ÿ����������ݵĴ�С
	int sleepMs = (dataSize * compressRate) / bytesMs;			// ���������ݴ�С����ÿ����������ݴ�С���Ի�ȡsleepʱ��
	return sleepMs;
}

void AliSpeechRecognize::OnRecognitionStarted(AlibabaNls::NlsEvent* cbEvent, void* cbParam)
{

}

void AliSpeechRecognize::OnRecognitionResultChanged(AlibabaNls::NlsEvent* cbEvent, void* cbParam)
{

}

void AliSpeechRecognize::OnRecognitionCompleted(AlibabaNls::NlsEvent* cbEvent, void* cbParam)
{
	youmecommon::Value jsonResponse;
	youmecommon::Reader jsonReader;
	if (cbEvent != NULL && jsonReader.parse(cbEvent->getAllResponse(), jsonResponse))
	{
		if (jsonResponse.isMember("payload"))
		{
			youmecommon::Value payload = jsonResponse["payload"];
			if (payload.isMember("result"))
			{
				m_audioText = payload["result"].asString();
			}
		}
	}

	if (m_recognizeEvent != NULL)
	{
		SetEvent(m_recognizeEvent);
	}
}

void AliSpeechRecognize::OnRecognitionTaskFailed(AlibabaNls::NlsEvent* cbEvent, void* cbParam)
{
	if (cbEvent != NULL)
	{
		YouMe_LOG_Error(__XT("recognize error:%d"), cbEvent->getStausCode());
	}
	if (m_recognizeEvent != NULL)
	{
		SetEvent(m_recognizeEvent);
	}
}

void AliSpeechRecognize::OnRecognitionChannelCloseed(AlibabaNls::NlsEvent* cbEvent, void* cbParam)
{

}