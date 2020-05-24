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
	m_pCallback->setOnRecognitionStarted(AliSpeechRecognize::OnRecognitionStarted, NULL);	// start()成功回调
	m_pCallback->setOnTaskFailed(OnRecognitionTaskFailed, NULL);							// 异常识别回调
	m_pCallback->setOnChannelClosed(OnRecognitionChannelCloseed, NULL);						// 识别通道关闭回调
	m_pCallback->setOnRecognitionResultChanged(OnRecognitionResultChanged, NULL);			// 中间结果回调
	m_pCallback->setOnRecognitionCompleted(OnRecognitionCompleted, NULL);					// 设置识别结束回调


	/*m_recognizeRequest = AlibabaNls::NlsClient::getInstance()->createRecognizerRequest(m_pCallback);
	if (NULL == m_recognizeRequest)
	{
		return AUDIOERROR_INIT_FAILED;
	}
	//m_recognizeRequest->setUrl("wss://nls-gateway.cn-shanghai.aliyuncs.com/ws/v1");		// 设置服务端url, 必填参数
	m_recognizeRequest->setAppKey(appkey.c_str());
	m_recognizeRequest->setToken("a3b754471d1f447fbfcbcadd9a0f7e89");
	m_recognizeRequest->setFormat("pcm");												// 设置音频数据编码格式, 可选参数, 目前支持pcm, opus, opu. 默认是pcm
	m_recognizeRequest->setSampleRate(m_nSampleRate);									// 设置音频数据采样率, 可选参数, 目前支持16000, 8000. 默认是16000
	m_recognizeRequest->setIntermediateResult("false");									// 设置是否返回中间识别结果, 可选参数. 默认false
	m_recognizeRequest->setPunctuationPrediction("false");								// 设置是否在后处理中添加标点, 可选参数. 默认false
	m_recognizeRequest->setInverseTextNormalization("false");							// 设置是否在后处理中执行ITN, 可选参数. 默认false
	m_recognizeRequest->setPunctuationPrediction("true");								// 是否在后处理中添加标点, 可选参数. 默认false*/

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
	m_recognizeRequest->setFormat("pcm");												// 设置音频数据编码格式, 可选参数, 目前支持pcm, opus, opu. 默认是pcm
	m_recognizeRequest->setSampleRate(m_nSampleRate);									// 设置音频数据采样率, 可选参数, 目前支持16000, 8000. 默认是16000
	m_recognizeRequest->setIntermediateResult("false");									// 设置是否返回中间识别结果, 可选参数. 默认false
	m_recognizeRequest->setPunctuationPrediction("false");								// 设置是否在后处理中添加标点, 可选参数. 默认false
	m_recognizeRequest->setInverseTextNormalization("false");							// 设置是否在后处理中执行ITN, 可选参数. 默认false
	m_recognizeRequest->setPunctuationPrediction("true");								// 是否在后处理中添加标点, 可选参数. 默认false

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
		int nSize = 10 * m_nFrameSize; // 每次写入200ms音频(16k，16bit)：1帧音频20ms，10帧=200ms。16k采样率的16位音频，一帧的大小为640Byte
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
			// 发送失败
			YouMe_LOG_Error(__XT("recognize error"));
			bRet = false;
			goto RECOGNIZE_EXIT;
		}

		lSendSize += (long)nSize;
		lAudioFileSize -= (long)nSize;

		// 语音数据是实时的, 不用sleep控制速率, 直接发送即可.
		// 语音数据来自文件, 发送时需要控制速率, 使单位时间内发送的数据大小接近单位时间原始语音数据存储的大小.
		//unsigned sleepMs = GetSendAudioSleepTime(nSize, m_nSampleRate, 1);
		//Sleep(sleepMs);
		Sleep(30);
	}
	// 停止语音输入
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
* @brief 获取sendAudio发送延时时间
* @param dataSize 待发送数据大小
* @param sampleRate 采样率 16k/8K
* @param compressRate 数据压缩率，例如压缩比为10:1的16k opus编码，此时为10；非压缩数据则为1
* @return 返回sendAudio之后需要sleep的时间
* @note 对于8k pcm 编码数据, 16位采样，建议每发送3200字节 sleep 200 ms.
对于16k pcm 编码数据, 16位采样，建议每发送6400字节 sleep 200 ms.
对于其它编码格式的数据, 用户根据压缩比, 自行估算, 比如压缩比为10:1的 16k opus,需要每发送6400/10=640 sleep 200ms.
*/
unsigned int AliSpeechRecognize::GetSendAudioSleepTime(const int dataSize, const int sampleRate, const int compressRate)
{
	const int sampleBytes = 16;	// 仅支持16位采样
	const int soundChannel = 1;	// 仅支持单通道
	int bytes = (sampleRate * sampleBytes * soundChannel) / 8;	// 当前采样率，采样位数下每秒采样数据的大小
	int bytesMs = bytes / 1000;									// 当前采样率，采样位数下每毫秒采样数据的大小
	int sleepMs = (dataSize * compressRate) / bytesMs;			// 待发送数据大小除以每毫秒采样数据大小，以获取sleep时间
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