package com.youme.speech;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.json.JSONTokener;

import com.iflytek.cloud.InitListener;
import com.iflytek.cloud.RecognizerListener;
import com.iflytek.cloud.RecognizerResult;
import com.iflytek.cloud.SpeechConstant;
import com.iflytek.cloud.SpeechError;
import com.iflytek.cloud.SpeechRecognizer;
import com.iflytek.cloud.SpeechUtility;
import com.iflytek.msc.MSC;
import com.youme.im.CommonConst;
import com.youme.im.IMEngine;
import com.youme.voice.AudioErrorCode;
import com.youme.voice.IAudioRecordListener;
import com.youme.voice.ISpeechRecognize;
import com.youme.voice.YouMeAudioRecorder;
import com.youme.voice.AudioRecognizeType;
import com.youme.voice.AudioRecorder;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;

public class IflySpeechRecognizer implements InitListener, RecognizerListener, IAudioRecordListener, ISpeechRecognize {
	private Context m_context;
	//private int m_sampleRate = 16000;
	//int m_channel = 1;
	//int m_sampleBitSize = 16;
	private long m_serial;
	private String m_audioPath;
	private StringBuilder m_audioText = new StringBuilder();
	private SpeechRecognizer m_speechRecognizer = null;
	private IAudioRecordListener m_recordListener;
	private AudioRecognizeType m_recognizeType = AudioRecognizeType.RECOGNIZETYPE_IFLY_RECORD;
	//private YouMeAudioRecorder m_youmeAudioRecorder = null;
	//private boolean m_isSpeechTimeout = false;
	private int m_recognizeErrorcode = 0;
	private int m_audioErrorcode = AudioErrorCode.AUDIOERROR_SUCCESS.getValue();
	
	@Override
	public boolean Init(Context context){
		m_context = context;
		MSC.loadLibrary("msc");
		return true;
	}
	
	@Override
	public void UnInit() {
		Log.i(CommonConst.LOG_TAG, "UnInit:");
	}
	
	@Override
	public void SetAudioRecordListener(IAudioRecordListener listener) {
		m_recordListener = listener;
	}
	
