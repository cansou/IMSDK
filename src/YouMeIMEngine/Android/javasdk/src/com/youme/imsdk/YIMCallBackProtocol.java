package com.youme.imsdk;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import android.util.Log;

public class YIMCallBackProtocol implements Runnable {
	private final static String TAG = YIMCallBackProtocol.class.getSimpleName();

	private Object mObject = null;
	private String mMethodName = "";
	private Object[] mParams = null;
	private Class<?>[] mParamTypes = null;
	
	public YIMCallBackProtocol(Object object, String method, Object...params) {
		if( object != null ) {
			mObject = object;
			mMethodName = method;
			mParams = params;
			getParamType(mParams);
			Log.d(TAG, "object lass : " + mObject.getClass().getName() + ", method : " + mMethodName);
			for (int i = 0; i < mParams.length; ++i) {
				Log.d(TAG, "param[" + i + "]" + mParams[i] + ", type:" + mParamTypes[i].getSimpleName());
			}
		}
	}
	
	
	
	@Override
	public void run() {
		try{
			if( mObject != null ) {
				Method callback = mObject.getClass().getMethod(mMethodName, mParamTypes);
				callback.invoke(mObject, mParams);
			}
		}catch(NullPointerException e){
			e.printStackTrace();
		}catch (NoSuchMethodException e) {
			Log.e(TAG, "can't find method:" + mMethodName);
			e.printStackTrace();
		}catch(IllegalAccessException e){
			e.printStackTrace();
		}catch (IllegalArgumentException e) {
			e.printStackTrace();
		}catch (InvocationTargetException e) {
			e.printStackTrace();
		}
	}

	private void getParamType(Object[] params){
		mParamTypes = new Class[params.length];
		for (int i = 0; i < params.length; ++i){
			mParamTypes[i] = params[i].getClass();
		}
	}
}
