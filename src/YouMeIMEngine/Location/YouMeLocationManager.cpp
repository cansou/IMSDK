#include "YouMeLocationManager.h"
#include <YouMeCommon/Log.h>
#include <YouMeCommon/StringUtil.hpp>
#include <YouMeIMEngine/pb/server_protocol_code.h>
#include <YouMeIMEngine/pb/youme_getdistrict.pb.h>
#include <YouMeIMEngine/pb/youme_nearby.pb.h>
#include <YouMeIMEngine/pb/youme_userinfo.pb.h>
#include <YouMeIMEngine/YouMeIMManager.h>
#include <YouMeIMEngine/Location/LocationUtil.h>
#ifdef WIN32
//#include <YouMeIMEngine/Location/WinLocationManager.h>
#elif (OS_IOS || OS_IOSSIMULATOR)
#include <YouMeIMEngine/Location/IOS/IOSLocationManager.h>
#elif OS_ANDROID
#include <YouMeIMEngine/Location/Android/AndroidLocationManager.h>
#endif


#define NEIGHBOUR_MAX_DISTANCE 200000
#define NEIGHBOUR_MAX_COUNT 200


GeographyLocation::~GeographyLocation(){}

class GeographyLocationImp : public GeographyLocation
{
	friend class YouMeLocationManager;
public:
	GeographyLocationImp() : districtCode(0), longitude(0), latitude(0){}
	~GeographyLocationImp(){}
	virtual const XCHAR* GetCountry() override
	{
		return country.c_str();
	}
	virtual const XCHAR* GetProvince() override
	{
		return province.c_str();
	}
	virtual const XCHAR* GetCity() override
	{
		return city.c_str();
	}
	virtual const XCHAR* GetDistrictCounty() override
	{
		return districtCounty.c_str();
	}
	virtual const XCHAR* GetStreet() override
	{
		return street.c_str();
	}
	virtual double GetLongitude() override
	{
		return longitude;
	}
	virtual double GetLatitude() override
	{
		return latitude;
	}
	unsigned int GetDistrictCode() override
	{
		return districtCode;
	}
private:
	unsigned int districtCode;
	double longitude;
	double latitude;
	XString country;
	XString province;
	XString city;
	XString districtCounty;
	XString street;
};


RelativeLocation::~RelativeLocation(){}

class RelativeLocationImp : public RelativeLocation
{
	friend class YouMeLocationManager;
public:
	RelativeLocationImp() : distance(0), longitude(0), latitude(0){}
	virtual const XCHAR* GetUserID() override
	{
		return userID.c_str();
	}
	virtual unsigned int GetDistance() override
	{
		return distance;
	}
	virtual double GetLongitude() override
	{
		return longitude;
	}
	virtual double GetLatitude() override
	{
		return latitude;
	}
	virtual const XCHAR* GetCountry() override
	{
		return country.c_str();
	}
	virtual const XCHAR* GetProvince() override
	{
		return province.c_str();
	}
	virtual const XCHAR* GetCity() override
	{
		return city.c_str();
	}
	virtual const XCHAR* GetDistrictCounty() override
	{
		return districtCounty.c_str();
	}
	virtual const XCHAR* GetStreet() override
	{
		return street.c_str();
	}
private:
	//LocationObjectType type;
	unsigned int distance;
	double longitude;
	double latitude;
	XString userID;
	XString country;
	XString province;
	XString city;
	XString districtCounty;
	XString street;
};


YouMeLocationManager::YouMeLocationManager(YouMeIMManager* pIManager) : IManagerBase(pIManager)
, m_pLocationManager(NULL)
, m_pCallback(NULL)
, m_nStartDistance(0)
, m_nUpdateInterval(0)
, m_ullLastUpdateTime(0)
, m_iNearbyMaxDistance(NEIGHBOUR_MAX_DISTANCE)
{
#ifdef WIN32
	//m_pLocationManager = new WinLocationManager;
	//WinLocationManager::SetIMManager(m_pIManager);
#elif (OS_IOS || OS_IOSSIMULATOR)
	m_pLocationManager = new IOSLocationManager;
#elif OS_ANDROID
	m_pLocationManager = new AndroidLocationManager;
#endif
	if (m_pLocationManager != NULL)
	{
		m_pLocationManager->SetLocationListen(this);
	}

	if (m_pIManager)
	{
		m_iNearbyMaxDistance = m_pIManager->GetConfigure<unsigned int>(CONFIG_NEARBY_MAX_DISTANCE, NEIGHBOUR_MAX_DISTANCE);
	}
}

