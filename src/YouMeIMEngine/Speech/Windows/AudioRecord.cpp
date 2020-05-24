#include "AudioRecord.h"
#include <process.h>


#define WM_STARTRECORD WM_USER + 20000
#define WM_STOPRECORD WM_USER + 20001
#define WM_PLAYAUDIO WM_USER + 20003

#define INP_BUFFER_SIZE 8192
#define FLAG_RECORD 0
#define FLAG_PLAY 1

HANDLE AudioRecordThread::m_hStartEvent;

AudioRecordThread::AudioRecordThread() : m_hThread(NULL)
, m_hWaveOutDev(NULL)
, m_bOpenRecordDevice(false)
, m_recordStatus(AUDIOSTATUS_IDLE)
, m_pFile(NULL)
, m_pCallback(NULL)
, m_playStatus(AUDIOSTATUS_IDLE)
, m_bInterruptPlay(false)
, m_hPlayEvent(NULL)
, m_ullSerail(0)
{

}

AudioRecordThread::~AudioRecordThread()
{
	UnInit();
}

AudioRecordThread* AudioRecordThread::Instance()
{
	static AudioRecordThread instance;
	return &instance;
}

int AudioRecordThread::Init()
{
	SetAudioRecordParam(SAMPLERATE_16K, CHANNEL_NUMBER, SAMPLE_BIT_SIZE);

	m_hStartEvent = ::CreateEvent(0, FALSE, FALSE, 0);
	if (m_hStartEvent == NULL)
	{
		return AUDIOERROR_OTHREERROR;
	}

	unsigned int threadID = 0;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, &AudioRecordThread::ThreadProc, NULL, 0, &threadID);
	if (m_hThread == 0)
	{
		CloseHandle(m_hStartEvent);
		m_hStartEvent = NULL;
		return AUDIOERROR_OTHREERROR;
	}

	::WaitForSingleObject(m_hStartEvent, INFINITE);
	CloseHandle(m_hStartEvent);

	return AUDIOERROR_SUCCESS;
}

void AudioRecordThread::UnInit()
{
	if (IsRedording())
	{
		StopRecord();
	}

	if (m_hThread != 0)
	{
		PostThreadMessage(GetThreadId(m_hThread), WM_QUIT, 0, 0);
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	if (m_pFile != NULL)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}

	CloseRecordDevice();

	if (m_waveHeadOut.lpData != NULL)
	{
		delete[] m_waveHeadOut.lpData;
		m_waveHeadOut.lpData = NULL;
	}
	if (m_hPlayEvent != NULL)
	{
		if (CloseHandle(m_hPlayEvent))
		{
			m_hPlayEvent = NULL;
		}
	}
}

void AudioRecordThread::SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize)
{
	m_waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	m_waveFormat.nChannels = channel;
	m_waveFormat.nSamplesPerSec = sampleRate;
	m_waveFormat.wBitsPerSample = sampleBitSize;
	m_waveFormat.nAvgBytesPerSec = m_waveFormat.nSamplesPerSec * m_waveFormat.nChannels * m_waveFormat.wBitsPerSample / 8;
	m_waveFormat.nBlockAlign = m_waveFormat.nChannels * m_waveFormat.wBitsPerSample / 8;
	m_waveFormat.cbSize = 0;
}

