#ifndef WIN_VOICE_MANAGER_H
#define WIN_VOICE_MANAGER_H


#include "YouMeIMEngine/Speech/VoiceInterface.h"
#include "YouMeIMEngine/Speech/Windows/AudioRecord.h"


class IRecognizeInterface;

class WinVoiceManager : public IYoueVoiceManager, public IAudioCallback
{
public:
	WinVoiceManager();
	~WinVoiceManager();

	virtual bool Init(const std::string& appkey, const std::string& secret, AudioRecognizeType audioRecognizeType) override;
	virtual void UnInit() override;
	virtual void SetAudioListener(IAudioListener* listener) override;

	virtual void SetAudioRecordCacheDir(const XString& path) override;
	virtual XString GetAudioRecordCacheDir() override;
	virtual void SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize) override;
	virtual int StartSpeech(XUINT64 serial) override;
	virtual int StartOnlySpeech(XUINT64 serial) override;
	virtual int StopSpeech() override;
	virtual int CancleSpeech() override;
	virtual void SetRecognizeLanguage(RecognizeLanguage language) override;
	virtual void SetKeepRecordModel(bool keep) override;
	virtual void UpdateToken(const std::string& token) override;

	virtual int StartPlayAudio(const XString& path) override;
	virtual int StopPlayAudio() override;
	virtual bool IsPlaying() override;
	virtual void SetPlayVolume(float volume) override;

	virtual int GetMicrophoneStatus() override;

private:
	virtual void OnRecordFinish(AudioErrorCode errorcode, const std::wstring& path, unsigned long long serail) override;
	virtual void OnPlayFinish(AudioErrorCode errocode, const std::wstring& path) override;
	virtual void OnRecordData(unsigned char* buffer, unsigned int size) override;
	virtual void OnPlayData(unsigned char* buffer, unsigned int size) override;

	IAudioListener* m_pAudioListener;
	IRecognizeInterface* m_pSpeechRecognize;
	std::wstring m_wstrRecordDir;
	bool m_bSpeechOnly;
	MCIDEVICEID m_mciDeviceID;
	int m_nSampleRate;
	AudioRecognizeType m_audioRecognizeType;
};

#endif