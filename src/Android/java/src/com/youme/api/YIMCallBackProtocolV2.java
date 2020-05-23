package com.youme.api;

import android.util.Log;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class YIMCallBackProtocolV2 implements Runnable {
	private final static String TAG = YIMCallBackProtocolV2.class.getSimpleName();

	private Object mObject = null;
	private String mMethodName = "";
	private Object[] mParams = null;
	private Class<?>[] mParamTypes = null;

	public YIMCallBackProtocolV2(Object object, String method, Object...params) {
		if( object != null ) {
			mObject = object;
			mMethodName = method;
			mParams = params;
			getParamTypeV2(mParams);
			Log.d(TAG, "object lass : " + mObject.getClass().getName() + ", method : " + mMethodName);
			for (int i = 0; i < mParams.length; ++i) {
				Log.d(TAG, "param[" + i + "]" + mParams[i] + ", type:" + mParamTypes[i].getSimpleName()+"， param： "+mParamTypes[i]);
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

	private void getParamTypeV2(Object[] params){
		mParamTypes = new Class[params.length];
		for (int i = 0; i < params.length; ++i){
			String strParam = params[i].getClass().getSimpleName();
			switch (strParam) {
				case "Integer":
					mParamTypes[i] = Integer.TYPE;
					break;
				case "Long":
					mParamTypes[i] = Long.TYPE;
					break;
				case "Double":
					mParamTypes[i] = Double.TYPE;
					break;
				case "Float":
					mParamTypes[i] = Float.TYPE;
					break;
				case "Boolean":
					mParamTypes[i] = Boolean.TYPE;
					break;
				case "Short":
					mParamTypes[i] = Short.TYPE;
					break;
				default:
					mParamTypes[i] = params[i].getClass();
					break;
			}
		}
	}
}
