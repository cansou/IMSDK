#import "UscAudioRecognizer.h"
#import "YouMeIMEngine/Speech/IOS/YouMeAudioRecorder.h"
#include "YouMeCommon/Log.h"




/************************************** 带语音识别定义 begin **************************************/

//#undef USC_RECOGNIZE
#ifdef USC_RECOGNIZE

#import <USCModule.framework/Headers/USCSpeechUnderstander.h>
#import <USCModule.framework/Headers/USCSpeechConstant.h>
#import <USCModule.framework/Headers/USCRestInterface.h>
#import <USCModule.framework/Headers/USCPreferenceConfigure.h>

#define USC_RECOGNIZE_BUFFER_SIZE 9600

@interface UscAudioRecognizer()<USCSpeechUnderstanderDelegate, IAudioQueueRecorderDelegate>

@property (nonatomic,strong) USCSpeechUnderstander *m_speechUnderstander;
@property (nonatomic, assign) IAudioListener* m_recorderLisener;
@property (nonatomic) int m_sampleRate;
@property (nonatomic, strong) NSMutableString *m_audioText;
@property (nonatomic, strong) NSString *m_audioPath;
@property (nonatomic) unsigned long long m_serial;
@property (nonatomic) unsigned long long m_recognizeErrorCode;
@property (nonatomic) AudioErrorCode m_recordErrorCode;
@property (nonatomic) AudioDeviceStatus m_status;
//@property (nonatomic, strong) NSMutableData *m_recordingDatas;
@property (nonatomic,assign) NSInteger m_readedSize;
@property(nonatomic, strong) dispatch_queue_t m_queue;

@end


@implementation UscAudioRecognizer

-(instancetype) init
{
    self = [super init];
    if (!self)
    {
        return nil;
    }
    _m_recorderLisener = NULL;
    _m_sampleRate = SAMPLERATE_16K;
    _m_audioText = [[NSMutableString alloc] init];
    _m_status = AUDIOSTATUS_IDEL;
    
    return self;
}

-(void) SetAudioListener:(IAudioListener*)listener
{
    _m_recorderLisener = listener;
}

-(void) InitRecognizer:(NSString*)appkey secret:(NSString*)secret recognizeType:(AudioRecognizeType)recognizeType
{
    //[USCPreferenceConfigure setLogEnable:NO];
    [USCPreferenceConfigure setLogLevel:LVL_N];
    
    kUSCRestShareInterface.appKey = appkey;
    kUSCRestShareInterface.appSecret = secret;
    
    _m_speechUnderstander = [[USCSpeechUnderstander alloc] initWithContext:nil];
    [_m_speechUnderstander setOption:USC_ASR_DOMAIN value:USC_ENGINE_GENERAL];
    [_m_speechUnderstander setOption:USC_ASR_VAD_TIMEOUT_FRONTSIL value:@(60000)];
    [_m_speechUnderstander setOption:USC_ASR_VAD_TIMEOUT_BACKSIL value:@(60000)];
    [_m_speechUnderstander setOption:USC_ASR_OPT_VAD_ENABLED value:@(NO)];
    [_m_speechUnderstander setOption:USC_ASR_SAMPLING_RATE value:[NSString stringWithFormat:@"%d", SAMPLERATE_16K]];
    [_m_speechUnderstander setOption:USC_ASR_LANGUAGE value:USC_LANGUAGE_MANDARIN];
    [_m_speechUnderstander setOption:USC_AEC_ENABLE_MIC_CHENNEL value:nil]; // AEC
    [_m_speechUnderstander setOption:ASR_RECOGNIZE_ONLINE_TYPE value:ASR_RECOGNIZE_ONLINE_TYPE_Asr];
    [_m_speechUnderstander setOption:ASR_RECOGNIZE_LINE_TYPE value:ASR_RECOGNIZE_ONLINE_ONLY];
    [_m_speechUnderstander setOption:USC_NLU_ENABLE value:@(YES)];
    [_m_speechUnderstander setOption:USC_TR_ENABLE  value:@(NO)];
    [_m_speechUnderstander setOption:USC_WRITE_FILE value:@(NO)];
    [_m_speechUnderstander setOption:USC_SERIESASR  value:@(NO)];
    //[_m_speechUnderstander setOption:USC_NLU_SCENARIO value:@"incar"];
    
    _m_speechUnderstander.delegate = self;
    //[_m_speechUnderstander setAudioSource:self];
    [[YouMeAudioRecorder Instance] SetAudioRecognizeDelegate:self];
    
    _m_queue = dispatch_queue_create("RecognizeQueue", DISPATCH_QUEUE_SERIAL);
}

