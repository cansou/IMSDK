#import "IflyAudioRecognizer.h"
#include <YouMeCommon/Log.h>
#import "AudioQueueRecorder.h"
#import "YouMeAudioRecorder.h"


/************************************** 带语音识别定义 begin **************************************/

//#undef IFLY_RECOGNIZE
#ifdef IFLY_RECOGNIZE
#import <iflyMSC.framework/Headers/IFlyMSC.h>


@interface IflyAudioRecognizer()<IFlySpeechRecognizerDelegate, IAudioQueueRecorderDelegate>

@property (nonatomic, assign) IAudioListener* m_recorderLisener;
@property (nonatomic, strong) IFlySpeechRecognizer* m_iFlySpeechRecognizer;
@property (nonatomic) int m_sampleRate;
@property (nonatomic, strong) NSMutableString *m_audioText;
@property (nonatomic, strong) NSString *m_audioPath;
@property (nonatomic) unsigned long long m_serial;
@property (nonatomic) unsigned long long m_recognizeErrorCode;
@property (nonatomic) AudioErrorCode m_recordErrorCode;
@property (nonatomic) AudioDeviceStatus m_status;
@property(nonatomic, strong) dispatch_queue_t m_queue;

@end


@implementation IflyAudioRecognizer

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
    if (_m_iFlySpeechRecognizer != nil)
    {
        return;
    }
    
    [IFlySetting setLogFile:LVL_LOW];   //设置sdk的log等级，log保存在下面设置的工作路径中
    [IFlySetting showLogcat:NO];        //打开输出在console的log开关
    NSString *initString = [[NSString alloc] initWithFormat:@"appid=%@", appkey];
    [IFlySpeechUtility createUtility:initString];
    
    _m_iFlySpeechRecognizer = [IFlySpeechRecognizer sharedInstance];
    [_m_iFlySpeechRecognizer setParameter:@"" forKey:[IFlySpeechConstant PARAMS]];
    //听写模式
    [_m_iFlySpeechRecognizer setParameter:@"iat" forKey: [IFlySpeechConstant IFLY_DOMAIN]];
    //引擎类型
    [_m_iFlySpeechRecognizer setParameter:[IFlySpeechConstant TYPE_CLOUD] forKey:[IFlySpeechConstant ENGINE_TYPE]];
    //数据格式
    [_m_iFlySpeechRecognizer setParameter:@"json" forKey:[IFlySpeechConstant RESULT_TYPE]];
    //设置最长录音时间
    [_m_iFlySpeechRecognizer setParameter:@"60000" forKey:[IFlySpeechConstant SPEECH_TIMEOUT]];
    //设置后端点
    [_m_iFlySpeechRecognizer setParameter:@"60000" forKey:[IFlySpeechConstant VAD_EOS]];
    //设置前端点
    [_m_iFlySpeechRecognizer setParameter:@"60000" forKey:[IFlySpeechConstant VAD_BOS]];
    //网络等待时间
    [_m_iFlySpeechRecognizer setParameter:@"20000" forKey:[IFlySpeechConstant NET_TIMEOUT]];
    //设置采样率
    int samplerate = _m_sampleRate == SAMPLERATE_8K ? _m_sampleRate : SAMPLERATE_16K;
    [_m_iFlySpeechRecognizer setParameter:[NSString stringWithFormat:@"%d", samplerate] forKey:[IFlySpeechConstant SAMPLE_RATE]];
    //设置语言
    [_m_iFlySpeechRecognizer setParameter:[IFlySpeechConstant LANGUAGE_CHINESE] forKey:[IFlySpeechConstant LANGUAGE]];
    //设置方言
    [_m_iFlySpeechRecognizer setParameter:[IFlySpeechConstant ACCENT_MANDARIN] forKey:[IFlySpeechConstant ACCENT]];
    //设置是否返回标点符号
    [_m_iFlySpeechRecognizer setParameter:@"1" forKey:[IFlySpeechConstant ASR_PTT]];
    [_m_iFlySpeechRecognizer setParameter:@"-1" forKey:@"audio_source"];
    
    [_m_iFlySpeechRecognizer setDelegate:self];
    
    [[YouMeAudioRecorder Instance] SetAudioRecognizeDelegate:self];
    
    _m_queue = dispatch_queue_create("RecognizeQueue", DISPATCH_QUEUE_SERIAL);
}

