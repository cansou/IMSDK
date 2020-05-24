package com.youme.imsdk.internal;

import com.google.gson.annotations.SerializedName;


public class BlockUserInfo {

    @SerializedName("UserID")
    private String userID;
    
    @SerializedName("Block")
    private Integer block;

    public String getUserID() {
        return userID;
    }

    public void setUserID(String userID) {
        this.userID = userID;
    }

    public boolean getBlock() {
        return (block == 1);
    }

    public void setBlock(boolean block) {
        this.block = (block == true ? 1 : 0);
    }
}
