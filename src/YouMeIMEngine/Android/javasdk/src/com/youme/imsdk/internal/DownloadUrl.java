package com.youme.imsdk.internal;

import com.google.gson.annotations.SerializedName;

public class DownloadUrl{
    @SerializedName("FromUrl")
    public String fromUrl;

    @SerializedName("SavePath")
    public String savePath;

    @SerializedName("AudioTime")
    public int audioTime;
}
