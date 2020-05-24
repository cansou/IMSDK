package com.youme.imsdk.internal;


import com.google.gson.annotations.SerializedName;


public class RoomInfo {

	@SerializedName("RoomID")
    public String roomID;
	
	@SerializedName("Count")
    public int memberCount;
}
