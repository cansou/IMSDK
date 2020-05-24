package com.youme.lbs;

public interface IGeographyLocationCallback{

	public enum LocationErrorcode{
		LOCATIONERROR_SUCCESS(0),			// 成功
		LOCATIONERROR_INIT_FAILED(1),		// 初始化失败
		LOCATIONERROR_AUTHORIZE(2),			// 无权限
		LOCATIONERROR_FAILED(3),			// 失败
		LOCATIONERROR_RUNNING(4),			// 正在运行
		LOCATIONERROR_TIMEOUT(5),			// 超时
		LOCATIONERROR_START_FAILED(6),		// 启动定位失败
		LOCATIONERROR_RESOLVE_FAILED(7),	// 解析位置失败

		LOCATIONERROR_OTHER(99);			// 其它错误
	    
	    private int value;
	    private LocationErrorcode(int value){
	        this.value = value;
	    } 
	    public int getValue(){
	        return this.value;
	    }
	}
	
	public abstract void OnUploadGeoLocation(LocationErrorcode errorcode, double longitude, double latitude);
}
