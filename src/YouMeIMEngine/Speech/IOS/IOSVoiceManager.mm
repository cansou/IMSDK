#include "IOSVoiceManager.h"
#include <YouMeIMEngine/Speech/IOS/RecognizeManager.h>
#include <YouMeIMEngine/Speech/IOS/AudioPlayer.h>
#include <UIKit/UIKit.h>


IOSVoiceManager::IOSVoiceManager()
{
    
}

bool IOSVoiceManager::Init(const std::string& appkey, const std::string& secret, AudioRecognizeType audioRecognizeType)
{
    NSString* strAppkey = [NSString stringWithUTF8String:appkey.c_str()];
    NSString* strSecret = [NSString stringWithUTF8String:secret.c_str()];
    [[RecognizeManager Instance] InitRecognizer:strAppkey secret:strSecret recognizeType:audioRecognizeType];
    return true;
}

void IOSVoiceManager::UnInit()
{
    [[RecognizeManager Instance] UnInit];
}

void IOSVoiceManager::SetAudioListener(IAudioListener* listener)
{
    [[RecognizeManager Instance] SetAudioListener:listener];
    [[AudioPlayer Instance] SetAudioListener:listener];
}

void IOSVoiceManager::SetAudioRecordCacheDir(const XString& path)
{
    [[RecognizeManager Instance] SetAudioRecordCacheDir:path];
}

XString IOSVoiceManager::GetAudioRecordCacheDir()
{
    return [[RecognizeManager Instance] GetAudioRecordCacheDir];
}

void IOSVoiceManager::SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize)
{
    [[RecognizeManager Instance] SetAudioRecordParam:sampleRate channel:channel sampleBitSize:sampleBitSize];
}

int IOSVoiceManager::StartSpeech(XUINT64 serial)
{
    return [[RecognizeManager Instance] StartSpeech:serial isRecognize:true];
}

int IOSVoiceManager::StartOnlySpeech(XUINT64 serial)
{
   return [[RecognizeManager Instance] StartSpeech:serial isRecognize:false];
}

int IOSVoiceManager::StopSpeech()
{
    return [[RecognizeManager Instance] StopSpeech];
}

int IOSVoiceManager::CancleSpeech()
{
   return [[RecognizeManager Instance] CancelSpeech];
}

void IOSVoiceManager::SetRecognizeLanguage(RecognizeLanguage language)
{
    [[RecognizeManager Instance] SetRecognizeLanguage:language];
}

void IOSVoiceManager::SetKeepRecordModel(bool keep)
{
    [[RecognizeManager Instance] SetKeepRecordModel:keep];
}

void IOSVoiceManager::UpdateToken(const std::string& token)
{
    NSString* nsToken = [NSString stringWithUTF8String:token.c_str()];
    [[RecognizeManager Instance] UpdateToken:nsToken];
}

int IOSVoiceManager::StartPlayAudio(const XString& path)
{
    NSString* strPath = [NSString stringWithUTF8String:path.c_str()];
    return [[AudioPlayer Instance] StartPlayAudio:strPath];
}

int IOSVoiceManager::StopPlayAudio()
{
    return [[AudioPlayer Instance] StopPlayAudio];
}

bool IOSVoiceManager::IsPlaying()
{
    return [[AudioPlayer Instance] IsPlaying];
}

void IOSVoiceManager::SetPlayVolume(float volume)
{
    [[AudioPlayer Instance] SetPlayVolume:volume];
}

int IOSVoiceManager::GetMicrophoneStatus()
{
    BOOL available = YES;
    if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 6.0) {
        available = [AVAudioSession sharedInstance].inputAvailable;
    } else {
        available = [[AVAudioSession sharedInstance] inputIsAvailable];
    }
    if (!available) {
        return AUDIOSTATUS_NOT_AVIAIBLE;
    }
    
    AVAuthorizationStatus audioAuthStatus = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeAudio];
    if(audioAuthStatus == AVAuthorizationStatusRestricted || audioAuthStatus == AVAuthorizationStatusDenied)
    {
        return AUDIOSTATUS_NO_AUTHORIZE;
    }
    return AUDIOSTATUS_AVIAIBLE;
}
