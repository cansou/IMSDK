package com.youme.imsdk;

import android.util.Base64;

import com.google.gson.annotations.SerializedName;

public class YIMHistoryMessageBody {
	@SerializedName("ChatType")
	private Integer chatType;
	@SerializedName("MessageType")
	private Integer messageType;
	@SerializedName("Param")
	private String param;
	@SerializedName("ReceiveID")
	private String receiveID;
	@SerializedName("SenderID")
	private String senderID;
	@SerializedName("Serial")
	private Long messageID;
	@SerializedName("Content")
	private String text;
	@SerializedName("Text")
	private String voiceToText;
	@SerializedName("LocalPath")
	private String localPath;
	@SerializedName("CreateTime")
	private int createTime;
	@SerializedName("Duration")
	private int duration;
	@SerializedName("IsRead")
    private int isRead; 
	@SerializedName("IsPlayed")
    private int isPlayed;
	@SerializedName("FileName")
	private String fileName;
	@SerializedName("FileExtension")
	private String fileExtension;
	@SerializedName("FileSize")
	private int fileSize;
	@SerializedName("FileType")
	private int fileType;


	public int getFileType() {
		return fileType;
	}

	public void setFileType(int fileType) {
		this.fileType = fileType;
	}
	
	public String getFileName() {
		return fileName;
	}

	public void setFileName(String fileName) {
		this.fileName = fileName;
	}

	public String getFileExtension() {
		return fileExtension;
	}

	public void setFileExtension(String fileExtension) {
		this.fileExtension = fileExtension;
	}

	public int getFileSize() {
		return fileSize;
	}

	public void setFileSize(int fileSize) {
		this.fileSize = fileSize;
	}	
	 
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
	
	public boolean getIsPlayed(){
		return (isPlayed == 1);
	}
	
	public void setPlayed(boolean isPlayed){
		if (isPlayed){
			this.isPlayed = 1;
		}else{
			this.isPlayed = 0;
		}	
	}

	public int getChatType() {
		return chatType;
	}

	public void setChatType(int chatType) {
		this.chatType = chatType;
	}

	public int getMessageType() {
		return messageType;
	}

	public void setMessageType(int messageType) {
		this.messageType = messageType;
	}

	public String getParam() {
		return param;
	}

	public void setParam(String param) {
		this.param = param;
	}

	public String getReceiveID() {
		return receiveID;
	}

	public void setReceiveID(String receiveID) {
		this.receiveID = receiveID;
	}

	public String getSenderID() {
		return senderID;
	}

	public void setSenderID(String senderID) {
		this.senderID = senderID;
	}

	public long getMessageID() {
		return messageID;
	}

	public void setMessageID(long messageID) {
		this.messageID = messageID;
	}

	public String getText() {
		if(messageType == YIMService.MessageBodyType.Voice){
			return voiceToText;
		}
		return text;
	}

	public byte[] getCustomMesssageContent(){
		if(messageType == YIMService.MessageBodyType.CustomMesssage){
			return Base64.decode(text,Base64.DEFAULT);
		}
		return null;
	}

	public void setText(String text) {
		this.text = text;
	}

	public String getLocalPath() {
		return localPath;
	}

	public void setLocalPath(String localPath) {
		this.localPath = localPath;
	}

	public int getCreateTime() {
		return createTime;
	}

	public void setCreateTime(int createTime) {
		this.createTime = createTime;
	}

	public int getDuration() {
		return duration;
	}

	public void setDuration(int duration) {
		this.duration = duration;
	}

	public String getVoiceToText() {
		return voiceToText;
	}

	public void setVoiceToText(String voiceToText) {
		this.voiceToText = voiceToText;
	}
};
