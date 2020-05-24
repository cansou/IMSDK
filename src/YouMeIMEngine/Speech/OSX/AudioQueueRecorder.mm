#import "AudioQueueRecorder.h"
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import <string>


#define BUFFER_NUMBER 3
#define BUFFER_TIME 0.2


struct AQRecorderState
{
    AudioStreamBasicDescription dataFormat;
    AudioQueueRef queue;
    AudioQueueBufferRef buffers[BUFFER_NUMBER];
    AudioFileID audioFile;
    UInt32 bufferByteSize;
    SInt64 currentPacket;
    AudioDeviceStatus status;
    unsigned long long startTime;
};

void HandleInputBuffer(void* aqData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer, const AudioTimeStamp* inStartTime, UInt32 inNumPackets, const AudioStreamPacketDescription* inPacketDesc)
{
    AQRecorderState* pAqData = (AQRecorderState*) aqData;
    if (NULL == pAqData)
    {
        NSLog(@"aqdata is null");
        return;
    }
    
//    NSLog(@"HandleInputBuffer inNumPackets:%lu byteSize:%lu running:%d time:%f", inNumPackets, inBuffer->mAudioDataByteSize, pAqData->status, [[NSProcessInfo processInfo] systemUptime] - pAqData->startTime);
    
    if (inBuffer->mAudioDataByteSize > 0)
    {
        OSStatus status = AudioFileWritePackets(pAqData->audioFile, false, inBuffer->mAudioDataByteSize, inPacketDesc, pAqData->currentPacket, &inNumPackets, inBuffer->mAudioData);
        if (status == noErr)
        {
            pAqData->currentPacket += inNumPackets;
        }
        else
        {
            NSLog(@"write file failed %ld", (long)status);
            
            if (AUDIOSTATUS_RECORDING == pAqData->status)
            {
                pAqData->status = AUDIOSTATUS_RECORD_ERROR;
                [[AudioQueueRecorder Instance] StopAudioQueue];
            }
        }
    }
    
    if (inBuffer->mAudioDataByteSize > 0 && inBuffer->mAudioData != NULL && [[AudioQueueRecorder Instance] recorderDelegate] != nil)
    {
        [[[AudioQueueRecorder Instance] recorderDelegate] OnAudioRecordData:(unsigned char*)inBuffer->mAudioData size:inBuffer->mAudioDataByteSize];
    }
    
    if (AUDIOSTATUS_RECORDING == pAqData->status)
    {
        AudioQueueEnqueueBuffer(pAqData->queue, inBuffer, 0, NULL);
        
        if ([[NSProcessInfo processInfo] systemUptime] - pAqData->startTime >= SPEECH_DURATION_MAX)
        {
            pAqData->status = AUDIOSTATUS_TIMEOUT;
            [[AudioQueueRecorder Instance] StopAudioQueue];
            return;
        }
    }
    else
    {
        *(int*)(inBuffer->mUserData) = 0;
        bool isStop = true;
        for (int i = 0; i < BUFFER_NUMBER; ++i)
        {
            if (*(int*)(pAqData->buffers[i]->mUserData) == 1)
            {
                isStop = false;
                break;
            }
        }
        if (!isStop)
        {
            return;
        }
        
        OSStatus status = AudioFileClose(pAqData->audioFile);
        if (status != noErr)
        {
            NSLog(@"close error(%ld)", (long)status);
        }
        
        [[AudioQueueRecorder Instance] ResetAudioCategory];
        
        if (AUDIOSTATUS_CANCEL == pAqData->status || AUDIOSTATUS_RECORD_TOO_SHORT == pAqData->status)
        {
            NSFileManager *fileManager = [NSFileManager defaultManager];
            if (fileManager)
            {
                NSString* audioPath = [[AudioQueueRecorder Instance] m_strAudioPath];
                [fileManager removeItemAtPath:audioPath error:nil];
            }
            
            if (AUDIOSTATUS_RECORD_TOO_SHORT == pAqData->status && [[AudioQueueRecorder Instance] recorderDelegate] != nil)
            {
                pAqData->status = AUDIOSTATUS_IDEL;
                [[[AudioQueueRecorder Instance] recorderDelegate] OnRecordFinish:AUDIOERROR_RECORD_TIME_TOO_SHORT path:@"" serial:[AudioQueueRecorder Instance].m_ullSerial];
            }
            
            pAqData->status = AUDIOSTATUS_IDEL;
        }
        else if (AUDIOSTATUS_STOP == pAqData->status || AUDIOSTATUS_TIMEOUT == pAqData->status)
        {
            AudioErrorCode errorcode = AUDIOSTATUS_TIMEOUT == pAqData->status ? AUDIOERROR_RECORD_TIMEOUT : AUDIOERROR_SUCCESS;
            if ([[AudioQueueRecorder Instance] recorderDelegate] != nil)
            {
                pAqData->status = AUDIOSTATUS_IDEL;
                
                NSString* audioPath = [[AudioQueueRecorder Instance] m_strAudioPath];
                [[[AudioQueueRecorder Instance] recorderDelegate] OnRecordFinish:errorcode path:audioPath serial:[AudioQueueRecorder Instance].m_ullSerial];
            }
            pAqData->status = AUDIOSTATUS_IDEL;
        }
        else if (AUDIOSTATUS_RECORD_ERROR == pAqData->status)
        {
            pAqData->status = AUDIOSTATUS_IDEL;
            [[[AudioQueueRecorder Instance] recorderDelegate] OnRecordFinish:AUDIOERROR_START_RECORD_FAILED path:@"" serial:[AudioQueueRecorder Instance].m_ullSerial];
        }
     }
}

