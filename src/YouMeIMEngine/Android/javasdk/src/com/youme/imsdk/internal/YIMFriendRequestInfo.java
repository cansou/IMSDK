package com.youme.imsdk.internal;
import com.google.gson.annotations.SerializedName;

public class YIMFriendRequestInfo {
	@SerializedName("AskerID")
	public String askerID;

    @SerializedName("AskerNickname")
    public String askerNickname;
    
    @SerializedName("InviteeID")
    public String inviteeID;

    @SerializedName("InviteeNickname")
    public String inviteeNickname;
    
    @SerializedName("ValidateInfo")
    public String validateInfo;
    
    @SerializedName("Status")
    public int addStatus;
    
    @SerializedName("CreateTime")
    public int createTime;
}
