package com.youme.imsdk;

public class YIMMessage {
	private int mChatType = YIMService.ChatType.Unknow;
	private String mReceiveId = null;
	/*private String mGroupId = null;*/
	private String mSenderId = null;
	private IYIMMessageBodyBase mMessageBody = null;
	private long mMsgId = 0;
	private int mMsgType = YIMService.MessageBodyType.Unknow;
	private int createTime;
	private int distance;
	private int isRead;
		
	public boolean getIsRead() {
		return (isRead == 1);
	}

	public void setRead(boolean isRead) {
		if (isRead){
			this.isRead = 1;
		}else{
			this.isRead = 0;
		}		
	}
	
	public int getDistance() {
		return distance;
	}

	public void setDistance(int distance) {
		this.distance = distance;
	}

	// 聊天类型
	public int getChatType(){
		return mChatType;
	}

	// 接收者
	public String getReceiveID(){
		return mReceiveId;
	}

	// 群组ID(空：私聊 非空：群组 聊天室)
	/*public String getGroupID(){
		return mGroupId;
	}*/

	// 发送者
	public String getSenderID(){
		return mSenderId;
	}

	// 消息体
	public IYIMMessageBodyBase getMessageBody(){
		return mMessageBody;
	}

	public int getMessageType(){
		return mMsgType;
	}
	
	public long getMessageID(){
		return mMsgId;
	}
	
	public void setChatType(int chatType){
		mChatType = chatType;
	}
	
	public void setReceiveID(String receiveId){
		mReceiveId = receiveId;
	}
	
	/*public void setGroupID(String groupId){
		mGroupId = groupId;
	}*/
	
	public void setSenderID(String senderId){
		mSenderId = senderId;
	}
	
	public void setMessageBody(IYIMMessageBodyBase msgBody){
		mMessageBody = msgBody;
	}
	
	public void setMeesageID(long msgId){
		mMsgId = msgId;
	}
	
	public void setMessageType(int type){
		mMsgType = type;
	}

	public int getCreateTime() {
		return createTime;
	}

	public void setCreateTime(int createTime) {
		this.createTime = createTime;
	}
};
