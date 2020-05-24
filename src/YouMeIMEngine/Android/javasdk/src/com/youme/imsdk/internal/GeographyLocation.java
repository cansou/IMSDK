package com.youme.imsdk.internal;

import com.google.gson.annotations.SerializedName;

/**
 * Created by caoyong on 2017/4/27.
 */

public class GeographyLocation {

    @SerializedName("DistrictCode")
    public int iDistrictCode;

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

    @SerializedName("Longitude")
    public double fLongitude;

    @SerializedName("Latitude")
    public double fLatitude;

}
