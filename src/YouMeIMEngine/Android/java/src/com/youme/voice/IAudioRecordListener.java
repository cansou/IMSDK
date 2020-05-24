package com.youme.voice;


public interface IAudioRecordListener{
	public abstract void OnRecordFinish(int errorcode, String path, long serial, String text);
	public abstract byte[] OnRecordData(byte buffer[], int size);
	public abstract void OnRecordVolumeChange(int volume, AudioRecognizeType type);
}

//音频设备状态
enum AudioDeviceStatus{
	AUDIOSTATUS_AVIAIBLE,			// 可用
	AUDIOSTATUS_NO_AUTHORIZE,		// 没有权限
	AUDIOSTATUS_MUTE,				// 静音
	AUDIOSTATUS_NOT_AVIAIBLE		// 不可用
	//AUDIOSTATUS_RECORDING,		// 正在录音
	//AUDIOSTATUS_SPEAKERPHONE_OFF	// 扬声器关闭
}