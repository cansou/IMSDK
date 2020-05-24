#ifndef OSX_VOICE_MANAGER_H
#define OSX_VOICE_MANAGER_H


#include <string>
#include "YouMeIMEngine/Speech/VoiceInterface.h"


class OSXVoiceManager : public IYoueVoiceManager
{
public:
    OSXVoiceManager();
    
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
    
};


#endif
