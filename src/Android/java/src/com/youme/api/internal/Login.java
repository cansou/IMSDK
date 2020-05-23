package com.youme.api.internal;

import com.google.gson.annotations.SerializedName;

/**
 * Created by fishg on 2017/2/19.
 */
public class Login {
    @SerializedName("UserID")
    private String youmeId;

    public String getYoumeId() {
        return youmeId;
    }

    public void setYoumeId(String youmeId) {
        this.youmeId = youmeId;
    }
}