-(void) SetRecognizeLanguage:(RecognizeLanguage)language
{
    if (RECOGNIZELANG_YUEYU == language)
    {
        [_m_speechUnderstander setOption:USC_ASR_LANGUAGE value:USC_LANGUAGE_CANTONESE];
    }
    else if (RECOGNIZELANG_ENGLISH == language)
    {
        [_m_speechUnderstander setOption:USC_ASR_LANGUAGE value:USC_LANGUAGE_ENGLISH];
    }
    else
    {
         [_m_speechUnderstander setOption:USC_ASR_LANGUAGE value:USC_LANGUAGE_MANDARIN];
    }
}

-(void) SetAudioRecordParam:(int)sampleRate channel:(int)channel sampleBitSize:(int)sampleBitSize
{
    if (_m_sampleRate == sampleRate)
    {
        return;
    }
    _m_sampleRate = sampleRate;
    int sample = sampleRate == SAMPLERATE_8K ? sampleRate : SAMPLERATE_16K;
    [_m_speechUnderstander setOption:USC_ASR_SAMPLING_RATE value:[NSString stringWithFormat:@"%d", sample]];
}

-(int) StartSpeech:(NSString*)path serial:(unsigned long long)serial
{
    NSLog(@"StartSpeech enter");
    
    if (_m_status != AUDIOSTATUS_IDEL)
    {
        return AUDIOERROR_RECORDING;
    }
    _m_status = AUDIOSTATUS_START_RECORD;
    _m_serial = serial;
    _m_audioPath = path;
    [_m_audioText setString:@""];
    _m_readedSize = 0;
    _m_recognizeErrorCode = 0;
    _m_recordErrorCode = AUDIOERROR_SUCCESS;
    
    AudioErrorCode errorcode = [[YouMeAudioRecorder Instance] StartSpeech:_m_audioPath serial:_m_serial recognizeType:RECOGNIZETYPE_USC];
    if (errorcode == AUDIOERROR_SUCCESS)
    {
        if (_m_status == AUDIOSTATUS_STOP )
        {
            [[YouMeAudioRecorder Instance] StopSpeech];
            return errorcode;
        }
        else if (_m_status == AUDIOSTATUS_CANCEL)
        {
            _m_status = AUDIOSTATUS_IDEL;
            [[YouMeAudioRecorder Instance] CancelSpeech];
            return errorcode;
        }
        else
        {
            _m_status = AUDIOSTATUS_RECORDING;
        }
        
        [_m_speechUnderstander startRecognizeData];
        
        if (_m_status == AUDIOSTATUS_STOP )
        {
            [[YouMeAudioRecorder Instance] StopSpeech];
        }
        else if (_m_status == AUDIOSTATUS_CANCEL)
        {
            _m_status = AUDIOSTATUS_IDEL;
            [[YouMeAudioRecorder Instance] CancelSpeech];
        }
    }
    
    NSLog(@"StartSpeech leave");
    
    return errorcode;
}

-(int) StopSpeech
{
    NSLog(@"StopSpeech enter");
    
    AudioErrorCode errorcode = AUDIOERROR_SUCCESS;
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
        errorcode =  [[YouMeAudioRecorder Instance] StopSpeech];
    }
   
    NSLog(@"StopSpeech leave");
    
    return errorcode;
}

-(int) CancelSpeech
{
    NSLog(@"CancleSpeech enter");
    
    AudioErrorCode errorcode = AUDIOERROR_SUCCESS;
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
        _m_serial = 0;
        _m_status = AUDIOSTATUS_IDEL;
        [_m_speechUnderstander cancel];
        errorcode = [[YouMeAudioRecorder Instance] CancelSpeech];
    }
    
    NSLog(@"CancleSpeech leave");
    
    return errorcode;
}