UInt32 GetBufferSize(AudioQueueRef audioQueue, AudioStreamBasicDescription& ASBDescription, Float64 seconds)
{
    static const int maxBufferSize = 0x50000;
    int maxPacketSize = ASBDescription.mBytesPerPacket;
    if (maxPacketSize == 0)
    {
        UInt32 maxVBRPacketSize = sizeof(maxPacketSize);
        AudioQueueGetProperty(audioQueue, kAudioQueueProperty_MaximumOutputPacketSize, &maxPacketSize, &maxVBRPacketSize);
    }
    
    Float64 numBytesForTime = ASBDescription.mSampleRate * maxPacketSize * seconds;
    return UInt32(numBytesForTime < maxBufferSize ? numBytesForTime : maxBufferSize);
}


@interface AudioQueueRecorder()

@property (nonatomic) AQRecorderState m_aqData;
@property (nonatomic) bool m_bInit;
@property (nonatomic) int m_nSampleRate;
@property (nonatomic) int m_nChannel;
@property (nonatomic) int m_nSampleBitSize;
@property (nonatomic) bool m_bKeepRecordModel;
@property (nonatomic, copy) NSString* m_audioSessionCategory;

@end


@implementation AudioQueueRecorder

+(instancetype) Instance
{
    static AudioQueueRecorder *instance = nil;
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
    
    _m_bInit = false;
    _m_nSampleRate = SAMPLERATE_16K;
    _m_nChannel = CHANNEL_NUMBER;
    _m_nSampleBitSize = SAMPLE_BIT_SIZE;
    [self SetAudioParam:_m_nSampleRate channel:_m_nChannel sampleBiteSize:_m_nSampleBitSize];
    _m_bKeepRecordModel = true;
    
//    //耳机插拔事件
//    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(AudioRouteChangeListen:)  name:AVAudioSessionRouteChangeNotification object:[AVAudioSession sharedInstance]];
    
    return self;
}

-(bool) InitAudioQueue
{
    if (_m_bInit)
    {
        return true;
    }
    _m_aqData.currentPacket = 0;
    _m_aqData.status = AUDIOSTATUS_RECORDING;
    
    OSStatus status = AudioQueueNewInput(&_m_aqData.dataFormat, HandleInputBuffer, &_m_aqData, NULL, kCFRunLoopCommonModes, 0, &_m_aqData.queue);
    if (status != noErr)
    {
        NSLog(@"create audio error(%ld)", (long)status);
        return false;
    }
    
    UInt32 dataFormatSize = sizeof(_m_aqData.dataFormat);
    status = AudioQueueGetProperty(_m_aqData.queue, kAudioQueueProperty_StreamDescription, &_m_aqData.dataFormat, &dataFormatSize);
    if (status != noErr)
    {
        NSLog(@"get property error(%ld)", (long)status);
        return false;
    }
    
    _m_aqData.bufferByteSize = GetBufferSize(_m_aqData.queue, _m_aqData.dataFormat, BUFFER_TIME);
    for (int i = 0; i < BUFFER_NUMBER; ++i)
    {
        status = AudioQueueAllocateBuffer(_m_aqData.queue, _m_aqData.bufferByteSize, &_m_aqData.buffers[i]);
        if (status == noErr)
        {
            _m_aqData.buffers[i]->mUserData = new int;
        }
        else
        {
            NSLog(@"alloc buffer error(%ld)", (long)status);
            [self DeleteRecordBuffer];
            return false;
        }
    }
    
    /*status = AudioQueueAddPropertyListener(m_aqData.queue, kAudioQueueProperty_IsRunning, AudioQueueRecorder::PropertyListener, &m_aqData);
     if (status != noErr)
     {
     NSLog(@"set property listener failed");
     return false;
     }*/
    
    _m_bInit = true;
    
    return true;
}

