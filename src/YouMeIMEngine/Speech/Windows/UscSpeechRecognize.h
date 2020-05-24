#ifndef USC_SPEECH_RECOGNIZE_H
#define USC_SPEECH_RECOGNIZE_H


#include "YouMeIMEngine/Speech/Windows/RecognizeInterface.h"
#include "yzs/libusc.h"


class UscSpeechRecognize : public IRecognizeInterface
{
public:
	UscSpeechRecognize();
	~UscSpeechRecognize();

	virtual int Init(const std::string& appkey, const std::string& secret) override;
	virtual int UnInit() override;
	virtual void SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize) override;
	virtual void SetRecognizeLanguage(RecognizeLanguage language) override;
	virtual bool AudioRecognize(const std::wstring& audioPath, std::wstring& text) override;

	virtual bool IsSupportStreamAudio()override;
	virtual bool StartStreamAudioRecognize() override;
	virtual bool StopStreamAudioRecognize(std::wstring& text )override;
	virtual bool CancelStreamAudioRecognize()override;
	virtual bool InputAudioData(char* data, int data_size, bool isLast)override;

private:
	USC_HANDLE m_uscHandle;
	int m_nFrameSize;
	int m_nSampleRate;
	std::string m_curAudioText;
};

#endif