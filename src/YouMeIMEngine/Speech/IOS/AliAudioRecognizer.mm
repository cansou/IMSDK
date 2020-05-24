#import "AliAudioRecognizer.h"
#import "YouMeIMEngine/Speech/IOS/YouMeAudioRecorder.h"
#include "YouMeCommon/Log.h"


/************************************** 带语音识别定义 begin **************************************/
//#undef ALI_RECOGNIZE
#ifdef ALI_RECOGNIZE

#import "AliyunNlsSdk.framework/Headers/NlsSpeechRecognizerRequest.h"
#import "AliyunNlsSdk.framework/Headers/RecognizerRequestParam.h"
#import "AliyunNlsSdk.framework/Headers/AliyunNlsClientAdaptor.h"


@interface AliAudioRecognizer()<NlsSpeechRecognizerDelegate, IAudioQueueRecorderDelegate>

@property(nonatomic,strong) NlsClientAdaptor *m_nlsClient;
@property(nonatomic,strong) NlsSpeechRecognizerRequest *m_recognizeRequest;
@property(nonatomic,strong) RecognizerRequestParam *m_recognizeRequestParam;
@property (nonatomic, assign) IAudioListener* m_recorderLisener;
@property (nonatomic) int m_sampleRate;
@property (nonatomic, strong) NSMutableString *m_audioText;
@property (nonatomic, strong) NSString *m_audioPath;
@property (nonatomic) unsigned long long m_serial;
@property (nonatomic) unsigned long long m_recognizeErrorCode;
@property (nonatomic) AudioDeviceStatus m_status;
@property (nonatomic,assign) NSInteger m_readedSize;
@property (nonatomic, strong) NSMutableData *m_recordingDatas;
@property (nonatomic) AudioErrorCode m_recordErrorCode;
@property (nonatomic) bool m_isRecognizeStart;

@end


@implementation AliAudioRecognizer

-(instancetype) init
{
    self = [super init];
    if (!self)
    {
        return nil;
    }
    _m_recorderLisener = NULL;
    _m_sampleRate = SAMPLERATE_16K;
    _m_recordingDatas = [[NSMutableData alloc] init];
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
    _m_nlsClient = [[NlsClientAdaptor alloc]init];
    //[_m_nlsClient setLog:NULL logLevel:LOGINFO];  // 设置log级别
    
    _m_recognizeRequestParam = [[RecognizerRequestParam alloc]init];
    [_m_recognizeRequestParam setToken:secret];
    [_m_recognizeRequestParam setAppkey:appkey];
    [_m_recognizeRequestParam setFormat:@"opu"];
    [_m_recognizeRequestParam setEnableIntermediateResult:NO];
    [_m_recognizeRequestParam setEnablePunctuationPrediction:YES];
    
    [[YouMeAudioRecorder Instance] SetAudioRecognizeDelegate:self];
}

-(void) SetRecognizeLanguage:(RecognizeLanguage)language
{
  
}

-(void) SetAudioRecordParam:(int)sampleRate channel:(int)channel sampleBitSize:(int)sampleBitSize
{
    if (_m_sampleRate == sampleRate)
    {
        return;
    }
    _m_sampleRate = sampleRate;
    //int sample = sampleRate == SAMPLERATE_8K ? sampleRate : SAMPLERATE_16K;
    //[_m_speechUnderstander setOption:USC_ASR_SAMPLING_RATE value:[NSString stringWithFormat:@"%d", sample]];
}

-(int) StartSpeech:(NSString*)path serial:(unsigned long long)serial
{
    NSLog(@"StartSpeech enter");
    
    if (_m_status != AUDIOSTATUS_IDEL)
    {
        return AUDIOERROR_RECORDING;
    }
    _m_serial = serial;
    _m_audioPath = path;
    [_m_audioText setString:@""];
    [_m_recordingDatas setLength:0];
    _m_readedSize = 0;
    _m_status = AUDIOSTATUS_RECORDING;
    _m_recognizeErrorCode = 0;
    _m_recordErrorCode = AUDIOERROR_SUCCESS;
    _m_isRecognizeStart = false;
    
    AudioErrorCode errorcode = [[YouMeAudioRecorder Instance] StartSpeech:_m_audioPath serial:_m_serial recognizeType:RECOGNIZETYPE_ALI];
    if (errorcode == AUDIOERROR_SUCCESS)
    {
        if (_m_recognizeRequest != nil)
        {
            [_m_recognizeRequest releaseRequest];
            _m_recognizeRequest = nil;
        }
        _m_recognizeRequest = [_m_nlsClient createRecognizerRequest];
        if (_m_recognizeRequest)
        {
            [_m_recognizeRequest setRecognizeParams:_m_recognizeRequestParam];
            _m_recognizeRequest.delegate = self;
            [_m_recognizeRequest start];
            _m_isRecognizeStart = true;
        }
        else
        {
            _m_recognizeErrorCode = -1;
        }
    }
    
    NSLog(@"StartSpeech leave");
    
    return errorcode;
}

