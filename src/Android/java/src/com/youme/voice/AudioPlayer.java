package com.youme.voice;


import android.media.AudioManager;
import android.media.MediaPlayer;
import java.io.File;

import com.youme.im.CommonConst;

import android.media.MediaPlayer.OnPreparedListener;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnErrorListener;
import android.util.Log;


public class AudioPlayer implements OnCompletionListener, OnPreparedListener, OnErrorListener {

	//private static AudioPlayer m_instance = null;
	private MediaPlayer m_mediaPlayer = null;
	private boolean m_isPaused = false;
	private boolean m_lastStateIsStop = true;
	private IAudioPlayListener m_playerListener = null;
	private String m_audioPath = "";
	private float m_playVolume = 1.0f;
	
	/*public static AudioPlayer GetInstance( ){
		if(m_instance==null){
			m_instance = new AudioPlayer();
		}
		return m_instance;
	}*/
	
	public AudioPlayer(){
		m_mediaPlayer = new MediaPlayer();
		m_mediaPlayer.setOnPreparedListener(this);
		m_mediaPlayer.setOnCompletionListener(this);
		m_mediaPlayer.setOnErrorListener(this);
	}
	
	public void SetPlayListener(IAudioPlayListener listener) {
		m_playerListener = listener;
	}
	
	public void SetPlayVolume(float volume) {
		Log.d(CommonConst.LOG_TAG, "SetVolume: " + volume);
		this.m_playVolume = volume;
	}
	
	public boolean IsPlaying(){
		if(m_mediaPlayer!=null){
			return m_mediaPlayer.isPlaying();
		}
		return false;
	}
	
	public AudioErrorCode Play(String path){
		File file =new File(path); 
		if (!file.exists()) {
			return AudioErrorCode.AUDIOERROR_FILE_NOT_EXIT;
		}
		
		if (m_mediaPlayer == null) {
			m_mediaPlayer = new MediaPlayer(); 
			m_mediaPlayer.setOnPreparedListener(this);
			m_mediaPlayer.setOnCompletionListener(this);
			m_mediaPlayer.setOnErrorListener(this);
		}
		
		if (m_mediaPlayer == null) {
			return AudioErrorCode.AUDIOERROR_START_PLAY_FAILED;
		}
		
		m_isPaused = false;
		AudioErrorCode errorcode = AudioErrorCode.AUDIOERROR_SUCCESS;
		if(m_mediaPlayer.isPlaying()){
			m_mediaPlayer.stop();
			errorcode = AudioErrorCode.AUDIOERROR_PLAYING;
		}
		// mediaPlayer.reset();
		m_audioPath = path;
		try {
			m_mediaPlayer.reset();
			m_mediaPlayer.setOnPreparedListener(this);
			m_mediaPlayer.setOnCompletionListener(this);
			m_mediaPlayer.setOnErrorListener(this);
			m_mediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
			m_mediaPlayer.setDataSource(path);
			m_lastStateIsStop = false;
			m_mediaPlayer.setVolume(m_playVolume, m_playVolume);
			m_mediaPlayer.prepareAsync();
			Log.d(CommonConst.LOG_TAG, "prepareAsync4");
		} catch (Exception e) {
			m_lastStateIsStop = true;
			
			e.printStackTrace();
			return AudioErrorCode.AUDIOERROR_START_PLAY_FAILED;
		}
		return errorcode;
	}
	
	public boolean Pause(){
		if (m_mediaPlayer == null) {
			return true;
		}
		if(m_mediaPlayer.isPlaying()){
			try {
				m_mediaPlayer.pause();
				m_isPaused = true;
			} catch (IllegalStateException e){
				e.printStackTrace();
				m_isPaused = false;
			}
			return m_isPaused;
		}else{
			return false;
		}
	}
	
	public boolean ResumePlay(){
		if (m_mediaPlayer == null) {
			return false;
		}
		
		if(m_isPaused && !m_mediaPlayer.isPlaying()){
			m_mediaPlayer.start();
			return true;
		}
		return false;
	}
	
	public boolean IsPaused(){
		return m_isPaused;
	}
	
	public AudioErrorCode Stop(){
		if (m_mediaPlayer == null || !m_mediaPlayer.isPlaying()) {
			return AudioErrorCode.AUDIOERROR_NOT_START_PLAY;
		}
		
		m_isPaused = false;
		m_lastStateIsStop = true;
		try {
			m_mediaPlayer.stop();
		}catch (IllegalStateException e){
			return AudioErrorCode.AUDIOERROR_STOP_PLAY_FAILED;
		}
		return AudioErrorCode.AUDIOERROR_SUCCESS;
	}
    
    @Override
	public void onCompletion(MediaPlayer currentPlayer) {
		Log.d(CommonConst.LOG_TAG, "onCompletion");
    	m_mediaPlayer.reset();
    	if (m_playerListener != null) {
    		m_playerListener.OnPlayFinish(AudioErrorCode.AUDIOERROR_SUCCESS, m_audioPath);
		}
	}
    
    @Override  
    public void onPrepared(MediaPlayer mp) {  
    	//避免异步操作延时导致的操作无效
		Log.d(CommonConst.LOG_TAG, "onPrepared");
    	if(!m_lastStateIsStop){
			Log.d(CommonConst.LOG_TAG, "start play");
    		m_mediaPlayer.start();
    	}
    }
    
    @Override
	public boolean onError(MediaPlayer mp, int what, int extra) {
		Log.d(CommonConst.LOG_TAG, "what:"+what+" extra:"+extra);
		if (m_mediaPlayer != null){
			m_mediaPlayer.reset();
		}
    	
		AudioErrorCode errorcode = AudioErrorCode.AUDIOERROR_OTHREERROR;
    	if (MediaPlayer.MEDIA_ERROR_SERVER_DIED == what) {
    		if (m_mediaPlayer != null){
    			m_mediaPlayer.release();
    		}
    		m_mediaPlayer = new MediaPlayer(); 
    		m_mediaPlayer.setOnPreparedListener(this);
    		m_mediaPlayer.setOnCompletionListener(this);
			m_mediaPlayer.setOnErrorListener(this);
		}
    	
    	if (MediaPlayer.MEDIA_ERROR_UNSUPPORTED == extra) {
			errorcode = AudioErrorCode.AUDIOERROR_UNSUPPORT_FORMAT;
		} else if (MediaPlayer.MEDIA_ERROR_TIMED_OUT == extra) {
			errorcode = AudioErrorCode.AUDIOERROR_PLAY_TIMEOUT;
		}
    	    	
    	if (m_playerListener != null) {
    		m_playerListener.OnPlayFinish(errorcode, m_audioPath);
		}
    	return true;
    }
}
