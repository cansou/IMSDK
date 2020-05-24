#ifndef AUDIO_QUEUE_RECORDER_H
#define AUDIO_QUEUE_RECORDER_H

#import <Foundation/Foundation.h>
#include <YouMeIMEngine/Speech/VoiceInterface.h>


enum AudioDeviceStatus
{
    AUDIOSTATUS_IDEL,
    AUDIOSTATUS_RECORDING,
    AUDIOSTATUS_START_RECORD,
    AUDIOSTATUS_STOP,
    AUDIOSTATUS_CANCEL,
    AUDIOSTATUS_TIMEOUT,
    AUDIOSTATUS_RECORD_TOO_SHORT,
    AUDIOSTATUS_RECORD_ERROR
};


@protocol IAudioQueueRecorderDelegate <NSObject>

@required
- (void) OnAudioRecordData:(unsigned char*)buffer size:(unsigned int)size;

@required
- (void) OnRecordFinish:(AudioErrorCode)errorcode path:(NSString*)path serial:(unsigned long long)serial;

@end


@interface AudioQueueRecorder : NSObject

+(instancetype) Instance;
-(void) UninitAudioQueue;
-(void) SetAudioParam:(int)sampleRate channel:(int)channel sampleBiteSize:(int)sampleBitSize;
-(AudioErrorCode) StartRecord:(NSString*)path serial:(unsigned long long)serial;
-(AudioErrorCode) StopRecord;
-(AudioErrorCode) CancelRecord;
-(bool) IsRecording;
-(bool) StopAudioQueue;
-(void) SetKeepRecordModel:(bool)keep;
-(void) ResetAudioCategory;

@property (nonatomic, assign) id<IAudioQueueRecorderDelegate> recorderDelegate;
@property (nonatomic, copy) NSString* m_strAudioPath;
@property (nonatomic) unsigned long long m_ullSerial;

@end


#endif
