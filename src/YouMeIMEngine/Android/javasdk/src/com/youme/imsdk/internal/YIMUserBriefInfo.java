package com.youme.imsdk.internal;
import com.google.gson.annotations.SerializedName;

public class YIMUserBriefInfo {
	@SerializedName("UserID")
	public String userID;

    @SerializedName("Nickname")
    public String nickName;
    
    @SerializedName("Status")
    public int userStatus;   
}
