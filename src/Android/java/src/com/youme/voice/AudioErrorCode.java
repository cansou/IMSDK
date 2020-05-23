package com.youme.voice;


//音频错误码，为方便统一处理，各平台共用（修改记得同步到C++和Android）
public enum AudioErrorCode{
	//-------------------common-------------------
    AUDIOERROR_SUCCESS(0),					// 成功
    AUDIOERROR_NOT_INIT(1),					// 未初始化
    AUDIOERROR_INIT_FAILED(2),				// 初始化失败
    AUDIOERROR_INVALID_PARAM(3),			// 参数错误
	AUDIOERROR_FILE_NOT_EXIT(4),			// 文件不存在
	AUDIOERROR_READWRITE_FILE_ERROR(5),		// 读写文件错误
	AUDIOERROR_CREATE_FILE_FAILED(6),		// 创建文件失败
	AUDIOERROR_NO_AUDIO_DEVICE(7),			// 无音频设备
	AUDIOERROR_NODRIVER(8),					// 驱动问题
	AUDIOERROR_DEVICE_STATUS_INVALID(9),	// 设备状态错误
	AUDIOERROR_UNSUPPORT_FORMAT(10),		// 不支持的格式
	AUDIOERROR_RESOLVE_FILE_ERROR(11),		// 解析文件错误
	//-------------------record-------------------
    AUDIOERROR_AUTHORIZE(100),				// 录音权限
    AUDIOERROR_RECORDING(101),				// 正在录音
    AUDIOERROR_NOT_START_RECORD(102),		// 未开始录音
    AUDIOERROR_START_RECORD_FAILED(103),	// 启动录音失败
    AUDIOERROR_STOP_RECORD_FAILED(104),		// 停止录音失败
	AUDIOERROR_WRITE_WAV_FAILED(105),		// 写WAV头失败
	AUDIOERROR_RESAMPLE_FAILED(106),		// 重采样错误
	AUDIOERROR_RECORD_TIMEOUT(107),       	// 录音超时
	AUDIOERROR_RECORD_TIME_TOO_SHORT(108),  // 录音时间太短
	AUDIOERROR_RECOGNIZE_FAILED(109),       // 语音识别失败(但录音成功)
	//-------------------play-------------------
    AUDIOERROR_NOT_START_PLAY(200),			// 未开始播放
    AUDIOERROR_PLAYING(201),                // 正在播放
    AUDIOERROR_START_PLAY_FAILED(202),      // 启动播放失败
	AUDIOERROR_STOP_PLAY_FAILED(203),       // 停止播放失败
	AUDIOERROR_PLAY_TIMEOUT(204),       	// 播放超时
	//------------------------------------------
	
    AUDIOERROR_OTHREERROR(999);				// 其他错误
    
    
    private int value;
    private AudioErrorCode(int value){
        this.value = value;
    } 
    public int getValue(){
        return this.value;
    }
};