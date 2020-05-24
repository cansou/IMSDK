package com.youme.lbs;


import com.youme.im.NativeEngine;

import android.content.Context;


public class GeographyLocationManager implements IGeographyLocationCallback {
	private static GeographyLocationManager s_instance = null; 
	private static GeographyLocation geographyLocation = null; 
	
	public static GeographyLocationManager Instance(){
		if (s_instance == null) {
			s_instance = new GeographyLocationManager();
		}
		return s_instance;
	}
	
	public void Init(Context context){
		geographyLocation = new GeographyLocation();
		geographyLocation.Init(context);
		geographyLocation.SetCallback(this);
	}
	
	 public static int GetGeographyLocation() {
		if (geographyLocation != null) {
			IGeographyLocationCallback.LocationErrorcode errorcode = geographyLocation.GetLocation();
			return errorcode.ordinal();
		}
		return IGeographyLocationCallback.LocationErrorcode.LOCATIONERROR_INIT_FAILED.getValue();
	}
	
	@Override
	public void OnUploadGeoLocation(LocationErrorcode errorcode, double longitude, double latitude) {
		NativeEngine.OnUpdateLocation(errorcode.ordinal(), longitude, latitude);
	}	
}