YouMeLocationManager::~YouMeLocationManager()
{
	if (m_pLocationManager != NULL)
	{
		m_pLocationManager->SetLocationListen(NULL);
		delete m_pLocationManager;
	}
}

void YouMeLocationManager::UpdateLocation()
{
	if (m_nUpdateInterval == 0 || m_ullLastUpdateTime == 0)
	{
		return;
	}
	XUINT64 ullCurTime = youmecommon::CTimeUtil::GetTimeOfDay_MS();
	if (ullCurTime - m_ullLastUpdateTime < m_nUpdateInterval)
	{
		return;
	}
	GetCurrentLocation();
}

YIMErrorcode YouMeLocationManager::GetCurrentLocation()
{
#if (defined WIN32) || (defined OS_OSX)
	return YIMErrorcode_Unsupport;
#endif
	if (m_pLocationManager == NULL || m_pIManager == NULL)
	{
		return YIMErrorcode_EngineNotInit;
	}
	if (m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success && m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Reconnecting)
	{
		return YIMErrorcode_NotLogin;
	}

	YIMErrorcode errorcode = YIMErrorcode_UnknowError;
	LocationErrorcode retCode = m_pLocationManager->GetCurrentLocation();
	if (LOCATIONERROR_SUCCESS == retCode || LOCATIONERROR_RUNNING == retCode)
	{
		errorcode = YIMErrorcode_Success;
	}
	else if (LOCATIONERROR_AUTHORIZE == retCode)
	{
		errorcode = YIMErrorcode_NoLocationAuthrize;
	}
	else if (LOCATIONERROR_INIT_FAILED == retCode)
	{
		errorcode = YIMErrorcode_InitFailed;
	}
	YouMe_LOG_Debug(__XT("GetCurrentLocation errorcode:%d"), retCode);

	return errorcode;
}

void YouMeLocationManager::OnUpdateLocation(LocationErrorcode errorcode, double longitude, double latitude)
{
	YouMe_LOG_Info(__XT("errorcode:%d logitude:%f latitude:%f"), errorcode, longitude, latitude);

	if (longitude < -180 || longitude > 180 || latitude < -90 || latitude > 90)
	{
		YouMe_LOG_Error(__XT("logitude:%f latitude:%f"), longitude, latitude);
		return;
	}
	if (LOCATIONERROR_SUCCESS == errorcode
		|| (LOCATIONERROR_TIMEOUT == errorcode && (longitude != -180 || longitude != -180)))
	{
		m_currentLocation.longitude = longitude;
		m_currentLocation.latitude = latitude;
		RequestGeocoding();
	}
	else
	{
		if (m_pCallback == NULL)
		{
			YouMe_LOG_Error(__XT("location callback is null"));
			return;
		}
		YIMErrorcode error = YIMErrorcode_UnknowError;
		if (LOCATIONERROR_INIT_FAILED == errorcode)
		{
			error = YIMErrorcode_InitFailed;
		}
		else if (LOCATIONERROR_AUTHORIZE == errorcode)
		{
			error = YIMErrorcode_NoLocationAuthrize;
		}
		else if (LOCATIONERROR_TIMEOUT == errorcode)
		{
			error = YIMErrorcode_TimeOut;
		}
		std::shared_ptr<GeographyLocationImp> pLocation(new GeographyLocationImp);
		m_pCallback->OnUpdateLocation(error, pLocation);
	}
}

void YouMeLocationManager::OnRecvPacket(ServerPacket& serverPacket)
{
	switch (serverPacket.commondID)
	{
	case YOUMEServiceProtocol::CMD_GET_DISTRICT:
	{
		OnGeocodingRsp(serverPacket);
	}
		break;
	case YOUMEServiceProtocol::CMD_GET_PEOPLE_NEARBY:
	{
		OnNearbyObjectsRsp(serverPacket);
	}
		break;
	case YOUMEServiceProtocol::CMD_GET_USR_INFO:
	{
		OnUserLocationRsp(serverPacket);
	}
		break;
	default:
		break;
	}
}

