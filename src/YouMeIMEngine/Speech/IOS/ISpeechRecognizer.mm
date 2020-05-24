#import "ISpeechRecognizer.h"


@implementation ISpeechRecognizer

-(void) SetAudioListener:(IAudioListener*)listener
{
    
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
    return 0;
}

-(int) StopSpeech
{
    return 0;
}

-(int) CancelSpeech
{
    return 0;
}

-(bool) IsRecording
{
    return false;
}

-(void) SetKeepRecordModel:(bool)keep
{
    
}

- (void)UnInit
{
    
}

-(void) UpdateToken:(NSString*) token
{
    
}

@end
