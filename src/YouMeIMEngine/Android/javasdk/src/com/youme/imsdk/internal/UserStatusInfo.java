package com.youme.imsdk.internal;

import com.google.gson.annotations.SerializedName;

/**
 * Created by fishg on 2017/2/19.
 */
public class UserStatusInfo {
    @SerializedName("UserID")
    public String userID;

    @SerializedName("Status")
    public int status;

    public  boolean isOnline(){
        return status ==0;
    }
}
