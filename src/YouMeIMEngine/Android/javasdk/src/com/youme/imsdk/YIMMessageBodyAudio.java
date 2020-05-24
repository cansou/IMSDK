package com.youme.imsdk;

public class YIMMessageBodyAudio implements IYIMMessageBodyBase {
	//private int mMsgType = YouMeIMManager.MessageBodyType_Unknow;
	private String mExtraText = null;
	private String mParam = null;
//	private int mFileSize = 0;
	private int mAudioTime = 0;
	private String localPath;
	/*@Override
	public int getMessageType() {
		return mMsgType;
	}*/
	
	/*public void setMessageType(int msgType){
		mMsgType = msgType;
	}*/
	
	public String getText(){
		return mExtraText;
	}

	public String getParam() {
		return mParam;
	}

//	public int getFileSize() {
//		return mFileSize;
//	}
	
	public int getAudioTime() {
		return mAudioTime;
	}
	
	public void setText(String text){
		mExtraText = text;
	}
	
	public void setParam(String param){
		mParam = param;
	}
	
//	public void setFileSize(int size){
//		mFileSize = size;
//	}
	
	public void setAudioTime(int time){
		mAudioTime = time;
	}

	public void setLocalPath(String path){
		this.localPath = path;
	}

	public String getLocalPath(){
		return this.localPath;
	}
}
