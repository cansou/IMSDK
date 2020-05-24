/**
 * 消息已读状态更新
 * created by bruce 2020/4/15
 */


package com.youme.imsdk.internal;

import com.google.gson.annotations.SerializedName;

public class MsgReadStatus {
    @SerializedName("RecvId")
    private String recvId;

    @SerializedName("ChatType")
    private int chatType;

    @SerializedName("MsgId")
    private long msgId;

    public void setRecvId(String recvId) {
        this.recvId = recvId;
    }

    public String getRecvId() {
        return recvId;
    }

    public void setChatType(int chatType) {
        this.chatType = chatType;
    }

    public int getChatType() {
        return chatType;
    }

    public void setMsgId(long msgId) {
        this.msgId = msgId;
    }

    public long getMsgId() {
        return msgId;
    }
}
