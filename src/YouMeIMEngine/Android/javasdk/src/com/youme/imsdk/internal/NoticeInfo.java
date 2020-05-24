package com.youme.imsdk.internal;

import com.google.gson.annotations.SerializedName;

public class NoticeInfo {

    @SerializedName("NoticeID")
    public long iNoticeID;
    
    @SerializedName("NoticeType")
	public int iNoticeType;
    
    @SerializedName("ChannelID")
	public String strChannelID;
    
    @SerializedName("NoticeContent")
	public String strContent;
    
    @SerializedName("LinkeText")
	public String strLinkeText;
    
    @SerializedName("LinkAddress")
	public String strLinkAddr;
    
    @SerializedName("BeginTime")
	public int iBeginTime;
    
    @SerializedName("EndTime")
	public int iEndTime;
}
