#import <AVFoundation/AVFoundation.h>

@protocol IAudioReordDelegate <NSObject>

- (void) OnRecordFinish:(BOOL)result path:(NSString*)path;
- (void) OnPlayFinish:(int)errorcode path:(NSString*)path;

@end


@interface AudioRecord : NSObject<AVAudioRecorderDelegate, AVAudioPlayerDelegate>

+ (BOOL)isHeadsetPluggedIn;

-(void) SetAudioSetting:(int)sampleRate channel:(int)channel sampleBiteSize:(int)sampleBitSize;
-(bool) StartRecord:(NSString*)path;
-(void) StopRecord;
-(void) CancelRecord;
-(BOOL) IsRecording;
-(void) SetVolume:(float)volume;
-(int) StartPlayAudio:(NSString*)path;
-(int) StopPlayAudio;
-(bool) IsPlaying;
-(void)SetKeepRecordModel:(bool)keep;
-(bool)GetKeepRecordModel;



@property (nonatomic, assign) id<IAudioReordDelegate> recrodDelegate;
@property (nonatomic, strong) NSMutableDictionary *audioSettingDic;
@property (nonatomic, strong) AVAudioRecorder *audioRecorder;
@property (nonatomic, strong) AVAudioPlayer *audioPlayer;
@property (nonatomic, copy) NSString *recordPath;
@property (nonatomic, copy) NSString *playAudioPath;
@property (nonatomic, assign) BOOL forceStopRecording;
@property (nonatomic, assign) BOOL prepareRecording;
@property (nonatomic, strong) dispatch_queue_t queue;

@end
