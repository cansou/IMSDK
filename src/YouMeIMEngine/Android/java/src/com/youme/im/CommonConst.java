package com.youme.im;


public class CommonConst{
	
	public enum LogLevel{
		LOG_LEVEL_FATAL,
		LOG_LEVEL_ERROR,
		LOG_LEVEL_WARNING,
		LOG_LEVEL_INFO,
		LOG_LEVEL_DEBUG
	}
	
	public static final String LOG_TAG = "YouMe_IM";
	
	public static final int CHANNEL_NUMBER = 1;
	public static final int SAMPLE_BIT_SIZE = 16;
	public static final int SPEECH_DURATION_MAX = 60000;
	public static final int SPEECH_DURATION_MIN = 1000;
	
	public static final String LANGUAGE_DEFAULT = "zh_cn";
	public static final String ACCENT_DEFAULT = "mandarin";

	public static final int SAMPLERATE_8K = 8000;
	public static final int SAMPLERATE_16K = 16000;
	public static final int SAMPLERATE_32K = 32000;
	public static final int SAMPLERATE_44K = 44100;
	public static final int SAMPLERATE_48K = 48000;
	
	
	public enum RecordStatus{
		RECORDSTATUS_IDLE,
		RECORDSTATUS_RECORDING,
		RECORDSTATUS_STOP,
		RECORDSTATUS_CANCEL
	}
}