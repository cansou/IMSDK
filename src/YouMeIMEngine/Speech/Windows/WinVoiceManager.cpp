#include "WinVoiceManager.h"
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeCommon/XFile.h>
#include <YouMeCommon/Log.h>
#include "YouMeIMEngine/Speech/Windows/IflySpeechRecognize.h"
#include "YouMeIMEngine/Speech/Windows/UscSpeechRecognize.h"
#include "YouMeIMEngine/Speech/Windows/AliSpeechRecognize.h"
#include "YouMeIMEngine/Speech/AudioResample.h"


WinVoiceManager::WinVoiceManager() : m_bSpeechOnly(false)
, m_pAudioListener(NULL)
, m_pSpeechRecognize(NULL)
, m_nSampleRate(SAMPLERATE_16K)
{

}

WinVoiceManager::~WinVoiceManager()
{
	
}

bool WinVoiceManager::Init(const std::string& appkey, const std::string& secret, AudioRecognizeType audioRecognizeType)
{
	if (AudioRecordThread::Instance()->Init() != AUDIOERROR_SUCCESS)
	{
		return false;
	}

	if (NULL == m_pSpeechRecognize)
	{
		#if 0
		if (RECOGNIZETYPE_IFLY_RECORD == audioRecognizeType || RECOGNIZETYPE_IFLY_STREAM == audioRecognizeType)
		{
			m_pSpeechRecognize = new IflySpeechRecognize;
		}
		else 
		#endif
		if (RECOGNIZETYPE_USC == audioRecognizeType)
		{
			m_pSpeechRecognize = new UscSpeechRecognize;
		}
		else if (RECOGNIZETYPE_ALI == audioRecognizeType)
		{
			m_pSpeechRecognize = new AliSpeechRecognize;
		}
	}

	if (m_pSpeechRecognize != NULL)
	{
		m_pSpeechRecognize->Init(appkey, secret);
	}

	m_audioRecognizeType = audioRecognizeType;
	SetAudioRecordCacheDir(m_wstrRecordDir);
	AudioRecordThread::Instance()->SetAudioCallback(this);

	return true;
}

void WinVoiceManager::UnInit()
{
	AudioRecordThread::Instance()->UnInit();
	if (m_pSpeechRecognize != NULL)
	{
		m_pSpeechRecognize->UnInit();
		delete m_pSpeechRecognize;
		m_pSpeechRecognize = NULL;
	}
}

void WinVoiceManager::SetAudioListener(IAudioListener* listener)
{
	m_pAudioListener = listener;
}

void WinVoiceManager::SetAudioRecordCacheDir(const XString& path)
{
	if (path.empty())
	{
		wchar_t wszPath[MAX_PATH] = { 0 };
		if (GetTempPathW(MAX_PATH, wszPath) > 0)
		{
			m_wstrRecordDir = std::wstring(wszPath);
			m_wstrRecordDir.append(L"record\\");
			if (GetFileAttributesW(m_wstrRecordDir.c_str()) != FILE_ATTRIBUTE_DIRECTORY)
			{
				CreateDirectoryW(m_wstrRecordDir.c_str(), NULL);
			}
		}
	}
	else
	{
		if (GetFileAttributesW(m_wstrRecordDir.c_str()) == FILE_ATTRIBUTE_DIRECTORY)
		{
			m_wstrRecordDir = path;
		}
		else
		{
			if (youmecommon::CXFile::make_dir_tree(path.c_str()))
			{
				m_wstrRecordDir = path;
			}
		}
	}
}

XString WinVoiceManager::GetAudioRecordCacheDir()
{
	return m_wstrRecordDir;
}

void WinVoiceManager::SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize)
{
	m_nSampleRate = sampleRate;
	AudioRecordThread::Instance()->SetAudioRecordParam(sampleRate, channel, sampleBitSize);
	if (m_pSpeechRecognize != NULL)
	{
		m_pSpeechRecognize->SetAudioRecordParam(sampleRate, channel, sampleBitSize);
	}
}

