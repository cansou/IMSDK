package com.youme.imsdk;

import android.util.Base64;

public class YIMMessageBodyCustom implements IYIMMessageBodyBase {
	//private int mMsgType = YouMeIMManager.MessageBodyType_Unknow;
	private byte[] mMsgContent;
	/*@Override
	public int getMessageType() {
		return mMsgType;
	}

	public void setMessageType(int msgType){
		mMsgType = msgType;
	}*/
	
	public void setMessageContent(String content){
		mMsgContent = Base64.decode(content,Base64.DEFAULT);
	}
	
	public byte[] getMessageContent(){
		return mMsgContent;
	}

}
