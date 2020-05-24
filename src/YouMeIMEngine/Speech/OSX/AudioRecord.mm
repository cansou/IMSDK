#include "AudioRecord.h"
#include <YouMeCommon/Log.h>


@implementation AudioRecord
{
    bool isRecording;
    bool isCanceled;
    NSString *audioSessionCategory;
    float playVolume;
    bool keepRecordModel;
}

-(id) init{
    self = [super init];
    if (!self) {
        return nil;
    }
  
    self.audioSettingDic = [[NSMutableDictionary alloc] init];
    [self.audioSettingDic setValue:[NSNumber numberWithInt:kAudioFormatLinearPCM] forKey:AVFormatIDKey];  // 录音格式
    [self.audioSettingDic setValue:[NSNumber numberWithInt:16000] forKey:AVSampleRateKey];                 // 采样率
    [self.audioSettingDic setValue:[NSNumber numberWithInt:1] forKey:AVNumberOfChannelsKey];              // 声道
    [self.audioSettingDic setValue:[NSNumber numberWithInt:16] forKey:AVLinearPCMBitDepthKey];            // 样本位数
    [self.audioSettingDic setValue:[NSNumber numberWithBool:NO] forKey:AVLinearPCMIsFloatKey];            // 是否使用浮点数采样
    [self.audioSettingDic setValue:[NSNumber numberWithBool:NO] forKey:AVLinearPCMIsBigEndianKey];
    
    isRecording = false;
    isCanceled = false;
    playVolume = 1.0f;
    keepRecordModel = true;
    self.prepareRecording = NO;
    _queue = dispatch_queue_create("YIMStopAudioQueue", DISPATCH_QUEUE_SERIAL);

    return self;
}



-(void) SetAudioSetting:(int)sampleRate channel:(int)channel sampleBiteSize:(int)sampleBitSize{
    [self.audioSettingDic setObject:@(sampleRate) forKey:AVSampleRateKey];
    [self.audioSettingDic setObject:@(channel) forKey:AVNumberOfChannelsKey];
    [self.audioSettingDic setObject:@(sampleBitSize) forKey:AVLinearPCMBitDepthKey];
}

-(void) SetDelegate : (id) delegateObj{
    _recrodDelegate = delegateObj;
}

-(bool) StartRecord:(NSString*) path{
    if (path == nil || [path length] == 0) {
        YouMe_LOG_Error(__XT("record path is nil."));
        return false;
    }
	if (isRecording || isCanceled){
		YouMe_LOG_Error(__XT("record status error"));
		return false;
	}
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    if (fileManager && [fileManager fileExistsAtPath:path]) {
        [fileManager removeItemAtPath:path error:nil];
    }
    
    self.recordPath = path;
    NSError *error = nil;
    NSURL *urlAudio = [NSURL fileURLWithPath:path];
    self.audioRecorder = [[AVAudioRecorder alloc]initWithURL:urlAudio settings:self.audioSettingDic error:&error];
    self.audioRecorder.delegate = self;
    //_audioRecorder.meteringEnabled = YES; // 监控声波则为YES
    if (error) {
        YouMe_LOG_Error(__XT("create audio record failed, error：%s"), error.localizedDescription.UTF8String);
        return false;
    }
    
    [self.audioRecorder prepareToRecord];
    BOOL bRet = [self.audioRecorder record];
    NSLog(@"record");
    
    if (bRet == NO) {
        YouMe_LOG_Error(__XT("record audio error"));
        return false;
    }
    
    isRecording = true;
    return true;
}

-(void) StopRecord{
    if (self.audioRecorder) {
        [self.audioRecorder stop];
    }
}

-(void) CancelRecord{
    _forceStopRecording = YES;
    if (self.IsRecording && self.audioRecorder) {
        isCanceled = true;
        [self.audioRecorder stop];
    }
}

-(BOOL) IsRecording{
    if (self.audioRecorder) {
        if (self.audioRecorder.recording == YES || isRecording) {
            return YES;
        } else {
            return NO;
        }
    }
    return NO;
}

-(void)audioRecorderDidFinishRecording:(AVAudioRecorder *)recorder successfully:(BOOL)flag{
    dispatch_async(_queue, ^{
        
        if (isCanceled) {
            isCanceled = false;
            if (self.audioRecorder != nil) {
                [self.audioRecorder deleteRecording];
            }
        } else {
            if (self.recrodDelegate != NULL) {
                [self.recrodDelegate OnRecordFinish:flag path:self.recordPath];
            }
        }
        
        if (self.audioRecorder != nil) {
            self.audioRecorder = nil;
        }
        isRecording = false;
    });
}

- (void)audioRecorderEncodeErrorDidOccur:(AVAudioRecorder *)recorder error:(NSError * __nullable)error{
    isRecording = false;
    isCanceled = false;
    
    if (self.recrodDelegate != NULL) {
        [self.recrodDelegate OnRecordFinish:NO path:@""];
    }
}

- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag{
    if (self.audioPlayer != nil) {
        self.audioPlayer = nil;
    }
    if (self.recrodDelegate == nil) {
        return;
    }
    [self.recrodDelegate OnPlayFinish:flag ? 0 : -1 path:_playAudioPath];
}

- (void)audioPlayerDecodeErrorDidOccur:(AVAudioPlayer *)player error:(NSError *)error
{
    if (self.audioPlayer != nil) {
        self.audioPlayer = nil;
    }
    if (self.recrodDelegate == nil) {
        return;
    }
    [self.recrodDelegate OnPlayFinish:-1 path:_playAudioPath];
}

-(void) SetVolume:(float)volume
{
    playVolume = volume;
}

-(void)SetKeepRecordModel:(bool)keep{
    keepRecordModel = keep;
}

-(bool)GetKeepRecordModel{
    return keepRecordModel;
}

-(int) StartPlayAudio:(NSString*)path{
    if ([path length] == 0 || ![[NSFileManager defaultManager] fileExistsAtPath:path]) {
        return 4;
        YouMe_LOG_Error(__XT("file not exist:%s"), path.UTF8String);
    }
    
    int errorcode = 0;
    if (self.audioPlayer != nil && [self.audioPlayer isPlaying]) {
        errorcode = 5;
        [self.audioPlayer stop];
    }
    _playAudioPath = path;
    NSURL *url = [NSURL fileURLWithPath:path];
    NSError *error = nil;
    self.audioPlayer = [[AVAudioPlayer alloc]initWithContentsOfURL:url error:&error];
    if (self.audioPlayer == nil) {
        YouMe_LOG_Error(__XT("create audio player failed,error：%s"), error.localizedDescription.UTF8String);
        return -1;
    }
    self.audioPlayer.numberOfLoops = 0;
    self.audioPlayer.volume = playVolume;
    self.audioPlayer.delegate = self;
    BOOL ret = [self.audioPlayer prepareToPlay];
    if (!ret) {
        return -1;
    }
    ret = [self.audioPlayer play];
    
    if (!ret) {
        return -1;
    }
    return errorcode;
}

-(int) StopPlayAudio
{
    if (self.audioPlayer == nil || [self.audioPlayer isPlaying] == NO) {
        return 6;
    }
    [self.audioPlayer stop];
    return 0;
}

-(bool) IsPlaying
{
    if (self.audioPlayer == nil) {
        return false;
    } else {
        return [self.audioPlayer isPlaying];
    }
}
@end
