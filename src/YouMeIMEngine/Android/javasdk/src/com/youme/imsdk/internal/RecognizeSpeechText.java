package com.youme.imsdk.internal;

import com.google.gson.annotations.SerializedName;

public class RecognizeSpeechText {
	@SerializedName("RequestID")
    public Long requestId;
	@SerializedName("Text")
    public String text;
}
