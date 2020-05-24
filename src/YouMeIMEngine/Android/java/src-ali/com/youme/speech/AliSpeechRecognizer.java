package com.youme.speech;


import java.io.ByteArrayOutputStream;

import org.json.JSONException;
import org.json.JSONObject;

import com.alibaba.idst.util.NlsClient;
import com.alibaba.idst.util.SpeechRecognizer;
import com.alibaba.idst.util.SpeechRecognizerCallback;
import com.youme.im.CommonConst;
import com.youme.im.IMEngine;
import com.youme.voice.AudioErrorCode;
import com.youme.voice.AudioRecognizeType;
import com.youme.voice.IAudioRecordListener;
import com.youme.voice.ISpeechRecognize;
import com.youme.voice.YouMeAudioRecorder;

import android.content.Context;
import android.util.Log;


public class AliSpeechRecognizer implements ISpeechRecognize, IAudioRecordListener, SpeechRecognizerCallback {
	public static final String LOG_TAG = "YouMe_IM";
	
	private NlsClient m_nlsClient = null;
	private SpeechRecognizer m_speechRecognizer = null;
	private IAudioRecordListener m_recordListener = null;
	private long m_serial;
	private String m_audioPath;
	private CommonConst.RecordStatus m_recordStatus = CommonConst.RecordStatus.RECORDSTATUS_IDLE;
	private int m_sampleRate = CommonConst.SAMPLERATE_16K;
	private int m_audioErrorcode = 0;
	private Thread startThread;
	private Thread stopThread;
	private String m_appkey;
	private String m_secret;
	private ByteArrayOutputStream m_audioData;
	private boolean m_isRecognizeStart = false;
	private boolean m_callStartRecognizer = false;

	private ByteArrayOutputStream m_tmpAudioData;
	private int m_limitDataSize = 8000;
	
	@Override
	public boolean Init(Context context){
		return true;
	}
	
	@Override
	public void UnInit(){
		m_nlsClient.release();
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
		m_appkey = appid;
		m_secret = secret;
		YouMeAudioRecorder.Instance().SetRecognizeListener(this);
		m_nlsClient = new NlsClient();
		m_audioData = new ByteArrayOutputStream();
		m_tmpAudioData = new ByteArrayOutputStream();
	}
	
