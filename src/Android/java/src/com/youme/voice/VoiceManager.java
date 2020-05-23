package com.youme.voice;

import java.io.File;
import java.util.UUID;

import com.youme.im.CommonConst;
import com.youme.im.IMEngine;
import com.youme.im.NativeEngine;

import com.youme.speech.AliSpeechRecognizer;
import com.youme.speech.IflySpeechRecognizer;
import com.youme.speech.UscSpeechRecognizer;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;
import android.support.v4.app.ActivityCompat;
import android.util.Log;
import android.widget.Toast;

public class VoiceManager implements IAudioRecordListener, IAudioPlayListener {
	private static VoiceManager s_instance;
	private Context m_context;
	private String m_recordCacheDir = "";
	private int m_sampleRate = CommonConst.SAMPLERATE_16K;
	private int m_channel = CommonConst.CHANNEL_NUMBER;
	private int m_sampleBitSize = CommonConst.SAMPLE_BIT_SIZE;
	//private YouMeAudioRecorder m_youmeAudioRecorder = null;
	//private IflySpeechRecognizer m_speechRecognizer = null;
	private ISpeechRecognize m_speechRecognizer = null;
	private boolean m_speechOnly = false;
	private String m_recognizeAppID;
	private String m_recognizeSecret;
	private ISpeechRecognize.RecognizeLanguage m_recognizeLanguage = ISpeechRecognize.RecognizeLanguage.RECOGNIZELANG_MANDARIN;
	private CommonConst.AudioRecognizeType m_recognizeType = CommonConst.AudioRecognizeType.RECOGNIZETYPE_NO;
	private boolean m_isInit = false;
	
	
	private AudioPlayer m_audioPlayer = null;
	private float m_playVolume = 1.0f;
	
	public static VoiceManager Instance()
	{
		if (s_instance == null) {
			s_instance = new VoiceManager();
		}
		return s_instance;
	}
	
	public void Init(Context context){
		VoiceManager.Instance().m_context = context;
		if (m_recordCacheDir.isEmpty()){
			SetAudioRecordCacheDir("");
		}
	}
	
	public static void UnInit(){
		if (VoiceManager.Instance().m_speechRecognizer != null){
			VoiceManager.Instance().m_speechRecognizer.UnInit();
		}
	}
	
	public static void InitSpeechRecognizer(String appkey, String secret, int recognizeType){
		VoiceManager.Instance().m_recognizeAppID = appkey;
		VoiceManager.Instance().m_recognizeSecret = secret;
		VoiceManager.Instance().m_recognizeType = CommonConst.AudioRecognizeType.values()[recognizeType];
		if (VoiceManager.Instance().m_recognizeType != CommonConst.AudioRecognizeType.RECOGNIZETYPE_IFLY_RECORD){
			InitYouMeAudioRecord();
		}
	}
	
	private static void InitYouMeAudioRecord(){
		/*if (VoiceManager.Instance().m_youmeAudioRecorder != null){
			return;
		}
		VoiceManager.Instance().m_youmeAudioRecorder = new YouMeAudioRecorder();
		VoiceManager.Instance().m_youmeAudioRecorder.Init(VoiceManager.Instance().m_context);
		VoiceManager.Instance().m_youmeAudioRecorder.SetAudioRecordListener(VoiceManager.Instance());
		VoiceManager.Instance().m_youmeAudioRecorder.SetAudioRecordParam(VoiceManager.Instance().m_sampleRate, VoiceManager.Instance().m_channel, VoiceManager.Instance().m_sampleBitSize);*/
		
		if (VoiceManager.Instance().m_isInit){
			return;
		}
		VoiceManager.Instance().m_isInit = true;
		
		YouMeAudioRecorder.Instance().Init(VoiceManager.Instance().m_context);
		YouMeAudioRecorder.Instance().SetAudioRecordListener(VoiceManager.Instance());
		YouMeAudioRecorder.Instance().SetAudioRecordParam(VoiceManager.Instance().m_sampleRate, VoiceManager.Instance().m_channel, VoiceManager.Instance().m_sampleBitSize);
	}
	
