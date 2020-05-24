#ifndef RECOGNIZER_MANAGER_H
#define RECOGNIZER_MANAGER_H


#import <Foundation/Foundation.h>
#include "YouMeIMEngine/Speech/VoiceInterface.h"


class IAudioListener;


@interface RecognizeManager : NSObject

+(instancetype) Instance;
-(void) SetAudioListener:(IAudioListener*)listener;
-(void) InitRecognizer:(NSString*)appkey secret:(NSString*)secret recognizeType:(AudioRecognizeType)recognizeType;
-(void) SetRecognizeLanguage:(RecognizeLanguage)language;
-(void) SetAudioRecordParam:(int) sampleRate channel:(int) channel sampleBitSize:(int)sampleBitSize;
-(int) StartSpeech:(unsigned long long)serial isRecognize:(bool)isRecognize;
-(int) StopSpeech;
-(int) CancelSpeech;
-(bool) IsRecording;
-(void) SetKeepRecordModel:(bool)keep;
-(void) SetAudioRecordCacheDir:(const std::string&)path;
-(std::string) GetAudioRecordCacheDir;
-(void) UnInit;

@end

#endif