-(void) SetRecognizeLanguage:(RecognizeLanguage)language
{
    if (!_m_iFlySpeechRecognizer)
    {
        return;
    }
    
    NSString *strAccent = [IFlySpeechConstant ACCENT_MANDARIN];
    NSString *strLanguage = [IFlySpeechConstant LANGUAGE_CHINESE];
    if (RECOGNIZELANG_YUEYU == language)
    {
        strAccent = [IFlySpeechConstant ACCENT_CANTONESE];
    }
    else if (RECOGNIZELANG_SICHUAN == language)
    {
        strAccent = [IFlySpeechConstant ACCENT_SICHUANESE];
    }
    else if (RECOGNIZELANG_HENAN == language)
    {
        strAccent = [IFlySpeechConstant ACCENT_HENANESE];
    }
    else if (RECOGNIZELANG_ENGLISH == language)
    {
        strLanguage = [IFlySpeechConstant LANGUAGE_ENGLISH];
    }
    [_m_iFlySpeechRecognizer setParameter:strAccent forKey:[IFlySpeechConstant ACCENT]];
    [_m_iFlySpeechRecognizer setParameter:strLanguage forKey:[IFlySpeechConstant LANGUAGE]];
    
    NSLog(@"SetSpeechRecognizeParam accent:%@ language:%@", strAccent, strLanguage);
}

-(void) SetAudioRecordParam:(int)sampleRate channel:(int)channel sampleBitSize:(int)sampleBitSize
{
    _m_sampleRate = sampleRate;
    if (_m_iFlySpeechRecognizer != nil)
    {
        int samplerate = _m_sampleRate == SAMPLERATE_8K ? _m_sampleRate : SAMPLERATE_16K;
        [_m_iFlySpeechRecognizer setParameter:[NSString stringWithFormat:@"%d", samplerate] forKey:[IFlySpeechConstant SAMPLE_RATE]];
    }
}

-(int) StartSpeech:(NSString*)path serial:(unsigned long long)serial
{
    NSLog(@"StartSpeech enter %d", _m_status);
    
    if (_m_status != AUDIOSTATUS_IDEL)
    {
        return AUDIOERROR_RECORDING;
    }
    
    _m_status = AUDIOSTATUS_START_RECORD;
    _m_serial = serial;
    _m_audioPath = path;
    [_m_audioText setString:@""];
    _m_recognizeErrorCode = -9999;
    _m_recordErrorCode = AUDIOERROR_SUCCESS;
    
    AudioErrorCode errorcode = [[YouMeAudioRecorder Instance] StartSpeech:path serial:serial recognizeType:RECOGNIZETYPE_IFLY_STREAM];
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
        
        if (![_m_iFlySpeechRecognizer startListening])
        {
            YouMe_LOG_Error(__XT("start recognize failed"));
        }
        
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
    else
    {
        _m_status = AUDIOSTATUS_IDEL;
        YouMe_LOG_Error(__XT("start record error:%d"), errorcode);
    }
    
    NSLog(@"StartSpeech leave");
    
    return errorcode;
}

-(int) StopSpeech
{
    NSLog(@"StopSpeech enter status:%d", _m_status);
    
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
    NSLog(@"CancleSpeech enter status:%d", _m_status);
    
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
        [_m_iFlySpeechRecognizer cancel];
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
    if ([_m_iFlySpeechRecognizer isListening])
    {
        [_m_iFlySpeechRecognizer stopListening];
    }
}