void YouMeLocationManager::SetCallback(IYIMLocationCallback* callback)
{
	m_pCallback = callback;
}

LocationInfo YouMeLocationManager::GetCurrentUserLocationInfo()
{
	return m_currentLocation;
}

bool YouMeLocationManager::RequestGeocoding()
{
	YOUMEServiceProtocol::GetDistrictReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_longitude(m_currentLocation.longitude);
	req.set_latitude(m_currentLocation.latitude);
	req.set_adcode(m_currentLocation.districtCode);
	req.set_keywords(m_currentLocation.district);
	std::string strData;
	req.SerializeToString(&strData);
	XUINT64 msgSerial = 0;
	std::map<std::string, std::string> extend;
	if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_GET_DISTRICT, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		YouMe_LOG_Error(__XT("send request failed"));
		return false;
	}
	return true;
}

void YouMeLocationManager::OnGeocodingRsp(ServerPacket& serverPacket)
{
	if (m_pCallback == NULL)
	{
		YouMe_LOG_Error(__XT("location callback is null"));
		return;
	}

	YIMErrorcode errorCode = YIMErrorcode_Success;
	if (serverPacket.result == -1)
	{
		//errorCode = YIMErrorcode_TimeOut;
		YouMe_LOG_Error(__XT("geocoding timeout"));
		return;
	}

	std::shared_ptr<GeographyLocationImp> pLocation(new GeographyLocationImp);
	pLocation->longitude = m_currentLocation.longitude;
	pLocation->latitude = m_currentLocation.latitude;

	YOUMEServiceProtocol::GetDistrictRsp rsp;
	if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
	{
		YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
		errorCode = YIMErrorcode_ServerError;
	}
	else
	{
		int nRet = rsp.ret();
		if (nRet != 0)
		{
			YouMe_LOG_Error(__XT("geocoding error(%d)"), nRet);
		}
		else
		{
			YouMe_LOG_Info(__XT("district:%s district_level:%d city_code:%s ad_code:%u higher_adcode:%u city_adcode:%u province_adcode:%u"),
				UTF8TOXString(rsp.district_name()).c_str(), rsp.district_level(), UTF8TOXString(rsp.city_code()).c_str(), rsp.adcode(), rsp.higher_adcode(), rsp.city_adcode(), rsp.province_adcode());

			m_currentLocation.district = rsp.district_name();
			m_currentLocation.districtLevel = static_cast<DistrictLevel>(rsp.district_level());
			m_currentLocation.districtCode = rsp.adcode();
			m_currentLocation.cityCode = rsp.city_adcode();
			m_currentLocation.provinceCode = rsp.province_adcode();
			pLocation->districtCode = rsp.adcode();
			if (!rsp.district_name().empty())
			{
				XString strDistrict = UTF8TOXString(rsp.district_name());
				std::vector<XString> vDistrict;
				CStringUtilT<XCHAR>::splitString(strDistrict, __XT(","), vDistrict, true);
				for (std::vector<XString>::size_type i = 0; i < vDistrict.size(); ++i)
				{
					if (0 == i)
					{
						pLocation->country = vDistrict[i];
					}
					else if (1 == i)
					{
						pLocation->province = vDistrict[i];
					}
					else if (2 == i)
					{
						pLocation->city = vDistrict[i];
					}
					else if (3 == i)
					{
						pLocation->districtCounty = vDistrict[i];
					}
					else if (4 == i)
					{
						pLocation->street = vDistrict[i];
					}
				}
			}

			m_ullLastUpdateTime = youmecommon::CTimeUtil::GetTimeOfDay_MS();
		}
	}
	m_pCallback->OnUpdateLocation(errorCode, pLocation);
}

