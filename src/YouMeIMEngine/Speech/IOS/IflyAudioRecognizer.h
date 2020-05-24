#ifndef IFLY_SPEECH_RECOGNIZER_H
#define IFLY_SPEECH_RECOGNIZER_H

#import "ISpeechRecognizer.h"


class IAudioListener;

@interface IflyAudioRecognizer : ISpeechRecognizer

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

@end


#endif