	@Override
	public void InitSpeechRecognizer(String appid, String secret, AudioRecognizeType recognizeType) {
		String initParam = String.format("appid=%s,force_login=%s", appid, "true");
		try {
			SpeechUtility.createUtility(m_context, initParam);
			
			m_speechRecognizer = SpeechRecognizer.createRecognizer(m_context, this);
			if (null == m_speechRecognizer){
				Log.e(CommonConst.LOG_TAG, "create recognize failed");
				return;
			}
			m_speechRecognizer.setParameter(SpeechConstant.ENGINE_TYPE, SpeechConstant.TYPE_CLOUD);
			m_speechRecognizer.setParameter(SpeechConstant.RESULT_TYPE, "json");
			m_speechRecognizer.setParameter(SpeechConstant.LANGUAGE, "zh_cn");
			m_speechRecognizer.setParameter(SpeechConstant.ACCENT, "mandarin");
			m_speechRecognizer.setParameter(SpeechConstant.DOMAIN, "iat");
			m_speechRecognizer.setParameter(SpeechConstant.VAD_BOS, "60000");
			m_speechRecognizer.setParameter(SpeechConstant.VAD_EOS, "60000");
			m_speechRecognizer.setParameter(SpeechConstant.ASR_PTT, "1");
			m_speechRecognizer.setParameter(SpeechConstant.AUDIO_FORMAT, "wav");
			m_speechRecognizer.setParameter(SpeechConstant.SAMPLE_RATE, "16000");
			
			if (AudioRecognizeType.RECOGNIZETYPE_IFLY_STREAM == recognizeType) {
				IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_INFO, "recognize stream");
				
				m_recognizeType = recognizeType;
				m_speechRecognizer.setParameter(SpeechConstant.AUDIO_SOURCE, "-1");	// 音频流识别

				YouMeAudioRecorder.Instance().SetRecognizeListener(this);
			}
		}catch(Throwable e){
			m_speechRecognizer = null;
			Log.e("YouMeSpeechRecognizer", "SpeechRecognizer init error:");
			e.printStackTrace();
		}		
	}
	
	@Override
	public void SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize){
		//m_sampleRate = sampleRate;
		//m_channel = channel;
		//m_sampleBitSize = sampleBitSize;
		if (m_speechRecognizer != null){
			return;
		}
		String strSampleRate;
		if (CommonConst.SAMPLERATE_8K == sampleRate || CommonConst.SAMPLERATE_16K == sampleRate){
			strSampleRate = String.format("%d", sampleRate);
		} else {
			strSampleRate = String.format("%d", CommonConst.SAMPLERATE_16K);
		}
		m_speechRecognizer.setParameter(SpeechConstant.SAMPLE_RATE, strSampleRate);
	}
	
	@Override
	public void SetRecognizeLanguage(ISpeechRecognize.RecognizeLanguage language){
		IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_INFO, "recognize language:" + language);
		if (m_speechRecognizer != null) {
			String strAccent = "mandarin";
			String strLanguage = "zh_cn";
			if (ISpeechRecognize.RecognizeLanguage.RECOGNIZELANG_YUEYU == language)
			{
				strAccent = "cantonese";
			}
			else if (ISpeechRecognize.RecognizeLanguage.RECOGNIZELANG_SICHUAN == language)
			{
				strAccent = "lmz";
			}
			else if (ISpeechRecognize.RecognizeLanguage.RECOGNIZELANG_HENAN == language)
			{
				strAccent = "henanese";
			}
			else if (ISpeechRecognize.RecognizeLanguage.RECOGNIZELANG_ENGLISH == language)
			{
				strLanguage = "en_us";
				strAccent = "mandarin";
			}
			m_speechRecognizer.setParameter(SpeechConstant.LANGUAGE, strLanguage);
			m_speechRecognizer.setParameter(SpeechConstant.ACCENT, strAccent);
		}
	}
	
	@Override
	public void UpdateToken(String token){
		
	}
	
	@Override
	public int StartSpeech(String path, long serial){
		if (m_speechRecognizer == null){
			Log.e(CommonConst.LOG_TAG, "not init");
			return AudioErrorCode.AUDIOERROR_NOT_INIT.getValue();
		}
		
		if (m_speechRecognizer.isListening()) {
			Log.e(CommonConst.LOG_TAG, "recording");
			return AudioErrorCode.AUDIOERROR_RECORDING.getValue();
		}
		
		m_serial = serial;
		m_audioText = new StringBuilder();
		m_audioPath = path;
		m_recognizeErrorcode = 0;
		m_audioErrorcode = AudioErrorCode.AUDIOERROR_SUCCESS.getValue();
		if (AudioRecognizeType.RECOGNIZETYPE_IFLY_RECORD == m_recognizeType){
			//讯飞录音识别
			Log.d(CommonConst.LOG_TAG, "StartSpeech 1");
			
			m_speechRecognizer.setParameter(SpeechConstant.ASR_AUDIO_PATH, m_audioPath);
			return m_speechRecognizer.startListening(this);
		}else{
			//音频流识别
			Log.d(CommonConst.LOG_TAG, "StartSpeech 2");
			
			int errorcode = m_speechRecognizer.startListening(this);
			if (errorcode != 0){
				return errorcode;
			}
			errorcode = YouMeAudioRecorder.Instance().StartSpeech(path, serial, AudioRecognizeType.RECOGNIZETYPE_IFLY_STREAM).getValue();
			if (errorcode != 0){
				m_speechRecognizer.stopListening();
			}
			return errorcode;
		}
	}
	
	@Override
	public AudioErrorCode StopSpeech(){
		if (m_speechRecognizer == null){
			Log.e(CommonConst.LOG_TAG, "StopSpeech not init");
			return AudioErrorCode.AUDIOERROR_NOT_INIT;
		}
		
		if(AudioRecognizeType.RECOGNIZETYPE_IFLY_RECORD == m_recognizeType){
			if (!m_speechRecognizer.isListening()) {
				Log.e(CommonConst.LOG_TAG, "StopSpeech not start record");
				return AudioErrorCode.AUDIOERROR_NOT_START_RECORD;
			}
			m_speechRecognizer.stopListening();
			return AudioErrorCode.AUDIOERROR_SUCCESS;
		} else if (AudioRecognizeType.RECOGNIZETYPE_IFLY_STREAM == m_recognizeType){
			return YouMeAudioRecorder.Instance().StopSpeech();
		}
		return AudioErrorCode.AUDIOERROR_SUCCESS;
	}
	
	@Override
	public AudioErrorCode CancleSpeech(){
		if (m_speechRecognizer == null){
			Log.e(CommonConst.LOG_TAG, "not init");
			return AudioErrorCode.AUDIOERROR_NOT_INIT;
		}
		if (!m_speechRecognizer.isListening()) {
			Log.e(CommonConst.LOG_TAG, "not start record");
			return AudioErrorCode.AUDIOERROR_NOT_START_RECORD;
		}
		m_speechRecognizer.cancel();
		if(AudioRecognizeType.RECOGNIZETYPE_IFLY_RECORD == m_recognizeType){
			m_speechRecognizer.cancel();
			return AudioErrorCode.AUDIOERROR_SUCCESS;
		} else if (AudioRecognizeType.RECOGNIZETYPE_IFLY_STREAM == m_recognizeType){
			return YouMeAudioRecorder.Instance().CancleSpeech();
		}
		return AudioErrorCode.AUDIOERROR_SUCCESS;
	}
	
	
	//---------------------------InitListener---------------------------
	
	@Override
	public void onInit(int errorcode) {
		Log.i(CommonConst.LOG_TAG, "onInit:" + errorcode);
	}
	
	
	//---------------------------RecognizerListener---------------------------
	
	@Override
	public void onBeginOfSpeech() {
		Log.i(CommonConst.LOG_TAG, "onBeginOfSpeech");
	}

	@Override
	public void onEndOfSpeech() {
		Log.i(CommonConst.LOG_TAG, "onEndOfSpeech");
	}

	@Override
	public void onError(SpeechError errorcode) {
		int code = errorcode.getErrorCode();
		Log.e(CommonConst.LOG_TAG, "onError " + errorcode.getErrorCode());
		if (AudioRecognizeType.RECOGNIZETYPE_IFLY_RECORD == m_recognizeType && m_recordListener != null){
			m_recordListener.OnRecordFinish(errorcode.getErrorCode(), m_audioPath, m_serial, m_audioText.toString());
		}
		if (code == 10118){
			m_recognizeErrorcode = code;
		}
		if (AudioRecognizeType.RECOGNIZETYPE_IFLY_STREAM == m_recognizeType && code != 0 && !AudioRecorder.Instance().IsRecording()){
			m_recordListener.OnRecordFinish(m_audioErrorcode, m_audioPath, m_serial, m_audioText.toString());
		}
	}

	@Override
	public void onEvent(int arg0, int arg1, int arg2, Bundle arg3) {
		
	}

	@Override
	public void onResult(RecognizerResult result, boolean bIsLast) {
		Log.d(CommonConst.LOG_TAG, "onResult get recognize result");
		
		String strResult = result.getResultString();
		JSONTokener token = new JSONTokener(strResult);
		try {
			JSONObject jsonObject = new JSONObject(token);
			JSONArray wordsArray = jsonObject.getJSONArray("ws");
			for(int i=0;i<wordsArray.length();i++)
			{
				JSONArray wordItems = wordsArray.getJSONObject(i).getJSONArray("cw");
				//获取第一个
				if (wordItems.length() < 1) {
					continue;
				}
				m_audioText.append(wordItems.getJSONObject(0).getString("w"));
			}
		} catch (JSONException e) {
			IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_ERROR, "json resolve error");
			e.printStackTrace();
		}
		
		if (bIsLast) {
			Log.i(CommonConst.LOG_TAG, "recognize end:" + m_audioText.toString());
			
			if (AudioRecorder.Instance().IsRecording()){
				Log.i(CommonConst.LOG_TAG, "recognize end status:recording");
				m_recognizeErrorcode = 999999999;
				return;
			}
			
			if (m_recordListener != null){
				int errorcode = m_audioErrorcode;
				if (!(m_audioErrorcode == AudioErrorCode.AUDIOERROR_RECORD_TIMEOUT.getValue() || m_audioErrorcode == AudioErrorCode.AUDIOERROR_RECORD_TIME_TOO_SHORT.getValue())) {
					errorcode = AudioErrorCode.AUDIOERROR_SUCCESS.getValue();
				}
				m_recordListener.OnRecordFinish(errorcode, m_audioPath, m_serial, m_audioText.toString());
			}
		}
	}

	@Override
	public void onVolumeChanged(int volume, byte[] data) {
		if (m_recordListener != null){
			m_recordListener.OnRecordVolumeChange(volume, AudioRecognizeType.RECOGNIZETYPE_IFLY_RECORD);
		}
	}
	
	
	//---------------------------IAudioRecordListener---------------------------
	
	@Override
	public void OnRecordFinish(int errorcode, String path, long serial, String text) {
		Log.i(CommonConst.LOG_TAG, "OnRecordFinish errorcode:" + errorcode + " type:" + m_recognizeType);
		m_audioErrorcode = errorcode;
		//m_isSpeechTimeout = AudioErrorCode.AUDIOERROR_RECORD_TIMEOUT.getValue() == errorcode ? true : false;
		if (m_speechRecognizer != null){
			m_speechRecognizer.stopListening();
		}
		if (AudioRecognizeType.RECOGNIZETYPE_IFLY_STREAM == m_recognizeType && m_recognizeErrorcode != 0){
			m_recordListener.OnRecordFinish(errorcode, m_audioPath, m_serial, m_audioText.toString());
		}
	}
	
	@Override
	public byte[] OnRecordData(byte[] buffer, int size) {
		if (m_recordListener == null){
			Log.e(CommonConst.LOG_TAG, "record listener is null");
			return null;
		}
		if (m_speechRecognizer == null){
			Log.e(CommonConst.LOG_TAG, "recognizer is null");
			return null;
		}
		byte[] resampleAudioData = m_recordListener.OnRecordData(buffer, size);
		if (resampleAudioData != null && resampleAudioData.length > 0){
			m_speechRecognizer.writeAudio(resampleAudioData, 0, resampleAudioData.length);
		} else {
			m_speechRecognizer.writeAudio(buffer, 0, size);
		}
		return null;
	}
	
	@Override
	public void OnRecordVolumeChange(int volume, AudioRecognizeType type){
		
	}
}