YIMErrorcode YouMeLocationManager::GetNearbyObjects(int count, const XCHAR* serverAreaID, DistrictLevel districtlevel, bool resetStartDistance)
{
#if (defined WIN32) || (defined OS_OSX)
	return YIMErrorcode_Unsupport;
#endif
	if (m_pIManager == NULL)
	{
		return YIMErrorcode_EngineNotInit;
	}
	if (m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success && m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Reconnecting)
	{
		return YIMErrorcode_NotLogin;
	}
	if (m_currentLocation.longitude == 0 && m_currentLocation.latitude == 0)
	{
		return YIMErrorcode_UnknowLocation;
	}
	if (resetStartDistance)
	{
		m_nStartDistance = 0;
	}

	int nLimit = count <= 0 || count > NEIGHBOUR_MAX_COUNT ? NEIGHBOUR_MAX_COUNT : count;
	YOUMEServiceProtocol::GetNearbyUserReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_longitude(m_currentLocation.longitude);
	req.set_latitude(m_currentLocation.latitude);
	req.set_district_name(m_currentLocation.district);
	req.set_adcode(m_currentLocation.districtCode);
	req.set_number_limit(nLimit);
	req.set_min_distance_limit(m_nStartDistance);
	req.set_max_distance_limit(m_iNearbyMaxDistance);
	if (districtlevel != DISTRICT_UNKNOW)
	{
		req.set_district_level((YOUMECommonProtocol::DistrictLevel)districtlevel);
		if (m_currentLocation.districtLevel == districtlevel)
		{
			req.set_where_adcode(m_currentLocation.districtCode);
		}
		else if (DISTRICT_CITY == districtlevel)
		{
			req.set_where_adcode(m_currentLocation.cityCode);
		}
		else if (DISTRICT_PROVINCE == districtlevel)
		{
			req.set_where_adcode(m_currentLocation.provinceCode);
		}
	}
	if (serverAreaID != NULL && !XString(serverAreaID).empty())
	{
		req.set_service_area_id(XStringToUTF8(XString(serverAreaID)));
	}
	std::string strData;
	req.SerializeToString(&strData);
	XUINT64 msgSerial = 0;
	std::map<std::string, std::string> extend;
	if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_GET_PEOPLE_NEARBY, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		YouMe_LOG_Error(__XT("send request failed"));
		return YIMErrorcode_NetError;
	}

	return YIMErrorcode_Success;
}

void YouMeLocationManager::SetUpdateInterval(unsigned int interval)
{
	m_nUpdateInterval = interval * 60000;
}

void YouMeLocationManager::OnNearbyObjectsRsp(ServerPacket& serverPacket)
{
	if (m_pCallback == NULL)
	{
		YouMe_LOG_Error(__XT("location callback is null"));
		return;
	}

	YIMErrorcode errorCode = YIMErrorcode_Success;
	if (serverPacket.result == -1)
	{
		//errorCode = YIMErrorcode_TimeOut;
		YouMe_LOG_Error(__XT("get nearby people timeout"));
		return;
	}
	
	std::list<std::shared_ptr<RelativeLocationImp> > tempList;
	std::list< std::shared_ptr<RelativeLocation> > objectList;
	double maxDistance = 0;
	double minDistance = m_nStartDistance;
	YOUMEServiceProtocol::GetNearbyUserRsp rsp;
	if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
	{
		YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
		errorCode = YIMErrorcode_ServerError;
	}
	else
	{
		int nRet = rsp.ret();
		if (nRet != 0 && nRet != YOUMEServiceProtocol::EC_NO_APP_USER)
		{
			//errorCode = YIMErrorcode_UnknowError;
			YouMe_LOG_Error(__XT("geocoding error(%d)"), nRet);
		}
		else
		{
			for (int i = 0; i < rsp.people_list_size(); i++)
			{
				YOUMEServiceProtocol::YoumeUserLocation userLocation = rsp.people_list(i);
				std::shared_ptr<RelativeLocationImp> pRelativeLocation(new RelativeLocationImp);
				pRelativeLocation->userID = UTF8TOXString(userLocation.user_id());
				pRelativeLocation->longitude = userLocation.longitude();
				pRelativeLocation->latitude = userLocation.latitude();
				pRelativeLocation->distance = static_cast<unsigned int>(userLocation.distance());
				XString strDistrict = UTF8TOXString(userLocation.district_name());
				std::vector<XString> vDistrict;
				CStringUtilT<XCHAR>::splitString(strDistrict, __XT(","), vDistrict, true);
				for (std::vector<XString>::size_type i = 0; i < vDistrict.size(); ++i)
				{
					if (0 == i)
					{
						pRelativeLocation->country = vDistrict[i];
					}
					else if (1 == i)
					{
						pRelativeLocation->province = vDistrict[i];
					}
					else if (2 == i)
					{
						pRelativeLocation->city = vDistrict[i];
					}
					else if (3 == i)
					{
						pRelativeLocation->districtCounty = vDistrict[i];
					}
					else if (4 == i)
					{
						pRelativeLocation->street = vDistrict[i];
					}
				}

				tempList.push_back(pRelativeLocation);
                objectList.push_back( pRelativeLocation );

				if (maxDistance < userLocation.distance())
				{
					maxDistance = userLocation.distance();
				}
			}
			if (maxDistance != 0)
			{
				m_nStartDistance = maxDistance;
				m_nStartDistance += 0.001;
			}
			if (rsp.people_cnt() == 0 || m_nStartDistance >= m_iNearbyMaxDistance)
			{
				m_nStartDistance = 0;
			}
			YouMe_LOG_Info(__XT("nearby poople total:%d maxDistance:%f"), rsp.people_cnt(), maxDistance);
		}
	}

	if (0 == maxDistance)
	{
		maxDistance = m_iNearbyMaxDistance;
	}

	m_pCallback->OnGetNearbyObjects(errorCode, objectList, static_cast<unsigned int>(minDistance), static_cast<unsigned int>(maxDistance));
}

