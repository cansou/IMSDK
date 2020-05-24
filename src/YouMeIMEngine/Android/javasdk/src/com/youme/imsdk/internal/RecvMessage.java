package com.youme.imsdk.internal;

import com.google.gson.annotations.SerializedName;

/**
 * Created by fishg on 2017/2/19.
 */
public class RecvMessage {
    @SerializedName("ChatType")
    public Integer chatType;

    @SerializedName("SenderID")
    public String senderId;

    @SerializedName("ReceiveID")
    public String receiveId;

    @SerializedName("Serial")
    public Long serial;

    @SerializedName("MessageType")
    public Integer msgType;

    @SerializedName("Content")
    public String content;

    @SerializedName("AttachParam")
    public String attachParam;

    @SerializedName("CreateTime")
    public Integer createTime;

    @SerializedName("Text")
    public String extraText;

    @SerializedName("Param")
    public String param;

    @SerializedName("Duration")
    public Integer duration;

    // file for file msg
    @SerializedName("FileName")
    public String fileName;

    @SerializedName("FileSize")
    public Integer fileSize;

    @SerializedName("FileType")
    public Integer fileType;

    @SerializedName("FileExtension")
    public String fileExtension;

    @SerializedName("ExtraParam")
    public String extraParam;

    // for gift msg
    @SerializedName("GiftID")
    public Integer giftID;

    @SerializedName("GiftCount")
    public Integer giftCount;

    @SerializedName("Anchor")
    public String anchor;

    @SerializedName("Distance")
    public Integer distance;
    
    @SerializedName("IsRead")
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

}
