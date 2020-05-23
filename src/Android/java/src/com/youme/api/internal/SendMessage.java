package com.youme.api.internal;

import com.google.gson.annotations.SerializedName;

/**
 * Created by fishg on 2017/2/19.
 */
public class SendMessage {
    @SerializedName("RequestID")
    private long requestId;
    
    @SerializedName("SendTime")
    private int sendTime;

    @SerializedName("IsForbidRoom")
    private int isForbidRoom;

    @SerializedName("reasonType")
    private int reasonType;

    @SerializedName("forbidEndTime")
    private String endTime;

    @SerializedName("messageID")
    private Long messageID;

    public long getRequestId() {
        return requestId;
    }

    public void setRequestId(long requestId) {
        this.requestId = requestId;
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
        this.isForbidRoom = (isForbidRoom == false ? 0 : 1);;
    }

    public int getReasonType() {
        return reasonType;
    }

    public void setReasonType(int reasonType) {
        this.reasonType = reasonType;
    }

    public long getEndTime() {
        if(endTime == null || endTime.isEmpty()){
            return 0;
        }
        try {
            long t = Long.parseLong(endTime);
            return t;
        }catch (Throwable e) {
            return Long.MAX_VALUE;
        }
    }

    public String getEndTimeStr() {
        return this.endTime;
    }

    public void setEndTime(long endTime) {
        this.endTime = ""+endTime;
    }

    public long getMessageID() {
        return messageID;
    }

    public void setMessageID(long msgID) {
        this.messageID = msgID;
    }
}