-(bool) IsRecording
{
    return AUDIOSTATUS_RECORDING == _m_status || AUDIOSTATUS_START_RECORD == _m_status;
}

-(void) SetKeepRecordModel:(bool)keep
{
    [[YouMeAudioRecorder Instance] SetKeepRecordModel:keep];
}

-(void) UnInit
{
    if (_m_status == AUDIOSTATUS_RECORDING)
    {
        [[YouMeAudioRecorder Instance] CancelSpeech];
        [_m_speechUnderstander stop];
    }
}


#pragma mark - USCSpeechUnderstanderDelegate

- (void)onRecognizeError:(int)type error:(NSError *)error
{
    if (error)
    {
        YouMe_LOG_Error(__XT("onRecognizeError type = %d error = %d"), type, (int)error.code);
    }
    else
    {
        YouMe_LOG_Error(__XT("onRecognizeError type = %d"), type);
    }
    
    _m_recognizeErrorCode = (int)error.code;
}

- (void)onRecognizeEvent:(int)type timeMs:(int)timeMs;
{
    //NSLog(@"onRecognizeEvent type = %d timeMs = %d", type, timeMs);
    
    if (type == USC_ASR_EVENT_VOLUMECHANGE)
    {
        /*if (_m_recorderLisener != NULL) 
        {
            int volume = [[_m_speechUnderstander getOption:USC_ASR_EVENT_VOLUMECHANGE] intValue];
            _m_recorderLisener->OnRecordVolumeChange(volume, RECOGNIZETYPE_USC_STREAM);
        }*/
    }
    else if (USC_ASR_EVENT_RECORDING_START == type)
    {
        NSLog(@"EVENT_RECORDING_START");
    }
    else if (USC_ASR_EVENT_RECOGNIZE_START == type)
    {
        NSLog(@"EVENT_RECOGNIZE_START");
    }
    else if (USC_ASR_EVENT_RECOGNIZE_STOP == type)
    {
        NSLog(@"EVENT_RECOGNIZE_STOP");
    }
    else if (USC_ASR_EVENT_RECOGNIZE_CANCEL == type)
    {
        NSLog(@"EVENT_RECOGNIZE_CANCEL");
    }
    else if (USC_ASR_EVENT_RECOGNIZE_FINISH == type)
    {
        NSLog(@"EVENT_RECOGNIZE_FINISH");
        //_m_status = AUDIOSTATUS_IDEL;
    }
    else if (USC_ASR_EVENT_RECOGNIZE_FINISH_ERROR == type)
    {
        YouMe_LOG_Error(__XT("onRecognizeEvent type = %d timeMs = %d"), type, timeMs);
    }
}

- (void)onRecognizeResult:(int)type jsonString:(NSString *)jsonString
{
    //NSLog(@"onRecognizeResult type = %d", type);
    
    // 可变结果（返回的过程结果，不是最终的结果）
    if (ASR_ONLINE_VARIABLE_RESULT == type)
    {
        //NSLog(@"ASR_ONLINE_VARIABLE_RESULT json= %@", jsonString);
    }
    /* 注意这个方法会调用多次，注意根据type判断，得到想要的结果  */
    else if (ASR_ONLINE_PARTIAL_RESULT == type)
    {
        //NSLog(@"ASR_ONLINE_PARTIAL_RESULT json= %@", jsonString);
    }
    else if (ASR_ONLINE_LAST_RESULT == type)
    {
        //NSLog(@"ASR_ONLINE_LAST_RESULT json= %@", jsonString);
        
        if (0 == _m_serial)
        {
            _m_status = AUDIOSTATUS_IDEL;
            return;
        }
        if (_m_recognizeErrorCode != 0)
        {
            return;
        }
        _m_status = AUDIOSTATUS_IDEL;
        
        if (!jsonString)
        {
            return;
        }
        
        NSError *error = nil;
        NSDictionary *resObj = [NSJSONSerialization JSONObjectWithData:[jsonString dataUsingEncoding:NSUTF8StringEncoding] options:NSJSONReadingAllowFragments error:&error];
        if (resObj)
        {
            NSString *part = [resObj objectForKey:@"asr_recongize"];
            if(part){
                [_m_audioText appendFormat:@"%@", part];
                YouMe_LOG_Info("part:%s", [part UTF8String]);
                YouMe_LOG_Info("text:%s", [_m_audioText UTF8String]);
            }else{
                [_m_audioText setString:@""];
                YouMe_LOG_Error("usc recognise result is empty");
            }
        }
        
        dispatch_async(_m_queue, ^{
            _m_recorderLisener->OnRecordFinish(AUDIOERROR_SUCCESS, [_m_audioPath UTF8String], _m_serial, [_m_audioText UTF8String]);
        });
    }
    else if (ASR_OFFLINE_LAST_RESULT == type)
    {
        //NSLog(@"type = %d ,json= %@",type,jsonString);
    }
    else if (ASR_ONLINE_SESSIONID == type)
    {
        //NSLog(@"session id = %@", jsonString);
    }
}


