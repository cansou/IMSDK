#ifndef ALI_SPEECH_RECOGNIZE_H
#define ALI_SPEECH_RECOGNIZE_H


#include "YouMeIMEngine/Speech/Windows/RecognizeInterface.h"
#include "ali/NlsEvent.h"
#include "ali/speechRecognizerRequest.h"


class AliSpeechRecognize : public IRecognizeInterface
{
public:
	AliSpeechRecognize();
	~AliSpeechRecognize();

	virtual int Init(const std::string& appkey, const std::string& secret) override;
	virtual int UnInit() override;
	virtual void SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize) override;
	virtual void SetRecognizeLanguage(RecognizeLanguage language) override;
	virtual bool AudioRecognize(const std::wstring& audioPath, std::wstring& text) override;
	virtual void UpdateToken(const std::string& token) override;

private:
	static void OnRecognitionStarted(AlibabaNls::NlsEvent* cbEvent, void* cbParam);
	static void OnRecognitionResultChanged(AlibabaNls::NlsEvent* cbEvent, void* cbParam);
	static void OnRecognitionCompleted(AlibabaNls::NlsEvent* cbEvent, void* cbParam);
	static void OnRecognitionTaskFailed(AlibabaNls::NlsEvent* cbEvent, void* cbParam);
	static void OnRecognitionChannelCloseed(AlibabaNls::NlsEvent* cbEvent, void* cbParam);
	unsigned int GetSendAudioSleepTime(const int dataSize, const int sampleRate, const int compressRate);

	int m_nFrameSize;
	int m_nSampleRate;
	AlibabaNls::SpeechRecognizerCallback* m_pCallback;
	AlibabaNls::SpeechRecognizerRequest* m_recognizeRequest;
	static std::string m_audioText;
	static HANDLE m_recognizeEvent;
};

#endif