-(int) StopSpeech
{
    NSLog(@"StopSpeech enter");
    
    if (_m_status != AUDIOSTATUS_RECORDING)
    {
        return AUDIOERROR_NOT_START_RECORD;
    }
    
    _m_status = AUDIOSTATUS_STOP;
    
    AudioErrorCode errorcode = [[YouMeAudioRecorder Instance] StopSpeech];
    //[_m_recognizeRequest stop];
    
    NSLog(@"StopSpeech leave");
    
    return errorcode;
}

-(int) CancelSpeech
{
    NSLog(@"CancleSpeech enter");
    
    if (_m_status != AUDIOSTATUS_RECORDING)
    {
        return AUDIOERROR_NOT_START_RECORD;
    }
    _m_status = AUDIOSTATUS_CANCEL;
    
    AudioErrorCode errorcode = [[YouMeAudioRecorder Instance] CancelSpeech];
    if (AUDIOERROR_SUCCESS == errorcode)
    {
        _m_status = AUDIOSTATUS_IDEL;
    }
    [_m_recognizeRequest stop];    
    
    NSLog(@"CancleSpeech leave");
    
    return errorcode;
}

-(bool) IsRecording
{
    return _m_status == AUDIOSTATUS_RECORDING;
}

-(void) SetKeepRecordModel:(bool)keep
{
    [[YouMeAudioRecorder Instance] SetKeepRecordModel:keep];
}

- (void)UnInit
{
    if (_m_status == AUDIOSTATUS_RECORDING && _m_recognizeRequest)
    {
        [[YouMeAudioRecorder Instance] CancelSpeech];
        [_m_recognizeRequest stop];
    }
}

-(void) UpdateToken:(NSString*) token
{
    [_m_recognizeRequestParam setToken:token];
}


#pragma mark - NlsSpeechRecognizerDelegate

/**
 * @brief 错误回调函数
 * @note 在请求过程中出现错误时，触发该回调。用户可以在事件的消息头中检查状态码和状态消息，以确认失败的具体原因。
 * @param event 回调方法
 * @param statusCode 状态码
 * @param eMsg 错误消息
 */
-(void)OnTaskFailed:(NlsDelegateEvent)event statusCode:(NSString*)statusCode errorMessage:(NSString*)eMsg
{
    if (![statusCode isEqualToString:@"0"])
    {
        _m_recognizeErrorCode = -1;
        YouMe_LOG_Error(__XT("recognize error:%s, msg:%s"), [statusCode UTF8String],[eMsg UTF8String]);
    }
}

/**
 * @brief 与服务端连接关闭的回调
 * @note 在与服务端连接关闭时，触发该回调
 * @param event 回调方法
 * @param statusCode 状态码
 * @param eMsg 错误消息
 */
-(void)OnChannelClosed:(NlsDelegateEvent)event statusCode:(NSString*)statusCode errorMessage:(NSString*)eMsg
{
//    if (![statusCode isEqualToString:@"0"])
//    {
//        YouMe_LOG_Error(__XT("Ali OnChannelClosed, statusCode: %s,errMsg: %s"),[statusCode UTF8String],[eMsg UTF8String]);
//    }
}

/**
 * @brief 语音识别结束回调函数
 * @note 在语音识别完成时，触发该回调
 * @param event 回调方法
 * @param statusCode 状态码
 * @param result 识别最终结果
 * @param eMsg 错误消息
 */
