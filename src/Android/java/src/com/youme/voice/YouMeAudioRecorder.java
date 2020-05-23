package com.youme.voice;


import android.content.Context;

import com.youme.im.CommonConst;

public class YouMeAudioRecorder implements IAudioRecordListener {
	private IAudioRecordListener m_recordListener = null;
	private IAudioRecordListener m_recognizeListener = null;
	private static YouMeAudioRecorder s_instance = null;
	private CommonConst.AudioRecognizeType m_recognizeType = CommonConst.AudioRecognizeType.RECOGNIZETYPE_NO;
	
	public static YouMeAudioRecorder Instance()
	{
		if (s_instance == null) {
			s_instance = new YouMeAudioRecorder();
		}
		return s_instance;
	}
	
	public boolean Init(Context context){
		AudioRecorder.Instance().Init(context);
		AudioRecorder.Instance().SetRecordListener(this);
		return true;
	}
	
	public void Init(String appid) {
		
	}
	
	public void UnInit() {
		
	}
	
	public void SetAudioRecordListener(IAudioRecordListener listener) {
		//AudioRecorder.Instance().SetRecordListener(listener);
		m_recordListener = listener;
	}
	
	public void SetRecognizeListener(IAudioRecordListener listener) {
		m_recognizeListener = listener;
	}
	
	public void SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize){
		AudioRecorder.Instance().SetAudioRecordParam(sampleRate, channel, sampleBitSize);
	}
	
	public void SetSpeechRecognizeParam(String accent, String language){
		
	}
	
	public AudioErrorCode StartSpeech(String path, long serial, CommonConst.AudioRecognizeType recognizeType){
		m_recognizeType = recognizeType;
		return AudioRecorder.Instance().StartRecord(serial, path);
	}
	
	public AudioErrorCode StopSpeech(){
		return  AudioRecorder.Instance().StopRecord();
	}
	
	public AudioErrorCode CancleSpeech(){
		return AudioRecorder.Instance().CancelRecord();
	}
	
	public AudioDeviceStatus GetMicrophoneStatus(){
		return AudioRecorder.Instance().GetMicrophoneStatus();
	}

	
	@Override
	public void OnRecordFinish(int errorcode, String path, long serial, String text) {
		if (CommonConst.AudioRecognizeType.RECOGNIZETYPE_NO == m_recognizeType){
			if (m_recordListener != null){
				m_recordListener.OnRecordFinish(errorcode, path, serial, text);
			}
		} else {
			if (m_recognizeListener != null){
				m_recognizeListener.OnRecordFinish(errorcode, path, serial, text);
			}
		}
	}

	@Override
	public byte[] OnRecordData(byte[] buffer, int size) {
		if (CommonConst.AudioRecognizeType.RECOGNIZETYPE_NO == m_recognizeType){
			if (m_recordListener != null){
				m_recordListener.OnRecordData(buffer, size);
			}
		} else {
			if (m_recognizeListener != null){
				m_recognizeListener.OnRecordData(buffer, size);
			}
		}
		return null;
	}
	
	@Override
	public void OnRecordVolumeChange(int volume, CommonConst.AudioRecognizeType type){
		
	}
}
