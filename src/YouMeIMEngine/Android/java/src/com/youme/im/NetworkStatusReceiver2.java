package com.youme.im;


import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.util.Log;


interface INetworkStatus{
	public abstract void OnNetworkChange(int networkType);
}
//动态注册的方式
public class NetworkStatusReceiver2 extends BroadcastReceiver {
	
	public enum NetworkType{
		NETWORKTYPE_NONETWORK,
		NETWORKTYPE_MOBILE,
		NETWORKTYPE_WIFI,
		NETWORKTYPE_UNINET,
		NETWORKTYPE_UNIWAP,
		NETWORKTYPE_WAP_3G,
		NETWORKTYPE_NET_3G,
		NETWORKTYPE_CMWAP,
		NETWORKTYPE_CMNET,
		NETWORKTYPE_CTWAP,
		NETWORKTYPE_CTNET,
		NETWORKTYPE_LTE
	}
	
	private static NetworkInfo m_lastActiveNetworkInfo = null;
	private static WifiInfo m_lastWifiInfo = null;
	private static boolean m_isLastConnected = true;
	
		
    public static NetworkType GetNetworkType(Context context) {
        ConnectivityManager cm = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
        if (cm == null) {
			return NetworkType.NETWORKTYPE_NONETWORK;
		}
        NetworkInfo networkInfo = cm.getActiveNetworkInfo();
        if (networkInfo == null || !networkInfo.isConnected()) {
            return NetworkType.NETWORKTYPE_NONETWORK;
        }
        
        if (networkInfo.getType() == ConnectivityManager.TYPE_WIFI) {
            return NetworkType.NETWORKTYPE_WIFI;
        } else {
            /*if (networkInfo.getExtraInfo() != null) {
                if (networkInfo.getExtraInfo().equalsIgnoreCase("uninet")) {
                    return NetworkType.NETWORKTYPE_UNINET;
                }
                if (networkInfo.getExtraInfo().equalsIgnoreCase("uniwap")) {
                    return NetworkType.NETWORKTYPE_UNIWAP;
                }
                if (networkInfo.getExtraInfo().equalsIgnoreCase("3gwap")) {
                	return NetworkType.NETWORKTYPE_WAP_3G;
                }
                if (networkInfo.getExtraInfo().equalsIgnoreCase("3gnet")) {
                    return NetworkType.NETWORKTYPE_NET_3G;
                }
                if (networkInfo.getExtraInfo().equalsIgnoreCase("cmwap")) {
                    return NetworkType.NETWORKTYPE_CMWAP;
                }
                if (networkInfo.getExtraInfo().equalsIgnoreCase("cmnet")) {
                    return NetworkType.NETWORKTYPE_CMNET;
                }
                if (networkInfo.getExtraInfo().equalsIgnoreCase("ctwap")) {
                    return NetworkType.NETWORKTYPE_CTWAP;
                }
                if (networkInfo.getExtraInfo().equalsIgnoreCase("ctnet")) {
                    return NetworkType.NETWORKTYPE_CTNET;
                }
                if (networkInfo.getExtraInfo().equalsIgnoreCase("LTE")) {
                    return NetworkType.NETWORKTYPE_LTE;
                }
            }*/
            return NetworkType.NETWORKTYPE_MOBILE;
        }
    }
    
