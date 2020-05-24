package com.youme.imsdk.internal;

import com.google.gson.annotations.SerializedName;

/**
 * Created by fishg on 2017/2/19.
 */
public class ChatVoice {
    @SerializedName("RequestID")
    public Long requestId;

    @SerializedName("Text")
    public String extraText;

    @SerializedName("LocalPath")
    public String localPath;

    @SerializedName("Duration")
    public Integer duration;
    
    @SerializedName("SendTime")
    public Integer sendTime;

    @SerializedName("IsForbidRoom")
    public Integer isForbidRoom;

    @SerializedName("reasonType")
    public Integer reasonType;

    @SerializedName("forbidEndTime")
    public Long endTime;
}
