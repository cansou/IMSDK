#ifndef LOCATION_INTERFACE_H
#define LOCATION_INTERFACE_H


/*struct GeoLocationInfo
{
	double longitude;
	double latitude;
	XString country;
	XString province;
	XString city;
	XString subCity;
	XString detailAddress;

	GeoLocationInfo() : longitude(0), latitude(0){};
};*/

// 错误码各平台共用（修改记得同步到C++和Android）
enum LocationErrorcode
{
	LOCATIONERROR_SUCCESS = 0,			// 成功
	LOCATIONERROR_INIT_FAILED = 1,		// 初始化失败
	LOCATIONERROR_AUTHORIZE = 2,		// 无权限
	LOCATIONERROR_FAILED = 3,			// 失败
	LOCATIONERROR_RUNNING = 4,			// 正在运行
	LOCATIONERROR_TIMEOUT = 5,			// 超时
	LOCATIONERROR_START_FAILED = 6,		// 启动定位失败
	LOCATIONERROR_RESOLVE_FAILED = 7,	// 解析位置失败

	LOCATIONERROR_OTHER = 99		// 其它错误
};

class ILocationListen
{
public:
	//virtual void OnUpdateLocation(GeoLocationInfo& location) = 0;
	virtual void OnUpdateLocation(LocationErrorcode errorcode, double longitude, double latitude) = 0;
};

class ILocationManager
{
public:
	virtual ~ILocationManager(){};
	virtual void SetLocationListen(ILocationListen* listen) = 0;
	virtual LocationErrorcode GetCurrentLocation() = 0;
};

#endif