    @Override
    public void onReceive(Context context, Intent intent) {
        if (context == null || intent == null) {
            return;
        }

		ConnectivityManager cm = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo networkInfo = null;
		try {
			networkInfo = cm.getActiveNetworkInfo();
		} catch (Throwable err) {
			err.printStackTrace();
			Log.e("NetworkStatusReceiver", "getActiveNetworkInfo failed.");
		}

        if (networkInfo == null) {
        	m_lastActiveNetworkInfo = null;
            m_lastWifiInfo = null;
            if(IMEngine.m_init) {
                NativeEngine.onNetWorkChanged(NetworkType.NETWORKTYPE_NONETWORK.ordinal());
            }
        }
        else if (networkInfo.getDetailedState() != NetworkInfo.DetailedState.CONNECTED) {
            if (m_isLastConnected) {
            	m_lastActiveNetworkInfo = null;
                m_lastWifiInfo = null;
                if(IMEngine.m_init) {
                    NativeEngine.onNetWorkChanged(NetworkType.NETWORKTYPE_NONETWORK.ordinal());
                }
            }
            m_isLastConnected = false;
        }
        else {
            if (IsNetworkChange(context, networkInfo)) {
            	NetworkType netType = NetworkType.NETWORKTYPE_NONETWORK;
            	if (networkInfo.getType() == ConnectivityManager.TYPE_WIFI) {
            		netType = NetworkType.NETWORKTYPE_WIFI;
                } else {
                	/*if (networkInfo.getExtraInfo() != null) {
                        if (networkInfo.getExtraInfo().equalsIgnoreCase("uninet")) {
                        	netType = NetworkType.NETWORKTYPE_UNINET;
                        }
                        if (networkInfo.getExtraInfo().equalsIgnoreCase("uniwap")) {
                        	netType = NetworkType.NETWORKTYPE_UNIWAP;
                        }
                        if (networkInfo.getExtraInfo().equalsIgnoreCase("3gwap")) {
                        	netType = NetworkType.NETWORKTYPE_WAP_3G;
                        }
                        if (networkInfo.getExtraInfo().equalsIgnoreCase("3gnet")) {
                        	netType = NetworkType.NETWORKTYPE_NET_3G;
                        }
                        if (networkInfo.getExtraInfo().equalsIgnoreCase("cmwap")) {
                        	netType = NetworkType.NETWORKTYPE_CMWAP;
                        }
                        if (networkInfo.getExtraInfo().equalsIgnoreCase("cmnet")) {
                        	netType = NetworkType.NETWORKTYPE_CMNET;
                        }
                        if (networkInfo.getExtraInfo().equalsIgnoreCase("ctwap")) {
                        	netType = NetworkType.NETWORKTYPE_CTWAP;
                        }
                        if (networkInfo.getExtraInfo().equalsIgnoreCase("ctnet")) {
                        	netType = NetworkType.NETWORKTYPE_CTNET;
                        }
                        if (networkInfo.getExtraInfo().equalsIgnoreCase("LTE")) {
                        	netType = NetworkType.NETWORKTYPE_LTE;
                        }
                    }*/
                	netType = NetworkType.NETWORKTYPE_MOBILE;
                }
            	
            	if(IMEngine.m_init) {
                    NativeEngine.onNetWorkChanged(netType.ordinal());
                }
            }
            m_isLastConnected = true;
        }
    }
    
    public boolean IsNetworkChange(final Context context, final NetworkInfo activeNetInfo) {
        if (activeNetInfo.getType() == ConnectivityManager.TYPE_WIFI) {
            WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
			try {
				WifiInfo wi = wifiManager.getConnectionInfo();
				
				if (wi != null && m_lastWifiInfo != null && m_lastWifiInfo.getBSSID().equals(wi.getBSSID())
						&& m_lastWifiInfo.getSSID().equals(wi.getSSID())
						&& m_lastWifiInfo.getNetworkId() == wi.getNetworkId()) {
					Log.i("NetworkStatusReceiver", "return same Wifi");
					return false;
				}
				m_lastWifiInfo = wi;
			}catch(Throwable err) {
				err.printStackTrace();
				Log.e("NetworkStatusReceiver", "getConnectionInfo failed.");
			}			
			
		} else if (m_lastActiveNetworkInfo != null && m_lastActiveNetworkInfo.getExtraInfo() != null
				&& activeNetInfo.getExtraInfo() != null
				&& m_lastActiveNetworkInfo.getExtraInfo().equals(activeNetInfo.getExtraInfo())
				&& m_lastActiveNetworkInfo.getSubtype() == activeNetInfo.getSubtype()
				&& m_lastActiveNetworkInfo.getType() == activeNetInfo.getType()) {
			Log.i("NetworkStatusReceiver", "return same newtwork");
			return false;
		} else if (m_lastActiveNetworkInfo != null && m_lastActiveNetworkInfo.getExtraInfo() == null
				&& activeNetInfo.getExtraInfo() == null
				&& m_lastActiveNetworkInfo.getSubtype() == activeNetInfo.getSubtype()
				&& m_lastActiveNetworkInfo.getType() == activeNetInfo.getType()) {
			Log.i("NetworkStatusReceiver", "return same newtwork 2");
			return false;
		}
        
        m_lastActiveNetworkInfo = activeNetInfo;
        
        return true;
    }
}