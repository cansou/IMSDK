package com.youme.imsdk;

import com.google.gson.annotations.SerializedName;


public class ContactsSessionInfo {
    @SerializedName("ContactID")
    public String strContactID;
    
    @SerializedName("MessageType")
    public Integer iMessageType;

    @SerializedName("MessageContent")
    public String strMessageContent;
    
    @SerializedName("CreateTime")
    public Integer iCreateTime;
    
    @SerializedName("NotReadMsgNum")
    public Integer iNotReadMsgNum;
    
    @SerializedName("LocalPath")
    public String strLocalPath;   
}
