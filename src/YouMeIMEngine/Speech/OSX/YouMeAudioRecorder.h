#ifndef YOUME_AUDIO_RECORDER_H
#define YOUME_AUDIO_RECORDER_H


#import "AudioQueueRecorder.h"


@interface YouMeAudioRecorder : NSObject<IAudioQueueRecorderDelegate>

+(instancetype) Instance;
-(void) UninitAudioRecorder;
-(void) SetAudioListener:(IAudioListener*)listener;
-(void) SetAudioRecognizeDelegate:(id<IAudioQueueRecorderDelegate>)delegate;
-(void) SetAudioRecordParam:(int) sampleRate channel:(int) channel sampleBitSize:(int)sampleBitSize;
-(AudioErrorCode) StartSpeech:(NSString*)path serial:(unsigned long long)serial recognizeType:(AudioRecognizeType)recognizeType;
-(AudioErrorCode) StopSpeech;
-(AudioErrorCode) CancelSpeech;
-(bool) IsRecording;
-(void) SetKeepRecordModel:(bool)keep;
- (int) GetRecordPermission;

@end

#endif
