#include "OSXVoiceManager.h"
#include <YouMeIMEngine/Speech/OSX/RecognizeManager.h>
#include <YouMeIMEngine/Speech/OSX/AudioPlayer.h>
#include <AppKit/AppKit.h>
#include "YouMeCommon/Log.h"


OSXVoiceManager::OSXVoiceManager()
{
    
}

bool OSXVoiceManager::Init(const std::string& appkey, const std::string& secret, AudioRecognizeType audioRecognizeType)
{
    NSString* strAppkey = [NSString stringWithUTF8String:appkey.c_str()];
    NSString* strSecret = [NSString stringWithUTF8String:secret.c_str()];
   
    [[RecognizeManager Instance] InitRecognizer:strAppkey secret:strSecret recognizeType:audioRecognizeType];
    return true;
}

void OSXVoiceManager::UnInit()
{
    [[RecognizeManager Instance] UnInit];
}

void OSXVoiceManager::SetAudioListener(IAudioListener* listener)
{
    [[RecognizeManager Instance] SetAudioListener:listener];
    [[AudioPlayer Instance] SetAudioListener:listener];
}

void OSXVoiceManager::SetAudioRecordCacheDir(const XString& path)
{
    [[RecognizeManager Instance] SetAudioRecordCacheDir:path];
}

XString OSXVoiceManager::GetAudioRecordCacheDir()
{
    return [[RecognizeManager Instance] GetAudioRecordCacheDir];
}

void OSXVoiceManager::SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize)
{
    [[RecognizeManager Instance] SetAudioRecordParam:sampleRate channel:channel sampleBitSize:sampleBitSize];
}

int OSXVoiceManager::StartSpeech(XUINT64 serial)
{
    return [[RecognizeManager Instance] StartSpeech:serial isRecognize:true];
}

int OSXVoiceManager::StartOnlySpeech(XUINT64 serial)
{
   return [[RecognizeManager Instance] StartSpeech:serial isRecognize:false];
}

int OSXVoiceManager::StopSpeech()
{
    return [[RecognizeManager Instance] StopSpeech];
}

int OSXVoiceManager::CancleSpeech()
{
   return [[RecognizeManager Instance] CancelSpeech];
}

void OSXVoiceManager::SetRecognizeLanguage(RecognizeLanguage language)
{
    [[RecognizeManager Instance] SetRecognizeLanguage:language];
}

void OSXVoiceManager::SetKeepRecordModel(bool keep)
{
    [[RecognizeManager Instance] SetKeepRecordModel:keep];
}

void OSXVoiceManager::UpdateToken(const std::string& token)
{
    
}

int OSXVoiceManager::StartPlayAudio(const XString& path)
{
    NSString* strPath = [NSString stringWithUTF8String:path.c_str()];
    return [[AudioPlayer Instance] StartPlayAudio:strPath];
}

int OSXVoiceManager::StopPlayAudio()
{
    return [[AudioPlayer Instance] StopPlayAudio];
}

bool OSXVoiceManager::IsPlaying()
{
    return [[AudioPlayer Instance] IsPlaying];
}

void OSXVoiceManager::SetPlayVolume(float volume)
{
    [[AudioPlayer Instance] SetPlayVolume:volume];
}

int OSXVoiceManager::GetMicrophoneStatus()
{
    return AUDIOSTATUS_AVIAIBLE;
}
