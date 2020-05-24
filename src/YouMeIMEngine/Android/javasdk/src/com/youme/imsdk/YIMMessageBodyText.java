package com.youme.imsdk;

public class YIMMessageBodyText implements IYIMMessageBodyBase {
	//private int mMsgType = YouMeIMManager.MessageBodyType_Unknow;
	private String mMsgContent = null;

	private String mAttachParam = null;
	/*@Override
	public int getMessageType() {
		return mMsgType;
	}
	
	public void setMessageType(int msgType){
		mMsgType = msgType;
	}*/

	public void setMessageContent(String msgContent){
		mMsgContent = msgContent;
	}
	
	public String getMessageContent(){
		return mMsgContent;
	}

	public String getAttachParam() {
		return mAttachParam;
	}

	public void setAttachParam(String attachParam) {
		this.mAttachParam = attachParam;
	}
}
