package com.youme.imsdk;

import android.media.MediaPlayer;

import java.io.IOException;

import android.media.MediaPlayer.OnPreparedListener;
import android.media.MediaPlayer.OnCompletionListener;

public class AudioPlayer {
	
	private MediaPlayer mediaPlayer;
	private boolean isPaused;
	private boolean lastStateIsStop;
	private OnCompletionListener completeListener;
	
	private static AudioPlayer _instance;
	public static AudioPlayer GetInstance( ){
		if(_instance==null){
			_instance = new AudioPlayer();
		}
		return _instance;
	}
	
	public AudioPlayer(){
		mediaPlayer = new MediaPlayer(); 
		mediaPlayer.setOnPreparedListener(preparedListener);
		mediaPlayer.setOnCompletionListener(completionListener);
	}
	
	public boolean IsPlaying(){
		if(mediaPlayer!=null){
			return mediaPlayer.isPlaying();
		}
		return false;
	}
	
	public void SetOnCompletionListener(OnCompletionListener listener){
		completeListener = listener;
	}
	
	public boolean Play(String audioPath){
		isPaused = false;
		if(mediaPlayer.isPlaying()){
			mediaPlayer.stop();
		}
		// mediaPlayer.reset();
		try {
			mediaPlayer.reset();
			mediaPlayer.setDataSource( audioPath );
			lastStateIsStop = false;
			mediaPlayer.prepareAsync();
		} catch (IllegalArgumentException | SecurityException | IllegalStateException | IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		}
		return true;
	}
	
	public boolean Pause(){
		if(mediaPlayer.isPlaying()){
			try {
				mediaPlayer.pause();
				isPaused = true;
			} catch (IllegalStateException e){
				e.printStackTrace();
				isPaused = false;
			}
			return isPaused;
		}else{
			return false;
		}
	}
	
	public boolean ResumePlay(){
		if(isPaused && !mediaPlayer.isPlaying()){
			mediaPlayer.start();
			return true;
		}
		return false;
	}
	
	public boolean IsPaused(){
		return isPaused;
	}
	
	public boolean Stop(){
		isPaused = false;
		lastStateIsStop = true;
		try {
			mediaPlayer.stop();
			return true;
		}catch (IllegalStateException e){
			return false;
		}
	}
	
	OnPreparedListener preparedListener = new OnPreparedListener() {  
        
        @Override  
        public void onPrepared(MediaPlayer mp) {  
        	//避免异步操作延时导致的操作无效
        	if(!lastStateIsStop){
        		mediaPlayer.start();
        	}
        }  
    };
    
    OnCompletionListener completionListener = new OnCompletionListener(){

		@Override
		public void onCompletion(MediaPlayer currentPlayer) {
			// TODO Auto-generated method stub
			if(completeListener!=null){
				isPaused = false;
				completeListener.onCompletion (currentPlayer);
			}
		}
    	
    };
}
