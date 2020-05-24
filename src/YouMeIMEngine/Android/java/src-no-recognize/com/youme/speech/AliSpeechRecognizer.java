package com.youme.speech;


import com.youme.voice.AudioErrorCode;
import com.youme.voice.AudioRecognizeType;
import com.youme.voice.AudioRecorder;
import com.youme.voice.IAudioRecordListener;
import com.youme.voice.ISpeechRecognize;
import com.youme.voice.YouMeAudioRecorder;

import android.content.Context;

public class AliSpeechRecognizer implements ISpeechRecognize {
	
	@Override
	public boolean Init(Context context){
		AudioRecorder.Instance().Init(context);
		return true;
	}
	
	@Override
	public void UnInit() {
		
	}
	
	@Override
	public void SetAudioRecordListener(IAudioRecordListener listener) {
		YouMeAudioRecorder.Instance().SetAudioRecordListener(listener);
	}
	
	@Override
	public void InitSpeechRecognizer(String appid, String secret, AudioRecognizeType recognizeType) {
		
	}	
	
	@Override
	public void SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize){
		YouMeAudioRecorder.Instance().SetAudioRecordParam(sampleRate, channel, sampleBitSize);
	}
	
	@Override
	public void SetRecognizeLanguage(RecognizeLanguage language){
		
	}
	
	@Override
	public int StartSpeech(String path, long serial){
		AudioErrorCode errorcode = YouMeAudioRecorder.Instance().StartSpeech(path, serial, AudioRecognizeType.RECOGNIZETYPE_NO);
		return errorcode.ordinal();
	}
	
	@Override
	public AudioErrorCode StopSpeech(){
		return YouMeAudioRecorder.Instance().StopSpeech();
	}
	
	@Override
	public AudioErrorCode CancleSpeech(){
		return YouMeAudioRecorder.Instance().CancleSpeech();
	}
	
	@Override
	public void UpdateToken(String token){
		
	}
}
