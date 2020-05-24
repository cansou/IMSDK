package com.youme.speech;

import java.io.ByteArrayOutputStream;
import java.math.BigDecimal;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import com.unisound.client.IAudioSource;
import com.unisound.client.SpeechConstants;
import com.unisound.client.SpeechUnderstander;
import com.unisound.client.SpeechUnderstanderListener;
import com.youme.voice.AudioErrorCode;
import com.youme.voice.AudioRecognizeType;
import com.youme.im.CommonConst;
import com.youme.im.IMEngine;
import com.youme.voice.IAudioRecordListener;
import com.youme.voice.ISpeechRecognize;
import com.youme.voice.YouMeAudioRecorder;

import android.content.Context;
import android.util.Log;


public class UscSpeechRecognizer implements SpeechUnderstanderListener, IAudioSource, ISpeechRecognize, IAudioRecordListener {
	public static final String LOG_TAG = "YouMe_IM";
	
	private Context m_context;
	private SpeechUnderstander m_speechRecognizer;
	private IAudioRecordListener m_recordListener;
	private long m_serial;
	private String m_audioPath;
	private StringBuilder m_audioText;
	private CommonConst.RecordStatus m_recordStatus = CommonConst.RecordStatus.RECORDSTATUS_IDLE;
	private int m_recognizeErrorcode = 0;
	private int m_sampleRate = CommonConst.SAMPLERATE_16K;
	private ByteArrayOutputStream m_audioData;
	private int m_readedSize = 0;
	private int m_audioErrorcode = AudioErrorCode.AUDIOERROR_SUCCESS.getValue();
	
	@Override
	public boolean Init(Context context){
		m_context = context;
		return true;
	}
	
	@Override
	public void UnInit(){
		
	}
	
	@Override
	public void SetAudioRecordListener(IAudioRecordListener listener){
		m_recordListener = listener;
	}
	
	@Override
	public void InitSpeechRecognizer(String appid, String secret, AudioRecognizeType recognizeType){
		if (m_speechRecognizer != null){
			return;
		}
		try {
			m_speechRecognizer = new SpeechUnderstander(m_context, appid, secret);
		}catch (Throwable e){
			e.printStackTrace();
		}
		if(m_speechRecognizer == null){
			Log.w(LOG_TAG, "m_speechRecognizer null");
			return;
		}
		m_speechRecognizer.setOption(SpeechConstants.NLU_ENABLE, false);											// 设置无语义结果
		m_speechRecognizer.setOption(SpeechConstants.ASR_DOMAIN, "general");										// 通用
		m_speechRecognizer.setOption(SpeechConstants.ASR_VAD_TIMEOUT_FRONTSIL, CommonConst.SPEECH_DURATION_MAX);	// 前端点超时(静音超时时间，即多长时间不说话则当做超时处理)
		m_speechRecognizer.setOption(SpeechConstants.ASR_VAD_TIMEOUT_BACKSIL, CommonConst.SPEECH_DURATION_MAX);		// 后端点超时(后端点静音检测时间，即停止说话多长时间内即认为不再输入， 自动停止录音)
		m_speechRecognizer.setOption(SpeechConstants.ASR_SAMPLING_RATE, m_sampleRate);								// 采样率
		m_speechRecognizer.setOption(SpeechConstants.ASR_LANGUAGE, SpeechConstants.LANGUAGE_MANDARIN);				// 识别语言
		
		m_speechRecognizer.setListener(this);
		m_speechRecognizer.setAudioSource(this);
		
		m_speechRecognizer.init("");
		
		m_audioText = new StringBuilder();
		m_audioData = new ByteArrayOutputStream();
		YouMeAudioRecorder.Instance().SetRecognizeListener(this);
	}
	