void WinVoiceManager::SetRecognizeLanguage(RecognizeLanguage language)
{
	if (m_pSpeechRecognize != NULL)
	{
		m_pSpeechRecognize->SetRecognizeLanguage(language);
	}
}

int WinVoiceManager::StartSpeech(XUINT64 serial)
{	
	if (AudioRecordThread::Instance()->IsRedording())
	{
		return AUDIOERROR_RECORDING;
	}
	if (m_wstrRecordDir.empty())
	{
		return AUDIOERROR_INIT_FAILED;
	}
	srand(time(NULL));
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	wchar_t wszTemp[32] = { 0 };
	swprintf_s(wszTemp, L"%04d%02d%02d%02d%02d%02d%02d.wav", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, rand() % 100);
	std::wstring path = m_wstrRecordDir;
	path.append(wszTemp);

	AudioErrorCode ret = AudioRecordThread::Instance()->StartRecord(path, serial);
	if (ret == AUDIOERROR_SUCCESS && m_pSpeechRecognize)
	{
		m_pSpeechRecognize->StartStreamAudioRecognize();
	}

	return ret;
}

int WinVoiceManager::StartOnlySpeech(XUINT64 serial)
{
	if (AudioRecordThread::Instance()->IsRedording() || m_wstrRecordDir.empty())
	{
		return -1;
	}

	srand(time(NULL));
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	wchar_t wszTemp[32] = { 0 };
	swprintf_s(wszTemp, L"%04d%02d%02d%02d%02d%02d%02d.wav", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, rand() % 100);
	std::wstring path = m_wstrRecordDir;
	path.append(wszTemp);

	m_bSpeechOnly = true;

	AudioErrorCode ret = AudioRecordThread::Instance()->StartRecord(path, serial);

	return ret;
}

int WinVoiceManager::StopSpeech()
{
	return AudioRecordThread::Instance()->StopRecord();
}

int WinVoiceManager::CancleSpeech()
{
	//cancle�ղ���onRecordFinish��
	if (m_pSpeechRecognize && !m_bSpeechOnly)
	{
		m_pSpeechRecognize->CancelStreamAudioRecognize();
	}

	return AudioRecordThread::Instance()->StopRecord(true);
}

void WinVoiceManager::SetKeepRecordModel(bool keep)
{

}

void WinVoiceManager::UpdateToken(const std::string& token)
{
	if (m_pSpeechRecognize != NULL)
	{
		m_pSpeechRecognize->UpdateToken(token);
	}
}

void WinVoiceManager::SetPlayVolume(float volume)
{

}

int WinVoiceManager::StartPlayAudio(const XString& path)
{
	return AudioRecordThread::Instance()->PlayAudio(path);
}

int WinVoiceManager::StopPlayAudio()
{
	return AudioRecordThread::Instance()->StopPlayAudio();
}

bool WinVoiceManager::IsPlaying()
{
	return AudioRecordThread::Instance()->IsPlaying();
}

int WinVoiceManager::GetMicrophoneStatus()
{
	return AudioRecordThread::Instance()->GetMicrophoneStatus();
}

