package com.youme.imsdk.internal;

import com.google.gson.annotations.SerializedName;

public class UploadProgress {
    @SerializedName("RequestId")
    private long requestId;

    @SerializedName("Percent")
    private float percent;

    public void setRequestId(long requestId) {
        this.requestId = requestId;
    }

    public long getRequestId() {
        return requestId;
    }

    public void setPercent(float percent) {
        this.percent = percent;
    }

    public float getPercent() {
        return percent;
    }
}
