package com.youme.imsdk.internal;
import com.google.gson.annotations.SerializedName;

public class YIMFriendDealResult {
	@SerializedName("UserID")
    public String userID;

	@SerializedName("Comments")
    public String comments;
	
	@SerializedName("DealResult")
	public int dealResult;
}
