package com.youme.imsdk.internal;

import com.google.gson.annotations.SerializedName;

/**
 * Created by fishg on 2017/2/19.
 */
public class SendVoiceMsgInfo {
    @SerializedName("RequestID")
    private Long requestId;

    @SerializedName("Text")
    private String text;

    @SerializedName("LocalPath")
    private String localPath;

    @SerializedName("Duration")
    private Integer duration;
    
    @SerializedName("SendTime")
    private Integer sendTime;

    @SerializedName("IsForbidRoom")
    private Integer isForbidRoom;

    @SerializedName("reasonType")
    private Integer reasonType;

    @SerializedName("forbidEndTime")
    private Long endTime;

    @SerializedName("messageID")
    private Long messageID;

    public long getRequestId() {
        return requestId;
    }

    public void setRequestId(long requestId) {
        this.requestId = requestId;
    }

    public String getText() {
        return text;
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

    public int getDuration() {
        return duration;
    }

    public void setDuration(int duration) {
        this.duration = duration;
    }

    public int getSendTime() {
        return sendTime;
    }

    public void setSendTime(int sendTime) {
        this.sendTime = sendTime;
    }

    public boolean getIsForbidRoom() {
        return (isForbidRoom == 1);
    }

    public void setIsForbidRoom(boolean isForbidRoom) {
        this.isForbidRoom = (isForbidRoom == false ? 0 : 1);
    }

    public int getReasonType() {
        return reasonType;
    }

    public void setReasonType(int reasonType) {
        this.reasonType = reasonType;
    }

    public long getEndTime() {
        return endTime;
    }

    public void setEndTime(long endTime) {
        this.endTime = endTime;
    }

    public long getMessageID() {
        return messageID;
    }

    public void setMessageID(long msgID) {
        this.messageID = msgID;
    }
}