	@Override
	public void SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize){
		m_sampleRate = sampleRate;
	}
	
	@Override
	public void SetRecognizeLanguage(RecognizeLanguage language){	
		
	}
	
	@Override
	public void UpdateToken(String token){
		m_secret = token;
	}
	
	@Override
	public int StartSpeech(String path, long serial){
		Log.d(LOG_TAG, "Ali StartSpeech");
		
		if (m_nlsClient == null){
			return AudioErrorCode.AUDIOERROR_NOT_INIT.getValue();
		}
		if (m_recordStatus == CommonConst.RecordStatus.RECORDSTATUS_RECORDING) {
			Log.e(LOG_TAG, "recording");
			return AudioErrorCode.AUDIOERROR_RECORDING.getValue();
		}
		
		m_serial = serial;
		m_audioPath = path;
		m_audioErrorcode = 0;
//		m_isRecognizeStart = false;
		m_audioData.reset();
		m_tmpAudioData.reset();

		if (!m_callStartRecognizer && !m_isRecognizeStart) {
			m_speechRecognizer = m_nlsClient.createRecognizerRequest(this);
			m_speechRecognizer.setToken(m_secret);
			m_speechRecognizer.setAppkey(m_appkey);
			m_speechRecognizer.setSampleRate(CommonConst.SAMPLERATE_16K);
			m_speechRecognizer.enableIntermediateResult(false);
			m_speechRecognizer.enablePunctuationPrediction(true);

			startThread = new Thread(new Runnable() {
				public void run() {
					m_callStartRecognizer = true;
					int errorcode = m_speechRecognizer.start();

					if (errorcode < 0) {
						m_audioErrorcode = -1;
					}
				}
			}, "AudioRecognize1 Thread");
			startThread.start();
		}
		
//		int errStartRecognize = m_speechRecognizer.start();
//		if(errStartRecognize < 0) {
//			m_speechRecognizer.stop();
//			m_isRecognizeStart = false;
//			return AudioErrorCode.AUDIOERROR_START_RECORD_FAILED.getValue();
//		}

        int errorcode = YouMeAudioRecorder.Instance().StartSpeech(m_audioPath, m_serial, AudioRecognizeType.RECOGNIZETYPE_ALI).getValue();
		if (errorcode != AudioErrorCode.AUDIOERROR_SUCCESS.getValue()){
			if (m_isRecognizeStart) {
				m_speechRecognizer.stop();
				m_isRecognizeStart = false;
				m_callStartRecognizer = false;
			}

			return errorcode;
		}
		m_recordStatus = CommonConst.RecordStatus.RECORDSTATUS_RECORDING;
		return errorcode;
	}
	
	@Override
	public AudioErrorCode StopSpeech(){
		Log.d(LOG_TAG, "StopSpeech");
		
		if (m_speechRecognizer == null){
			Log.e(LOG_TAG, "StopSpeech not init");
			return AudioErrorCode.AUDIOERROR_NOT_INIT;
		}
		if (CommonConst.RecordStatus.RECORDSTATUS_RECORDING != m_recordStatus) {
			YouMeAudioRecorder.Instance().CancleSpeech();
			return AudioErrorCode.AUDIOERROR_NOT_START_RECORD;
		}
		m_recordStatus = CommonConst.RecordStatus.RECORDSTATUS_STOP;
		
		AudioErrorCode errorcode = YouMeAudioRecorder.Instance().StopSpeech();
		if (errorcode != AudioErrorCode.AUDIOERROR_SUCCESS){
			Log.e(LOG_TAG, "StopSpeech error:" + errorcode);
		}
		
		//m_speechRecognizer.stop();
		
		return errorcode;
	}
	
	@Override
	public AudioErrorCode CancleSpeech(){
		Log.d(LOG_TAG, "CancleSpeech");
		
		if (m_speechRecognizer == null){
			Log.e(LOG_TAG, "CancleSpeech not init");
			return AudioErrorCode.AUDIOERROR_NOT_INIT;
		}
		if (CommonConst.RecordStatus.RECORDSTATUS_RECORDING != m_recordStatus) {
			return AudioErrorCode.AUDIOERROR_NOT_START_RECORD;
		}
		m_recordStatus = CommonConst.RecordStatus.RECORDSTATUS_CANCEL;

		AudioErrorCode errorcode = YouMeAudioRecorder.Instance().CancleSpeech();
		if (errorcode != AudioErrorCode.AUDIOERROR_SUCCESS){
			Log.e(LOG_TAG, "CancleSpeech error:" + errorcode);
		}

		if (m_isRecognizeStart) {
			stopThread = new Thread(new Runnable() {
				public void run() {
					m_speechRecognizer.stop();
					m_isRecognizeStart = false;
					m_callStartRecognizer = false;
				}
			}, "AudioRecognize2 Thread");
			stopThread.start();
		}

		return errorcode;
	}
	
	
	//--------------------------IAudioRecordListener---------------------------
	@Override
	public void OnRecordFinish(int errorcode, String path, long serial, String text){
//		Log.i(LOG_TAG, "Ali OnRecordFinish errorcode:" + errorcode);

		if (m_isRecognizeStart) {
			m_speechRecognizer.stop();
			m_isRecognizeStart = false;
			m_callStartRecognizer = false;
		}

		if (errorcode != AudioErrorCode.AUDIOERROR_SUCCESS.getValue() || m_audioErrorcode == -1){
			if (m_recordListener != null){
				m_recordListener.OnRecordFinish(errorcode, path, serial, text);
			}
		}
	}
	
	@Override
	public byte[] OnRecordData(byte buffer[], int size){
		//Log.d(LOG_TAG, "OnRecordData size:" + size);
		
		if (m_recordListener == null){
			Log.e(CommonConst.LOG_TAG, "record listener is null");
			return null;
		}
		if (m_speechRecognizer == null){
			Log.e(CommonConst.LOG_TAG, "recognizer is null");
			return null;
		}
		
		int errorcode = 0;
		if (m_isRecognizeStart){
			if (m_audioData.size() > 0) {
				if (m_audioData.size() > m_limitDataSize) {  // ali limit the data size of sendAudio 8000

					if (m_sampleRate == CommonConst.SAMPLERATE_16K) {

						int beginSize = 0;
						int writeSize = m_limitDataSize;
						do {
							m_tmpAudioData.write(m_audioData.toByteArray(), beginSize, writeSize);
							if (m_tmpAudioData.size() > 0) {
								errorcode = m_speechRecognizer.sendAudio(m_tmpAudioData.toByteArray(), m_tmpAudioData.size());

								if (errorcode < 0) {
									Log.e(LOG_TAG, "send audio error:" + errorcode);
								}
							}

							beginSize += m_tmpAudioData.size();
							if (m_audioData.size() - beginSize > m_limitDataSize) {
								writeSize = m_limitDataSize;
							} else {
								writeSize = m_audioData.size() - beginSize;
							}

							m_tmpAudioData.reset();

						} while (m_audioData.size() - beginSize > 0);

					} else {
						byte[] resampleAudioData = m_recordListener.OnRecordData(m_audioData.toByteArray(), m_audioData.size());
						if (resampleAudioData != null && resampleAudioData.length > 0) {
							if (resampleAudioData.length > m_limitDataSize) {

								int beginSize = 0;
								int writeSize = m_limitDataSize;
								do {
									m_tmpAudioData.write(resampleAudioData, beginSize, writeSize);
									if (m_tmpAudioData.size() > 0) {
										errorcode = m_speechRecognizer.sendAudio(m_tmpAudioData.toByteArray(), m_tmpAudioData.size());
										if (errorcode < 0) {
											Log.e(LOG_TAG, "send audio error:" + errorcode);
										}
									}

									beginSize += m_tmpAudioData.size();
									if (resampleAudioData.length - beginSize > m_limitDataSize) {
										writeSize = m_limitDataSize;
									} else {
										writeSize = resampleAudioData.length - beginSize;
									}

									m_tmpAudioData.reset();

								} while (resampleAudioData.length - beginSize > 0);

							} else {
								errorcode = m_speechRecognizer.sendAudio(resampleAudioData, resampleAudioData.length);
								if (errorcode < 0) {
									Log.e(LOG_TAG, "send audio error:" + errorcode);
								}
							}
						}
					}

					m_audioData.reset();

				} else {
					if (m_sampleRate == CommonConst.SAMPLERATE_16K) {
						errorcode = m_speechRecognizer.sendAudio(m_audioData.toByteArray(), m_audioData.size());
						if (errorcode < 0) {
							Log.e(LOG_TAG, "send audio error:" + errorcode);
						}
					} else {
						byte[] resampleAudioData = m_recordListener.OnRecordData(m_audioData.toByteArray(), m_audioData.size());
						if (resampleAudioData != null && resampleAudioData.length > 0) {
							errorcode = m_speechRecognizer.sendAudio(resampleAudioData, resampleAudioData.length);
							if (errorcode < 0) {
								Log.e(LOG_TAG, "send audio error:" + errorcode);
							}
						}
					}
					m_audioData.reset();
				}
			}

			byte[] resampleAudioData = m_recordListener.OnRecordData(buffer, size);
			if (resampleAudioData != null && resampleAudioData.length > 0) {
				if (resampleAudioData.length > m_limitDataSize) {

					int beginSize = 0;
					int writeSize = m_limitDataSize;
					do {
						m_tmpAudioData.write(resampleAudioData, beginSize, writeSize);
						if (m_tmpAudioData.size() > 0) {
							errorcode = m_speechRecognizer.sendAudio(m_tmpAudioData.toByteArray(), m_tmpAudioData.size());
							if (errorcode < 0) {
								Log.e(LOG_TAG, "send audio error:" + errorcode);
							}
						} 

						beginSize += m_tmpAudioData.size();
						if (resampleAudioData.length - beginSize > m_limitDataSize) {
							writeSize = m_limitDataSize;
						} else {
							writeSize = resampleAudioData.length - beginSize;
						}

						m_tmpAudioData.reset();

					}while (resampleAudioData.length - beginSize > 0);

				} else {
					errorcode = m_speechRecognizer.sendAudio(resampleAudioData, resampleAudioData.length);
					if (errorcode < 0) {
						Log.e(LOG_TAG, "send audio error:" + errorcode);
					}
				}

			} else {
				errorcode = m_speechRecognizer.sendAudio(buffer, size);
				if (errorcode < 0) {
					Log.e(LOG_TAG, "send audio error:" + errorcode);
				}
			}
		} else {
			m_audioData.write(buffer, 0, size);
		}
		return null;
	}
	
	@Override
	public void OnRecordVolumeChange(int volume, AudioRecognizeType type){
		
	}
	
	
	//--------------------------SpeechRecognizerCallback---------------------------
    @Override
    public void onRecognizedStarted(String msg, int code)
    {
		Log.d(LOG_TAG, "alis recognized started " + msg + ", code : " + String.valueOf(code));
        m_isRecognizeStart = true;
    }

    // 请求失败
    @Override
    public void onTaskFailed(String msg, int code)
    {
    	if (code != 0){
    		Log.e(LOG_TAG, "recognize error:" + String.valueOf(code) + " " + msg);
            m_audioErrorcode = -1;
			IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_ERROR, "alis taskfailed recognize error:" + String.valueOf(code) + " " + msg);
    	}
    }

    // 识别返回中间结果，只有开启相关选项时才会回调
    @Override
    public void onRecognizedResultChanged(final String msg, int code)
    {
        //Log.d(LOG_TAG, "OnRecognizedResultChanged " + msg + ": " + String.valueOf(code));
    }

    @Override
    public void onRecognizedCompleted(final String msg, int code)
    {
        Log.d(LOG_TAG, "recognized code:" + code);
        
        if (m_audioErrorcode != 0 || CommonConst.RecordStatus.RECORDSTATUS_CANCEL == m_recordStatus){
        	return;
        }
        
        String result = "";
        try{
        	JSONObject jsonObject = new JSONObject(msg);
            JSONObject payload= jsonObject.getJSONObject("payload");
            if (payload != null){
            	result= payload.getString("result");
            }
        } catch (JSONException e) {
			e.printStackTrace();
		}

        if (m_recordStatus != CommonConst.RecordStatus.RECORDSTATUS_CANCEL && m_recordListener != null ) {
			m_recordListener.OnRecordFinish(AudioErrorCode.AUDIOERROR_SUCCESS.getValue(), m_audioPath, m_serial, result);
		}
    }

    // 请求结束，关闭连接
    @Override
    public void onChannelClosed(String msg, int code) {
        Log.d(LOG_TAG, "request end close connect " + msg + ": " + String.valueOf(code));
    }
}
