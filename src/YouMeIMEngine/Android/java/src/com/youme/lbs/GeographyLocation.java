package com.youme.lbs;

import java.util.HashMap;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;

import com.anthonycr.grant.PermissionsManager;
import com.anthonycr.grant.PermissionsResultAction;
import com.youme.im.CommonConst;
import com.youme.im.IMEngine;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.location.Criteria;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Looper;
import android.util.Log;


public class GeographyLocation {
	
	private static final String LOG_TAG = "GeographyLocation";
	private Context m_context;
	private LocationManager m_locactionManager = null;
	private IGeographyLocationCallback m_callback = null;
	private boolean mIsRunning = false;
	private boolean mIsAuthorize = true;
	private Location m_currentBestLocation = null;
	private Location m_lastLocation = null;
	private int m_currentTimeout;
	private int m_updateCount = 0;
	private Looper m_threadLooper;
	private LBSThread m_lbsThread;
	private Timer m_timer = null;
	private TimerTask m_timerTask = null;
	private Map<String, GeoLocationListener> m_geoLocationListeners = new HashMap<String, GeoLocationListener>();
	
	private static final int TWO_MINUTES = 1000 * 60 * 2;
	private static final int MIN_TIME = 60000;
	private static final float MIN_DISTANCE = 1000;
	private static final int LOCATION_TIMEOUT_MAX = 90000;
	private static final int LOCATION_TIMEOUT_MIN = 10000;
	
	private static final short LOCATION_SUCCESS = 0;
	private static final short LOCATION_FAILED = 1;
	private static final short LOCATION_TIMEOUT = 2;
	
	
	public void Init(Context context) {
		this.m_context = context;
	}
	
	public void SetCallback(IGeographyLocationCallback callback) {
		this.m_callback = callback; 
	}
	