unsigned __stdcall AudioRecordThread::ThreadProc(void*)
{
	MSG msg;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	if (!SetEvent(m_hStartEvent))
	{
		return 1;
	}

	BOOL bRet = true;
	while ((bRet = GetMessage(&msg, 0, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			continue;
		}

		switch (msg.message)
		{
		case WM_STARTRECORD:
		{
			AudioRecordThread::Instance()->OnStartRecord();
		}
		break;
		case WM_STOPRECORD:
		{
			AudioRecordThread::Instance()->OnStopRecord();
		}
		break;
		case WM_PLAYAUDIO:
		{
			AudioRecordThread::Instance()->OnPlayAudio();
		}
		break;
		case MM_WIM_OPEN:
		{
			AudioRecordThread::Instance()->OnMM_WIM_OPEN(msg.wParam, msg.lParam);
		}
		break;
		case MM_WIM_DATA:
		{
			AudioRecordThread::Instance()->OnMM_WIM_DATA(msg.wParam, msg.lParam);
		}
		break;
		case MM_WIM_CLOSE:
		{
			AudioRecordThread::Instance()->OnMM_WIM_CLOSE(msg.wParam, msg.lParam);
		}
		break;
		case MM_WOM_OPEN:
		{
			AudioRecordThread::Instance()->OnMM_WOM_OPEN(msg.wParam, msg.lParam);
		}
		break;
		case MM_WOM_DONE:
		{
			AudioRecordThread::Instance()->OnMM_WOM_DONE(msg.wParam, msg.lParam);
		}
		break;
		case MM_WOM_CLOSE:
		{
			AudioRecordThread::Instance()->OnMM_WOM_CLOSE(msg.wParam, msg.lParam);
		}
		break;
		default:
			break;
		}
	}
	return 0;
}

AudioErrorCode AudioRecordThread::StartRecord(std::wstring path, unsigned long long serial)
{
	if (path.empty())
	{
		return AUDIOERROR_INVALID_PARAM;
	}
	if (!HasAudioInputDevice())
	{
		return AUDIOERROR_NO_AUDIO_DEVICE;
	}
	{
		std::lock_guard<std::mutex> lock(m_recordMutex);
		if (m_recordStatus != AUDIOSTATUS_IDLE)
		{
			return AUDIOERROR_RECORDING;
		}
		m_strAudioPath = path;
		m_ullSerail = serial;
		BOOL bRet = PostThreadMessage(GetThreadId(m_hThread), WM_STARTRECORD, 0, 0);
		if (!bRet)
		{
			int errorcode = GetLastError();
			return AUDIOERROR_OTHREERROR;
		}
		m_recordStatus = AUDIOSTATUS_START_RECORD;
	}

	return AUDIOERROR_SUCCESS;
}

AudioErrorCode AudioRecordThread::StopRecord(bool cancel)
{
	{
		std::lock_guard<std::mutex> lock(m_recordMutex);
		if (m_recordStatus != AUDIOSTATUS_RECORDING && m_recordStatus != AUDIOSTATUS_START_RECORD)
		{
			return AUDIOERROR_NOT_START_RECORD;
		}
		m_recordStatus = cancel ? AUDIOSTATUS_CANCEL_RECORD : AUDIOSTATUS_STOP_RECORD;
	}
	BOOL bRet = PostThreadMessage(GetThreadId(m_hThread), WM_STOPRECORD, 0, 0);
	if (!bRet)
	{
		return AUDIOERROR_OTHREERROR;
	}
	return AUDIOERROR_SUCCESS;
}

void AudioRecordThread::OnStartRecord()
{	
	if (!m_bOpenRecordDevice)
	{
		OpenRecordDevice();
	}

	for (int i = 0; i < BUFFER_NUM; ++i)
	{
		if ((WHDR_INQUEUE & m_waveHead[i].dwFlags) == 0)
		{
			m_waveHead[i].dwUser = i + 1;
			MMRESULT errorcode = waveInAddBuffer(m_hWaveInDev, &m_waveHead[i], sizeof(WAVEHDR));
			if (errorcode != MMSYSERR_NOERROR)
			{
				waveInReset(m_hWaveInDev);
				OnError(errorcode, FLAG_RECORD);
				return;
			}
		}
	}

	{
		std::lock_guard<std::mutex> lock(m_recordMutex);
		/*if (AUDIOSTATUS_STOP_RECORD == m_recordStatus || AUDIOSTATUS_CANCEL_RECORD == m_recordStatus)
		{
			m_recordStatus = AUDIOSTATUS_IDLE;
			return;
		}*/

		MMRESULT errorcode = waveInStart(m_hWaveInDev);
		if (errorcode != MMSYSERR_NOERROR)
		{
			waveInReset(m_hWaveInDev);
			OnError(errorcode, FLAG_RECORD);
			return;
		}
		m_recordStatus = AUDIOSTATUS_RECORDING;
	}
	m_ulRecordStartTime = GetTickCount();
}

void AudioRecordThread::OnStopRecord()
{
	waveInReset(m_hWaveInDev);
}

bool AudioRecordThread::HasAudioInputDevice()
{
	return waveInGetNumDevs() != 0;
}

bool AudioRecordThread::HasAudioOutputDevice()
{
	return waveOutGetNumDevs() != 0;
}

void AudioRecordThread::OnError(MMRESULT result, int flag)
{
	if (FLAG_RECORD == flag)
	{
		std::lock_guard<std::mutex> lock(m_recordMutex);
		m_recordStatus = AUDIOSTATUS_IDLE;
	}
	else
	{
		std::lock_guard<std::mutex> lock(m_playMutex);
		m_playStatus = AUDIOSTATUS_IDLE;
	}
	if (m_pCallback == NULL)
	{
		return;
	}
	AudioErrorCode errorcode = AUDIOERROR_OTHREERROR;
	if (MMSYSERR_BADDEVICEID == result || MMSYSERR_NODRIVER == result)
	{
		errorcode = AUDIOERROR_NO_AUDIO_DEVICE;
	}
	else if (WAVERR_BADFORMAT == result)
	{
		errorcode = AUDIOERROR_UNSUPPORT_FORMAT;
	}
	else if (MMSYSERR_INVALHANDLE == result)
	{
		errorcode = AUDIOERROR_DEVICE_STATUS_INVALID;
	}
	else if (MMSYSERR_NODRIVER == result)
	{
		errorcode = AUDIOERROR_NODRIVER;
	}

	if (FLAG_RECORD == flag)
	{
		m_pCallback->OnRecordFinish(errorcode, m_strAudioPath, m_ullSerail);
	}
	else if (FLAG_PLAY == flag)
	{
		m_pCallback->OnPlayFinish(errorcode, m_strPlayPath);
	}
}

void AudioRecordThread::CallbackDone(AudioErrorCode errorcode, int flag)
{
	if (m_pCallback == NULL)
	{
		return;
	}
	if (FLAG_RECORD == flag)
	{
		m_pCallback->OnRecordFinish(errorcode, m_strAudioPath, m_ullSerail);
	}
	else if (FLAG_PLAY == flag)
	{
		m_pCallback->OnPlayFinish(errorcode, m_strAudioPath);
	}
}

int AudioRecordThread::OpenRecordDevice()
{
	MMRESULT errorcode = waveInOpen(&m_hWaveInDev, WAVE_MAPPER, &m_waveFormat, GetThreadId(m_hThread), NULL, CALLBACK_THREAD);
	if (errorcode != MMSYSERR_NOERROR)
	{
		return -1;
	}
	for (int i = 0; i < BUFFER_NUM; ++i)
	{
		BYTE* buffer = new BYTE[INP_BUFFER_SIZE];
		m_waveHead[i].lpData = (LPSTR)buffer;
		m_waveHead[i].dwBufferLength = INP_BUFFER_SIZE;
		m_waveHead[i].dwBytesRecorded = 0;
		m_waveHead[i].dwUser = i + 1;
		m_waveHead[i].dwFlags = 0;
		m_waveHead[i].dwLoops = 1;
		m_waveHead[i].lpNext = NULL;
		m_waveHead[i].reserved = 0;
		errorcode = waveInPrepareHeader(m_hWaveInDev, &m_waveHead[i], sizeof(WAVEHDR));
		if (errorcode != MMSYSERR_NOERROR)
		{
			DeleteRecordBuffer();
			return -1;
		}
	}
	m_bOpenRecordDevice = true;
	return 0;
}

int AudioRecordThread::CloseRecordDevice()
{
	if (m_hWaveInDev != NULL)
	{
		waveInClose(m_hWaveInDev);
		DeleteRecordBuffer();
		m_hWaveInDev = NULL;
		m_bOpenRecordDevice = false;
	}
	return 0;
}

void AudioRecordThread::DeleteRecordBuffer()
{
	for (int i = 0; i < BUFFER_NUM; ++i)
	{
		if (m_waveHead[i].lpData == NULL)
		{
			continue;
		}
		if (WHDR_PREPARED & m_waveHead[i].dwFlags)
		{
			waveInUnprepareHeader(m_hWaveInDev, &m_waveHead[i], sizeof(WAVEHDR));
		}
		delete[] m_waveHead[i].lpData;
		m_waveHead[i].lpData = NULL;
	}
}

int AudioRecordThread::CreateAudiofile()
{
	if (m_strAudioPath.empty())
	{
		StopRecord();
		return -1;
	}
	if (m_pFile != NULL)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
	errno_t errCode = _wfopen_s(&m_pFile, m_strAudioPath.c_str(), L"wb+");
	if (errCode != 0 || NULL == m_pFile)
	{
		StopRecord();
		CallbackDone(AUDIOERROR_READWRITE_FILE_ERROR, FLAG_RECORD);
		return -1;
	}
	BYTE byteFill = 0X0;
	size_t stWrite = fwrite(&byteFill, sizeof(BYTE), sizeof(WAVHeadInfo), m_pFile);
	if (stWrite < sizeof(WAVHeadInfo))
	{
		fclose(m_pFile);
		m_pFile = NULL;
		StopRecord();

		CallbackDone(AUDIOERROR_READWRITE_FILE_ERROR, FLAG_RECORD);
	}
	return 0;
}

void AudioRecordThread::OnMM_WIM_OPEN(WPARAM wParam, LPARAM lParam)
{

}

void AudioRecordThread::OnMM_WIM_DATA(WPARAM wParam, LPARAM lParam)
{
	if (NULL == m_pFile)
	{
		if (AUDIOSTATUS_RECORDING == m_recordStatus)
		{
			if (CreateAudiofile() != 0)
			{
				return;
			}
		}
		else if (AUDIOSTATUS_STOP_RECORD == m_recordStatus || AUDIOSTATUS_CANCEL_RECORD == m_recordStatus)
		{
			if (AUDIOSTATUS_STOP_RECORD == m_recordStatus && GetTickCount() - m_ulRecordStartTime < SPEECH_DURATION_MIN * 1000)	//  Â¼ÒôÊ±³¤Ì«¶Ì
			{
				m_recordStatus = AUDIOSTATUS_IDLE;
				if (m_pCallback != NULL)
				{
					m_pCallback->OnRecordFinish(AUDIOERROR_RECORD_TIME_TOO_SHORT, m_strAudioPath, m_ullSerail);
				}
			}
			m_recordStatus = AUDIOSTATUS_IDLE;
		}
		else
		{
			return;
		}
	}
	if (m_pFile == NULL)
	{
		return;
	}
	
	PWAVEHDR pWavHeader = (PWAVEHDR)lParam;
	if (pWavHeader->dwBytesRecorded > 0)
	{
		fwrite(pWavHeader->lpData, sizeof(BYTE), pWavHeader->dwBytesRecorded, m_pFile);
	}

	if (pWavHeader->dwBytesRecorded > 0 && m_pCallback != NULL)
	{
		m_pCallback->OnRecordData((unsigned char*)((PWAVEHDR)lParam)->lpData, ((PWAVEHDR)lParam)->dwBytesRecorded);
	}
	
	if (AUDIOSTATUS_RECORDING == m_recordStatus)
	{
		waveInAddBuffer(m_hWaveInDev, pWavHeader, sizeof(WAVEHDR));

		if (GetTickCount() - m_ulRecordStartTime >= SPEECH_DURATION_MAX * 1000)	// ³¬Ê±
		{
			m_recordStatus = AUDIOSTATUS_RECORD_TIMEOUT;
			waveInReset(m_hWaveInDev);
		}
	}
	else
	{
		pWavHeader->dwUser = 0;

		bool isStoped = true;
		for (int i = 0; i < BUFFER_NUM; ++i)
		{
			if ((m_waveHead[i].dwFlags & WHDR_INQUEUE) || m_waveHead[i].dwUser != 0)
			{
				isStoped = false;
				break;
			}
		}
		if (!isStoped)
		{
			return;
		}

		if (AUDIOSTATUS_CANCEL_RECORD == m_recordStatus)
		{
			fclose(m_pFile);
			m_pFile = NULL;
			DeleteFileW(m_strAudioPath.c_str());
			m_recordStatus = AUDIOSTATUS_IDLE;
		}
		else if (AUDIOSTATUS_STOP_RECORD == m_recordStatus || AUDIOSTATUS_RECORD_TIMEOUT == m_recordStatus)
		{
			if (GetTickCount() - m_ulRecordStartTime < SPEECH_DURATION_MIN * 1000)	// Â¼ÒôÊ±³¤Ì«¶Ì
			{
				fclose(m_pFile);
				m_pFile = NULL;
				DeleteFileW(m_strAudioPath.c_str());

				m_recordStatus = AUDIOSTATUS_IDLE;

				if (m_pCallback != NULL)
				{
					m_pCallback->OnRecordFinish(AUDIOERROR_RECORD_TIME_TOO_SHORT, m_strAudioPath, m_ullSerail);
				}
			}
			else
			{
				fseek(m_pFile, 0, SEEK_END);
				long nFileSize = ftell(m_pFile);

				WAVHeadInfo headInfo = { { 'R', 'I', 'F', 'F' },
					nFileSize - 8,
					{ 'W', 'A', 'V', 'E' },
					{ 'f', 'm', 't', ' ' },
					16,
					0x0001,
					m_waveFormat.nChannels,
					m_waveFormat.nSamplesPerSec,
					m_waveFormat.nAvgBytesPerSec,
					m_waveFormat.nBlockAlign,
					m_waveFormat.wBitsPerSample,
					{ 'd', 'a', 't', 'a' },
					nFileSize - sizeof(WAVHeadInfo) };

				fseek(m_pFile, 0, SEEK_SET);
				size_t stWrite = fwrite(&headInfo, sizeof(headInfo), 1, m_pFile);

				fclose(m_pFile);
				m_pFile = NULL;

				AudioStatus lastStatus = m_recordStatus;
				m_recordStatus = AUDIOSTATUS_IDLE;

				if (m_pCallback != NULL)
				{
					m_pCallback->OnRecordFinish(AUDIOSTATUS_RECORD_TIMEOUT == lastStatus ? AUDIOERROR_RECORD_TIMEOUT : AUDIOERROR_SUCCESS, m_strAudioPath, m_ullSerail);
				}
			}
		}
		m_strAudioPath = L"";
	}
}

void AudioRecordThread::OnMM_WIM_CLOSE(WPARAM wParam, LPARAM lParam)
{
	CloseRecordDevice();
}

AudioErrorCode AudioRecordThread::PlayAudio(const std::wstring& path)
{
	if (path.empty())
	{
		return AUDIOERROR_INVALID_PARAM;
	}
	if (!HasAudioOutputDevice())
	{
		return AUDIOERROR_NO_AUDIO_DEVICE;
	}
	AudioStatus playStatus = AUDIOSTATUS_IDLE;
	{
		std::lock_guard<std::mutex> lock(m_playMutex);
		playStatus = m_playStatus;
	}
	AudioErrorCode errorcode = AUDIOERROR_SUCCESS;
	if (AUDIOSTATUS_PLAYING == playStatus || AUDIOSTATUS_STOP_PLAY == playStatus)
	{
		if (AUDIOSTATUS_PLAYING == playStatus)
		{
			errorcode = AUDIOERROR_PLAYING;
		}
		m_bInterruptPlay = true;
		if (m_hPlayEvent == NULL)
		{
			m_hPlayEvent = ::CreateEvent(0, FALSE, FALSE, 0);
		}
		StopPlayAudio();

		if (m_hPlayEvent != NULL)
		{
			::WaitForSingleObject(m_hPlayEvent, 3000);
		}
	}

	m_strPlayPath = path;
	{
		std::lock_guard<std::mutex> lock(m_playMutex);
		m_playStatus = AUDIOSTATUS_START_PLAY;
	}
	BOOL bRet = PostThreadMessage(GetThreadId(m_hThread), WM_PLAYAUDIO, 0, 0);
	if (!bRet)
	{
		{
			std::lock_guard<std::mutex> lock(m_playMutex);
			m_playStatus = AUDIOSTATUS_IDLE;
		}
		errorcode = AUDIOERROR_OTHREERROR;
	}
	return errorcode;
}

AudioErrorCode AudioRecordThread::StopPlayAudio()
{
	if (NULL == m_hWaveOutDev)
	{
		return AUDIOERROR_NOT_START_PLAY;
	}
	{
		std::lock_guard<std::mutex> lock(m_playMutex);
		if (m_playStatus != AUDIOSTATUS_PLAYING)
		{
			return AUDIOERROR_NOT_START_PLAY;
		}
		m_playStatus = AUDIOSTATUS_STOP_PLAY;
	}
	AudioErrorCode errorcode = AUDIOERROR_OTHREERROR;
	MMRESULT result = waveOutReset(m_hWaveOutDev);
	if (MMSYSERR_NOERROR == result)
	{
		errorcode = AUDIOERROR_SUCCESS;
	}
	else if (MMSYSERR_INVALHANDLE == result)
	{
		errorcode = AUDIOERROR_DEVICE_STATUS_INVALID;
	}
	else if (MMSYSERR_NODRIVER == result)
	{
		errorcode = AUDIOERROR_NODRIVER;
	}
	return errorcode;
}

AudioDeviceState AudioRecordThread::GetMicrophoneStatus()
{
	unsigned int nNum = waveInGetNumDevs();
	if (nNum == 0)
	{
		return AUDIOSTATUS_NOT_AVIAIBLE;
	}
	if (AUDIOSTATUS_RECORDING == m_recordStatus)
	{
		return AUDIOSTATUS_AVIAIBLE;
	}

	WAVEINCAPS cap;
	MMRESULT ret = waveInGetDevCaps(WAVE_MAPPER, &cap, sizeof(WAVEINCAPS));
	if (MMSYSERR_NODRIVER == ret)
	{
		return AUDIOSTATUS_NOT_AVIAIBLE;
	}
	/*else if (MMSYSERR_BADDEVICEID == ret)
	{
	return 1;
	}*/
	return AUDIOSTATUS_AVIAIBLE;
}

void AudioRecordThread::OnPlayAudio()
{
	WAVHeadInfo wavHead;
	if (!GetWAVHead(m_strPlayPath, wavHead))
	{
		CallbackDone(AUDIOERROR_RESOLVE_FILE_ERROR, FLAG_PLAY);
		return;
	}
	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = wavHead.formatTag;
	waveFormat.nChannels = wavHead.channels;
	waveFormat.nSamplesPerSec = wavHead.sampleFrequency;
	waveFormat.wBitsPerSample = wavHead.sampleBitSize;
	waveFormat.nAvgBytesPerSec = wavHead.byteRate;
	waveFormat.nBlockAlign = wavHead.blockAlign;
	waveFormat.cbSize = 0;
	MMRESULT errorcode = waveOutOpen(&m_hWaveOutDev, WAVE_MAPPER, &waveFormat, GetThreadId(m_hThread), NULL, CALLBACK_THREAD);
	{
		std::lock_guard<std::mutex> lock(m_playMutex);
		m_playStatus = errorcode == MMSYSERR_NOERROR ? AUDIOSTATUS_PLAYING : AUDIOSTATUS_IDLE;
	}
	if (errorcode != MMSYSERR_NOERROR)
	{
		OnError(errorcode, FLAG_PLAY);
	}
}

bool AudioRecordThread::GetWAVHead(std::wstring path, WAVHeadInfo& head)
{
	FILE* pFile = NULL;
	errno_t errCode = _wfopen_s(&pFile, path.c_str(), L"rb");
	if (errCode != 0 || NULL == pFile)
	{
		return false;
	}
	fseek(pFile, 20, SEEK_SET);

	fread(&head.formatTag, sizeof(unsigned short), 1, pFile);
	fread(&head.channels, sizeof(unsigned short), 1, pFile);
	fread(&head.sampleFrequency, sizeof(unsigned int), 1, pFile);
	fread(&head.byteRate, sizeof(unsigned int), 1, pFile);
	fread(&head.blockAlign, sizeof(unsigned short), 1, pFile);
	fread(&head.sampleBitSize, sizeof(unsigned short), 1, pFile);

	fclose(pFile);
	return true;
}

void AudioRecordThread::OnMM_WOM_OPEN(WPARAM wParam, LPARAM lParam)
{
	FILE* pFile = NULL;
	errno_t errCode = _wfopen_s(&pFile, m_strPlayPath.c_str(), L"rb");
	if (errCode != 0 || NULL == pFile)
	{
		CallbackDone(AUDIOERROR_READWRITE_FILE_ERROR, FLAG_PLAY);
		return;
	}

	fseek(pFile, 0, SEEK_END);
	long lAudioFileSize = ftell(pFile);
	if (lAudioFileSize <= sizeof(WAVHeadInfo))
	{
		fclose(pFile);
		return;
	}
	lAudioFileSize -= sizeof(WAVHeadInfo);
	fseek(pFile, sizeof(WAVHeadInfo), SEEK_SET);

	char* szAudioData = new char[lAudioFileSize];
	if (NULL == szAudioData)
	{
		fclose(pFile);
		return;
	}
	if (fread((void *)szAudioData, 1, lAudioFileSize, pFile) != lAudioFileSize)
	{
		fclose(pFile);

		CallbackDone(AUDIOERROR_READWRITE_FILE_ERROR, FLAG_PLAY);
		delete[] szAudioData;
		return;
	}
	fclose(pFile);

	m_waveHeadOut.lpData = szAudioData;
	m_waveHeadOut.dwBufferLength = lAudioFileSize;
	m_waveHeadOut.dwBytesRecorded = 0;
	m_waveHeadOut.dwUser = 0;
	m_waveHeadOut.dwFlags = WHDR_PREPARED | WHDR_BEGINLOOP | WHDR_ENDLOOP;
	m_waveHeadOut.dwLoops = 1;
	m_waveHeadOut.lpNext = NULL;
	m_waveHeadOut.reserved = 0;

	MMRESULT result = waveOutPrepareHeader(m_hWaveOutDev, &m_waveHeadOut, sizeof(WAVEHDR));
	if (result != MMSYSERR_NOERROR)
	{
		OnError(result, FLAG_PLAY);
	}
	result = waveOutWrite(m_hWaveOutDev, &m_waveHeadOut, sizeof(WAVEHDR));
	if (result != MMSYSERR_NOERROR)
	{
		OnError(result, FLAG_PLAY);
	}
}

void AudioRecordThread::OnMM_WOM_DONE(WPARAM wParam, LPARAM lParam)
{
	MMRESULT result = waveOutUnprepareHeader(m_hWaveOutDev, &m_waveHeadOut, sizeof(WAVEHDR));
	result = waveOutClose(m_hWaveOutDev);
}

void AudioRecordThread::OnMM_WOM_CLOSE(WPARAM wParam, LPARAM lParam)
{
	if (m_waveHeadOut.lpData != NULL)
	{
		delete[] m_waveHeadOut.lpData;
		m_waveHeadOut.lpData = NULL;
	}

	{
		std::lock_guard<std::mutex> lock(m_playMutex);
		m_playStatus = AUDIOSTATUS_IDLE;
	}
	if (m_bInterruptPlay)
	{
		m_bInterruptPlay = false;
		if (m_hPlayEvent != NULL)
		{
			SetEvent(m_hPlayEvent);
		}
	}
	else
	{
		if (m_pCallback != NULL)
		{
			m_pCallback->OnPlayFinish(AUDIOERROR_SUCCESS, m_strPlayPath);
		}
	}
}