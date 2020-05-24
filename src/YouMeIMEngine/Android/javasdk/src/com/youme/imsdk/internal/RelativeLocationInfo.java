package com.youme.imsdk.internal;

import com.google.gson.annotations.SerializedName;

import java.util.ArrayList;

/**
 * Created by caoyong on 2017/4/27.
 */

public class RelativeLocationInfo {

    @SerializedName("StartDistance")
    public int startDistance ;

    @SerializedName("EndDistance")
    public int endDistance ;

    @SerializedName("NeighbourList")
    public ArrayList<RelativeLocation> relativeLocations ;
}
