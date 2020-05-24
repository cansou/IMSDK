#ifndef VOICE_INTERFACE_H
#define VOICE_INTERFACE_H


#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>


#define CHANNEL_NUMBER 1
#define SAMPLE_BIT_SIZE 16
#define LANGUAGE_DEFAULT "zh_cn"
#define ACCENT_DEFAULT "mandarin"
#define SPEECH_DURATION_MAX 60
#define SPEECH_DURATION_MIN 1

#define SAMPLERATE_8K 8000
#define SAMPLERATE_16K 16000
#define SAMPLERATE_32K 32000
#define SAMPLERATE_44K 44100
#define SAMPLERATE_48K 48000


//音频错误码，为方便统一处理，各平台共用（修改记得同步到C++和Android）
enum AudioErrorCode
{
	//-------------------common-------------------
    AUDIOERROR_SUCCESS,					// 成功
    AUDIOERROR_NOT_INIT,				// 未初始化
    AUDIOERROR_INIT_FAILED,				// 初始化失败
    AUDIOERROR_INVALID_PARAM,			// 参数错误
	AUDIOERROR_FILE_NOT_EXIT,			// 文件不存在
	AUDIOERROR_READWRITE_FILE_ERROR,	// 读写文件错误
	AUDIOERROR_CREATE_FILE_FAILED,		// 创建文件失败
	AUDIOERROR_NO_AUDIO_DEVICE,			// 无音频设备
	AUDIOERROR_NODRIVER,				// 驱动问题
	AUDIOERROR_DEVICE_STATUS_INVALID,	// 设备状态错误
	AUDIOERROR_UNSUPPORT_FORMAT,		// 不支持的格式
	AUDIOERROR_RESOLVE_FILE_ERROR,		// 解析文件错误
	//-------------------record-------------------
    AUDIOERROR_AUTHORIZE = 100,			// 录音权限
    AUDIOERROR_RECORDING,				// 正在录音
    AUDIOERROR_NOT_START_RECORD,		// 未开始录音
    AUDIOERROR_START_RECORD_FAILED,		// 启动录音失败
    AUDIOERROR_STOP_RECORD_FAILED,		// 停止录音失败
	AUDIOERROR_WRITE_WAV_FAILED,		// 写WAV头失败
	AUDIOERROR_RESAMPLE_FAILED,			// 重采样错误
	AUDIOERROR_RECORD_TIMEOUT,       	// 录音超时
	AUDIOERROR_RECORD_TIME_TOO_SHORT,   // 录音时间太短
	AUDIOERROR_RECOGNIZE_FAILED,       	// 语音识别失败(但录音成功)
	//-------------------play-------------------
    AUDIOERROR_NOT_START_PLAY = 200,	// 未开始播放
    AUDIOERROR_PLAYING,                 // 正在播放
    AUDIOERROR_START_PLAY_FAILED,       // 启动播放失败
	AUDIOERROR_STOP_PLAY_FAILED,       	// 停止播放失败
	AUDIOERROR_PLAY_TIMEOUT,       		// 播放超时
	//------------------------------------------
    
    AUDIOERROR_OTHREERROR = 999			// 其他错误
};

//音频设备状态
enum AudioDeviceState
{
	AUDIOSTATUS_AVIAIBLE,			// 可用
	AUDIOSTATUS_NO_AUTHORIZE,		// 没有权限
	AUDIOSTATUS_MUTE,				// 静音
	AUDIOSTATUS_NOT_AVIAIBLE		// 不可用
	//AUDIOSTATUS_RECORDING,		// 正在录音
	//AUDIOSTATUS_SPEAKERPHONE_OFF	// 扬声器关闭
};

//语音识别方式
enum AudioRecognizeType
{
	RECOGNIZETYPE_NO, 	 		// 无语音识别
	RECOGNIZETYPE_IFLY_RECORD,  // 讯飞录音
	RECOGNIZETYPE_IFLY_STREAM, 	// 讯飞音频流
	RECOGNIZETYPE_USC,  		// 云知声
	RECOGNIZETYPE_ALI			// 阿里
};

//语音识别语言
enum RecognizeLanguage
{
	RECOGNIZELANG_MANDARIN,		// 普通话(Android IOS Windows)
	RECOGNIZELANG_YUEYU,		// 粤语(Android IOS Windows)
	RECOGNIZELANG_SICHUAN,		// 四川话(Android IOS)
	RECOGNIZELANG_HENAN,		// 河南话(IOS)
	RECOGNIZELANG_ENGLISH,		// 英语(Android IOS Windows)
	RECOGNIZELANG_TRADITIONAL	// 繁体中文(Android IOS Windows)
};

class IAudioListener
{
public:
	virtual ~IAudioListener(){};
	virtual void OnRecordFinish(int errorcode, const XString& path, unsigned long long serial, const XString& text) = 0;
	virtual void OnAudioRecordData(unsigned char* buffer, unsigned int size) = 0;
	virtual void OnPlayFinish(int errorcode, const XString& path) = 0;
	virtual void OnAudioPlayData(unsigned char* buffer, unsigned int size) = 0;
	virtual void OnRecordVolumeChange(int volume, int type) = 0;
	// 为方便处理将重采样放到回调里面
	virtual unsigned char* ResampleAudioData(unsigned char* inBuffer, unsigned int inSize, unsigned int* outSize) = 0;
	//virtual void DestryResampleAudioData(unsigned char* buffer) = 0;
};

/*class IAudioRecordListener
{
public:
	//virtual void OnSpeechInit(int iErrorcode) = 0;
	virtual void OnSpeechResult(int errorcode, XUINT64 serial, const XString& text, const XString& path) = 0;
	virtual void OnAudioRecordData(unsigned char* buffer, unsigned int size) = 0;
};

class IAudioPlayListener
{
public:
	virtual void OnAudioPlayData(unsigned char* buffer, unsigned int size) = 0;
	virtual void OnAudioPlayCompletion(int errorcode, const XString& path) = 0;
};*/

class IYoueVoiceManager
{
public:
	virtual ~IYoueVoiceManager(){}
	
	virtual bool Init(const std::string& appkey, const std::string& secret, AudioRecognizeType audioRecognizeType) = 0;
	virtual void UnInit() = 0;
	//virtual void SetAudioRecordListener(IAudioRecordListener* listener) = 0;
	//virtual void SetAudioPlayListener(IAudioPlayListener* listener) = 0;
	virtual void SetAudioListener(IAudioListener* listener) = 0;

	//--------------------------audio record--------------------------
	virtual void SetAudioRecordCacheDir(const XString& path) = 0;
	virtual XString GetAudioRecordCacheDir() = 0;
	virtual void SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize) = 0;
	virtual void SetRecognizeLanguage(RecognizeLanguage language) = 0;
	virtual void SetKeepRecordModel(bool keep) = 0;	// 针对IOS语音结束后是否保持原Category
	virtual int StartSpeech(XUINT64 serial) = 0;
	virtual int StartOnlySpeech(XUINT64 serial) = 0;
	virtual int StopSpeech() = 0;
	virtual int CancleSpeech() = 0;
	virtual void UpdateToken(const std::string& token) = 0;
	
	//--------------------------audio play--------------------------
	virtual int StartPlayAudio(const XString& path) = 0;
	virtual int StopPlayAudio() = 0;
	virtual bool IsPlaying() = 0;
	virtual void SetPlayVolume(float volume) = 0;
	
	virtual int GetMicrophoneStatus() = 0;
};

#endif
