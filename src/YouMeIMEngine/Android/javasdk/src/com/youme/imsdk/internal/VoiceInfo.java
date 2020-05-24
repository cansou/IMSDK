package com.youme.imsdk.internal;

import com.google.gson.annotations.SerializedName;

/**
 * Created by winnie on 2018/8/13.
 */

public class VoiceInfo {
    @SerializedName("RequestID")
    public Long requestId;

    @SerializedName("Text")
    public String extraText;

    @SerializedName("LocalPath")
    public String localPath;

    @SerializedName("Duration")
    public Integer duration;
}
