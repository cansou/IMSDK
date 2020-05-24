#pragma once


#include <windows.h>
#include <mmsystem.h>
#include <string>
#include <mutex>
#include "YouMeIMEngine/Speech/VoiceInterface.h"


#define BUFFER_NUM 4

struct WAVHeadInfo
{
	// RIFF chunk
	char riffID[4];					//"RIFF"
	unsigned int fileSize;
	char riffFormat[4];				//"WAVE"
	// format chunk
	char fmtID[4];					//"fmt "
	unsigned int fmtSize;
	unsigned short formatTag;		//编码方式
	unsigned short channels;		//声道数
	unsigned int sampleFrequency;	//采样频率
	unsigned int byteRate;			//比特率
	unsigned short blockAlign;		//块对齐大小
	unsigned short sampleBitSize;	//样本宽度
	// data chunk
	char dataID[4];					//"data"
	unsigned int dataChunkSize;
};

enum AudioStatus
{
	AUDIOSTATUS_IDLE,
	AUDIOSTATUS_START_RECORD,
	AUDIOSTATUS_RECORDING,
	AUDIOSTATUS_STOP_RECORD,
	AUDIOSTATUS_CANCEL_RECORD,
	AUDIOSTATUS_RECORD_TIMEOUT,
	AUDIOSTATUS_START_PLAY,
	AUDIOSTATUS_PLAYING,
	AUDIOSTATUS_STOP_PLAY
};

class IAudioCallback
{
public:
	virtual void OnRecordFinish(AudioErrorCode errorcode, const std::wstring& path, unsigned long long serail) = 0;
	virtual void OnRecordData(unsigned char* buffer, unsigned int size) = 0;
	virtual void OnPlayFinish(AudioErrorCode errocode, const std::wstring& path) = 0;
	virtual void OnPlayData(unsigned char* buffer, unsigned int size) = 0;
};

class AudioRecordThread
{
public:
	static AudioRecordThread* Instance();
	int Init();
	void UnInit();
	void SetAudioCallback(IAudioCallback* callback){ m_pCallback = callback; }
	void SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize);
	AudioErrorCode StartRecord(std::wstring path, unsigned long long serial = 0);
	AudioErrorCode StopRecord(bool cancel = false);
	bool IsRedording(){ return AUDIOSTATUS_RECORDING == m_recordStatus || AUDIOSTATUS_START_RECORD == m_recordStatus; }
	bool IsPlaying(){ return AUDIOSTATUS_PLAYING == m_playStatus || AUDIOSTATUS_START_PLAY == m_playStatus; }
	AudioErrorCode PlayAudio(const std::wstring& path);
	AudioErrorCode StopPlayAudio();
	AudioDeviceState GetMicrophoneStatus();

private:
	AudioRecordThread();
	~AudioRecordThread();
	AudioRecordThread(const AudioRecordThread&);
	AudioRecordThread& operator= (const AudioRecordThread&);

	static unsigned __stdcall ThreadProc(void*);

	void OnMM_WIM_OPEN(WPARAM wParam, LPARAM lParam);
	void OnMM_WIM_DATA(WPARAM wParam, LPARAM lParam);
	void OnMM_WIM_CLOSE(WPARAM wParam, LPARAM lParam);
	void OnMM_WOM_OPEN(WPARAM wParam, LPARAM lParam);
	void OnMM_WOM_DONE(WPARAM wParam, LPARAM lParam);
	void OnMM_WOM_CLOSE(WPARAM wParam, LPARAM lParam);
	void OnStartRecord();
	void OnStopRecord();
	void OnPlayAudio();
	bool GetWAVHead(std::wstring path, WAVHeadInfo& head);
	bool HasAudioInputDevice();
	bool HasAudioOutputDevice();
	void OnError(MMRESULT result, int flag);
	void CallbackDone(AudioErrorCode errorcode, int flag);
	int OpenRecordDevice();
	int CloseRecordDevice();
	void DeleteRecordBuffer();
	int CreateAudiofile();

	HANDLE m_hThread;
	IAudioCallback* m_pCallback;
	static HANDLE m_hStartEvent;

	HWAVEIN m_hWaveInDev;
	WAVEFORMATEX m_waveFormat;
	WAVEHDR m_waveHead[BUFFER_NUM];
	FILE* m_pFile;
	std::wstring m_strAudioPath;
	DWORD m_ulRecordStartTime;
	unsigned long long m_ullSerail;
	volatile AudioStatus m_recordStatus;
	std::mutex m_recordMutex;
	bool m_bOpenRecordDevice;

	HWAVEOUT m_hWaveOutDev;
	WAVEHDR m_waveHeadOut;
	std::wstring m_strPlayPath;
	volatile AudioStatus m_playStatus;
	bool m_bInterruptPlay;
	std::mutex m_playMutex;
	HANDLE m_hPlayEvent;
};