	@Override
	public void SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize){
		m_sampleRate = sampleRate;
		if (CommonConst.SAMPLERATE_8K == sampleRate || CommonConst.SAMPLERATE_16K == sampleRate){
			m_speechRecognizer.setOption(SpeechConstants.ASR_SAMPLING_RATE, sampleRate);
		} else {
			m_speechRecognizer.setOption(SpeechConstants.ASR_SAMPLING_RATE, CommonConst.SAMPLERATE_16K);
		}
	}
	
	@Override
	public void SetRecognizeLanguage(RecognizeLanguage language){	
		if (RecognizeLanguage.RECOGNIZELANG_YUEYU == language){
			m_speechRecognizer.setOption(SpeechConstants.ASR_LANGUAGE, SpeechConstants.LANGUAGE_CANTONESE);	// 粤语
		}else if(RecognizeLanguage.RECOGNIZELANG_ENGLISH == language){
			m_speechRecognizer.setOption(SpeechConstants.ASR_LANGUAGE, SpeechConstants. LANGUAGE_ENGLISH);	// 英文
		}else{
			m_speechRecognizer.setOption(SpeechConstants.ASR_LANGUAGE, SpeechConstants.LANGUAGE_MANDARIN);	// 普通话
		}
	}
	
	@Override
	public void UpdateToken(String token){
		
	}
	
	@Override
	public int StartSpeech(String path, long serial){
		Log.d(LOG_TAG, "StartSpeech");		
		
		if (m_speechRecognizer == null){
			Log.e(LOG_TAG, "not init");			
			return AudioErrorCode.AUDIOERROR_NOT_INIT.getValue();
		}
		
		if (m_recordStatus == CommonConst.RecordStatus.RECORDSTATUS_RECORDING) {
			Log.e(LOG_TAG, "recording");			
			return AudioErrorCode.AUDIOERROR_RECORDING.getValue();
		}
		
		m_recordStatus = CommonConst.RecordStatus.RECORDSTATUS_RECORDING;
		m_serial = serial;
		m_audioPath = path;
		m_audioText.setLength(0);
		m_recognizeErrorcode = 0;
		m_audioData.reset();
		m_readedSize = 0;
		m_audioErrorcode = AudioErrorCode.AUDIOERROR_SUCCESS.getValue();
		
		int errorcode = YouMeAudioRecorder.Instance().StartSpeech(m_audioPath, m_serial, AudioRecognizeType.RECOGNIZETYPE_IFLY_STREAM).getValue();
		if (errorcode == AudioErrorCode.AUDIOERROR_SUCCESS.getValue()){			
			m_speechRecognizer.start();
		}		

		return AudioErrorCode.AUDIOERROR_SUCCESS.getValue();
	}
	
	@Override
	public AudioErrorCode StopSpeech(){
		Log.d(LOG_TAG, "StopSpeech");
		
		if (m_speechRecognizer == null){
			Log.e(LOG_TAG, "StopSpeech not init");
			return AudioErrorCode.AUDIOERROR_NOT_INIT;
		}
		if (CommonConst.RecordStatus.RECORDSTATUS_RECORDING != m_recordStatus) {
			m_recordStatus = CommonConst.RecordStatus.RECORDSTATUS_CANCEL;
			try {
				AudioErrorCode errorcode = YouMeAudioRecorder.Instance().CancleSpeech();
				m_speechRecognizer.cancel();
			}catch (Throwable e){

			}
			return AudioErrorCode.AUDIOERROR_NOT_START_RECORD;
		}
		
		AudioErrorCode errorcode = YouMeAudioRecorder.Instance().StopSpeech();
		//m_speechRecognizer.stop();
		return errorcode;
	}
	
	@Override
	public AudioErrorCode CancleSpeech(){
		Log.d(LOG_TAG, "CancleSpeech");
		
		if (m_speechRecognizer == null){
			Log.e(LOG_TAG, "StopSpeech not init");
			return AudioErrorCode.AUDIOERROR_NOT_INIT;
		}
		if (CommonConst.RecordStatus.RECORDSTATUS_RECORDING != m_recordStatus) {
			return AudioErrorCode.AUDIOERROR_NOT_START_RECORD;
		}
		
		m_recordStatus = CommonConst.RecordStatus.RECORDSTATUS_CANCEL;
		AudioErrorCode errorcode = YouMeAudioRecorder.Instance().CancleSpeech();
		m_speechRecognizer.cancel();
		return errorcode;
	}
	
	
	//--------------------------SpeechUnderstanderListener---------------------------
	@Override
	public void onResult(int type, String jsonResult) {
		switch (type) {
		case SpeechConstants.ASR_RESULT_NET:
			// 在线识别结果，通常onResult接口多次返回结果，保留识别结果组成完整的识别内容。
			Log.d("USCSpeechRecognizer", "onRecognizerResult");
			if (jsonResult.contains("net_asr")) {
				try {
					JSONObject json = new JSONObject(jsonResult);
					JSONArray jsonArray = json.getJSONArray("net_asr");
					JSONObject jsonObject = jsonArray.getJSONObject(0);
					String status = jsonObject.getString("result_type");
					if (status.equals("partial")) {
						String result = (String)jsonObject.get("recognition_result");
						m_audioText.append(result);
					}
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
			break;
		default:
			break;
		}				
	}
	
	@Override
	public void onEvent(int type, int timeMs) {
		switch (type) {
		case SpeechConstants.ASR_EVENT_VAD_TIMEOUT:
			Log.d(LOG_TAG, "vad timeout");
			if (m_recordListener != null) {
				m_recordListener.OnRecordFinish(10118, "", m_serial, ""); // YIMErrorcode_PTT_NotSpeech
			}
			CancleSpeech();
			break;
		case SpeechConstants.ASR_EVENT_VOLUMECHANGE:
			// 音量变化
			int volume = (Integer)m_speechRecognizer.getOption(SpeechConstants.GENERAL_UPDATE_VOLUME);
			//Log.d(LOG_TAG, "volume:" + volume);
			if (m_recordListener != null){
				m_recordListener.OnRecordVolumeChange(volume, AudioRecognizeType.RECOGNIZETYPE_USC);
			}
			break;
		case SpeechConstants.ASR_EVENT_SPEECH_DETECTED:
			// 检测到说话
			Log.d(LOG_TAG, "speech started");
			break;
		case SpeechConstants.ASR_EVENT_RECORDING_START: {//录音设备打开，开始识别，用户可以开始说话
			Log.d(LOG_TAG, "record start");
			if (m_recordStatus != CommonConst.RecordStatus.RECORDSTATUS_IDLE) {
				m_recordStatus = CommonConst.RecordStatus.RECORDSTATUS_RECORDING;
			}
		}
			break;
		case SpeechConstants.ASR_EVENT_RECORDING_STOP:
			// 停止录音，请等待识别结果回调
			Log.d(LOG_TAG, "record stop");
			//m_recordStatus = CommonConst.RecordStatus.RECORDSTATUS_RECORDING;
			break;
		case SpeechConstants.ASR_EVENT_USERDATA_UPLOADED:
			Log.d(LOG_TAG, "userdata upload");
			break;
		case SpeechConstants.ASR_EVENT_NET_END:
			// NET识别结束
			Log.d(LOG_TAG, "recognize end text:" + m_audioText);

			if (m_recognizeErrorcode == 0 ){
				m_recordStatus = CommonConst.RecordStatus.RECORDSTATUS_IDLE;
				
				if (m_recordListener != null){
					String audioResult = m_audioText.toString();
					if(audioResult.indexOf("{}") > -1){
						Log.d(LOG_TAG, "recognize result remove {}");
						audioResult = audioResult.substring(2);
					}
					if(m_audioErrorcode != AudioErrorCode.AUDIOERROR_SUCCESS.getValue()){
						m_recordListener.OnRecordFinish(m_audioErrorcode, m_audioPath, m_serial, audioResult);
					}else{
						m_recordListener.OnRecordFinish(m_recognizeErrorcode == 0 ? m_recognizeErrorcode : AudioErrorCode.AUDIOERROR_RECOGNIZE_FAILED.getValue(), m_audioPath, m_serial, audioResult);
					}					
				}
			}
			m_recordStatus = CommonConst.RecordStatus.RECORDSTATUS_IDLE;
			break;
		default:
			Log.d(LOG_TAG, "type:" + type);
		}
	}
	
	@Override
	public void onError(int type, final String errorMSG) {
		if (errorMSG != null) {
			try {
				JSONObject json = new JSONObject(errorMSG);
				m_recognizeErrorcode = json.getInt("errorCode");
				String errText = json.getString("errorMsg");
				IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_ERROR, "usc onError type:" + type + " errcode:" + m_recognizeErrorcode + " text:" + errText);
			} catch (JSONException e) {
				e.printStackTrace();
			}
			
		} else {
			IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_ERROR, "usc onError type:" + type);
		}
		/*if (m_recordListener != null){
			m_recordListener.OnRecordFinish(errcode, m_audioPath, m_serial, m_audioText.toString());
		}*/
	}
	
	
	
	//--------------------------IAudioSource---------------------------
	@Override
	public int openAudioIn() {
		Log.d(LOG_TAG, "openAudioIn");
		return 0;
		/*int errorcode = YouMeAudioRecorder.Instance().StartSpeech(m_audioPath, m_serial, AudioRecognizeType.RECOGNIZETYPE_IFLY_STREAM).getValue();
		if (errorcode != AudioErrorCode.AUDIOERROR_SUCCESS.getValue()){
			if (m_recordListener != null){
				m_recordListener.OnRecordFinish(errorcode, "", m_serial, "");
			}
		}
		return errorcode;*/
	}

	@Override
	public int readData(byte[] buffer, int size) {
		//8k和16k直接识别无需重采样，并且OnRecordData 目前只做音量计算，识别中已有音量，不回调到上层
		if (m_sampleRate == CommonConst.SAMPLERATE_16K || m_sampleRate == CommonConst.SAMPLERATE_8K){
			int readLen = 0;
			synchronized(m_audioData){
				int lastSize = m_audioData.size() - m_readedSize;
				if (lastSize == 0){
					return 0;
				}
				readLen = lastSize < size ? lastSize : size;
				System.arraycopy(m_audioData.toByteArray(), m_readedSize, buffer, 0, readLen);
			}
			m_readedSize += readLen;
			return readLen;
		} else {
			int readLen = 0;
			int originalDataSize = (int) (size * round((double)m_sampleRate / CommonConst.SAMPLERATE_16K, 2, BigDecimal.ROUND_DOWN));
			byte[] originalData = null;
			synchronized(m_audioData){
				int lastSize = m_audioData.size() - m_readedSize;
				if (lastSize == 0){
					return 0;
				}
				readLen = lastSize < originalDataSize ? lastSize : originalDataSize;
				originalData = new byte[readLen];
				System.arraycopy(m_audioData.toByteArray(), m_readedSize, originalData, 0, readLen);
				m_readedSize += readLen;
			}
			
			byte[] resampleAudioData = m_recordListener.OnRecordData(originalData, readLen);
			if (resampleAudioData != null && resampleAudioData.length > 0){
				//Log.d(LOG_TAG, "input size:" + size + "  resample size:" + resampleAudioData.length + " readLen:" + readLen + " originalDataSize:" + originalDataSize + " m_readedSize:" + m_readedSize + " total:" + m_audioData.size());
				System.arraycopy(resampleAudioData, 0, buffer, 0, resampleAudioData.length);
				return resampleAudioData.length;
			} else {
				//Log.d(LOG_TAG, "not resample size:" + readLen);
				System.arraycopy(originalData, 0, buffer, 0, readLen);
				return readLen;
			}
		}
	}
	
	private double round(double value, int scale, int roundingMode) {
		BigDecimal bd = new BigDecimal(value);
		bd = bd.setScale(scale, roundingMode);
		double d = bd.doubleValue();
		bd = null;
		return d;
	}
	
	@Override
	public void closeAudioIn() {
		Log.d(LOG_TAG, "closeAudioIn");
	}
	
	@Override
	public int openAudioOut() {
		Log.d(LOG_TAG, "openAudioOut");
		return 0;
	}
	
	@Override
	public int writeData(byte[] buffer, int size) {
		Log.d(LOG_TAG, "writeData size:" + size);
		return 0;
	}
	
	@Override
	public void closeAudioOut() {
		Log.d(LOG_TAG, "closeAudioOut");
	}
	
	
	//--------------------------IAudioRecordListener---------------------------
	@Override
	public void OnRecordFinish(int errorcode, String path, long serial, String text){
		Log.i(LOG_TAG, "OnRecordFinish errorcode:" + errorcode);
		m_recordStatus = CommonConst.RecordStatus.RECORDSTATUS_IDLE;
		m_audioErrorcode = errorcode;
		if(errorcode == 0) {
			if(m_speechRecognizer != null) m_speechRecognizer.stop();
		}else{
			if(m_speechRecognizer != null) m_speechRecognizer.cancel();
		}
		if (m_recognizeErrorcode != 0 || errorcode != 0){
			if (m_recordListener != null){
				m_recordListener.OnRecordFinish(errorcode == 0 ? AudioErrorCode.AUDIOERROR_RECOGNIZE_FAILED.getValue() : errorcode, path, serial, text);
			}
		}
	}
	
	@Override
	public byte[] OnRecordData(byte buffer[], int size){
		if (size > 0){
			synchronized(m_audioData){
				m_audioData.write(buffer, 0, size);
			}
		}
		return null;
	}
	
	@Override
	public void OnRecordVolumeChange(int volume, AudioRecognizeType type){
		
	}
}