#pragma mark - IAudioQueueRecorderDelegate

- (void) OnAudioRecordData:(unsigned char*)buffer size:(unsigned int)size;
{
    //NSLog(@"OnAudioRecordData size:%d", size);
    /*@synchronized(_m_recordingDatas) {
        [_m_recordingDatas appendData:[NSData dataWithBytes:buffer length:size]];
    }*/
    
    unsigned char* audioData = buffer;
    unsigned int audioDataSize = size;
    if (_m_sampleRate != SAMPLERATE_8K && _m_sampleRate != SAMPLERATE_16K)
    {
        audioData = _m_recorderLisener->ResampleAudioData(buffer, size, &audioDataSize);
        if (audioData == NULL || audioDataSize == 0)
        {
            YouMe_LOG_Error(__XT("resample error"));
            return;
        }
    }
    
    //NSLog(@"OnAudioRecordData size:%d resampleSize:%d", size, audioDataSize);
    
    for (unsigned int begin = 0; begin < audioDataSize; )
    {
        int readSize = USC_RECOGNIZE_BUFFER_SIZE;
        unsigned remainSize = audioDataSize - begin;
        if (remainSize < USC_RECOGNIZE_BUFFER_SIZE)
        {
            readSize = remainSize;
        }
        unsigned char* temp = audioData + begin;
        [_m_speechUnderstander recognizeAudioFileFromBytesData:[NSData dataWithBytes:temp length:readSize]];
        begin += readSize;
    }
    
    if (_m_recorderLisener != NULL)
    {
        _m_recorderLisener->OnAudioRecordData(buffer, size);
    }
}

- (void) OnRecordFinish:(AudioErrorCode)errorcode path:(NSString*)path serial:(unsigned long long)serial
{
    YouMe_LOG_Info("OnRecordFinish errorcode:%d serial:%llu status:%d", errorcode, serial, _m_status);
    
    _m_recordErrorCode = errorcode;
    [_m_speechUnderstander stop];
    AudioDeviceStatus status = _m_status;
    _m_status = AUDIOSTATUS_IDEL;
    
    if (_m_recorderLisener == NULL)
    {
        return;
    }
    
    if (AUDIOSTATUS_STOP == status)
    {
        _m_recorderLisener->OnRecordFinish(errorcode, [path UTF8String], _m_serial, "");
    }
    else if (AUDIOSTATUS_CANCEL == status)
    {
    }
    else
    {
        if (_m_recognizeErrorCode != 0)
        {
            _m_recorderLisener->OnRecordFinish(errorcode == AUDIOERROR_SUCCESS ? AUDIOERROR_RECOGNIZE_FAILED : errorcode, [_m_audioPath UTF8String], _m_serial, [_m_audioText UTF8String]);
        }
    }
}


