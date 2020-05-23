package com.youme.voice;


import android.content.Context;

import com.youme.im.CommonConst;


public interface ISpeechRecognize{
	//语音识别语言
	public enum RecognizeLanguage
	{
		RECOGNIZELANG_MANDARIN(0),		// 普通话
		RECOGNIZELANG_YUEYU(1),			// 粤语
		RECOGNIZELANG_SICHUAN(2),		// 四川话
		RECOGNIZELANG_HENAN(3),			// 河南话
		RECOGNIZELANG_ENGLISH(4),		// 英语
		RECOGNIZELANG_TRADITIONAL(5);	// 繁体中文
		
		private int value;
		private RecognizeLanguage(int value){
			this.value = value;
		} 
		public int getValue(){
			return this.value;
		}
	};
	
	public abstract boolean Init(Context context);
	public abstract void UnInit();
	public abstract void SetAudioRecordListener(IAudioRecordListener listener);
	public abstract void InitSpeechRecognizer(String appid, String secret, CommonConst.AudioRecognizeType recognizeType);
	public abstract void SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize);
	public abstract void SetRecognizeLanguage(RecognizeLanguage language);
	public abstract int StartSpeech(String path, long serial);
	public abstract AudioErrorCode StopSpeech();
	public abstract AudioErrorCode CancleSpeech();
	public abstract void UpdateToken(String token);
}

