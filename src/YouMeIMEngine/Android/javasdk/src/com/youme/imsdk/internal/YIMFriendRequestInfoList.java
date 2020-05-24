package com.youme.imsdk.internal;

import java.util.ArrayList;

import com.google.gson.annotations.SerializedName;

public class YIMFriendRequestInfoList {
	@SerializedName("StartIndex")
	public int startIndex;
	
	@SerializedName("UserList")
    public ArrayList<YIMFriendRequestInfo> userList ;
}
