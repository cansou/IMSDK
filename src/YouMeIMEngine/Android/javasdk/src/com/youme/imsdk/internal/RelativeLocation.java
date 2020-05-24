package com.youme.imsdk.internal;

import com.google.gson.annotations.SerializedName;

/**
 * Created by caoyong on 2017/4/27.
 */

public class RelativeLocation {

    @SerializedName("Distance")
    public int iDistance;

    @SerializedName("Longitude")
    public double fLongitude;

    @SerializedName("Latitude")
    public double fLatitude;

    @SerializedName("UserID")
    public String strUserID;

    @SerializedName("Country")
    public String strCountry;

    @SerializedName("Province")
    public String strProvince;

    @SerializedName("City")
    public String strCity;

    @SerializedName("DistrictCounty")
    public String strDistrictCounty;

    @SerializedName("Street")
    public String strStreet;
}