/*#pragma mark - USCIAudioSource
 
 - (int)openAudioIn
 {
    NSLog(@"openAudioIn");
 
    AudioErrorCode errorcode = [[YouMeAudioRecorder Instance] StartSpeech:_m_audioPath serial:_m_serial recognizeType:RECOGNIZETYPE_USC_STREAM];
    return errorcode;
 }
 
 - (NSData *)readDataSize:(int)size
 {
 NSLog(@"==================readData size:%d", size);
 
 NSData* audioData;
 if (_m_sampleRate == SAMPLERATE_8K || _m_sampleRate == SAMPLERATE_16K)
 {
 @synchronized(_m_recordingDatas) {
 NSInteger lastedSize = _m_recordingDatas.length - _m_readedSize;
 if (lastedSize == 0 )
 {
 //NSLog(@"lastedSize == 0");
 audioData = [NSData data];
 return audioData;
 }
 long readSize = lastedSize < size ? (unsigned int)lastedSize : size;
 audioData = [_m_recordingDatas subdataWithRange:NSMakeRange(_m_readedSize, readSize)];
 _m_readedSize += readSize;
 
 //NSLog(@"readData size = %d readSize = %ld readedSize = %ld", size, readSize, _readedSize);
 }
 return audioData;
 }
 else
 {
 if (NULL == _m_recorderLisener)
 {
 YouMe_LOG_Error(__XT("record listener is NULL"));
 audioData = [NSData data];
 return audioData;
 }
 
 int originalSize = 0;
 NSData* originalData;
 @synchronized(_m_recordingDatas) {
 NSInteger lastedSize = _m_recordingDatas.length - _m_readedSize;
 if (lastedSize == 0 )
 {
 //NSLog(@"lastedSize == 0");
 audioData = [NSData data];
 return audioData;
 }
 
 originalSize = ((double)_m_sampleRate / SAMPLERATE_16K) * size;
 if (lastedSize < originalSize) {
 originalSize = (int)lastedSize;
 }
 originalData = [_m_recordingDatas subdataWithRange:NSMakeRange(_m_readedSize, originalSize)];
 _m_readedSize += originalSize;
 }
 unsigned int outSize = 0;
 unsigned char* outBuffer = _m_recorderLisener->ResampleAudioData((unsigned char*)originalData.bytes, (unsigned int)originalData.length, &outSize);
 if (outBuffer != NULL)
 {
 audioData = [NSData dataWithBytes:outBuffer length:outSize];
 }
 else
 {
 YouMe_LOG_Error(__XT("resample error"));
 audioData = [NSData data];
 }
 NSLog(@"readData size = %d originalSize = %d resamplesize = %d", size, originalSize, outSize);
 return audioData;
 }
 }
 
 - (void) closeAudioIn
 {
    //NSLog(@"closeAudioIn");
    [[YouMeAudioRecorder Instance] StopSpeech];
 }*/

@end

/************************************** 带语音识别定义 end **************************************/


#else

/************************************** 不带语音识别定义 begin **************************************/

@implementation UscAudioRecognizer

-(void) SetAudioListener:(IAudioListener*)listener
{
    [[YouMeAudioRecorder Instance] SetAudioListener:listener];
}

-(void) InitRecognizer:(NSString*)appkey secret:(NSString*)secret recognizeType:(AudioRecognizeType)recognizeType
{
    
}

-(void) SetRecognizeLanguage:(RecognizeLanguage)language
{
    
}

-(void) SetAudioRecordParam:(int) sampleRate channel:(int) channel sampleBitSize:(int)sampleBitSize
{
    
}

-(int) StartSpeech:(NSString*)path serial:(unsigned long long)serial
{
    return [[YouMeAudioRecorder Instance] StartSpeech:path serial:serial recognizeType:RECOGNIZETYPE_NO];
}

-(int) StopSpeech
{
    return [[YouMeAudioRecorder Instance] StopSpeech];
}

-(int) CancelSpeech
{
    return [[YouMeAudioRecorder Instance] CancelSpeech];
}

-(bool) IsRecording
{
    return [[YouMeAudioRecorder Instance] IsRecording];
}

-(void) SetKeepRecordModel:(bool)keep;
{
    [[YouMeAudioRecorder Instance] SetKeepRecordModel:keep];
}

-(void) UnInit
{
    
}

@end

/************************************** 不带语音识别定义 end **************************************/

#endif