-(void)OnRecognizedCompleted:(NlsDelegateEvent)event result:(NSString *)result statusCode:(NSString*)statusCode errorMessage:(NSString*)eMsg
{
    if (_m_status == AUDIOSTATUS_CANCEL)
    {
        _m_status = AUDIOSTATUS_IDEL;
        return;
    }
    if (_m_recognizeErrorCode != 0)
    {
        return;
    }
    _m_status = AUDIOSTATUS_IDEL;
    
    NSError *error = nil;
    NSDictionary *resObj = [NSJSONSerialization JSONObjectWithData:[result dataUsingEncoding:NSUTF8StringEncoding] options:NSJSONReadingAllowFragments error:&error];
    if (resObj)
    {
        NSDictionary *payload = [resObj objectForKey:@"payload"];
        if (payload)
        {
            NSString *text = [payload objectForKey:@"result"];
            if (text != nil)
            {
                [_m_audioText appendString:text];
            }
        }
    }
    _m_recorderLisener->OnRecordFinish(_m_recordErrorCode, [_m_audioPath UTF8String], _m_serial, [_m_audioText UTF8String]);
}

/**
 * @brief 语音识别中间结果回调函数
 * @note 只有response_mode为STREAMING，才会有中间结果
 * @param event 回调方法
 * @param statusCode 状态码
 * @param result 识别中间结果
 * @param eMsg 错误消息
 */
-(void)OnRecognizedResultChanged:(NlsDelegateEvent)event result:(NSString *)result statusCode:(NSString*)statusCode errorMessage:(NSString*)eMsg
{
    //NSLog(@"OnRecognizedResultChanged");
}


#pragma mark - IAudioQueueRecorderDelegate

- (void) OnAudioRecordData:(unsigned char*)buffer size:(unsigned int)size;
{
    // 只能加这个判断，估计阿里又作了修改，内部应该是线程发送数据，测试情况是stop之后又收到语音数据会报错:send audio data failed
    if (_m_status == AUDIOSTATUS_STOP)
    {
        return;
    }
    if (_m_recognizeRequest == nil || size == 0)
    {
        return;
    }
    
    if (_m_isRecognizeStart)
    {
        if ([_m_recordingDatas length] > 0)
        {
            if (_m_sampleRate != SAMPLERATE_8K && _m_sampleRate != SAMPLERATE_16K)
            {
                unsigned int audioDataSize = 0;
                unsigned char* audioData = _m_recorderLisener->ResampleAudioData((unsigned char*)_m_recordingDatas.mutableBytes, (unsigned int)_m_recordingDatas.length, &audioDataSize);
                if (audioData != NULL && audioDataSize > 0)
                {
                    [_m_recognizeRequest sendAudio:[NSData dataWithBytes:audioData length:audioDataSize] length:audioDataSize];
                }
                else
                {
                    YouMe_LOG_Error(__XT("resample error"));
                }
            }
            else
            {
                [_m_recognizeRequest sendAudio:_m_recordingDatas length:(unsigned int)_m_recordingDatas.length];
            }
            [_m_recordingDatas setLength:0];
        }
        
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
        [_m_recognizeRequest sendAudio:[NSData dataWithBytes:audioData length:audioDataSize] length:audioDataSize];
    }
    else
    {
        [_m_recordingDatas appendBytes:buffer length:size];
    }
    
    if (_m_recorderLisener != NULL)
    {
        _m_recorderLisener->OnAudioRecordData(buffer, size);
    }
}

- (void) OnRecordFinish:(AudioErrorCode)errorcode path:(NSString*)path serial:(unsigned long long)serial
{
    NSLog(@"OnRecordFinish errorcode:%d", errorcode);
    
    [_m_recognizeRequest stop];
    _m_status = AUDIOSTATUS_IDEL;
    _m_recordErrorCode = errorcode;
    /*if (errorcode != AUDIOERROR_SUCCESS)
    {
        [_m_recognizeRequest stop];
    }*/
    if (_m_recognizeErrorCode != 0)
    {
        _m_recorderLisener->OnRecordFinish(errorcode == AUDIOERROR_SUCCESS ? AUDIOERROR_RECOGNIZE_FAILED : errorcode, [_m_audioPath UTF8String], _m_serial, [_m_audioText UTF8String]);
    }
}

@end

/************************************** 带语音识别定义 end **************************************/


#else

/************************************** 不带语音识别定义 begin **************************************/

@implementation AliAudioRecognizer

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
