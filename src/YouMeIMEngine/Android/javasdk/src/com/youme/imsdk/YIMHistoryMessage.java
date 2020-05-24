package com.youme.imsdk;

import com.google.gson.annotations.SerializedName;

import java.util.List;

public class YIMHistoryMessage {
	@SerializedName("Remain")
	public int remain;

	@SerializedName("TargetID")
	public String targetID;

	@SerializedName("messageList")
	public List<YIMHistoryMessageBody> messageList;
};
