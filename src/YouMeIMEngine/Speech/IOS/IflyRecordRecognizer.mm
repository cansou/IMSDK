#import "IflyRecordRecognizer.h"
#include <YouMeCommon/Log.h>


/************************************** 带语音识别定义 begin **************************************/

//#undef IFLY_RECOGNIZE
#ifdef IFLY_RECOGNIZE
#import <iflyMSC.framework/Headers/IFlyMSC.h>
#include <YouMeIMEngine/Speech/SpeechUtil.h>

@interface IflyRecordRecognizer()<IFlySpeechRecognizerDelegate, IFlyPcmRecorderDelegate>

@property (nonatomic, assign) IAudioListener* m_recorderLisener;
@property (nonatomic, strong) IFlySpeechRecognizer *m_iFlySpeechRecognizer;//不带界面的识别对象
@property (nonatomic, strong) IFlyPcmRecorder *m_pcmRecorder;//录音器，用于音频流识别的数据传入
@property (nonatomic) unsigned long long m_ullCurSerial;
@property (nonatomic, strong) NSMutableString *m_curResult;
@property (nonatomic, strong) NSString *m_pcmPath;
@property (nonatomic) bool m_bGetSpeechResult;
@property (nonatomic, assign) BOOL m_pcmRecording;
@property (nonatomic, assign) BOOL m_forceCancelRecording;
@property (nonatomic) int m_count;
@property (nonatomic) int m_sampleRate;
@property (nonatomic, strong) NSString *m_audioSessionCategory;
@property (nonatomic) bool m_bKeepRecordModel;
@property (nonatomic, strong) dispatch_queue_t m_queue;

@end


@implementation IflyRecordRecognizer


-(instancetype) init
{
    self = [super init];
    if (!self)
    {
        return nil;
    }
    
    _m_recorderLisener = NULL;
    _m_pcmRecording = NO;
    _m_sampleRate = SAMPLERATE_16K;
    _m_queue = dispatch_queue_create("startOnlyAudioQueue", DISPATCH_QUEUE_SERIAL);
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(audioRouteChangeListenerCallback:)  name:AVAudioSessionRouteChangeNotification object:[AVAudioSession sharedInstance]];
    
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
    
    //设置sdk的log等级，log保存在下面设置的工作路径中
    [IFlySetting setLogFile:LVL_LOW];
    //打开输出在console的log开关
    [IFlySetting showLogcat:NO];
    
    //创建语音配置,appid必须要传入，仅执行一次则可
    NSString *initString = [[NSString alloc] initWithFormat:@"appid=%@", appkey];
    //所有服务启动前，需要确保执行createUtility
    [IFlySpeechUtility createUtility:initString];
    
    self.m_curResult = [[NSMutableString alloc]init];
    _m_iFlySpeechRecognizer = [IFlySpeechRecognizer sharedInstance];
    [_m_iFlySpeechRecognizer setParameter:[IFlySpeechConstant TYPE_CLOUD] forKey:[IFlySpeechConstant ENGINE_TYPE]];
    [_m_iFlySpeechRecognizer setParameter:@"json" forKey:[IFlySpeechConstant RESULT_TYPE]];
    [_m_iFlySpeechRecognizer setParameter:[IFlySpeechConstant ACCENT_MANDARIN] forKey:[IFlySpeechConstant ACCENT]];
    [_m_iFlySpeechRecognizer setParameter:[IFlySpeechConstant LANGUAGE_CHINESE] forKey:[IFlySpeechConstant LANGUAGE]];
    [_m_iFlySpeechRecognizer setParameter:@"60000" forKey:[IFlySpeechConstant VAD_BOS]];
    [_m_iFlySpeechRecognizer setParameter:@"60000" forKey:[IFlySpeechConstant VAD_EOS]];
    [_m_iFlySpeechRecognizer setParameter:@"1" forKey:[IFlySpeechConstant ASR_PTT]];
    [_m_iFlySpeechRecognizer setParameter:@"iat" forKey: [IFlySpeechConstant IFLY_DOMAIN]];
    [_m_iFlySpeechRecognizer setParameter:[NSString stringWithFormat:@"%d", SAMPLERATE_16K] forKey:[IFlySpeechConstant SAMPLE_RATE]];
    // [_m_iFlySpeechRecognizer setParameter:@"wav" forKey:[IFlySpeechConstant AUDIO_FORMAT]];
    [_m_iFlySpeechRecognizer setDelegate:self];
    
    //初始化录音器
    if (_m_pcmRecorder == nil)
    {
        _m_pcmRecorder = [IFlyPcmRecorder sharedInstance];
    }
    [_m_pcmRecorder setSample:[NSString stringWithFormat:@"%d", SAMPLERATE_16K]];
    _m_pcmRecorder.isNeedDeActive = NO;
    //[_pcmRecorder setSaveAudioPath:nil];    //不保存录音文件
    
    _m_bGetSpeechResult = false;
    _m_count = 0;
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
    if (sampleRate != SAMPLERATE_8K && sampleRate != SAMPLERATE_16K)
    {
        return;
    }
    _m_sampleRate = sampleRate;
    if (_m_iFlySpeechRecognizer != nil)
    {
        [_m_iFlySpeechRecognizer setParameter:[NSString stringWithFormat:@"%d", sampleRate] forKey:[IFlySpeechConstant SAMPLE_RATE]];
    }
    [_m_pcmRecorder setSample:[NSString stringWithFormat:@"%d", sampleRate]];
}

