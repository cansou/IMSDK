package com.youme.imsdk;

import com.google.gson.annotations.SerializedName;
import com.youme.im.IMEngine;

/**
 * Created by caoyong on 2017/4/27.
 */

public class YIMForbiddenSpeakInfo {

    @SerializedName("ChannelID")
    private String channelID;

    @SerializedName("IsForbidRoom")
    private Integer isForbidRoom;


    @SerializedName("reasonType")
    private Integer reasonType;

    @SerializedName("forbidEndTime")
    private Long  endTime;

    public String getChannelID(){
        return channelID;
    }

    public boolean getIsForbidRoom() {
        return isForbidRoom!=0;
    }

    public int getReasonType(){
        return reasonType;
    }

    public long getEndTime(){
        return endTime;
    }
}
