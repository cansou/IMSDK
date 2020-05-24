#import "RecognizeManager.h"
#include "YouMeCommon/Log.h"
#include <YouMeCommon/CrossPlatformDefine/IYouMeSystemProvider.h>
#import "YouMeIMEngine/Speech/IOS/YouMeAudioRecorder.h"
#import "YouMeIMEngine/Speech/IOS/UscAudioRecognizer.h"
#import "YouMeIMEngine/Speech/IOS/IflyAudioRecognizer.h"
#import "YouMeIMEngine/Speech/IOS/IflyRecordRecognizer.h"
#import "YouMeIMEngine/Speech/IOS/AliAudioRecognizer.h"


extern IYouMeSystemProvider* g_pSystemProvider;

@interface RecognizeManager()

@property(nonatomic, strong) ISpeechRecognizer *m_speechRecognizer;
@property(nonatomic, assign) IAudioListener *m_audioListener;
@property(nonatomic, strong) dispatch_queue_t m_queue;
@property(nonatomic, strong) NSString *m_recordCacheDir;
@property(nonatomic) int m_nRecognizeType;
@property(nonatomic) bool m_bSpeechOnly;
@property(nonatomic) unsigned long long m_ullSerial;

@property(nonatomic) bool m_recognizeRecording;
@property(nonatomic) dispatch_semaphore_t m_semaphore;

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
    _m_recordCacheDir = nil;
    _m_audioListener = NULL;
    _m_nRecognizeType = RECOGNIZETYPE_NO;
    _m_bSpeechOnly = false;
    _m_ullSerial = 0;
    _m_recognizeRecording = false;
    _m_queue = dispatch_queue_create("AudioRecordQueue", DISPATCH_QUEUE_SERIAL);
    _m_semaphore = dispatch_semaphore_create(1);
    return self;
}

-(void) SetAudioListener:(IAudioListener*)listener
{
    _m_audioListener = listener;
    if (_m_speechRecognizer != nil)
    {
        [_m_speechRecognizer SetAudioListener:listener];
    }
    [[YouMeAudioRecorder Instance] SetAudioListener:listener];
}

-(void) InitRecognizer:(NSString*)appkey secret:(NSString*)secret recognizeType:(AudioRecognizeType)recognizeType
{
    if (_m_recordCacheDir == nil)
    {
        [self SetAudioRecordCacheDir:""];
    }
    _m_nRecognizeType = recognizeType;
    
    if (RECOGNIZETYPE_IFLY_RECORD == recognizeType)
    {
        _m_speechRecognizer = [[IflyRecordRecognizer alloc] init];
    }
    else if (RECOGNIZETYPE_IFLY_STREAM == recognizeType)
    {
        _m_speechRecognizer = [[IflyAudioRecognizer alloc] init];
    }
    else if (RECOGNIZETYPE_USC == recognizeType)
    {
        _m_speechRecognizer = [[UscAudioRecognizer alloc] init];
    }
    else if (RECOGNIZETYPE_ALI == recognizeType)
    {
        _m_speechRecognizer = [[AliAudioRecognizer alloc] init];
    }
    
    if (_m_speechRecognizer)
    {
        [_m_speechRecognizer InitRecognizer:appkey secret:secret recognizeType:recognizeType];
    }
}

-(void) SetRecognizeLanguage:(RecognizeLanguage)language
{
    if (_m_speechRecognizer != nil)
    {
        [_m_speechRecognizer SetRecognizeLanguage:language];
    }
}

-(void) SetAudioRecordParam:(int)sampleRate channel:(int)channel sampleBitSize:(int)sampleBitSize
{
    if (_m_speechRecognizer != nil)
    {
        [_m_speechRecognizer SetAudioRecordParam:sampleRate channel:channel sampleBitSize:sampleBitSize];
    }
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
    if (isRecognize)
    {
        _m_bSpeechOnly = false;
        if (_m_speechRecognizer == nil)
        {
            return AUDIOERROR_NOT_INIT;
        }
        if (RECOGNIZETYPE_IFLY_STREAM == _m_nRecognizeType
            || RECOGNIZETYPE_USC == _m_nRecognizeType || RECOGNIZETYPE_ALI == _m_nRecognizeType)  
        {            
            dispatch_semaphore_wait(_m_semaphore, DISPATCH_TIME_FOREVER);
            _m_recognizeRecording = true;
            dispatch_semaphore_signal(_m_semaphore);
            
            dispatch_async(_m_queue, ^{
                if (!_m_recognizeRecording) {                    
                    return;
                }
                
                int retcode = [_m_speechRecognizer StartSpeech:audioPath serial:serial];
                if (retcode != AUDIOERROR_SUCCESS && _m_audioListener != NULL)
                {
                    YouMe_LOG_Error(__XT("StartSpeech error:%d"), retcode);
                    _m_audioListener->OnRecordFinish(retcode, "", serial, "");
                }
            });
        }
        else
        {
            errorcode = [_m_speechRecognizer StartSpeech:audioPath serial:serial];
        }
    }
    else
    {
        _m_bSpeechOnly = true;
        dispatch_async(_m_queue, ^{
            int retcode = [[YouMeAudioRecorder Instance] StartSpeech:audioPath serial:serial recognizeType:RECOGNIZETYPE_NO];
            if (retcode != AUDIOERROR_SUCCESS && _m_audioListener != NULL)
            {
                YouMe_LOG_Error(__XT("StartSpeech error:%d"), retcode);
                _m_audioListener->OnRecordFinish(retcode, "", serial, "");
            }
        });
        
    }
    return errorcode;
}