-(void) UninitAudioQueue
{    
    OSStatus status = AudioQueueStop(_m_aqData.queue, true);
    if (status != noErr)
    {
        NSLog(@"stop queue error(%lu)", (long)status);
    }
    status = AudioFileClose(_m_aqData.audioFile);
    if (status != noErr)
    {
        NSLog(@"close error(%lu)", (long)status);
    }
    
    [self DeleteRecordBuffer];
    
    status = AudioQueueDispose(_m_aqData.queue, true);
    if (status != noErr)
    {
        NSLog(@"dispose queue(%lu)", (long)status);
    }
    
    _m_aqData.status = AUDIOSTATUS_IDEL;
    _m_bInit = false;
}

-(void) DeleteRecordBuffer
{
    for (int i = 0; i < BUFFER_NUMBER; ++i)
    {
        if (_m_aqData.buffers[i] != NULL && _m_aqData.buffers[i]->mUserData != NULL)
        {
            delete (int*)_m_aqData.buffers[i]->mUserData;
            _m_aqData.buffers[i]->mUserData = NULL;
        }
        if (_m_aqData.buffers[i] != NULL)
        {
            OSStatus status = AudioQueueFreeBuffer(_m_aqData.queue, _m_aqData.buffers[i]);
            if (status == noErr)
            {
                _m_aqData.buffers[i] = NULL;
            }
        }
    }
}

-(bool) StopAudioQueue
{
    NSLog(@"StopAudioQueue enter");
    
    OSStatus status = AudioQueueStop(_m_aqData.queue, false);
    if (status != noErr)
    {
        NSLog(@"dispose error(%ld)", (long)status);
        return false;
    }
    
    NSLog(@"StopAudioQueue leave");
    
    return true;
}

-(void) SetAudioParam:(int)sampleRate channel:(int)channel sampleBiteSize:(int)sampleBitSize
{
    _m_aqData.dataFormat.mFormatID = kAudioFormatLinearPCM;
    _m_aqData.dataFormat.mSampleRate = sampleRate;
    _m_aqData.dataFormat.mChannelsPerFrame = channel;
    _m_aqData.dataFormat.mBitsPerChannel = sampleBitSize;
    _m_aqData.dataFormat.mBytesPerPacket = (_m_aqData.dataFormat.mBitsPerChannel / 8) * _m_aqData.dataFormat.mChannelsPerFrame;
    _m_aqData.dataFormat.mBytesPerFrame = _m_aqData.dataFormat.mBytesPerPacket;
    _m_aqData.dataFormat.mFramesPerPacket = 1;
    _m_aqData.dataFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
    _m_aqData.dataFormat.mReserved = 0;
    
    _m_nSampleRate = sampleRate;
    _m_nChannel = channel;
    _m_nSampleBitSize = sampleBitSize;
}