	private void StartTimer() {
		if (m_timerTask == null) {
			m_timerTask = new TimerTask() {
				@Override
				public void run() {
					if (m_currentTimeout == LOCATION_TIMEOUT_MIN) {
						IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_INFO, "timer reached 1");
						
						m_currentTimeout = LOCATION_TIMEOUT_MAX;
						if (m_callback != null && m_lastLocation != null && m_updateCount == 0) {
							Location location = m_lastLocation;
							if (m_currentBestLocation != null && IsBetterLocation(m_currentBestLocation, m_lastLocation)) {
								location = m_currentBestLocation;
								m_lastLocation = m_currentBestLocation;
							}
							m_callback.OnUploadGeoLocation(IGeographyLocationCallback.LocationErrorcode.LOCATIONERROR_SUCCESS, location.getLongitude(), location.getLatitude());
						}
					} else {
						IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_INFO, "timer reached 2");
						StopTimer(LOCATION_TIMEOUT);
					}
				}
			};
		}
		if (m_timer == null) {
			m_timer = new Timer();
		}
		if (m_timerTask != null && m_timer != null) {
			if (m_lastLocation != null) {
				// 若上一次定位结果不为空，以LOCATION_TIMEOUT_MIN作为第一次超时回调，之后以LOCATION_TIMEOUT_MAX - LOCATION_TIMEOUT_MIN作为第二次超时，视定位结果进行第二次回调
				m_currentTimeout = LOCATION_TIMEOUT_MIN;
				m_timer.schedule(m_timerTask, m_currentTimeout, LOCATION_TIMEOUT_MAX - LOCATION_TIMEOUT_MIN);
			} else {
				m_currentTimeout = LOCATION_TIMEOUT_MAX;
				m_timer.schedule(m_timerTask, m_currentTimeout);
			}
		}
	}
	
	private void StopTimer(int result) {
		if (m_threadLooper != null) {
			m_threadLooper.quit();
			m_threadLooper = null;
		}
		if (m_lbsThread != null) {
			m_lbsThread = null;
		}
		StopRequestLocationUpdates();
		if (m_timer != null) {
			m_timer.cancel();
		}
		if (m_timerTask != null) {
			m_timerTask.cancel();
		}
		m_timer = null;
		m_timerTask = null;
		
		mIsRunning = false;
		
		NotifyLocationResult(result);
	}
	
	public void NotifyLocationResult(int result){
		IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_INFO, "NotifyLocationResult result:" + result);
		
		Location location = null;
		IGeographyLocationCallback.LocationErrorcode errorcode = IGeographyLocationCallback.LocationErrorcode.LOCATIONERROR_SUCCESS;
		if (LOCATION_SUCCESS == result) {
			if (m_updateCount == 0 || IsBetterLocation(m_currentBestLocation, m_lastLocation)){
				location = m_currentBestLocation;
			}
		} else if (LOCATION_FAILED == result) {
			errorcode = IGeographyLocationCallback.LocationErrorcode.LOCATIONERROR_FAILED;
		} else if (LOCATION_TIMEOUT == result) {
			// 超时情况可能已经回调lastLocation，只有currentBestLocation比lastLocation精确才第二次回调
			if (m_currentBestLocation == null) {
				errorcode = IGeographyLocationCallback.LocationErrorcode.LOCATIONERROR_TIMEOUT;
			} else if (m_lastLocation == null){
				location = m_currentBestLocation;
			} else if (m_currentBestLocation != null && m_lastLocation != null && IsBetterLocation(m_currentBestLocation, m_lastLocation)) {
				location = m_currentBestLocation;
			} else {
				return;
			}
		}
		
		if (m_currentBestLocation != null) {
			m_lastLocation = m_currentBestLocation;
		}
		
		if (m_callback != null) {
			if (location != null) {
				m_callback.OnUploadGeoLocation(errorcode, location.getLongitude(), location.getLatitude());
			} else {
				m_callback.OnUploadGeoLocation(errorcode, -180, -180);
			}
		}
	}
	
	public IGeographyLocationCallback.LocationErrorcode GetLocation() {
		if (mIsRunning) {
			IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_INFO, "location running");
			
			return IGeographyLocationCallback.LocationErrorcode.LOCATIONERROR_RUNNING;
		}
		//synchronized(this) {
			mIsRunning = true;
		//}
		
		if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && m_context!=null && m_context instanceof Activity && m_context.getApplicationInfo().targetSdkVersion >22){
			//针对android 6.0以上的授权检查
			try {
				boolean hasLocationPermission = PermissionsManager.getInstance().hasPermission(m_context, Manifest.permission.ACCESS_FINE_LOCATION);
				if(!hasLocationPermission){
					mIsAuthorize = false;
					PermissionsManager.getInstance().requestPermissionsIfNecessaryForResult((Activity)m_context,
							new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, new PermissionsResultAction() {

							@Override
							public void onGranted() {
								Log.i(LOG_TAG, " Success granted ACCESS_FINE_LOCATION permission.");
							}

							@Override
							public void onDenied(String permission) {
								mIsRunning = false;
								IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_WARNING, "Decline granted ACCESS_FINE_LOCATION permission.");
							}
					});
					
					IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_WARNING, "Not granted ACCESS_FINE_LOCATION permission");
					if (!mIsRunning){
						return IGeographyLocationCallback.LocationErrorcode.LOCATIONERROR_AUTHORIZE;
					}
				} else {
					mIsAuthorize = true;
				}
			}catch (Exception e ){
				Log.e(LOG_TAG, e.toString());
				IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_ERROR, "location permission exception");
				return IGeographyLocationCallback.LocationErrorcode.LOCATIONERROR_AUTHORIZE;
			}
		}
		
		if (m_locactionManager == null) {
			m_locactionManager = (LocationManager) m_context.getSystemService(Context.LOCATION_SERVICE);
			if (m_locactionManager == null) {
				IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_ERROR, "get location service failed");
				
				m_locactionManager = (LocationManager) m_context.getSystemService(Context.LOCATION_SERVICE);
				mIsRunning = false;
				return IGeographyLocationCallback.LocationErrorcode.LOCATIONERROR_INIT_FAILED;
			}
		}
		
		boolean gpsEnable = m_locactionManager.isProviderEnabled(LocationManager.GPS_PROVIDER);
		boolean newworkEnable = m_locactionManager.isProviderEnabled(LocationManager.NETWORK_PROVIDER);
		boolean passiveEnable = m_locactionManager.isProviderEnabled(LocationManager.PASSIVE_PROVIDER);
		
		if (gpsEnable) {
			if (!m_geoLocationListeners.containsKey(LocationManager.GPS_PROVIDER)) {
				m_geoLocationListeners.put(LocationManager.GPS_PROVIDER, new GeoLocationListener());
			}
		} else {
			m_geoLocationListeners.remove(LocationManager.GPS_PROVIDER);
		}
		
		if (newworkEnable) {
			if (!m_geoLocationListeners.containsKey(LocationManager.NETWORK_PROVIDER)) {
				m_geoLocationListeners.put(LocationManager.NETWORK_PROVIDER, new GeoLocationListener());
			}
		} else {
			m_geoLocationListeners.remove(LocationManager.NETWORK_PROVIDER);
		}
		
		/*if (passiveEnable) {
			if (!geoLocationListeners.containsKey(LocationManager.PASSIVE_PROVIDER)) {
				geoLocationListeners.put(LocationManager.PASSIVE_PROVIDER, new GeoLocationListener());
			}
		} else {
			geoLocationListeners.remove(LocationManager.PASSIVE_PROVIDER);
		}*/
		
		IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_INFO, "gps:" + gpsEnable + " network:" + newworkEnable + " passive:" + passiveEnable + " total:" + m_geoLocationListeners.size());
		
		if (m_geoLocationListeners.size() == 0) {
			IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_ERROR, "location unenable");
			
			mIsRunning = false;
			return IGeographyLocationCallback.LocationErrorcode.LOCATIONERROR_AUTHORIZE;
		}
		
		Criteria criteria = new Criteria();
		criteria.setAccuracy(Criteria.ACCURACY_FINE);			//设置定位精准度
		criteria.setPowerRequirement(Criteria.POWER_LOW);		//设置相对省电
		criteria.setAltitudeRequired(false);					//是否要求海拔
		criteria.setBearingRequired(false);						//是否要求方向
		criteria.setCostAllowed(true);							//是否要求收费
		criteria.setSpeedRequired(false);						//是否要求速度
		//criteria.setBearingAccuracy(Criteria.ACCURACY_LOW);	//设置方向精确度
		//criteria.setSpeedAccuracy(Criteria.ACCURACY_LOW);		//设置速度精确度
		//criteria.setHorizontalAccuracy(Criteria.ACCURACY_LOW);//设置水平方向精确度
		//criteria.setVerticalAccuracy(Criteria.ACCURACY_LOW);	//设置垂直方向精确度
		
		m_currentBestLocation = null;
		
		String locationProvider  = m_locactionManager.getBestProvider(criteria, true);
		if (locationProvider != null){
			Log.d(LOG_TAG, " provider:" + locationProvider);
			Location location = m_locactionManager.getLastKnownLocation(locationProvider);
			if (location != null) {
				m_currentBestLocation = location;
				IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_INFO, "last location:" + m_currentBestLocation.getLongitude() + "x" + m_currentBestLocation.getLatitude() + " provider:" + locationProvider);
			}
		}
		
		StartTimer();
		m_lbsThread = new LBSThread();
		m_lbsThread.start();
		//StartRequestLocationUpdates();
		
		return IGeographyLocationCallback.LocationErrorcode.LOCATIONERROR_SUCCESS;
	}
	
	public boolean StartRequestLocationUpdates(){				
		m_updateCount = 0;
		try{
			if (m_geoLocationListeners.containsKey(LocationManager.GPS_PROVIDER)) {
				m_locactionManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, MIN_TIME, MIN_DISTANCE, m_geoLocationListeners.get(LocationManager.GPS_PROVIDER));
			}
			if (m_geoLocationListeners.containsKey(LocationManager.NETWORK_PROVIDER)) {
				m_locactionManager.requestLocationUpdates(LocationManager.NETWORK_PROVIDER, MIN_TIME, MIN_DISTANCE, m_geoLocationListeners.get(LocationManager.NETWORK_PROVIDER));
			}
		} catch (Exception e) {
			e.printStackTrace();
			return false;
		}
		
		return true;
	}
	
	public void StopRequestLocationUpdates(){		
		Log.d(LOG_TAG, "stop listen");
		
		if (m_geoLocationListeners.containsKey(LocationManager.GPS_PROVIDER)) {
			m_locactionManager.removeUpdates(m_geoLocationListeners.get(LocationManager.GPS_PROVIDER));
            m_geoLocationListeners.remove(LocationManager.GPS_PROVIDER);
		}
		if (m_geoLocationListeners.containsKey(LocationManager.NETWORK_PROVIDER)) {
			m_locactionManager.removeUpdates(m_geoLocationListeners.get(LocationManager.NETWORK_PROVIDER));
            m_geoLocationListeners.remove(LocationManager.NETWORK_PROVIDER);
		}
	}
	
	protected boolean IsBetterLocation(Location location, Location currentBestLocation) {
	    if (currentBestLocation == null) {
	        return true;
	    }

	    // Check whether the new location fix is newer or older
	    long timeDelta = location.getTime() - currentBestLocation.getTime();
	    boolean isSignificantlyNewer = timeDelta > TWO_MINUTES;
	    boolean isSignificantlyOlder = timeDelta < -TWO_MINUTES;
	    boolean isNewer = timeDelta > 0;

	    // If it's been more than two minutes since the current location, use the new location
	    // because the user has likely moved
	    if (isSignificantlyNewer) {
	        return true;
	    // If the new location is more than two minutes older, it must be worse
	    } else if (isSignificantlyOlder) {
	        return false;
	    }

	    // Check whether the new location fix is more or less accurate
	    int accuracyDelta = (int) (location.getAccuracy() - currentBestLocation.getAccuracy());
	    boolean isLessAccurate = accuracyDelta > 0;
	    boolean isMoreAccurate = accuracyDelta < 0;
	    boolean isSignificantlyLessAccurate = accuracyDelta > 200;

	    // Check if the old and new location are from the same provider
	    boolean isFromSameProvider = IsSameProvider(location.getProvider(),
	            currentBestLocation.getProvider());

	    // Determine location quality using a combination of timeliness and accuracy
	    if (isMoreAccurate) {
	        return true;
	    } else if (isNewer && !isLessAccurate) {
	        return true;
	    } else if (isNewer && !isSignificantlyLessAccurate && isFromSameProvider) {
	        return true;
	    }
	    return false;
	}

	private boolean IsSameProvider(String provider1, String provider2) {
	    if (provider1 == null) {
	      return provider2 == null;
	    }
	    return provider1.equals(provider2);
	}
	
	
	public class LBSThread extends Thread{
		@Override
		public void run() {
			IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_INFO, "thread start");
			Looper.prepare();
			m_threadLooper = Looper.myLooper();
			if (!mIsAuthorize){
				try {
					Thread.sleep(2000);
				} catch (InterruptedException e) {
					e.printStackTrace();
					StopTimer(LOCATION_FAILED);
					return;
				}
			}
			boolean ret = StartRequestLocationUpdates();
			if (!ret){
				StopTimer(LOCATION_FAILED);
				
				IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_ERROR, "start location failed");
				return;
			}
			IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_INFO, "thread running");
			Looper.loop();
			IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_INFO, "thread exit");
		}
	}
	
	
	private class GeoLocationListener implements LocationListener {
		@Override
		public void onLocationChanged(Location location) {
			//if (location.getLatitude() == 0.0 && location.getLongitude() == 0.0) {
            //    return;
            //}
            
			IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_INFO, "location changed longtitude:" + location.getLongitude() + " latitude:" + location.getLatitude() + " provider:" + location.getProvider());
            
            boolean bRet = IsBetterLocation(location, m_currentBestLocation);
            if (bRet) {
            	m_currentBestLocation = location;
			}
            
            if (location.getProvider().equals(LocationManager.GPS_PROVIDER)) {
            	if (m_geoLocationListeners.containsKey(LocationManager.GPS_PROVIDER)) {
        			m_locactionManager.removeUpdates(m_geoLocationListeners.get(LocationManager.GPS_PROVIDER));
                    m_geoLocationListeners.remove(LocationManager.GPS_PROVIDER);
        		}
            }
            if (location.getProvider().equals(LocationManager.NETWORK_PROVIDER)) {
            	if (m_geoLocationListeners.containsKey(LocationManager.NETWORK_PROVIDER)) {
        			m_locactionManager.removeUpdates(m_geoLocationListeners.get(LocationManager.NETWORK_PROVIDER));
                    m_geoLocationListeners.remove(LocationManager.NETWORK_PROVIDER);
        		}
            }
            
            if (++m_updateCount >= m_geoLocationListeners.size()) {
            	StopTimer(LOCATION_SUCCESS);
			} else {
				NotifyLocationResult(LOCATION_SUCCESS);
			}
		}
		
		@Override
		public void onStatusChanged(String provider, int status, Bundle extras) {
			Log.i(LOG_TAG, "onStatusChanged provider:" + provider + " status:" + status);
		}
		
		@Override
		public void onProviderEnabled(String provider) {
			Log.i(LOG_TAG, "provider enabled name:" + provider);
		}

		@Override
		public void onProviderDisabled(String provider) {
			Log.i(LOG_TAG, "provider disabled name:" + provider);
		}
	}
}