#pragma once


#include <string>
#include "YouMeIMEngine/Speech/VoiceInterface.h"


class IRecognizeInterface
{
public:
	virtual ~IRecognizeInterface(){};
	virtual int Init(const std::string& appkey, const std::string& secret) = 0;
	virtual int UnInit() = 0;
	virtual void SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize) = 0;
	virtual void SetRecognizeLanguage(RecognizeLanguage language) = 0;
	virtual bool AudioRecognize(const std::wstring& audioPath, std::wstring& text) = 0;
	virtual void UpdateToken(const std::string& token){}
	virtual bool IsSupportStreamAudio(){ return false; };
	virtual bool StartStreamAudioRecognize() { return true;  };
	virtual bool StopStreamAudioRecognize(std::wstring& text){ return true; };
	virtual bool CancelStreamAudioRecognize(){ return true;  };
	virtual bool InputAudioData(char* data, int data_size, bool isLast){ return true;  };
};

