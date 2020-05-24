package com.youme.imsdk.internal;

import com.youme.imsdk.YIMMessage;

import java.util.ArrayList;

/**
 * Created by winnie on 2018/8/14.
 */

public class RoomHistoryMsgInfo {
    private String roomID;
    private int remain;
    private ArrayList<YIMMessage> msgInfoList;

    public RoomHistoryMsgInfo(String roomID, int remain, ArrayList<YIMMessage> msgInfoList){
        this.roomID = roomID;
        this.remain = remain;
        this.msgInfoList = msgInfoList;
    }

    public String getRoomID() {
        return roomID;
    }

    public void setRoomID(String roomID) {
        this.roomID = roomID;
    }

    public int getRemain() {
        return remain;
    }

    public void setRemain(int remain) {
        this.remain = remain;
    }

    public ArrayList<YIMMessage> getMessageList() {
        return msgInfoList;
    }

    public void setMessageList(ArrayList<YIMMessage> messageList) {
        this.msgInfoList = messageList;
    }
}
