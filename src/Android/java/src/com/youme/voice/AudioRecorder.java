package com.youme.voice;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import com.youme.im.CommonConst;
import com.youme.im.IMEngine;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Build;
import android.os.SystemClock;
import android.support.v4.app.ActivityCompat;
import android.util.Log;
import android.widget.Toast;

enum RecordStatus{
	RECORDSTATUS_IDLE,
	RECORDSTATUS_RECORDING,
	RECORDSTATUS_STOP,
	RECORDSTATUS_CANCEL
}

public class AudioRecorder{
	private Context m_context;
	private int m_sampleRate = CommonConst.SAMPLERATE_16K;			// 采样频率
	private int m_channel = AudioFormat.CHANNEL_IN_MONO;			// 声道
	private int m_audioFormat = AudioFormat.ENCODING_PCM_16BIT;		// 样本位宽
	private int m_recordBufferSize = 12800;
	//private boolean m_isRecording = false;
	//private boolean m_isCanceled = false;
	private RecordStatus m_recordStatus = RecordStatus.RECORDSTATUS_IDLE;
	private WAVHeadInfo m_wavHeadInfo = null;
	private AudioRecord m_audioRecord = null;
	private AudioManager m_audioManager = null;
	private Thread recordThread;
	private IAudioRecordListener m_recordListener = null;
	//private String m_audioPath;
	//private long m_serial;
	private static AudioRecorder s_instance;
	private boolean m_isInit = false;
	
	/*public AudioRecorder(Context context){
		this.m_context = context;
		m_audioManager = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
		m_wavHeadInfo = new WAVHeadInfo();
		SetAudioRecordParam(16000, 1, 16);
	}*/
	
	public static AudioRecorder Instance()
	{
		if (s_instance == null) {
			s_instance = new AudioRecorder();
		}
		return s_instance;
	}
	
	public boolean Init(Context context){
		if (m_isInit){
			return true;
		}
		m_context = context;
		m_audioManager = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
		if (null == m_audioManager){
			IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_ERROR, "audiomanager is null");
			return false;
		}
		m_wavHeadInfo = new WAVHeadInfo();
		SetAudioRecordParam(CommonConst.SAMPLERATE_16K, CommonConst.CHANNEL_NUMBER, CommonConst.SAMPLE_BIT_SIZE);
		
		m_isInit = true;
		
		return true;
	}
	
	public void Uninit(){
		if (m_audioRecord != null) {
			m_audioRecord.stop();
			m_audioRecord.release();
	    	m_audioRecord = null;
		}
		m_isInit = false;
	}
	
	public void SetAudioRecordParam(int sampleRate, int channel, int sampleBitSize){
		m_sampleRate = sampleRate;
		if (channel == 1){
			m_channel = AudioFormat.CHANNEL_IN_MONO;
		}else if (channel == 2){
			m_channel = AudioFormat.CHANNEL_IN_STEREO;
		}
		
		if (sampleBitSize == 8){
			m_audioFormat = AudioFormat.ENCODING_PCM_8BIT;
		}else if (sampleBitSize == 16){
			m_audioFormat = AudioFormat.ENCODING_PCM_16BIT;
		}else{
			m_audioFormat = AudioFormat.ENCODING_DEFAULT;
		}
		m_wavHeadInfo.SetAudioProperty(sampleRate, channel, sampleBitSize);
		m_recordBufferSize = AudioRecord.getMinBufferSize(m_sampleRate, m_channel, m_audioFormat) * 2;
	}
	
	public void SetRecordListener(IAudioRecordListener listener){
		m_recordListener = listener;
	}
	
	public boolean IsRecording() {
		return m_recordStatus != RecordStatus.RECORDSTATUS_IDLE;
		
		/*if (null == m_audioRecord){
			return false;
		}else{
			return m_audioRecord.getRecordingState() == AudioRecord.RECORDSTATE_RECORDING;
		}*/
	}
	
	public AudioErrorCode StartRecord(final long serial, final String path){
		IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_DEBUG, "AudioRecorder StartRecord enter");
		
		if (IsRecording()){
			Log.i(CommonConst.LOG_TAG, "AudioRecorder StartRecord leave recording");			
			return AudioErrorCode.AUDIOERROR_RECORDING;
		}
		
