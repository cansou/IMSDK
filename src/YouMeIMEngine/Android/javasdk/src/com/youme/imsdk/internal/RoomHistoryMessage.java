package com.youme.imsdk.internal;

import java.util.ArrayList;

import com.google.gson.annotations.SerializedName;

/**
 * Created by fishg on 2017/2/19.
 */
public class RoomHistoryMessage {
    @SerializedName("RoomID")
    public String roomID;

    @SerializedName("Remain")
    public Integer remain;
    
    @SerializedName("MessageList")
    public ArrayList<RecvMessage> messageList;
}
