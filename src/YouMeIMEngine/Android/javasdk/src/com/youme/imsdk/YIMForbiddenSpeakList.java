package com.youme.imsdk;

import com.google.gson.annotations.SerializedName;

import java.util.ArrayList;

/**
 * Created by caoyong on 2017/4/27.
 */

public class YIMForbiddenSpeakList {
    @SerializedName("ForbiddenSpeakList")
    public ArrayList<YIMForbiddenSpeakInfo> forbiddenList ;
}
