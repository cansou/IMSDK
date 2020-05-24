#ifndef YOUME_SPEECH_MANAGER_H
#define YOUME_SPEECH_MANAGER_H


#include "YouMeIMEngine/Speech/VoiceInterface.h"
#include "YouMeIMEngine/YIM.h"
#include "YouMeIMEngine/SimpleComplexConvert.h"


class YouMeIMManager;
class AudioResample;
class YouMeIMMessageManager;
struct ServerPacket;

class CYouMeSpeechManager : public IAudioListener
{
public:
	CYouMeSpeechManager();
	~CYouMeSpeechManager();
   
	bool Init();
	void UnInit();
	void SetIMManager(YouMeIMManager* immanager){ m_pIManager = immanager; }
	void SetMessageManager(YouMeIMMessageManager* messageManager){ m_pMessageManager = messageManager; }
	void SetAudioPlayCallback(IYIMAudioPlayCallback* callback){ m_pAudioPlayCallback = callback; };

	YIMErrorcode SetSampleRate(int sampleRate);
	static void SetAudioRecordCacheDir(const XCHAR* path);
	void SetAudioRecordCacheDir(const XString& path);
	XString GetAudioRecordCacheDir();
	YIMErrorcode SetSpeechRecognizeLanguage(SpeechLanguage language);
	void SetKeepRecordModel(bool keep);
	YIMErrorcode StartSpeech(XUINT64 serial, bool speechRecognize);
	YIMErrorcode StopSpeech();
	YIMErrorcode CancleSpeech();

	void SetPlayVolume(float volume);
	YIMErrorcode StartPlayAudio(const XCHAR* path);
	YIMErrorcode StopPlayAudio();
	bool IsPlaying();
	int GetMicrophoneStatus();

	XUINT64 GetAliTokenSerial(){ return m_ullAliTokenSerial; }
	void ReqAliAccessToken();
	void OnAliAccessTokenRsp(ServerPacket& serverPacket);

private:
	virtual void OnRecordFinish(int errorcode, const XString& path, unsigned long long serial, const XString& text) override;
	virtual void OnAudioRecordData(unsigned char* buffer, unsigned int size) override;
	virtual void OnAudioPlayData(unsigned char* buffer, unsigned int size) override;
	virtual void OnPlayFinish(int errrocode, const XString& path) override;
	virtual void OnRecordVolumeChange(int volume, int type) override;
	virtual unsigned char* ResampleAudioData(unsigned char* inBuffer, unsigned int inSize, unsigned int* outSize) override;

	float CalculateVolume(byte* buffer, unsigned int size);
	void GetMicrophoneStatusThread();
	YIMErrorcode GetErrorcode(int audioErrorcode);
	int InitResample(int channels, int inSampleRate, int outSampleRate);
	int DestroyResample();
    void ReportSpeechRecoginzeStatus(int status, short recognizeType);

	static XString m_strAudioCacheDir;

	IYoueVoiceManager* m_pVoiceManager;
	YouMeIMManager* m_pIManager;
	YouMeIMMessageManager* m_pMessageManager;
	IYIMAudioPlayCallback* m_pAudioPlayCallback;
	int m_iSpeechRecognizeFailedCount;	//语音识别失败次数（连续三次关闭识别）
	int m_iSampleRate;
	AudioRecognizeType m_recognizeType;
	AudioResample* m_pAudioResample;
	std::string m_strSpeechAccent;
	std::string m_strSpeechLanguage;
	SimpleComplexConvert m_fontConverter;
	SpeechLanguage m_speechLanguage;
	static XUINT64 m_ullAliTokenSerial;
};

#endif