-(NSString*) stringFromJson:(NSString*)params
{
    if (params == NULL)
    {
        return nil;
    }
    
    NSDictionary *resultDic  = [NSJSONSerialization JSONObjectWithData:[params dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:nil];
    NSMutableString *tempStr = [[NSMutableString alloc] init];
    if (resultDic!= nil)
    {
        NSArray *wordArray = [resultDic objectForKey:@"ws"];
        for (int i = 0; i < [wordArray count]; i++)
        {
            NSDictionary *wsDic = [wordArray objectAtIndex: i];
            NSArray *cwArray = [wsDic objectForKey:@"cw"];
            for (int j = 0; j < [cwArray count]; j++)
            {
                NSDictionary *wDic = [cwArray objectAtIndex:j];
                NSString *str = [wDic objectForKey:@"w"];
                [tempStr appendString: str];
            }
        }
    }
    return tempStr;
}


#pragma mark - IFlySpeechRecognizerDelegate

/**
 音量回调函数
 volume 0－30
 ****/
/*- (void) onVolumeChanged: (int)volume
{
    NSLog(@"volume:%d", volume);
}*/

/**
 开始识别回调
 ****/
- (void) onBeginOfSpeech
{
    NSLog(@"onBeginOfSpeech");
    
    _m_recognizeErrorCode = 0;
}

/**
 停止录音回调
 ****/
- (void) onEndOfSpeech
{
    NSLog(@"onEndOfSpeech");
}

/**
 听写结束回调（注：无论听写是否正确都会回调）
 error.errorCode =
 0     听写正确
 other 听写出错
 ****/
- (void) onError:(IFlySpeechError *) error
{
    int errorcode = [error errorCode];
    _m_recognizeErrorCode = errorcode;
    
    if (errorcode != 0)
    {
        YouMe_LOG_Info(__XT("recognize error:%d"), errorcode);
    }
}

/**
 无界面，听写结果回调
 results：听写结果
 isLast：表示最后一次
 ****/
- (void) onResults:(NSArray *) results isLast:(BOOL)isLast
{
    NSLog(@"onResults");
    
    if (_m_serial == -1)
    {
        return;
    }
    
    NSMutableString *resultString = [[NSMutableString alloc] init];
    NSDictionary *dic = results[0];
    for (NSString *key in dic)
    {
        [resultString appendFormat:@"%@",key];
    }
    NSString *resultFromJson =  [self stringFromJson:resultString];
    [_m_audioText appendString:resultFromJson];
    
    if (isLast)
    {
        NSLog(@"recognize end");
        
        if (_m_recognizeErrorCode != 0)
        {
            return;
        }
        if (NULL != _m_recorderLisener)
        {
            dispatch_async(_m_queue, ^{
                _m_recorderLisener->OnRecordFinish(_m_recordErrorCode, [_m_audioPath UTF8String], _m_serial, [_m_audioText UTF8String]);
            });
        }
    }
}

/**
 听写取消回调
 ****/
- (void) onCancel
{
    NSLog(@"onCancel");
}


#pragma mark - IAudioQueueRecorderDelegate

- (void) OnAudioRecordData:(unsigned char*)buffer size:(unsigned int)size
{
    if (NULL == _m_recorderLisener)
    {
        YouMe_LOG_Error(__XT("record listener is NULL"));
        return;
    }
    
    NSData* data;
    if (_m_sampleRate == SAMPLERATE_8K || _m_sampleRate == SAMPLERATE_16K)
    {
        data = [NSData dataWithBytes:buffer length:size];
    }
    else
    {
        unsigned int outSize = 0;
        unsigned char* outBuffer = _m_recorderLisener->ResampleAudioData(buffer, size, &outSize);
        if (outBuffer != NULL)
        {
            data = [NSData dataWithBytes:outBuffer length:outSize];
        }
        else
        {
            YouMe_LOG_Error(__XT("resample error"));
            return;
        }
    }
    [_m_iFlySpeechRecognizer writeAudio:data];
    
    _m_recorderLisener->OnAudioRecordData(buffer, size);
}

- (void) OnRecordFinish:(AudioErrorCode)errorcode path:(NSString*)path serial:(unsigned long long)serial
{
    NSLog(@"OnRecordFinish errorcode:%d serial:%llu status:%d", errorcode, serial, _m_status);
    
    _m_recordErrorCode = errorcode;
    [_m_iFlySpeechRecognizer stopListening];
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
            _m_recorderLisener->OnRecordFinish(errorcode, [path UTF8String], _m_serial, "");
        }
    }
}

@end

/************************************** 带语音识别定义 end **************************************/


#else

/************************************** 不带语音识别定义 begin **************************************/

@implementation IflyAudioRecognizer


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


