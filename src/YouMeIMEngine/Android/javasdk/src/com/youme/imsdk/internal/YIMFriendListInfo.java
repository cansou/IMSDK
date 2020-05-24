package com.youme.imsdk.internal;
import java.util.ArrayList;

import com.google.gson.annotations.SerializedName;

public class YIMFriendListInfo {
	@SerializedName("Type")
	public int type;
	
	@SerializedName("StartIndex")
	public int startIndex;
	
	@SerializedName("UserList")
    public ArrayList<YIMUserBriefInfo> userList ;
}
