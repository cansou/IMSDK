package com.youme.imsdk.internal;

import com.google.gson.annotations.SerializedName;

/**
 * Created by fishg on 2017/2/19.
 */
public class UserDistanceInfo {
    @SerializedName("UserID")
    public String userID;

    @SerializedName("Distance")
    public int distance;
}
