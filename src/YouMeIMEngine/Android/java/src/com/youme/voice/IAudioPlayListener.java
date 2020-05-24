package com.youme.voice;


public interface IAudioPlayListener {
	public abstract void OnPlayFinish(AudioErrorCode errrocode, String path);
	public abstract void OnPlayData(byte buffer[], int size);
}