-(int) StartSpeech:(NSString*)path serial:(unsigned long long)serial;
{
    YouMe_LOG_Debug("StartSpeech enter");
    
    if ([_m_iFlySpeechRecognizer isListening])
    {
        return AUDIOERROR_RECORDING;
    }
    _m_ullCurSerial = serial;
    [self.m_curResult setString:@""];
    self.m_pcmPath = [path stringByAppendingString:@".pcm"];
    _m_forceCancelRecording = NO;
    _m_bGetSpeechResult = false;
    _m_count = 0;
    
    [_m_iFlySpeechRecognizer setParameter:IFLY_AUDIO_SOURCE_STREAM forKey:@"audio_source"];
    
    dispatch_async(_m_queue, ^{
        AVAudioSession *audioSession=[AVAudioSession sharedInstance];
        if (audioSession)
        {
            _m_audioSessionCategory = [audioSession category];
            // [audioSession setCategory:AVAudioSessionCategoryPlayAndRecord withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker error:nil];
            // 不使用AVAudioSessionCategoryOptionDefaultToSpeaker可以节约300ms
            if( ![_m_audioSessionCategory isEqualToString:AVAudioSessionCategoryPlayAndRecord] )
            {
                [audioSession setCategory:AVAudioSessionCategoryPlayAndRecord withOptions:AVAudioSessionCategoryOptionAllowBluetooth  error:nil];
                [audioSession setActive:YES error:nil];
            }
        }

        if (!_m_forceCancelRecording && YES == [_m_iFlySpeechRecognizer startListening])
        {
            dispatch_async(dispatch_get_main_queue(), ^{
                if(!_m_forceCancelRecording)
                {
                    _m_pcmRecording = YES;
                    _m_pcmRecorder.delegate = self;
                    [_m_pcmRecorder setSaveAudioPath:self.m_pcmPath];    //保存录音文件
                    if([_m_pcmRecorder start])
                    {
                        NSLog(@"_pcmRecorder started.");
                    }
                    else
                    {
                        NSLog(@"_pcmRecorder start failed.");
                        [self resetAudioCategory];
                        _m_recorderLisener->OnRecordFinish(AUDIOERROR_START_RECORD_FAILED, "", _m_ullCurSerial, "");
                    }
                }
                else
                {
                    [_m_iFlySpeechRecognizer cancel];
                }
            });
            
            if(![self IsHeadsetPluggedIn])
            {
                //NSLog(@"not HeadsetPluggedIn");
                [audioSession overrideOutputAudioPort:AVAudioSessionPortOverrideSpeaker error:nil];
            }
        }
        else if (!_m_forceCancelRecording)
        {
            [self resetAudioCategory];
            _m_recorderLisener->OnRecordFinish(AUDIOERROR_START_RECORD_FAILED, "", _m_ullCurSerial, "");
        }
        else
        {
            [self resetAudioCategory];
        }
    });
	
    YouMe_LOG_Debug("StartSpeech leave");
    
    return 0;
}

-(int) StopSpeech
{
    if (![_m_iFlySpeechRecognizer isListening])
    {
        [_m_iFlySpeechRecognizer stopListening];
        _m_forceCancelRecording = YES;
        [_m_pcmRecorder stop];
        _m_pcmRecording = NO;
        return AUDIOERROR_NOT_START_RECORD;
    }
    
    dispatch_async(_m_queue, ^{
        [_m_iFlySpeechRecognizer stopListening];
        //NSLog(@"StopSpeech _pcmRecorder stop");
        //[_pcmRecorder stop];
        //_pcmRecording = NO;
    });
    
    return AUDIOERROR_SUCCESS;
}

