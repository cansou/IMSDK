#ifndef IFLY_SPEECH_RECOGNIZE_H
#define IFLY_SPEECH_RECOGNIZE_H

#include "YouMeIMEngine/Speech/Windows/RecognizeInterface.h"


class IflySpeechRecognize : public IRecognizeInterface
{
public:
	IflySpeechRecognize();
	~IflySpeechRecognize();

	virtual int Init(const std::string& appkey, const std::string& secret) override;
	virtual int UnInit() override;
	virtual void SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize) override;
	virtual void SetRecognizeLanguage(RecognizeLanguage language) override;
	virtual bool AudioRecognize(const std::wstring& audioPath, std::wstring& text) override;

private:
	void SetRecognizeSession(const std::string& accent, const std::string& language);

	bool m_bAudioConvertInit;
	int m_nSampleRate;
	//int m_nSampleBitSize;
	int m_nFrameSize;
	std::string m_strSessionParam;
};

#endif