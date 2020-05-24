package com.youme.imsdk.internal;

import com.google.gson.annotations.SerializedName;

public class UserProfileInfo {
	@SerializedName("UserID")
	public String userID;
	
	@SerializedName("PhotoUrl")
	public String photoUrl;

	@SerializedName("OnlineState")
	public int onlineState;

	@SerializedName("BeAddPermission")
	public int beAddPermission;

	@SerializedName("FoundPermission")
	public int foundPermission;
	
	@SerializedName("NickName")
	public String nickName;

	@SerializedName("Sex")
	public int sex;

	@SerializedName("Signature")
	public String signature;

	@SerializedName("Country")
	public String country;

	@SerializedName("Province")
	public String province;

	@SerializedName("City")
	public String city;

	@SerializedName("ExtraInfo")
	public String extraInfo;
}
