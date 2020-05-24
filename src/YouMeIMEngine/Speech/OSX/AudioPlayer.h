#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H


#import <AVFoundation/AVFoundation.h>
#include <YouMeIMEngine/Speech/VoiceInterface.h>


class IAudioListener;

@interface AudioPlayer : NSObject<AVAudioPlayerDelegate>

+(instancetype) Instance;
-(void) SetAudioListener:(IAudioListener*)listener;
-(AudioErrorCode) StartPlayAudio:(NSString*)path;
-(AudioErrorCode) StopPlayAudio;
-(bool) IsPlaying;
-(void) SetPlayVolume:(float)volume;

@end


#endif