-(int) StopSpeech
{
    dispatch_semaphore_wait(_m_semaphore, DISPATCH_TIME_FOREVER);
    if (_m_recognizeRecording)
    {
        _m_recognizeRecording = false;
    }
    dispatch_semaphore_signal(_m_semaphore);
    
    if (![self IsRecording])
    {
        return AUDIOERROR_NOT_START_RECORD;
    }
    int errorcode = AUDIOERROR_SUCCESS;
    if (_m_bSpeechOnly)
    {
        errorcode = [[YouMeAudioRecorder Instance] StopSpeech];
    }
    else
    {
        if (_m_speechRecognizer)
        {
            if (_m_nRecognizeType == RECOGNIZETYPE_ALI)
            {
                dispatch_async(_m_queue, ^{
                    int retcode = [_m_speechRecognizer StopSpeech];
                    if (retcode != AUDIOERROR_SUCCESS && _m_audioListener != NULL)
                    {
                        YouMe_LOG_Error(__XT("StopSpeech error:%d"), retcode);
                        _m_audioListener->OnRecordFinish(retcode, "", _m_ullSerial, "");
                    }
                });
            }
            else
            {
                errorcode = [_m_speechRecognizer StopSpeech];
            }
        }
        else
        {
            errorcode = AUDIOERROR_NOT_INIT;
        }
    }
    return errorcode;
}

-(int) CancelSpeech
{
    dispatch_semaphore_wait(_m_semaphore, DISPATCH_TIME_FOREVER);
    if (_m_recognizeRecording)
    {
        _m_recognizeRecording = false;
    }
    dispatch_semaphore_signal(_m_semaphore);
    
    if (![self IsRecording])
    {
        return AUDIOERROR_NOT_START_RECORD;
    }
    int errorcode = AUDIOERROR_SUCCESS;
    if (_m_bSpeechOnly)
    {
        errorcode = [[YouMeAudioRecorder Instance] CancelSpeech];
    }
    else
    {
        if (_m_speechRecognizer)
        {
            if (_m_nRecognizeType == RECOGNIZETYPE_ALI)
            {
                dispatch_async(_m_queue, ^{
                    int retcode = [_m_speechRecognizer CancelSpeech];
                    if (retcode != AUDIOERROR_SUCCESS && _m_audioListener != NULL)
                    {
                        YouMe_LOG_Error(__XT("StopSpeech error:%d"), retcode);
                        _m_audioListener->OnRecordFinish(retcode, "", _m_ullSerial, "");
                    }
                });
            }
            else
            {
                errorcode = [_m_speechRecognizer CancelSpeech];
            }
        }
        else
        {
            errorcode = AUDIOERROR_NOT_INIT;
        }
    }
    return errorcode;
}

-(bool) IsRecording
{
    if (_m_speechRecognizer != nil)
    {
        return [_m_speechRecognizer IsRecording] | [[YouMeAudioRecorder Instance] IsRecording];
    }
    else
    {
        return [[YouMeAudioRecorder Instance] IsRecording];
    }
}

-(void) SetKeepRecordModel:(bool)keep
{
    if (_m_speechRecognizer != nil)
    {
        [_m_speechRecognizer SetKeepRecordModel:keep];
    }
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
    if (_m_recordCacheDir)
    {
        return [_m_recordCacheDir UTF8String];
    }
    else
    {
        return "";
    }
}

-(void) UnInit
{
    _m_audioListener = NULL;
    [[YouMeAudioRecorder Instance] UninitAudioRecorder];
}

-(void) UpdateToken:(NSString*) token
{
    if (_m_speechRecognizer != nil)
    {
        [_m_speechRecognizer UpdateToken:token];
    }
}

@end