void WinVoiceManager::OnRecordFinish(AudioErrorCode errorcode, const std::wstring& path, unsigned long long serail)
{
	if (NULL == m_pAudioListener)
	{
		return;
	}

	AudioErrorCode retCode = errorcode;
	XString audioText;
	if ((AUDIOERROR_SUCCESS == errorcode || AUDIOERROR_RECORD_TIMEOUT == errorcode) && !m_bSpeechOnly && m_pSpeechRecognize )
	{
		YouMe_LOG_Error(__XT("OnRecordFinish begin"));

		if (m_pSpeechRecognize->IsSupportStreamAudio())
		{
			m_pSpeechRecognize->StopStreamAudioRecognize(audioText);
		}
		else
		{
			bool needDelete = false;
			XString destPath(path);
			// �߲������ز�����16K��ʶ��
			if ((RECOGNIZETYPE_IFLY_STREAM == m_audioRecognizeType && m_nSampleRate != SAMPLERATE_16K)
				|| (RECOGNIZETYPE_USC == m_audioRecognizeType && m_nSampleRate != SAMPLERATE_16K && m_nSampleRate != SAMPLERATE_8K)
				|| (RECOGNIZETYPE_ALI == m_audioRecognizeType && m_nSampleRate != SAMPLERATE_16K))
			{
				needDelete = true;
				destPath += __XT(".data");
				AudioResample resample;
				int ret = resample.InitResample(1, m_nSampleRate, SAMPLERATE_16K);
				if (ret != 0)
				{
					m_pAudioListener->OnRecordFinish(AUDIOERROR_RESAMPLE_FAILED, path, serail, audioText);
					YouMe_LOG_Error(__XT("resample init failed"));
					return;
				}
				ret = resample.ResampleFile(1, 16, m_nSampleRate, path, SAMPLERATE_16K, destPath);
				if (ret != 0)
				{
					m_pAudioListener->OnRecordFinish(AUDIOERROR_RESAMPLE_FAILED, path, serail, audioText);
					YouMe_LOG_Error(__XT("resample failed"));
					return;
				}
				ret = resample.DestroyResample();
				//YouMe_LOG_Debug(__XT("resample %lu"), GetTickCount() - begin);
			}

			if (m_pSpeechRecognize != NULL)
			{
				bool bRet = m_pSpeechRecognize->AudioRecognize(destPath, audioText);
				if (!bRet)
				{
					retCode = AUDIOERROR_RECOGNIZE_FAILED;
				}
			}

			if (needDelete)
			{
				youmecommon::CXFile::remove_file(destPath);
			}
		}

		YouMe_LOG_Error(__XT("OnRecordFinish end"));
	}
	m_pAudioListener->OnRecordFinish(retCode, path, serail, audioText);

	m_bSpeechOnly = false;
}

void WinVoiceManager::OnPlayFinish(AudioErrorCode errocode, const std::wstring& path)
{
	if (NULL == m_pAudioListener)
	{
		return;
	}
	m_pAudioListener->OnPlayFinish(errocode, path);
}

void WinVoiceManager::OnRecordData(unsigned char* buffer, unsigned int size)
{
	if (NULL == m_pAudioListener)
	{
		return;
	}
	m_pAudioListener->OnAudioRecordData(buffer, size);
	unsigned char* audioData = buffer;
	unsigned int audioSize = size;

	if ((RECOGNIZETYPE_IFLY_STREAM == m_audioRecognizeType && m_nSampleRate != SAMPLERATE_16K)
		|| (RECOGNIZETYPE_USC == m_audioRecognizeType && m_nSampleRate != SAMPLERATE_16K && m_nSampleRate != SAMPLERATE_8K)
		|| (RECOGNIZETYPE_ALI == m_audioRecognizeType && m_nSampleRate != SAMPLERATE_16K))
	{
		audioData = m_pAudioListener->ResampleAudioData(buffer, size, &audioSize);
		if (audioData == NULL || audioSize == 0)
		{
			YouMe_LOG_Error(__XT("resample error "));
			return;
		}
	}

	if (m_pSpeechRecognize != NULL && !m_bSpeechOnly)
	{
		bool bRet = m_pSpeechRecognize->InputAudioData((char*)audioData, audioSize, false);
		if (!bRet)
		{
			AudioErrorCode retCode = AUDIOERROR_RECOGNIZE_FAILED;
		}
	}
}

void WinVoiceManager::OnPlayData(unsigned char* buffer, unsigned int size)
{
	if (NULL == m_pAudioListener)
	{
		return;
	}
	m_pAudioListener->OnAudioPlayData(buffer, size);
}