	private static void InitSpeechRecognizer(){
		if (VoiceManager.Instance().m_speechRecognizer != null){
			return;
		}
		if (CommonConst.AudioRecognizeType.RECOGNIZETYPE_IFLY_STREAM == VoiceManager.Instance().m_recognizeType
				|| CommonConst.AudioRecognizeType.RECOGNIZETYPE_IFLY_RECORD == VoiceManager.Instance().m_recognizeType){
			VoiceManager.Instance().m_speechRecognizer = new IflySpeechRecognizer();
		} else if (CommonConst.AudioRecognizeType.RECOGNIZETYPE_USC == VoiceManager.Instance().m_recognizeType) {
			VoiceManager.Instance().m_speechRecognizer = new UscSpeechRecognizer();
		} else if (CommonConst.AudioRecognizeType.RECOGNIZETYPE_ALI == VoiceManager.Instance().m_recognizeType) {
			VoiceManager.Instance().m_speechRecognizer = new AliSpeechRecognizer();
		} else {
			return;
		}
		VoiceManager.Instance().m_speechRecognizer.Init(VoiceManager.Instance().m_context);
		VoiceManager.Instance().m_speechRecognizer.InitSpeechRecognizer(VoiceManager.Instance().m_recognizeAppID, VoiceManager.Instance().m_recognizeSecret, VoiceManager.Instance().m_recognizeType);
		VoiceManager.Instance().m_speechRecognizer.SetAudioRecordListener(VoiceManager.Instance());
		VoiceManager.Instance().m_speechRecognizer.SetAudioRecordParam(VoiceManager.Instance().m_sampleRate, VoiceManager.Instance().m_channel, VoiceManager.Instance().m_sampleBitSize);
		VoiceManager.Instance().m_speechRecognizer.SetRecognizeLanguage(VoiceManager.Instance().m_recognizeLanguage);
	}

