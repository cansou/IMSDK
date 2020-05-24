#include "AudioPlayer.h"
#include <YouMeCommon/Log.h>


@interface AudioPlayer()

@property (nonatomic, assign) IAudioListener* m_recorderLisener;
@property (nonatomic, strong) AVAudioPlayer* m_audioPlayer;
@property (nonatomic, copy) NSString* m_audioPath;
@property (nonatomic) float m_playVolume;

@end


@implementation AudioPlayer

+(instancetype) Instance
{
    static AudioPlayer *instance = nil;
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
    _m_recorderLisener = NULL;
    _m_playVolume = 1;
    return self;
}

-(void) SetAudioListener:(IAudioListener*)listener
{
    _m_recorderLisener = listener;
}

-(AudioErrorCode) StartPlayAudio:(NSString*)path
{
    NSLog(@"StartPlayAudio enter");
    
    if ([path length] == 0 || ![[NSFileManager defaultManager] fileExistsAtPath:path])
    {
        return AUDIOERROR_FILE_NOT_EXIT;
    }
    
    AudioErrorCode errorcode = AUDIOERROR_SUCCESS;
    if (_m_audioPlayer != nil && [_m_audioPlayer isPlaying])
    {
        errorcode = AUDIOERROR_PLAYING;
        [_m_audioPlayer stop];
    }
    _m_audioPath = path;
    NSURL *url = [NSURL fileURLWithPath:path];
    NSError *error = nil;
    _m_audioPlayer = [[AVAudioPlayer alloc]initWithContentsOfURL:url error:&error];
    if (nil == _m_audioPlayer)
    {
        YouMe_LOG_Error(__XT("create audio player failed,error：%s"), error.localizedDescription.UTF8String);
        return AUDIOERROR_START_PLAY_FAILED;
    }
    _m_audioPlayer.numberOfLoops = 0;
    _m_audioPlayer.volume = _m_playVolume;
    _m_audioPlayer.delegate = self;
    //_m_audioPlayer.meteringEnabled = YES; // 监控声波则为YES
    
    BOOL ret = [_m_audioPlayer prepareToPlay];
    if (!ret)
    {
        YouMe_LOG_Error(__XT("prepare play failed"));
        return AUDIOERROR_START_PLAY_FAILED;
    }
    ret = [_m_audioPlayer play];
    if (!ret)
    {
        YouMe_LOG_Error(__XT("start play failed"));
        return AUDIOERROR_START_PLAY_FAILED;
    }
    
    NSLog(@"StartPlayAudio leave");
    
    return errorcode;
}

-(AudioErrorCode) StopPlayAudio
{
    if (_m_audioPlayer == nil || ![_m_audioPlayer isPlaying])
    {
        return AUDIOERROR_NOT_START_PLAY;
    }
    [_m_audioPlayer stop];
    return AUDIOERROR_SUCCESS;
}

-(bool) IsPlaying
{
    if (_m_audioPlayer == nil)
    {
        return false;
    }
    else
    {
        return [_m_audioPlayer isPlaying];
    }
}

-(void) SetPlayVolume:(float)volume
{
    _m_playVolume = volume;
}


#pragma mark - AVAudioPlayerDelegate

- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag
{
    if (_m_audioPlayer != nil)
    {
        _m_audioPlayer = nil;
    }
    if (_m_recorderLisener != NULL)
    {
        _m_recorderLisener->OnPlayFinish(AUDIOERROR_SUCCESS, [_m_audioPath UTF8String]);
    }
}

- (void)audioPlayerDecodeErrorDidOccur:(AVAudioPlayer *)player error:(NSError *)error
{
    YouMe_LOG_Error(__XT("audio play error：%s"), error.localizedDescription.UTF8String);
    
    if (_m_audioPlayer != nil)
    {
        _m_audioPlayer = nil;
    }
    if (_m_recorderLisener != NULL)
    {
        _m_recorderLisener->OnPlayFinish(AUDIOERROR_START_PLAY_FAILED, [_m_audioPath UTF8String]);
    }
}

@end