-(AudioErrorCode) StartRecord:(NSString*)path serial:(unsigned long long)serial
{
    NSLog(@"StartRecord begin");
    if (_m_aqData.status != AUDIOSTATUS_IDEL)
    {
        NSLog(@"start record failed,recording,status:%d",_m_aqData.status);
        return AUDIOERROR_RECORDING;
    }
    _m_aqData.status = AUDIOSTATUS_RECORDING;
    
    if (!_m_bInit)
    {
        if (![self InitAudioQueue])
        {
            _m_aqData.status = AUDIOSTATUS_IDEL;
            return AUDIOERROR_INIT_FAILED;
        }
    }
    
    CFURLRef audioFileURL = CFURLCreateWithString(NULL, (__bridge CFStringRef)path, NULL);
    // 默认创建音频文件会对齐到4K页大小，导致data chunk之前填充一段内容为0的字节，kAudioFileFlags_DontPageAlignAudioData 消除填充
    OSStatus status = AudioFileCreateWithURL(audioFileURL, kAudioFileWAVEType, &_m_aqData.dataFormat, kAudioFileFlags_DontPageAlignAudioData, &_m_aqData.audioFile);
    CFRelease(audioFileURL);
    if (status != noErr)
    {
        NSLog(@"create file error(%ld)", (long)status);
        _m_aqData.status = AUDIOSTATUS_IDEL;
        return AUDIOERROR_CREATE_FILE_FAILED;
    }
    
    for (int i = 0; i < BUFFER_NUMBER; ++i)
    {
        *((int*)_m_aqData.buffers[i]->mUserData) = 1;
        OSStatus status = AudioQueueEnqueueBuffer(_m_aqData.queue, _m_aqData.buffers[i], 0, NULL);
        if (status != noErr)
        {
            NSLog(@"enqueue buffer error(%ld)", (long)status);
            return AUDIOERROR_START_RECORD_FAILED;
        }
    }
    
    _m_strAudioPath = path;
    _m_ullSerial = serial;
    _m_aqData.currentPacket = 0;
    status = AudioQueueStart(_m_aqData.queue, NULL);
    if (status != noErr)
    {
        NSLog(@"start record error(%ld)", (long)status);
        [self ResetAudioCategory];
        _m_aqData.status = AUDIOSTATUS_IDEL;
        return AUDIOERROR_START_RECORD_FAILED;
    }
    _m_aqData.startTime = [[NSProcessInfo processInfo] systemUptime];
    
    NSLog(@"StartRecord end");
    
    return AUDIOERROR_SUCCESS;
}

-(AudioErrorCode) StopRecord
{
    NSLog(@"StopRecord begin");
    
    if (_m_aqData.status != AUDIOSTATUS_RECORDING)
    {
        NSLog(@"not start record");
        return AUDIOERROR_NOT_START_RECORD;
    }
    _m_aqData.status = AUDIOSTATUS_STOP;
    //OSStatus status = AudioQueueStop(_m_aqData.queue, false);
    AudioQueueFlush(_m_aqData.queue);
    // 语音最小时长检查不在回调中检查，上面AudioQueueStop使用非立即停止方式，会不准确
    if ([[NSProcessInfo processInfo] systemUptime] - _m_aqData.startTime < SPEECH_DURATION_MIN)
    {
        _m_aqData.status = AUDIOSTATUS_RECORD_TOO_SHORT;
    }
    OSStatus status = AudioQueueStop(_m_aqData.queue, true);
    if (status != noErr)
    {
        NSLog(@"stop record error(%ld)", (long)status);
        return AUDIOERROR_STOP_RECORD_FAILED;
    }
    
    NSLog(@"StopRecord end");
    
    return AUDIOERROR_SUCCESS;
}

-(AudioErrorCode) CancelRecord
{
    NSLog(@"StopRecord begin");
    
    if (_m_aqData.status != AUDIOSTATUS_RECORDING)
    {
        NSLog(@"not start record");
        return AUDIOERROR_NOT_START_RECORD;
    }
    _m_aqData.status = AUDIOSTATUS_CANCEL;
    OSStatus status = AudioQueueStop(_m_aqData.queue, false);
    if (status != noErr)
    {
        NSLog(@"start record error(%ld)", (long)status);
        return AUDIOERROR_STOP_RECORD_FAILED;
    }
    
    NSLog(@"StopRecord end");
    
    return AUDIOERROR_SUCCESS;
}

-(bool) IsRecording
{
    return _m_aqData.status == AUDIOSTATUS_RECORDING;
}

-(void) SetKeepRecordModel:(bool)keep
{
    _m_bKeepRecordModel = keep;
}

-(void) ResetAudioCategory
{
    if (_m_bKeepRecordModel || _m_audioSessionCategory == nil)
    {
        return;
    }
}

-(void) AudioRouteChangeListen:(NSNotification*)notification
{
    NSDictionary *interuptionDict = notification.userInfo;
    if(nil == interuptionDict)
    {
        return;
    }
}

@end