//		if (m_audioRecord == null || m_audioRecord.getState() != AudioRecord.STATE_INITIALIZED) {
			try {				
								
				if ((m_audioRecord != null) && (m_audioRecord.getRecordingState() != AudioRecord.RECORDSTATE_STOPPED)){
					
					IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_DEBUG, "AudioRecorder StartRecord is recording");
			    	return AudioErrorCode.AUDIOERROR_RECORDING;
				}
				
				if (m_audioRecord != null){
					m_audioRecord.release();
					m_audioRecord = null;
				}
					
				if (m_audioManager != null){
					//m_audioManager.setMode(AudioManager.MODE_NORMAL);
				}
				IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_DEBUG, "create AudioRecorder");
				m_audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, m_sampleRate, m_channel, m_audioFormat, m_recordBufferSize);

			    if (m_audioRecord == null || m_audioRecord.getState() != AudioRecord.STATE_INITIALIZED || m_audioRecord.getRecordingState() != AudioRecord.RECORDSTATE_STOPPED) {
			    	
			    	IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_DEBUG, "AudioRecorder StartRecord init failed");
			    	return AudioErrorCode.AUDIOERROR_INIT_FAILED;
			    }
			} catch (Throwable e) {
				IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_ERROR, "init audiorecord failed");
				e.printStackTrace();
				return AudioErrorCode.AUDIOERROR_INIT_FAILED;
			}
