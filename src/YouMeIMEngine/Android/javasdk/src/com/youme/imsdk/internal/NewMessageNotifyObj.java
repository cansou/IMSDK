package com.youme.imsdk.internal;

import com.google.gson.annotations.SerializedName;

/**
 * Created by fishg on 2017/4/17.
 */

public class NewMessageNotifyObj {

    @SerializedName("TargetID")
    public String targetID;

    @SerializedName("ChatType")
    public Integer chatType;
}
