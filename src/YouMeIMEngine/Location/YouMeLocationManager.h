#ifndef YOUME_LOCATION_MANAGER_H
#define YOUME_LOCATION_MANAGER_H

#include <string>
#include <YouMeIMEngine/YIM.h>
#include <YouMeIMEngine/Location/LocationInterface.h>
#include <YouMeIMEngine/YouMeIMCommonDef.h>

struct LocationInfo 
{
	double longitude;
	double latitude;
	DistrictLevel districtLevel;
	unsigned int districtCode;
	unsigned int cityCode;
	unsigned int provinceCode;

	std::string district;
	LocationInfo() : longitude(0), latitude(0), districtCode(0), cityCode(0), provinceCode(0){}
};

class YouMeLocationManager : public YIMLocationManager, public ILocationListen, public IManagerBase
{
public:
	YouMeLocationManager(YouMeIMManager* pIManager);
	~YouMeLocationManager();

	void UpdateLocation();

	virtual YIMErrorcode GetCurrentLocation() override;
	virtual YIMErrorcode GetNearbyObjects(int count, const XCHAR* serverAreaID, DistrictLevel districtlevel = DISTRICT_UNKNOW, bool resetStartDistance = false) override;
	virtual YIMErrorcode GetDistance(const XCHAR* userID) override;
	virtual void SetUpdateInterval(unsigned int interval) override;

	virtual void OnRecvPacket(ServerPacket& serverPacket) override;

	void SetCallback(IYIMLocationCallback* callback);
	LocationInfo GetCurrentUserLocationInfo();

private:
	virtual void OnUpdateLocation(LocationErrorcode errorcode, double longitude, double latitude) override;

	bool RequestGeocoding();
	void OnGeocodingRsp(ServerPacket& serverPacket);
	void OnNearbyObjectsRsp(ServerPacket& serverPacket);
	void OnUserLocationRsp(ServerPacket& serverPacket);

	ILocationManager* m_pLocationManager;
	IYIMLocationCallback* m_pCallback;
	LocationInfo m_currentLocation;
	double m_nStartDistance;
	unsigned int m_nUpdateInterval;
	XUINT64 m_ullLastUpdateTime;
	unsigned int m_iNearbyMaxDistance;
};

#endif