//		} else {
//			if (m_audioRecord.getRecordingState() != AudioRecord.RECORDSTATE_STOPPED){
//				
//				IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_DEBUG, "AudioRecorder StartRecord is recording 2");
//		    	return AudioErrorCode.AUDIOERROR_RECORDING;
//			}
//	    }
		
        m_recordStatus = RecordStatus.RECORDSTATUS_RECORDING;
		IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_DEBUG, "create Record thread");
        recordThread = new Thread(new Runnable() { 
            public void run() {
            	try {
					Record(path, serial);
				}catch (Throwable e){
            		e.printStackTrace();
				}
            }  
        }, "AudioRecorder Thread");  
        recordThread.start();
        
        Log.i(CommonConst.LOG_TAG, "AudioRecorder StartRecord leave");        
        return AudioErrorCode.AUDIOERROR_SUCCESS;
	}
	
	public AudioErrorCode StopRecord(){
		IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_DEBUG, "AudioRecorder StopRecord enter status:"+ m_recordStatus);
		if (!IsRecording()){
			IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_DEBUG, "record status:" + m_recordStatus);
			return AudioErrorCode.AUDIOERROR_NOT_START_RECORD;
		}
		m_recordStatus = RecordStatus.RECORDSTATUS_STOP;
		//recordThread = null;
		
		Log.i(CommonConst.LOG_TAG, "StopRecord leave");		
		
		return AudioErrorCode.AUDIOERROR_SUCCESS;
	}
	
	public AudioErrorCode CancelRecord(){
		IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_DEBUG, "CancelRecord enter");
		
		if (!IsRecording()){
			return AudioErrorCode.AUDIOERROR_NOT_START_RECORD;
		}
		m_recordStatus = RecordStatus.RECORDSTATUS_CANCEL;
		//recordThread = null;
		
		Log.i(CommonConst.LOG_TAG, "CancelRecord leave");
		
		return AudioErrorCode.AUDIOERROR_SUCCESS;
	}
	
	private void Record(String path, long serial) {
		Log.i(CommonConst.LOG_TAG, "AudioRecorder Record enter");
				
		if (RecordStatus.RECORDSTATUS_STOP == m_recordStatus){
			Log.i(CommonConst.LOG_TAG, "Record leave stop");			
			m_recordStatus = RecordStatus.RECORDSTATUS_IDLE;
			m_recordListener.OnRecordFinish(AudioErrorCode.AUDIOERROR_RECORD_TIME_TOO_SHORT.getValue(), "", serial, "");
			return;
		}else if (RecordStatus.RECORDSTATUS_CANCEL == m_recordStatus){
			Log.i(CommonConst.LOG_TAG, "Record leave cancel");			
			m_recordStatus = RecordStatus.RECORDSTATUS_IDLE;
			return;
		}
		try {
			m_audioRecord.startRecording();
		}catch (Throwable e){
			m_recordStatus = RecordStatus.RECORDSTATUS_IDLE;
			e.printStackTrace();
			m_recordListener.OnRecordFinish(AudioErrorCode.AUDIOERROR_START_RECORD_FAILED.getValue(), "", serial, "");
			return;
		}
		if (m_audioRecord.getRecordingState() != AudioRecord.RECORDSTATE_RECORDING) {
			//是否开启录音权限
			IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_ERROR, "start record failed status:" + m_audioRecord.getRecordingState());
			
			m_recordStatus = RecordStatus.RECORDSTATUS_IDLE;
			m_recordListener.OnRecordFinish(AudioErrorCode.AUDIOERROR_AUTHORIZE.getValue(), path, serial, "");
			return;
		}
		
		File file = new File(path);
		if (file.exists() == true){
			file.delete();
		}  
        FileOutputStream os = null;
        try {
            os = new FileOutputStream(file);             
        } catch (FileNotFoundException e) { 
            e.printStackTrace();
            m_recordStatus = RecordStatus.RECORDSTATUS_IDLE;  
            IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_DEBUG, "Record create file failed");
            m_recordListener.OnRecordFinish(AudioErrorCode.AUDIOERROR_CREATE_FILE_FAILED.getValue(), path, serial, "");
            return;
        }
        
    	byte fillBytes[] = new byte[44];
    	try { 
    		os.write(fillBytes);  	
        } catch (IOException e) {
        	IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_ERROR, "write file error");
            e.printStackTrace();
			m_recordStatus = RecordStatus.RECORDSTATUS_IDLE;
			m_recordListener.OnRecordFinish(AudioErrorCode.AUDIOERROR_READWRITE_FILE_ERROR.getValue(), "", serial, "");
			return;
        }
    	
    	boolean hasAuthorize = true;
    	int count = 0;
    	int muteCount = 0;
    	boolean isTimeout = false;
    	long beginTime = SystemClock.elapsedRealtime();
    	
    	byte data[] = new byte[m_recordBufferSize];
    	int readByte = 0;
    	int bufferMiniSecond = 50;
    	int readBufferSieze = m_sampleRate * 2 / 1000 * bufferMiniSecond;	// 读缓冲大小 50毫秒
    	while (readBufferSieze > m_recordBufferSize && bufferMiniSecond > 0){
    		bufferMiniSecond -= 10;
    		readBufferSieze = m_sampleRate * 2 / 1000 * bufferMiniSecond;
    		Log.i(CommonConst.LOG_TAG, "buffer size:" + readBufferSieze + "," + m_recordBufferSize);
    		IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_DEBUG, "buffer size:" + readBufferSieze + "," + m_recordBufferSize);
    	}
    	if (bufferMiniSecond == 0){
    		IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_ERROR, "read buffer size error");
    		try {
        		os.close();    		
        	} catch (IOException e) {
        		IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_ERROR, "close file error");
        		e.printStackTrace();
        	}
    		m_recordStatus = RecordStatus.RECORDSTATUS_IDLE;             
            m_recordListener.OnRecordFinish(AudioErrorCode.AUDIOERROR_START_RECORD_FAILED.getValue(), path, serial, "");
            return;
    	}
    	byte fileData[] = new byte[readBufferSieze * 5];
    	int i = 0;
    	int fileDataPos = 0;
    	
    	Log.i(CommonConst.LOG_TAG, "readBufferSieze:  " + readBufferSieze + " m_recordBufferSize:" + m_recordBufferSize + " " + readByte);
    	long totalWrite = 0;
    	while (m_recordStatus == RecordStatus.RECORDSTATUS_RECORDING) {
    		readByte = m_audioRecord.read(data, 0, readBufferSieze);
    		if (readByte > 0) {
    			System.arraycopy(data, 0, fileData, fileDataPos, readByte);
    			fileDataPos += readByte;
				totalWrite += readByte;
						++i;
    			if (i == 5) {	// 5次写一次文件
    				try {
        				os.write(fileData, 0, fileDataPos);
        			} catch (IOException e) {
        				e.printStackTrace();
        				break;
        			}
    				
    				if (count < 20) {
            			++count;
            			int volume = GetVolume(fileData, fileDataPos);
            			if (volume == 0 && ++muteCount == 20) {
            				hasAuthorize = false;
            				break;
            			}
            		}
    				
    				i= 0;
    				fileDataPos = 0;
    			}
    		}
    		
    		if (readByte > 0) {
    			m_recordListener.OnRecordData(data, readByte);
    		}
    		if (SystemClock.elapsedRealtime() - beginTime > CommonConst.SPEECH_DURATION_MAX){	// 超时
    			isTimeout = true;
    			break;
    		}
    	}
    	//停止录音，读取处理AudioRecord缓冲区剩余数据
    	if (hasAuthorize){
    		readByte = m_audioRecord.read(data, 0, readBufferSieze);
        	if (readByte > 0) {
				totalWrite += readByte;
        		try {
    				os.write(data, 0, readByte);      				
    			} catch (IOException e) {
    				e.printStackTrace();
    			}
        		m_recordListener.OnRecordData(data, readByte);
        	}
    	}
    	    	
    	m_audioRecord.stop();    	
    	//m_audioRecord.release();
    	//m_audioRecord = null;

    	try {
    		os.close();    		
    	} catch (IOException e) {
    		IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_ERROR, "close file error");
    		e.printStackTrace();
    		
    		m_recordStatus = RecordStatus.RECORDSTATUS_IDLE;      		
    		m_recordListener.OnRecordFinish(AudioErrorCode.AUDIOERROR_READWRITE_FILE_ERROR.getValue(), path, serial, "");
    		return;
    	}
    	
    	if (RecordStatus.RECORDSTATUS_CANCEL == m_recordStatus) {
    		file.delete();
    		m_recordStatus = RecordStatus.RECORDSTATUS_IDLE;
    		return;
    	} else {
    		m_recordStatus = RecordStatus.RECORDSTATUS_IDLE;
    		
    		if (hasAuthorize) {
				long fileMSeconds = totalWrite / (readBufferSieze / bufferMiniSecond );
				long durationTime = SystemClock.elapsedRealtime() - beginTime;
				IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_DEBUG, "AudioRecorder data time:"+fileMSeconds  +" duration:"+durationTime);
        		if (    durationTime < CommonConst.SPEECH_DURATION_MIN ||  // 录音时长太短
						durationTime < (fileMSeconds - 1000)  //录音数据异常
				) {
        			file.delete();
        			m_recordListener.OnRecordFinish(AudioErrorCode.AUDIOERROR_RECORD_TIME_TOO_SHORT.getValue(), "", serial, "");
        		} else {
        			boolean	ret = m_wavHeadInfo.WriteHeadInfo(path);                
            		if (ret ) {        			   
            			m_recordListener.OnRecordFinish(isTimeout ? AudioErrorCode.AUDIOERROR_RECORD_TIMEOUT.getValue() : AudioErrorCode.AUDIOERROR_SUCCESS.getValue(), path, serial, "");
            		} else {
            			m_recordListener.OnRecordFinish(AudioErrorCode.AUDIOERROR_READWRITE_FILE_ERROR.getValue(), path, serial, "");
         			}
        		}
        	} else {
        		file.delete();
        		m_recordListener.OnRecordFinish(AudioErrorCode.AUDIOERROR_AUTHORIZE.getValue(), path, serial, "");
        	}
    	}
    	
    	Log.i(CommonConst.LOG_TAG, "AudioRecorder Record leave");
	}
	
	//16位pcm编码，取录音数据用的是byte，需要将两个bye转换成一个short
	private int GetVolume(byte[] data, int size) {
		int max = 0;
		for (int i = 0; i < size; i += 2) {
		   short temp = (short) ((data[i] & 0xff) | (data[i + 1] & 0xff) << 8);
		   if (Math.abs(temp) > max) {
               max = Math.abs(temp);
           }
		}
		return max;
    }

	private static final int GET_RECODE_AUDIO = 1;
	private static String[] PERMISSION_AUDIO = {
			Manifest.permission.RECORD_AUDIO
	};
	public boolean HasAudioPermission(){
		if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && m_context !=null	&& m_context instanceof Activity && m_context.getApplicationInfo().targetSdkVersion >22 ){
			//针对android 6.0以上的授权检查
			if (ActivityCompat.checkSelfPermission(AudioRecorder.Instance().m_context, Manifest.permission.RECORD_AUDIO) != PackageManager.PERMISSION_GRANTED) {
				Toast.makeText(AudioRecorder.Instance().m_context,"没有权限,请手动开启定位权限",Toast.LENGTH_SHORT).show();
				ActivityCompat.requestPermissions((Activity)AudioRecorder.Instance().m_context,PERMISSION_AUDIO, GET_RECODE_AUDIO);
			}
		}
		
    	if (m_audioRecord != null && m_audioRecord.getRecordingState() == AudioRecord.RECORDSTATE_RECORDING) {
            return true;
    	}
    	
    	AudioRecord audioRecorder = null;
    	int bufferSize = AudioRecord.getMinBufferSize(CommonConst.SAMPLERATE_8K, m_channel, m_audioFormat);
        try{
        	audioRecorder = new AudioRecord(MediaRecorder.AudioSource.MIC, CommonConst.SAMPLERATE_8K, m_channel, m_audioFormat, bufferSize);
        	audioRecorder.startRecording();
        }catch (Throwable e){
            e.printStackTrace();
            return false;
        }
        
        boolean bRet = true;
        if (audioRecorder.getRecordingState() != AudioRecord.RECORDSTATE_RECORDING) {
        	bRet = false;
        } else{
        	int count = 0;
        	for (int i = 0; i < 20; ++i) {
				byte data[] = new byte[bufferSize];
				int readByte = audioRecorder.read(data, 0, bufferSize);
				if (readByte <= 0) {
					bRet = false;
					break;
				}
				 
				int volume = GetVolume(data, readByte);
				if (volume == 0) {
					 ++count;
				}
			}
        	if (count == 20) {
				bRet = false;
			}
    	}
        try {
        	audioRecorder.stop();
        	audioRecorder.release();
        	audioRecorder = null;
        } catch (Exception e){
            e.printStackTrace();
            return false;
        }
        
        return bRet;
    }
	
	public AudioDeviceStatus GetMicrophoneStatus(){    	
		boolean bHasPerssion = HasAudioPermission();
 		if (!bHasPerssion) {
 			return AudioDeviceStatus.AUDIOSTATUS_NO_AUTHORIZE;
 		}		
 		if (m_audioManager == null) {
 			Log.i(CommonConst.LOG_TAG, "AudioManager is null");
 			return AudioDeviceStatus.AUDIOSTATUS_NOT_AVIAIBLE;
 		}
 		if (m_audioManager.isMicrophoneMute()) {
 			return AudioDeviceStatus.AUDIOSTATUS_MUTE;
 		}
 		if (0 == m_audioManager.getStreamVolume(AudioManager.STREAM_MUSIC)) {
 			return AudioDeviceStatus.AUDIOSTATUS_MUTE;
 		}
 		return AudioDeviceStatus.AUDIOSTATUS_AVIAIBLE;
	}
}