package com.youme.imsdk.internal;

import com.google.gson.annotations.SerializedName;

public class AccusationResult {

    @SerializedName("Result")
    public int iResult;
    
    @SerializedName("UserID")
    public String strUserID;
    
    @SerializedName("AccusationTime")
    public int iAccusationTime;
}
