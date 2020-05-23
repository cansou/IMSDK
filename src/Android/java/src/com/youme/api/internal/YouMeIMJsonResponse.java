package com.youme.api.internal;

import com.google.gson.annotations.SerializedName;

/**
 * Created by fishg on 2017/2/19.
 */
public class YouMeIMJsonResponse {
    @SerializedName("Command")
    public Integer command;

    @SerializedName("Errorcode")
    public Integer errcode;
}