YIMErrorcode YouMeLocationManager::GetDistance(const XCHAR* userID)
{
#if (defined WIN32) || (defined OS_OSX)
    return YIMErrorcode_Unsupport;
#endif
    if (m_pLocationManager == NULL || m_pIManager == NULL)
    {
        return YIMErrorcode_EngineNotInit;
    }
    
    if (m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success && m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Reconnecting)
    {
        return YIMErrorcode_NotLogin;
    }
    
	if (NULL == userID || XStrLen(userID) == 0)
	{
		return YIMErrorcode_ParamInvalid;
	}
	if (m_currentLocation.longitude == 0 && m_currentLocation.latitude == 0)
	{
		return YIMErrorcode_UnknowLocation;
	}

	std::string strUserID = XStringToUTF8(XString(userID));
	// ���˻�ȡ�û���ϢЭ��
	YOUMEServiceProtocol::GetUserInfoReq req;
	req.set_version(PROTOCOL_VERSION);
	req.set_user_id(strUserID);
	std::string strData;
	req.SerializeToString(&strData);
	XUINT64 msgSerial = 0;
	std::map<std::string, std::string> extend;
	extend["LocationUserID"] = strUserID;
	if (!m_pIManager->SendData(YOUMEServiceProtocol::CMD_GET_USR_INFO, strData.c_str(), (int)strData.length(), extend, msgSerial))
	{
		YouMe_LOG_Error(__XT("send request failed"));
		return YIMErrorcode_NetError;
	}
	return YIMErrorcode_Success;
}

void YouMeLocationManager::OnUserLocationRsp(ServerPacket& serverPacket)
{
	std::map<std::string, std::string>::const_iterator itr = serverPacket.extend.find("LocationUserID");
	if (itr == serverPacket.extend.end())
	{
		return;
	}

	if (m_pCallback == NULL)
	{
		YouMe_LOG_Error(__XT("location callback is null"));
		return;
	}

	XString userID = UTF8TOXString(itr->second);
	unsigned int distance = 0;
	YIMErrorcode errorCode = YIMErrorcode_Success;

	if (serverPacket.result == -1)
	{
		errorCode = YIMErrorcode_TimeOut;
		YouMe_LOG_Error(__XT("get location timeout %s"), userID.c_str());
	}
	else
	{
		if (m_pIManager != NULL && userID != m_pIManager->GetCurrentUser())
		{
			YOUMEServiceProtocol::GetUserInfoRsp rsp;
			if (!rsp.ParseFromArray(serverPacket.packetBuffer, serverPacket.packetSize))
			{
				YouMe_LOG_Error(__XT("unpack error serail:%llu size:%u"), serverPacket.reqSerial, serverPacket.packetSize);
				errorCode = YIMErrorcode_ServerError;
			}
			else
			{
				double longitude = rsp.longitude();
				double latitude = rsp.latitude();
				if (-180 == longitude && -90 == latitude)
				{
					errorCode = YIMErrorcode_UnknowLocation;
				}
				else
				{
					distance = LocationUtil::GetDistanceSimplify(Point(m_currentLocation.longitude, m_currentLocation.latitude), Point(longitude, latitude));
				}
			}
		}
	}

	m_pCallback->OnGetDistance(errorCode, userID.c_str(), distance);
}
