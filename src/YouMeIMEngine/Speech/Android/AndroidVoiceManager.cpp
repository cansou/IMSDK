#include "AndroidVoiceManager.h"


extern void YouMe_SetAndroidSpeechListen(IAudioListener* pListen);

extern void YouMe_SetAudioRecordCacheDir(const char* path);
extern std::string YouMe_GetAudioRecordCacheDir();
extern void YouMe_SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize);
extern void YouMe_InitSpeechRecognizer(const char* appkey, const char* secret, int audioRecognizeType);
extern void Youme_SetRecognizeLanguage(int language);
extern int YouMe_StartSpeech(XUINT64 serial);
extern int YouMe_StartOnlySpeech(XUINT64 serial);
extern int YouMe_StopSpeech();
extern int YouMe_CancleSpeech();
extern void YouMe_UnInit();
extern void Youme_UpdateToken(const char* token);

extern void YouMe_SetPlayVolume(float volume);
extern int YouMe_StartPlayAudio(const char* pszPath);
extern int YouMe_StopPlayAudio();
extern bool YouMe_IsPlaying();
extern int Youme_GetMicrophoneStatus();


bool AndroidVoiceManager::Init(const std::string& appkey, const std::string& secret, AudioRecognizeType audioRecognizeType)
{
	YouMe_InitSpeechRecognizer(appkey.c_str(), secret.c_str(), audioRecognizeType);
	return true;
}

void AndroidVoiceManager::UnInit()
{
	YouMe_UnInit();
}

void AndroidVoiceManager::SetAudioListener(IAudioListener* listener)
{
	YouMe_SetAndroidSpeechListen(listener);
}

void AndroidVoiceManager::SetAudioRecordCacheDir(const XString& path)
{
	YouMe_SetAudioRecordCacheDir(XStringToUTF8(path).c_str());
}

XString AndroidVoiceManager::GetAudioRecordCacheDir()
{
	return YouMe_GetAudioRecordCacheDir();
}

void AndroidVoiceManager::SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize)
{
	YouMe_SetAudioRecordParam(sampleRate, channel, sampleBitSize);
}

int AndroidVoiceManager::StartSpeech(XUINT64 serial)
{
	return YouMe_StartSpeech(serial);
}

int AndroidVoiceManager::StartOnlySpeech(XUINT64 serial)
{
	return YouMe_StartOnlySpeech(serial);
}

int AndroidVoiceManager::StopSpeech()
{
	return YouMe_StopSpeech();
}

int AndroidVoiceManager::CancleSpeech()
{
	return YouMe_CancleSpeech();
}

void AndroidVoiceManager::SetRecognizeLanguage(RecognizeLanguage language)
{
	Youme_SetRecognizeLanguage(language);
}

void AndroidVoiceManager::SetKeepRecordModel(bool keep)
{

}

void AndroidVoiceManager::UpdateToken(const std::string& token)
{
	Youme_UpdateToken(token.c_str());
}

int AndroidVoiceManager::StartPlayAudio(const XString& path)
{
	return YouMe_StartPlayAudio(XStringToUTF8(path).c_str());
}

int AndroidVoiceManager::StopPlayAudio()
{
	return YouMe_StopPlayAudio();
}

bool AndroidVoiceManager::IsPlaying()
{
	return YouMe_IsPlaying();
}

void AndroidVoiceManager::SetPlayVolume(float volume)
{
	YouMe_SetPlayVolume(volume);
}

int AndroidVoiceManager::GetMicrophoneStatus()
{
	return Youme_GetMicrophoneStatus();
}