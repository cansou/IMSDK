#import "RecognizeManager.h"
#include "YouMeCommon/Log.h"
#include <YouMeCommon/CrossPlatformDefine/IYouMeSystemProvider.h>
#import "YouMeIMEngine/Speech/OSX/YouMeAudioRecorder.h"

extern IYouMeSystemProvider* g_pSystemProvider;

@interface RecognizeManager()

@property(nonatomic, assign) IAudioListener *m_audioListener;
@property(nonatomic, strong) dispatch_queue_t m_queue;
@property(nonatomic, retain) NSString *m_recordCacheDir;
@property(nonatomic) int m_nRecognizeType;
@property(nonatomic) bool m_bSpeechOnly;
@property(nonatomic) unsigned long long m_ullSerial;

@end


@implementation RecognizeManager

+(instancetype) Instance
{
    static RecognizeManager *instance = nil;
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
    if (!self)
    {
        return nil;
    }
    
    _m_audioListener = NULL;
    _m_nRecognizeType = RECOGNIZETYPE_NO;
    _m_bSpeechOnly = true;
    _m_ullSerial = 0;
    _m_recordCacheDir = @"";
    _m_queue = dispatch_queue_create("AudioRecordQueue", DISPATCH_QUEUE_SERIAL);
    
    return self;
}

-(void) SetAudioListener:(IAudioListener*)listener
{
    _m_audioListener = listener;

    [[YouMeAudioRecorder Instance] SetAudioListener:listener];
}

-(void) InitRecognizer:(NSString*)appkey secret:(NSString*)secret recognizeType:(AudioRecognizeType)recognizeType
{
    if (_m_recordCacheDir == nil || [_m_recordCacheDir isEqualToString:@""])
    {
        [self SetAudioRecordCacheDir:""];
    }
//    _m_nRecognizeType = recognizeType;
}

-(void) SetRecognizeLanguage:(RecognizeLanguage)language
{

}

-(void) SetAudioRecordParam:(int)sampleRate channel:(int)channel sampleBitSize:(int)sampleBitSize
{
    [[YouMeAudioRecorder Instance] SetAudioRecordParam:sampleRate channel:channel sampleBitSize:sampleBitSize];
}

-(int) StartSpeech:(unsigned long long)serial isRecognize:(bool)isRecognize
{
    if ([[YouMeAudioRecorder Instance] GetRecordPermission] == 0)
    {
        return AUDIOERROR_AUTHORIZE;
    }
    
    if ([self IsRecording])
    {
        return AUDIOERROR_RECORDING;
    }
    _m_ullSerial = serial;
    
    int errorcode = AUDIOERROR_SUCCESS;
    
    NSString *audioPath = [NSString stringWithFormat:@"%@%u.wav", _m_recordCacheDir, arc4random()];
   
        _m_bSpeechOnly = true;
        dispatch_async(_m_queue, ^{
            int retcode = [[YouMeAudioRecorder Instance] StartSpeech:audioPath serial:serial recognizeType:RECOGNIZETYPE_NO];
            if (retcode != AUDIOERROR_SUCCESS && _m_audioListener != NULL)
            {
                YouMe_LOG_Error(__XT("StartSpeech error:%d"), retcode);
                _m_audioListener->OnRecordFinish(retcode, "", serial, "");
            }
        });

    return errorcode;
}

-(int) StopSpeech
{
    if (![self IsRecording])
    {
        return AUDIOERROR_NOT_START_RECORD;
    }
    int errorcode = AUDIOERROR_SUCCESS;
    
    if (_m_bSpeechOnly)
    {
        errorcode = [[YouMeAudioRecorder Instance] StopSpeech];
    }

    return errorcode;
}

-(int) CancelSpeech
{
    if (![self IsRecording])
    {
        return AUDIOERROR_NOT_START_RECORD;
    }
    int errorcode = AUDIOERROR_SUCCESS;
    if (_m_bSpeechOnly)
    {
        errorcode = [[YouMeAudioRecorder Instance] CancelSpeech];
    }

    return errorcode;
}

-(bool) IsRecording
{
   return [[YouMeAudioRecorder Instance] IsRecording];
}

-(void) SetKeepRecordModel:(bool)keep
{
    if (RECOGNIZETYPE_NO == _m_nRecognizeType)
    {
        [[YouMeAudioRecorder Instance] SetKeepRecordModel:keep];
    }
}

-(void) SetAudioRecordCacheDir:(const std::string&)path
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    if (fileManager == nil)
    {
        return;
    }
    if (path.empty())
    {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
        _m_recordCacheDir = [[paths objectAtIndex:0] stringByAppendingString:@"/AudioRecord/"];
        if ([fileManager fileExistsAtPath:_m_recordCacheDir] == NO)
        {
            if ([fileManager createDirectoryAtPath:_m_recordCacheDir withIntermediateDirectories:YES attributes:nil error:nil] == NO)
            {
                YouMe_LOG_Error(__XT("create audio cache directory failed"));
            }
        }
    }
    else
    {
        NSString* audioCacheDir = [NSString stringWithUTF8String:path.c_str()];
        if ([fileManager fileExistsAtPath:audioCacheDir] == YES)
        {
            _m_recordCacheDir = audioCacheDir;
        }
        else
        {
            if ([fileManager createDirectoryAtPath:audioCacheDir withIntermediateDirectories:YES attributes:nil error:nil] == YES)
            {
                _m_recordCacheDir = audioCacheDir;
            }
        }
    }
    
    g_pSystemProvider->setAudioCachePath(path);
}

-(std::string) GetAudioRecordCacheDir
{
    if (_m_recordCacheDir && (![_m_recordCacheDir isEqualToString:@""]))
    {
        return [_m_recordCacheDir UTF8String];
    }
    else
    {
        [self SetAudioRecordCacheDir:""];
        if(_m_recordCacheDir)
        {
            return [_m_recordCacheDir UTF8String];
        }
    }
    return "";
}

-(void) UnInit
{
    _m_audioListener = NULL;
    [[YouMeAudioRecorder Instance] UninitAudioRecorder];
}

@end