-(int) CancelSpeech
{
    if (![_m_iFlySpeechRecognizer isListening])
    {
        _m_forceCancelRecording = YES;
        return AUDIOERROR_NOT_START_RECORD;
    }
    
    dispatch_async(_m_queue, ^{
        NSLog(@"CancleSpeech _pcmRecorder stop");
        [_m_pcmRecorder stop];
        [_m_iFlySpeechRecognizer cancel];
        _m_pcmRecording = NO;
        NSFileManager *fileManager = [NSFileManager defaultManager];
        if (fileManager && [fileManager fileExistsAtPath:self.m_pcmPath])
        {
            [fileManager removeItemAtPath:self.m_pcmPath error:nil];
        }
    });
    
    return AUDIOERROR_SUCCESS;
}

-(bool) IsRecording
{
    if (_m_iFlySpeechRecognizer != nil)
    {
        return [_m_iFlySpeechRecognizer isListening] == YES;
    }
    return false;
}

-(void)SetKeepRecordModel:(bool)keep
{
    _m_bKeepRecordModel = keep;
}

-(void) UnInit
{
    if ([_m_iFlySpeechRecognizer isListening])
    {
        [_m_iFlySpeechRecognizer stopListening];
        [_m_pcmRecorder stop];
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
- (void) onVolumeChanged: (int)volume
{
}

/**
 开始识别回调
 ****/
- (void) onBeginOfSpeech
{
    NSLog(@"onBeginOfSpeech");
    _m_bGetSpeechResult = false;
}

/**
 停止录音回调
 ****/
- (void) onEndOfSpeech
{
    NSLog(@"onEndOfSpeech");
    //目前流模式没有这个回调
    dispatch_async(_m_queue, ^{
        [self resetAudioCategory];
    });
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
    NSLog(@"onError %d", errorcode);
    
    if(!_m_bGetSpeechResult)
    {
        if(_m_pcmRecording)
        {
            _m_pcmRecording = NO;
            NSLog(@"onError _pcmRecorder stop");
            [_m_pcmRecorder stop];
        }
        dispatch_async(_m_queue, ^{
            [self resetAudioCategory];
        });
    }
    
    if (/*!_m_bGetSpeechResult && */errorcode != 0)
    {
        //onError remove tmp file
        NSFileManager *fileManager = [NSFileManager defaultManager];
        if (fileManager)
        {
            [fileManager removeItemAtPath:self.m_pcmPath error:nil];
        }
        if (_m_recorderLisener != NULL)
        {
            _m_recorderLisener->OnRecordFinish(errorcode, "", _m_ullCurSerial, "");
        }
        NSLog(@"speech error(%d)", [error errorCode]);
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
    
    _m_bGetSpeechResult = true;
    NSMutableString *result = [[NSMutableString alloc] init];
    NSMutableString * resultString = [[NSMutableString alloc]init];
    NSDictionary *dic = results[0];
    for (NSString *key in dic)
    {
        [result appendFormat:@"%@",key];
        NSString * resultFromJson = [self stringFromJson:result];
        [resultString appendString:resultFromJson];
    }
    [self.m_curResult appendString:resultString];
    
    if (isLast)
    {
        NSLog(@"onResults _pcmRecorder stoping");
        _m_pcmRecording = NO;
        [_m_pcmRecorder stop];
        NSLog(@"onResults _pcmRecorder stoped");
        
        //把PCM 转换成wav然后上传
        NSString *strWavPath = [self.m_pcmPath substringToIndex:_m_pcmPath.length - 4];
        if (NULL != _m_recorderLisener)
        {
            dispatch_async(_m_queue, ^{
                
                NSLog(@"IFLY onResults");
                
                bool bSuccess = SpeechUtil::PcmToWav([_m_pcmPath UTF8String], SAMPLE_BIT_SIZE, CHANNEL_NUMBER, _m_sampleRate, 0, [strWavPath UTF8String]);
                if (!bSuccess)
                {
                    YouMe_LOG_Error(__XT("convert wav failed"));
                    _m_recorderLisener->OnRecordFinish(AUDIOERROR_WRITE_WAV_FAILED, "", _m_ullCurSerial, "");
                }
                else
                {
                    _m_recorderLisener->OnRecordFinish(AUDIOERROR_SUCCESS, [strWavPath UTF8String], _m_ullCurSerial, [_m_curResult UTF8String]);
                    NSFileManager *fileManager = [NSFileManager defaultManager];
                    if (fileManager)
                    {
                        [fileManager removeItemAtPath:_m_pcmPath error:nil];
                    }
                }
            });
            dispatch_async(_m_queue, ^{
                [self resetAudioCategory];
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
    
    dispatch_async(_m_queue, ^{
        [self resetAudioCategory];
    });
}

- (void) resetAudioCategory
{
    AVAudioSession *audioSession=[AVAudioSession sharedInstance];
    if (!_m_bKeepRecordModel && audioSession && _m_audioSessionCategory != nil && ![[audioSession category] isEqualToString:_m_audioSessionCategory])
    {
        //YouMe_LOG_Info(__XT("resetAudioCategory to:%s"),[_audioSessionCategory UTF8String]);
        [audioSession setCategory:_m_audioSessionCategory withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker error:nil];
    }
}


#pragma mark - IFlyPcmRecorderDelegate

- (void) onIFlyRecorderBuffer: (const void *)buffer bufferSize:(int)size
{
    NSData *audioBuffer = [NSData dataWithBytes:buffer length:size];
    if (![_m_iFlySpeechRecognizer writeAudio:audioBuffer])
    {
        //[_m_iFlySpeechRecognizer stopListening];
        NSLog(@"onIFlyRecorderBuffer writeAuio fail,stop.");
        [self StopSpeech];
    }
}

- (void) onIFlyRecorderError:(IFlyPcmRecorder*)recoder theError:(int) error
{
    NSLog(@"onIFlyRecorderError");
    
    [_m_pcmRecorder stop];
    [self StopSpeech];
}

//power:0-100,注意控件返回的音频值为0-30
- (void) onIFlyRecorderVolumeChanged:(int) power
{
    if (++_m_count > 25)
    {
        _m_recorderLisener->OnRecordVolumeChange(power, RECOGNIZETYPE_IFLY_RECORD);
        _m_count = 0;
    }
}


-(BOOL) IsHeadsetPluggedIn
{
    AVAudioSessionRouteDescription* route = [[AVAudioSession sharedInstance] currentRoute];
    for (AVAudioSessionPortDescription* desc in [route outputs])
    {
        if ([[desc portType] isEqualToString:AVAudioSessionPortHeadphones]
            || [[desc portType] isEqualToString:AVAudioSessionPortBluetoothHFP]
            || [[desc portType] isEqualToString:AVAudioSessionPortBluetoothLE])
            return YES;
    }
    return NO;
}

/**
 *  耳机通知
 *  @param notification 通知
 */
- (void)audioRouteChangeListenerCallback:(NSNotification*)notification
{
    NSDictionary *interuptionDict = notification.userInfo;
    if(interuptionDict==nil)
    {
        return;
    }
    NSInteger routeChangeReason = [[interuptionDict valueForKey:AVAudioSessionRouteChangeReasonKey] integerValue];
    switch (routeChangeReason) {
        case AVAudioSessionRouteChangeReasonNewDeviceAvailable:
        {
            NSLog(@"AVAudioSessionRouteChangeReasonNewDeviceAvailable");
            //插入耳机
            AVAudioSession *audioSessionT1 = [AVAudioSession sharedInstance];
                
            if(audioSessionT1 && [[audioSessionT1 category] isEqualToString:AVAudioSessionCategoryPlayAndRecord] )
            {
                if( [self IsHeadsetPluggedIn] )
                {
                    [audioSessionT1 overrideOutputAudioPort:AVAudioSessionPortOverrideNone error:nil];
                }else{
                    [audioSessionT1 overrideOutputAudioPort:AVAudioSessionPortOverrideSpeaker error:nil];
                }
            }
            
            break;
        }
            
        case AVAudioSessionRouteChangeReasonOldDeviceUnavailable:
        {
            NSLog(@"AVAudioSessionRouteChangeReasonOldDeviceUnavailable");
            //拔出耳机
            AVAudioSession *audioSessionT2=[AVAudioSession sharedInstance];
                
            if(audioSessionT2 && [[audioSessionT2 category] isEqualToString:AVAudioSessionCategoryPlayAndRecord] )
            {
                if( [self IsHeadsetPluggedIn] )
                {
                    [audioSessionT2 overrideOutputAudioPort:AVAudioSessionPortOverrideNone error:nil];
                }else{
                    [audioSessionT2 overrideOutputAudioPort:AVAudioSessionPortOverrideSpeaker error:nil];
                }
            }
            
            break;
        }
        case AVAudioSessionRouteChangeReasonCategoryChange:
            // called at start - also when other audio wants to play
            NSLog(@"AVAudioSessionRouteChangeReasonCategoryChange");
            break;
    }
}

@end

/************************************** 带语音识别定义 end **************************************/


#else

/************************************** 不带语音识别定义 begin **************************************/

#import "YouMeAudioRecorder.h"


@implementation IflyRecordRecognizer


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