	public static void SetAudioRecordCacheDir(String path){
		if (path.length() == 0) {
			VoiceManager.Instance().m_recordCacheDir = VoiceManager.Instance().m_context.getCacheDir().getAbsolutePath() + "/AudioRecord/";
			File file = new File(VoiceManager.Instance().m_recordCacheDir);
			if (!file.exists()) {
				file.mkdirs();
			}
		}else{
			File file =new File(path); 
			if (file.exists() && file.isDirectory()) {
				VoiceManager.Instance().m_recordCacheDir = path;
			}
			else 
			{
				if (file.mkdirs()) {
					VoiceManager.Instance().m_recordCacheDir = path;
				} else {
					IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_ERROR, "create dir failed:" + path);
				}
			}
			if (!VoiceManager.Instance().m_recordCacheDir.isEmpty() && path.charAt(path.length() - 1) != '/') {
				VoiceManager.Instance().m_recordCacheDir += "/";
			}
		}
	}
	
	public static String GetAudioRecordCacheDir(){
		return VoiceManager.Instance().m_recordCacheDir;
	}
	
	public static void SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize){
		VoiceManager.Instance().m_sampleRate = sampleRate;
		VoiceManager.Instance().m_channel = channel;
		VoiceManager.Instance().m_sampleBitSize = sampleBitSize;
		if (VoiceManager.Instance().m_recognizeType != CommonConst.AudioRecognizeType.RECOGNIZETYPE_IFLY_RECORD){
			YouMeAudioRecorder.Instance().SetAudioRecordParam(VoiceManager.Instance().m_sampleRate, VoiceManager.Instance().m_channel, VoiceManager.Instance().m_sampleBitSize);
		}
		if (VoiceManager.Instance().m_speechRecognizer != null){
			VoiceManager.Instance().m_speechRecognizer.SetAudioRecordParam(sampleRate, channel, sampleBitSize);
		}
	}
	
	public static void UpdateToken(String token){
		VoiceManager.Instance().m_recognizeSecret = token;
		if (VoiceManager.Instance().m_speechRecognizer != null){
			VoiceManager.Instance().m_speechRecognizer.UpdateToken(token);
		}
	}

    private static final int GET_RECODE_AUDIO = 1;
	private static String[] PERMISSION_AUDIO = {
	        Manifest.permission.RECORD_AUDIO
	};

	public static int StartSpeech(long serial){
		if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && VoiceManager.Instance().m_context!=null
				&& VoiceManager.Instance().m_context instanceof Activity && VoiceManager.Instance().m_context.getApplicationInfo().targetSdkVersion >22 ){
			//针对android 6.0以上的授权检查
            if (ActivityCompat.checkSelfPermission(VoiceManager.Instance().m_context, Manifest.permission.RECORD_AUDIO) != PackageManager.PERMISSION_GRANTED) {
                Toast.makeText(VoiceManager.Instance().m_context,"没有权限,请手动开启定位权限",Toast.LENGTH_SHORT).show();
                ActivityCompat.requestPermissions((Activity)VoiceManager.Instance().m_context,PERMISSION_AUDIO, GET_RECODE_AUDIO);
            }
		}
		
		if (VoiceManager.Instance().m_speechRecognizer == null){
			InitSpeechRecognizer();
		}
		
		String path = VoiceManager.Instance().m_recordCacheDir + UUID.randomUUID().toString() + ".wav";
		VoiceManager.Instance().m_speechOnly = false;
		return VoiceManager.Instance().m_speechRecognizer.StartSpeech(path, serial);
	}

	public static int StopSpeech(){
		try {
			if (VoiceManager.Instance().m_speechOnly) {
				if (!VoiceManager.Instance().m_isInit){
					IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_ERROR, "VoiceManager not init");
					return AudioErrorCode.AUDIOERROR_NOT_START_RECORD.getValue();
				}else{
					AudioErrorCode errorcode = YouMeAudioRecorder.Instance().StopSpeech();
					return errorcode.getValue();
				}
			} else {
				if (VoiceManager.Instance().m_speechRecognizer == null){
					IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_ERROR, "m_speechRecognizer is null");
					return AudioErrorCode.AUDIOERROR_NOT_START_RECORD.getValue();
				}else{
					AudioErrorCode errorcode = VoiceManager.Instance().m_speechRecognizer.StopSpeech();
					return errorcode.getValue();
				}
			}
		}catch (Throwable e ){
			Log.e(CommonConst.LOG_TAG, e.toString());
			return AudioErrorCode.AUDIOERROR_OTHREERROR.getValue();
		}
	}
	
	public static int CancleSpeech(){
		try {
			if (VoiceManager.Instance().m_speechOnly) {
				if (!VoiceManager.Instance().m_isInit) {
					return AudioErrorCode.AUDIOERROR_NOT_START_RECORD.getValue();
				} else {
					AudioErrorCode errorcode = YouMeAudioRecorder.Instance().CancleSpeech();
					return errorcode.getValue();
				}
			} else {
				if (VoiceManager.Instance().m_speechRecognizer == null) {
					return AudioErrorCode.AUDIOERROR_NOT_START_RECORD.getValue();
				} else {
					AudioErrorCode errorcode = VoiceManager.Instance().m_speechRecognizer.CancleSpeech();
					return errorcode.getValue();
				}
			}
		}catch (Throwable e ){
			Log.e(CommonConst.LOG_TAG, e.toString());
			return AudioErrorCode.AUDIOERROR_OTHREERROR.getValue();
		}
	}
	
	public static void SetRecognizeLanguage(int language){
		VoiceManager.Instance().m_recognizeLanguage = ISpeechRecognize.RecognizeLanguage.values()[language];
		if (VoiceManager.Instance().m_speechRecognizer != null){
			VoiceManager.Instance().m_speechRecognizer.SetRecognizeLanguage(VoiceManager.Instance().m_recognizeLanguage);
		}
	}
	
	@Override
	public void OnRecordFinish(int errorcode, String path, long serial, String text) {
		Log.d(CommonConst.LOG_TAG, "VoiceManager OnRecordFinish errorcode:" + errorcode + " serial:" + serial + " path:" + path + " text:" + text);
		NativeEngine.OnRecordFinish(errorcode, path, serial, text);
	}
	
	@Override
	public byte[] OnRecordData(byte buffer[], int size) {
		if ((CommonConst.AudioRecognizeType.RECOGNIZETYPE_IFLY_STREAM == m_recognizeType
				|| CommonConst.AudioRecognizeType.RECOGNIZETYPE_USC == m_recognizeType)
				&& m_sampleRate != CommonConst.SAMPLERATE_16K && m_sampleRate != CommonConst.SAMPLERATE_8K){
			return NativeEngine.OnRecordData(buffer, size, true);
		} else if (CommonConst.AudioRecognizeType.RECOGNIZETYPE_ALI == m_recognizeType) {
			return NativeEngine.OnRecordData(buffer, size, m_sampleRate != CommonConst.SAMPLERATE_16K);
		} else if (CommonConst.AudioRecognizeType.RECOGNIZETYPE_NO == m_recognizeType || m_speechOnly){
			return NativeEngine.OnRecordData(buffer, size, false);
		} else {
			return null;
		}
	}
	
	@Override
	public void OnRecordVolumeChange(int volume, CommonConst.AudioRecognizeType type){
		NativeEngine.NotifyVolumeChange(volume, type.ordinal());
	}
	
	@Override
	public void OnPlayFinish(AudioErrorCode errrocode, String path){
		NativeEngine.OnPlayFinish(errrocode.getValue(), path);
	}
	
	@Override
	public void OnPlayData(byte buffer[], int size){
		NativeEngine.OnPlayData(buffer, size);
	}
}
