#import "YouMeAudioRecorder.h"
#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#include <YouMeCommon/Log.h>


@interface YouMeAudioRecorder()

@property (nonatomic, assign) IAudioListener* m_recorderLisener;
@property (nonatomic, assign) id<IAudioQueueRecorderDelegate> m_audioRecognizeDelegate;
@property (nonatomic) AudioRecognizeType m_audioRecognizeType;
@property (nonatomic) AudioDeviceStatus m_status;

@end


@implementation YouMeAudioRecorder

+(instancetype) Instance
{
    static YouMeAudioRecorder *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[self alloc] initPrivate];
    });
    return instance;
}

-(instancetype) init
{
    @throw [NSException exceptionWithName:@"Singleton" reason:@"Use +(instancetype) instance" userInfo:nil];
    return nil;
}

-(instancetype) initPrivate
{
    self = [super init];
    if (!self) {
        return nil;
    }
    
    [[AudioQueueRecorder Instance] setRecorderDelegate:self];
    
    return self;
}

-(void) UninitAudioRecorder
{
    [[AudioQueueRecorder Instance] UninitAudioQueue];
}

-(void) SetAudioListener:(IAudioListener*)listener
{
    _m_recorderLisener = listener;
}

-(void) SetAudioRecognizeDelegate:(id<IAudioQueueRecorderDelegate>)delegate
{
    _m_audioRecognizeDelegate = delegate;
}

-(void) SetAudioRecordParam:(int) sampleRate channel:(int) channel sampleBitSize:(int)sampleBitSize
{
    [[AudioQueueRecorder Instance] SetAudioParam:sampleRate channel:channel sampleBiteSize:sampleBitSize];
}

-(AudioErrorCode) StartSpeech:(NSString*)path serial:(unsigned long long)serial recognizeType:(AudioRecognizeType)recognizeType
{
    _m_audioRecognizeType = recognizeType;
    AudioErrorCode errorcode = AUDIOERROR_SUCCESS;
    if (RECOGNIZETYPE_NO == recognizeType)
    {
        _m_status = AUDIOSTATUS_START_RECORD;
        errorcode = [[AudioQueueRecorder Instance] StartRecord:path serial:serial];
        if (AUDIOERROR_SUCCESS == errorcode)
        {
            if (_m_status == AUDIOSTATUS_STOP )
            {
                [[AudioQueueRecorder Instance] StopRecord];
            }
            else if (_m_status == AUDIOSTATUS_CANCEL)
            {
                _m_status = AUDIOSTATUS_IDEL;
                [[AudioQueueRecorder Instance] CancelRecord];
            }
            else
            {
                _m_status = AUDIOSTATUS_RECORDING;
            }
        }
    }
    else
    {
        errorcode = [[AudioQueueRecorder Instance] StartRecord:path serial:serial];
    }
    return errorcode;
}

-(AudioErrorCode) StopSpeech
{
    AudioErrorCode errorcode = AUDIOERROR_SUCCESS;
    if (RECOGNIZETYPE_NO == _m_audioRecognizeType)
    {
        if (_m_status == AUDIOSTATUS_START_RECORD)
        {
            _m_status = AUDIOSTATUS_STOP;
        }
        else
        {
            if (_m_status != AUDIOSTATUS_RECORDING)
            {
                return AUDIOERROR_NOT_START_RECORD;
            }
            errorcode = [[AudioQueueRecorder Instance] StopRecord];
        }
    }
    else
    {
        errorcode = [[AudioQueueRecorder Instance] StopRecord];
    }
    return errorcode;
}

-(AudioErrorCode) CancelSpeech
{
    AudioErrorCode errorcode = AUDIOERROR_SUCCESS;
    if (RECOGNIZETYPE_NO == _m_audioRecognizeType)
    {
        if (_m_status == AUDIOSTATUS_START_RECORD)
        {
            _m_status = AUDIOSTATUS_CANCEL;
        }
        else
        {
            if (_m_status != AUDIOSTATUS_RECORDING)
            {
                return AUDIOERROR_NOT_START_RECORD;
            }
            errorcode = [[AudioQueueRecorder Instance] CancelRecord];
        }
    }
    else
    {
        errorcode = [[AudioQueueRecorder Instance] CancelRecord];
    }
    return errorcode;
}

-(bool) IsRecording
{
    return [[AudioQueueRecorder Instance] IsRecording];
}

-(void) SetKeepRecordModel:(bool)keep
{
    [[AudioQueueRecorder Instance] SetKeepRecordModel:keep];
}

- (int) GetRecordPermission
{
    float ver = [[[UIDevice currentDevice] systemVersion] floatValue];
    if( ver >= 8 || ver==0.0f)
    {
        AVAudioSessionRecordPermission recPermission = AVAudioSessionRecordPermissionUndetermined;
        recPermission = [[AVAudioSession sharedInstance] recordPermission];
        if (AVAudioSessionRecordPermissionGranted == recPermission)
        {
            return 1;
        }
        else if (AVAudioSessionRecordPermissionDenied == recPermission)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else if( ver >=7 )
    {
        [[AVAudioSession sharedInstance] requestRecordPermission:^(BOOL granted){
            YouMe_LOG_Warning(__XT("ios record permission:%d"), granted);
        }];
        return 1;
    }
    return 1;
}


#pragma mark - IAudioQueueRecorderDelegate

- (void) OnAudioRecordData:(unsigned char*)buffer size:(unsigned int)size
{
    if (RECOGNIZETYPE_NO == _m_audioRecognizeType)
    {
        if (_m_recorderLisener != NULL)
        {
            _m_recorderLisener->OnAudioRecordData(buffer, size);
        }
    }
    else
    {
        if (_m_audioRecognizeDelegate != nil)
        {
            [_m_audioRecognizeDelegate OnAudioRecordData:buffer size:size];
        }
    }
}

- (void) OnRecordFinish:(AudioErrorCode)errorcode path:(NSString*)path serial:(unsigned long long)serial
{    
    if (RECOGNIZETYPE_NO == _m_audioRecognizeType)
    {
        if (_m_recorderLisener != NULL)
        {
            std::string strPath = [path UTF8String];
            _m_recorderLisener->OnRecordFinish(errorcode, strPath, serial, "");
        }
    }
    else
    {
        if (_m_audioRecognizeDelegate != nil)
        {
            [_m_audioRecognizeDelegate OnRecordFinish:errorcode path:path serial:serial];
        }
    }
}

@end
