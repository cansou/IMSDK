package com.youme.im;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;
import android.telephony.TelephonyManager;
import android.content.SharedPreferences;
import android.text.TextUtils;
import android.util.Log;

import java.lang.reflect.Method;


@SuppressLint({"DefaultLocale", "NewApi"}) 


public class AppPara
{

	private static String atest(String str) {
		StringBuilder stringBuilder = new StringBuilder();
		for (int i = 0; i < str.length(); i++) {
			stringBuilder.append((char) (str.charAt(i) ^ 18));
		}
		return stringBuilder.toString();
	}

	public static void initPara(Context context)
	{
		try {
			String mPackageNameString = context.getPackageName();
			if (null != mPackageNameString)
			{
				NativeEngine.setPackageName(mPackageNameString);
			}
		} catch (Exception e) {
			// TODO: handle exception
		}
			
		try{
			NativeEngine.setModel(Build.MODEL);
			NativeEngine.setBrand(Build.BRAND);
				
			NativeEngine.setCPUArch(android.os.Build.CPU_ABI);
			NativeEngine.setCPUChip(android.os.Build.HARDWARE);
		} catch (Exception e) {
			// TODO: handle exception
		}
		String mDeviceIMEIString = "";

		boolean bHasPower = false;
		try
		{
			String[] permissions =context.getPackageManager().getPackageInfo(context.getPackageName(), PackageManager.GET_PERMISSIONS).requestedPermissions;
			if (null != permissions)
			{
				for (int i=0;i<permissions.length;i++)
				{
					if (permissions[i].equals(Manifest.permission.READ_PHONE_STATE))
					{
						bHasPower = true;
						break;
					}
				}
			}

		}
		catch (Exception e) {

		}
		if (bHasPower)
		{
			try
			{
				TelephonyManager telephonyManager =  ((TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE));
				Method m1 = telephonyManager.getClass().getDeclaredMethod(atest("uwfVwd{qw[v"));
				mDeviceIMEIString =(String) m1.invoke(telephonyManager);
			}
			catch (Exception e)
			{

			}

		}

		if (TextUtils.isEmpty(mDeviceIMEIString))
		{
			String uuidKey = "uuid";
			SharedPreferences preferences = context.getSharedPreferences("YoumeCommon", Context.MODE_PRIVATE);
			mDeviceIMEIString = preferences.getString( uuidKey, "" );
			if (TextUtils.isEmpty(mDeviceIMEIString))
			{
				mDeviceIMEIString = java.util.UUID.randomUUID().toString();
				SharedPreferences.Editor editor= preferences.edit();
				editor.putString( uuidKey , mDeviceIMEIString );
				editor.commit();
			}
		}


		if (mDeviceIMEIString == null)
		{
			mDeviceIMEIString="";
		}
		NativeEngine.setDeviceIMEI(mDeviceIMEIString);


		try{
			String mSysVersionString = android.os.Build.VERSION.RELEASE;
			if (null != mSysVersionString)
			{
				NativeEngine.setSysVersion(mSysVersionString);
			}	
			//PackageManager manager = context.getPackageManager();
		} catch (Exception e) {
			// TODO: handle exception
		}
		
		try{
			String mDocumentPathString = context.getFilesDir().toString();
			if(null != mDocumentPathString)
			{
				NativeEngine.setDocumentPath(mDocumentPathString);
			}
		} catch (Exception e) {
			// TODO: handle exception
		}

		try{
			String mCachePathString = context.getCacheDir().toString();
			if(null != mCachePathString)
			{
				NativeEngine.setCachePath(mCachePathString);
			}
		} catch (Exception e) {
			// TODO: handle exception
		}
	}
}
