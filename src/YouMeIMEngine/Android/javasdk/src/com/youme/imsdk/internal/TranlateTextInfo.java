package com.youme.imsdk.internal;

import com.google.gson.annotations.SerializedName;

/**
 * Created by caoyong on 2017/4/27.
 */

public class TranlateTextInfo {

    @SerializedName("RequestID")
    public int requestID ;

    @SerializedName("Text")
    public String text ;

    @SerializedName("SrcLangCode")
    public int srcLangCode ;

    @SerializedName("DestLangCode")
    public int destLangCode ;
}
