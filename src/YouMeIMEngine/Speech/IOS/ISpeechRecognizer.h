#ifndef ISPEECH_RECOGNIZER_H
#define ISPEECH_RECOGNIZER_H


#import <Foundation/Foundation.h>
#include "YouMeIMEngine/Speech/VoiceInterface.h"


class IAudioListener;


@interface ISpeechRecognizer : NSObject

-(void) SetAudioListener:(IAudioListener*)listener;
-(void) InitRecognizer:(NSString*)appkey secret:(NSString*)secret recognizeType:(AudioRecognizeType)recognizeType;
-(void) SetRecognizeLanguage:(RecognizeLanguage)language;
-(void) SetAudioRecordParam:(int) sampleRate channel:(int) channel sampleBitSize:(int)sampleBitSize;
-(int) StartSpeech:(NSString*)path serial:(unsigned long long)serial;
-(int) StopSpeech;
-(int) CancelSpeech;
-(bool) IsRecording;
-(void) SetKeepRecordModel:(bool)keep;
-(void) UnInit;
-(void) UpdateToken:(NSString*) token;

@end

#endif
